#ifndef __GC_SEND_NEW_WARINFO_H__
#define __GC_SEND_NEW_WARINFO_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "SubItemInfo.h"
#include <list>

#define MAX_RACE 3
#define MAX_RANK 10

typedef struct _RaceData
{
	int getPacketSize() const
	{ 
		return 		szBYTE + 
				RaceBookType.size() +
				szGold +
				szGold +
				szint;
	}

	static int getPacketMaxSize() 
	{ 
		return 		szBYTE + 
				255 +
				szGold +
				szGold +
				szint;
	}
	string toString() const
	{
		StringStream msg;
		msg << "RaceData( "
			<< "BookCount : " << BookCount
			<< ", RaceBookType : (";

		list<ItemType_t>::const_iterator itr = RaceBookType.begin();
		list<ItemType_t>::const_iterator endItr = RaceBookType.end();

		for ( ; itr != endItr ; ++itr )
		{
			msg << *itr << ", ";
		}

		msg << "), RaceKillCount : " << RaceKillCount
			<< ", RaceDeadCount : " << RaceDeadCount
			<< ", RacePoint : " << RacePoint;

		return msg.toString();
	}
	BYTE     		BookCount;
	list<ItemType_t>     	RaceBookType;
	Gold_t			RaceKillCount;
	Gold_t			RaceDeadCount;
	int      		RacePoint;
} RaceData;
typedef struct _ScoreListSlayer
{
	int getPacketSize() const
	{ 
		return RankName.size() + 
				szGold + 
				szGold +
				szint ;
	}

	static int getPacketMaxSize() 
	{ 
		return 255 + 
				szGold + 
				szGold +
				szint;
	}

	string     		RankName;
	Gold_t			RankKillCount;
	Gold_t			RankDeadCount;
	int      		RankPoint;
} ScoreListSlayer;
typedef struct _ScoreListVampire
{
	int getPacketSize() const
	{ 
		return RankName.size() + 
				szGold + 
				szGold +
				szint ;
	}

	static int getPacketMaxSize() 
	{ 
		return 255 + 
				szGold + 
				szGold +
				szint;
	}

	string     		RankName;
	Gold_t			RankKillCount;
	Gold_t			RankDeadCount;
	int      		RankPoint;
} ScoreListVampire;
typedef struct _ScoreListOusters
{
	int getPacketSize() const
	{ 
		return RankName.size() + 
				szGold + 
				szGold +
				szint ;
	}

	static int getPacketMaxSize() 
	{ 
		return 255 + 
				szGold + 
				szGold +
				szint;
	}

	string     		RankName;
	Gold_t			RankKillCount;
	Gold_t			RankDeadCount;
	int      		RankPoint;
} ScoreListOusters;
//////////////////////////////////////////////////////////////////////////////
// class GCSendNewWarInfo;
//////////////////////////////////////////////////////////////////////////////

class Item;

class GCSendNewWarInfo : public Packet 
{
public:
	GCSendNewWarInfo() throw();
	virtual ~GCSendNewWarInfo() throw();

	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_SEND_NEW_WARINFO; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "GCSendNewWarInfo"; }
	string toString() const throw();

public:
	void		addRaceData( RaceData* pGI ) { m_RaceData.push_back( pGI ); }
	RaceData*	popRaceData( ) { RaceData* pRet = m_RaceData.front(); if ( pRet ) m_RaceData.pop_front(); return pRet; }

	void		addScoreListSlayer( ScoreListSlayer* pGI ) { m_ScoreListSlayer.push_back( pGI ); }
	ScoreListSlayer*	popScoreListSlayer( ) { ScoreListSlayer* pRet = m_ScoreListSlayer.front(); if ( pRet ) m_ScoreListSlayer.pop_front(); return pRet; }

	void		addScoreListVampire( ScoreListVampire* pGI ) { m_ScoreListVampire.push_back( pGI ); }
	ScoreListVampire*	popScoreListVampire( ) { ScoreListVampire* pRet = m_ScoreListVampire.front(); if ( pRet ) m_ScoreListVampire.pop_front(); return pRet; }

	void		addScoreListOusters( ScoreListOusters* pGI ) { m_ScoreListOusters.push_back( pGI ); }
	ScoreListOusters*	popScoreListOusters( ) { ScoreListOusters* pRet = m_ScoreListOusters.front(); if ( pRet ) m_ScoreListOusters.pop_front(); return pRet; }

	Gold_t getPlayerKillCount(void) const throw() { return m_PlayerKillCount;}
	void setPlayerKillCount(Gold_t PlayerKillCount) throw() { m_PlayerKillCount = PlayerKillCount;}

	Gold_t getPlayerDeadCount(void) const throw() { return m_PlayerDeadCount;}
	void setPlayerDeadCount(Gold_t PlayerDeadCount) throw() { m_PlayerDeadCount = PlayerDeadCount;}

	Gold_t getPlayerRacePoint(void) const throw() { return m_PlayerRacePoint;}
	void setPlayerRacePoint(Gold_t PlayerRacePoint) throw() { m_PlayerRacePoint = PlayerRacePoint;}

	Gold_t getPlayerKillPoint(void) const throw() { return m_PlayerKillPoint;}
	void setPlayerKillPoint(Gold_t PlayerKillPoint) throw() { m_PlayerKillPoint = PlayerKillPoint;}

	Gold_t getPlayerTotalPoint(void) const throw() { return m_PlayerTotalPoint;}
	void setPlayerTotalPoint(Gold_t PlayerTotalPoint) throw() { m_PlayerTotalPoint = PlayerTotalPoint;}


private:
	list<RaceData*> m_RaceData;
	list<ScoreListSlayer*> m_ScoreListSlayer;
	list<ScoreListVampire*> m_ScoreListVampire;
	list<ScoreListOusters*> m_ScoreListOusters;

	Gold_t 		m_PlayerKillCount;
	Gold_t 		m_PlayerDeadCount;
	Gold_t 		m_PlayerRacePoint;
	Gold_t 		m_PlayerKillPoint;
	Gold_t 		m_PlayerTotalPoint;
};


//////////////////////////////////////////////////////////////////////////////
// class GCSendNewWarInfoFactory;
//////////////////////////////////////////////////////////////////////////////

class GCSendNewWarInfoFactory : public PacketFactory 
{
public :
	Packet* createPacket() throw() { return new GCSendNewWarInfo(); }
	string getPacketName() const throw() { return "GCSendNewWarInfo"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_SEND_NEW_WARINFO; }
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		PacketSize_t size = szBYTE;
		size += RaceData::getPacketMaxSize() * MAX_RACE + ScoreListSlayer::getPacketMaxSize() * MAX_RANK + ScoreListVampire::getPacketMaxSize() * MAX_RANK + ScoreListOusters::getPacketMaxSize() * MAX_RANK + szGold + szGold + szGold + szGold + szGold;

		return size;
	}
};


//////////////////////////////////////////////////////////////////////////////
// class GCSendNewWarInfoHandler;
//////////////////////////////////////////////////////////////////////////////

class GCSendNewWarInfoHandler 
{
public :
	static void execute(GCSendNewWarInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
