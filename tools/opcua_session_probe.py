import asyncio

from asyncua import Client


SERVER_URL = "opc.tcp://192.168.21.10:4840"


async def main():
    print("========================================")
    print(" RobotEmbeddedTerminal OPC UA Session Probe")
    print("========================================")
    print("Server:", SERVER_URL)
    print()

    client = Client(
        url=SERVER_URL,
        timeout=5
    )

    connected = False

    try:
        print("[1/3] Connecting OPC UA session...")

        await client.connect()
        connected = True

        print("Session connection: SUCCESS")
        print()

        print("[2/3] Reading namespace array...")

        namespaces = await client.get_namespace_array()

        print("Namespace count:", len(namespaces))

        for index, namespace in enumerate(namespaces):
            print(
                f"  ns={index}: {namespace}"
            )

        print()

        print("[3/3] Browsing Objects node...")

        objects = client.nodes.objects

        children = await objects.get_children()

        print("Objects child count:", len(children))
        print()

        for child in children:
            node_id = child.nodeid

            try:
                browse_name = await child.read_browse_name()
            except Exception:
                browse_name = "<read failed>"

            try:
                display_name = await child.read_display_name()
            except Exception:
                display_name = "<read failed>"

            print("----------------------------------------")
            print("NodeId:", node_id)
            print("BrowseName:", browse_name)
            print("DisplayName:", display_name)

        print()
        print("========================================")
        print(" Session / Namespace / Browse: SUCCESS")
        print("========================================")

    except Exception as exc:
        print()
        print("========================================")
        print(" Session / Namespace / Browse: FAILED")
        print("========================================")
        print("Exception:", type(exc).__name__)
        print("Message:", str(exc))

    finally:
        if connected:
            try:
                await client.disconnect()
            except Exception as exc:
                print(
                    "Disconnect warning:",
                    type(exc).__name__,
                    str(exc)
                )


if __name__ == "__main__":
    asyncio.run(main())