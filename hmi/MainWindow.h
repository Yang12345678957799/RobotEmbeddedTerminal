#pragma once

#include "model/RobotState.h"
#include "model/IOState.h"

#include <QMainWindow>
#include <QString>
#include <QtGlobal>

class QLabel;
class QListWidget;
class QStackedWidget;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QThread;

class NetworkWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(
        QWidget* parent = nullptr
    );

    ~MainWindow() override;

signals:

    void connectGatewayRequested(
        const QString& ip,
        quint16 port
    );

    void disconnectGatewayRequested();

private slots:

    void onConnectButtonClicked();

    void onConnectionChanged(
        bool connected
    );

    void onRobotStateReceived(
        const RobotState& state
    );

    void onIoStateReceived(
    const IOState& state
    );

    void onNetworkLog(
        const QString& message
    );

private:

    QWidget* createStatusPage();

    QWidget* createIoPage();

    QWidget* createTaskPage();

    QWidget* createAlarmPage();

    QWidget* createSettingPage();

    void setupNetworkWorker();

private:

    // --------------------------
    // 左侧导航与页面
    // --------------------------

    QListWidget* navigationList_{
        nullptr
    };

    QStackedWidget* pageStack_{
        nullptr
    };

    // --------------------------
    // Status 页面
    // --------------------------

    QLabel* connectionLabel_{
        nullptr
    };

    QLabel* jointLabels_[6]{};

    QLabel* poseLabels_[6]{};

    QLabel* modeLabel_{
        nullptr
    };

    QLabel* taskLabel_{
        nullptr
    };

    QLabel* xcoreLabel_{
        nullptr
    };

    QLabel* cpuLabel_{
        nullptr
    };

    QLabel* memoryLabel_{
        nullptr
    };

 
    // --------------------------
    // IO 页面
    // --------------------------

    QLabel* diLabels_[8]{};

    QLabel* doLabels_[8]{};

    QLabel* ioRefreshLabel_{
        nullptr
    };

    // --------------------------
    // Setting 页面
    // --------------------------

    QLineEdit* gatewayIpEdit_{
        nullptr
    };

    QSpinBox* gatewayPortSpin_{
        nullptr
    };

    QPushButton* connectButton_{
        nullptr
    };

    bool gatewayConnected_{
        false
    };

    // --------------------------
    // 网络线程
    // --------------------------

    QThread* networkThread_{
        nullptr
    };

    NetworkWorker* networkWorker_{
        nullptr
    };
};