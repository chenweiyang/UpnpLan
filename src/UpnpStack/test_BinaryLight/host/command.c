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
#include "BinaryLight.h"
#include "SwitchPower.h"
#include "UpnpCode.h"

static int g_loop = 0;
static UpnpRuntime * gRuntime = NULL;
static BinaryLight * gBinaryLight = NULL;
static bool gTaget = false;
static bool gStatus = false;

UpnpCode OnGetTarget(SwitchPower *thiz, SwitchPower_GetTargetResult *result, void *ctx)
{
    printf("OnGetTarget\n");

    result->theTargetValue = gTaget;

    return UPNP_SUCCESS;
}

UpnpCode OnSetTarget(SwitchPower *thiz, bool newTargetValue, void *ctx)
{
    printf("OnSetTarget: %s\n", newTargetValue ? "ON" : "OFF");

    gTaget = newTargetValue;

    return UPNP_SUCCESS;
}

UpnpCode OnGetStatus(SwitchPower *thiz, SwitchPower_GetStatusResult *result, void *ctx)
{
    printf("OnGetStatus\n");

    result->theResultStatus = gStatus;

    return UPNP_SUCCESS;
}

static void cmd_help(void)
{
    fprintf(stdout, "\n------------ help --------------\n");
    fprintf(stdout, "h          --  show help information\n");
    fprintf(stdout, "x          --  exit\n");
    fprintf(stdout, "start      --  start BinaryLight\n");
    fprintf(stdout, "stop       --  stop BinaryLight\n");
    fprintf(stdout, "sett       --  set target\n");
    fprintf(stdout, "sets       --  set status\n");
    fprintf(stdout, "sendevents --  sendEvents\n");
}

static void cmd_Start(void)
{
    LOG("BinaryLight_Start", BinaryLight_Start(gBinaryLight));
}

static void cmd_Stop(void)
{
    LOG("BinaryLight_Stop", BinaryLight_Stop(gBinaryLight));
}

static void cmd_SetTarget(void)
{
    gTaget = !gTaget;
    LOG("SwitchPower_SetTarget", SwitchPower_SetTarget(BinaryLight_GetSwitchPower(gBinaryLight), gTaget));
}

static void cmd_SetStatus(void)
{
    gStatus = !gStatus;
    LOG("SwitchPower_SetStatus", SwitchPower_SetStatus(BinaryLight_GetSwitchPower(gBinaryLight), gStatus));
}

static void cmd_SendEvents(void)
{
    LOG("SwitchPower_SendEvents", SwitchPower_SendEvents(BinaryLight_GetSwitchPower(gBinaryLight)));
}

static void cmd_exit(void)
{
    g_loop = 0;
    UpnpRuntime_Stop(gRuntime);
}

typedef void (*cb_command)(void);

struct _cmd_exec {
    const char * name;
    cb_command exec;
};

struct _cmd_exec cmd_exec[] = {
        { "h", cmd_help },
        { "x", cmd_exit },
        { "start", cmd_Start},
        { "stop", cmd_Stop},
        { "sett", cmd_SetTarget},
        { "sets", cmd_SetStatus},
        { "sendevents", cmd_SendEvents},
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
void cmd_loop(UpnpRuntime * runtime, BinaryLight *light)
{
    char buf[1024];

    gRuntime = runtime;
    gBinaryLight = light;
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
            buf[strlen(buf) - 1] = 0;

        command(buf);
    }
}

void cmd_pre_select(int *p_max_soc, fd_set *p_read_set, fd_set *p_write_set, fd_set *p_error_set)
{
    int soc = 0;

    FD_SET(soc, p_error_set);
    FD_SET(soc, p_read_set);

    if (*p_max_soc < soc)
        *p_max_soc = soc;
}

/**
 * @brief Linuxƽ̨�����У��ȴ��û���������ָ�ִ��
 */
void cmd_loop(UpnpRuntime * runtime, BinaryLight *light)
{
    fd_set  read_set;
    fd_set  write_set;
    fd_set  error_set;
    int     max_soc = 0;
    int     ret = 0;

    gRuntime = runtime;
    gBinaryLight = light;
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
