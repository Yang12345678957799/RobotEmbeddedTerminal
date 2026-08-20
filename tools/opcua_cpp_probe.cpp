#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>

#include <array>
#include <iomanip>
#include <iostream>
#include <string>

namespace
{

constexpr const char* kEndpoint =
    "opc.tcp://192.168.21.10:4840";

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
constexpr UA_UInt32 kXCoreVersionNode = 54424;


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
            << "Read Double failed. Node ns=1;i="
            << identifier
            << ", status="
            << UA_StatusCode_name(status)
            << '\n';
    }

    UA_Variant_clear(&value);

    return ok;
}


bool readInt32(
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
        UA_Variant_hasScalarType(
            &value,
            &UA_TYPES[UA_TYPES_INT32]))
    {
        output =
            *static_cast<UA_Int32*>(
                value.data
            );

        ok = true;
    }
    else
    {
        std::cerr
            << "Read Int32 failed. Node ns=1;i="
            << identifier
            << ", status="
            << UA_StatusCode_name(status)
            << '\n';
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
            << "Read Boolean failed. Node ns=1;i="
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
            << "Read String failed. Node ns=1;i="
            << identifier
            << ", status="
            << UA_StatusCode_name(status)
            << '\n';
    }

    UA_Variant_clear(&value);

    return ok;
}

} // namespace


int main()
{
    std::cout
        << "========================================\n"
        << " RobotEmbeddedTerminal C++ OPC UA Probe\n"
        << "========================================\n"
        << "Server: "
        << kEndpoint
        << "\n\n";

    UA_Client* client =
        UA_Client_new();

    if (!client)
    {
        std::cerr
            << "UA_Client_new failed.\n";

        return 1;
    }

    UA_ClientConfig* config =
        UA_Client_getConfig(client);

    UA_ClientConfig_setDefault(config);

    std::cout
        << "[1/4] Connecting...\n";

    UA_StatusCode status =
        UA_Client_connect(
            client,
            kEndpoint
        );

    if (status != UA_STATUSCODE_GOOD)
    {
        std::cerr
            << "Connection FAILED: "
            << UA_StatusCode_name(status)
            << '\n';

        UA_Client_delete(client);

        return 2;
    }

    std::cout
        << "Connection: SUCCESS\n\n";

    // ---------------------------------------------
    // Robot mode
    // ---------------------------------------------

    std::cout
        << "[2/4] Robot mode\n";

    UA_Int32 mode = -1;

    if (readInt32(
            client,
            kModeNode,
            mode))
    {
        std::cout
            << "OperationalMode: "
            << mode
            << " ("
            << modeToString(mode)
            << ")\n";
    }

    std::cout << '\n';

    // ---------------------------------------------
    // Joint position
    // ---------------------------------------------

    std::cout
        << "[3/4] Joint positions\n";

    std::cout
        << std::fixed
        << std::setprecision(6);

    for (std::size_t i = 0;
         i < kJointNodes.size();
         ++i)
    {
        double position = 0.0;

        if (readDouble(
                client,
                kJointNodes[i],
                position))
        {
            std::cout
                << "J"
                << (i + 1)
                << ": "
                << position
                << " deg\n";
        }
    }

    std::cout << '\n';

    // ---------------------------------------------
    // Task / xCore
    // ---------------------------------------------

    std::cout
        << "[4/4] Task / Controller\n";

    bool taskLoaded = false;
    std::string taskName;
    std::string xcoreVersion;

    if (readBoolean(
            client,
            kTaskLoadedNode,
            taskLoaded))
    {
        std::cout
            << "TaskProgramLoaded: "
            << (taskLoaded ? "true" : "false")
            << '\n';
    }

    if (readString(
            client,
            kTaskNameNode,
            taskName))
    {
        std::cout
            << "TaskProgramName: "
            << taskName
            << '\n';
    }

    if (readString(
            client,
            kXCoreVersionNode,
            xcoreVersion))
    {
        std::cout
            << "xCore Version: "
            << xcoreVersion
            << '\n';
    }

    UA_Client_disconnect(client);
    UA_Client_delete(client);

    std::cout
        << "\n========================================\n"
        << " C++ OPC UA probe: SUCCESS\n"
        << "========================================\n";

    return 0;
}