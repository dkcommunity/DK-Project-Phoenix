//////////////////////////////////////////////////////////////////////////////
// Filename    : GCOtherModifyInfoHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GCOTHERGEARINFO.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCOTHERGEARINFOHandler::execute ( GCOTHERGEARINFO * pGCOTHERGEARINFO , Player * pPlayer )
	 throw ( Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#if __TEST_CLIENT__

	//cout << pGCOtherModifyInfo->toString() << endl;
	
#elif __WINDOWS__

	AfxMessageBox( pGCOTHERGEARINFO->toString().c_str() );

#endif

	__END_DEBUG_EX __END_CATCH
}
