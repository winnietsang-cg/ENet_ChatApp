#include "Server.h"
#include "ChatPacketHeader.h"
#include <iostream>
#include <assert.h> 

using namespace std;

bool Server::Start()
{
	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	m_address.host = ENET_HOST_ANY;
	/* Bind the server to port 1234. */
	m_address.port = 1234;
	m_server = enet_host_create(&m_address /* the address to bind the server host to */,
		32      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);

	ChatPacket::TestSerialize();

	if (m_server != nullptr)
	{
		ProcessPackets();
		return true;
	}
	return false;
}

void Server::ProcessPackets()
{
	while (true)
	{
		ENetEvent event;
		/* Wait up to 1000 milliseconds for an event. */
		while (enet_host_service(m_server, &event, 1000) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				{	
					/* Store any relevant client information here. */
					string userInfo = "User_" + std::to_string(m_server->connectedPeers);
					event.peer->data = (char*)userInfo.c_str();
					cout << userInfo << " connected from " << event.peer->address.host << ":" << event.peer->address.port << endl;
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				// no user data, broadcast the packet
				if (event.peer->data == NULL)
				{	
					ChatPacket chatPacket;
					((char*)event.packet->data) >> chatPacket;
					cout << "boardcast: " << chatPacket.userName << ": " << chatPacket.message << endl;
					// Relay the message to all clients
					ENetPacket* packet = enet_packet_create(event.packet->data,
						event.packet->dataLength,
						ENET_PACKET_FLAG_RELIABLE);
					// Send over channel 0 (no particular reason)
					enet_host_broadcast(m_server, 1, packet);
					enet_host_flush(m_server);
					enet_packet_destroy(event.packet);
					enet_host_flush(m_server);
				}
				// send to all connected peers except for the on that sent it
				else
				{
					for (int i = 0; i < m_server->connectedPeers; i++) 
					{	
						ENetPeer* peer = m_server->peers + i;
						if (peer != event.peer) 
						{
							ChatPacket chatPacket;
							((char*)event.packet->data) >> chatPacket;
							cout << "send peer[" << event.peer->connectID << "]:" << chatPacket.userName << ": " << chatPacket.message << endl;
							ENetPacket* packet = enet_packet_create(event.packet->data,
								event.packet->dataLength,
								ENET_PACKET_FLAG_RELIABLE);
							// Send over channel 1 (no particular reason)
							enet_peer_send(peer, 1, packet); 
							enet_host_flush(m_server);
						}
					}
				}
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				cout << (char*)event.peer->data << "disconnected." << endl;
				/* Reset the peer's client information. */
				event.peer->data = NULL;
			}
		}
	}
}

void Server::Terminate()
{
	if (m_server != nullptr)
	{
		enet_host_destroy(m_server);
	}
}
