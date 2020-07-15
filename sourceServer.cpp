#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>

#pragma warning(disable: 4996)

SOCKET arrConnections[100]{ 0 }; //масив сокетів
int counter = 0; //індекс з'єднання

enum Packet { P_ChatMSG, P_Test };

bool CloseClient(int index)
{
	if (!recv(arrConnections[index], NULL, 0, 0))
		return false;
	return true;
}

bool SentInt(int index, int _int) {
	int checkReturn(send(arrConnections[index], (char*)&_int, sizeof(int), NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool RecvInt(int index, int& _int) {
	int checkReturn(recv(arrConnections[index], (char*)&_int, sizeof(int), NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool SentPacketType(int index, Packet packet) {
	int checkReturn(send(arrConnections[index], (char*)&packet, sizeof(Packet), NULL));
	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool RecvPacketType(int index, Packet& packet) {
	int checkReturn(recv(arrConnections[index], (char*)&packet, sizeof(Packet), NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool SentStrig(int index, std::string _string) {
	if (!SentPacketType(index, P_ChatMSG))
		return false;

	int lenght = _string.size();
	if (!SentInt(index, lenght))
		return false;

	int checkReturn(send(arrConnections[index], _string.c_str(), lenght, NULL));

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool RecvString(int index, std::string& _string) {
	int lenght;
	if (!RecvInt(index, lenght))
		return false;

	char* msg = new char[lenght + 1];
	msg[lenght] = '\0';
	int checkReturn(recv(arrConnections[index], msg, lenght, NULL));
	_string = msg;
	delete[] msg;

	if (checkReturn == SOCKET_ERROR)
		return false;
	return true;
}

bool ProcessPacket(int index, Packet packet)
{
	switch (packet)
	{
	case P_ChatMSG:
	{
		std::string msg("");
		if (!RecvString(index, msg))
			return false;

		for (int i(0); i < 100; ++i) {
			if (i == index || !arrConnections[i])
				continue; //skip

			if (!SentStrig(i, msg))
				std::cout << "Error sent msg!" << std::endl;
		}
		break;
	}
	case P_Test:
		std::cout << "You have received the test packet!" << std::endl;
		break;
	default:
		std::cout << "Unknown packet: " << packet << std::endl;
	}
	return true;
}

void ClientHandler(int index)
{
	Packet packet;
	while (!CloseClient(index)) {
		if (!RecvPacketType(index, packet))
			break;

		if (!ProcessPacket(index, packet))
			break;
	}

	closesocket(arrConnections[index]);
	counter == 0 ? counter : counter--;
	arrConnections[index] = 0;
	std::cout << "Client " << index << " disconnected, counter: " << counter << std::endl;
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

	//створюємо сокет
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	//прив'язка сокета
	bind(sListen, (SOCKADDR*)&addr, sizeof_addr);
	//прослушка по порту sListen, SOMAXCONN - максимальне число запитів
	listen(sListen, SOMAXCONN);

	std::cout << "Server loaded, waiting for Clients...\n";

	//сокет, по-якому отримаємо з'єднання з клієнтом
	SOCKET newConnection;

	while (true) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeof_addr);

		if (newConnection != 0) { //перевірка з'єднання
			for (int i(0); i < 100; ++i) { //надаємо вільний id
				if (arrConnections[i] == 0) {
					arrConnections[i] = newConnection;
					counter++; //загальне число клієнтів
					std::cout << "new Client connected!" << std::endl;
					CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);

					//відправка тестового пакета
					SentPacketType(i, P_Test);

					//відправка повідомлення клієнту
					std::string msg("Welcome our chat!");
					SentStrig(i, msg);
					break;
				}
			}

		}

	}

	system("pause");
	return 0;
}
