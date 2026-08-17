#include "backend/MockBackend.h"
#include "network/TcpServer.h"
#include "protocol/ProtocolCodec.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include <nlohmann/json.hpp>

int main()
{
    std::cout
        << "Robot Gateway starting..."
        << std::endl;

    MockBackend backend;

    if (!backend.connect())
    {
        std::cerr
            << "Failed to connect MockBackend."
            << std::endl;

        return 1;
    }

    std::cout
        << "MockBackend connected."
        << std::endl;

    TcpServer server;

    constexpr std::uint16_t port = 9000;

    if (!server.start(port))
    {
        std::cerr
            << "Failed to start TCP server."
            << std::endl;

        return 1;
    }

    RobotState state;

    while (true)
    {
        if (!server.waitForClient())
        {
            continue;
        }

        std::uint64_t frame = 0;

        auto lastHeartbeat =
            std::chrono::steady_clock::now();

        while (true)
        {
            // --------------------------
            // 1. 接收客户端数据
            // --------------------------

            std::vector<std::string> payloads;

            if (!server.receivePackets(payloads))
            {
                std::cout
                    << "Client disconnected."
                    << std::endl;

                break;
            }

            for (const auto& payload : payloads)
            {
                const auto message =
                    nlohmann::json::parse(
                        payload,
                        nullptr,
                        false);

                if (message.is_discarded())
                {
                    std::cerr
                        << "Invalid JSON received."
                        << std::endl;

                    continue;
                }

                if (!message.contains("type"))
                {
                    continue;
                }

                const std::string type =
                    message["type"];

                if (type == "ping")
                {
                    lastHeartbeat =
                        std::chrono::steady_clock::now();

                    nlohmann::json pong;

                    pong["type"] = "pong";

                    const std::string pongJson =
                        pong.dump();

                    const auto pongPacket =
                        ProtocolCodec::packMessage(
                            pongJson);

                    if (!server.sendPacket(pongPacket))
                    {
                        std::cout
                            << "Failed to send pong."
                            << std::endl;

                        break;
                    }
                }
            }

            // --------------------------
            // 2. 心跳超时检测
            // --------------------------

            const auto now =
                std::chrono::steady_clock::now();

            const auto heartbeatTimeout =
                std::chrono::duration_cast<
                    std::chrono::seconds>(
                    now - lastHeartbeat);

            if (heartbeatTimeout.count() >= 3)
            {
                std::cout
                    << "Heartbeat timeout."
                    << std::endl;

                break;
            }

            // --------------------------
            // 3. 获取机器人状态
            // --------------------------

            if (!backend.readRobotState(state))
            {
                std::cerr
                    << "Failed to read RobotState."
                    << std::endl;

                break;
            }

            // --------------------------
            // 4. RobotState -> JSON
            // --------------------------

            const std::string json =
                ProtocolCodec::robotStateToJson(
                    state);

            const auto packet =
                ProtocolCodec::packMessage(
                    json);

            // --------------------------
            // 5. 发给客户端
            // --------------------------

            if (!server.sendPacket(packet))
            {
                std::cout
                    << "Client disconnected."
                    << std::endl;

                break;
            }

            if (frame % 10 == 0)
            {
                std::cout
                    << "Frame: "
                    << frame
                    << " | J1: "
                    << state.joint[0]
                    << " | J2: "
                    << state.joint[1]
                    << " | Packet: "
                    << packet.size()
                    << " bytes"
                    << std::endl;
            }

            ++frame;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
        }

        server.closeClient();

        std::cout
            << "Waiting for next client..."
            << std::endl;
    }

    return 0;
}