#pragma once

#include "model/RobotState.h"

class RobotBackend
{
public:
    virtual ~RobotBackend() = default;

    virtual bool connect() = 0;

    virtual void disconnect() = 0;

    virtual bool isConnected() const = 0;

    virtual bool readRobotState(RobotState& state) = 0;
};