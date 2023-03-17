#include "Server.h"

#include <algorithm>
#include <cstdio>
#include <winsock2.h>


#include "Command.h"
#include "Communication.h"
#include "Log.h"
#include "platform.h"

int Server::Init(const char* Address, const uint16_t Port)
{
    if(NetworkAgent::Init(Address, Port) == StartupError)
    {
        Log::Write(Log::Error, "Server's network agent failed to initialize\n");
        return StartupError;
    }

    Log::Write(Log::Message, "Successfully initialized server's network client\n");

    FD_ZERO(&sockets_);
    
    // Create the listen socket
    listenSocket_= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket_ == INVALID_SOCKET)
    {
        Log::Write(Log::Error, "Socket function incorrect\n");
        //printf("DEBUG// Socket function incorrect\n");
        return INVALID_SOCKET;
    }

    Log::Write(Log::Message, "Created listen socket %llu\n", listenSocket_);

    // Bind to the listen socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    serverAddr.sin_port = htons(Port);

    int result = bind(listenSocket_, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        Log::Write(Log::Error, "Bind function incorrect\n");
        //printf("DEBUG// Bind function incorrect\n");
        return BindError;
    }

    Log::Write(Log::Message, "Bound to listen socket %llu\n", listenSocket_);

    FD_SET(listenSocket_, &sockets_);

    Log::Write(Log::Message, "Listening for initial connection\n");
    //Listen
    result = listen(listenSocket_, 1);
    if (result == SOCKET_ERROR)
    {
        Log::Write(Log::Error, "Listen function incorrect\n");
        //printf("DEBUG// Listen function incorrect\n");
        return SetupError;
    }
        
    return Start();
}

int Server::Start()
{
    while (true)
    {
        const auto _ = Update();
        if(_ != Success && _ != AlreadyConnected)
        {
            Log::Write(Log::Error, "Ret from update: %d\n", _);
            //printf("Ret from update: %d", _);
            break;
        }
    }

    Log::Write(Log::Message, "Stopping server\n");
    Stop();
    return Success;
}

int Server::Update()
{
    fd_set readFds;
        
    for (u_int i = 0; i < sockets_.fd_count; i++)
    {
        SOCKET s = sockets_.fd_array[i];
        FD_ZERO(&readFds);
        FD_SET(s, &readFds);

        timeval tv;
        tv.tv_sec = 1; // Wait 1 second to listen for new connections
        tv.tv_usec = 0;

        int status = select(s+1, &readFds, nullptr, nullptr, &tv);
        if(status == SOCKET_ERROR)
        {
            // There were no sockets ready. TODO: Verify that this is really an error?
            Log::Write(Log::Error, "SocketError from select\n");
            return SOCKET_ERROR;
        }
        if(status == 0)
        {
            // No data to receive
            continue;
        }

        // Accept connections if its the listen socket.
        if(s == listenSocket_)
        {
            const auto _ = AcceptConnections();
            if(_ != Success && _ != AlreadyConnected)
            {
                return _;
            }
                
            continue;
        }

        // Read data, this is not a listen socket.
        uint8_t size = 255;
        char* buffer = new char[size];
        buffer[0] = '\0';

        // Read incoming data to the buffer.
        const auto result = Read(s, buffer, size);

        if(result == Shutdown || result == Disconnect)
        {
            return result;
        }

        if(*buffer == '\0')
        {
            continue;
        }
        
        Log::Write(Log::Message, "%llu: %s\n", s, buffer);
        //printf("%llu: %s\n", s, buffer);

        Event callback;
        bool isDisconnecting = false;
        std::string retS;
        Command c{callback};
        
        if(!Command::TryExecute(buffer, c, isDisconnecting))
        {
            retS = "[Server] Command " + std::string(buffer) + " Invalid\n";
            Log::Write(Log::Warning, retS.c_str());
            
            strcpy(buffer, retS.c_str());
            buffer[strlen(buffer)+1] = '\0';
            const uint8_t length = strlen(buffer)+1;

            Send(s, buffer, length);
        }

        // Don't send a message to a disconnected client.
        if(!isDisconnecting)
        {
            retS = "[Server] Command " + std::string(buffer) + " Succeeded\n";
            Log::Write(Log::Warning, retS.c_str());
                
            strcpy(buffer, retS.c_str());
            buffer[strlen(buffer)+1] = '\0';
            const uint8_t length = strlen(buffer)+1;
                
            Send(s, buffer, length);
        }
        
        // Uncomment to send chat to other users.
        // for (u_int i1 = 0; i1 < sockets_.fd_count; ++i1)
        // {
        //     SOCKET s1 = sockets_.fd_array[i1];
        //     if(s1 == listenSocket_ || s1 == s) continue; // Only send message to other clients.
        //     char str[] = "%llu: %s";
        //     char formatted[255];
        //     sprintf(formatted, str, s1, buffer);
        //     
        //     Send(s, formatted, strlen(formatted));
        // }
    }

    return Success;
}

int Server::Read(const SOCKET TargetSocket, char* Buffer, int32_t Size)
{
    int result = Communication::Receive(TargetSocket, (char*)&Size, 1);
    if (result == SOCKET_ERROR) {
        Log::Write(Log::Error, "Read result was {%d}, disconnecting!\n", result);
        //printf("ERROR: Read result was {%d}, disconnecting!\n", result);
        return Disconnect;
    }
    if (result == 0) {
        char str[] = "%llu Disconnected!\n";
        char formatted[255];
        sprintf(formatted, str, TargetSocket);
        Log::Write(Log::Warning, formatted, TargetSocket);
        //printf(formatted, TargetSocket);
        
        FD_CLR(TargetSocket, &sockets_);
        
        return 0;
    }
    
    // Receive the actual message.
    result = Communication::Receive(TargetSocket, Buffer, Size);
    if (result == SOCKET_ERROR) {
        Log::Write(Log::Error, "Read result was {%d}, disconnecting!\n", result);
        //printf("ERROR: Read result was {%d}, disconnecting!\n", result);
        return Disconnect;
    }
    if (result == 0) {
        Log::Write(Log::Error, "Read result was {%d}, shutting down!\n", result);
        //printf("ERROR: Read result was {%d}, shutting down!\n", result);
        return Shutdown;
    }

    if (result < Size) {
        Log::Write(Log::Error, "Read size was {%d < %d}, disconnecting!\n", result, Size);
        //printf("ERROR: Read size was {%d < %d}, disconnecting!\n", result, Size);
        return ParameterError;
    }

    return Success;
}

int Server::Send(const SOCKET TargetSocket, const char* Data, int32_t Size)
{
    // Send the length of the message.
    int result = Communication::Send(TargetSocket, (char*)&Size, 1);
    if (result == SOCKET_ERROR) {
        return Disconnect;
    }
    if (result == 0) {
        return Shutdown;
    }

    if (Size < 0 || Size > 255) {
        return ParameterError;
    }

    // Send the actual message.
    result = Communication::Send(TargetSocket, Data, Size);
    if (result == SOCKET_ERROR) {
        return Disconnect;
    }
    if (result == 0) {
        return Shutdown;
    }

    return Success;
}

void Server::Stop()
{
    for (u_int i = 0; i < sockets_.fd_count; ++i)
    {
        SOCKET s = sockets_.fd_array[i];
        shutdown(s, SD_BOTH);
        closesocket(s);
    }

    listenSocket_ = 0;
    
    NetworkAgent::Stop();
}

int Server::AcceptConnections()
{
    SOCKET comSocket = accept(listenSocket_, nullptr, nullptr);
    if (comSocket == INVALID_SOCKET)
    {
        Log::Write(Log::Error, "Accept function incorrect\n");
        //printf("DEBUG// Accept function incorrect\n");
        return INVALID_SOCKET;
    }

    if(FD_ISSET(comSocket, &sockets_))
    {
        Log::Write(Log::Error, "Socket was already accepted!\n");
        //printf("DEBUG// Socket was already accepted!\n");
        return AlreadyConnected;
    }
    FD_SET(comSocket, &sockets_);

    char str[] = "%llu connected!\n";
    char formatted[255];
    sprintf(formatted, str, comSocket);
    Log::Write(Log::Message, formatted, comSocket);
    //printf(formatted, comSocket);

    // Send a message to all other clients
    for (int i = 0; i < sockets_.fd_count; ++i)
    {
        SOCKET s = sockets_.fd_array[i];
        if(s == listenSocket_)
        {
            continue;
        }
    
        Send(s, formatted, strlen(formatted)+1);
    }

    return Success;
}
