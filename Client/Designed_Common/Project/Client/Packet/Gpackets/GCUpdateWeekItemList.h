//////////////////////////////////////////////////////////////////////////////
// Filename    : GCUpdateWeekItemList.h 
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_UPDATE_WEEKITEM_LIST_H__
#define __GC_UPDATE_WEEKITEM_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"
#include <list>

using namespace std;

#define MAX_WEEKITEM_LIST 500

#if __CONTENTS(__PREMIUM_GIVE_ITEM_UI)
typedef struct _WeekItemInfo
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
				szDurability;
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
				szDurability;
	}

	std::string toString() const
	{
		StringStream msg;
		msg << "WeekItem( "
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
			<< ", TimeLimit : " << timeLimit
			<< ", Durability : " << Durability;
		
		return msg.toString();
	}

	ObjectID_t     		ID;
	BYTE           		itemClass;
	ItemType_t     		itemType;
	Grade_t				grade;
	std::list<OptionType_t>  optionType;
	ItemNum_t      		num;
	DWORD				timeLimit;
	Durability_t		Durability;
} WeekItemInfo;

//////////////////////////////////////////////////////////////////////////////
// class GCUpdateWeekItemList;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateWeekItemList : public Packet 
{
public:
	GCUpdateWeekItemList() ;
	virtual ~GCUpdateWeekItemList() ;

	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_UPDATE_WEEKITEM_LIST; }
	PacketSize_t getPacketSize() const ;
#ifdef __DEBUG_OUTPUT__	
	std::string getPacketName() const  { return "GCUpdateWeekItemList"; }
	std::string toString() const ;
#endif
	
public:
	void		addWeekItemInfo( WeekItemInfo* pSI ) { m_WeekItemList.push_back( pSI ); }

	WeekItemInfo* popWeekItemInfo() 
	{ 
		WeekItemInfo* pRet = NULL;
		
		if( m_WeekItemList.size() > 0 ) 
		{ 
			pRet = m_WeekItemList.front(); 
			m_WeekItemList.pop_front(); 
		}
		
		return pRet; 
	}
	
private:
	std::list<WeekItemInfo*> m_WeekItemList;
};


//////////////////////////////////////////////////////////////////////////////
// class GCUpdateWeekItemListFactory;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateWeekItemListFactory : public PacketFactory 
{
public :
	Packet* createPacket()  { return new GCUpdateWeekItemList(); }
	std::string getPacketName() const  { return "GCUpdateWeekItemList"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_UPDATE_WEEKITEM_LIST; }
	PacketSize_t getPacketMaxSize() const  
	{ 
		PacketSize_t size = szBYTE;
		size += WeekItemInfo::getPacketMaxSize() * MAX_WEEKITEM_LIST;

		return size;
	}
};


//////////////////////////////////////////////////////////////////////////////
// class GCUpdateWeekItemListHandler;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateWeekItemListHandler 
{
public :
	static void execute(GCUpdateWeekItemList* pPacket, Player* pPlayer) ;

};

#endif
#endif
