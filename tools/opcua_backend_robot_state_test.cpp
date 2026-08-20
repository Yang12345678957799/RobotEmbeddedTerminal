#include "backend/OpcUaBackend.h"

#include <iomanip>
#include <iostream>

int main()
{
    std::cout
        << "========================================\n"
        << " OpcUaBackend RobotState Test\n"
        << "========================================\n";

    OpcUaBackend backend;

    std::cout
        << "\n[1/3] Connecting...\n";

    if (!backend.connect())
    {
        std::cerr
            << "Connection: FAILED\n";

        return 1;
    }

    std::cout
        << "Connection: SUCCESS\n";

    RobotState state;

    std::cout
        << "\n[2/3] Reading RobotState...\n";

    if (!backend.readRobotState(state))
    {
        std::cerr
            << "readRobotState: FAILED\n";

        backend.disconnect();

        return 1;
    }

    std::cout
        << "readRobotState: SUCCESS\n\n";

    std::cout
        << "connected: "
        << (state.connected
                ? "true"
                : "false")
        << '\n';

    std::cout
        << "mode: "
        << state.mode
        << '\n';

    std::cout
        << std::fixed
        << std::setprecision(6);

    for (std::size_t i = 0;
         i < state.joint.size();
         ++i)
    {
        std::cout
            << "J"
            << (i + 1)
            << ": "
            << state.joint[i]
            << " deg\n";
    }

    std::cout
        << "xcoreRunning: "
        << (state.xcoreRunning
                ? "true"
                : "false")
        << '\n';

    std::cout
        << "timestampMs: "
        << state.timestampMs
        << '\n';

    std::cout
        << "\n[3/3] Disconnecting...\n";

    backend.disconnect();

    std::cout
        << "isConnected: "
        << (backend.isConnected()
                ? "true"
                : "false")
        << '\n';

    std::cout
        << "\n========================================\n"
        << " OpcUaBackend RobotState test: SUCCESS\n"
        << "========================================\n";

    return 0;
}