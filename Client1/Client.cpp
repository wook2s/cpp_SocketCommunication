#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <windows.h>
#include <cstdlib>
#include <string> 

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")
#define PORT 4578
#define MAX 1024
#define SERVER_IP "192.168.0.53" 

static int flag = 1;

void recvData(SOCKET& s)
{
	char buffer[MAX];
	while (flag==1)
	{
		ZeroMemory(buffer, sizeof(buffer));
		recv(s, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;

		if (strcmp(buffer, "exit") == 0)
		{
			break;
		}
		if (strcmp(buffer, "SERVER_EXIT") == 0)
		{
			std::cout << "서버가 종료되었습니다. 엔터키를 누르면 종료됩니다.";
			flag = 0;
		}
	}
	ZeroMemory(buffer, sizeof(buffer));
}

int main()
{
	char name[MAX] = { 0 };

	std::cout << "이름을 입력하세요 : ";
	std::cin >> name;


	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != -1)
	{
		std::cout << "WinSock ok\n";
	}
	else
		flag = 0;
	SOCKET clientSocket;
	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket != -1)
	{
		std::cout << "clientSock ok\n";
	}
	else
		flag = 0;
	SOCKADDR_IN serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);


	if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) != -1)
	{
		std::cout << "connect ok\n";
	}
	else
		flag = 0;


	send(clientSocket, name, sizeof(name), 0);
	std::thread(recvData, std::ref(clientSocket)).detach();

	std::cout << "채팅을 시작합니다 \n";
	std::cout << "종료 : exit \n";
	while (flag ==1)
	{
		ZeroMemory(name, sizeof(name));
		std::string tmp;
		std::getline(std::cin, tmp);

		if (strlen(tmp.c_str()) == 0)
		{
			continue;
		}

		strcpy(name, tmp.c_str());
		send(clientSocket, name, sizeof(name), 0);
		//----------
		if (strcmp(tmp.c_str(), "exit") == 0)
		{
			break;
		}
		//-----------
		tmp = {};
	}
	ZeroMemory(name, sizeof(name));

	shutdown(clientSocket, SD_SEND); //스레드 정상종료를 위해 보내기만 종료 
	std::cout << "소켓 닫힘\n";
	WSACleanup();
}
