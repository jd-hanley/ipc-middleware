#pragma once

#include <sys/un.h>
#include <unistd.h>

class Socket
{
    
public:
    explicit Socket(int connected_fd);

    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    ssize_t send_msg();
    ssize_t recv_msg();

    int fd() const;

private:
    int _fd = -1;

};