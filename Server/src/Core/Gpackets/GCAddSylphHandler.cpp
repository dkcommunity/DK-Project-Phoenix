//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddSylphHandler.cc
// Written By  : rappi76
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GCAddSylph.h"

void GCAddSylphHandler::execute ( GCAddSylph * pPacket , Player * pPlayer )
	 throw ( Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_CLIENT__

	//cout << pPacket->toString() << endl;
	
#elif __WINDOWS__

	AfxMessageBox( pPacket->toString().c_str() );

#endif

	__END_DEBUG_EX __END_CATCH
}
