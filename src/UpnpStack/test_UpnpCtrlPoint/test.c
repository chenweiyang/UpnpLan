#include "UpnpCtrlPoint.h"
#include "command.h"


int main(void)
{
    UpnpCtrlPoint *cp = NULL;

    UpnpStackInit();

    cp = UpnpCtrlPoint_New();
    if (cp == NULL)
    {
        printf("UpnpCtrlPoint_New failed!\n");
        return 0;
    }

    cmd_loop(cp);

    UpnpCtrlPoint_Delete(cp);

    return 0;
}