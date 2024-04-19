#include "server.h"

using namespace std;

Server::Server(int port): socket_(socket(AF_INET, SOCK_STREAM, 0)) {
    struct sockaddr_in server_address_;
    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(8080);
    server_address_.sin_addr.s_addr = INADDR_ANY;
    socket_.bind(server_address_);
}

void Server::listen() {
    socket_.listen(5);
}

void Server::process_connection(Socket&& s) {
    int message_size;
    while(s.recv(&message_size, 4) > 0) {
        std::string message = "";
        s.recv(message, message_size);
        std::string response = connection_processor.process_message(message);
        std::cout << response << std::endl;
        s.send(response.size());
        s.send(response);
    }
    std::cout << "Connection closed" << std::endl;
}

void Server::accept() {
    Socket clientSocket = socket_.accept();
    std::cout << "New connection" << std::endl;
    //TODO: Подумать когда удалять потоки из вектора
    thread_pool.push_back(std::thread(&Server::process_connection, this, std::move(clientSocket)));
}

Server::~Server() {
    for(auto& thread: thread_pool) {
        thread.join();
    }
}