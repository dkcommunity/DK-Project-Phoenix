//////////////////////////////////////////////////////////////////////
//
// Filename    : CGCashChargeRequest.h
// Written By  : Phoenix (2026-08-20)
// Description : PayPal Cash Charge — TMall "Recharge" butonu bu paketi
//               gonderir (bos body). Server oturum + one-time key uretip
//               GCCashChargeURL (550) ile checkout URL doner.
//               Fiyat/paket bilgisi ASLA client'tan gitmez.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_CASH_CHARGE_REQUEST_H__
#define __CG_CASH_CHARGE_REQUEST_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGCashChargeRequest : public Packet
{
public:
    void read(SocketInputStream & iStream) ;

    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_CG_CASH_CHARGE_REQUEST; }

	// get packet's body size — bos body (sablon: CGRequestPing)
	PacketSize_t getPacketSize() const  { return 0; }

	// get packet name
	std::string getPacketName() const  { return "CGCashChargeRequest"; }

	// get packet's debug std::string
	std::string toString() const  { return "CGCashChargeRequest()"; }
};



class CGCashChargeRequestFactory : public PacketFactory {

public:

	// constructor
	CGCashChargeRequestFactory()  {}

	// destructor
	virtual ~CGCashChargeRequestFactory() {}


public:

	// create packet
	Packet* createPacket()  { return new CGCashChargeRequest(); }

	// get packet name
	std::string getPacketName() const  { return "CGCashChargeRequest"; }

	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_CG_CASH_CHARGE_REQUEST; }

	// get Packet Max Size — bos body
	PacketSize_t getPacketMaxSize() const  { return 0; }
};


class CGCashChargeRequestHandler {

public:

	// execute packet's handler
	static void execute(CGCashChargeRequest* pPacket, Player* pPlayer) ;

};

#endif
