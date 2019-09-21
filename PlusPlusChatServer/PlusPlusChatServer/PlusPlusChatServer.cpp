#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>

#pragma warning(disable: 4996)

int clientCounter = 0;
SOCKET Connections[32];

enum Packet
{
	P_ChatMessage
};

bool ProcessPacket(int index, Packet ppackettype)
{
	switch (ppackettype)
	{
	case P_ChatMessage:
	{
		break;
	}
	default:
		std::cout << "Unrecognized packet: " << packettype << std::endl;
	}
}

void ClientHandler(int index)
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
			send(Connections[i], (char*)&msgSize, sizeof(int), NULL);
			send(Connections[i], msg, msgSize, NULL);
		}
		delete[]msg;
	}

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

	//int inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeAddr);

	for (int i = 0; i < 32; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeAddr);
		if (newConnection == 0)
		{
			std::cout << "Error #2\n";
		}
		else
		{
			std::cout << "Client " << i << " connected! \n";
			std::string msg = "Hello Network";
			int msgSize = msg.size();
			send(newConnection, (char*)&msgSize, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msgSize, NULL);

			Connections[i] = newConnection;
			clientCounter++;
			//создаем поток для каждого клиента
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}
	system("pause");
	return 0;
}