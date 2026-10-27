//////////////////////////////////////////////////////////////////////
//
// Filename    : GCNPCAskHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCNPCAsk.h"
#include "ClientDef.h"
#include "TalkBox.h"
#include "UIDialog.h"
#include "MNPCScriptTable.h"
#include "TempInformation.h"
#include "SystemAvailabilities.h"

//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////
void GCNPCAskHandler::execute ( GCNPCAsk * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	if (g_pPlayer==NULL
		|| g_pZone==NULL
		|| g_pNPCScriptTable==NULL
		|| g_pUIDialog==NULL
		|| g_pPCTalkBox==NULL)
	{
		DEBUG_ADD("[Error] Some Object is NULL");
		return;
	}

	g_pPCTalkBox->Release();
	int talkBoxType = PCTalkBox::NORMAL;

	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_NPC_ASK)
	{
		g_pPlayer->SetWaitVerifyNULL();

		int objectID = pPacket->getObjectID();
		
		// NPC
		MCreature* pCreature = g_pZone->GetCreature(objectID);
		int npcCreatureType = pPacket->getNPCID();
		unsigned int CreatureType;
		
		if(!objectID)
		{
			CreatureType = npcCreatureType;
			talkBoxType = PCTalkBox::GLOBAL_NPC;
		}
		
//		pPacket->

		else if (pCreature!=NULL || pPacket->getNPCID() >= 634 && pPacket->getNPCID() <= 806) //637 )
		{		

			if(pPacket->getNPCID() >= 634 && pPacket->getNPCID() <= 806) //637 )
			{
				CreatureType = npcCreatureType;
			} else
			{
				CreatureType= pCreature->GetCreatureType();
			}
			
			//---------------------------------------------------
			// g_PCTalkBox에 추가하면 된다.
			//---------------------------------------------------
			// 기존에 있던것 제거
			
		}
	
	unsigned int scriptID = pPacket->getScriptID();
	const char* content = g_pNPCScriptTable->GetSubject(scriptID, 0);
	unsigned int npcID = pPacket->getObjectID();

// 20081021 기존의 코드들이 반복되어서 함수화 시켜 한줄로 통합 안쪽에서 해결 보세
	g_pPCTalkBox->SetNPCTalkBox(talkBoxType, content, npcID, CreatureType,scriptID);

	}
	else
	{
		DEBUG_ADD("[Error] WaitVerifyMode is Not WAIT_VERIFY_NPC_ASK");
	}

	
#endif

	__END_CATCH
}
