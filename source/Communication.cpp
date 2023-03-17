#include "Communication.h"

#include <cstdio>
#include <iostream>

#include "platform.h"

// loop recv func
int Communication::TcpRecvWhole(const SOCKET S, char* Buf, const int Len)
{
    int total = 0;

    do
    {
        const int ret = recv(S, Buf + total, Len - total, 0);
        if (ret < 1)
        {
            if(getError() == 0) return ret;
            printf("ERROR: %s:%d\n", strerror(getError()), getError());
            return ret;
        }
            
        
        total += ret;
    } while (total < Len);

    return total;
}

// loop send func
int Communication::TcpSendWhole(const SOCKET SkSocket, const char *Data, const uint16_t Length)
{
    int bytesSent = 0;

    while (bytesSent < Length)
    {
        const int result = send(SkSocket, Data + bytesSent, Length - bytesSent, 0);

        if (result <= 0)
            return result;

        bytesSent += result;
    }

    return bytesSent;
}

std::string Communication::ReadInput()
{
    std::cin.clear();
    std::string answer;
    std::getline(std::cin, answer);
    return answer;
}

int Communication::Receive(const SOCKET TargetSocket, char* TargetBuffer, const int ExpectedLength)
{
    return TcpRecvWhole(TargetSocket, TargetBuffer, ExpectedLength);
}

int Communication::Send(const SOCKET TargetSocket, const char* Data, const uint16_t Length)
{
    return TcpSendWhole(TargetSocket, Data, Length);
}
