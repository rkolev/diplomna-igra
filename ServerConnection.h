#pragma once
#include "Connection.h"

class ServerConnection : public Connection
{
public: //methods
	//Default constructor
	//TODO: private?
	ServerConnection();

	//Constructor with parameters
	ServerConnection(u32 protocolId, u32 timeout);

	//Default destructor
	~ServerConnection();

	//Update method - checks for connection timeout
	void Update(s32 timeElapsedMs) override;

	//Sends packet containing @data with size @size
	bool SendPacket(const u8 data[], u32 size) override;

	//Sends packet containing @data with size @size to client with @clientId
	bool SendToSingleClient(const u8 data[], u32 size, u16 clientId) override;

	//Receives packet and saves it to @data; returns size of received packet (0 if packet not sent)
	u32 ReceivePacket(u8 data[], u32 size) override;

	//Returns the average round-trip time for packets of connection with @clientId
	u32 GetRoundTripTime(u16 clientId) { return m_reliabilitySystems[clientId]->GetRoundTripTime(); }

private: //methods
	//Reset connection to initial values
	void ClearData() override;

private: //members
	u16 m_activeConnections;
	std::vector<Address> m_clients;
	std::vector<u16> m_clientTimeouts;
	std::vector<ReliabilitySystem*> m_reliabilitySystems;
}; //class ServerConnection