#include "Client_PCH.h"
#include "GCPayEventList.h"
#include "ClientDef.h"

#include "UIFunction.h"

void GCPayEventListHandler::execute ( GCPayEventList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 

	UI_RunPayEvent(pPacket);

	__END_CATCH
}
