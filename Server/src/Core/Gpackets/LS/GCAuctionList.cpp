//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAuctionList.cpp
// Description : Auction House item list packet.
//////////////////////////////////////////////////////////////////////////////

#include "GCAuctionList.h"
#include "Assert.h"

GCAuctionList::GCAuctionList()
	throw()
{
	__BEGIN_TRY
	m_Entries.clear();
	__END_CATCH
}

GCAuctionList::~GCAuctionList()
	throw()
{
	__BEGIN_TRY
	clearEntries();
	__END_CATCH
}

void GCAuctionList::clearEntries()
	throw()
{
	__BEGIN_TRY
	std::list<Entry*>::iterator itr = m_Entries.begin();
	std::list<Entry*>::iterator endItr = m_Entries.end();
	for (; itr != endItr; ++itr)
		delete *itr;
	m_Entries.clear();
	__END_CATCH
}

void GCAuctionList::read(SocketInputStream& iStream)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	BYTE count;
	iStream.read(count);
	if (count > MAX_AUCTION_LIST)
		throw InvalidProtocolException("GCAuctionList count too large");

	clearEntries();
	for (int i = 0; i < count; ++i)
	{
		Entry* pEntry = new Entry();
		Assert(pEntry != NULL);

		iStream.read(pEntry->ListingID);
		iStream.read(pEntry->Price);
		iStream.read(pEntry->SellerRace);
		iStream.read(pEntry->ItemRace);

		BYTE nameSize;
		iStream.read(nameSize);
		if (nameSize > 32)
			throw InvalidProtocolException("GCAuctionList seller name too large");
		if (nameSize > 0)
			iStream.read(pEntry->SellerName, nameSize);

		pEntry->ItemInfo.read(iStream);
		m_Entries.push_back(pEntry);
	}

	__END_CATCH
}

void GCAuctionList::write(SocketOutputStream& oStream) const
	throw(ProtocolException, Error)
{
	__BEGIN_TRY

	if (m_Entries.size() > MAX_AUCTION_LIST)
		throw InvalidProtocolException("GCAuctionList count too large");

	BYTE count = (BYTE)m_Entries.size();
	oStream.write(count);

	std::list<Entry*>::const_iterator itr = m_Entries.begin();
	std::list<Entry*>::const_iterator endItr = m_Entries.end();
	for (; itr != endItr; ++itr)
	{
		const Entry* pEntry = *itr;
		Assert(pEntry != NULL);

		if (pEntry->SellerName.size() > 32)
			throw InvalidProtocolException("GCAuctionList seller name too large");

		oStream.write(pEntry->ListingID);
		oStream.write(pEntry->Price);
		oStream.write(pEntry->SellerRace);
		oStream.write(pEntry->ItemRace);

		BYTE nameSize = (BYTE)pEntry->SellerName.size();
		oStream.write(nameSize);
		if (nameSize > 0)
			oStream.write(pEntry->SellerName);

		pEntry->ItemInfo.write(oStream);
	}

	__END_CATCH
}

void GCAuctionList::execute(Player* pPlayer)
	throw(ProtocolException, Error)
{
	__BEGIN_TRY
	GCAuctionListHandler::execute(this, pPlayer);
	__END_CATCH
}

PacketSize_t GCAuctionList::getPacketSize() const
	throw()
{
	__BEGIN_TRY
	PacketSize_t size = szBYTE;
	std::list<Entry*>::const_iterator itr = m_Entries.begin();
	std::list<Entry*>::const_iterator endItr = m_Entries.end();
	for (; itr != endItr; ++itr)
	{
		if (*itr != NULL)
			size += (*itr)->getPacketSize();
	}
	return size;
	__END_CATCH
}

string GCAuctionList::toString() const
	throw()
{
	__BEGIN_TRY
	StringStream msg;
	msg << "GCAuctionList(Count:" << (int)m_Entries.size() << ")";
	return msg.toString();
	__END_CATCH
}
