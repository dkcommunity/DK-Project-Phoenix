//////////////////////////////////////////////////////////////////////////////
// Filename    : GCOtherModifyInfoHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCOtherGearInfo.h"
#include "ClientDef.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCOtherGearInfoHandler::execute ( GCOtherGearInfo * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
	////------------------------------------------------------
	//// Zone이 아직 생성되지 않은 경우
	////------------------------------------------------------
	//if (g_pZone==NULL)
	//{
	//	// message
	//	DEBUG_ADD("[Error] Zone is Not Init.. yet.");
	//	
	//}	
	////------------------------------------------------------
	//// 정상.. 
	////------------------------------------------------------

	//MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());

	//if (pCreature==NULL)
	//{
	//	DEBUG_ADD_FORMAT("[Error] the Creature is Not Exist. id=%d", pPacket->getObjectID());			
	//}
	//else
	//{
	//	SetGearInfo(pPacket->getGearInfo(), NULL, 0, true);
	//	UI_RunOtherInfo(pCreature);
	//}


	__END_CATCH
}
