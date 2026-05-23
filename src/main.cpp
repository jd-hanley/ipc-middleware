#include "unixserver.hpp"
#include <stdexcept>
#include <iostream>

int main(void)
{
    try
    {
        UnixServer myServer("/tmp/mysocket");
        myServer.handle_connections();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    

    // std::cout << "Done" << std::endl;
    return EXIT_SUCCESS;
}