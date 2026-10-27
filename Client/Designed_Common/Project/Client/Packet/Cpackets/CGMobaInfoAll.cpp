//////////////////////////////////////////////////////////////////////////////
// Filename    : CGMobaInfoAll.cpp
// Description : AOS/MOBA scoreboard request packet.
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGMobaInfoAll.h"

void CGMobaInfoAll::read(SocketInputStream& iStream)
{
	__BEGIN_TRY

	BYTE nameSize;
	iStream.read(nameSize);
	if (nameSize > 32)
		throw InvalidProtocolException("CGMobaInfoAll target name too large");
	if (nameSize > 0)
		iStream.read(m_TargetName, nameSize);
	else
		m_TargetName = "";

	__END_CATCH
}

void CGMobaInfoAll::write(SocketOutputStream& oStream) const
{
	__BEGIN_TRY

	if (m_TargetName.size() > 32)
		throw InvalidProtocolException("CGMobaInfoAll target name too large");

	BYTE nameSize = (BYTE)m_TargetName.size();
	oStream.write(nameSize);
	if (nameSize > 0)
		oStream.write(m_TargetName);

	__END_CATCH
}

void CGMobaInfoAll::execute(Player* pPlayer)
{
	__BEGIN_TRY

	CGMobaInfoAllHandler::execute(this, pPlayer);

	__END_CATCH
}

std::string CGMobaInfoAll::toString() const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "CGMobaInfoAll(TargetName:" << m_TargetName << ")";
	return msg.toString();

	__END_CATCH
}
