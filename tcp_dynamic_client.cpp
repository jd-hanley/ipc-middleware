
#include <cstdint>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>

#define BUFFER_SIZE 1

int main(void)
{

    /* 
    TODO
        - socket()
        - connect()
        - recv() / send()
        - close()
    */
    int                         data_socket;
    int                         ret;
    ssize_t                     r, s;
    struct addrinfo             hints;
    struct addrinfo             *result, *rp;
    uint8_t                     val;
    uint8_t                     buffer[BUFFER_SIZE];

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo("127.0.0.1", "49200", &hints, &result);
    if (s != 0)
    {
        perror("Error calling getaddrinfo()");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        data_socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (data_socket == -1)
        {
            continue;
        }

        if (connect(data_socket, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            /* Success binding here */
            break;
        }

        close(data_socket);
    }

    freeaddrinfo(result);

    if (rp == NULL)
    {
        perror("Could not bind");
        exit(EXIT_FAILURE);
    }

    /* Main input loop */
    for (;;)
    {
        int input;
        std::cout << "Enter a number to add to the sum (254 to end and shut down server, 255 to end): ";
        std::cin >> input;

        // Now, cast to val
        val = static_cast<uint8_t>(input);

        // note that val needs to be sent to the server no matter what
        s = send(data_socket, &val, 1, 0);

        // Need to check the user input, they may have requested to break out
        if (val == 0xFF)
        {
            break;
        }

        if (val == 0xFE)
        {
            val = 0xFF;
            s = send(data_socket, &val, 1, 0);
            break;
        }


    }

    // Receive the result from the server
    r = recv(data_socket, buffer, sizeof(buffer), 0);
    if (r == -1) 
    {
        std::perror("Error during read");
        exit(EXIT_FAILURE);
    }

    printf("Result = %d\n", buffer[0]);

    /* Close socket.  */

    close(data_socket);

    exit(EXIT_SUCCESS);
   
}