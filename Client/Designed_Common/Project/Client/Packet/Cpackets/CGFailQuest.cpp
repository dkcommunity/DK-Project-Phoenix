// Filename    : CGFailQuest.cpp 
// Written By  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGFailQuest.h"


void CGFailQuest::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY

	iStream.read(m_bFail);

	__END_CATCH
}

void CGFailQuest::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	oStream.write(m_bFail);
		
	__END_CATCH
}

void CGFailQuest::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY
		
#ifndef __GAME_CLIENT__
	CGFailQuestHandler::execute(this , pPlayer);
#endif

	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGFailQuest::toString () const
       
{
	__BEGIN_TRY
		
	StringStream msg;
    msg << "CGFailQuest("
		<< ")" ;
	return msg.toString();

	__END_CATCH
}
#endif
