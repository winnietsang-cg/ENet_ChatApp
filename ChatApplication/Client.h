#pragma once
#include <enet/enet.h>
#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include "ChatPacketHeader.h"

using namespace std;

class Client
{
public:
	bool Connect();
	void Start();
	void Terminate();
	void GetUserMessage();
	void ProcessPackets();

private:
	ENetHost* m_client = nullptr;
	ENetPeer* m_peer = nullptr;
	unsigned char m_UserColor; 
	std::string m_UserName;
	std::queue<std::string> m_UserMessage;
	std::queue<ChatPacket> m_ReceiveMessages;
	std::thread m_UserInputThread;
	std::thread m_PacketThread;

	std::mutex m_UserMessageMutex;
	std::mutex m_ReceiveMessageMutex;
	std::atomic<bool> m_quit = false;

};

