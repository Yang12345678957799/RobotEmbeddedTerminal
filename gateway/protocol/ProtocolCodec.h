#pragma once

#include "model/RobotState.h"
#include "model/IOState.h"

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

    static std::vector<std::uint8_t> packMessage(
        const std::string& payload
    );
};