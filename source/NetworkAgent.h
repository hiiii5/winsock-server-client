#pragma once
#include <cstdint>
#include <winsock2.h>

class NetworkAgent
{
public:
    virtual int Init(const char* Address, uint16_t Port);
    virtual int Start() = 0;
    virtual int Update() = 0;
    /**
     * \brief Reads data from the target socket into the buffer.
     * \param TargetSocket Socket to read data from.
     * \param Buffer Buffer to ready data into.
     * \param Size Maximum size of the buffer.
     * \return Status of read, Success if no errors occurred.
     */
    virtual int Read(SOCKET TargetSocket, char* Buffer, int32_t Size) = 0;
    /**
     * \brief Sends data to the target socket.
     * \param TargetSocket Socket to send data to.
     * \param Data Data to send.
     * \param Size Size of the data to send.
     * \return Status of send, Success if no errors occurred.
     */
    virtual int Send(SOCKET TargetSocket, const char* Data, int32_t Size) = 0;
    virtual void Stop();
    
protected:
    char* sendBuffer_ = nullptr;
};
