//////////////////////////////////////////////////////////////////////////////
// Filename    : CGSkillToObject.cpp 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "CGSkillToObject.h"
#include "SocketEncryptInputStream.h"
#include "SocketEncryptOutputStream.h"
#include "DarkAssert.h"

CGSkillToObject::CGSkillToObject ()   
{
	__BEGIN_TRY
	m_CEffectID		= 0;
	m_ActionStep	= 0;
	__END_CATCH
}

CGSkillToObject::~CGSkillToObject() {
	__BEGIN_TRY
	__END_CATCH
}

void CGSkillToObject::read (SocketInputStream & iStream)  
{
	__BEGIN_TRY
		
#ifdef __USE_ENCRYPTER__

	SocketEncryptInputStream* pEIStream = dynamic_cast<SocketEncryptInputStream*>(&iStream);
    Assert(pEIStream!=NULL);

	if (pEIStream->getEncryptCode()!=0)
	{

		SHUFFLE_STATEMENT_4(pEIStream->getEncryptCode(),
							pEIStream->readEncrypt(m_SkillType),
							pEIStream->readEncrypt(m_CEffectID),
							pEIStream->readEncrypt(m_TargetObjectID),
							pEIStream->readEncrypt(m_ActionStep));
	}
	else
#endif
	{
		iStream.read((char*)&m_SkillType , szSkillType);
		iStream.read((char*)&m_CEffectID , szCEffectID);
		iStream.read((char*)&m_TargetObjectID , szObjectID);
		iStream.read((char*)&m_ActionStep, szBYTE);
	}
	__END_CATCH
}

void CGSkillToObject::write (SocketOutputStream & oStream) const 
{
	__BEGIN_TRY

#ifdef __USE_ENCRYPTER__
	SocketEncryptOutputStream* pEOStream = dynamic_cast<SocketEncryptOutputStream*>(&oStream);
    Assert(pEOStream!=NULL);

	if (pEOStream->getEncryptCode()!=0)
	{

		SHUFFLE_STATEMENT_4(pEOStream->getEncryptCode(),
							pEOStream->writeEncrypt(m_SkillType),
							pEOStream->writeEncrypt(m_CEffectID),
							pEOStream->writeEncrypt(m_TargetObjectID),
							pEOStream->writeEncrypt(m_ActionStep));
	}
	else
#endif
	{
		oStream.write((char*)&m_SkillType , szSkillType);
		oStream.write((char*)&m_CEffectID , szCEffectID);
		oStream.write((char*)&m_TargetObjectID , szObjectID);
		oStream.write((char*)&m_ActionStep, szBYTE);
	}
	__END_CATCH
}

void CGSkillToObject::execute (Player* pPlayer)  
{
	__BEGIN_TRY
#ifndef __GAME_CLIENT__
		CGSkillToObjectHandler::execute(this , pPlayer);
#endif 
	__END_CATCH
}

std::string CGSkillToObject::toString () const 
{
	__BEGIN_TRY

	StringStream msg;
    msg << "CGSkillToObject("
		<< "SkillType:"      << (int)m_SkillType 
		<< ",CEffectID:"     << (int)m_CEffectID 
		<< ",TargetSelfID :" << (int)m_TargetObjectID 
		<< ",ActionStep:"    << (int)m_ActionStep
		<< ")" ;
	return msg.toString();
	__END_CATCH
}

