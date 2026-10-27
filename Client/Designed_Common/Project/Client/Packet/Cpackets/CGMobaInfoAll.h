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
	void read(SocketInputStream& iStream);
	void write(SocketOutputStream& oStream) const;
	void execute(Player* pPlayer);

	PacketID_t getPacketID() const { return PACKET_CG_MOBA_INFO_ALL; }
	PacketSize_t getPacketSize() const { return szBYTE + m_TargetName.size(); }
	std::string getPacketName() const { return "CGMobaInfoAll"; }
	std::string toString() const;

	const std::string& getTargetName() const { return m_TargetName; }
	void setTargetName(const std::string& name) { m_TargetName = name; }

private:
	std::string m_TargetName;
};

class CGMobaInfoAllFactory : public PacketFactory
{
public:
	Packet* createPacket() { return new CGMobaInfoAll(); }
	std::string getPacketName() const { return "CGMobaInfoAll"; }
	PacketID_t getPacketID() const { return Packet::PACKET_CG_MOBA_INFO_ALL; }
	PacketSize_t getPacketMaxSize() const { return szBYTE + 32; }
};

class CGMobaInfoAllHandler
{
public:
	static void execute(CGMobaInfoAll* pPacket, Player* pPlayer);
};

#endif
