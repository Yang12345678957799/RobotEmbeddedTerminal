import asyncio

from asyncua import Client

SERVER_URL = "opc.tcp://192.168.21.10:4840"


def enum_name(value):
    try:
        return value.name
    except Exception:
        return str(value)


async def main():
    print("========================================")
    print(" RobotEmbeddedTerminal OPC UA Probe")
    print("========================================")
    print("Server:", SERVER_URL)
    print()

    client = Client(url=SERVER_URL, timeout=5)

    try:
        print("[1/1] Requesting server endpoints...")

        endpoints = await client.connect_and_get_server_endpoints()

        print()
        print("Endpoint discovery: SUCCESS")
        print("Endpoint count:", len(endpoints))
        print()

        for index, endpoint in enumerate(endpoints):
            print("========================================")
            print(f"Endpoint # {index}")
            print("========================================")

            print(
                "Endpoint URL:",
                getattr(endpoint, "EndpointUrl", "")
            )

            print(
                "Security Mode:",
                enum_name(
                    getattr(endpoint, "SecurityMode", "")
                )
            )

            print(
                "Security Policy:",
                getattr(endpoint, "SecurityPolicyUri", "")
            )

            server = getattr(endpoint, "Server", None)

            if server is not None:
                print(
                    "Application URI:",
                    getattr(server, "ApplicationUri", "")
                )

                application_name = getattr(
                    server,
                    "ApplicationName",
                    None
                )

                if application_name is not None:
                    print(
                        "Application Name:",
                        getattr(application_name, "Text", "")
                    )

                print(
                    "Product URI:",
                    getattr(server, "ProductUri", "")
                )

            print("User Identity Tokens:")

            tokens = getattr(
                endpoint,
                "UserIdentityTokens",
                []
            )

            if not tokens:
                print("  <none>")

            for token in tokens:
                print(
                    "  PolicyId:",
                    getattr(token, "PolicyId", "")
                )

                print(
                    "  TokenType:",
                    enum_name(
                        getattr(token, "TokenType", "")
                    )
                )

                print(
                    "  Token Security Policy:",
                    getattr(
                        token,
                        "SecurityPolicyUri",
                        ""
                    )
                )

                print()

    except Exception as exc:
        print()
        print("Endpoint discovery: FAILED")
        print("Exception:", type(exc).__name__)
        print("Message:", str(exc))


if __name__ == "__main__":
    asyncio.run(main())