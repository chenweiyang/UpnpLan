#include "UpnpInitializer.h"
#include "UpnpRegistry.h"
#include "command.h"

int main(void)
{
    UpnpRegistry *r = NULL;

    UpnpInitializer_Initialize();

    r = UpnpRegistry_New();
    if (r == NULL)
    {
        printf("UpnpRegistry_New failed!\n");
        return 0;
    }

    cmd_loop(r);

    UpnpRegistry_Delete(r);

    UpnpInitializer_Destroy();

    return 0;
}