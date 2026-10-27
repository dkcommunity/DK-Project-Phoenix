//////////////////////////////////////////////////////////////////////////////
// Filename    : CGCashChargeRequest.h
// Description : PayPal Cash yükleme isteği (client -> server).
//               Boş body — oyuncu kimliği socket bağlantısından alınır.
//               Server tek-kullanımlık token üretip PayPalTransactions'a
//               PENDING kayıt atar ve GCCashChargeURL ile checkout URL'i döner.
//               Tasarım: Dev_Log/Server/paypal_cash_charge_system_design_2026-08-20.md
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_CASH_CHARGE_REQUEST_H__
#define __CG_CASH_CHARGE_REQUEST_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"


class CGCashChargeRequest : public Packet
{
public:
    void read(SocketInputStream & iStream) throw(ProtocolException, Error);

    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_CG_CASH_CHARGE_REQUEST; }

	// get packet's body size — boş body (CGRequestPing örüntüsü)
	PacketSize_t getPacketSize() const throw() { return 0; }

	// get packet name
	string getPacketName() const throw() { return "CGCashChargeRequest"; }

	// get packet's debug string
	string toString() const throw() { return "CGCashChargeRequest"; }
};



class CGCashChargeRequestFactory : public PacketFactory {

public:

	// constructor
	CGCashChargeRequestFactory() throw() {}

	// destructor
	virtual ~CGCashChargeRequestFactory() throw() {}


public:

	// create packet
	Packet* createPacket() throw() { return new CGCashChargeRequest(); }

	// get packet name
	string getPacketName() const throw() { return "CGCashChargeRequest"; }

	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_CASH_CHARGE_REQUEST; }

	// get Packet Max Size — boş body
	PacketSize_t getPacketMaxSize() const throw() { return 0; }
};


class CGCashChargeRequestHandler {

public:

	// execute packet's handler
	static void execute(CGCashChargeRequest* pPacket, Player* pPlayer) throw(Error);

};

#endif
