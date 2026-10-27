////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeMoney.h 
// Written By  : 김성민
// Description : 
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TRADE_MONEY_H__
#define __CG_TRADE_MONEY_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
// 교환 코드
////////////////////////////////////////////////////////////////////////////////

enum
{
	// 교환할 돈의 액수를 늘린다.
	CG_TRADE_MONEY_INCREASE = 0,

	// 교환할 돈의 액수를 줄인다.
	CG_TRADE_MONEY_DECREASE
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeMoney;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeMoney : public Packet 
{
public:
	void read ( SocketInputStream & iStream ) ;
	void write ( SocketOutputStream & oStream ) const ;
	void execute ( Player * pPlayer ) ;
	PacketID_t getPacketID () const  { return PACKET_CG_TRADE_MONEY; }
	PacketSize_t getPacketSize () const  { return szObjectID + szGold + szBYTE; }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGTradeMoney"; }
		std::string toString () const ;
	#endif
	
public:
	ObjectID_t getTargetObjectID() const  { return m_TargetObjectID; }
	void setTargetObjectID(ObjectID_t id)  { m_TargetObjectID = id; }

	Gold_t getAmount() const  { return m_Gold; }
	void setAmount(Gold_t gold)  { m_Gold = gold; }

	BYTE getCode() const  { return m_Code; }
	void setCode(BYTE code)  { m_Code = code; }

private:
	ObjectID_t m_TargetObjectID; // 교환을 원하는 상대방의 ObjectID
	Gold_t     m_Gold;           // 원하는 액수
	BYTE       m_Code;           // 코드

};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeMoneyFactory;
//
////////////////////////////////////////////////////////////////////////////////
#ifdef __DEBUG_OUTPUT__
class CGTradeMoneyFactory : public PacketFactory {
public:
	Packet * createPacket ()  { return new CGTradeMoney(); }

	#ifdef __DEBUG_OUTPUT__
		std::string getPacketName () const  { return "CGTradeMoney"; }
	#endif

	PacketID_t getPacketID () const  { return Packet::PACKET_CG_TRADE_MONEY; }
	PacketSize_t getPacketMaxSize () const  { return szObjectID + szGold + szBYTE; }
};
#endif

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeMoneyHandler;
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_CLIENT__
	class CGTradeMoneyHandler 
	{
	public:
		static void execute ( CGTradeMoney * pPacket , Player * player ) ;
		static void executeSlayer ( CGTradeMoney * pPacket , Player * player ) ;
		static void executeVampire ( CGTradeMoney * pPacket , Player * player ) ;
		static void executeError ( CGTradeMoney * pPacket , Player * player, BYTE ErrorCode ) ;
	};
#endif

#endif
