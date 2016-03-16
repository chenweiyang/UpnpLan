#include "UpnpInitializer.h"
#include "UpnpRuntime.h"
#include "command.h"

int main(void)
{
    UpnpRuntime *runtime = NULL;

    UpnpInitializer_Initialize();

    runtime = UpnpRuntime_New();
    if (runtime == NULL)
    {
        printf("UpnpRuntime_New failed!\n");
        return 0;
    }

    cmd_loop(runtime);

    UpnpRuntime_Delete(runtime);

    UpnpInitializer_Destroy();

    return 0;
}