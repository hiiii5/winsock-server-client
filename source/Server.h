#pragma once
#include <winsock2.h>

#include "Event.h"
#include "NetworkAgent.h"

class Server : public NetworkAgent
{
public:
    int Init(const char* Address, uint16_t Port) override;
    int Start() override;
    int Update() override;
    int Read(SOCKET TargetSocket, char* Buffer, int32_t Size) override;
    int Send(SOCKET TargetSocket, const char* Data, int32_t Size) override;
    void Stop() override;

    int AcceptConnections();

protected:
    SOCKET listenSocket_ = 0;
    fd_set sockets_ = {};

    Event acceptEvent_;
};
