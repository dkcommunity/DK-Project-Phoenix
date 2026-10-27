////////////////////////////////////////////////////////////////////////////////
// Filename    : GCSendNewWarInfoHandler.cpp
// Written By  : ±èŒº¹Î
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSendNewWarInfo.h"
#include "ClientDef.h"

#include "UIFunction.h"

void GCSendNewWarInfoHandler::execute ( GCSendNewWarInfo * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
	
	UI_RunHollyWar(pPacket);

	__END_CATCH
}
