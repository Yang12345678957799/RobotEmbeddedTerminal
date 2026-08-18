cmake_minimum_required(VERSION 3.16)

project(
    RobotEmbeddedTerminal
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(
    Qt5
    REQUIRED
    COMPONENTS
    Widgets
    Network
)

# ==================================================
# Gateway
# ==================================================

add_executable(
    robot_gateway

    gateway/main.cpp

    gateway/backend/MockBackend.cpp

    gateway/model/RobotState.h
    gateway/model/IOState.h
    gateway/model/TaskState.h
    gateway/model/AlarmEvent.h

    gateway/protocol/ProtocolCodec.cpp

    gateway/network/TcpServer.cpp
)

target_include_directories(
    robot_gateway

    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/gateway
)

# ==================================================
# PC HMI
# ==================================================

add_executable(
    robot_hmi

    hmi/main.cpp

    hmi/MainWindow.cpp
    hmi/MainWindow.h

    hmi/model/RobotState.h
    hmi/model/IOState.h
    hmi/model/TaskState.h
    hmi/model/AlarmEvent.h

    hmi/network/NetworkWorker.cpp
    hmi/network/NetworkWorker.h
)

target_include_directories(
    robot_hmi

    PRIVATE

    ${CMAKE_CURRENT_SOURCE_DIR}/hmi
)

target_link_libraries(
    robot_hmi

    PRIVATE

    Qt5::Widgets
    Qt5::Network
)