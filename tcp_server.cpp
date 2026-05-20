#include <cstdint>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#define BUFFER_SIZE 1
#define LISTEN_BACKLOG 50

int main(void)
{
    /* 
    TODO:
        - socket()
        - bind()
        - listen()
        - accept()
            - recv()
            - send()
        - close()
    */

    int                         down_flag = 0;
    int                         ret;
    int                         connection_socket;
    int                         data_socket;
    uint8_t                     sum;
    ssize_t                     r, s;
    uint8_t                    buffer[BUFFER_SIZE];
    struct addrinfo             hints;
    struct addrinfo             *result, *rp;


    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(NULL, "49200", &hints, &result);
    if (s != 0)
    {
        perror("Error calling getaddrinfo()");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        connection_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (connection_socket == -1)
        {
            continue;
        }

        // Add it here, before bind()
        int opt = 1;
        setsockopt(connection_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (bind(connection_socket, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            /* Success binding here */
            break;
        }

        close(connection_socket);
    }

    freeaddrinfo(result);

    if (rp == NULL)
    {
        perror("Could not bind");
        exit(EXIT_FAILURE);
    }

    /* Like the unix socket demo: sum up values from the client and return */
    ret = listen(connection_socket, LISTEN_BACKLOG);
    if (ret == -1)
    {
        perror("Error during listen");
        exit(EXIT_FAILURE);
    }

    /* Main loop for handling connections */
    for (;;)
    {
        data_socket = accept(connection_socket, NULL, NULL);

        if (data_socket == -1)
        {
            perror("Error during accept");
            exit(EXIT_FAILURE);
        }

        sum = 0;

        for (;;)
        {
            /* Wait for the next data packet */
            r = recv(data_socket, buffer, sizeof(buffer), 0);
            if (r == -1)
            {
                perror("Error during read");
                exit(EXIT_FAILURE);
            }

            /* Handle commands */
            if (buffer[0] == 0xFE)
            {
                down_flag = 1;
                continue;
            }

            if (buffer[0] == 0xFF)
            {
                break;
            }

            if (down_flag)
            {
                continue;
            }

            sum += buffer[0];
        }

        /* Send result */
        s = send(data_socket, &sum, 1, 0);
        if (s == -1)
        {
            perror("Error during send");
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
    exit(EXIT_SUCCESS);
}