#pragma once

#include "model/RobotState.h"

#include <cstdint>
#include <string>
#include <vector>

class ProtocolCodec
{
public:
    // RobotState -> JSON 字符串
    static std::string robotStateToJson(const RobotState& state);

    // JSON 字符串 -> [4 Byte Length][JSON Payload]
    static std::vector<std::uint8_t> packMessage(
        const std::string& payload);
};