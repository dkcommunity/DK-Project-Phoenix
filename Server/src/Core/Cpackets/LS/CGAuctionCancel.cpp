//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionCancel.cpp
// Description : Auction House cancel request packet.
//////////////////////////////////////////////////////////////////////////////

#include "CGAuctionCancel.h"

CGAuctionCancel::CGAuctionCancel()
	throw()
	: m_ListingID(0)
{
}

void CGAuctionCancel::read(SocketInputStream& iStream)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	iStream.read(m_ListingID);
	__END_CATCH
}

void CGAuctionCancel::write(SocketOutputStream& oStream) const
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	oStream.write(m_ListingID);
	__END_CATCH
}

void CGAuctionCancel::execute(Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	CGAuctionCancelHandler::execute(this, pPlayer);
	__END_CATCH
}

string CGAuctionCancel::toString() const
	throw()
{
	__BEGIN_TRY
	StringStream msg;
	msg << "CGAuctionCancel(ListingID:" << m_ListingID << ")";
	return msg.toString();
	__END_CATCH
}
