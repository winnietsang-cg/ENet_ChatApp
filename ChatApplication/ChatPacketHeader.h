#pragma once
#include <string>
#include <sstream>

struct ChatPacket
{
	std::string userName;
	std::string message;
	unsigned char consoleColor = 15; // white (0-255)

	// size of serialized packet
	size_t size() const
	{
		size_t buffer_size = 0;
		buffer_size += userName.length() + 1;
		buffer_size += message.length() + 1;
		buffer_size += sizeof(unsigned char);
		return buffer_size;
	}

	// this is a non member friend function, that has access to ChatPacket's private memebers
	// serialize cp to buffer
	friend size_t operator<<(char* buffer, const ChatPacket& cp)
	{
		size_t buffer_size = 0;
		size_t n = cp.userName.copy(buffer, cp.userName.length());
		buffer[n] = '\0';
		buffer_size += (n + 1);
		buffer += (n + 1);  // advance the buffer pointer

		n = cp.message.copy(buffer, cp.message.length());
		buffer[n] = '\0';
		buffer_size += (n + 1);

		buffer += (n + 1);  // advance the buffer pointer
		*((unsigned char*)buffer) = cp.consoleColor;
		buffer_size += sizeof(unsigned char);

		return buffer_size;
	}

	// this is a non member friend function, that has access to ChatPacket's private memebers
	// deserialize buffer to cp
	friend void operator>>(const char* buffer, ChatPacket& cp)
	{
		cp.userName = std::string{ buffer };
		buffer += cp.userName.length() + 1;
		cp.message = std::string{ buffer };
		buffer += cp.message.length() + 1;
		cp.consoleColor = *((unsigned char*)buffer);
	}

	static void TestSerialize()
	{
		ChatPacket chatPacket;
		chatPacket.userName = "Winnie";
		chatPacket.message = "How is this QA going?";
		
		char* buffer = new char[chatPacket.size()];
		// serialize chatPacket to buffer
		size_t buffersize = buffer << chatPacket;

		// deserialize buffer to chatPacket2
		ChatPacket chatPacket2;
		buffer >> chatPacket2;
		delete[] buffer;
	}
};
