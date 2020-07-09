﻿#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>

#pragma warning(disable: 4996)

SOCKET arrConnections[100]{ 0 }; //масив сокетів
int counter = 0; //індекс з'єднання

void ClientHandler(int index)
{
	//чекаємо від'єднання клієнта
	if (recv(arrConnections[index], NULL, 0, 0) != 0)
		std::cout << "Client " << index << " disconnected, ";
	counter == 0 ? counter : counter--;
	std::cout << "counter: " << counter << std::endl;
	arrConnections[index] = 0;
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
					break;
				}
			}

		}

	}

	system("pause");
	return 0;
}
