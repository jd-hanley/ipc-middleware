#define SOCKET_NAME "/tmp/237yeh2y162twh2.socket"
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

int main(int argc, char *argv[])
{
    int                 ret;
    int                 data_socket;
    ssize_t             r, s;
    struct sockaddr_un  addr;
    uint8_t             buffer[BUFFER_SIZE];
    uint8_t             val;

    /* Create local socket.  */

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (data_socket == -1) 
    {
        std::perror("Error obtaining new socket");
        exit(EXIT_FAILURE);
    }

    /*
    * For portability clear the whole structure, since some
    * implementations have additional (nonstandard) fields in
    * the structure.
    */

    memset(&addr, 0, sizeof(addr));

    /* Connect socket to socket address.  */

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    ret = connect(data_socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (ret == -1) 
    {
        std::perror("Error during connect");
        exit(EXIT_FAILURE);
    }

    /* Send arguments.  */

    for (int i = 1; i < argc; ++i) 
    {
        val = (uint8_t)atoi(argv[i]);
        s = send(data_socket, &val, 1, 0);
        if (s == -1) 
        {
            std::perror("Error during write");
            break;
        }
    }

    /* Request result.  */

    // strcpy(buffer, "END");
    val = 0xFF;
    // w = write(data_socket, buffer, strlen(buffer) + 1);
    s = send(data_socket, &val, 1, 0);
    if (s == -1) 
    {
        std::perror("Error during write");
        exit(EXIT_FAILURE);
    }

    /* Receive result.  */

    // r = read(data_socket, buffer, sizeof(buffer));
    r = recv(data_socket, buffer, sizeof(buffer), 0);
    if (r == -1) 
    {
        std::perror("Error during read");
        exit(EXIT_FAILURE);
    }

    /* Ensure buffer is 0-terminated.  */

    // buffer[sizeof(buffer) - 1] = 0;

    printf("Result = %d\n", buffer[0]);

    /* Close socket.  */

    close(data_socket);

    exit(EXIT_SUCCESS);
}