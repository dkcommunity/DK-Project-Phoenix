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

	GCMobaJoin() throw()
		: m_State(QUEUE_STATE_CANCELLED), m_JoinedCount(0), m_MaxCount(0), m_RemainingSeconds(0) {}
	virtual ~GCMobaJoin() throw() {}

	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_MOBA_JOIN; }
	PacketSize_t getPacketSize() const throw() { return szBYTE + szBYTE + szBYTE + szWORD; }
	string getPacketName() const throw() { return "GCMobaJoin"; }
	string toString() const throw();

	BYTE getState() const throw() { return m_State; }
	void setState(BYTE state) throw() { m_State = state; }

	BYTE getJoinedCount() const throw() { return m_JoinedCount; }
	void setJoinedCount(BYTE count) throw() { m_JoinedCount = count; }

	BYTE getMaxCount() const throw() { return m_MaxCount; }
	void setMaxCount(BYTE count) throw() { m_MaxCount = count; }

	WORD getRemainingSeconds() const throw() { return m_RemainingSeconds; }
	void setRemainingSeconds(WORD seconds) throw() { m_RemainingSeconds = seconds; }

private:
	BYTE m_State;
	BYTE m_JoinedCount;
	BYTE m_MaxCount;
	WORD m_RemainingSeconds;
};

class GCMobaJoinFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new GCMobaJoin(); }
	string getPacketName() const throw() { return "GCMobaJoin"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_MOBA_JOIN; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szBYTE + szBYTE + szWORD; }
};

class GCMobaJoinHandler
{
public:
	static void execute(GCMobaJoin* pPacket, Player* pPlayer) throw(Error);
};

#endif
