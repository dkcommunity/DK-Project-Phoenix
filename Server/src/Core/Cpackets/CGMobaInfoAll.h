//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMobaInfoAll.h
// Description : AOS/MOBA scoreboard request packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_MOBA_INFO_ALL_H__
#define __CG_MOBA_INFO_ALL_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGMobaInfoAll : public Packet
{
public:
	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_CG_MOBA_INFO_ALL; }
	PacketSize_t getPacketSize() const throw() { return szBYTE + m_TargetName.size(); }
	string getPacketName() const throw() { return "CGMobaInfoAll"; }
	string toString() const throw();

	const string& getTargetName() const throw() { return m_TargetName; }
	void setTargetName(const string& name) throw() { m_TargetName = name; }

private:
	string m_TargetName;
};

class CGMobaInfoAllFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new CGMobaInfoAll(); }
	string getPacketName() const throw() { return "CGMobaInfoAll"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_MOBA_INFO_ALL; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + 32; }
};

class CGMobaInfoAllHandler
{
public:
	static void execute(CGMobaInfoAll* pPacket, Player* pPlayer) throw(Error);
};

#endif
