#pragma once
#include "Socket.h"
#include "ReliabilitySystem.h"

class Connection
{
public: //methods
	//Default constructor
	//TODO: private?
	Connection();

	//Constructor with values
	Connection(u32 protocolId, u32 timeout);
	
	//Default destructor
	virtual ~Connection();

	//Connects game's update signal to update slot
	virtual void ConnectSlots(CL_Signal_v1<s32> updateSignal);

	//Starts connection on @port
	bool Start(u16 port);

	//Stops connection
	void Stop();
	
	//Checks if the connection is started
	bool IsRunning() const { return m_running; }

	//Returns the connection state
	ConnectionState GetState() const { return m_state; }

	//Returns the connection mode
	ConnectionMode GetMode() const { return m_mode; }

	//Returns the connection's connectionDropSignal
	CL_Signal_v1<u8> GetConnectionDropSignal() const { return m_connectionDropSignal; }

	//Connects to the specified @address
	virtual void Connect(const Address &address) { }

	//Updates the connection from frame to frame
	virtual void Update(s32 timeElapsedMs) { }

	//Sends @data over the connection
	virtual bool SendPacket(const u8 data[], u32 size) { return false; }

	//Sends @data to the specified connection by @clientId
	//TODO: does this need to be shared by both types of connections?
	virtual bool SendToSingleClient(const u8 data[], u32 size, u16 clientId) { return false; }

	//Receives @data sent over the connection
	virtual u32 ReceivePacket(u8 data[], u32 size) { return 0; }

protected: //methods
	//Clears all saved information since the start of the connection
	virtual void ClearData() { }

	//Assigns @value to the first 4 bytes of @data
	void WriteInteger(u8* data, u32 value);

	//Composes the packet header
	void WriteHeader(u8* header, u32 sequence, u32 ack, u32 ackBits);

	//Reads the first 4 bytes from @data and saves them in @value
	void ReadInteger(u8 const *data, u32 &value);

	//Reads data from the packet header
	void ReadHeader(u8 const *header, u32 &sequence, u32 &ack, u32 &ackBits);

protected: //members
	static u32 ms_headerSize;
	u32 m_protocolId;
	u32 m_timeout;
	bool m_running;
	ConnectionMode m_mode;
	ConnectionState m_state;
	Socket m_socket;
	CL_Slot m_updateSlot;
	CL_Signal_v1<u8> m_connectionDropSignal;
	CL_Signal_v1<s32> m_updateSignal;

}; //class Connection

