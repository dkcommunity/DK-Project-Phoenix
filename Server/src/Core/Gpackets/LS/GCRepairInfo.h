//////////////////////////////////////////////////////////////////////
// 
// Filename    : GCRepairInfo.h 
// Description : 
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GC_REPAIR_INFO_H__
#define __GC_REPAIR_INFO_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRepairInfo;
//
//////////////////////////////////////////////////////////////////////

class GCRepairInfo : public Packet {

public :
	virtual ~GCRepairInfo() throw() {}	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_REPAIR_INFO; }
	
	// get packet's body size
	PacketSize_t getPacketSize() const throw() { return szObjectID + szPrice * 2; }

	// get packet name
	string getPacketName() const throw() { return "GCRepairInfo"; }
	
	// get packet's debug string
	string toString() const throw();

	// get/set sender's creature id
	ObjectID_t GetItemOID() const throw() { return m_ItemOID; }
	void SetItemOID(const ObjectID_t & ItemOID) throw() { m_ItemOID = ItemOID; }

	Price_t GetRepairPrice() const throw() { return m_RepairPrice; }
	void SetRepairPrice(const Price_t & RepairPrice) throw() { m_RepairPrice = RepairPrice; }
	
	Price_t GetRepairDiscountPrice() const throw() { return m_RepairDiscountPrice; }
	void SetRepairDiscountPrice(const Price_t & RepairDiscountPrice) throw() { m_RepairDiscountPrice = RepairDiscountPrice; }

private :
	
	ObjectID_t m_ItemOID;

	Price_t m_RepairPrice;
	Price_t m_RepairDiscountPrice;
	
};


//////////////////////////////////////////////////////////////////////
//
// class GCRepairInfoFactory;
//
// Factory for GCRepairInfo
//
//////////////////////////////////////////////////////////////////////

class GCRepairInfoFactory : public PacketFactory {

public :
	
	// create packet
	Packet* createPacket() throw() { return new GCRepairInfo(); }

	// get packet name
	string getPacketName() const throw() { return "GCRepairInfo"; }
	
	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_REPAIR_INFO; }

	// get packet's max body size
	// *OPTIMIZATION HINT*
	// const static GCRepairInfoPacketMaxSize 를 정의, 리턴하라.
	PacketSize_t getPacketMaxSize() const throw() { return szObjectID + szPrice * 2 ; }

};


//////////////////////////////////////////////////////////////////////
//
// class GCRepairInfoHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRepairInfoHandler {
	
public :
	
	// execute packet's handler
	static void execute(GCRepairInfo* pPacket, Player* pPlayer) throw(ProtocolException, Error);

};

#endif
