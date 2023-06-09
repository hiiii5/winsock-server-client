#include <iostream>
#include <sstream>
#include <string>

#include "Client.h"
#include "Server.h"

int main()
{
	int choice;
	std::string str;
	do
	{
		printf("Would you like to Create a Server or Client?\n");
		printf("1> Server\n");
		printf("2> Client\n");
		std::getline(std::cin, str);
		choice = atoi(str.c_str());
	} while (choice != 1 && choice != 2);

	//Server
	if (choice == 1)
	{
		Server s{};
		s.Init(nullptr, 31337);
	}

	//Client
	if (choice == 2)
	{
		Client c{};
		printf("What is the ip? (default=127.0.0.1)\t");
		std::string ip;
		std::getline(std::cin, ip);
		printf("\n");

		printf("What is the port? (default=31337)\t");
		std::string portStr;
		std::getline(std::cin, portStr);
		printf("\n");

		// Validate the port
		uint16_t port = atoi(portStr.c_str());
		
		while (port == 0)
		{
			printf("Invalid port!\n");

			printf("What is the port? (default=31337)\t");
			std::getline(std::cin, portStr);
			printf("\n");
			
			port = atoi(portStr.c_str());
		}
		
		c.Init(ip.c_str(), port);
	}

	return 0;
}
