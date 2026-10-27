//////////////////////////////////////////////////////////////////////////////
// Filename    : GCCashChargeURL.h
// Description : PayPal Cash yükleme checkout URL'i (server -> client).
//               CGCashChargeRequest'e cevaptır: result + BYTE-uzunluklu URL.
//               String serileştirme GCShowWebMarket örüntüsünün aynasıdır
//               (szBYTE uzunluk + ham byte'lar); hata dönüşünde boş URL
//               legaldir (GCAuctionResult örüntüsü).
//               Tasarım: Dev_Log/Server/paypal_cash_charge_system_design_2026-08-20.md
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_CASH_CHARGE_URL_H__
#define __GC_CASH_CHARGE_URL_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

// Sonuç kodları — onaylanmış tasarım: 0=OK, 1=hata (boş URL döner)
enum CashChargeResultCode
{
	CASH_CHARGE_RESULT_OK	= 0,
	CASH_CHARGE_RESULT_ERROR	= 1
};

class GCCashChargeURL : public Packet {

public :

	GCCashChargeURL() throw();

    void read(SocketInputStream & iStream) throw(ProtocolException, Error);

    void write(SocketOutputStream & oStream) const throw(ProtocolException, Error);

	// execute packet's handler
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	// get packet id
	PacketID_t getPacketID() const throw() { return PACKET_GC_CASH_CHARGE_URL; }

	// get packet's body size
	PacketSize_t getPacketSize() const throw()
	{
		return szBYTE +			// Result
			   szBYTE +			// URL length
			   m_URL.size();	// URL
	}

	// get packet name
	string getPacketName() const throw() { return "GCCashChargeURL"; }

	// get packet's debug string
	string toString() const throw();

	// get/set Result
	BYTE getResult() const throw() { return m_Result; }
	void setResult( BYTE result ) throw() { m_Result = result; }

	// get/set URL
	const string& getURL() const throw() { return m_URL; }
	void setURL( const string& url ) throw() { m_URL = url; }

private :

	// Sonuç kodu (CashChargeResultCode)
	BYTE m_Result;

	// Checkout URL'i — hata durumunda boş string
	string m_URL;
};


class GCCashChargeURLFactory : public PacketFactory {

public :

	// create packet
	Packet* createPacket() throw() { return new GCCashChargeURL(); }

	// get packet name
	string getPacketName() const throw() { return "GCCashChargeURL"; }

	// get packet id
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_CASH_CHARGE_URL; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const throw()
	{
		return szBYTE +		// Result
			   szBYTE +		// URL length
			   255;			// URL (BYTE uzunluk sınırı)
	}

};


class GCCashChargeURLHandler {

public :

	// execute packet's handler
	static void execute(GCCashChargeURL* pPacket, Player* pPlayer) throw(Error);

};

#endif
