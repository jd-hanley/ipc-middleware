#include "socket.hpp"
#include <sys/un.h>

Socket::Socket(int  fd)
{
    _fd = fd;
}

Socket::~Socket()
{
    close(_fd);
}