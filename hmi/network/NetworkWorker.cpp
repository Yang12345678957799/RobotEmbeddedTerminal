#include "network/NetworkWorker.h"

#include <QTcpSocket>
#include <QTimer>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <QtEndian>

#include <cstring>


NetworkWorker::NetworkWorker(
    QObject* parent
)
    : QObject(parent)
{
}


// ==================================================
// 在线程启动后初始化 QTcpSocket / QTimer
// ==================================================

void NetworkWorker::initialize()
{
    if (socket_ != nullptr)
    {
        return;
    }

    socket_ =
        new QTcpSocket(this);

    heartbeatTimer_ =
        new QTimer(this);

    heartbeatTimer_->setInterval(
        1000
    );

    connect(
        socket_,
        &QTcpSocket::connected,
        this,
        &NetworkWorker::onConnected
    );

    connect(
        socket_,
        &QTcpSocket::disconnected,
        this,
        &NetworkWorker::onDisconnected
    );

    connect(
        socket_,
        &QTcpSocket::readyRead,
        this,
        &NetworkWorker::onReadyRead
    );

    connect(
        heartbeatTimer_,
        &QTimer::timeout,
        this,
        &NetworkWorker::sendHeartbeat
    );

    emit logMessage(
        "NetworkWorker initialized."
    );
}


// ==================================================
// 连接 Gateway
// ==================================================

void NetworkWorker::connectServer(
    const QString& ip,
    quint16 port
)
{
    if (socket_ == nullptr)
    {
        emit logMessage(
            "Socket is not initialized."
        );

        return;
    }

    if (socket_->state() !=
        QAbstractSocket::UnconnectedState)
    {
        socket_->abort();
    }

    receiveBuffer_.clear();

    emit logMessage(
        QString(
            "Connecting to %1:%2 ..."
        )
        .arg(ip)
        .arg(port)
    );

    socket_->connectToHost(
        ip,
        port
    );
}


// ==================================================
// 主动断开
// ==================================================

void NetworkWorker::disconnectServer()
{
    if (heartbeatTimer_ != nullptr)
    {
        heartbeatTimer_->stop();
    }

    if (socket_ == nullptr)
    {
        return;
    }

    socket_->disconnectFromHost();
}


// ==================================================
// TCP连接成功
// ==================================================

void NetworkWorker::onConnected()
{
    emit logMessage(
        "Gateway connected."
    );

    emit connectionChanged(
        true
    );

    if (heartbeatTimer_ != nullptr)
    {
        heartbeatTimer_->start();
    }

    // 连接成功后立即发一次 ping
    sendHeartbeat();
}


// ==================================================
// TCP断开
// ==================================================

void NetworkWorker::onDisconnected()
{
    if (heartbeatTimer_ != nullptr)
    {
        heartbeatTimer_->stop();
    }

    receiveBuffer_.clear();

    emit connectionChanged(
        false
    );

    emit logMessage(
        "Gateway disconnected."
    );
}


// ==================================================
// 收到 TCP 数据
// ==================================================

void NetworkWorker::onReadyRead()
{
    if (socket_ == nullptr)
    {
        return;
    }

    receiveBuffer_.append(
        socket_->readAll()
    );

    parseFrames();
}


// ==================================================
// 解析
//
// 协议：
// [4 Byte Length][N Byte JSON]
// ==================================================

void NetworkWorker::parseFrames()
{
    constexpr quint32 maxPayloadSize =
        1024 * 1024;

    while (receiveBuffer_.size() >= 4)
    {
        quint32 networkLength = 0;

        std::memcpy(
            &networkLength,
            receiveBuffer_.constData(),
            sizeof(networkLength)
        );

        const quint32 payloadLength =
            qFromBigEndian(
                networkLength
            );

        if (payloadLength >
            maxPayloadSize)
        {
            emit logMessage(
                "Invalid payload length."
            );

            receiveBuffer_.clear();

            disconnectServer();

            return;
        }

        const int totalLength =
            4 +
            static_cast<int>(
                payloadLength
            );

        // 数据还没有收完整
        if (receiveBuffer_.size() <
            totalLength)
        {
            return;
        }

        const QByteArray payload =
            receiveBuffer_.mid(
                4,
                payloadLength
            );

        receiveBuffer_.remove(
            0,
            totalLength
        );

        handlePayload(
            payload
        );
    }
}


// ==================================================
// 解析 JSON
// ==================================================

void NetworkWorker::handlePayload(
    const QByteArray& payload
)
{
    QJsonParseError error;

    const QJsonDocument document =
        QJsonDocument::fromJson(
            payload,
            &error
        );

    if (error.error !=
        QJsonParseError::NoError)
    {
        emit logMessage(
            QString(
                "JSON parse error: %1"
            )
            .arg(
                error.errorString()
            )
        );

        return;
    }

    if (!document.isObject())
    {
        return;
    }

    const QJsonObject root =
        document.object();

    const QString type =
        root.value(
            "type"
        ).toString();

    // --------------------------
    // Heartbeat pong
    // --------------------------

    if (type == "pong")
    {
        emit logMessage(
            "Heartbeat pong."
        );

        return;
    }

    // --------------------------
    // RobotState
    // --------------------------

    if (type != "robot_state")
    {
        return;
    }

    const QJsonObject data =
        root.value(
            "data"
        ).toObject();

    RobotState state;

    state.connected =
        data.value(
            "connected"
        ).toBool();

    const QJsonArray joint =
        data.value(
            "joint"
        ).toArray();

    for (int i = 0;
         i < 6 &&
         i < joint.size();
         ++i)
    {
        state.joint[i] =
            joint.at(i).toDouble();
    }

    const QJsonArray pose =
        data.value(
            "pose"
        ).toArray();

    for (int i = 0;
         i < 6 &&
         i < pose.size();
         ++i)
    {
        state.pose[i] =
            pose.at(i).toDouble();
    }

    state.mode =
        data.value(
            "mode"
        ).toString();

    state.task =
        data.value(
            "task"
        ).toString();

    state.xcoreRunning =
        data.value(
            "xcore_running"
        ).toBool();

    state.cpuUsage =
        data.value(
            "cpu_usage"
        ).toDouble();

    state.memoryUsage =
        data.value(
            "memory_usage"
        ).toDouble();

    state.timestampMs =
        static_cast<quint64>(
            data.value(
                "timestamp_ms"
            ).toDouble()
        );

    emit robotStateReceived(
        state
    );
}


// ==================================================
// 每秒发送 ping
// ==================================================

void NetworkWorker::sendHeartbeat()
{
    if (socket_ == nullptr)
    {
        return;
    }

    if (socket_->state() !=
        QAbstractSocket::ConnectedState)
    {
        return;
    }

    QJsonObject ping;

    ping.insert(
        "type",
        "ping"
    );

    const QByteArray json =
        QJsonDocument(
            ping
        ).toJson(
            QJsonDocument::Compact
        );

    sendJson(
        json
    );
}


// ==================================================
// JSON -> Length + JSON
// ==================================================

void NetworkWorker::sendJson(
    const QByteArray& json
)
{
    if (socket_ == nullptr)
    {
        return;
    }

    if (socket_->state() !=
        QAbstractSocket::ConnectedState)
    {
        return;
    }

    const quint32 payloadLength =
        static_cast<quint32>(
            json.size()
        );

    const quint32 networkLength =
        qToBigEndian(
            payloadLength
        );

    QByteArray packet;

    packet.resize(4);

    std::memcpy(
        packet.data(),
        &networkLength,
        sizeof(networkLength)
    );

    packet.append(
        json
    );

    socket_->write(
        packet
    );
}