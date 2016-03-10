
#include <stdio.h>
#include "sc_log.h"
#include "sc_socket.h"
#include "ScTimer.h"
#include "command.h"

int main(int argc, char *argv[])
{
    ScTimer *t = NULL;

    LOG_OPEN(NULL);
    sc_socket_init();

    t = ScTimer_New();
    ScTimer_Initialize(t, 1000 * 1000 * 1, 0);

    ScTimer_Start(t, on_timeout_listener, NULL);

  	cmd_loop(t);

    ScTimer_Stop(t);
    ScTimer_Delete(t);

    LOG_CLOSE();

    return 0;
}
