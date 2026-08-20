# RobotEmbeddedTerminal Real Data Sources

## 1. Controller Baseline

Robot Assist UI:
5.0.10.0218

Controller:
2.2.2

Interpreter:
v2.2.2

Motion Control:
2.2.0.1

Ubuntu:
16.04.6 LTS

Kernel:
4.14.128-xenomai-3.0.9

Xenomai:
3.0.9

Orocos RTT:
2.9.x / Xenomai


## 2. Known Network Services

| Service | Process | Current Status | Purpose |
|---|---|---|---|
| TCP 4567 | updatemanager | Listening | Robot Assist / Update service |
| UDP 4567 | updatemanager | Listening | Purpose TBD |
| TCP 5050 | xCore | Listening | Robot Assist Controller service |
| UDP 5052 | xCore | Listening | Purpose TBD |
| TCP 6666 | xCore | Listening | Purpose TBD |
| TCP 127.0.0.1:2066 | xCore | Listening | Local service, purpose TBD |
| TCP 4840 | OPC UA | Not listening | OPC UA currently disabled |


## 3. Official SDK Investigation

Controller Version:

2.2.2

Official xCore SDK found:

No obvious official SDK found in the first filesystem investigation.

Investigated locations:

- /opt
- /usr/local
- /home/luoshi
- /home/luoshi/bin/controller

Observed candidates:

- ROS Kinetic development files
- Orocos RTT / Xenomai development libraries
- libemllI8254x.so
- libemllI8254x_v3.so

Conclusion:

No file found so far can be confirmed as an official xCore Controller 2.2.2 application SDK.


## 4. OPC UA Investigation

Configuration file:

/home/luoshi/bin/controller/module/opcua/opcua.settings.json

OPC UA module configuration exists:

YES

Configured port:

4840

Current enabled state:

false

Anonymous access:

false

Server certificate:

Not configured

Server private key:

Not configured

Configured users:

None found

Configured variable list:

null

Current TCP 4840 listener:

NO

Current conclusion:

Controller 2.2.2 contains an OPC UA configuration entry,
but OPC UA is currently disabled and no usable external
data nodes have yet been verified.

OPC UA Server:
CONFIRMED

Endpoint:
opc.tcp://192.168.21.10:4840

Server Application URI:
urn:xcore.opcua.server

Application Name:
Robot OPC UA Application

Product URI:
http://open62541.org

Endpoint Discovery:
PASSED

Session Creation:
PASSED

Namespace Array:
ns=0 http://opcfoundation.org/UA/
ns=1 urn:xcore.opcua.server
ns=2 http://opcfoundation.org/UA/DI/
ns=3 http://opcfoundation.org/UA/Robotics/

Robot Root Object:
ns=1;i=58192

Browse Name:
RokaeRobot

Address Space Probe:
IN PROGRESS

Anonymous Session:
PASSED after controlled OPC UA authentication
configuration change.

## 5. Current Data Availability


| Data | OPC UA |
|---|---|
| Connection | Confirmed |
| Controller Version | Confirmed |
| Robot Model | Confirmed |
| Robot Mode | Node confirmed, enum mapping pending |
| Protective Stop | Confirmed |
| Emergency Stop | Confirmed |
| Joint J1-J6 | Node confirmed |
| Joint Speed | Node confirmed |
| Task | Basic nodes confirmed |
| TCP Pose | Not found |
| IO | Not found |
| Alarm | Not found |

OperationalMode:
ns=1;i=54435

J1 ActualPosition:
ns=1;i=54329

J2 ActualPosition:
ns=1;i=54336

J3 ActualPosition:
ns=1;i=54343

J4 ActualPosition:
ns=1;i=54350

J5 ActualPosition:
ns=1;i=54357

J6 ActualPosition:
ns=1;i=54364

xCore SoftwareRevision:
ns=1;i=54424

## 6. Backend Decision

Priority:

1. Official xCore SDK
2. OPC UA / public interface
3. ControllerAgent
4. MockBackend remains permanently for testing

Current decision:

NOT YET FINALIZED

Reason:

No official xCore SDK has been confirmed.

An OPC UA configuration module exists, but it is currently
disabled and its exposed variable list is null.

The next step is to perform a safe OPC UA availability PoC
before deciding whether OpcUaBackend can be used.