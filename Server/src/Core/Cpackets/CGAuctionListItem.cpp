//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuctionListItem.cpp
// Description : Auction House item listing packet.
//////////////////////////////////////////////////////////////////////////////

#include "CGAuctionListItem.h"

CGAuctionListItem::CGAuctionListItem()
	throw()
	: m_ObjectID(0), m_Price(0)
{
}

void CGAuctionListItem::read(SocketInputStream& iStream)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	iStream.read(m_ObjectID);
	iStream.read(m_Price);
	__END_CATCH
}

void CGAuctionListItem::write(SocketOutputStream& oStream) const
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	oStream.write(m_ObjectID);
	oStream.write(m_Price);
	__END_CATCH
}

void CGAuctionListItem::execute(Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	CGAuctionListItemHandler::execute(this, pPlayer);
	__END_CATCH
}

string CGAuctionListItem::toString() const
	throw()
{
	__BEGIN_TRY
	StringStream msg;
	msg << "CGAuctionListItem(ObjectID:" << m_ObjectID << ", Price:" << m_Price << ")";
	return msg.toString();
	__END_CATCH
}
