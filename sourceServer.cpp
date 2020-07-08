#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>

#pragma warning(disable: 4996)

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

	std::cout << "Server loaded, waiting for clients...\n";

	//сокет, по-якому отримаємо з'єднання з клієнтом
	SOCKET newConnection;
	newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeof_addr); //return ptr на новий сокет

	//перевірка з'єднання
	if (newConnection != 0)
		std::cout << "Client connected!" << std::endl;
	else
		std::cout << "Error connection!" << std::endl;

	system("pause");
	return 0;
}
