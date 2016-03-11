/*
 * Copyright (C) 2013-2015
 *
 * @author ouyangchengfeng@xiaomi.com
 * @date   2013-11-19
 *
 * @file   command.c
 *
 * @remark
 *
 */

#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG(func, ret)  printf("%s: %s\n", func, tiny_ret_to_str(ret))

static int g_loop = 0;
static UpnpRegistry * gRegistry = NULL;

static void cmd_help(void)
{
    fprintf(stdout, "\n------------ help --------------\n");
    fprintf(stdout, "h          --  show help information\n");
    fprintf(stdout, "x          --  exit\n");
    fprintf(stdout, "start      --  start\n");
    fprintf(stdout, "stop       --  stop\n");
    fprintf(stdout, "discover   --  discover\n");
    fprintf(stdout, "sd         --  stop discovery\n");
}

static void cmd_start(void)
{
    LOG("UpnpRegistry_Start", UpnpRegistry_Start(gRegistry));
}

static void cmd_stop(void)
{
    LOG("UpnpRegistry_Stop", UpnpRegistry_Stop(gRegistry));
}

static void cmd_discover(void)
{
    LOG("UpnpRegistry_Discover", UpnpRegistry_Discover(gRegistry, NULL, NULL));
}

static void cmd_stopDiscovery(void)
{
    LOG("UpnpRegistry_StopDiscovery", UpnpRegistry_StopDiscovery(gRegistry));
}

static void cmd_exit(void)
{
    g_loop = 0;
    UpnpRegistry_Stop(gRegistry);
}

typedef void (*cb_command)(void);

struct _cmd_exec {
    const char * name;
    cb_command exec;
};

struct _cmd_exec cmd_exec[] = {
        { "h", cmd_help },
        { "x", cmd_exit },
        { "start", cmd_start },
        { "stop", cmd_stop },
        { "discover", cmd_discover },
        { "sd", cmd_stopDiscovery },
};

static
void command(const char *buf)
{
    int i   = 0;
    int len = DIM(cmd_exec);

    for (i=0; i < len; i++)
    {
        if (strcmp(cmd_exec[i].name, buf) == 0)
        {
            cmd_exec[i].exec();
            break;
        }
    }
}

#ifdef _WIN32
void cmd_loop(UpnpRegistry * r)
{
    char buf[1024];

    gRegistry = r;
    g_loop = 1;

    while (g_loop)
    {
        int ret = 0;
        memset(buf, 0, 1024);

        printf("> ");
        ret = scanf("%s", buf);
		command(buf);
    }
}
#else /* Linux */
void cmd_post_select(fd_set *p_read_set, fd_set *p_write_set, fd_set *p_error_set)
{
    char buf[2048];
    size_t nbytes;
    ssize_t bytes_read;
    int fd = 0;

    nbytes = sizeof(buf);
    memset(buf, 0, nbytes);

    if (FD_ISSET(fd, p_read_set))
    {
        //bytes_read = read(fd, (void*)buf, nbytes);
        bytes_read = read(fd, (void*)buf, nbytes);
        /* remove '\n' */
        if (strlen(buf) > 0)
        {
            buf[strlen(buf) - 1] = 0;
        }

        command(buf);
    }
}

void cmd_pre_select(int *p_max_soc, fd_set *p_read_set, fd_set *p_write_set, fd_set *p_error_set)
{
    int soc = 0;

    FD_SET(soc, p_error_set);
    FD_SET(soc, p_read_set);

    if (*p_max_soc < soc)
    {
        *p_max_soc = soc;
    }
}

/**
 * @brief Linuxƽ̨�����У��ȴ��û���������ָ�ִ��
 */
void cmd_loop(UpnpRegistry * r)
{
    fd_set  read_set;
    fd_set  write_set;
    fd_set  error_set;
    int     max_soc = 0;
    int     ret = 0;

    gRegistry = r;
    g_loop = 1;

    while (g_loop)
    {
        ret = write(1, "> ", 2);

        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        FD_ZERO(&error_set);

        cmd_pre_select(&max_soc, &read_set, &write_set, &error_set);

        select(max_soc + 1, &read_set, &write_set, &error_set, NULL);

        cmd_post_select(&read_set, &write_set, &error_set);
    }

    fprintf(stdout, "\n");

    printf("-------------- cmd_run -----------------\n");
}
#endif /* Linux */
