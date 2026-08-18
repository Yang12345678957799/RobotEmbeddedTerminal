#pragma once

#include "model/RobotState.h"
#include "model/IOState.h"
#include "model/TaskState.h"

class RobotBackend
{
public:
    virtual ~RobotBackend() = default;

    virtual bool connect() = 0;

    virtual void disconnect() = 0;

    virtual bool isConnected() const = 0;

    virtual bool readRobotState(RobotState& state) = 0;

    virtual bool readIOState(IOState& state) = 0;

    virtual bool readTaskState(TaskState& state)=0;
    
};