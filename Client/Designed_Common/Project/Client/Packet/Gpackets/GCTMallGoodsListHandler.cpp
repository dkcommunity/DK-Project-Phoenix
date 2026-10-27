////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTMallGoodsListHandler.cpp
// Written By  : ±èŒº¹Î
// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTMallGoodsList.h"
#include "ClientDef.h"

#include "UIFunction.h"

void GCTMallGoodsListHandler::execute ( GCTMallGoodsList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
	__BEGIN_DEBUG

	UI_RunTMall(pPacket);

	__END_DEBUG
	__END_CATCH
}
