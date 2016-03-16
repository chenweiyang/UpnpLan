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
#include "UpnpDeviceDefinition.h"
#include "UpnpDeviceFactory.h"
#include "BinaryLight.h"
#include "SwitchPower.h"
#include "UpnpCode.h"
#include "UpnpUri.h"

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
    fprintf(stdout, "gett       --  BinaryLigth.SwitchPower.GetTarget\n");
    fprintf(stdout, "sett       --  BinaryLigth.SwitchPower.SetTarget\n");
    fprintf(stdout, "gets       --  BinaryLigth.SwitchPower.GetStatus\n");
}

static bool device_filter(UpnpUri *uri, void *ctx)
{
    bool result = false;

    // printf("device_filter: schemas:[%s] deviceType:[%s]\n", schemas, deviceType);

    do
    {
        if (UpnpUri_IsEqual(uri, BINARYLIGHT_DEVICE_TYPE))
        {
            result = true;
            break;
        }
    } while (0);

    return result;
}

static UpnpDevice *device = NULL;
static BinaryLight *binaryLight = NULL;
static bool gTargetValue = false;

static void device_listener(UpnpDeviceSummary *deviceSummary, bool alive, void *ctx)
{
    printf("--- %s ---\n", alive ? "online" : "offline");
    printf("ip: %s\n", deviceSummary->deviceIp);
    printf("deviceId: %s\n", deviceSummary->deviceId);
    printf("domainName: %s\n", deviceSummary->domainName);
    printf("deviceType: %s\n", deviceSummary->deviceType);
    printf("deviceVersion: %s\n", deviceSummary->deviceVersion);
    printf("deviceUrl: %s\n", deviceSummary->deviceUrl);
    printf("upnpStackInfo: %s\n", deviceSummary->upnpStackInfo);

    if (alive)
    {
        if (device != NULL)
        {
            if (binaryLight != NULL)
            {
                BinaryLight_Delete(binaryLight);
                binaryLight = NULL;
            }

            UpnpDevice_Delete(device);
            device = NULL;
        }

        device = UpnpDeviceFactory_Create(deviceSummary);
        if (device != NULL)
        {
            // print_device(device);

            binaryLight = BinaryLight_Create(device, gRuntime);
            if (binaryLight == NULL)
            {
                printf("BinaryLight_Create failed\n");
            }
        }
    }
    else
    {
        if (device != NULL)
        {
            if (STR_EQUAL(deviceSummary->deviceId, UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_UDN)))
            {
                if (binaryLight != NULL)
                {
                    BinaryLight_Delete(binaryLight);
                    binaryLight = NULL;
                }

                UpnpDevice_Delete(device);
                device = NULL;
            }
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

static void cmd_GetTarget(void)
{
    TinyRet ret = TINY_RET_OK;
    UpnpError error;
    SwitchPower_GetTargetResult result;

    if (binaryLight == NULL)
    {
        printf("BinaryLight not found\n");
        return;
    }

    memset(&error, 0, sizeof(UpnpError));
    memset(&result, 0, sizeof(SwitchPower_GetTargetResult));

    ret = SwitchPower_GetTarget(BinaryLight_GetSwitchPower(binaryLight), &result, &error);
    LOG("SwitchPower_GetTarget, ret: ", ret);

    if (RET_FAILED(ret))
    {
        return;
    }

    if (error.code != UPNP_SUCCESS)
    {
        printf("SwitchPower_GetTarget failed: %d %s\n", error.code, error.description);
        return;
    }

    gTargetValue = result.theTargetValue;

    printf("SwitchPower_GetTarget OK, targetValue is : %s\n", gTargetValue ? "true" : "false");
}


static void cmd_SetTarget(void)
{
    TinyRet ret = TINY_RET_OK;
    UpnpError error;

    if (binaryLight == NULL)
    {
        printf("BinaryLight not found\n");
        return;
    }

    memset(&error, 0, sizeof(UpnpError));

    gTargetValue = !gTargetValue;

    ret = SwitchPower_SetTarget(BinaryLight_GetSwitchPower(binaryLight), gTargetValue, &error);
    LOG("SwitchPower_SetTarget, ret: ", ret);

    if (RET_FAILED(ret))
    {
        return;
    }

    if (error.code != UPNP_SUCCESS)
    {
        printf("SwitchPower_SetTarget failed: %d %s\n", error.code, error.description);
        return;
    }

    printf("SwitchPower_SetTarget OK!\n");
}

static void cmd_GetStatus(void)
{
    TinyRet ret = TINY_RET_OK;
    UpnpError error;
    SwitchPower_GetStatusResult result;

    if (binaryLight == NULL)
    {
        printf("BinaryLight not found\n");
        return;
    }

    memset(&error, 0, sizeof(UpnpError));
    memset(&result, 0, sizeof(SwitchPower_GetStatusResult));

    ret = SwitchPower_GetStatus(BinaryLight_GetSwitchPower(binaryLight), &result, &error);
    LOG("SwitchPower_GetStatus, ret: ", ret);

    if (RET_FAILED(ret))
    {
        return;
    }

    if (error.code != UPNP_SUCCESS)
    {
        printf("SwitchPower_GetStatus failed: %d %s\n", error.code, error.description);
        return;
    }

    printf("SwitchPower_GetStatus, status is : %s\n", result.theResultStatus ? "true" : "false");
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
        { "gett", cmd_GetTarget },
        { "sett", cmd_SetTarget },
        { "gets", cmd_GetStatus },
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
