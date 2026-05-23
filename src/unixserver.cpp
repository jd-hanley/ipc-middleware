#include "unixserver.hpp"
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

constexpr int LISTEN_BACKLOG { 50 };

UnixServer::UnixServer(const std::string& path)
{
    int ret;

    /*
    Need to work through all of the steps to set up this server, but making the assumption this is a unix server
        - socket()
        - bind()
        - listen()
    At that point we will be able to accept incoming connections, to be handled in a different function
    */

    /* Create a local socket */
    cfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (cfd < 0)
    {
        throw std::runtime_error("Failed to create socket.");
    }

    /* Bind the socket to a given pathname */
    unlink(path.c_str());

    /* Set the family appropriately */
    addr.sun_family = AF_UNIX;
    int written = snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", path.c_str());
    /* Check for errors here */
    if (written < 0)
    {
        throw std::runtime_error("Encoding error during snprintf.");
    }
    else if (written >= static_cast<int>(sizeof(addr.sun_path)))
    {
        throw std::runtime_error("Truncation of socket path occurred.");
    }

    ret = bind(cfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    /* Check for errors here */
    if (ret < 0)
    {
        throw std::runtime_error("Failed to bind socket.");
    }

    /* Declare the socket as a passive listening socket */
    ret = listen(cfd, LISTEN_BACKLOG);
    /* Check for errors here */
    if (ret < 0)
    {
        throw std::runtime_error("Failed to set up socket to receive connections.");
    }
}

void UnixServer::accept_client()
{
    /* Goal here is to accept a new connection and return the file descriptor to it */
    dfd = accept(cfd, NULL, NULL);

    if (dfd < 0)
    {
        throw std::runtime_error("Failed to accept connection.");
    }
}

void UnixServer::handle_connections()
{
    uint8_t                 result;
    ssize_t                 r, s;
    bool                    shutdown = false;
    /* This contains the main loop for handling connections */
    for (;;)
    {
        accept_client();

        result = 0;
        
        for (;;)
        {
            /* Wait for the next incoming byte */
            r = recv(dfd, buffer, sizeof(buffer), 0);

            /* Check for errors here */
            if (r < 0)
            {
                throw std::runtime_error("Failed to receive incoming byte.");
            }

            /* Handle commands */
            if (buffer[0] == 0xFE)
            {
                break;
            }

            else if (buffer[0] == 0xFF)
            {
                shutdown = true;
                break;
            }

            result += buffer[0];

        }

        if (!shutdown)
        {
            /* Send the result */
            s = send(dfd, &result, sizeof(result), 0);

            /* Check for errors */
            if (s < 0)
            {
                throw std::runtime_error("Failed to send result.");
            }
        }

        close(dfd);

        if (shutdown)
        {
            break;
        }
    }

    close(cfd);
    unlink(path.c_str());
    return;
}

