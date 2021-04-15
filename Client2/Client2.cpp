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

static int flag = 1; // 1�̸� ������, 0�̸� while�� Ż��, ����.

void recvData(SOCKET& s)
{
	char buffer[MAX];
	while (flag == 1)
	{
		ZeroMemory(buffer, sizeof(buffer)); 
		recv(s, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;

		if (strcmp(buffer, "exit") == 0)//Ŭ���̾�Ʈ ���� ��û
		{
			break;
		}
		if (strcmp(buffer, "SERVER_EXIT") == 0)//���� ������û
		{
			std::cout << "������ ����Ǿ����ϴ�. ����Ű�� ������ ����˴ϴ�.";
			flag = 0;
		}
	}
	ZeroMemory(buffer, sizeof(buffer));
}

int main()
{
	char name[MAX] = { 0 };

	std::cout << "�̸��� �Է��ϼ��� : ";
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
	std::thread(recvData, std::ref(clientSocket)).detach();//�ޱ� ��� ������ ����

	std::cout << "ä���� �����մϴ� \n";
	std::cout << "���� : exit \n";
	while (flag == 1)
	{
		ZeroMemory(name, sizeof(name));
		std::string tmp;
		std::getline(std::cin, tmp);

		if (strlen(tmp.c_str()) == 0)//������ ���ڿ��� ������ continue
		{
			continue;
		}

		strcpy(name, tmp.c_str());
		send(clientSocket, name, sizeof(name), 0);
		//----------
		if (strcmp(tmp.c_str(), "exit") == 0) // ������ ������û
		{
			break;
		}
		//-----------
		tmp = {};
	}
	ZeroMemory(name, sizeof(name));

	shutdown(clientSocket, SD_SEND); //������ �������Ḧ ���� �����⸸ ���� 
	std::cout << "���� ����\n";
	WSACleanup();
}
