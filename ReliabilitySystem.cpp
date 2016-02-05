#include "ReliabilitySystem.h"

ReliabilitySystem::ReliabilitySystem(u32 maxSequence)
{
	m_maxSequence = maxSequence;
	Reset();
}

void ReliabilitySystem::Reset()
{
	m_localSequence = 0;
	m_remoteSequence = 0;
	m_sentQueue.clear();
	m_recvQueue.clear();
	m_pendingAckQueue.clear();
	m_ackedQueue.clear();
	m_rtt = 0;
	m_rttMax = 1000;
}

void ReliabilitySystem::PacketSent(u32 size)
{
	//Create an object to store packet metadata
	PacketData data;
	data.sequence = m_localSequence;
	data.time = 0; 
	data.size = size;

	//Add packet to the relevant queues
	m_sentQueue.InsertSorted(data, m_maxSequence);
	m_pendingAckQueue.InsertSorted(data, m_maxSequence);

	//increase local sequence and handle sequence wrap-around
	m_localSequence++;
	if (m_localSequence > m_maxSequence) {
		m_localSequence = 0;
	}
}

void ReliabilitySystem::PacketReceived(u32 sequence, u32 size)
{
	//create an object to store packet metadata
	PacketData data;
	data.sequence = sequence;
	data.time = 0;
	data.size = size;

	//add to received queue
	m_recvQueue.InsertSorted(data, m_maxSequence);

	//update remote sequence
	if (SequenceMoreRecent(sequence, m_remoteSequence, m_maxSequence)) {
		m_remoteSequence = sequence;
	}
}

u32 ReliabilitySystem::GenerateAckBits()
{
	u32 ackBits = 0;
	PacketQueue::const_iterator it = m_recvQueue.begin();
	for ( ; it != m_recvQueue.end(); ++it) {
		if (it->sequence == m_remoteSequence) {
			break;
		}
		u32 bitIndex = GetBitIndex(it->sequence, m_remoteSequence);

		if (bitIndex <= 31)	{
			ackBits |= 1 << bitIndex;
		}
	}
	return ackBits;
}

u32 ReliabilitySystem::GetBitIndex(u32 sequence, u32 ack)
{
	if (sequence > ack) {
		return ack + (m_maxSequence - sequence);
	} else {
		return ack - 1 - sequence;
	}
}

void ReliabilitySystem::ProcessAck(unsigned int ack, unsigned int ackBits)
{
	if (m_pendingAckQueue.empty()) {
		return;
	}

	PacketQueue::iterator it = m_pendingAckQueue.begin();
	for ( ; it != m_pendingAckQueue.end(); ) {
		bool acked = false;

		if (it->sequence == ack) {
			acked = true;			
		} else if (SequenceMoreRecent(it->sequence, ack, m_maxSequence) == false) {
			u32 bitIndex = GetBitIndex(it->sequence, ack);
			if (bitIndex <= 31)	{
				acked = (ackBits >> bitIndex) & 1;
			}
		}

		if (acked) {
			m_rtt += static_cast<int>((it->time - m_rtt) * 0.1f);
			m_ackedQueue.InsertSorted(*it, m_maxSequence);
			m_acks.push_back(it->sequence);
			it = m_pendingAckQueue.erase(it);
		} else {
			++it;
		}
	}	
}

void ReliabilitySystem::Update(s32 timeElapsedMs)
{
	m_acks.clear();
	AdvanceQueueTime(timeElapsedMs);
	UpdateQueues();
}

void ReliabilitySystem::ConnectSlots(CL_Signal_v1<s32> updateSignal)
{
	m_updateSlot = updateSignal.connect(this, &ReliabilitySystem::Update);
}

void ReliabilitySystem::AdvanceQueueTime(s32 timeElapsedMs)
{
	PacketQueue::iterator it;

	it = m_sentQueue.begin();
	for ( ; it != m_sentQueue.end(); it++) {
		it->time += timeElapsedMs;
	}

	it = m_recvQueue.begin();
	for ( ; it != m_recvQueue.end(); it++) {
		it->time += timeElapsedMs;
	}

	it = m_pendingAckQueue.begin();
	for ( ; it != m_pendingAckQueue.end(); it++) {
		it->time += timeElapsedMs;
	}
	
	it = m_ackedQueue.begin();
	for ( ; it != m_ackedQueue.end(); it++) {
		it->time += timeElapsedMs;
	}
}

void ReliabilitySystem::UpdateQueues()
{
	while (m_sentQueue.size() && m_sentQueue.front().time > m_rttMax) {
		m_sentQueue.pop_front();
	}

	if (m_recvQueue.size()) {
		const u32 latestSequence = m_recvQueue.back().sequence;
		const u32 minSequence = latestSequence >= 34 ? (latestSequence - 34) : m_maxSequence - (34 - latestSequence);
		while (m_recvQueue.size() && (SequenceMoreRecent(m_recvQueue.front().sequence, minSequence, m_maxSequence) == false))	{
			m_recvQueue.pop_front();
		}
	}

	while (m_ackedQueue.size() && m_ackedQueue.front().time > m_rttMax) {
		m_ackedQueue.pop_front();
	}

	while (m_pendingAckQueue.size() && m_pendingAckQueue.front().time > m_rttMax) {
		m_pendingAckQueue.pop_front();
	}
}