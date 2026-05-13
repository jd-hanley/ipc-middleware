

/* High level review of everything I need to do here:
        - I am creating the server side
        - That means this process will listen for new connections
        - Order of attack
            - socket(): create an endpoint for communication
            - bind(): bind a name to a socket (name=filepath?)
            - listen(): listen for connections on a socket
            - accept(): accept a connection on a socket
            - send(), recv()
*/

#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>

#define SOCKET_NAME "/tmp/237yeh2y162twh2.socket"
#define LISTEN_BACKLOG 50
#define BUFFER_SIZE 12

int main(void)
{
    int                         down_flag = 0;
    int                         ret;
    int                         connection_socket;
    int                         data_socket;
    int                         result;
    ssize_t                     r,w;
    struct sockaddr_un          name;
    char                        buffer[BUFFER_SIZE];

    /* Create a local socket */
    connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (connection_socket == -1)
    {
        std::perror("Error obtaining new socket");
        exit(EXIT_FAILURE);
    }

    /* Bind the socket to the given pathname */

    /* Begin by clearing the sockaddr_un struct */
    memset(&name, 0, sizeof(name));

    /* Set the family appropriately */
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
    unlink(SOCKET_NAME);

    ret = bind(connection_socket, reinterpret_cast<sockaddr *>(&name), sizeof(name));

    if (ret == -1)
    {
        std::perror("Error during bind");
        exit(EXIT_FAILURE);
    }

    /* Declare the socket as a passive listening socket */
    ret = listen(connection_socket, LISTEN_BACKLOG);

    if (ret == -1)
    {
        std::perror("Error during listen");
        exit(EXIT_FAILURE);
    }

    /* This is the main loop for handling connections */
    for (;;)
    {
        /* Wait for incoming connection */
        data_socket = accept(connection_socket, NULL, NULL);

        if (data_socket == -1)
        {
            std::perror("Error during accept");
            exit(EXIT_FAILURE);
        }

        result = 0;
        for (;;)
        {
            /* Wait for the next data packet */
            r = read(data_socket, buffer, sizeof(buffer));
            if (r == -1)
            {
                std::perror("Error during read");
                exit(EXIT_FAILURE);
            }

            /* Ensure buffer is null terminated */
            buffer[sizeof(buffer) - 1] = 0;

            /* Handle commands */
            if (!strncmp(buffer, "DOWN", sizeof(buffer)))
            {
                down_flag = 1;
                continue;
            }

            if (!strncmp(buffer, "END", sizeof(buffer)))
            {
                break;
            }

            if (down_flag)
            {
                continue;
            }

            result += atoi(buffer);
        }

        /* Send result */
        sprintf(buffer, "%d", result);
        w = write(data_socket, buffer, strlen(buffer) + 1);
        if (w == -1)
        {
            std::perror("Error during write");
            exit(EXIT_FAILURE);
        }

        /* Close socket */
        close(data_socket);
        if (down_flag)
        {
            break;
        }
    }

    close(connection_socket);
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}
