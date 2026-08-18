#pragma once

#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QtGlobal>

struct TaskState
{
    QString projectName{"None"};

    QStringList taskList{};

    QString currentTask{"None"};

    QString status{"IDLE"};

    quint64 timestampMs{0};
};

Q_DECLARE_METATYPE(TaskState)