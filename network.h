#pragma once
#include "Precomp.h"

namespace Network
{
	//TODO: this should be configurable
	const u8 c_maxConnections = 7;

	struct PacketData
	{
		u32 sequence;	// packet sequence number
		u32 time;		// time offset since packet was sent or received (depending on context)
		u32 size;		// packet size in bytes
	};

	//Initialize sockets
	inline bool InitializeSockets()
	{
		WSADATA WsaData;
		return WSAStartup(MAKEWORD(2, 2), &WsaData) == NO_ERROR;
	}

	//Shutdown sockets
	inline void ShutdownSockets()
	{
		WSACleanup();
	}

	//Compares the two sequence numbers to determine if @s1 is the more recent than @s2
	//Takes into account sequence rollover via the maxSequence 
	inline bool SequenceMoreRecent(u32 s1, u32 s2, u32 maxSequence)
	{		
		return (s1 > s2) && (s1 - s2 <= maxSequence / 2) || (s2 > s1) && (s2 - s1 > maxSequence / 2);
	}
} //namespace Network