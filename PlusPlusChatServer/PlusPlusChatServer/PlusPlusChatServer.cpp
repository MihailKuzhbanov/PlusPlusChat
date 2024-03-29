#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>

#pragma warning(disable: 4996)

int clientCounter = 0;
SOCKET Connections[8];

enum Packet
{
	P_ChatMessage,
	P_Test
};

bool ProcessPacket(int index, Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int msgSize;
		while (true)
		{
			recv(Connections[index], (char*)&msgSize, sizeof(int), NULL);
			char *msg = new char[msgSize + 1];
			msg[msgSize] = '\0';
			recv(Connections[index], msg, msgSize, NULL);
			for (int i = 0; i < clientCounter; i++)
			{
				if (i == index) continue;
				Packet msgtype = P_ChatMessage;
				send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
				send(Connections[i], (char*)&msgSize, sizeof(int), NULL);
				send(Connections[i], msg, msgSize, NULL);
			}
			delete[]msg;
		}

		break;
	}
	case P_Test:
	{
		std::cout << "Test packet.\n";
		break;
	}
	default:
		std::cout << "Unrecognized packet: " << packettype << std::endl;
		break;
	}

	return true;
}

void ClientHandler(int index)
{
	Packet packettype;
	while (true)
	{
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
		if (!ProcessPacket(index, packettype))
		{
			break;
		}
	}
	closesocket(Connections[index]);
}

int main(int argc, char* argv[])
{
	
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	

	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeAddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeAddr);

	for (int i = 0; i < 32; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeAddr);
		if (newConnection == 0)
		{
			std::cout << "Failed to accept the client's connection.\n";
		}
		else
		{
			std::cout << "Client " << i << " connected! \n";
			std::string msg = "Hello Network";
			int msgSize = msg.size();
			Packet msgtype = P_ChatMessage;
			send(newConnection, (char*)&msgtype, sizeof(Packet), NULL);
			send(newConnection, (char*)&msgSize, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msgSize, NULL);
			Connections[i] = newConnection;
			clientCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);

			Packet testpacket = P_Test;
			send(newConnection, (char*)&testpacket, sizeof(Packet), NULL);
		}
	}
	system("pause");
	return 0;
}