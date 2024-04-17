#include "socket.h"

#include <vector>
#include <iostream>
#include <exception>

Socket::Socket(int sock_desc): sock_desc_(sock_desc) {
}

Socket::Socket(Socket&& s): sock_desc_(s.sock_desc_) {
    s.sock_desc_ = -1;
}

void Socket::bind(struct sockaddr_in server_address) {
    server_address_ = server_address;
    if(::bind(sock_desc_, (struct sockaddr*)&server_address_, sizeof(server_address_)) == -1) {
        std::cerr << "Error binding" << std::endl;
        throw std::runtime_error("Error binding socket");
    }
}

void Socket::listen(int max_queue_size) const {
    ::listen(sock_desc_, max_queue_size);
}

Socket Socket::accept() const {
    return Socket(::accept(sock_desc_, nullptr, nullptr));
}

int Socket::recv(int* data, unsigned data_size) const {
    int recived_count = ::recv(sock_desc_, reinterpret_cast<char*>(data), data_size, 0);
    return recived_count;
}

int Socket::recv(std::string& data, unsigned data_size) const {
    int recived_total = 0;
    std::vector<char> result;
    char buffer[1024];
    while(recived_total < data_size) {
        int recived_count = ::recv(sock_desc_, buffer, data_size, 0);
        if(recived_count > 0) {
            result.insert(result.end(), buffer, buffer + recived_count);
            recived_total += recived_count;
        }
    }
    if(result.size() > 0) {
        data = result.data();
    } else {
        return -1;
    }
    return recived_total;
}

void Socket::send(const std::string message) const {
    ::send(sock_desc_, message.data(), message.size(), MSG_CONFIRM);
}

void Socket::send(int message) const {
    ::send(sock_desc_, reinterpret_cast<char*>(&message), sizeof(int), MSG_CONFIRM);
}

Socket::~Socket() {
    if(sock_desc_ > 0) {
        #ifdef _WIN32
        closesocket(sock_desc_);
        #else
        close(sock_desc_);
        #endif
    }
}