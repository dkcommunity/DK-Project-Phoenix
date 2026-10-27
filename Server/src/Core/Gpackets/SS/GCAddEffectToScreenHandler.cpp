//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddEffectToScreenHandler.cpp
// Written By  : rappi@darkeden.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCAddEffectToScreen.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddEffectToScreenHandler::execute ( GCAddEffectToScreen * pGCAddEffectToScreen , Player * pPlayer )
	 throw ( Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_CLIENT__

	//cout << pGCAddEffectToScreen->toString() << endl;
	
#elif __WINDOWS__

#endif

	__END_DEBUG_EX __END_CATCH
}
