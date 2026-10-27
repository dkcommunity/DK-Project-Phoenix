//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCashChargeURL.h
// Written By  : Phoenix (2026-08-20)
// Description : PayPal Cash Charge — server'in checkout URL cevabi (550).
//               String serilestirme GCShowWebMarket oruntusunun aynasi
//               (szBYTE uzunluk + char[]).
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CASH_CHARGE_URL_H__
#define __GC_CASH_CHARGE_URL_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCCashChargeURL;
//
//////////////////////////////////////////////////////////////////////

class GCCashChargeURL : public Packet {

public :

    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read(SocketInputStream & iStream) ;

    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write(SocketOutputStream & oStream) const ;

	// execute packet's handler
	void execute(Player* pPlayer) ;

	// get packet id
	PacketID_t getPacketID() const  { return PACKET_GC_CASH_CHARGE_URL; }

	// get packet's body size
	PacketSize_t getPacketSize() const
	{
		return szBYTE +			// Result
			   szBYTE +			// URL length
			   m_URL.size();	// URL
	}

	// get packet name
	std::string getPacketName() const  { return "GCCashChargeURL"; }

	// get packet's debug std::string
	std::string toString() const ;

	// get/set Result — 0=OK, 1=kapali, 2=cok fazla PENDING, 3=sistem hatasi
	BYTE getResult() const  { return m_Result; }
	void setResult( BYTE result )  { m_Result = result; }

	// get/set URL — result==0 iken dolu, hata cevabinda bos olabilir
	const std::string& getURL() const  { return m_URL; }
	void setURL( const std::string url )  { m_URL = url; }

private :
	// Result code
	BYTE m_Result;

	// Checkout URL
	std::string m_URL;
};


//////////////////////////////////////////////////////////////////////
//
// class GCCashChargeURLFactory;
//
//////////////////////////////////////////////////////////////////////

class GCCashChargeURLFactory : public PacketFactory {

public :

	// create packet
	Packet* createPacket()  { return new GCCashChargeURL(); }

	// get packet name
	std::string getPacketName() const  { return "GCCashChargeURL"; }

	// get packet id
	PacketID_t getPacketID() const  { return Packet::PACKET_GC_CASH_CHARGE_URL; }

	// get packet's max body size
	PacketSize_t getPacketMaxSize() const
	{
		return szBYTE +		// Result
			   szBYTE +		// URL length
			   255;			// URL (BYTE uzunluk — maks 255)
	}

};


//////////////////////////////////////////////////////////////////////
//
// class GCCashChargeURLHandler;
//
//////////////////////////////////////////////////////////////////////

class GCCashChargeURLHandler {

public :

	// execute packet's handler
	static void execute(GCCashChargeURL* pPacket, Player* pPlayer) ;

};

#endif
