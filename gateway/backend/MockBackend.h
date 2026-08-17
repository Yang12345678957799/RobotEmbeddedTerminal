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

private:
    bool connected_{false};

    double simulationTime_{0.0};
};