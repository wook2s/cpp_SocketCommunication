
#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <utility>
#include <vector>
#include <string>
#include <cstring>
#include <windows.h>
#include <fstream>
#include <ctime>
#include <time.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#define PORT 4578
#define MAX 1024


void writeFile(std::string& fileName, std::string buffer);
std::string getTime();

static int flag = 1;

class Client
{
public:
	SOCKET cSocket;
	SOCKADDR_IN cAddr = {};
	int sizeCAddr = sizeof(cAddr);
	Client() {}
	int number = -1;
};
typedef std::pair<Client, std::string> clientPair;
std::vector<clientPair> clientVector;


void recvData(SOCKET s, int count)
{
	char buffer[MAX];
	char bufferSend[MAX];

	recv(s, buffer, sizeof(buffer), 0);
	clientVector[count].second = buffer;   //�̸� ����

	while (flag ==1) 
	{
		ZeroMemory(buffer, sizeof(buffer));
		ZeroMemory(bufferSend, sizeof(bufferSend));

		recv(s, buffer, sizeof(buffer), 0);
		
		if (flag != 1)
			break;

		std::cout << clientVector[count].second << " : " << buffer << std::endl;


		//-----------------client �̸��ϰ� ���ļ� ����-------------------
		std::string tmp = clientVector[count].second + " : " + buffer;
		char *c = (char*)tmp.c_str();
		strcpy(bufferSend, c);
		//---------------------------------------------------------------

		//----------------�α� ���� ------------------------------
		std::string file = "communication_Log.txt";
		writeFile(file, bufferSend);

		//------------------------------------------------------
		for (int i = 0; i < clientVector.size(); i++)
		{
			if (count != clientVector[i].first.number)
			{
				send(clientVector[i].first.cSocket, bufferSend, sizeof(bufferSend), 0);
			}
		}

		//------------------------------client ��û���� ���� 
		if (strcmp(buffer, "exit") == 0)
		{
			for (int i = 0; i < clientVector.size(); i++)
			{
				if (count == clientVector[i].first.number)//���� ��û�� client�� ���� ����
				{
					send(clientVector[i].first.cSocket, buffer, sizeof(buffer), 0);
				}
			}

			shutdown(clientVector[count].first.cSocket, SD_BOTH);//���� ��û�� client�� ���� ����
			clientVector.erase(clientVector.begin() + count);
			break;
		}
		//------------------------------client ��û���� ���� 
		
	}
}

int checkClient(std::string &name)
{
	for (int i = 0; i < clientVector.size(); i++)
	{
		if (name.compare(clientVector[i].second)==0)
		{
			return 1;
		}
	}
	return -1;
}

void acceptClient(SOCKET& s)
{
	int count = 0;
	while (flag==1)
	{
		clientVector.push_back(clientPair(Client(), ""));
		clientVector[count].first.cSocket = accept(s, (SOCKADDR*)&clientVector[count].first.cAddr, &clientVector[count].first.sizeCAddr);
		clientVector[count].first.number = count;
		std::thread(recvData, clientVector[count].first.cSocket, count).detach();
		count++;
	}
}

void writeFile(std::string &fileName, std::string buffer) // �α� ����
{
	std::string timeTmp = getTime();
	std::ofstream write(fileName, std::ios::app);
	buffer =timeTmp + +" : " +buffer + '\n';
	if (write.is_open())
	{
		write << buffer;
	}
	else
	{
		std::cout << "file open fail\n";
	}
	write.close();
	return;
}

void readFile(std::string& fileName) //�α� �о����
{
	std::ifstream read(fileName);
	char buf[500];

	while (read) 
	{
		if (read.eof())
		{
			break;
		}
		read.getline(buf, 500);
		std::cout << buf << std::endl;
	}
	read.close();
	
}

std::string getTime() // �α� ���� �� �ð�
{
	time_t currentTime = time(NULL);
	struct tm* timeinfo = localtime(&currentTime);
	int year = timeinfo->tm_year + 1900;
	int month = timeinfo->tm_mon + 1;
	int day = timeinfo->tm_mday;
	int hour = timeinfo->tm_hour;
	int min = timeinfo->tm_min;
	int sec = timeinfo->tm_sec;
	return std::to_string(year) + "/" + std::to_string(month) + "/" + std::to_string(day) + "  " + std::to_string(hour) + ":"+std::to_string(min) + ":"+std::to_string(sec);
}




int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != -1)
	{
		std::cout<<"WinSock ok\n";
	}

	SOCKET serverSocket;
	serverSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (serverSocket != -1)
	{
		std::cout << "serverSock ok\n";
	}
	SOCKADDR_IN serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) != -1)
	{
		std::cout<<"bind ok\n";
	}

	if (listen(serverSocket, SOMAXCONN) != -1)
	{
		std::cout << "lieten ok\n";
	}
	
	std::thread(acceptClient, std::ref(serverSocket)).detach();////////////
	
	char name[MAX];
	char message[MAX];
	char messageTmp[MAX];

	std::string serverFile = "serverLog.txt";
	std::string serverFileTmp = "-----SERVER OPEN TIME-----\n";
	writeFile(serverFile, serverFileTmp);
	serverFileTmp.clear();

	while (true)
	{
		ZeroMemory(name, sizeof(name));
		ZeroMemory(message, sizeof(message));
		std::cout << "1. Client���� �޼��� ������\n";
		std::cout << "2. log Ȯ��\n";
		std::cout << "3. Server ����\n";
		std::string serverInput;
		std::cin >> serverInput;
		int num;
		try
		{
			num = stoi(serverInput);
		}
		catch (std::invalid_argument e)
		{
			std::cout << "�߸� �Է���\n";
			continue;
		}
		if (num < 1 || num >3)
		{
			std::cout << "�߸� �Է��߽��ϴ�\n";
			continue;
		}
		if (num == 1)
		{
			if (clientVector.size() == 0)
			{
				std::cout << "���� ���� client�� �����ϴ�\n";
				continue;
			}
			std::string tmp;
			std::cout << "client�� �Է��Ͻÿ�\n";
			std::cin >> name;
			std::string str(name);
			if (checkClient(str) == -1)
			{
				std::cout << "�ش� client�� �����ϴ�\n";
				continue;
			}
			std::cout << name << " ���� ���� �޼����� �Է��ϼ���\n";
			std::cin.ignore();///////// cin ���� '\n' ����
			std::getline(std::cin, tmp);

			std::cout << "��ٸ��� �� \n";
			tmp = "SERVER : " + tmp;
			strcpy(message, tmp.c_str());
			std::cout << "������ �� \n";
			for (int i = 0; i < clientVector.size(); i++)
			{
				if (strcmp(clientVector[i].second.c_str(), name) == 0)
				{
					send(clientVector[i].first.cSocket, message, sizeof(message), 0);
				}
			}
		}
		else if (num == 2)
		{
			std::string file = "communication_Log.txt";
			readFile(file);
		}
		if (num == 3)
		{
			for (int i = 0; i < clientVector.size(); i++)
			{
				char tmp[]= "SERVER_EXIT";
				strcpy(message, tmp);
				send(clientVector[i].first.cSocket, message, sizeof(message), 0);
			}
			flag = 0;
			for (int i = 0; i < 2000; i++)
			{
				int a = 0;
				a++;
			}
			std::cout << "Server�� �����մϴ�\n";
			break;
		}
	}

	for (int i = 0; i < clientVector.size(); i++)
	{
		shutdown(clientVector[i].first.cSocket, SD_BOTH);
	}
	shutdown(serverSocket, SD_BOTH);
	std::cout << "clientSocket close\n";
	WSACleanup();
	std::cout << "WinSock close\n";

	serverFileTmp = "-----SERVER CLOSE TIME-----\n";
	writeFile(serverFile, serverFileTmp);


}
