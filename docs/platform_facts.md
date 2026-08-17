# Platform Facts

## Original Robot Controller

Robot Assist UI: 5.0.10.0218

xCore Controller: 2.2.2

Upgrade Manager: 0.7.1

Interpreter: v2.2.2

Motion Control: 2.2.0.1

Operating System:
Ubuntu 16.04.6 LTS

Kernel:
4.14.128-xenomai-3.0.9

Xenomai:
3.0.9

Orocos RTT:
2.9.x with Xenomai

## Startup Chain

rc.local
→ updatemanager --simu_mode --monitor
→ xCore --simu_mode

## Known Network Ports

TCP 4567:
updatemanager / Robot Assist Upgrade Service

TCP 5050:
xCore / Robot Assist Controller Service

UDP 5052:
xCore, purpose TBD

TCP 6666:
xCore, purpose TBD

127.0.0.1:2066:
xCore local service, purpose TBD

## Development Policy

Ubuntu 16.04 controller is treated as Golden Image.

Do NOT:

- upgrade Ubuntu
- replace Xenomai
- replace Orocos
- replace system libraries
- modify xCore
- modify updatemanager
- modify controller core files

All new development is performed on Ubuntu 22.04.