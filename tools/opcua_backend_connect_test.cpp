#include "backend/OpcUaBackend.h"

#include <iostream>

int main()
{
    std::cout
        << "========================================\n"
        << " OpcUaBackend Connection Test\n"
        << "========================================\n";

    OpcUaBackend backend;

    std::cout
        << "\n[1/3] Initial state\n";

    std::cout
        << "isConnected: "
        << (backend.isConnected()
                ? "true"
                : "false")
        << '\n';

    std::cout
        << "\n[2/3] Connecting\n";

    if (!backend.connect())
    {
        std::cerr
            << "OpcUaBackend connect: FAILED\n";

        return 1;
    }

    std::cout
        << "isConnected: "
        << (backend.isConnected()
                ? "true"
                : "false")
        << '\n';

    if (!backend.isConnected())
    {
        std::cerr
            << "Unexpected connection state\n";

        return 1;
    }

    std::cout
        << "\n[3/3] Disconnecting\n";

    backend.disconnect();

    std::cout
        << "isConnected: "
        << (backend.isConnected()
                ? "true"
                : "false")
        << '\n';

    if (backend.isConnected())
    {
        std::cerr
            << "Disconnect test: FAILED\n";

        return 1;
    }

    std::cout
        << "\n========================================\n"
        << " OpcUaBackend connection test: SUCCESS\n"
        << "========================================\n";

    return 0;
}