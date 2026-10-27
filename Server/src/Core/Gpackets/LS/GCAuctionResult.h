//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuctionResult.h
// Description : Auction House operation result packet.
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_AUCTION_RESULT_H__
#define __GC_AUCTION_RESULT_H__

#include "Packet.h"
#include "PacketFactory.h"

enum AuctionResultCode
{
	AUCTION_RESULT_OK = 0,
	AUCTION_RESULT_LISTED = 1,
	AUCTION_RESULT_BOUGHT = 2,
	AUCTION_RESULT_CANCELLED = 3,
	AUCTION_RESULT_ERROR = 100
};

class GCAuctionResult : public Packet
{
public:
	GCAuctionResult() throw();

	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	void execute(Player* pPlayer) throw(ProtocolException, Error);

	PacketID_t getPacketID() const throw() { return PACKET_GC_AUCTION_RESULT; }
	PacketSize_t getPacketSize() const throw() { return szBYTE + szBYTE + m_Message.size(); }
	string getPacketName() const throw() { return "GCAuctionResult"; }
	string toString() const throw();

	BYTE getResultCode() const throw() { return m_ResultCode; }
	void setResultCode(BYTE resultCode) throw() { m_ResultCode = resultCode; }
	const string& getMessage() const throw() { return m_Message; }
	void setMessage(const string& message) throw() { m_Message = message; }

private:
	BYTE m_ResultCode;
	string m_Message;
};

class GCAuctionResultFactory : public PacketFactory
{
public:
	Packet* createPacket() throw() { return new GCAuctionResult(); }
	string getPacketName() const throw() { return "GCAuctionResult"; }
	PacketID_t getPacketID() const throw() { return Packet::PACKET_GC_AUCTION_RESULT; }
	PacketSize_t getPacketMaxSize() const throw() { return szBYTE + szBYTE + 128; }
};

class GCAuctionResultHandler
{
public:
	static void execute(GCAuctionResult* pPacket, Player* pPlayer) throw(Error);
};

#endif
