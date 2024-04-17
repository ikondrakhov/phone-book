#pragma once
#include <string>

#ifdef _WIN32
#include <windows.h>
#define MSG_CONFIRM 0
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

class Socket {
public:
    Socket(int sock_desc);
    Socket(Socket&& s);
    void bind(struct sockaddr_in server_address);
    void listen(int max_queue_size) const;
    Socket accept() const;
    int recv(int* data, unsigned data_size) const;
    int recv(std::string& data, unsigned data_size) const;
    void send(const std::string message) const;
    void send(int message) const;
    ~Socket();

private:
    int sock_desc_;
    struct sockaddr_in server_address_;
};