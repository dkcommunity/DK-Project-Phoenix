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
	GCMobaInfoSelf()
		: m_Level(0), m_KillPlayer(0), m_Dead(0), m_KillMonster(0), m_TimeLeft(0) {}

	void read(SocketInputStream& iStream);
	void write(SocketOutputStream& oStream) const;
	void execute(Player* pPlayer);

	PacketID_t getPacketID() const { return PACKET_GC_MOBA_INFO_SELF; }
	PacketSize_t getPacketSize() const { return szuint * 5; }
	std::string getPacketName() const { return "GCMobaInfoSelf"; }
	std::string toString() const;

	uint getLevel() const { return m_Level; }
	uint getKillPlayer() const { return m_KillPlayer; }
	uint getDead() const { return m_Dead; }
	uint getKillMonster() const { return m_KillMonster; }
	uint getTimeLeft() const { return m_TimeLeft; }

	void setLevel(uint value) { m_Level = value; }
	void setKillPlayer(uint value) { m_KillPlayer = value; }
	void setDead(uint value) { m_Dead = value; }
	void setKillMonster(uint value) { m_KillMonster = value; }
	void setTimeLeft(uint value) { m_TimeLeft = value; }

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
	Packet* createPacket() { return new GCMobaInfoSelf(); }
	std::string getPacketName() const { return "GCMobaInfoSelf"; }
	PacketID_t getPacketID() const { return Packet::PACKET_GC_MOBA_INFO_SELF; }
	PacketSize_t getPacketMaxSize() const { return szuint * 5; }
};

class GCMobaInfoSelfHandler
{
public:
	static void execute(GCMobaInfoSelf* pPacket, Player* pPlayer);
};

#endif
