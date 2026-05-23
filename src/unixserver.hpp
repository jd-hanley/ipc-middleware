#pragma once
#include "server.hpp"
#include <sys/un.h>

class UnixServer : public Server
{
    private:
        std::string path;
        sockaddr_un addr{};

    public:
        UnixServer(const std::string& path);
        void accept_client() override;
        void handle_connections() override;
};