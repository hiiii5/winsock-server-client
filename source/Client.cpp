#include "Client.h"

#include <cstdio>
#include <string>

#include "Communication.h"

int Client::Init(const char* Address, const uint16_t Port)
{
    const auto _ = NetworkAgent::Init(Address, Port);
    if(_ != Success)
    {
        return _;
    }

    //Socket
    comSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (comSocket_ == INVALID_SOCKET)
    {
        printf("DEBUG// Socket function incorrect\n");
        return INVALID_SOCKET;
    }

    //Connect
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = inet_addr(Address);
    serverAddr.sin_port = htons(Port);

    const int result = connect(comSocket_, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        printf("DEBUG// Connect function incorrect\n");
        return SOCKET_ERROR;
    }

    return Start();
}

int Client::Start()
{
    char buffer[1024];
    while (true)
    {
        const auto _ = Update();
        if(_ != Success && _ != AlreadyConnected)
        {
            printf("Ret from update: %d", _);
            break;
        }
        
        auto input = Communication::ReadInput();

        if(input.empty())
        {
            continue; // Don't send empty input.
        }
        
        strcpy(buffer, input.c_str());
        buffer[strlen(buffer)+1] = '\0';
        const uint8_t length = strlen(buffer)+1;
        Send(comSocket_, buffer, length);

        if(input.find("$exit") != std::string::npos)
        {
            break;
        }
    }
    
    Stop();
    return Success; // 0 is success
}

int Client::Update()
{
    // Read data as this is not a listen socket.
    constexpr uint8_t size = 255;
    const auto buffer = new char[size]; // Buffer is never repointed.
    buffer[0] = '\0';

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(comSocket_, &readSet);

    timeval tv;
    tv.tv_sec = 1; // Wait 1 second to listen for new connections
    tv.tv_usec = 0;

    const int status = select(comSocket_+1, &readSet, nullptr, nullptr, &tv);

    if(status == SOCKET_ERROR)
    {
        // There were no sockets ready. TODO: Verify that this is really an error?
        return Disconnect;
    }
    if(status == 0)
    {
        // No data to receive
        return Success;
    }
    
    // Read incoming data to the buffer.
    const auto result = Read(comSocket_, buffer, size);

    if(result != Success)
    {
        return result;
    }

    printf("%s\n", buffer);
    return Success;
}

int Client::Read(SOCKET TargetSocket, char* Buffer, int32_t Size)
{
    // Receive the size of the message.
    uint8_t messageSize = 0;
    int result = Communication::Receive(comSocket_, reinterpret_cast<char*>(&messageSize), 1);
    if (result == SOCKET_ERROR) {
        return Disconnect;
    }
    if (result == 0) {
        return Shutdown;
    }

    // Receive the actual message.
    result = Communication::Receive(comSocket_, Buffer, messageSize);
    if (result == SOCKET_ERROR) {
        return Disconnect;
    }
    if (result == 0) {
        return Shutdown;
    }

    if (result < messageSize) {
        return ParameterError;
    }

    return Success;
}

int Client::Send(SOCKET TargetSocket, const char* Data, int32_t Size)
{
    // Send the length of the message.
    int result = Communication::Send(comSocket_, reinterpret_cast<char*>(&Size), 1);
    if (result == SOCKET_ERROR) {
        return Disconnect;
    }
    if (result == 0) {
        return Shutdown;
    }

    if(Size < 0 || Size > 255) {
        return ParameterError;
    }

    // Send the actual message.
    result = Communication::Send(comSocket_, Data, static_cast<uint8_t>(Size));
    if (result == SOCKET_ERROR) {
        return Disconnect;
    }
    if (result == 0) {
        return Shutdown;
    }

    return Success;
}

void Client::Stop()
{
    // close both sockets
    shutdown(comSocket_, SD_BOTH);
    closesocket(comSocket_);
    
    NetworkAgent::Stop();
}
