#pragma once

#include <vector>
#include <thread>

#include "socket.h"
#include "connection_processor.h"

using namespace std;

class Server {
public:
    Server(int port);
    void listen();
    void process_connection(Socket&& s);
    void accept();
    ~Server();

private:
    Socket socket_;
    std::vector<std::thread> thread_pool;
    ConnectionProcessor connection_processor;
};