#pragma once

#include <array>

#include <QMetaType>
#include <QString>
#include <QtGlobal>

struct RobotState
{
    bool connected{false};

    // J1 ~ J6
    std::array<double, 6> joint{};

    // x, y, z, rx, ry, rz
    std::array<double, 6> pose{};

    QString mode{"Unknown"};
    QString task{"None"};

    bool xcoreRunning{false};

    double cpuUsage{0.0};
    double memoryUsage{0.0};

    quint64 timestampMs{0};
};

Q_DECLARE_METATYPE(RobotState)