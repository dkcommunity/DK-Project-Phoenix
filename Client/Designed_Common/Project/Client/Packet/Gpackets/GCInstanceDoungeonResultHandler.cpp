//////////////////////////////////////////////////////////////////////
//
// Filename    : GCInstanceDoungeonResultHandler.cpp
// Written By  : rappi76
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCInstanceDoungeonResult.h"

#include "VS_UI_INDOUN_NPC_DIALOG.h"
#include "VS_UI.h"

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
#if __CONTENTS(__TIPOJYU_CASTLE)
void GCInstanceDoungeonResultHandler::execute ( GCInstanceDoungeonResult * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
	//std::cout << pPacket->toString() << std::endl;
#endif

/*	MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

	if (pCreature!=NULL || pPacket->getNPCID() >= 634 && pPacket->getNPCID() <= 806) //637 )
	{		
		unsigned int CreatureType;

		if(pPacket->getNPCID() >= 634 && pPacket->getNPCID() <= 806) //637 )
		{
			CreatureType = pPacket->getNPCID();
		} else
		{
			CreatureType= pCreature->GetCreatureType();
		}
*/
		S_INDOUN_DLG_INFO Indoun_Dlg_Info;

/*		InstanceDoungeonInfo	*pDoungeon		= NULL;
		int contentSize = pPacket->getButtonSize();
			
		Indoun_Dlg_Info.Realese();
		for (int i = 0;		i < contentSize;	i++)
		{
			pDoungeon		= pPacket->getDoungeonInfo(i);
			Indoun_Dlg_Info.mapButtonNameList[pDoungeon->getDoungeonName()]	= pDoungeon->getDoungeonIndex();
		}
*/		
		char	charriToA[255];
		DWORD	dwValue;

		dwValue	= pPacket->getExp();
		memset(charriToA, NULL, sizeof(charriToA));
		wsprintf(charriToA, "%d", dwValue);
		Indoun_Dlg_Info.strExp				=	"Exp";
		Indoun_Dlg_Info.strExp				+=	charriToA;

		dwValue	= pPacket->getRankExp();
		memset(charriToA, NULL, sizeof(charriToA));
		wsprintf(charriToA, "%d", dwValue);
		Indoun_Dlg_Info.strRankExp			=	"RankExp";
		Indoun_Dlg_Info.strRankExp			+=	charriToA;

		dwValue	= pPacket->getRemainTime();
		memset(charriToA, NULL, sizeof(charriToA));
		wsprintf(charriToA, "%d", dwValue);
		Indoun_Dlg_Info.strDuration			=	"Duration";
		Indoun_Dlg_Info.strDuration			+=	charriToA;


		Indoun_Dlg_Info.strDlgTitleName		= pPacket->getTitleName();
//		Indoun_Dlg_Info.wButtonSize			= contentSize;

		gC_vs_ui.RunIndounNpcDlg(Indoun_Dlg_Info, COMPENSATION_DLG);

		//---------------------------------------------------
		// g_PCTalkBox에 추가하면 된다.
		//---------------------------------------------------
		// 기존에 있던것 제거
//	}

	__END_CATCH
}
#endif //__TIPOJYU_CASTLE
