#include "socket.hpp"
#include "server_socket.hpp"
#include <stdexcept>
#include <iostream>

int main(void)
{
    try
    {
        ServerSocket myServer = ServerSocket::listen_unix("/tmp/socket");
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

}
