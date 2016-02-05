#pragma once

#include "PacketQueue.h"

class ReliabilitySystem
{
public: //methods
	//Default constructor
	ReliabilitySystem(u32 maxSequence = 0xFFFFFFFF);

	//Default destructor
	~ReliabilitySystem() { }

	//Reset reliability system
	void Reset();

	//Process packet sent event
	void PacketSent(u32 size);

	//Process packet received event
	void PacketReceived(u32 sequence, u32 size);

	//Update reliability system
	void Update(s32 timeElapsedMs);

	//Connect update signal to update slots
	void ConnectSlots(CL_Signal_v1<s32> updateSignal);

	//Process packet ack
	void ProcessAck(u32 ack, u32 ackBits);
	
	//Get packet's bit index based on ack
	u32 GetBitIndex(u32 sequence, u32 ack);

	//Generate next packet ack
	u32 GenerateAckBits();	

	//Get local sequence
	u32 GetLocalSequence() const { return m_localSequence; }
	
	//Get remote sequence
	u32 GetRemoteSequence() const { return m_remoteSequence; }

	//Get max sequence
	u32 GetMaxSequence() const { return m_maxSequence; }

	//Get connection's round-trip time
	u32 GetRoundTripTime() const { return m_rtt; }

private: //methods
	//Advance queue times based on @timeElapsedMs
	void AdvanceQueueTime(s32 timeElapsedMs);

	//Update queues
	void UpdateQueues();

private: //members
	u32 m_maxSequence;
	u32 m_localSequence;
	u32 m_remoteSequence;
	u32 m_rtt;
	u32 m_rttMax;

	std::vector<u32> m_acks;

	PacketQueue m_sentQueue;
	PacketQueue m_pendingAckQueue;
	PacketQueue m_recvQueue;
	PacketQueue m_ackedQueue;

	CL_Slot m_updateSlot;
}; //class ReliabilitySystem