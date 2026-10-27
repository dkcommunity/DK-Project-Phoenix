#ifndef __GC_PAY_EVENT_LIST_H__
#define __GC_PAY_EVENT_LIST_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "SubItemInfo.h"
#include <list>

#define MAX_PAY_EVENT_LIST 27

typedef struct _PayEventInfo
{
	int getPacketSize() const
	{ 
		return szObjectID + // money 20 or 50 ---5000
				szBYTE + // itemclass
				szItemType + //item type
				szGrade + //item grade
				szBYTE + optionType.size() +//item option
				szItemNum +//item num
				szDWORD +//time Limit
				szGold;//IMAGEID
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
	Grade_t			grade;
	std::list<OptionType_t>      optionType;
	ItemNum_t      		num;
	DWORD			timeLimit;
	Gold_t			ImageID;
} PayEventInfo;

//////////////////////////////////////////////////////////////////////////////
// class GCPayEventList;
//////////////////////////////////////////////////////////////////////////////

class Item;

class GCPayEventList : public Packet 
{
public:
	GCPayEventList() ;
	virtual ~GCPayEventList() ;

	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_PAY_EVENT_LIST; }
	PacketSize_t getPacketSize() const ;
	std::string getPacketName() const  { return "GCPayEventList"; }
	std::string toString() const ;
	bool	isEmpty() { return m_ItemList.empty(); }

	ObjectID_t	getCount20() const { return m_Count20; }
	void		setCount20(ObjectID_t Count20) { m_Count20 = Count20; }

	ObjectID_t	getCount50() const { return m_Count50; }
	void		setCount50(ObjectID_t Count50) { m_Count50 = Count50; }

	ObjectID_t	getCount100() const { return m_Count100; }
	void		setCount100(ObjectID_t Count100) { m_Count100 = Count100; }

	ObjectID_t	getCount200() const { return m_Count200; }
	void		setCount200(ObjectID_t Count200) { m_Count200 = Count200; }

	ObjectID_t	getCount500() const { return m_Count500; }
	void		setCount500(ObjectID_t Count500) { m_Count500 = Count500; }

	ObjectID_t	getCount1000() const { return m_Count1000; }
	void		setCount1000(ObjectID_t Count1000) { m_Count1000 = Count1000; }

	ObjectID_t	getCount2000() const { return m_Count2000; }
	void		setCount2000(ObjectID_t Count2000) { m_Count2000 = Count2000; }

	ObjectID_t	getCount5000() const { return m_Count5000; }
	void		setCount5000(ObjectID_t Count5000) { m_Count5000 = Count5000; }
/////////////////////////////////status////////////////////////

	ObjectID_t	getStatus20() const { return m_Status20; }
	void		setStatus20(ObjectID_t Status20) { m_Status20 = Status20; }

	ObjectID_t	getStatus50() const { return m_Status50; }
	void		setStatus50(ObjectID_t Status50) { m_Status50 = Status50; }

	ObjectID_t	getStatus100() const { return m_Status100; }
	void		setStatus100(ObjectID_t Status100) { m_Status100 = Status100; }

	ObjectID_t	getStatus200() const { return m_Status200; }
	void		setStatus200(ObjectID_t Status200) { m_Status200 = Status200; }

	ObjectID_t	getStatus500() const { return m_Status500; }
	void		setStatus500(ObjectID_t Status500) { m_Status500 = Status500; }

	ObjectID_t	getStatus1000() const { return m_Status1000; }
	void		setStatus1000(ObjectID_t Status1000) { m_Status1000 = Status1000; }

	ObjectID_t	getStatus2000() const { return m_Status2000; }
	void		setStatus2000(ObjectID_t Status2000) { m_Status2000 = Status2000; }

	ObjectID_t	getStatus5000() const { return m_Status5000; }
	void		setStatus5000(ObjectID_t Status5000) { m_Status5000 = Status5000; }


public:
	void		addPayEventInfo( PayEventInfo* pGI ) { m_ItemList.push_back( pGI ); }
	PayEventInfo*	popPayEventInfo( ) { PayEventInfo* pRet = m_ItemList.front(); if ( pRet ) m_ItemList.pop_front(); return pRet; }

private:
	std::list<PayEventInfo*> m_ItemList;
	ObjectID_t	m_Count20;
	ObjectID_t	m_Count50;
	ObjectID_t	m_Count100;
	ObjectID_t	m_Count200;
	ObjectID_t	m_Count500;
	ObjectID_t	m_Count1000;
	ObjectID_t	m_Count2000;
	ObjectID_t	m_Count5000;

	ObjectID_t	m_Status20;
	ObjectID_t	m_Status50;
	ObjectID_t	m_Status100;
	ObjectID_t	m_Status200;
	ObjectID_t	m_Status500;
	ObjectID_t	m_Status1000;
	ObjectID_t	m_Status2000;
	ObjectID_t	m_Status5000;
};


//////////////////////////////////////////////////////////////////////////////
// class GCPayEventListFactory;
//////////////////////////////////////////////////////////////////////////////

class GCPayEventListFactory : public PacketFactory 
{
public :
	Packet* createPacket()  { return new GCPayEventList(); }
	std::string getPacketName() const  { return "GCPayEventList"; }
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_PAY_EVENT_LIST; }
	PacketSize_t getPacketMaxSize() const  
	{ 
		PacketSize_t size = szBYTE;
		size += PayEventInfo::getPacketMaxSize() * MAX_PAY_EVENT_LIST
			 + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID
			 + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID + szObjectID;

		return size;
	}
};


//////////////////////////////////////////////////////////////////////////////
// class GCPayEventListHandler;
//////////////////////////////////////////////////////////////////////////////

class GCPayEventListHandler 
{
public :
	static void execute(GCPayEventList* pPacket, Player* pPlayer) ;

};

#endif
