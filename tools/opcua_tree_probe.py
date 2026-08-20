import asyncio

from asyncua import Client, ua


SERVER_URL = "opc.tcp://192.168.21.10:4840"

# 已经通过上一阶段确认
ROKAE_ROBOT_NODE_ID = "ns=1;i=58192"

# 第一轮不要无限递归
MAX_DEPTH = 6
MAX_NODES = 500


visited = set()
node_count = 0


def short_value(value, max_length=300):
    try:
        text = repr(value)
    except Exception:
        text = "<repr failed>"

    if len(text) > max_length:
        text = text[:max_length] + " ..."

    return text


async def dump_node(node, depth, path):
    global node_count

    if node_count >= MAX_NODES:
        return

    node_id = node.nodeid.to_string()

    if node_id in visited:
        return

    visited.add(node_id)
    node_count += 1

    try:
        browse_name = await node.read_browse_name()
        browse_name_text = browse_name.Name
    except Exception as exc:
        browse_name_text = f"<failed: {exc}>"

    try:
        display_name = await node.read_display_name()
        display_name_text = display_name.Text
    except Exception as exc:
        display_name_text = f"<failed: {exc}>"

    try:
        node_class = await node.read_node_class()
        node_class_text = node_class.name
    except Exception as exc:
        node_class = None
        node_class_text = f"<failed: {exc}>"

    current_path = f"{path}/{browse_name_text}"

    indent = "  " * depth

    print()
    print(
        f"{indent}========================================"
    )
    print(
        f"{indent}Path: {current_path}"
    )
    print(
        f"{indent}NodeId: {node_id}"
    )
    print(
        f"{indent}NodeClass: {node_class_text}"
    )
    print(
        f"{indent}BrowseName: {browse_name_text}"
    )
    print(
        f"{indent}DisplayName: {display_name_text}"
    )

    #
    # Variable 节点才尝试读取值
    #
    if node_class == ua.NodeClass.Variable:

        try:
            data_type = await node.read_data_type()

            print(
                f"{indent}DataType: "
                f"{data_type.to_string()}"
            )

        except Exception as exc:

            print(
                f"{indent}DataType: "
                f"<failed: {exc}>"
            )

        try:
            value = await node.read_value()

            print(
                f"{indent}Value: "
                f"{short_value(value)}"
            )

        except Exception as exc:

            print(
                f"{indent}Value: "
                f"<failed: {exc}>"
            )

    if depth >= MAX_DEPTH:
        return

    try:
        children = await node.get_children()

    except Exception as exc:

        print(
            f"{indent}Children: "
            f"<failed: {exc}>"
        )

        return

    for child in children:

        await dump_node(
            child,
            depth + 1,
            current_path
        )


async def main():

    print(
        "========================================"
    )
    print(
        " RobotEmbeddedTerminal"
    )
    print(
        " RokaeRobot OPC UA Tree Probe"
    )
    print(
        "========================================"
    )
    print(
        "Server:",
        SERVER_URL
    )
    print(
        "Root Node:",
        ROKAE_ROBOT_NODE_ID
    )
    print(
        "Max Depth:",
        MAX_DEPTH
    )
    print(
        "Max Nodes:",
        MAX_NODES
    )
    print()

    try:

        async with Client(
            url=SERVER_URL,
            timeout=5
        ) as client:

            print(
                "[1/2] OPC UA session connected."
            )

            rokae_robot = client.get_node(
                ROKAE_ROBOT_NODE_ID
            )

            print(
                "[2/2] Browsing RokaeRobot tree..."
            )

            await dump_node(
                rokae_robot,
                0,
                ""
            )

        print()
        print(
            "========================================"
        )
        print(
            " RokaeRobot tree browse: SUCCESS"
        )
        print(
            "========================================"
        )
        print(
            "Visited nodes:",
            node_count
        )

    except Exception as exc:

        print()
        print(
            "========================================"
        )
        print(
            " RokaeRobot tree browse: FAILED"
        )
        print(
            "========================================"
        )
        print(
            "Exception:",
            type(exc).__name__
        )
        print(
            "Message:",
            str(exc)
        )


if __name__ == "__main__":
    asyncio.run(main())