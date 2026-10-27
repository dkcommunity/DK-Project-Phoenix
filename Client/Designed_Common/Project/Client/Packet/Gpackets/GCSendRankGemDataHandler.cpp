////////////////////////////////////////////////////////////////////////////////
// Filename    : GCSendRankGemDataHandler.cpp
// Written By  : rallser
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSendRankGemData.h"
#include "ClientDef.h"

#include "UIFunction.h"

void GCSendRankGemDataHandler::execute ( GCSendRankGemData * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 

		UI_RunMilitary(pPacket);

	__END_CATCH
}
