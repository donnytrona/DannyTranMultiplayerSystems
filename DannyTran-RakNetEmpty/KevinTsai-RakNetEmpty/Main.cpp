#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"

#include <iostream>
#include <thread>
#include <chrono>

static unsigned int SERVER_PORT = 65000;
static unsigned int CLIENT_PORT = 65001;
static unsigned int MAX_CONNECTIONS = 4;

bool isServer = false;
bool isRunning = true;

RakNet::RakPeerInterface *g_rakPeerInterface = nullptr;
unsigned char GetPacketIdentifier(RakNet::Packet *p);


enum NetworkState {
	NS_Init,
	NS_PendingStart,
	NS_Started
};

NetworkState g_networkState = NS_Init;

void InputHandler() {
	while (isRunning) {
		//Check if were in the right state
		char userInput[255];
		if (g_networkState == NS_Init) {
			std::cout << "press (s) for server (c) for client" << std::endl;
			std::cin >> userInput;
			isServer = userInput[0] == 's';
			g_networkState = NS_PendingStart;
		}

		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

void PacketHandler() {
	while (isRunning) {
		for (RakNet::Packet* packet = g_rakPeerInterface->Receive(); packet != nullptr; g_rakPeerInterface->DeallocatePacket(packet), packet = g_rakPeerInterface->Receive())
		{
			// We got a packet, get the identifier with our handy function
			unsigned char packetIdentifier = GetPacketIdentifier(packet);

			// Check if this is a network message packet
			switch (packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				// Connection lost normally
				printf("ID_ALREADY_CONNECTED with guid %" PRINTF_64_BIT_MODIFIER "u\n", packet->guid);
				break;
			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_CONNECTION_LOST\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
				printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
				break;
			case ID_CONNECTION_BANNED: // Banned from this server
				printf("We are banned from this server.\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				// Sorry, the server is full.  I don't do anything here but
				// A real app should tell the user
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;

			case ID_INVALID_PASSWORD:
				printf("ID_INVALID_PASSWORD\n");
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST\n");
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", packet->systemAddress.ToString(true), packet->guid.ToString());
				printf("My external address is %s\n", g_rakPeerInterface->GetExternalID(packet->systemAddress).ToString(true));
				break;
			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				printf("Ping from %s\n", packet->systemAddress.ToString(true));
				break;
			default:
				// It's a client, so just show the message
				printf("%s\n", packet->data);
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}


int main()
{
	g_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();

	//For input handling
	std::thread InputHandler(InputHandler);
	//Packet Handling
	std::thread PacketHandler(PacketHandler);

	while (isRunning) {
		if (g_networkState == NS_PendingStart) {

			if (isServer)
			{
				RakNet::SocketDescriptor socketDescriptors[1];
				socketDescriptors[0].port = SERVER_PORT;
				socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4

				bool isSuccess = g_rakPeerInterface->Startup(MAX_CONNECTIONS, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
				assert(isSuccess);
				g_rakPeerInterface->SetMaximumIncomingConnections(MAX_CONNECTIONS);

				std::cout << "server started" << std::endl;
				g_networkState = NS_Started;
			}
			else
			{
				RakNet::SocketDescriptor socketDescriptor(CLIENT_PORT, 0);
				socketDescriptor.socketFamily = AF_INET;
				bool isSuccess = g_rakPeerInterface->Startup(8, &socketDescriptor, 1) == RakNet::RAKNET_STARTED;
				assert(isSuccess);
				g_rakPeerInterface->SetOccasionalPing(true);

				RakNet::ConnectionAttemptResult result = g_rakPeerInterface->Connect("127.0.0.1", SERVER_PORT, 0, 0);
				RakAssert(result == RakNet::CONNECTION_ATTEMPT_STARTED);

				std::cout << "client success" << std::endl;
				g_networkState = NS_Started;
			}
		}
	}
	//std::cout << "press q and then return to exit" << std::endl;
	//std::cin >> userInput;

	InputHandler.join();
	PacketHandler.join();

	return 0;
}
