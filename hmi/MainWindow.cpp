#include "MainWindow.h"
#include "network/NetworkWorker.h"

#include <QDebug>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMetaObject>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(
        "Robot Embedded Terminal"
    );

    resize(
        1100,
        700
    );

    QWidget* centralWidget =
        new QWidget(this);

    setCentralWidget(
        centralWidget
    );

    QHBoxLayout* mainLayout =
        new QHBoxLayout(
            centralWidget
        );

    // --------------------------
    // 左侧导航栏
    // --------------------------

    navigationList_ =
        new QListWidget;

    navigationList_->setFixedWidth(
        160
    );

    navigationList_->addItem(
        "Status"
    );

    navigationList_->addItem(
        "IO"
    );

    navigationList_->addItem(
        "Task"
    );

    navigationList_->addItem(
        "Alarm"
    );

    navigationList_->addItem(
        "Setting"
    );

    // --------------------------
    // 右侧页面区域
    // --------------------------

    pageStack_ =
        new QStackedWidget;

    pageStack_->addWidget(
        createStatusPage()
    );

    pageStack_->addWidget(
        createIoPage()
    );

    pageStack_->addWidget(
        createTaskPage()
    );

    pageStack_->addWidget(
        createAlarmPage()
    );

    pageStack_->addWidget(
        createSettingPage()
    );

    mainLayout->addWidget(
        navigationList_
    );

    mainLayout->addWidget(
        pageStack_,
        1
    );

    // --------------------------
    // 导航切换页面
    // --------------------------

    connect(
        navigationList_,
        &QListWidget::currentRowChanged,
        pageStack_,
        &QStackedWidget::setCurrentIndex
    );

    navigationList_->setCurrentRow(
        0
    );

        qRegisterMetaType<RobotState>(
        "RobotState"
    );

    setupNetworkWorker();
}

MainWindow::~MainWindow()
{
    if (networkThread_ != nullptr &&
        networkThread_->isRunning())
    {
        QMetaObject::invokeMethod(
            networkWorker_,
            "disconnectServer",
            Qt::BlockingQueuedConnection
        );

        networkThread_->quit();

        networkThread_->wait();
    }
}




QWidget* MainWindow::createStatusPage()
{
    QWidget* page =
        new QWidget;

    QVBoxLayout* pageLayout =
        new QVBoxLayout(page);

    QLabel* title =
        new QLabel(
            "Robot Status"
        );

    title->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
    );

    pageLayout->addWidget(
        title
    );

    // --------------------------
    // 连接状态
    // --------------------------

    connectionLabel_ =
        new QLabel(
            "OFFLINE"
        );

    connectionLabel_->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
    );

    pageLayout->addWidget(
        connectionLabel_
    );

    // --------------------------
    // 关节状态
    // --------------------------

    QLabel* jointTitle =
        new QLabel(
            "Joint Position"
        );

    jointTitle->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
    );

    pageLayout->addWidget(
        jointTitle
    );

    QGridLayout* jointLayout =
        new QGridLayout;

    for (int i = 0; i < 6; ++i)
    {
        QLabel* name =
            new QLabel(
                QString("J%1").arg(i + 1)
            );

        jointLabels_[i] =
            new QLabel(
                "0.00 deg"
            );

        jointLayout->addWidget(
            name,
            i / 3,
            (i % 3) * 2
        );

        jointLayout->addWidget(
            jointLabels_[i],
            i / 3,
            (i % 3) * 2 + 1
        );
    }

    pageLayout->addLayout(
        jointLayout
    );

    // --------------------------
    // TCP 位姿
    // --------------------------

    QLabel* poseTitle =
        new QLabel(
            "TCP Pose"
        );

    poseTitle->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
    );

    pageLayout->addWidget(
        poseTitle
    );

    QGridLayout* poseLayout =
        new QGridLayout;

    const char* poseNames[6] =
    {
        "X",
        "Y",
        "Z",
        "RX",
        "RY",
        "RZ"
    };

    for (int i = 0; i < 6; ++i)
    {
        QLabel* name =
            new QLabel(
                poseNames[i]
            );

        poseLabels_[i] =
            new QLabel(
                "0.00"
            );

        poseLayout->addWidget(
            name,
            i / 3,
            (i % 3) * 2
        );

        poseLayout->addWidget(
            poseLabels_[i],
            i / 3,
            (i % 3) * 2 + 1
        );
    }

    pageLayout->addLayout(
        poseLayout
    );

    // --------------------------
    // 其他状态
    // --------------------------

    QFormLayout* stateLayout =
        new QFormLayout;

    modeLabel_ =
        new QLabel("Unknown");

    taskLabel_ =
        new QLabel("None");

    xcoreLabel_ =
        new QLabel("Unknown");

    cpuLabel_ =
        new QLabel("0.0 %");

    memoryLabel_ =
        new QLabel("0.0 %");

    stateLayout->addRow(
        "Robot Mode:",
        modeLabel_
    );

    stateLayout->addRow(
        "Current Task:",
        taskLabel_
    );

    stateLayout->addRow(
        "xCore:",
        xcoreLabel_
    );

    stateLayout->addRow(
        "CPU Usage:",
        cpuLabel_
    );

    stateLayout->addRow(
        "Memory Usage:",
        memoryLabel_
    );

    pageLayout->addLayout(
        stateLayout
    );

    pageLayout->addStretch();

    return page;
}


QWidget* MainWindow::createIoPage()
{
    QWidget* page =
        new QWidget;

    QVBoxLayout* layout =
        new QVBoxLayout(page);

    QLabel* title =
        new QLabel(
            "IO Monitor"
        );

    title->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
    );

    layout->addWidget(title);

    layout->addWidget(
        new QLabel(
            "DI / DO data will be displayed here."
        )
    );

    layout->addStretch();

    return page;
}


QWidget* MainWindow::createTaskPage()
{
    QWidget* page =
        new QWidget;

    QVBoxLayout* layout =
        new QVBoxLayout(page);

    QLabel* title =
        new QLabel(
            "Task Monitor"
        );

    title->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
    );

    layout->addWidget(title);

    layout->addWidget(
        new QLabel(
            "Robot project and task information."
        )
    );

    layout->addStretch();

    return page;
}


QWidget* MainWindow::createAlarmPage()
{
    QWidget* page =
        new QWidget;

    QVBoxLayout* layout =
        new QVBoxLayout(page);

    QLabel* title =
        new QLabel(
            "Alarm"
        );

    title->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
    );

    layout->addWidget(title);

    layout->addWidget(
        new QLabel(
            "Robot alarm and event history."
        )
    );

    layout->addStretch();

    return page;
}


QWidget* MainWindow::createSettingPage()
{
    QWidget* page =
        new QWidget;

    QVBoxLayout* layout =
        new QVBoxLayout(page);

    QLabel* title =
        new QLabel(
            "Connection Setting"
        );

    title->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
    );

    layout->addWidget(
        title
    );

    QFormLayout* form =
        new QFormLayout;

    // --------------------------
    // Gateway IP
    // --------------------------

    gatewayIpEdit_ =
        new QLineEdit(
            "127.0.0.1"
        );

    form->addRow(
        "Gateway IP:",
        gatewayIpEdit_
    );

    // --------------------------
    // Gateway Port
    // --------------------------

    gatewayPortSpin_ =
        new QSpinBox;

    gatewayPortSpin_->setRange(
        1,
        65535
    );

    gatewayPortSpin_->setValue(
        9000
    );

    form->addRow(
        "Gateway Port:",
        gatewayPortSpin_
    );

    layout->addLayout(
        form
    );

    // --------------------------
    // Connect 按钮
    // --------------------------

    connectButton_ =
        new QPushButton(
            "Connect"
        );

    connect(
        connectButton_,
        &QPushButton::clicked,
        this,
        &MainWindow::onConnectButtonClicked
    );

    layout->addWidget(
        connectButton_
    );

    layout->addStretch();

    return page;
}

void MainWindow::setupNetworkWorker()
{
    networkThread_ =
        new QThread(this);

    networkWorker_ =
        new NetworkWorker;

    networkWorker_->moveToThread(
        networkThread_
    );

    // --------------------------
    // 线程启动后初始化 socket
    // --------------------------

    connect(
        networkThread_,
        &QThread::started,
        networkWorker_,
        &NetworkWorker::initialize
    );

    // --------------------------
    // MainWindow -> NetworkWorker
    // --------------------------

    connect(
        this,
        &MainWindow::connectGatewayRequested,
        networkWorker_,
        &NetworkWorker::connectServer
    );

    connect(
        this,
        &MainWindow::disconnectGatewayRequested,
        networkWorker_,
        &NetworkWorker::disconnectServer
    );

    // --------------------------
    // NetworkWorker -> MainWindow
    // --------------------------

    connect(
        networkWorker_,
        &NetworkWorker::connectionChanged,
        this,
        &MainWindow::onConnectionChanged
    );

    connect(
        networkWorker_,
        &NetworkWorker::robotStateReceived,
        this,
        &MainWindow::onRobotStateReceived
    );

    connect(
        networkWorker_,
        &NetworkWorker::logMessage,
        this,
        &MainWindow::onNetworkLog
    );

    // --------------------------
    // Worker释放
    // --------------------------

    connect(
        networkThread_,
        &QThread::finished,
        networkWorker_,
        &QObject::deleteLater
    );

    networkThread_->start();
}

void MainWindow::onConnectButtonClicked()
{
    if (gatewayConnected_)
    {
        emit disconnectGatewayRequested();

        return;
    }

    const QString ip =
        gatewayIpEdit_->text().trimmed();

    const quint16 port =
        static_cast<quint16>(
            gatewayPortSpin_->value()
        );

    emit connectGatewayRequested(
        ip,
        port
    );
}

void MainWindow::onConnectionChanged(
    bool connected
)
{
    gatewayConnected_ =
        connected;

    if (connected)
    {
        connectionLabel_->setText(
            "ONLINE"
        );

        connectButton_->setText(
            "Disconnect"
        );

        gatewayIpEdit_->setEnabled(
            false
        );

        gatewayPortSpin_->setEnabled(
            false
        );
    }
    else
    {
        connectionLabel_->setText(
            "OFFLINE"
        );

        connectButton_->setText(
            "Connect"
        );

        gatewayIpEdit_->setEnabled(
            true
        );

        gatewayPortSpin_->setEnabled(
            true
        );
    }
}


void MainWindow::onRobotStateReceived(
    const RobotState& state
)
{
    // --------------------------
    // J1 ~ J6
    // --------------------------

    for (int i = 0; i < 6; ++i)
    {
        jointLabels_[i]->setText(
            QString(
                "%1 deg"
            )
            .arg(
                state.joint[i],
                0,
                'f',
                2
            )
        );
    }

    // --------------------------
    // TCP Pose
    // --------------------------

    for (int i = 0; i < 6; ++i)
    {
        poseLabels_[i]->setText(
            QString::number(
                state.pose[i],
                'f',
                2
            )
        );
    }

    // --------------------------
    // 其他机器人状态
    // --------------------------

    modeLabel_->setText(
        state.mode
    );

    taskLabel_->setText(
        state.task
    );

    xcoreLabel_->setText(
        state.xcoreRunning
            ? "Running"
            : "Stopped"
    );

    cpuLabel_->setText(
        QString(
            "%1 %"
        )
        .arg(
            state.cpuUsage,
            0,
            'f',
            1
        )
    );

    memoryLabel_->setText(
        QString(
            "%1 %"
        )
        .arg(
            state.memoryUsage,
            0,
            'f',
            1
        )
    );
}

void MainWindow::onNetworkLog(
    const QString& message
)
{
    qDebug()
        << "[Network]"
        << message;
}
