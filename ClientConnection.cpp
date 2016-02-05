#include "ClientConnection.h"

//Default constructor
//TODO: private?
ClientConnection::ClientConnection()
{ 

}

//Constructor with parameters
ClientConnection::ClientConnection(u32 protocolId, u32 timeout) : Connection(protocolId, timeout)
{
	m_mode = eClient;
	m_state = eDisconnected;
}

//Default destructor
ClientConnection::~ClientConnection()
{

}

//Update method - checks for connection timeout
void ClientConnection::Update(s32 timeElapsedMs)
{
	if (IsRunning() == false)
	{
		//connection is not running
		//TODO: log
		return;
	}
	
	//Update timeout accumulator and disconnect if timeout is detected
	m_timeoutAccumulator += timeElapsedMs;
	if (m_timeoutAccumulator > m_timeout)
	{
		//TODO: log
		m_connectionDropSignal.invoke(-1);
		ClearData();
	}
}

//Connects game's update signal to connection's and reliability system's update slot
void ClientConnection::ConnectSlots(CL_Signal_v1<s32> updateSignal)
{
	Connection::ConnectSlots(updateSignal);
	m_reliabilitySystem.ConnectSlots(updateSignal);
}

//Initiates connection to server at @address
void ClientConnection::Connect(const Address& destination)
{
	ClearData();
	m_state = eConnecting;
	m_address = destination;
}

bool ClientConnection::SendPacket(const u8 data[], u32 size)
{
	if (IsRunning() == false)
	{
		//Connection is not running
		//TODO: log
		return false;
	}
	if (m_address == 0)
	{
		//Not connected to a server
		//TODO: log
		return false;
	}

	bool success = false;

	//Attach protocolId to packet header
	u8* packet = new u8[size + ms_headerSize];
	u32 sequence = m_reliabilitySystem.GetLocalSequence();
	u32 ack = m_reliabilitySystem.GetRemoteSequence();
	u32 ackBits = m_reliabilitySystem.GenerateAckBits();

	WriteHeader(packet, sequence, ack, ackBits);
	memcpy(packet + ms_headerSize, data, size);
	
	//Send the message to each connected client
	success = m_socket.Send(m_address, packet, size + ms_headerSize);
	if (success)
	{
		//TODO: log
		m_reliabilitySystem.PacketSent(size);
	}

	delete[] packet;
	return success;
}

u32 ClientConnection::ReceivePacket(u8 data[], u32 size)
{
	if (IsRunning() == false)
	{
		//Connection is not running
		//TODO: log
		return 0;
	}

	if (m_state == eDisconnected)
	{
		//Not connected to a server
		//TODO: log
		return 0;
	}

	//NOTE: add const int protocol id size
	u8* packet = new u8[size + ms_headerSize];
	Address sender;
	u32 bytes_read = m_socket.Receive(sender, packet, size + ms_headerSize);
	
	if (m_address != sender)
	{
		//Packet doesn't come from the server
		delete[] packet;
		return 0;
	}

	if (bytes_read <= ms_headerSize)
	{
		//No point in reading packets that are too small to even contain
		//a protocol id in the header
		delete[] packet;
		return 0;
	}

	if (packet[0] != (unsigned char)(m_protocolId >> 24) ||
		packet[1] != (unsigned char)((m_protocolId >> 16) & 0xFF) ||
		packet[2] != (unsigned char)((m_protocolId >> 8) & 0xFF) ||
		packet[3] != (unsigned char)(m_protocolId & 0xFF))
	{
		//Sender's protocol ID doesn't match the expected protocol ID
		delete[] packet;
		return 0;
	}	

	u32 packetSequence = 0;
	u32 packetAck = 0;
	u32 packetAckBits = 0;

	m_state = eConnected;
	m_timeoutAccumulator = 0;

	//Process packet
	ReadHeader(packet + 4, packetSequence, packetAck, packetAckBits);
	m_reliabilitySystem.PacketReceived(packetSequence, bytes_read - ms_headerSize);
	m_reliabilitySystem.ProcessAck(packetAck, packetAckBits);
	memcpy(data, &packet[ms_headerSize], bytes_read - ms_headerSize);
	delete[] packet;
	//TODO: log
	return bytes_read - ms_headerSize;
}

void ClientConnection::ClearData()
{
	m_state = eDisconnected;
	m_address = Address();
	m_timeoutAccumulator = 0;
	m_reliabilitySystem.Reset();
}