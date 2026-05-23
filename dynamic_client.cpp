#define SOCKET_NAME "/tmp/mysocket"
#define LISTEN_BACKLOG 50
#define BUFFER_SIZE 1

#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdint>
#include <iostream>

int main(void)
{
    /* What I want to do here:
    - Create a client socket
    - Connect to the server
    - Take user input and send to the server
    */

    int                 data_socket;
    int                 ret;
    ssize_t             r,s;
    struct sockaddr_un  addr;
    uint8_t             val;
    uint8_t             buffer[BUFFER_SIZE];
    // bool                terminate_server { expect_result };

    // Think... how does this work
    // I first need to create the socket
    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    // Always good to do some error checking
    if (data_socket == - 1)
    {
        std::perror("Error obtaining socket");
        exit(EXIT_FAILURE);
    }

    // Ok, now we need to connect
    // I need to build the sockaddr structure
    // I think I start with memset to reset everything in the structure to zero
    memset(&addr, 0, sizeof(addr));

    // Nice, ok now I need to set the family and the path name of the struct
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    // Ready to call connect
    ret = connect(data_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (ret == -1)
    {
        std::perror("Error during connect");
        exit(EXIT_FAILURE);
    }

    // Here is where we divide 
    // I want to build this to take user input until the user specifies they are done
    for (;;)
    {
        // Start by taking in user input
        int input;
        std::cout << "Enter a number to add to the sum (254 to end and shut down server, 255 to end): ";
        std::cin >> input;

        // Now, cast to val
        val = static_cast<uint8_t>(input);

        // note that val needs to be sent to the server no matter what
        s = send(data_socket, &val, 1, 0);

        // Need to check the user input, they may have requested to break out
        if (val == 0xFE || val == 0xFF)
        {
            break;
        }
    }

    if (val != 0xFF)
    {
        // Receive the result from the server
        r = recv(data_socket, buffer, sizeof(buffer), 0);
        if (r == -1) 
        {
            std::perror("Error during read");
            exit(EXIT_FAILURE);
        }

        printf("Result = %d\n", buffer[0]);
    }


    /* Close socket.  */

    close(data_socket);

    exit(EXIT_SUCCESS);


}