#pragma once

#include "model/RobotState.h"
#include "model/IOState.h"
#include "model/TaskState.h"
#include "model/AlarmEvent.h"

#include <cstdint>
#include <string>
#include <vector>

class ProtocolCodec
{
public:
    static std::string robotStateToJson(
        const RobotState& state
    );

    static std::string ioStateToJson(
        const IOState& state
    );

    static std::string taskStateToJson(
        const TaskState& state
    );

    static std::string alarmEventToJson(
        const AlarmEvent& event
    );

    static std::vector<std::uint8_t> packMessage(
        const std::string& payload
    );
};