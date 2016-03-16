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
#include "UpnpDevice.h"
#include "UpnpDeviceFactory.h"

#define LOG(func, ret)  printf("%s: %s\n", func, tiny_ret_to_str(ret))

static int g_loop = 0;
static UpnpRuntime * gRuntime = NULL;

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

static bool device_filter(const char *schemas, const char *deviceType, void *ctx)
{
    bool result = true;

    printf("device_filter: schemas:[%s] deviceType:[%s]\n", schemas, deviceType);

#if 0
    do
    {
        if (!STR_EQUAL(schemas, SCHEMAS_UPNP_ORG))
        {
            result = false;
            break;
        }


        if (STR_EQUAL(deviceType, DEVICE_MEDIA_RENDERER) || STR_EQUAL(deviceType, DEVICE_MEDIA_SERVER))
        {
            result = true;
        }

    } while (0);
#endif

    return result;
}

static void print_property(Property *p, int spaceSize)
{
    int i = 0;
    for (i = 0; i < spaceSize; ++i)
    {
        printf(" ");
    }

    switch (p->definition.type.clazzType)
    {
    case CLAZZ_BYTE:
        printf("%s: %d\n", p->definition.name, p->value.object.value.byteValue);
        break;

    case CLAZZ_WORD:
        printf("%s: %d\n", p->definition.name, p->value.object.value.wordValue);
        break;

    case CLAZZ_INTEGER:
        printf("%s: %d\n", p->definition.name, p->value.object.value.integerValue);
        break;

    case CLAZZ_LONG:
        printf("%s: %lld\n", p->definition.name, p->value.object.value.longValue);
        break;

    case CLAZZ_BOOLEAN:
        printf("%s: %d\n", p->definition.name, p->value.object.value.boolValue);
        break;

    case CLAZZ_FLOAT:
        printf("%s: %f\n", p->definition.name, p->value.object.value.floatValue);
        break;

    case CLAZZ_DOUBLE:
        printf("%s: %f\n", p->definition.name, p->value.object.value.doubleValue);
        break;

    case CLAZZ_CHAR:
        printf("%s: %c\n", p->definition.name, p->value.object.value.charValue);
        break;

    case CLAZZ_STRING:
        printf("%s: %s\n", p->definition.name, p->value.object.value.stringValue);
        break;

    default:
        break;
    }
}

static void print_action(UpnpAction *action)
{
    uint32_t i = 0;
    PropertyList* properties = UpnpAction_GetPropertyList(action);
    PropertyList* args = UpnpAction_GetArgumentList(action);
    PropertyList* results = UpnpAction_GetResultList(action);

    printf("    - <Action>\n");

    for (i = 0; i < PropertyList_GetSize(properties); ++i)
    {
        Property *p = PropertyList_GetPropertyAt(properties, i);
        print_property(p, 6);
    }
    
    printf("        <arguments> \n");
    for (i = 0; i < PropertyList_GetSize(args); ++i)
    {
        Property *p = PropertyList_GetPropertyAt(args, i);
        print_property(p, 10);
    }

    printf("        <results> \n");
    for (i = 0; i < PropertyList_GetSize(results); ++i)
    {
        Property *p = PropertyList_GetPropertyAt(results, i);
        print_property(p, 10);
    }
}

static void print_state(UpnpState *state)
{
    printf("    - <State sendEvents:%s>\n", state->sendEvents ? "yes" : "no");
    printf("        name: %s\n", state->definition.name);
    printf("        type: %s\n", state->definition.type.clazzName);
}

static void print_service(UpnpService *service)
{
    uint32_t i = 0;
    PropertyList * properties = UpnpService_GetPropertyList(service);
    UpnpActionList * actions = UpnpService_GetActionList(service);
    UpnpStateList * states = UpnpService_GetStateList(service);

    printf(" - <Service>\n");

    for (i = 0; i < PropertyList_GetSize(properties); ++i)
    {
        Property *p = PropertyList_GetPropertyAt(properties, i);
        print_property(p, 4);
    }

    printf("    * <ActionList>\n");
    for (i = 0; i < UpnpActionList_GetSize(actions); ++i)
    {
        UpnpAction *a = UpnpActionList_GetActionAt(actions, i);
        print_action(a);
    }

    printf("    * <StateList>\n");
    for (i = 0; i < UpnpStateList_GetSize(states); ++i)
    {
        UpnpState *s = UpnpStateList_GetStateAt(states, i);
        print_state(s);
    }
}

static void print_device(UpnpDevice *device)
{
    uint32_t i = 0;
    PropertyList * properties = UpnpDevice_GetPropertyList(device);
    UpnpServiceList * services = UpnpDevice_GetServiceList(device);
    
    printf("--------------------- DEVICE -------------------\n");

    for (i = 0; i < PropertyList_GetSize(properties); ++i)
    {
        Property *p = PropertyList_GetPropertyAt(properties, i);
        print_property(p, 0);
    }

    for (i = 0; i < UpnpServiceList_GetSize(services); ++i)
    {
        UpnpService *s = UpnpServiceList_GetServiceAt(services, i);
        print_service(s);
    }

    printf("------------------------------------------------\n");
}

static void device_listener(UpnpDeviceSummary *deviceSummary, bool alive, void *ctx)
{
    printf("--- %s ---\n", alive ? "online" : "offline");
    printf("ip: %s\n", deviceSummary->deviceIp);
    printf("deviceId: %s\n", deviceSummary->deviceId);
    printf("domainName: %s\n", deviceSummary->domainName);
    printf("deviceType: %s\n", deviceSummary->deviceType);
    printf("deviceVersion: %d\n", deviceSummary->deviceVersion);
    printf("deviceUrl: %s\n", deviceSummary->deviceUrl);
    printf("upnpStackInfo: %s\n", deviceSummary->upnpStackInfo);

    if (alive)
    {
        UpnpDevice *device = UpnpDeviceFactory_Create(deviceSummary);
        if (device != NULL)
        {
            print_device(device);
            UpnpDevice_Delete(device);
        }
    }
}

static void cmd_start(void)
{
    LOG("UpnpRuntime_Start", UpnpRuntime_Start(gRuntime));
}

static void cmd_stop(void)
{
    LOG("UpnpRuntime_Stop", UpnpRuntime_Stop(gRuntime));
}

static void cmd_discover(void)
{
    LOG("UpnpRuntime_StartScan", UpnpRuntime_StartScan(gRuntime, device_listener, device_filter, NULL));
}

static void cmd_stopDiscovery(void)
{
    LOG("UpnpRuntime_StopScan", UpnpRuntime_StopScan(gRuntime));
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
void cmd_loop(UpnpRuntime * runtime)
{
    char buf[1024];

    gRuntime = runtime;
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
void cmd_loop(UpnpRuntime * cp)
{
    fd_set  read_set;
    fd_set  write_set;
    fd_set  error_set;
    int     max_soc = 0;
    int     ret = 0;

    gRuntime = cp;
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
