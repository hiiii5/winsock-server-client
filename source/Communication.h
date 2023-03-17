#pragma once
#include <cstdint>
#include <string>
#include <winsock2.h>


enum Status
{
    Success = 0,
    Shutdown,
    Disconnect,
    BindError,
    ConnectError,
    SetupError,
    StartupError,
    AddressError,
    ParameterError,
    MessageError,
    AlreadyConnected,
    ListenNotReady
};

class Communication
{
    static int TcpRecvWhole(SOCKET S, char* Buf, int Len);
    static int TcpSendWhole(SOCKET SkSocket, const char* Data, uint16_t Length);
    
public:
    static bool Communication::GetLineAsync(std::istream& Is, std::string& Str, const char Delim = '\n');
    static std::string ReadInput();
    
    /**
     * \brief Wrapper to receive data from the network.
     * \param TargetSocket Socket to receive data from.
     * \param TargetBuffer The buffer stream to write the data to.
     * \param ExpectedLength The length of the message.
     * \return The amount of received data.
     */
    static int Receive(SOCKET TargetSocket, char* TargetBuffer, int ExpectedLength);

    /**
     * \brief Wrapper to send data to the network for listeners to receive.
     * \param TargetSocket Socket to send the data to.
     * \param Data Stream of data to send.
     * \param Length Length of streamed data.
     * \return Amount of data sent.
     */
    static int Send(SOCKET TargetSocket, const char* Data, uint16_t Length);
};
