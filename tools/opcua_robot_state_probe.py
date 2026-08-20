import asyncio
from asyncua import Client, ua


SERVER_URL = "opc.tcp://192.168.21.10:4840"

NODE_MODE = "ns=1;i=54435"

NODE_PROTECTIVE_STOP = "ns=1;i=54434"
NODE_EMERGENCY_STOP = "ns=1;i=54436"

NODE_SPEED_OVERRIDE = "ns=1;i=54316"

JOINT_POSITION_NODES = [
    "ns=1;i=54329",
    "ns=1;i=54336",
    "ns=1;i=54343",
    "ns=1;i=54350",
    "ns=1;i=54357",
    "ns=1;i=54364",
]

JOINT_SPEED_NODES = [
    "ns=1;i=54327",
    "ns=1;i=54334",
    "ns=1;i=54341",
    "ns=1;i=54348",
    "ns=1;i=54355",
    "ns=1;i=54362",
]

NODE_ROBOT_MODEL = "ns=1;i=54322"
NODE_XCORE_VERSION = "ns=1;i=54424"

NODE_TASK_LOADED = "ns=1;i=54430"
NODE_TASK_NAME = "ns=1;i=54431"

MODE_DATA_TYPE = "ns=3;i=3006"


async def read_value(client, node_id):
    node = client.get_node(node_id)
    return await node.read_value()


async def dump_children(client, node_id, title):
    print()
    print("========================================")
    print(title)
    print("NodeId:", node_id)
    print("========================================")

    node = client.get_node(node_id)

    try:
        children = await node.get_children()
    except Exception as exc:
        print("Cannot browse children:", exc)
        return

    print("Child count:", len(children))

    for child in children:
        try:
            browse_name = await child.read_browse_name()
        except Exception:
            browse_name = None

        try:
            node_class = await child.read_node_class()
        except Exception:
            node_class = None

        print()
        print("Child NodeId:", child.nodeid)

        if browse_name is not None:
            print("BrowseName:", browse_name.Name)

        if node_class is not None:
            print("NodeClass:", node_class.name)

        if node_class == ua.NodeClass.Variable:
            try:
                value = await child.read_value()
                print("Value:", value)
            except Exception as exc:
                print("Value read failed:", exc)


async def main():
    print("========================================")
    print(" RobotEmbeddedTerminal")
    print(" OPC UA Robot State Probe")
    print("========================================")
    print("Server:", SERVER_URL)
    print()

    async with Client(
        url=SERVER_URL,
        timeout=5
    ) as client:

        print("[1/5] Session connected: SUCCESS")
        print()

        # -------------------------------------------------
        # Controller / Robot basic information
        # -------------------------------------------------

        print("[2/5] Controller information")

        robot_model = await read_value(
            client,
            NODE_ROBOT_MODEL
        )

        xcore_version = await read_value(
            client,
            NODE_XCORE_VERSION
        )

        print("Robot Model :", robot_model)
        print("xCore       :", xcore_version)
        print()

        # -------------------------------------------------
        # Robot mode / safety
        # -------------------------------------------------

        print("[3/5] Robot mode / safety")

        mode = await read_value(
            client,
            NODE_MODE
        )

        protective_stop = await read_value(
            client,
            NODE_PROTECTIVE_STOP
        )

        emergency_stop = await read_value(
            client,
            NODE_EMERGENCY_STOP
        )

        speed_override = await read_value(
            client,
            NODE_SPEED_OVERRIDE
        )

        print("OperationalMode :", mode)
        print("ProtectiveStop  :", protective_stop)
        print("EmergencyStop   :", emergency_stop)
        print("SpeedOverride   :", speed_override)
        print()

        # -------------------------------------------------
        # Joint position / speed
        # -------------------------------------------------

        print("[4/5] Joint state")

        position_nodes = [
            client.get_node(node_id)
            for node_id in JOINT_POSITION_NODES
        ]

        speed_nodes = [
            client.get_node(node_id)
            for node_id in JOINT_SPEED_NODES
        ]

        positions = await asyncio.gather(
            *[
                node.read_value()
                for node in position_nodes
            ]
        )

        speeds = await asyncio.gather(
            *[
                node.read_value()
                for node in speed_nodes
            ]
        )

        for index in range(6):
            print(
                f"J{index + 1}: "
                f"position={positions[index]}, "
                f"speed={speeds[index]}"
            )

        print()

        # -------------------------------------------------
        # Task
        # -------------------------------------------------

        print("[5/5] Task state")

        task_loaded = await read_value(
            client,
            NODE_TASK_LOADED
        )

        task_name = await read_value(
            client,
            NODE_TASK_NAME
        )

        print("TaskProgramLoaded :", task_loaded)
        print("TaskProgramName   :", repr(task_name))

        # -------------------------------------------------
        # Metadata investigation
        # -------------------------------------------------

        await dump_children(
            client,
            MODE_DATA_TYPE,
            "OperationalMode DataType metadata"
        )

        await dump_children(
            client,
            JOINT_POSITION_NODES[0],
            "Axis1 ActualPosition metadata"
        )

    print()
    print("========================================")
    print(" Robot state probe: SUCCESS")
    print("========================================")


if __name__ == "__main__":
    asyncio.run(main())