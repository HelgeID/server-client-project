#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <string>
#include <iostream>

#pragma warning(disable: 4996)

SOCKET newConnection;
enum Packet { P_ChatMSG, P_Test };

bool CloseClient()
{
	if (!recv(newConnection, NULL, 0, 0))
		return false;
	return true;
}

bool SentInt(int _int) {
	int checkReturn(send(newConnection, (char*)&_int, sizeof(int), NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool RecvInt(int& _int) {
	int checkReturn(recv(newConnection, (char*)&_int, sizeof(int), NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool SentPacketType(Packet packet) {
	int checkReturn(send(newConnection, (char*)&packet, sizeof(Packet), NULL));
	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool RecvPacketType(Packet& packet) {
	int checkReturn(recv(newConnection, (char*)&packet, sizeof(Packet), NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool SentStrig(std::string _string) {
	if (!SentPacketType(P_ChatMSG))
		return false;

	int lenght = _string.size();
	if (!SentInt(lenght))
		return false;

	int checkReturn(send(newConnection, _string.c_str(), lenght, NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool RecvString(std::string& _string) {
	int lenght;
	if (!RecvInt(lenght))
		return false;

	char* msg = new char[lenght + 1];
	msg[lenght] = '\0';
	int checkReturn(recv(newConnection, msg, lenght, NULL));
	_string = msg;
	delete[] msg;

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool ProcessPacket(Packet ptype)
{
	switch (ptype)
	{
	case P_ChatMSG:
	{
		std::string msg("");
		if (!RecvString(msg))
			return false;
		std::cout << ">>" << msg << std::endl;
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

void ClientMSG()
{
	Packet packet;
	while (!CloseClient()) {
		if (!RecvPacketType(packet))
			break;

		if (!ProcessPacket(packet))
			break;
	}

	std::cout << "Disconnected to server!" << std::endl;
	closesocket(newConnection);
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
	newConnection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(newConnection, (SOCKADDR*)&addr, sizeof_addr) != 0)
		std::cout << "Error connection!" << std::endl;
	else {
		std::cout << "Connected to server!" << std::endl; //connect() return 0

		//перевірка з'єднання
		if (newConnection != 0) {
			//опрацьовуємо повідомлення з сервера (отриманих від клієнтів)
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientMSG, (LPVOID)(newConnection), NULL, NULL);

			//відправляємо повідомлення на сервер
			std::string msg("");
			while (true) {
				std::getline(std::cin, msg);
				if (!SentStrig(msg))
					break;
				Sleep(10);
			}
		}
	}

	system("pause");
	return 0;
}
