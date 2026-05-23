#pragma once
#include <string>
#include <cstdint>

class Server
{
    protected:
        int cfd = -1;
        int dfd = -1;
        uint8_t buffer[1];
    
    public:
        virtual ~Server() = default;
        virtual void accept_client() = 0;
        virtual void handle_connections() = 0;
};
