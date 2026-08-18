#include "MainWindow.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
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

    layout->addWidget(title);

    QFormLayout* form =
        new QFormLayout;

    QLabel* ip =
        new QLabel(
            "127.0.0.1"
        );

    QLabel* port =
        new QLabel(
            "9000"
        );

    form->addRow(
        "Gateway IP:",
        ip
    );

    form->addRow(
        "Gateway Port:",
        port
    );

    layout->addLayout(
        form
    );

    QPushButton* connectButton =
        new QPushButton(
            "Connect"
        );

    layout->addWidget(
        connectButton
    );

    layout->addStretch();

    return page;
}



