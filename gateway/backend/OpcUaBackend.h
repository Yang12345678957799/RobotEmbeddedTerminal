#pragma once

#include "backend/RobotBackend.h"

#include <open62541/client.h>

#include <string>

class OpcUaBackend : public RobotBackend
{
public:
    explicit OpcUaBackend(
        const std::string& endpoint =
            "opc.tcp://192.168.21.10:4840"
    );

    ~OpcUaBackend() override;

    bool connect() override;

    void disconnect() override;

    bool isConnected() const override;

    bool readRobotState(RobotState& state) override;

    bool readIOState(IOState& state) override;

    bool readTaskState(TaskState& state) override;

    bool pollAlarmEvent(AlarmEvent& event) override;

private:
    std::string endpoint_;

    UA_Client* client_{nullptr};

    bool connected_{false};
};