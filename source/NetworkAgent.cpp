#include "NetworkAgent.h"

#include "Communication.h"
#include "platform.h"

int NetworkAgent::Init(const char* Address, uint16_t Port)
{
    if(startup())
    {
        return StartupError;
    }

    return Success;
}

void NetworkAgent::Stop()
{
    shutdown();
}
