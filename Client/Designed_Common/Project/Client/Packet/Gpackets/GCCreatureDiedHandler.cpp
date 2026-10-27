//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCreatureDiedHandler.cpp
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCreatureDied.h"
#include "ClientDef.h"
#include "crash_trace_tool.h"

//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////
void GCCreatureDiedHandler::execute ( GCCreatureDied * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	// message

	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
	}
	//------------------------------------------------------
	// 정상.. 
	//------------------------------------------------------
	else
	{
		int creatureID = pPacket->getObjectID();

		MCreature* pCreature = g_pZone->GetCreature(creatureID);
		CT_BD("[CDIED] enter id=%d p=%p vamp=%d",
		      creatureID, (void*)pCreature,
		      (g_pPlayer != NULL && g_pPlayer->IsVampire()) ? 1 : 0);

		//--------------------------------------------------
		// 새로운 Creature이면 추가
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			// message
			DEBUG_ADD_FORMAT("[Error] Not Exist Creature ID=%d", pPacket->getObjectID());
		}
		else
		{
			CT_Done("[CDIED] pCreature=%p ID=%d alive=%d type=%d",
			           pCreature, (int)creatureID, (int)pCreature->IsAlive(),
			           (int)pCreature->GetCreatureType());
			//---------------------------------------------------------
			// Creature를 죽여야 한다.
			//---------------------------------------------------------
			//pCreature->SetDead();

			if (pCreature->IsAlive())
			{
				CT_Done("[CDIED] calling SetDead()");
				pCreature->SetDead();
				CT_Done("[CDIED] post SetDead alive=%d", (int)pCreature->IsAlive());
			}
			else
			{
				CT_Done("[CDIED] already dead, skip");
			//	pCreature->SetCorpse();
			}

			g_pZone->AddCorpseFromCreature( creatureID );
			CT_BD("[CDIED] after AddCorpse id=%d", creatureID);
		}
	}

#endif

	__END_CATCH
}
