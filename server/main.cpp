#include <fstream>
#include <string>

#include "server.h"

int main() {
    #ifdef _WIN32
    WSADATA wsdata; 
    WSAStartup(0x0101,&wsdata);
    #endif 
    std::ifstream config;
    config.open("config.txt");
    std::string param;
    int port = 8080;
    config >> param;
    if(param == "PORT") {
        config >> port;
    }
    config.close();

    std::cout << "Creating server on port " << port << std::endl;
    Server server = Server(port);
    server.listen();
    while(1) {
        server.accept();
    }
    #ifdef _WIN32
    WSACleanup();
    #endif
}