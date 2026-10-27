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
		string Name;
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
	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_MOBA_INFO_ALL; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "GCMobaInfoAll"; }
	string toString() const throw();

	void clearMembers() throw() { m_Members.clear(); }
	void addMember(const Member& member) throw() { m_Members.push_back(member); }
	const vector<Member>& getMembers() const throw() { return m_Members; }

private:
	vector<Member> m_Members;
};

class GCMobaInfoAllFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new GCMobaInfoAll(); }
	string getPacketName() const throw() { return "GCMobaInfoAll"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_MOBA_INFO_ALL; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + 10 * (szBYTE + 32 + szBYTE + szBYTE + szuint * 5 + szBYTE + szBYTE); }
};

class GCMobaInfoAllHandler
{
public:
	static void execute(GCMobaInfoAll* pPacket, Player* pPlayer) throw(Error);
};

#endif
