//////////////////////////////////////////////////////////////////////////////
// Filename    : GCOtherStoreInfo.h 
// Written By  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_OTHER_STORE_INFO_H__
#define __GC_OTHER_STORE_INFO_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "StoreInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class GCOtherStoreInfo;
//////////////////////////////////////////////////////////////////////////////

class GCOtherStoreInfo : public Packet
{
public:
	GCOtherStoreInfo()   : m_pInfo(NULL)  { }
	virtual ~GCOtherStoreInfo() ;

public:
	void read(SocketInputStream & iStream) ;
	void write(SocketOutputStream & oStream) const ;
	void execute(Player* pPlayer) ;
	PacketID_t getPacketID() const  { return PACKET_GC_OTHER_STORE_INFO; }
	PacketSize_t getPacketSize() const  { return szObjectID + m_pInfo->getSize(true) + szBYTE; }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "GCOtherStoreInfo"; }
	std::string toString() const ;
#endif
public:
	ObjectID_t	getObjectID() const { return m_ObjectID; }
	void		setObjectID(ObjectID_t oid) { m_ObjectID = oid; }

	BYTE		isRequested() const { return m_IsRequested; }
	void		setRequested(BYTE flag) { m_IsRequested = flag; }
	
	StoreInfo*	getStoreInfo() const { return m_pInfo; }
	void		setStoreInfo(StoreInfo* pInfo ) { m_pInfo = pInfo; }

private:
	ObjectID_t	m_ObjectID;
	BYTE		m_IsRequested;
	StoreInfo*	m_pInfo;
};

//////////////////////////////////////////////////////////////////////////////
// class GCOtherStoreInfoFactory;
//////////////////////////////////////////////////////////////////////////////

class GCOtherStoreInfoFactory : public PacketFactory 
{
public:
	Packet* createPacket()  { return new GCOtherStoreInfo(); }
#ifdef __DEBUG_OUTPUT__
	std::string getPacketName() const  { return "GCOtherStoreInfo"; }
#endif
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_OTHER_STORE_INFO; }
	PacketSize_t getPacketMaxSize() const 
	{
		return szObjectID + StoreInfo::getMaxSize() + szBYTE;
	}
};

//////////////////////////////////////////////////////////////////////////////
// class GCOtherStoreInfoHandler;
//////////////////////////////////////////////////////////////////////////////

class GCOtherStoreInfoHandler 
{
public:
	static void execute(GCOtherStoreInfo* pPacket, Player* pPlayer) ;
};

#endif
