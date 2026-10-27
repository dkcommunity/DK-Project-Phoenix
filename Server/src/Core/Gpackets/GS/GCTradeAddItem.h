////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeAddItem.h 
// Written By  : 김성민
// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_ADD_ITEM_H__
#define __GC_TRADE_ADD_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "PCItemInfo.h"

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeAddItem;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeAddItem : public Packet 
{
public:

	GCTradeAddItem() throw();
	~GCTradeAddItem() throw();

public:

	void read(SocketInputStream & iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_TRADE_ADD_ITEM; }
	PacketSize_t getPacketSize() const throw() 
	{ 
		PacketSize_t size = 0;
		size += szObjectID;                           // m_TargetObjectID
		size += szCoordInven;                         // m_X
		size += szCoordInven;                         // m_Y
		size +=	m_PCItemInfo.getSize();

		return size;

	}

	string getPacketName() const throw() { return "GCTradeAddItem"; }
	string toString() const throw();

public:

	ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id) throw() { m_TargetObjectID = id; }

	CoordInven_t getX() const throw() { return m_X; }
	void setX(CoordInven_t x) throw() { m_X = x; }

	CoordInven_t getY() const throw() { return m_Y; }
	void setY(CoordInven_t y) throw() { m_Y = y; }
	
	void SetPCItemInfo(PCItemInfo& pcItemInfo) { m_PCItemInfo = pcItemInfo; }
	PCItemInfo* GetPCItemInfo() { return &m_PCItemInfo; }

private:
	ObjectID_t         m_TargetObjectID;  // 교환을 하고 있는 상대방의 OID
	CoordInven_t       m_X;               // 인벤토리에서의 X 좌표
	CoordInven_t       m_Y;               // 인벤토리에서의 Y 좌표
	
	PCItemInfo m_PCItemInfo;

};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeAddItemFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeAddItemFactory : public PacketFactory 
{
public:

	Packet* createPacket() throw() { return new GCTradeAddItem(); }
	string getPacketName() const throw() { return "GCTradeAddItem"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_TRADE_ADD_ITEM; }
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		PacketSize_t size = 0;
		size += szObjectID;                   // m_TargetObjectID
		size += szCoordInven;                 // m_X
		size += szCoordInven;                 // m_Y
		size +=	PCItemInfo::getMaxSize();
		
		return size;

	}
};



////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeAddItemHandler;
//
////////////////////////////////////////////////////////////////////////////////
class GCTradeAddItemHandler 
{
public:

	static void execute(GCTradeAddItem* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif

