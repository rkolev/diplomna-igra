#pragma once
#include "Connection.h"

class ClientConnection : public Connection
{

public: //methods
	//Default constructor
	//TODO: private?
	ClientConnection();

	//Constructor with parameters
	ClientConnection(u32 protocolId, u32 timeout);

	//Default destructor
	~ClientConnection();

	//Update method - checks for connection timeout
	void Update(s32 timeElapsedMs) override;

	//Connects game's update signal to connection's and reliability system's update slot
	void ConnectSlots(CL_Signal_v1<s32> updateSignal) override;

	//Initiates connection to server @address
	void Connect(const Address& destination);

	//Sends packet containing @data with size @size
	bool SendPacket(const u8 data[], u32 size) override;

	//Receives packet and saves it to @data; returns size of received packet (0 if packet not sent)
	u32 ReceivePacket(u8 data[], u32 size) override;

	//Returns the average round-trip time for packets of this connection
	u32 GetRoundTripTime() { return m_reliabilitySystem.GetRoundTripTime(); }

private: //methods
	//Resets member values to default values
	void ClearData() override;

private: //members
	u32 m_timeoutAccumulator;
	Address m_address;
	ReliabilitySystem m_reliabilitySystem;

}; //class ClientConnection

