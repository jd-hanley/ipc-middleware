#pragma once

#include <string>
#include <cstdint>
#include "socket.hpp"

class ServerSocket
{

public:

    /**
     * Opens a Unix domain socket to listen at the given path
     */
    static ServerSocket listen_unix(const std::string& path);

    /**
     * Opens a TCP domain socket to listen at the given port
     */
    static ServerSocket listen_tcp(uint16_t port);

    /**
     * Accept new connections and return the connected socket
     */
    Socket accept();

private:
    explicit ServerSocket(int fd);
    int _fd;
};