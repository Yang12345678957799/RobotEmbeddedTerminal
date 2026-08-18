#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct TaskState
{
    std::string projectName{"None"};

    std::vector<std::string> taskList{};

    std::string currentTask{"None"};

    std::string status{"IDLE"};

    std::uint64_t timestampMs{0};
};