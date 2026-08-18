#pragma once

#include <QMetaType>
#include <QString>
#include <QtGlobal>

struct AlarmEvent
{
    quint64 id{0};

    QString level{"INFO"};

    QString code{"NONE"};

    QString message{};

    bool active{false};

    quint64 timestampMs{0};
};

Q_DECLARE_METATYPE(AlarmEvent)