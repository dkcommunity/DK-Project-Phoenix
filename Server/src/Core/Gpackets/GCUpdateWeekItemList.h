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

#define MAX_WEEKITEM_LIST 500

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

	string toString() const
	{
		StringStream msg;
		msg << "SupplyItem( "
			<< "ID : " << ID
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
			<< ", TimeLimit : " << timeLimit
			<< ", Durability : " << durability;

		return msg.toString();
	}

	ObjectID_t     		ID;
	BYTE           		itemClass;
	ItemType_t     		itemType;
	Grade_t				grade;
	list<OptionType_t>  optionType;
	ItemNum_t      		num;
	DWORD				timeLimit;
	Durability_t		durability;
} WeekItemInfo;

//////////////////////////////////////////////////////////////////////////////
// class GCUpdateWeekItemList;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateWeekItemList : public Packet 
{
public:
	GCUpdateWeekItemList() throw();
	virtual ~GCUpdateWeekItemList() throw();

	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_UPDATE_WEEKITEM_LIST; }
	PacketSize_t getPacketSize() const throw();
	string getPacketName() const throw() { return "GCUpdateWeekItemList"; }
	string toString() const throw();

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
	list<WeekItemInfo*> m_WeekItemList;
};


//////////////////////////////////////////////////////////////////////////////
// class GCUpdateWeekItemListFactory;
//////////////////////////////////////////////////////////////////////////////

class GCUpdateWeekItemListFactory : public PacketFactory 
{
public :
	Packet* createPacket() throw() { return new GCUpdateWeekItemList(); }
	string getPacketName() const throw() { return "GCUpdateWeekItemList"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_UPDATE_WEEKITEM_LIST; }
	PacketSize_t getPacketMaxSize() const throw() 
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
	static void execute(GCUpdateWeekItemList* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
