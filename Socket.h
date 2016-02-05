#pragma once

#include "Address.h"

class Socket
{
public: //methods
	//Default constructor
	//TODO: private?
	Socket();

	//Default destructor
	~Socket();

	//Opens socket on @port
	bool Open(u16 port);

	//Closes socket
	void Close();

	//Cehcks if socket is open
	bool IsOpen() const;

	//Sends @data to @destination address
	bool Send(const Address &destination, const void *data, u32 size);

	//Receives @data from @sender address
	int Receive(Address &sender, void *data, u32 size);

private: //members
	int m_handle;
}; //class Socket

