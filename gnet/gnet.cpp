#include "gnet.hpp"

#include "../async-sockets/include/udpserver.hpp"
#include <iostream>

using namespace std;

GNet::GNet(){
    udpServer = new UDPServer();
}

GNet::~GNet(){
    free(udpServer);
}

void GNet::start_server(int port){
    udpServer->Bind(port, [](int errorCode, string errorMessage) {
        // BINDING FAILED:
        cout << errorCode << " : " << errorMessage << endl;
    });
}

// int main()
// {
//     UDPServer udpServer;

//     // onMessageReceived will run when a message received with information of ip & port of sender:
//     /*udpServer.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
//         //cout << ipv4 << ":" << port << " => " << message << endl;

//         // Just send without control:
//         udpServer.SendTo("A!", ipv4, port);
//     };*/

//     // If you want to use raw byte arrays:
    
//     udpServer.onRawMessageReceived = [&](const char* message, int length, string ipv4, uint16_t port) {
//         //cout << ipv4 << ":" << port << " => " << message << "(" << length << ")" << endl;

//         // Just send without control:
//         udpServer.SendTo(message, length, ipv4, port);
//     };
    

//     // Bind the server to a port.
//     udpServer.Bind(8888, [](int errorCode, string errorMessage) {
//         // BINDING FAILED:
//         cout << errorCode << " : " << errorMessage << endl;
//     });

//     // You should do an input loop so the program will not terminated immediately:
//     string input;
//     getline(cin, input);
//     while (input != "exit")
//     {
//         getline(cin, input);
//     }

//     udpServer.Close();

//     return 0;
// }