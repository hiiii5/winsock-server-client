#pragma once

#include "NetworkAgent.h"

class Client : public NetworkAgent
{
public:
    int Init(const char* Address, uint16_t Port) override;
    int Start() override;
    int Update() override;
    int Read(SOCKET TargetSocket, char* Buffer, int32_t Size) override;
    int Send(SOCKET TargetSocket, const char* Data, int32_t Size) override;
    void Stop() override;

protected:
    SOCKET comSocket_{};
};
