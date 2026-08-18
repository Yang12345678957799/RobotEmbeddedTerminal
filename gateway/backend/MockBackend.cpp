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

    lastAlarmBucket_ = 0;

    nextAlarmId_ = 1;

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


bool MockBackend::readIOState(
    IOState& state
)
{
    if (!connected_)
    {
        return false;
    }

    // simulationTime_ 已经由 readRobotState()
    // 每100ms增加0.1
    //
    // phase 大约每1秒变化一次
    const int phase =
        static_cast<int>(
            simulationTime_
        );

    // --------------------------
    // 模拟 DI0 ~ DI7
    // --------------------------

    for (std::size_t i = 0;
         i < state.di.size();
         ++i)
    {
        state.di[i] =
            ((phase +
              static_cast<int>(i))
             % 2) == 0;
    }

    // --------------------------
    // 模拟 DO0 ~ DO7
    // --------------------------

    for (std::size_t i = 0;
         i < state.dout.size();
         ++i)
    {
        state.dout[i] =
            (((phase / 2) +
              static_cast<int>(i))
             % 2) == 0;
    }

    // --------------------------
    // 时间戳
    // --------------------------

    const auto now =
        std::chrono::system_clock::now()
            .time_since_epoch();

    state.timestampMs =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                now
            )
            .count();

    return true;
}

bool MockBackend::readTaskState(
    TaskState& state
)
{
    if (!connected_)
    {
        return false;
    }

    // --------------------------
    // Mock 工程
    // --------------------------

    state.projectName =
        "DemoProject";

    // --------------------------
    // Mock 任务列表
    // --------------------------

    state.taskList =
    {
        "Home",
        "Pick",
        "Place"
    };

    // simulationTime_ 每 100 ms
    // 大约增加 0.1
    const int phase =
        static_cast<int>(
            simulationTime_
        );

    // 每 5 秒切换一次当前任务
    const int taskIndex =
        (phase / 5) %
        static_cast<int>(
            state.taskList.size()
        );

    state.currentTask =
        state.taskList[
            taskIndex
        ];

    // 大部分时间运行，
    // 每 10 秒短暂进入 PAUSED
    if ((phase % 10) < 8)
    {
        state.status =
            "RUNNING";
    }
    else
    {
        state.status =
            "PAUSED";
    }

    // --------------------------
    // 时间戳
    // --------------------------

    const auto now =
        std::chrono::system_clock::now()
            .time_since_epoch();

    state.timestampMs =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                now
            )
            .count();

    return true;
}

bool MockBackend::pollAlarmEvent(
    AlarmEvent& event
)
{
    if (!connected_)
    {
        return false;
    }

    // 每大约 5 秒进入一个新的 bucket
    const int alarmBucket =
        static_cast<int>(
            simulationTime_ / 5.0
        );

    // 还没到第一个报警时间
    if (alarmBucket <= 0)
    {
        return false;
    }

    // 当前 5 秒区间已经发过一次，
    // 不重复发送
    if (alarmBucket ==
        lastAlarmBucket_)
    {
        return false;
    }

    lastAlarmBucket_ =
        alarmBucket;

    event.id =
        nextAlarmId_++;

    // 奇数 bucket 模拟报警
    if ((alarmBucket % 2) != 0)
    {
        event.level =
            "WARNING";

        event.code =
            "MOCK_001";

        event.message =
            "Mock robot warning.";

        event.active =
            true;
    }
    else
    {
        // 偶数 bucket 模拟报警恢复
        event.level =
            "INFO";

        event.code =
            "MOCK_001";

        event.message =
            "Mock robot warning cleared.";

        event.active =
            false;
    }

    const auto now =
        std::chrono::system_clock::now()
            .time_since_epoch();

    event.timestampMs =
        std::chrono::duration_cast<
            std::chrono::milliseconds>(
                now
            )
            .count();

    return true;
}