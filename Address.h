#pragma once

#include "Network.h"

using namespace Network;

class Address
{

public: //methods
	//Default contstructor
	//TODO: private?
	Address();

	//Constructor with values /unsigned chars/
	Address(u8 a, u8 b, u8 c, u8 d, u16 port);

	//Constructor with values /int/
	Address(u32 address, u16 port);

	//Returns address as int
	u32 GetAddress() const;

	//Returns first hex of address
	u8 GetA() const;

	//Returns second hex of address 
	u8 GetB() const;

	//Returns third hex of address
	u8 GetC() const;

	//Returns fourth hex of address
	u8 GetD() const;

	//Returns port 
	u16 GetPort() const;

	//Equals operator override
	//Compare with other address
	bool operator == (const Address& other) const;

	//Equals operator override
	//Compare with address as int
	bool operator == (u32 other) const;

	//Less than operator override
	bool operator < (const Address& other) const;

	//TODO: probably don't need this
	bool operator != (const Address& other) const;

private: //members
	u32 m_address;
	u16 m_port;

}; //class Address

