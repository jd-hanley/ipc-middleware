#include "server_socket.hpp"
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

constexpr int LISTEN_BACKLOG { 50 };

ServerSocket ServerSocket::listen_unix(const std::string& path)
{
    /**
     * socket(): obtain the file descriptor for the listening socket
     * bind(): bind the socket to the given path
     * listen(): declare the socket as a passive listening socket that will accept new connections
     */


    // Create a local socket
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
    {
        throw std::runtime_error("Failed to create socket.");
    }

    // Bind to the given pathname
    unlink(path.c_str());

    sockaddr_un addr {};

    addr.sun_family = AF_UNIX;
    int written = snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", path.c_str());
    if (written < 0)
    {
        throw std::runtime_error("Encoding error during snprintf.");
    }
    else if (written >= static_cast<int>(sizeof(addr.sun_path)))
    {
        throw std::runtime_error("Truncation of socket path occurred.");
    }

    int ret = bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (ret < 0)
    {
        throw std::runtime_error("Failed to bind socket to the given path.");
    }

    // Declare the socket as a passive listening socket
    ret = listen(fd, LISTEN_BACKLOG);
    if (ret < 0)
    {
        throw std::runtime_error("Failed to declare socket as listening.");
    }

    return ServerSocket(fd);

}

Socket ServerSocket::accept()
{
    int client_fd = ::accept(_fd, NULL, NULL);
    if (client_fd < 0)
    {
        throw std::runtime_error("Failed to accept connection.");
    }

    return Socket(client_fd);
}

ServerSocket::ServerSocket(int fd)
{
    _fd = fd;
}