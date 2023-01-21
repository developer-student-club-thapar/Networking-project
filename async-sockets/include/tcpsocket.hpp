#pragma once

#include "basesocket.hpp"
#include <string>
#include <string.h>
#include <functional>
#include <thread>

class TCPSocket : public BaseSocket
{
public:
    // Event Listeners:
    std::function<void(std::string)> onMessageReceived;
    std::function<void(const char*, int)> onRawMessageReceived;
    std::function<void(int)> onSocketClosed;

    explicit TCPSocket(FDR_ON_ERROR, int socketId = -1) : BaseSocket(onError, TCP, socketId){}

    // Send TCP Packages
    int Send(const char *bytes, size_t byteslength)
    {
        if (this->isClosed)
            return -1;

        int sent = 0;
        if ((sent = send(this->sock, bytes, byteslength, 0)) < 0)
        {
            perror("send");
        }
        return sent;
    }
    int Send(std::string message) { return this->Send(message.c_str(), message.length()); }

    void Connect(std::string host, uint16_t port, std::function<void()> onConnected = [](){}, FDR_ON_ERROR)
    {
        struct addrinfo hints, *res, *it;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        // Get address info from DNS
        int status;
        if ((status = getaddrinfo(host.c_str(), NULL, &hints, &res)) != 0) {
            onError(errno, "Invalid address." + std::string(gai_strerror(status)));
            return;
        }

        for(it = res; it != NULL; it = it->ai_next)
        {
            if (it->ai_family == AF_INET) { // IPv4
                memcpy((void*)(&this->address), (void*)it->ai_addr, sizeof(sockaddr_in));
                break; // for now, just get first ip (ipv4).
            }
        }

        freeaddrinfo(res);

        this->Connect((uint32_t)this->address.sin_addr.s_addr, port, onConnected, onError);
    }
    void Connect(uint32_t ipv4, uint16_t port, std::function<void()> onConnected = [](){}, FDR_ON_ERROR)
    {
        this->address.sin_family = AF_INET;
        this->address.sin_port = htons(port);
        this->address.sin_addr.s_addr = ipv4;

        this->setTimeout(5);

        // Try to connect.
        if (connect(this->sock, (const sockaddr *)&this->address, sizeof(sockaddr_in)) < 0)
        {
            onError(errno, "Connection failed to the host.");
            this->setTimeout(0);
            return;
        }

        this->setTimeout(0);

        // Connected to the server, fire the event.
        onConnected();

        // Start listening from server:
        this->Listen();
    }

    void Listen()
    {
        std::thread t(TCPSocket::Receive, this);
        t.detach();
    }

    void setAddressStruct(sockaddr_in addr) {this->address = addr;}
    sockaddr_in getAddressStruct() const {return this->address;}

    bool deleteAfterClosed = false;

private:
    static void Receive(TCPSocket *socket)
    {
        char tempBuffer[socket->BUFFER_SIZE];
        int messageLength;

        while ((messageLength = recv(socket->sock, tempBuffer, socket->BUFFER_SIZE, 0)) > 0)
        {
            tempBuffer[messageLength] = '\0';
            if(socket->onMessageReceived)
                socket->onMessageReceived(std::string(tempBuffer, messageLength));
            
            if(socket->onRawMessageReceived)
                socket->onRawMessageReceived(tempBuffer, messageLength);
        }

        socket->Close();
        if(socket->onSocketClosed)
            socket->onSocketClosed(errno);
        
        if (socket->deleteAfterClosed && socket != nullptr)
            delete socket;
    }

    void setTimeout(int seconds)
    {
        struct timeval tv;      
        tv.tv_sec = seconds;
        tv.tv_usec = 0;

        setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
        setsockopt(this->sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
    }
};
