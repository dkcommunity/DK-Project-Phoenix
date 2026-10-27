//////////////////////////////////////////////////////////////////////////////
// Filename    : GCCreateItem.h 
// Written By  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_CREATE_ITEM_H__
#define __GC_CREATE_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"
#include "PCItemInfo.h"

//////////////////////////////////////////////////////////////////////////////
// class GCCreateItem;
//////////////////////////////////////////////////////////////////////////////

class GCCreateItem : public Packet 
{
public:
	GCCreateItem() throw();
	virtual ~GCCreateItem() throw() {}
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);
	PacketID_t getPacketID() const throw() { return PACKET_GC_CREATE_ITEM; }
	PacketSize_t getPacketSize() const throw() 
	{ 
		PacketSize_t packetSize;
		
		packetSize = szCoordInven * 2 +		// 아이템 X, Y 좌표
			m_PCItemInfo.getSize() +		// 아이템 정보
			szObjectID;    					// 인벤토리 오브젝트 아이디
		
		return packetSize;
	}
	string getPacketName() const throw() { return "GCCreateItem"; }
	string toString() const throw();

public:
	CoordInven_t getInvenX() const throw() { return m_InvenX; }
	void setInvenX(CoordInven_t InvenX) throw() { m_InvenX = InvenX; }

	CoordInven_t getInvenY() const throw() { return m_InvenY; }
	void setInvenY(CoordInven_t InvenY) throw() { m_InvenY = InvenY; }
	
	void SetPCItemInfo(PCItemInfo& pcItemInfo) { m_PCItemInfo = pcItemInfo; }
	PCItemInfo* GetPCItemInfo() { return &m_PCItemInfo; }

	// 2007 03 29 Add
	ObjectID_t getInventoryItemObjectID() const throw() { return m_InventoryItemObjectID; }
	void setInventoryItemObjectID(ObjectID_t InventoryItemObjectID) throw() { m_InventoryItemObjectID = InventoryItemObjectID; }

private:
	CoordInven_t   		m_InvenX;       // 인벤토리 X 좌표
	CoordInven_t   		m_InvenY;       // 인벤토리 Y 좌표
	
	PCItemInfo m_PCItemInfo;
	
	// 2007.03.29 Add
	ObjectID_t			m_InventoryItemObjectID;		// 보조 인벤토리인지 확인하기 위하여 추가
};


//////////////////////////////////////////////////////////////////////////////
// class GCCreateItemFactory;
//////////////////////////////////////////////////////////////////////////////

class GCCreateItemFactory : public PacketFactory 
{
public:
	Packet* createPacket() throw() { return new GCCreateItem(); }
	string getPacketName() const throw() { return "GCCreateItem"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_CREATE_ITEM; }
	PacketSize_t getPacketMaxSize() const throw() 
	{ 
		PacketSize_t packetSize = 0;
		
		packetSize = szCoordInven +		// 아이템 X 좌표
			szCoordInven +				// 아이템 Y 좌표
			szObjectID;					// 인벤토리 오브젝트 아이디
		packetSize +=	PCItemInfo::getMaxSize();
		
		return 	packetSize;
	}
};


//////////////////////////////////////////////////////////////////////////////
// class GCCreateItemHandler;
//////////////////////////////////////////////////////////////////////////////

class GCCreateItemHandler 
{
public:
	static void execute(GCCreateItem* pPacket, Player* pPlayer) throw(Error);

};

#endif
