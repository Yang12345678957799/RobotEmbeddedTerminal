#include "backend/MockBackend.h"

#include <chrono>
#include <cmath>

MockBackend::MockBackend()
{
}

bool MockBackend::connect()
{
    connected_ = true;
    simulationTime_ = 0.0;

    return true;
}

void MockBackend::disconnect()
{
    connected_ = false;
}

bool MockBackend::isConnected() const
{
    return connected_;
}

bool MockBackend::readRobotState(RobotState& state)
{
    if (!connected_)
    {
        return false;
    }

    simulationTime_ += 0.1;

    state.connected = true;

    // 模拟 J1 ~ J6 关节角变化
    for (std::size_t i = 0; i < state.joint.size(); ++i)
    {
        state.joint[i] =
            30.0 * std::sin(simulationTime_ + static_cast<double>(i) * 0.3);
    }

    // 模拟 TCP 位姿
    state.pose[0] = 500.0 + 20.0 * std::sin(simulationTime_);
    state.pose[1] = 100.0 + 10.0 * std::cos(simulationTime_);
    state.pose[2] = 300.0;

    state.pose[3] = 180.0;
    state.pose[4] = 0.0;
    state.pose[5] = 90.0;

    state.mode = "AUTO";
    state.task = "MockTask";

    state.xcoreRunning = true;

    state.cpuUsage =
        35.0 + 5.0 * std::sin(simulationTime_);

    state.memoryUsage =
        42.0 + 3.0 * std::cos(simulationTime_);

    const auto now =
        std::chrono::system_clock::now().time_since_epoch();

    state.timestampMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    return true;
}