#if 0
//////////////////////////////////////////////////////////////////////////////
// Filename    : GCUpdateSupplyItemList.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_SUPPLYITEM_LIST_H__
#define __GC_SUPPLYITEM_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"
#include <list>

using namespace std;


#define MAX_SUPPLYITEM_LIST 20

typedef struct _SupplyItemInfo
{
	int getPacketSize() const
	{ 
		return szObjectID + 
				szBYTE + 
				szItemType + 
				szGrade +
				szBYTE + optionType.size() +
				szItemNum +
				szDWORD;
	}

	static int getPacketMaxSize() 
	{ 
		return szObjectID + 
				szBYTE + 
				szItemType + 
				szGrade +
				szBYTE + 255 +
				szItemNum +
				szDWORD;
	}

	std::string toString() const
	{
		StringStream msg;
		msg << "SupplyItem( "
			<< "ID : " << ID
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

	ObjectID_t     		ID;
	BYTE           		itemClass;
	ItemType_t     		itemType;
	Grade_t				grade;
	std::list<OptionType_t>  optionType;
	ItemNum_t      		num;
	DWORD				timeLimit;
} SupplyItemInfo;

//////////////////////////////////////////////////////////////////////////////
// class GCUpdateSupplyItemList;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateSupplyItemList : public Packet 
{
public:
	GCUpdateSupplyItemList() ;
	virtual ~GCUpdateSupplyItemList() ;

	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_SUPPLYITEM_LIST; }
	PacketSize_t getPacketSize() const ;
#ifdef __DEBUG_OUTPUT__	
	std::string getPacketName() const  { return "GCUpdateSupplyItemList"; }
	std::string toString() const ;
#endif
public:
	void		addSupplyItemInfo( SupplyItemInfo* pSI ) { m_SupplyItemList.push_back( pSI ); }
	SupplyItemInfo*	popSupplyItemInfo() { SupplyItemInfo* pRet = NULL; if( m_SupplyItemList.size() > 0 ) { pRet = m_SupplyItemList.front(); m_SupplyItemList.pop_front(); } return pRet; }

private:
	std::list<SupplyItemInfo*> m_SupplyItemList;
};


//////////////////////////////////////////////////////////////////////////////
// class GCUpdateSupplyItemListFactory;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateSupplyItemListFactory : public PacketFactory 
{
public :
	Packet* createPacket()  { return new GCUpdateSupplyItemList(); }
	std::string getPacketName() const  { return "GCUpdateSupplyItemList"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_SUPPLYITEM_LIST; }
	PacketSize_t getPacketMaxSize() const  
	{ 
		PacketSize_t size = szBYTE;
		size += SupplyItemInfo::getPacketMaxSize() * MAX_SUPPLYITEM_LIST;

		return size;
	}
};


//////////////////////////////////////////////////////////////////////////////
// class GCUpdateSupplyItemListHandler;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateSupplyItemListHandler 
{
public :
	static void execute(GCUpdateSupplyItemList* pPacket, Player* pPlayer) ;

};

#endif

#endif
