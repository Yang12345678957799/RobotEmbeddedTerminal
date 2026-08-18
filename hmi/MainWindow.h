#pragma once

#include <QMainWindow>

class QLabel;
class QListWidget;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget* createStatusPage();

    QWidget* createIoPage();

    QWidget* createTaskPage();

    QWidget* createAlarmPage();

    QWidget* createSettingPage();

private:
    QListWidget* navigationList_{nullptr};

    QStackedWidget* pageStack_{nullptr};

    QLabel* connectionLabel_{nullptr};

    QLabel* jointLabels_[6]{};

    QLabel* poseLabels_[6]{};

    QLabel* modeLabel_{nullptr};

    QLabel* taskLabel_{nullptr};

    QLabel* xcoreLabel_{nullptr};

    QLabel* cpuLabel_{nullptr};

    QLabel* memoryLabel_{nullptr};
};