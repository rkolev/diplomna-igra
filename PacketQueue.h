#pragma once

#include "Network.h"

using namespace Network;

//Class for storing sent or received packets with added easy check to see
//if a packet already exists here and a sorted insert
class PacketQueue : public std::list<PacketData>
{
public: //methods
	//Checks if a packet with the provided sequence number exists in the queue
	bool Exists(u32 sequence);

	//Sorted insert into the queue
	void InsertSorted(const PacketData &p, u32 maxSequence);
}; //class PacketQueue