#include "Connection.h"

u32 Connection::ms_headerSize = 16;

Connection::Connection()
{
	m_protocolId = 0;
	m_timeout = 0;
	m_running = false;
	ClearData();
}

Connection::Connection(u32 protocolId, u32 timeout)
{
	m_protocolId = protocolId;
	m_timeout = timeout;
	m_running = false;
	ClearData();
}

Connection::~Connection()
{
	if (IsRunning()) {
		Stop();
	}
}

void Connection::ConnectSlots(CL_Signal_v1<int> updateSignal)
{
	m_updateSlot = updateSignal.connect(this, &Connection::Update);
	m_updateSignal = updateSignal;
}

bool Connection::Start(u16 port)
{
	if (m_running == true) {
		//Connection is already running.
		return true;
	}
	if (m_socket.Open(port) == false) {
		//Unable to start connection
		//TODO: Log
		return false;
	}
	m_running = true;
	return true;
}

void Connection::Stop()
{
	if (m_running == false) {
		//Connection is already stopped.
		return;
	}
	ClearData();
	m_socket.Close();
	m_running = false;
}

void Connection::WriteInteger(u8* data, u32 value)
{
	data[0] = (u8)(value >> 24);
	data[1] = (u8)((value >> 16) & 0xFF);
	data[2] = (u8)((value >> 8) & 0xFF);
	data[3] = (u8)(value & 0xFF);
}

void Connection::WriteHeader(u8* header, u32 sequence, u32 ack, u32 ackBits)
{
	WriteInteger(header, m_protocolId);
	WriteInteger(header + 4, sequence);
	WriteInteger(header + 8, ack);
	WriteInteger(header + 12, ackBits);
}

void Connection::ReadInteger(u8 const *data, u32 &value)
{
	value = (((u32)data[0] << 24) | ((u32)data[1] << 16) | ((u32)data[2] << 8) | ((u32)data[3]));
}

void Connection::ReadHeader(u8 const *header, u32 &sequence, u32 &ack, u32 &ackBits)
{
	ReadInteger(header, sequence);
	ReadInteger(header + 4, ack);
	ReadInteger(header + 8, ackBits);
}