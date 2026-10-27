//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaJoin.h
// Description : AOS/MOBA queue state packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_MOBA_JOIN_H__
#define __GC_MOBA_JOIN_H__

#include "Packet.h"
#include "PacketFactory.h"

class GCMobaJoin : public Packet
{
public:
	enum QueueState
	{
		QUEUE_STATE_CANCELLED = 0,
		QUEUE_STATE_WAITING = 1,
		QUEUE_STATE_STARTED = 2,
		QUEUE_STATE_TIMEOUT = 3
	};

	GCMobaJoin()
		: m_State(QUEUE_STATE_CANCELLED), m_JoinedCount(0), m_MaxCount(0), m_RemainingSeconds(0) {}

	void read(SocketInputStream& iStream);
	void write(SocketOutputStream& oStream) const;
	void execute(Player* pPlayer);

	PacketID_t getPacketID() const { return PACKET_GC_MOBA_JOIN; }
	PacketSize_t getPacketSize() const { return szBYTE + szBYTE + szBYTE + szWORD; }
	std::string getPacketName() const { return "GCMobaJoin"; }
	std::string toString() const;

	BYTE getState() const { return m_State; }
	void setState(BYTE state) { m_State = state; }

	BYTE getJoinedCount() const { return m_JoinedCount; }
	void setJoinedCount(BYTE count) { m_JoinedCount = count; }

	BYTE getMaxCount() const { return m_MaxCount; }
	void setMaxCount(BYTE count) { m_MaxCount = count; }

	WORD getRemainingSeconds() const { return m_RemainingSeconds; }
	void setRemainingSeconds(WORD seconds) { m_RemainingSeconds = seconds; }

private:
	BYTE m_State;
	BYTE m_JoinedCount;
	BYTE m_MaxCount;
	WORD m_RemainingSeconds;
};

class GCMobaJoinFactory : public PacketFactory
{
public:
	Packet* createPacket() { return new GCMobaJoin(); }
	std::string getPacketName() const { return "GCMobaJoin"; }
	PacketID_t getPacketID() const { return Packet::PACKET_GC_MOBA_JOIN; }
	PacketSize_t getPacketMaxSize() const { return szBYTE + szBYTE + szBYTE + szWORD; }
};

class GCMobaJoinHandler
{
public:
	static void execute(GCMobaJoin* pPacket, Player* pPlayer);
};

#endif
