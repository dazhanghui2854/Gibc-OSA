

#include <osa.h>


Int32 OSA_attachSignalHandler(Int32 sigId, void (*handler)(Int32 ) )
{
    struct sigaction sigAction;

    /* insure a clean shutdown if user types ctrl-c */
    sigAction.sa_handler = handler;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;

    return sigaction(sigId, &sigAction, NULL);
}


