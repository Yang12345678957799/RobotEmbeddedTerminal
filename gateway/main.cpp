#include "backend/RobotBackend.h"
#include "backend/MockBackend.h"
#include "backend/OpcUaBackend.h"

#include "network/TcpServer.h"
#include "protocol/ProtocolCodec.h"
#include "model/IOState.h"
#include "model/TaskState.h"
#include "model/AlarmEvent.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>

int main(int argc, char* argv[])
{
    std::cout
        << "Robot Gateway starting..."
        << std::endl;

    std::string backendName = "mock";

    if (argc >= 2)
    {
        backendName = argv[1];
    }

    std::unique_ptr<RobotBackend> backend;

    if (backendName == "mock")
    {
        backend =
            std::make_unique<MockBackend>();
    }
    else if (backendName == "opcua")
    {
        backend =
            std::make_unique<OpcUaBackend>();
    }
    else
    {
        std::cerr
            << "Unknown backend: "
            << backendName
            << std::endl;

        std::cerr
            << "Usage: "
            << argv[0]
            << " [mock|opcua]"
            << std::endl;

        return 1;
    }

    std::cout
        << "Selected backend: "
        << backendName
        << std::endl;

    if (!backend->connect())
    {
        std::cerr
            << "Failed to connect backend: "
            << backendName
            << std::endl;

        return 1;
    }

    std::cout
        << "Backend connected: "
        << backendName
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
    IOState ioState;
    TaskState taskState;
    AlarmEvent alarmEvent;

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

            if (!backend->readRobotState(state))
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


            // --------------------------
            // 6. 获取 IO 状态
            // --------------------------

            if (backendName == "mock")
            {
                if (!backend->readIOState(ioState))
                {
                    std::cerr
                        << "Failed to read IOState."
                        << std::endl;

                    break;
                }

                const std::string ioJson =
                    ProtocolCodec::ioStateToJson(
                        ioState
                    );

                const auto ioPacket =
                    ProtocolCodec::packMessage(
                        ioJson
                    );

                if (!server.sendPacket(ioPacket))
                {
                    std::cout
                        << "Client disconnected while sending IO."
                        << std::endl;

                    break;
                }
            }


            // --------------------------
            // 9. 获取 Task 状态
            // --------------------------

            if (!backend->readTaskState(taskState))
            {
                std::cerr
                    << "Failed to read TaskState."
                    << std::endl;

                break;
            }


            // --------------------------
            // 10. TaskState -> JSON
            // --------------------------

            const std::string taskJson =
                ProtocolCodec::taskStateToJson(
                    taskState
                );

            const auto taskPacket =
                ProtocolCodec::packMessage(
                    taskJson
                );


            // --------------------------
            // 11. 发给客户端
            // --------------------------

            if (!server.sendPacket(taskPacket))
            {
                std::cout
                    << "Client disconnected while sending Task."
                    << std::endl;

                break;
            }               
            
            // --------------------------
            // 12. 检查 Alarm Event
            // --------------------------

            if (backend->pollAlarmEvent(
                    alarmEvent
                ))
            {
                const std::string alarmJson =
                    ProtocolCodec::alarmEventToJson(
                        alarmEvent
                    );

                const auto alarmPacket =
                    ProtocolCodec::packMessage(
                        alarmJson
                    );

                if (!server.sendPacket(
                        alarmPacket
                    ))
                {
                    std::cout
                        << "Client disconnected while sending Alarm."
                        << std::endl;

                    break;
                }
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