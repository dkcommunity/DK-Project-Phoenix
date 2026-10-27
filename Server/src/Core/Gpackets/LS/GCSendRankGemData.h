#ifndef __GC_SEND_RANK_GEM_DATA_H__
#define __GC_SEND_RANK_GEM_DATA_H__

#include "Packet.h"
#include "PacketFactory.h"
#include <list>

#define MAX_RANK_GEM_DATA 56

typedef struct _RankGemData
{
	int getPacketSize() const
	{ 
		return szGold + szGold + szGold;
	}

	static int getPacketMaxSize() 
	{ 
		return szGold + szGold + szGold;
	}

	string toString() const
	{
		StringStream msg;
		msg << "RankGemData( "
			<< "Slot : " << Slot
			<< ", Type : " << Type
			<< ", Grade : " << Grade;

		return msg.toString();
	}

	Gold_t			Slot;
	Gold_t			Type;
	Gold_t			Grade;
} RankGemData;

class GCSendRankGemData : public Packet 
{
public:
	GCSendRankGemData() throw();
	virtual ~GCSendRankGemData() throw();

	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_SEND_RANK_GEM_DATA; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "GCSendRankGemData"; }
	string toString() const throw();

public:
	void		addRankGemData( RankGemData* pGI ) { m_RankGemData.push_back( pGI ); }
	RankGemData*	popRankGemData( ) { RankGemData* pRet = m_RankGemData.front(); if ( pRet ) m_RankGemData.pop_front(); return pRet; }

private:
	list<RankGemData*> m_RankGemData;
};

class GCSendRankGemDataFactory : public PacketFactory 
{
public :
	Packet* createPacket() throw() { return new GCSendRankGemData(); }
	string getPacketName() const throw() { return "GCSendRankGemData"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SEND_RANK_GEM_DATA; }
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		PacketSize_t size = szBYTE;
		size += RankGemData::getPacketMaxSize() * MAX_RANK_GEM_DATA;

		return size;
	}
};

class GCSendRankGemDataHandler 
{
public :
	static void execute(GCSendRankGemData* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
