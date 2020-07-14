#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <string>
#include <iostream>

#pragma warning(disable: 4996)

enum Packet
{
	P_ChatMSG,
	P_Test
};

bool ProcessPacket(Packet ptype, const SOCKET& newConnection)
{

	switch (ptype)
	{
	case P_ChatMSG:
	{
		int lenght;
		recv(newConnection, (char*)&lenght, sizeof(int), NULL); //get lenght
		char* msg = new char[lenght + 1]; //get msg
		msg[lenght] = '\0';
		recv(newConnection, msg, lenght, NULL);
		std::cout << ">>" << msg << std::endl;
		delete[] msg;
		break;
	}
	case P_Test:
		std::cout << "You have received the test packet!" << std::endl;
		break;
	default:
		std::cout << "Unknown packet: " << ptype << std::endl;
	}
	return true;
}

void ClientMSG(const SOCKET newConnection)
{
	Packet ptype;
	while (!recv(newConnection, NULL, 0, 0)) {
		//get the packet type
		recv(newConnection, (char*)&ptype, sizeof(Packet), NULL);
		if (!ProcessPacket(ptype, newConnection))
			break;
	}
	std::cout << "Disconnected to server!" << std::endl;
	return;
}

int main()
{
	//загружаємо бібліотеку
	WSAData wsaData;
	WORD dllVer = MAKEWORD(2, 1);
	if (WSAStartup(dllVer, &wsaData)) {
		std::cout << "Error!" << std::endl;
		exit(1);
	}

	//заповняємо інфу про адрес сокета
	SOCKADDR_IN addr;
	int sizeof_addr(sizeof(addr)); //розмір структури адреси
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //error 4996
	addr.sin_port = htons(1111); //порт
	addr.sin_family = AF_INET; //сімейство і-нет протокола

	std::cout << "Client loaded!\n";

	//створюємо сокет
	SOCKET newConnection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(newConnection, (SOCKADDR*)&addr, sizeof_addr) != 0)
		std::cout << "Error connection!" << std::endl;
	else {
		std::cout << "Connected to server!" << std::endl; //connect return 0

		//перевірка з'єднання
		if (newConnection != 0) {
			//опрацьовуємо повідомлення з сервера (отриманих від клієнтів)
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientMSG, (LPVOID)(newConnection), NULL, NULL);

			//відправляємо повідомлення на сервер
			std::string msg("");
			while (true) {
				std::getline(std::cin, msg);
				int lenght = msg.size();

				Packet packet = P_ChatMSG;
				send(newConnection, (char*)&packet, sizeof(Packet), NULL);

				send(newConnection, (char*)&lenght, sizeof(int), NULL);
				send(newConnection, msg.c_str(), lenght, NULL);
				Sleep(10);
			}
		}
	}

	system("pause");
	return 0;
}
