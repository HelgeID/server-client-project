#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>

#pragma warning(disable: 4996)

void ClientMSG(SOCKET newConnection)
{
	char msg[256];
	while (!recv(newConnection, NULL, 0, 0)) {
		recv(newConnection, msg, sizeof(msg), NULL);
		std::cout << ">>" << msg << std::endl;
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
			char msg[256];
			while (true) {
				std::cin.getline(msg, sizeof(msg));
				send(newConnection, msg, sizeof(msg), NULL);
				Sleep(10);
			}
		}
	}

	system("pause");
	return 0;
}
