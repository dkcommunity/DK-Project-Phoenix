#ifndef __GC_TMALL_GOODS_LIST_H__
#define __GC_TMALL_GOODS_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "SubItemInfo.h"
#include <list>

// 2026-08-19: sekme basina 3 sayfa -> 20 sayfa (27 -> 180; 9 item/sayfa). Server kopyalariyla SENKRON tutulmali - read() validasyonu 27'de kalirsa disconnect atar.
#define MAX_TMALL_GOODS_LIST 180

typedef struct _TmallGoodsInfo
{
	int getPacketSize() const
	{ 
		return szObjectID + 
				szBYTE + 
				szItemType + 
				szGrade +
				szBYTE + optionType.size() +
				szItemNum +
				szDWORD +
				//szBYTE +
				//szBYTE +
				szGold +
				szGold;
	}

	static int getPacketMaxSize() 
	{ 
		return szObjectID + 
				szBYTE + 
				szItemType + 
				szGrade +
				szBYTE + 255 +
				szItemNum +
				szDWORD +
				//szBYTE +
				//szBYTE +
				szGold +
				szGold;
	}

	std::string toString() const
	{
		StringStream msg;
		msg << "Good( "
			<< "ObjectID : " << objectID
			<< ", ItemClass : " << (int)itemClass
			<< ", ItemType : " << itemType
			<< ", Grade : " << grade
			<< ", Options : (";

		std::list<OptionType_t>::const_iterator itr = optionType.begin();
		std::list<OptionType_t>::const_iterator endItr = optionType.end();

		for ( ; itr != endItr ; ++itr )
		{
			msg << *itr << ", ";
		}

		msg << "), Num : " << num
			<< ", TimeLimit : " << timeLimit;

		return msg.toString();
	}

	ObjectID_t     		objectID;
	BYTE           		itemClass;
	ItemType_t     		itemType;
	Grade_t				grade;
	std::list<OptionType_t>  optionType;
	ItemNum_t      		num;
	DWORD				timeLimit;
	//BYTE			Race;
	//BYTE			Kind;
	Gold_t			Cash;
	Gold_t			Point;
} TmallGoodsInfo;

//////////////////////////////////////////////////////////////////////////////
// class GCTMallGoodsList;
//////////////////////////////////////////////////////////////////////////////

class Item;

class GCTMallGoodsList : public Packet 
{
public:
	GCTMallGoodsList() ;
	virtual ~GCTMallGoodsList() ;

	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_TMALL_GOODS_LIST; }
	PacketSize_t getPacketSize() const ;
	std::string getPacketName() const  { return "GCTMallGoodsList"; }
	std::string toString() const ;
	bool		isEmpty() { return m_GoodsList.empty(); }

public:
	void		addGoodsInfo( TmallGoodsInfo* pGI ) { m_GoodsList.push_back( pGI ); }
	TmallGoodsInfo*	popGoodsInfo( ) { TmallGoodsInfo* pRet = m_GoodsList.front(); if ( pRet ) m_GoodsList.pop_front(); return pRet; }

private:
	std::list<TmallGoodsInfo*> m_GoodsList;
};


//////////////////////////////////////////////////////////////////////////////
// class GCTMallGoodsListFactory;
//////////////////////////////////////////////////////////////////////////////

class GCTMallGoodsListFactory : public PacketFactory 
{
public :
	Packet* createPacket()  { return new GCTMallGoodsList(); }
	std::string getPacketName() const  { return "GCTMallGoodsList"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_TMALL_GOODS_LIST; }
	PacketSize_t getPacketMaxSize() const  
	{ 
		PacketSize_t size = szBYTE;
		size += TmallGoodsInfo::getPacketMaxSize() * MAX_TMALL_GOODS_LIST;

		return size;
	}
};


//////////////////////////////////////////////////////////////////////////////
// class GCTMallGoodsListtHandler;
//////////////////////////////////////////////////////////////////////////////

class GCTMallGoodsListHandler 
{
public :
	static void execute(GCTMallGoodsList* pPacket, Player* pPlayer) ;

};

#endif
