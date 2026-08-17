import json
import socket
import struct
import time


HOST = "127.0.0.1"
PORT = 9000


def make_packet(message):

    payload = json.dumps(
        message
    ).encode("utf-8")

    return (
        struct.pack(
            "!I",
            len(payload)
        )
        +
        payload
    )


sock = socket.socket(
    socket.AF_INET,
    socket.SOCK_STREAM
)

sock.connect(
    (HOST, PORT)
)

print(
    "Connected."
)

packet = make_packet(
    {
        "type": "ping"
    }
)

print(
    "Testing fragmented packet..."
)

# 故意拆成三段发送

sock.sendall(
    packet[:2]
)

time.sleep(0.1)

sock.sendall(
    packet[2:7]
)

time.sleep(0.1)

sock.sendall(
    packet[7:]
)

print(
    "Fragmented packet sent."
)

time.sleep(0.5)

print(
    "Testing sticky packets..."
)

packet1 = make_packet(
    {
        "type": "ping"
    }
)

packet2 = make_packet(
    {
        "type": "ping"
    }
)

# 故意把两条完整消息合起来一次发送

sock.sendall(
    packet1 + packet2
)

print(
    "Two packets sent together."
)

time.sleep(1)

sock.close()

print(
    "Test finished."
)