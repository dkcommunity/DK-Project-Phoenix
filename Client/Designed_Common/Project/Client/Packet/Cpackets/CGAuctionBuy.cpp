//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionBuy.cpp
// Description : Auction House buy request packet.
//////////////////////////////////////////////////////////////////////////////

#include "CGAuctionBuy.h"

CGAuctionBuy::CGAuctionBuy()
	throw()
	: m_ListingID(0)
{
}

void CGAuctionBuy::read(SocketInputStream& iStream)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	iStream.read(m_ListingID);
	__END_CATCH
}

void CGAuctionBuy::write(SocketOutputStream& oStream) const
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	oStream.write(m_ListingID);
	__END_CATCH
}

void CGAuctionBuy::execute(Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	CGAuctionBuyHandler::execute(this, pPlayer);
	__END_CATCH
}

string CGAuctionBuy::toString() const
	throw()
{
	__BEGIN_TRY
	StringStream msg;
	msg << "CGAuctionBuy(ListingID:" << m_ListingID << ")";
	return msg.toString();
	__END_CATCH
}
