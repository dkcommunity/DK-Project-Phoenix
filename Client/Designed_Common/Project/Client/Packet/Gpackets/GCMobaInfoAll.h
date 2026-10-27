//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaInfoAll.h
// Description : AOS/MOBA team scoreboard packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_MOBA_INFO_ALL_H__
#define __GC_MOBA_INFO_ALL_H__

#include "Packet.h"
#include "PacketFactory.h"
#include <vector>

class GCMobaInfoAll : public Packet
{
public:
	struct Member
	{
		std::string Name;
		BYTE Body;
		BYTE Hair;
		uint Level;
		uint KillPlayer;
		uint Dead;
		uint KillMonster;
		uint TeamCode;
		BYTE ResultCode;
		BYTE RaceCode;

		Member()
			: Body(0), Hair(0), Level(0), KillPlayer(0), Dead(0),
			  KillMonster(0), TeamCode(0), ResultCode(0), RaceCode(0) {}
	};

public:
	void read(SocketInputStream& iStream);
	void write(SocketOutputStream& oStream) const;
	void execute(Player* pPlayer);

	PacketID_t getPacketID() const { return PACKET_GC_MOBA_INFO_ALL; }
	PacketSize_t getPacketSize() const;
	std::string getPacketName() const { return "GCMobaInfoAll"; }
	std::string toString() const;

	const std::vector<Member>& getMembers() const { return m_Members; }

private:
	std::vector<Member> m_Members;
};

class GCMobaInfoAllFactory : public PacketFactory
{
public:
	Packet* createPacket() { return new GCMobaInfoAll(); }
	std::string getPacketName() const { return "GCMobaInfoAll"; }
	PacketID_t getPacketID() const { return Packet::PACKET_GC_MOBA_INFO_ALL; }
	PacketSize_t getPacketMaxSize() const { return szBYTE + 10 * (szBYTE + 32 + szBYTE + szBYTE + szuint * 5 + szBYTE + szBYTE); }
};

class GCMobaInfoAllHandler
{
public:
	static void execute(GCMobaInfoAll* pPacket, Player* pPlayer);
};

#endif
