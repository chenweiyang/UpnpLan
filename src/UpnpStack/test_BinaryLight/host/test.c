#include "UpnpInitializer.h"
#include "UpnpRuntime.h"
#include "BinaryLight.h"
#include "command.h"

#include "upnp_net_util.h"

int main(void)
{
    UpnpInitializer_Initialize();
    {
        UpnpRuntime *runtime = NULL;
        BinaryLight *binaryLight = NULL;
        UpnpDeviceConfig *config = NULL;

#if 0
        const char *remoteIp = "10.0.1.5";
        char selfIp[32];
        
        memset(selfIp, 0, 32);
        
        if (RET_SUCCEEDED(upnp_net_get_ip(remoteIp, selfIp, 32)))
        {
            printf("upnp_net_get_ip: OK, self ip: %s\n", selfIp);
        }
        else
        {
            printf("upnp_net_get_ip: failed\n");
        }

        return 0;
#endif 

        runtime = UpnpRuntime_New();
        if (runtime == NULL)
        {
            printf("UpnpRuntime_New failed!\n");
            return 0;
        }

        /**
         * 1. init device configuration
         */
        config = UpnpDeviceConfig_New();
        if (config == NULL)
        {
            printf("UpnpDeviceConfig_New failed!\n");
            return 0;
        }

        UpnpDeviceConfig_SetDeviceName(config, "Ouyang BinaryLight A");
        UpnpDeviceConfig_SetModelNumber(config, "2016");
        UpnpDeviceConfig_SetModelName(config, "Light");
        UpnpDeviceConfig_SetModelDescription(config, "BinaryLight for demo");
        UpnpDeviceConfig_SetModelUrl(config, "http://www.mi.com/light");
        UpnpDeviceConfig_SetManufacturer(config, "Xiaomi");
        UpnpDeviceConfig_SetManufacturerUrl(config, "http://www.mi.com");

        /**
         * 2. create device
         */
        binaryLight = BinaryLight_New(config, runtime);
        if (binaryLight == NULL)
        {
            printf("BinaryLight_New failed!\n");
            return 0;
        }

        /**
        * 3. set service handlers
        */
        {
            SwitchPower_ActionHandler handler;
            handler.OnGetStatus = OnGetStatus;
            handler.OnGetTarget = OnGetTarget;
            handler.OnSetTarget = OnSetTarget;
            SwitchPower_SetHandler(BinaryLight_GetSwitchPower(binaryLight), &handler, NULL);
        }

        LOG("UpnpRuntime_Start", UpnpRuntime_Start(runtime));
        {
            cmd_loop(runtime, binaryLight);
        }
        LOG("UpnpRuntime_Stop", UpnpRuntime_Stop(runtime));

        UpnpDeviceConfig_Delete(config);
        BinaryLight_Delete(binaryLight);
        UpnpRuntime_Delete(runtime);
    }
    UpnpInitializer_Destroy();

    return 0;
}
