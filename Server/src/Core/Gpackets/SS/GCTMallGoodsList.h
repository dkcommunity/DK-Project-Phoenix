#ifndef __GC_TMALL_GOODS_LIST_H__
#define __GC_TMALL_GOODS_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "SubItemInfo.h"
#include <list>

// 2026-08-19: sekme basina 3 sayfa -> 20 sayfa (27 -> 180; 9 item/sayfa). Client kopyasiyla (Designed_Common) SENKRON tutulmali - taraflardan biri 27'de kalirsa read() validasyonu disconnect atar.
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

	string toString() const
	{
		StringStream msg;
		msg << "Good( "
			<< "ObjectID : " << objectID
			<< ", ItemClass : " << (int)itemClass
			<< ", ItemType : " << itemType
			<< ", Grade : " << grade
			<< ", Options : (";

		list<OptionType_t>::const_iterator itr = optionType.begin();
		list<OptionType_t>::const_iterator endItr = optionType.end();

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
	list<OptionType_t>  optionType;
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
	GCTMallGoodsList() throw();
	virtual ~GCTMallGoodsList() throw();

	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_TMALL_GOODS_LIST; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "GCTMallGoodsList"; }
	string toString() const throw();

public:
	void		addGoodsInfo( TmallGoodsInfo* pGI ) { m_GoodsList.push_back( pGI ); }
	TmallGoodsInfo*	popGoodsInfo( ) { TmallGoodsInfo* pRet = m_GoodsList.front(); if ( pRet ) m_GoodsList.pop_front(); return pRet; }

private:
	list<TmallGoodsInfo*> m_GoodsList;
};


//////////////////////////////////////////////////////////////////////////////
// class GCTMallGoodsListFactory;
//////////////////////////////////////////////////////////////////////////////

class GCTMallGoodsListFactory : public PacketFactory 
{
public :
	Packet* createPacket() throw() { return new GCTMallGoodsList(); }
	string getPacketName() const throw() { return "GCTMallGoodsList"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_TMALL_GOODS_LIST; }
	PacketSize_t getPacketMaxSize() const throw() 
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
	static void execute(GCTMallGoodsList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
