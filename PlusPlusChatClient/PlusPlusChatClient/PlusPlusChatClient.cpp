#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>

#pragma warning(disable: 4996)

SOCKET Connection;

enum Packet
{
	P_ChatMessage
};

bool ProcessPacket(Packet packettype)
{
	switch (packettype)
	{
	case P_ChatMessage:
		{
		int msgSize;
		recv(Connection, (char*)&msgSize, sizeof(int), NULL);
		char * msg = new char[msgSize + 1];
		msg[msgSize] = '\0';
		recv(Connection, msg, msgSize, NULL);
		std::cout << msg << std::endl;
		delete[] msg;
		break;
		}
	}
	return true;
}

void ClientHandler()
{
	Packet packettype;
	while (true)
	{
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(packettype)) break;
	}
	closesocket(Connection);
}

int main(int argc, char* argv[])
{

	std::cout << "Enter server IP\n";

	std::string path;
	std::cin >> path;

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);


	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeAddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(path.c_str());
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
	
	std::string msg;
	while (true)
	{
		std::getline(std::cin, msg);
		int msgSize = msg.size();
		Packet packettype = P_ChatMessage;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);
		send(Connection, (char*)&msgSize, sizeof(int), NULL);
		send(Connection, msg.c_str(), msgSize, NULL);
		Sleep(10);
	}

	system("pause");

	return 0;
}