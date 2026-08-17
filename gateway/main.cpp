#include "backend/MockBackend.h"

#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    std::cout << "Robot Gateway starting..." << std::endl;

    MockBackend backend;

    if (!backend.connect())
    {
        std::cerr << "Failed to connect backend." << std::endl;

        return 1;
    }

    std::cout << "MockBackend connected." << std::endl;

    RobotState state;

    for (int frame = 0; frame < 20; ++frame)
    {
        if (backend.readRobotState(state))
        {
            std::cout
                << "Frame: " << frame
                << " | J1: " << state.joint[0]
                << " | J2: " << state.joint[1]
                << " | X: " << state.pose[0]
                << " | CPU: " << state.cpuUsage
                << "%"
                << " | Mode: " << state.mode
                << std::endl;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }

    backend.disconnect();

    std::cout << "Robot Gateway stopped." << std::endl;

    return 0;
}