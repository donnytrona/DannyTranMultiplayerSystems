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

char* name;

RakNet::RakPeerInterface *g_rakPeerInterface = nullptr;
unsigned char GetPacketIdentifier(RakNet::Packet *p);


enum NetworkState {
	NS_Init,
	NS_PendingStart,
	NS_Started,
	NS_Chat
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

void SendChat() {
	while (isRunning) {
		char userInput[2048];
		std::cout << "Enter message: " << std::endl;
		std::cin >> userInput;

		// message is the data to send
		// strlen(message)+1 is to send the null terminator
		// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
		// RELIABLE_ORDERED means make sure the message arrives in the right order
		char message2[2048];
		// Append Server: to the message so clients know that it ORIGINATED from the server
		// All messages to all clients come from the server either directly or by being
		// relayed from other clients
		message2[0] = 0;
		strncpy_s(message2, name, sizeof(message2));
		strncat_s(message2, userInput, sizeof(message2) - strlen(name) - 1);
		g_rakPeerInterface->Send(message2, (int)strlen(message2) + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
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
				if(!isServer)
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

			char userInput[255];
			if (isServer)
			{
				RakNet::SocketDescriptor socketDescriptors[1];
				socketDescriptors[0].port = SERVER_PORT;
				socketDescriptors[0].socketFamily = AF_INET; // Test out IPV4

				bool isSuccess = g_rakPeerInterface->Startup(MAX_CONNECTIONS, socketDescriptors, 1) == RakNet::RAKNET_STARTED;
				assert(isSuccess);

				std::cout << "Please input name" << std::endl;
				std::cin >> userInput;
				name = userInput;

				g_rakPeerInterface->SetMaximumIncomingConnections(MAX_CONNECTIONS);

				std::cout << "server started" << std::endl;
				g_networkState = NS_Chat;
			}
			else
			{
				RakNet::SocketDescriptor socketDescriptor(CLIENT_PORT, 0);
				socketDescriptor.socketFamily = AF_INET;
				bool isSuccess = g_rakPeerInterface->Startup(8, &socketDescriptor, 1) == RakNet::RAKNET_STARTED;
				assert(isSuccess);

				std::cout << "Please input name" << std::endl;
				std::cin >> userInput;
				name = userInput;

				g_rakPeerInterface->SetOccasionalPing(true);

				RakNet::ConnectionAttemptResult result = g_rakPeerInterface->Connect("127.0.0.1", SERVER_PORT, 0, 0);
				RakAssert(result == RakNet::CONNECTION_ATTEMPT_STARTED);

				std::cout << "client success" << std::endl;
				g_networkState = NS_Chat;
			}
		}
		if (g_networkState == NS_Chat) {
			//Send Handling
			std::thread ChatHandler(SendChat);
			ChatHandler.join();
		}
	}
	//std::cout << "press q and then return to exit" << std::endl;
	//std::cin >> userInput;

	InputHandler.join();
	PacketHandler.join();


	return 0;
}
