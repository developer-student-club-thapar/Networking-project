#ifndef ANDROID_PROJECT_RECTANGLE_H
#define ANDROID_PROJECT_RECTANGLE_H

#include "../async-sockets/include/udpserver.hpp"

class GNet {
UDPServer* udpServer;

public:
    GNet();
    ~GNet();

    void start_server(int port);
};

#endif