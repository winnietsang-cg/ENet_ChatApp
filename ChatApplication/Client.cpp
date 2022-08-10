#include "Client.h"
#include <iostream>

using namespace std;

bool Client::Connect()
{
	m_client = enet_host_create(NULL /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		0 /* assume any amount of incoming bandwidth */,
		0 /* assume any amount of outgoing bandwidth */);

	if (m_client != nullptr)
	{
		cout << "1) Enter User Name: ";
		cin >> m_UserName;

		ENetAddress address;
		ENetEvent event;
		/* Connect to some.server.net:1234. */
		enet_address_set_host(&address, "127.0.0.1");
		address.port = 1234;
		/* Initiate the connection, allocating the two channels 0 and 1. */
		m_peer = enet_host_connect(m_client, &address, 2, 0);
		if (m_peer == nullptr)
		{
			cout << "No available peers for initiating an ENet connection" << endl;
			return false;
		}
		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(m_client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			cout << "Connection to 127.0.0.1:1234 succeeded." << endl;
			return true;
		}
		else
		{
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(m_peer);
			cout << "Connection to 127.0.0.1:1234 failed." << endl;
		}
	}
	return false;
}

void Client::Start()
{
	cout << "Start Chat (Type \"Q\" to quit)" << endl;
	m_UserInputThread = std::thread(&Client::GetUserMessage, this);
	m_PacketThread = std::thread(&Client::ProcessPackets, this);

	while (!m_quit)
	{
		{
			// output recieved message
			std::lock_guard<std::mutex> lock(m_ReceiveMessageMutex);
			while(!m_ReceiveMessages.empty())
			{
				cout << endl << m_ReceiveMessages.front() << endl;
				cout << ">"; // prompt for input
				m_ReceiveMessages.pop();
			}
		}
		{
			// send queued up message to host
			std::lock_guard<std::mutex> lock(m_UserMessageMutex);
			while (!m_UserMessage.empty())
			{
				auto& m = m_UserMessage.front();
				if (m.length() > 0)
				{
					string userMessage = m_UserName + ": " + m;
					ENetPacket* packet = enet_packet_create(userMessage.c_str(),
						userMessage.length() + 1,
						ENET_PACKET_FLAG_RELIABLE);
					enet_host_broadcast(m_client, 0, packet);
					enet_host_flush(m_client);
				}
				m_UserMessage.pop();
			}
		}
	}}

void Client::ProcessPackets()
{
	while (!m_quit)
	{
		ENetEvent event;
		/* Wait up to 1000 milliseconds for an event. */
		while (enet_host_service(m_client, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				{
					// cout << "receive[" << event.peer->connectID << "]:" << event.packet->data << endl;
					std::lock_guard<std::mutex> lock(m_ReceiveMessageMutex);
					std::string message;
					message = (char*)event.packet->data;
					m_ReceiveMessages.push(message);					
					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
				}
			}
		}
	}
}

void Client::GetUserMessage()
{
	while (!m_quit)
	{	
		std::string input;
		cout << ">"; // prompt for input
		std::getline(std::cin, input);
		if (input == "quit")
		{ 
			m_quit=true;
			return;
		}
		std::lock_guard<std::mutex> lock(m_UserMessageMutex);
		m_UserMessage.push(input);
	}
}


void Client::Terminate()
{
	m_UserInputThread.join();
	m_PacketThread.join();
	if (m_client != nullptr)
	{
		enet_host_destroy(m_client);
	}
}
