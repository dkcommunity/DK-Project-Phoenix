//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaInfoSelf.h
// Description : AOS/MOBA self score packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_MOBA_INFO_SELF_H__
#define __GC_MOBA_INFO_SELF_H__

#include "Packet.h"
#include "PacketFactory.h"

class GCMobaInfoSelf : public Packet
{
public:
	GCMobaInfoSelf() throw()
		: m_Level(0), m_KillPlayer(0), m_Dead(0), m_KillMonster(0), m_TimeLeft(0) {}
	virtual ~GCMobaInfoSelf() throw() {}

	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_MOBA_INFO_SELF; }
	PacketSize_t getPacketSize() const throw() { return szuint * 5; }
	string getPacketName() const throw() { return "GCMobaInfoSelf"; }
	string toString() const throw();

	void setLevel(uint value) throw() { m_Level = value; }
	void setKillPlayer(uint value) throw() { m_KillPlayer = value; }
	void setDead(uint value) throw() { m_Dead = value; }
	void setKillMonster(uint value) throw() { m_KillMonster = value; }
	void setTimeLeft(uint value) throw() { m_TimeLeft = value; }

	uint getLevel() const throw() { return m_Level; }
	uint getKillPlayer() const throw() { return m_KillPlayer; }
	uint getDead() const throw() { return m_Dead; }
	uint getKillMonster() const throw() { return m_KillMonster; }
	uint getTimeLeft() const throw() { return m_TimeLeft; }

private:
	uint m_Level;
	uint m_KillPlayer;
	uint m_Dead;
	uint m_KillMonster;
	uint m_TimeLeft;
};

class GCMobaInfoSelfFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new GCMobaInfoSelf(); }
	string getPacketName() const throw() { return "GCMobaInfoSelf"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_MOBA_INFO_SELF; }
	PacketSize_t getPacketMaxSize() const throw() { return szuint * 5; }
};

class GCMobaInfoSelfHandler
{
public:
	static void execute(GCMobaInfoSelf* pPacket, Player* pPlayer) throw(Error);
};

#endif
