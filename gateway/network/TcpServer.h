#pragma once

#include <cstdint>
#include <string>
#include <vector>

class TcpServer
{
public:
    TcpServer();

    ~TcpServer();

    bool start(std::uint16_t port);

    bool waitForClient();

    bool sendPacket(
        const std::vector<std::uint8_t>& packet);

    bool receivePackets(
        std::vector<std::string>& payloads);

    void closeClient();

    void stop();

private:
    int serverFd_{-1};

    int clientFd_{-1};

    std::vector<std::uint8_t> receiveBuffer_;
};