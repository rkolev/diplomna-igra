#include "Address.h"

Address::Address()
{
	m_address = 0;
	m_port = 0;
}

Address::Address(u8 a, u8 b, u8 c, u8 d, u16 port)
{	
	m_address = (a << 24) | (b << 16) | (c << 8) | d;
	m_port = port;
}

Address::Address(unsigned int address, u16 port)
{
	m_address = address;
	m_port = port;
}

unsigned int Address::GetAddress() const
{
	return m_address;
}

u8 Address::GetA() const
{
	return (u8)((m_address >> 24) & 0xFF);
}

u8 Address::GetB() const
{
	return (u8)((m_address >> 16) & 0xFF);
}

u8 Address::GetC() const
{
	return (u8)((m_address >> 8) & 0xFF);
}

u8 Address::GetD() const
{
	return (u8)(m_address & 0xFF);
}

u16 Address::GetPort() const
{
	return m_port;
}

bool Address::operator == (const Address & other) const
{
	return m_address == other.m_address && m_port == other.m_port;
}

bool Address::operator == (u32 other) const
{
	return m_address == other;
}

bool Address::operator != (const Address & other) const
{
	return !(*this == other);
}

bool Address::operator < (const Address & other) const
{
	if (m_address == other.GetAddress()) {
		return m_port < other.GetPort();
	}
	return m_address < other.GetAddress();	
}