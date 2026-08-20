#include "backend/OpcUaBackend.h"

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <utility>

namespace
{

constexpr UA_UInt32 kModeNode = 54435;

constexpr std::array<UA_UInt32, 6> kJointNodes = {
    54329,
    54336,
    54343,
    54350,
    54357,
    54364
};

constexpr UA_UInt32 kTaskLoadedNode = 54430;
constexpr UA_UInt32 kTaskNameNode   = 54431;

const char* modeToString(UA_Int32 mode)
{
    switch (mode)
    {
        case 0:
            return "OTHER";

        case 1:
            return "MANUAL_REDUCED_SPEED";

        case 2:
            return "MANUAL_HIGH_SPEED";

        case 3:
            return "AUTOMATIC";

        case 4:
            return "AUTOMATIC_EXTERNAL";

        default:
            return "UNKNOWN";
    }
}

bool readDouble(
    UA_Client* client,
    UA_UInt32 identifier,
    double& output)
{
    UA_Variant value;
    UA_Variant_init(&value);

    const UA_NodeId nodeId =
        UA_NODEID_NUMERIC(
            1,
            identifier
        );

    const UA_StatusCode status =
        UA_Client_readValueAttribute(
            client,
            nodeId,
            &value
        );

    bool ok = false;

    if (status == UA_STATUSCODE_GOOD &&
        UA_Variant_hasScalarType(
            &value,
            &UA_TYPES[UA_TYPES_DOUBLE]))
    {
        output =
            *static_cast<UA_Double*>(
                value.data
            );

        ok = true;
    }
    else
    {
        std::cerr
            << "[OpcUaBackend] Read Double failed. "
            << "Node ns=1;i="
            << identifier
            << ", status="
            << UA_StatusCode_name(status)
            << '\n';
    }

    UA_Variant_clear(&value);

    return ok;
}

bool readEnumInt32(
    UA_Client* client,
    UA_UInt32 identifier,
    UA_Int32& output)
{
    UA_Variant value;
    UA_Variant_init(&value);

    const UA_NodeId nodeId =
        UA_NODEID_NUMERIC(
            1,
            identifier
        );

    const UA_StatusCode status =
        UA_Client_readValueAttribute(
            client,
            nodeId,
            &value
        );

    bool ok = false;

    if (status == UA_STATUSCODE_GOOD &&
        UA_Variant_isScalar(&value) &&
        value.data != nullptr &&
        value.type != nullptr)
    {
        switch (value.type->typeKind)
        {
        case UA_DATATYPEKIND_INT16:
            output =
                static_cast<UA_Int32>(
                    *static_cast<UA_Int16*>(
                        value.data
                    )
                );

            ok = true;
            break;

        case UA_DATATYPEKIND_INT32:
            output =
                *static_cast<UA_Int32*>(
                    value.data
                );

            ok = true;
            break;

        case UA_DATATYPEKIND_ENUM:
            if (value.type->memSize ==
                sizeof(UA_Int32))
            {
                output =
                    *static_cast<UA_Int32*>(
                        value.data
                    );

                ok = true;
            }

            break;

        default:
            break;
        }
    }

    if (!ok)
    {
        std::cerr
            << "[OpcUaBackend] "
            << "Read OperationalMode failed. "
            << "Node ns=1;i="
            << identifier
            << ", status="
            << UA_StatusCode_name(status);

        if (value.type != nullptr)
        {
            std::cerr
                << ", typeKind="
                << static_cast<int>(
                    value.type->typeKind
                )
                << ", memSize="
                << value.type->memSize;
        }

        std::cerr << '\n';
    }

    UA_Variant_clear(&value);

    return ok;
}

bool readBoolean(
    UA_Client* client,
    UA_UInt32 identifier,
    bool& output)
{
    UA_Variant value;
    UA_Variant_init(&value);

    const UA_NodeId nodeId =
        UA_NODEID_NUMERIC(
            1,
            identifier
        );

    const UA_StatusCode status =
        UA_Client_readValueAttribute(
            client,
            nodeId,
            &value
        );

    bool ok = false;

    if (status == UA_STATUSCODE_GOOD &&
        UA_Variant_hasScalarType(
            &value,
            &UA_TYPES[UA_TYPES_BOOLEAN]))
    {
        output =
            (*static_cast<UA_Boolean*>(
                value.data
            ) != 0);

        ok = true;
    }
    else
    {
        std::cerr
            << "[OpcUaBackend] Read Boolean failed. "
            << "Node ns=1;i="
            << identifier
            << ", status="
            << UA_StatusCode_name(status)
            << '\n';
    }

    UA_Variant_clear(&value);

    return ok;
}

bool readString(
    UA_Client* client,
    UA_UInt32 identifier,
    std::string& output)
{
    UA_Variant value;
    UA_Variant_init(&value);

    const UA_NodeId nodeId =
        UA_NODEID_NUMERIC(
            1,
            identifier
        );

    const UA_StatusCode status =
        UA_Client_readValueAttribute(
            client,
            nodeId,
            &value
        );

    bool ok = false;

    if (status == UA_STATUSCODE_GOOD &&
        UA_Variant_hasScalarType(
            &value,
            &UA_TYPES[UA_TYPES_STRING]))
    {
        const auto* text =
            static_cast<UA_String*>(
                value.data
            );

        output.assign(
            reinterpret_cast<const char*>(
                text->data
            ),
            text->length
        );

        ok = true;
    }
    else
    {
        std::cerr
            << "[OpcUaBackend] Read String failed. "
            << "Node ns=1;i="
            << identifier
            << ", status="
            << UA_StatusCode_name(status)
            << '\n';
    }

    UA_Variant_clear(&value);

    return ok;
}
} // namespace

OpcUaBackend::OpcUaBackend(
    const std::string& endpoint)
    : endpoint_(endpoint)
{
}

OpcUaBackend::~OpcUaBackend()
{
    disconnect();
}

bool OpcUaBackend::connect()
{
    if (connected_ && client_ != nullptr)
    {
        return true;
    }

    if (client_ != nullptr)
    {
        UA_Client_delete(client_);
        client_ = nullptr;
    }

    std::cout
        << "[OpcUaBackend] Connecting to "
        << endpoint_
        << '\n';

    client_ = UA_Client_new();

    if (client_ == nullptr)
    {
        std::cerr
            << "[OpcUaBackend] UA_Client_new failed\n";

        connected_ = false;

        return false;
    }

    UA_ClientConfig* config =
        UA_Client_getConfig(client_);

    UA_ClientConfig_setDefault(config);

    const UA_StatusCode status =
        UA_Client_connect(
            client_,
            endpoint_.c_str()
        );

    if (status != UA_STATUSCODE_GOOD)
    {
        std::cerr
            << "[OpcUaBackend] Connection failed: "
            << UA_StatusCode_name(status)
            << '\n';

        UA_Client_delete(client_);
        client_ = nullptr;

        connected_ = false;

        return false;
    }

    connected_ = true;

    std::cout
        << "[OpcUaBackend] Connection SUCCESS\n";

    return true;
}

void OpcUaBackend::disconnect()
{
    if (client_ != nullptr)
    {
        if (connected_)
        {
            UA_Client_disconnect(client_);
        }

        UA_Client_delete(client_);
        client_ = nullptr;
    }

    connected_ = false;
}

bool OpcUaBackend::isConnected() const
{
    return connected_;
}

bool OpcUaBackend::readRobotState(
    RobotState& state)
{
    if (!connected_ ||
        client_ == nullptr)
    {
        state.connected = false;

        return false;
    }

    UA_Int32 modeValue = -1;

    if (!readEnumInt32(
            client_,
            kModeNode,
            modeValue))
    {
        return false;
    }

    std::array<double, 6> joints{};

    for (std::size_t i = 0;
         i < kJointNodes.size();
         ++i)
    {
        if (!readDouble(
                client_,
                kJointNodes[i],
                joints[i]))
        {
            return false;
        }
    }

    // -----------------------------
    // 所有关键 OPC UA 数据读取成功后，
    // 再更新 RobotState。
    // -----------------------------

    state.connected = true;

    state.joint = joints;

    state.mode =
        modeToString(modeValue);

    state.xcoreRunning = true;

    // 当前尚未确认真实 TCP Pose，
    // 不生成假的位姿数据。
    state.pose.fill(0.0);

    // Task 下一阶段由 readTaskState()
    // 正式接入。
    state.task = "None";

    // CPU / Memory 当前 OPC UA
    // 尚未找到真实数据源。
    state.cpuUsage = 0.0;
    state.memoryUsage = 0.0;

    const auto now =
        std::chrono::system_clock::now()
            .time_since_epoch();

    state.timestampMs =
        static_cast<std::uint64_t>(
            std::chrono::duration_cast<
                std::chrono::milliseconds>(
                now
            ).count()
        );

    return true;
}

bool OpcUaBackend::readIOState(
    IOState& state)
{
    (void)state;

    // 当前尚未确认真实 OPC UA IO 节点。
    return false;
}

bool OpcUaBackend::readTaskState(
    TaskState& state)
{
    if (!connected_ ||
        client_ == nullptr)
    {
        return false;
    }

    bool taskLoaded = false;

    if (!readBoolean(
            client_,
            kTaskLoadedNode,
            taskLoaded))
    {
        return false;
    }

    std::string taskName;

    if (!readString(
            client_,
            kTaskNameNode,
            taskName))
    {
        return false;
    }

    // 当前 OPC UA 只明确确认了：
    // TaskProgramLoaded + TaskProgramName。
    //
    // 尚未找到真实工程名和完整任务列表接口，
    // 因此不要伪造不存在的数据。

    state.projectName = "";

    state.taskList.clear();

    if (taskLoaded &&
        !taskName.empty())
    {
        state.currentTask =
            taskName;

        state.taskList.push_back(
            taskName
        );

        // 注意：
        // TaskProgramLoaded 只证明任务已加载，
        // 不能据此声称任务正在运行。
        state.status =
            "LOADED";
    }
    else
    {
        state.currentTask =
            "None";

        state.status =
            "NOT_LOADED";
    }

    const auto now =
        std::chrono::system_clock::now()
            .time_since_epoch();

    state.timestampMs =
        static_cast<std::uint64_t>(
            std::chrono::duration_cast<
                std::chrono::milliseconds>(
                now
            ).count()
        );

    return true;
}

bool OpcUaBackend::pollAlarmEvent(
    AlarmEvent& event)
{
    (void)event;

    // 当前尚未确认真实 OPC UA Alarm 节点。
    return false;
}