#pragma once

#include <array>

#include <QMetaType>
#include <QtGlobal>

struct IOState
{
    std::array<bool, 8> di{};

    std::array<bool, 8> dout{};

    quint64 timestampMs{0};
};

Q_DECLARE_METATYPE(IOState)