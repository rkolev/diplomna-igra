#include "ServerConnection.h"

ServerConnection::ServerConnection() : Connection()
{

}

ServerConnection::ServerConnection(u32 protocolId, u32 timeout) : Connection(protocolId, timeout)
{
	m_mode = eServer;
	m_state = eListening;
}

ServerConnection::~ServerConnection()
{
	if (m_reliabilitySystems.size() > 0) {
		//Cleanup reliability systems
		std::vector<ReliabilitySystem*>::iterator it = m_reliabilitySystems.begin();
		for ( ; it != m_reliabilitySystems.end(); ++it)	{
			delete (*it);
		}
	}
}

void ServerConnection::Update(s32 timeElapsedMs)
{
	if (IsRunning() == false) {
		//Connection is not running
		//TODO: log
		return;
	}

	//Update timeouts for each connection
	std::vector<u16>::iterator it = m_clientTimeouts.begin();
	for ( ; it != m_clientTimeouts.end(); )	{
		*it += timeElapsedMs;
		if (*it > m_timeout) {
			//If any client's timeout exceeds the maximum timeout, drop the connection to it
			//TODO: optionally send a message to client informing of disconnect
			m_activeConnections--;
			if (m_activeConnections == 0) {
				//If all connections are dropped, simply clear all stored data
				ClearData();
				break;
			}
			//delete the address entry from the clients list,
			//delete the timeout entry from the timeouts list,
			//delete the associated Reliability System
			//and invoke the connection drop signal
			u8 index = it - m_clientTimeouts.begin();
			m_clients.erase(m_clients.begin() + index);
			m_reliabilitySystems.erase(m_reliabilitySystems.begin() + index);
			it = m_clientTimeouts.erase(it);
			m_connectionDropSignal.invoke(index);		

			if (m_state == eFull) {
				m_state = eListening;
			}			
		}
		else ++it;
	}	
}

bool ServerConnection::SendPacket(const u8 data[], u32 size)
{
	if (IsRunning() == false) {
		//Connection is not running
		//TODO: log
		return false;
	}
	if (m_clients.size() <= 0) {
		//There are no connected clients
		//TODO: log
		return false;
	}

	bool success = true;

	//Send the message to each connected client
	for (u32 i = 0; i < m_clients.size(); i++) {
		u8* packet = new u8[size + ms_headerSize];
		u32 sequence = m_reliabilitySystems[i]->GetLocalSequence();
		u32 ack = m_reliabilitySystems[i]->GetRemoteSequence();
		u32 ackBits = m_reliabilitySystems[i]->GenerateAckBits();

		WriteHeader(packet, sequence, ack, ackBits);
		memcpy(packet + ms_headerSize, data, size);

		if (m_socket.Send(m_clients[i], packet, size + ms_headerSize)) {
			m_reliabilitySystems[i]->PacketSent(size);
		} else {
			success = false;
		}
		delete[] packet;
	}

	return success;
}

bool ServerConnection::SendToSingleClient(const u8 data[], u32 size, u16 clientId)
{
	if (IsRunning() == false) {
		//Connection is not running
		//TODO: log
		return false;
	}

	if (m_clients.size() <= 0) {
		//There are no connected clients
		//TODO: log
		return false;
	}

	bool success = false;

	//Construct header
	u8* packet = new u8[size + ms_headerSize];
	u32 sequence = m_reliabilitySystems[clientId]->GetLocalSequence();
	u32 ack = m_reliabilitySystems[clientId]->GetRemoteSequence();
	u32 ackBits = m_reliabilitySystems[clientId]->GenerateAckBits();

	WriteHeader(packet, sequence, ack, ackBits);
	memcpy(packet + ms_headerSize, data, size);

	success = m_socket.Send(m_clients[clientId], packet, size + ms_headerSize);
	if (success) {
		m_reliabilitySystems[clientId]->PacketSent(size);
	}

	delete[] packet;	
	return success;
}

u32 ServerConnection::ReceivePacket(u8 data[], u32 size)
{	
	if (IsRunning() == false) {
		return 0;
	}
	
	u8* packet = new u8[size + ms_headerSize];
	Address sender;
	u32 bytes_read = m_socket.Receive(sender, packet, size + ms_headerSize);
	if (bytes_read <= ms_headerSize) {
		//packet too small
		delete[] packet;
		return 0;
	}

	if (packet[0] != (u8)(m_protocolId >> 24) ||
		packet[1] != (u8)((m_protocolId >> 16) & 0xFF) ||
		packet[2] != (u8)((m_protocolId >> 8) & 0xFF) ||
		packet[3] != (u8)(m_protocolId & 0xFF)) {
		//Sender's protocol ID doesn't match the expected protocol ID
		//TODO: log
		delete[] packet;
		return 0;
	}	

	u32 packetSequence = 0;
	u32 packetAck = 0;
	u32 packetAckBits = 0;

	//Check if the sender is one of the currently connected clients
	std::vector<Address>::iterator it = m_clients.begin();
	for ( ; it != m_clients.end(); ++it) {
		if (sender == *it) {
			//If we find the sender in the clients list, reset its respective timeout
			//and return the packet
			u16 index = it - m_clients.begin();
			m_clientTimeouts[index] = 0;
			ReadHeader(packet + 4, packetSequence, packetAck, packetAckBits);
			m_reliabilitySystems[index]->PacketReceived(packetSequence, bytes_read - ms_headerSize);
			m_reliabilitySystems[index]->ProcessAck(packetAck, packetAckBits);
			memcpy(data, &packet[ms_headerSize], bytes_read - ms_headerSize);
			delete[] packet;
			return bytes_read - ms_headerSize;
		}
	}

	if (m_state == eFull) {
		//If the server is at capacity, decline connection
		//TODO: log
		delete[] packet;
		return 0;
	}

	//Add a new entry into clients and clientTimeouts and invoke a new connection signal,
	//which will execute a method in the Game class, that will handle the new client
	m_clients.push_back(sender);
	m_clientTimeouts.push_back(0);
	m_reliabilitySystems.push_back(new ReliabilitySystem());
	m_reliabilitySystems.back()->ConnectSlots(m_updateSignal);

	ReadHeader(packet + 4, packetSequence, packetAck, packetAckBits);
	m_reliabilitySystems.back()->PacketReceived(packetSequence, bytes_read - 16);
	m_activeConnections++;
	memcpy(data, &packet[ms_headerSize], bytes_read - ms_headerSize);

	if (m_activeConnections == c_maxConnections) {
		m_state = eFull;
	}
	delete[] packet;
	return bytes_read - ms_headerSize;
}

void ServerConnection::ClearData()
{
	m_state = eListening;
	m_clients.clear();
	m_clientTimeouts.clear();
	m_reliabilitySystems.clear();
	m_activeConnections = 0;
}