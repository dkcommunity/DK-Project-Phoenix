//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPetGamble.cpp 
// Written By  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "CGPetGamble.h"
#include "DarkAssert.h"


CGPetGamble::CGPetGamble () 
     
{
	__BEGIN_TRY
	__END_CATCH
}

CGPetGamble::~CGPetGamble() {
	__BEGIN_TRY
	__END_CATCH
}

void CGPetGamble::read (SocketInputStream & iStream) 
	 
{
	__BEGIN_TRY
		
	__END_CATCH
}

void CGPetGamble::write (SocketOutputStream & oStream) const 
     
{
	__BEGIN_TRY

	__END_CATCH
}

void CGPetGamble::execute (Player* pPlayer) 
	 
{
	__BEGIN_TRY

#ifndef __GAME_CLIENT__
	CGPetGambleHandler::execute (this , pPlayer);
#endif
		
	__END_CATCH
}

#ifdef __DEBUG_OUTPUT__
std::string CGPetGamble::toString () 
	const 
{
	__BEGIN_TRY
		
	StringStream msg;
	msg << "CGPetGamble()";
	return msg.toString();

	__END_CATCH
}
#endif
