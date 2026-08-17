#include "network/TcpServer.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
    stop();
}

bool TcpServer::start(std::uint16_t port)
{
    serverFd_ = ::socket(AF_INET, SOCK_STREAM, 0);

    if (serverFd_ < 0)
    {
        std::cerr
            << "socket() failed: "
            << std::strerror(errno)
            << std::endl;

        return false;
    }

    int reuse = 1;

    if (::setsockopt(
            serverFd_,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse,
            sizeof(reuse)) < 0)
    {
        std::cerr
            << "setsockopt() failed: "
            << std::strerror(errno)
            << std::endl;
    }

    sockaddr_in address{};

    address.sin_family = AF_INET;

    address.sin_addr.s_addr = htonl(INADDR_ANY);

    address.sin_port = htons(port);

    if (::bind(
            serverFd_,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address)) < 0)
    {
        std::cerr
            << "bind() failed: "
            << std::strerror(errno)
            << std::endl;

        stop();

        return false;
    }

    if (::listen(serverFd_, 5) < 0)
    {
        std::cerr
            << "listen() failed: "
            << std::strerror(errno)
            << std::endl;

        stop();

        return false;
    }

    std::cout
        << "TCP Server listening on port "
        << port
        << std::endl;

    return true;
}

bool TcpServer::waitForClient()
{
    sockaddr_in clientAddress{};

    socklen_t clientLength =
        sizeof(clientAddress);

    std::cout
        << "Waiting for client..."
        << std::endl;

    clientFd_ = ::accept(
        serverFd_,
        reinterpret_cast<sockaddr*>(&clientAddress),
        &clientLength);

    if (clientFd_ < 0)
    {
        std::cerr
            << "accept() failed: "
            << std::strerror(errno)
            << std::endl;

        return false;
    }

    char clientIp[INET_ADDRSTRLEN]{};

    ::inet_ntop(
        AF_INET,
        &clientAddress.sin_addr,
        clientIp,
        sizeof(clientIp));

    std::cout
        << "Client connected: "
        << clientIp
        << ":"
        << ntohs(clientAddress.sin_port)
        << std::endl;

    return true;
}

bool TcpServer::sendPacket(
    const std::vector<std::uint8_t>& packet)
{
    if (clientFd_ < 0)
    {
        return false;
    }

    std::size_t totalSent = 0;

    while (totalSent < packet.size())
    {
        const ssize_t sent =
            ::send(
                clientFd_,
                packet.data() + totalSent,
                packet.size() - totalSent,
                MSG_NOSIGNAL);

        if (sent <= 0)
        {
            std::cerr
                << "send() failed or client disconnected."
                << std::endl;

            return false;
        }

        totalSent +=
            static_cast<std::size_t>(sent);
    }

    return true;
}

bool TcpServer::receivePackets(
    std::vector<std::string>& payloads)
{
    if (clientFd_ < 0)
    {
        return false;
    }

    std::uint8_t tempBuffer[4096];

    while (true)
    {
        const ssize_t received =
            ::recv(
                clientFd_,
                tempBuffer,
                sizeof(tempBuffer),
                MSG_DONTWAIT);

        if (received > 0)
        {
            receiveBuffer_.insert(
                receiveBuffer_.end(),
                tempBuffer,
                tempBuffer + received);
        }
        else if (received == 0)
        {
            return false;
        }
        else
        {
            if (errno == EAGAIN ||
                errno == EWOULDBLOCK)
            {
                break;
            }

            if (errno == EINTR)
            {
                continue;
            }

            std::cerr
                << "recv() failed: "
                << std::strerror(errno)
                << std::endl;

            return false;
        }
    }

    while (receiveBuffer_.size() >= 4)
    {
        std::uint32_t networkLength = 0;

        std::memcpy(
            &networkLength,
            receiveBuffer_.data(),
            sizeof(networkLength));

        const std::uint32_t payloadLength =
            ntohl(networkLength);

        constexpr std::uint32_t maxPayloadSize =
            1024 * 1024;

        if (payloadLength > maxPayloadSize)
        {
            std::cerr
                << "Invalid payload length: "
                << payloadLength
                << std::endl;

            return false;
        }

        const std::size_t totalLength =
            4 + payloadLength;

        if (receiveBuffer_.size() < totalLength)
        {
            break;
        }

        const char* payloadStart =
            reinterpret_cast<const char*>(
                receiveBuffer_.data() + 4);

        payloads.emplace_back(
            payloadStart,
            payloadLength);

        receiveBuffer_.erase(
            receiveBuffer_.begin(),
            receiveBuffer_.begin()
                + totalLength);
    }

    return true;
}

void TcpServer::closeClient()
{
    if (clientFd_ >= 0)
    {
        ::close(clientFd_);

        clientFd_ = -1;
    }

    receiveBuffer_.clear();
}

void TcpServer::stop()
{
    closeClient();

    if (serverFd_ >= 0)
    {
        ::close(serverFd_);

        serverFd_ = -1;
    }
}