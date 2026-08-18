#pragma once

#include <cstdint>
#include <string>

struct AlarmEvent
{
    std::uint64_t id{0};

    std::string level{"INFO"};

    std::string code{"NONE"};

    std::string message{};

    bool active{false};

    std::uint64_t timestampMs{0};
};