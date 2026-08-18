#pragma once

#include <array>
#include <cstdint>

struct IOState
{
    // 第一版先模拟 8 路数字输入
    std::array<bool, 8> di{};

    // 第一版先模拟 8 路数字输出
    std::array<bool, 8> dout{};

    std::uint64_t timestampMs{0};
};