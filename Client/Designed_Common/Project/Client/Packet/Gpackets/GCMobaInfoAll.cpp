//////////////////////////////////////////////////////////////////////////////
// Filename    : GCMobaInfoAll.cpp
// Description : AOS/MOBA team scoreboard packet.
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCMobaInfoAll.h"

void GCMobaInfoAll::read(SocketInputStream& iStream)
{
	__BEGIN_TRY

	BYTE count;
	iStream.read(count);
	if (count > 10)
		throw InvalidProtocolException("GCMobaInfoAll member count too large");

	m_Members.clear();
	for (int i = 0; i < count; ++i)
	{
		Member member;
		BYTE nameSize;
		iStream.read(nameSize);
		if (nameSize > 32)
			throw InvalidProtocolException("GCMobaInfoAll name too large");
		if (nameSize > 0)
			iStream.read(member.Name, nameSize);
		iStream.read(member.Body);
		iStream.read(member.Hair);
		iStream.read(member.Level);
		iStream.read(member.KillPlayer);
		iStream.read(member.Dead);
		iStream.read(member.KillMonster);
		iStream.read(member.TeamCode);
		iStream.read(member.ResultCode);
		iStream.read(member.RaceCode);
		m_Members.push_back(member);
	}

	__END_CATCH
}

void GCMobaInfoAll::write(SocketOutputStream& oStream) const
{
	__BEGIN_TRY

	if (m_Members.size() > 10)
		throw InvalidProtocolException("GCMobaInfoAll member count too large");

	BYTE count = (BYTE)m_Members.size();
	oStream.write(count);
	for (uint i = 0; i < m_Members.size(); ++i)
	{
		const Member& member = m_Members[i];
		if (member.Name.size() > 32)
			throw InvalidProtocolException("GCMobaInfoAll name too large");

		BYTE nameSize = (BYTE)member.Name.size();
		oStream.write(nameSize);
		if (nameSize > 0)
			oStream.write(member.Name);
		oStream.write(member.Body);
		oStream.write(member.Hair);
		oStream.write(member.Level);
		oStream.write(member.KillPlayer);
		oStream.write(member.Dead);
		oStream.write(member.KillMonster);
		oStream.write(member.TeamCode);
		oStream.write(member.ResultCode);
		oStream.write(member.RaceCode);
	}

	__END_CATCH
}

void GCMobaInfoAll::execute(Player* pPlayer)
{
	__BEGIN_TRY

	GCMobaInfoAllHandler::execute(this, pPlayer);

	__END_CATCH
}

PacketSize_t GCMobaInfoAll::getPacketSize() const
{
	__BEGIN_TRY

	PacketSize_t size = szBYTE;
	for (uint i = 0; i < m_Members.size(); ++i)
	{
		size += szBYTE + m_Members[i].Name.size();
		size += szBYTE + szBYTE;
		size += szuint * 5;
		size += szBYTE + szBYTE;
	}
	return size;

	__END_CATCH
}

std::string GCMobaInfoAll::toString() const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "GCMobaInfoAll(Count:" << (int)m_Members.size() << ")";
	return msg.toString();

	__END_CATCH
}
