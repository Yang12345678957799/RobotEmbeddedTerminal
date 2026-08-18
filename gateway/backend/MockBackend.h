#pragma once

#include "backend/RobotBackend.h"

class MockBackend : public RobotBackend
{
public:
    MockBackend();

    bool connect() override;

    void disconnect() override;

    bool isConnected() const override;

    bool readRobotState(RobotState& state) override;

    bool readIOState(IOState& state) override;

    bool readTaskState(TaskState& state) override;

    bool pollAlarmEvent(AlarmEvent& event) override;

private:
    bool connected_{false};

    double simulationTime_{0.0};

    int lastAlarmBucket_{0};

    std::uint64_t nextAlarmId_{1};
};