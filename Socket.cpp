#include "Socket.h"

Socket::Socket()
{
	m_handle = 0;
}

bool Socket::Open(u16 port)
{
	if (IsOpen()) {
		//TODO: If socket is already opened, return error
		//need implementation of a logger for this
		return false;
	}

	//Create socket
	if ((m_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 0) {
		//Socket creation has failed
		//TODO: Log("Failed to create socket. Code: %d\n", WSAGetLastError());
		return false;
	}

	//Bind socket to port
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (::bind(m_handle, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0) {
		//Bind socket to port has failed
		//TODO: Log("Failed to bind socket. Code: %d\n", WSAGetLastError());
		return false;
	}

	//Set socket to non-blocking
	DWORD nonBlocking = 1;
	if (ioctlsocket(m_handle, FIONBIO, &nonBlocking) != 0) {
		//Set socket to non-blocking has failed
		//TODO: Log("Failed to set non-blocking. Code: %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

void Socket::Close()
{
	if (m_handle != 0) {
		closesocket(m_handle);
		m_handle = 0;
	}
}

bool Socket::IsOpen() const
{
	return m_handle != 0;
}

bool Socket::Send(const Address &destination, const void *data, u32 size)
{
	//If the data doesn't exist or the size is 0, break the operation
	if (!data || size == 0)	{
		//TODO: Log("No data to send.\n");
		return false;
	}

	//Check if socket is opened
	if (IsOpen() == false) {
		//TODO: Log("Socket not opened.\n");
		return false;
	}

	//Check if address is valid
	if (destination.GetAddress() == 0 || destination.GetPort() == 0) {
		//TODO: Log("Address is not valid.\n");
		return false;
	}

	//Set destination address
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(destination.GetAddress());
	address.sin_port = htons(destination.GetPort());

	//Send data
	s32 sent_bytes = sendto(m_handle, (const s8*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in));

	//Check if all data has been sent successfully
	//TODO: worried that signed/unsigned mismatch might cause bugs
	//but that would only happen with extremely long messages (bigger than sizeof(int))
	return sent_bytes == size;
}

int Socket::Receive(Address &sender, void *data, u32 size)
{
	//Check if socket is opened
	if (IsOpen() == false) {
		//TODO: Log("Socket not opened.\n");
		return false;
	}

	sockaddr_in from;
	s32 fromLength = sizeof(from);

	//Recieve data
	s32 received_bytes = recvfrom(m_handle, (s8*)data, size, 0, (sockaddr*)&from, &fromLength);

	//If the received message's length is <= 0, break the operation and print an error
	if (received_bytes <= 0) {
		//Get error code
		s32 error = WSAGetLastError();
		if (error != 10035 || error != 10054) {
			//TODO: what were these error codes and why did I exclude them
			//TODO: Log("Recvfrom error. Code: %d\n", error);
		}
		return 0;
	}

	//Save the sender address/port to an Address object
	u32 address = ntohl(from.sin_addr.s_addr);
	u16 port = ntohs(from.sin_port);
	sender = Address(address, port);

	//Return the size of the received message
	return received_bytes;
}

Socket::~Socket()
{
	Close();
}
