#pragma once

#include "model/RobotState.h"
#include "model/IOState.h"
#include "model/TaskState.h"

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QtGlobal>

class QTcpSocket;
class QTimer;

class NetworkWorker : public QObject
{
    Q_OBJECT

public:
    explicit NetworkWorker(
        QObject* parent = nullptr
    );

public slots:

    // 必须在线程启动之后调用
    void initialize();

    // 连接 Gateway
    void connectServer(
        const QString& ip,
        quint16 port
    );

    // 主动断开 Gateway
    void disconnectServer();

private slots:

    void onConnected();

    void onDisconnected();

    void onReadyRead();

    void sendHeartbeat();

signals:

    // 通知 MainWindow 网络连接状态变化
    void connectionChanged(
        bool connected
    );

    // 收到新的机器人状态
    void robotStateReceived(
        const RobotState& state
    );

    // 收到新的 IO 状态
    void ioStateReceived(
        const IOState& state
    );

    void taskStateReceived(
        const TaskState& state
    );

    // 调试日志
    void logMessage(
        const QString& message
    );

private:

    // 解析 TCP 缓冲区中的完整帧
    void parseFrames();

    // 处理一个完整 JSON Payload
    void handlePayload(
        const QByteArray& payload
    );

    // 按 [4 Byte Length][JSON] 格式发送
    void sendJson(
        const QByteArray& json
    );

private:

    QTcpSocket* socket_{nullptr};

    QTimer* heartbeatTimer_{nullptr};

    QByteArray receiveBuffer_;
};