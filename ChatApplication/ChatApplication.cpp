#include <iostream>
#include "Server.h"
#include "Client.h"

using namespace std;

int main(int argc, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		cout << "An error occurred while initializing ENet." << endl;
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
	
	cout << "1) Create Server " << endl;
	cout << "2) Create Client " << endl;
	int UserInput;
	cin >> UserInput;
	if (UserInput == 1)
	{
		Server server;
		server.Start();
		server.Terminate();
	}
	else if (UserInput == 2)
	{
		Client client;
		if (client.Connect())
		{
			client.Start();
		}
		client.Terminate();
	}
	else
	{
		cout << "Invalid Input" << endl;
	}
	return EXIT_SUCCESS;
}