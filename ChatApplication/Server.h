#pragma once
#include <enet/enet.h>
#include <string>

using namespace std;

class Server
{
public:
	bool Start();
	void ProcessPackets();
	void Terminate();

private:
	ENetAddress m_address;
	ENetHost* m_server = nullptr;
};

