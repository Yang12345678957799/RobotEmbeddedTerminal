import json
import socket
import struct
import time


HOST = "127.0.0.1"
PORT = 9000


def pack_message(message):
    payload = json.dumps(
        message
    ).encode("utf-8")

    header = struct.pack(
        "!I",
        len(payload)
    )

    return header + payload


def recv_exact(sock, size):
    data = b""

    while len(data) < size:
        chunk = sock.recv(
            size - len(data)
        )

        if not chunk:
            raise ConnectionError(
                "Server closed connection"
            )

        data += chunk

    return data


with socket.socket(
    socket.AF_INET,
    socket.SOCK_STREAM
) as sock:

    sock.connect(
        (HOST, PORT)
    )

    print(
        f"Connected to {HOST}:{PORT}"
    )

    last_ping = 0.0

    try:

        while True:

            now = time.time()

            # 每 1 秒发送一次 ping
            if now - last_ping >= 1.0:

                ping_packet = pack_message(
                    {
                        "type": "ping"
                    }
                )

                sock.sendall(
                    ping_packet
                )

                print(
                    "[Heartbeat] ping"
                )

                last_ping = now

            # 读取 4 Byte Length
            length_bytes = recv_exact(
                sock,
                4
            )

            payload_length = struct.unpack(
                "!I",
                length_bytes
            )[0]

            # 读取完整 JSON
            payload = recv_exact(
                sock,
                payload_length
            )

            message = json.loads(
                payload.decode("utf-8")
            )

            message_type = message.get(
                "type"
            )

            if message_type == "robot_state":

                data = message["data"]

                print(
                    "J1:",
                    round(
                        data["joint"][0],
                        2
                    ),
                    "| J2:",
                    round(
                        data["joint"][1],
                        2
                    ),
                    "| X:",
                    round(
                        data["pose"][0],
                        2
                    ),
                    "| Mode:",
                    data["mode"]
                )

            elif message_type == "pong":

                print(
                    "[Heartbeat] pong"
                )

    except KeyboardInterrupt:

        print(
            "\nClient stopped."
        )