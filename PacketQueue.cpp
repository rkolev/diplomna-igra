#include "PacketQueue.h"

bool PacketQueue::Exists(u32 sequence)
{
	PacketQueue::iterator it = begin();
	for ( ; it != end(); it++) {
		if (it->sequence == sequence) {
			return true;
		}
	}
	return false;
}

void PacketQueue::InsertSorted(const PacketData &p, u32 maxSequence)
{
	//Queue is empty
	if (empty()) {
		push_back(p);
		return;
	}

	//Packet already exists in the queue
	if (Exists(p.sequence) == true)	{
		return;
	}

	//Packet is less recent than the least recent packet in the queue
	if (!SequenceMoreRecent(p.sequence, front().sequence, maxSequence))	{
		push_front(p);
		return;
	}

	//Packet is more recent than the most recent packet in the queue
	if (SequenceMoreRecent(p.sequence, back().sequence, maxSequence) == true) {
		push_back(p);
		return;
	}

	//Insert the packet in the appropriate position
	PacketQueue::iterator it = begin();
	for ( ; it != end(); it++) {				 
		if (SequenceMoreRecent(it->sequence, p.sequence, maxSequence)) {
			insert(it, p);
			return;
		}
	}	
}
