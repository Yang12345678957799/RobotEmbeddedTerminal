#include "protocol/ProtocolCodec.h"

#include <arpa/inet.h>
#include <cstring>

#include <nlohmann/json.hpp>

std::string ProtocolCodec::robotStateToJson(
    const RobotState& state)
{
    nlohmann::json json;

    json["type"] = "robot_state";

    json["data"] = {
        {"connected", state.connected},
        {"joint", state.joint},
        {"pose", state.pose},
        {"mode", state.mode},
        {"task", state.task},
        {"xcore_running", state.xcoreRunning},
        {"cpu_usage", state.cpuUsage},
        {"memory_usage", state.memoryUsage},
        {"timestamp_ms", state.timestampMs}
    };

    return json.dump();
}

std::string ProtocolCodec::ioStateToJson(
    const IOState& state
)
{
    nlohmann::json json;

    json["type"] =
        "io_state";

    json["data"] =
    {
        {
            "di",
            state.di
        },
        {
            "do",
            state.dout
        },
        {
            "timestamp_ms",
            state.timestampMs
        }
    };

    return json.dump();
}


std::string ProtocolCodec::taskStateToJson(
    const TaskState& state
)
{
    nlohmann::json json;

    json["type"] =
        "task_state";

    json["data"] =
    {
        {
            "project_name",
            state.projectName
        },
        {
            "task_list",
            state.taskList
        },
        {
            "current_task",
            state.currentTask
        },
        {
            "status",
            state.status
        },
        {
            "timestamp_ms",
            state.timestampMs
        }
    };

    return json.dump();
}

std::string ProtocolCodec::alarmEventToJson(
    const AlarmEvent& event
)
{
    nlohmann::json json;

    json["type"] =
        "log_event";

    json["data"] =
    {
        {
            "id",
            event.id
        },
        {
            "level",
            event.level
        },
        {
            "code",
            event.code
        },
        {
            "message",
            event.message
        },
        {
            "active",
            event.active
        },
        {
            "timestamp_ms",
            event.timestampMs
        }
    };

    return json.dump();
}


std::vector<std::uint8_t> ProtocolCodec::packMessage(
    const std::string& payload)
{
    const std::uint32_t payloadLength =
        static_cast<std::uint32_t>(payload.size());

    const std::uint32_t networkLength =
        htonl(payloadLength);

    std::vector<std::uint8_t> packet(
        sizeof(networkLength) + payload.size());

    std::memcpy(
        packet.data(),
        &networkLength,
        sizeof(networkLength));

    std::memcpy(
        packet.data() + sizeof(networkLength),
        payload.data(),
        payload.size());

    return packet;
}