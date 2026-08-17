#pragma once

#include <array>
#include <cstdint>
#include <string>

struct RobotState
{
    bool connected{false};

    // J1 ~ J6
    std::array<double, 6> joint{};

    // x, y, z, rx, ry, rz
    std::array<double, 6> pose{};

    std::string mode{"Unknown"};
    std::string task{"None"};

    bool xcoreRunning{false};

    double cpuUsage{0.0};
    double memoryUsage{0.0};

    std::uint64_t timestampMs{0};
};