#include "backend/OpcUaBackend.h"

#include <iostream>

int main()
{
    std::cout
        << "========================================\n"
        << " OpcUaBackend TaskState Test\n"
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

    TaskState state;

    std::cout
        << "\n[2/3] Reading TaskState...\n";

    if (!backend.readTaskState(state))
    {
        std::cerr
            << "readTaskState: FAILED\n";

        backend.disconnect();

        return 1;
    }

    std::cout
        << "readTaskState: SUCCESS\n\n";

    std::cout
        << "Project: "
        << (state.projectName.empty()
                ? "(unknown)"
                : state.projectName)
        << '\n';

    std::cout
        << "Current Task: "
        << state.currentTask
        << '\n';

    std::cout
        << "Status: "
        << state.status
        << '\n';

    std::cout
        << "Task List Size: "
        << state.taskList.size()
        << '\n';

    for (const auto& task :
         state.taskList)
    {
        std::cout
            << "  - "
            << task
            << '\n';
    }

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
        << " OpcUaBackend TaskState test: SUCCESS\n"
        << "========================================\n";

    return 0;
}