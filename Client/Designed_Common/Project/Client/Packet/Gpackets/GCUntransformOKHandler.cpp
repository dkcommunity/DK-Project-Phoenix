//--------------------------------------------------------------------------------
//
// Filename    : GCUntransformOKHandler.cpp
// Written By  : elca, Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCUntransformOK.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "MVampireGear.h"
#include "UserInformation.h"
#include "EffectSpriteTypeDef.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCUntransformOKHandler::execute ( GCUntransformOK * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	bool bWerWolf = g_pPlayer->GetCreatureType() == CREATURETYPE_WER_WOLF;

#if __CONTENTS(__FAST_TRANSFORTER)
	bool bFlitterMouse = g_pPlayer->GetCreatureType () == CREATURETYPE_FLITTERMOUSE;
	bool bNede = g_pPlayer->GetCreatureType () == CREATURETYPE_NEDE;
	bool bKltl = g_pPlayer->GetCreatureType () == CREATURETYPE_KLTL;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
	bool bShapeOfDemon = g_pPlayer->GetCreatureType () == CREATURETYPE_SHAPE_OF_DEMON;
#endif //__SECOND_TRANSFORTER
	//------------------------------------------------------------------
	// Player가 기다리던 skill의 성공유무를 검증받았다.
	//------------------------------------------------------------------
	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_SKILL_SUCCESS)
	{		
		g_pPlayer->SetWaitVerifyNULL();
	}
	else
	{
		DEBUG_ADD("[Error] Player is not WaitVerifySkillSuccess");
	}

	int x = pPacket->getX();
	int y = pPacket->getY();
	int dir = pPacket->getDir();

	int creatureType = g_pPlayer->IsMale()?CREATURETYPE_VAMPIRE_MALE1:CREATURETYPE_VAMPIRE_FEMALE1;

	MItem *pItem = g_pVampireGear->GetItem(MVampireGear::GEAR_VAMPIRE_COAT);
//	_MinTrace("Adasd2:%d %d\n",creatureType,g_pPlayer->IsMale());

	bool male = g_pPlayer->IsMale();
	TYPE_ITEMTYPE itemType = ITEMTYPE_NULL;

	if(pItem != NULL)
	{
		itemType = pItem->GetItemType();
	} 
	else
	{
		itemType = male;
	}

	creatureType = GetVampireCreatureType(SHAPE_NORMAL, male, itemType);

	if (g_pPlayer->GetCompetence()==0)
	{
		creatureType = CREATURETYPE_VAMPIRE_OPERATOR;
	}
	
	MActionResult* pResult = new MActionResult;

	pResult->Add( new MActionResultNodeChangeCreatureType( g_pPlayer->GetID(), creatureType ) );

	//--------------------------------------------------
	// 뱀파로 돌아간다.
	//--------------------------------------------------								
	ExecuteActionInfoFromMainNode(
		RESULT_MAGIC_UN_TRANSFORM,										// 사용 기술 번호
	
		x, y, 0,
		dir,
		
		OBJECTID_NULL,												// 목표에 대한 정보
		x, y, 0, 
		
		0,													// 기술의 (남은) 지속 시간		
		
		pResult, //NULL,
		
		false);			// 기술 첨부터 시작한다.

	g_pPlayer->SetDelay( 1000 );


	g_pPlayer->SetServerPosition( x, y );
	g_pPlayer->MovePosition( x, y );
	g_pPlayer->SetDirection( dir );
	g_pPlayer->SetCurrentDirection( dir );
#if __CONTENTS(__FAST_TRANSFORTER || __SECOND_TRANSFORTER)
	//데몬, 플리터마우스 이던간에 일단 인간형으로 왔을때임 따라서 인간형으로 세팅한다.
	g_pPlayer->SetWingType(0);
#endif //__FAST_TRANSFORTER

	// 승직 뱀파이어가 변신을 풀었을 때 고스트 이팩트가 있으면 지운다
	if(g_pPlayer->IsVampire() && g_pPlayer->IsAdvancementClass())
	{
		for(int i = 0; i < 6; ++i)
			g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_AC_VAMPIRE_GHOST_MOVE_BLACK + i);
	}
#if __CONTENTS(__FAST_TRANSFORTER)
	// 신규이동수단 이펙트가 있으면 지운다.
	if (bFlitterMouse)
	{
		for (int j = 0; j < 12; j += 2)
			g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_VAMPIRE_WING_MOVE_ORANGE + j);
	}
	if (bNede)
	{
		//for (int j = 0; j < 12; j += 2)
			g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_unknow_2328);
		//g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_VAMPIRE_WING_MOVE_ORANGE + j);
	}
	//if (bKltl)
	//{
	//	//for (int j = 0; j < 12; j += 2)
	//	g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_unknow_2695);
	//	//g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_VAMPIRE_WING_MOVE_ORANGE + j);
	//}
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
	if (bShapeOfDemon)
	{
		for (int j = 0; j < 8; j ++)
			g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_VAMPIRE_SHAPE_OF_DEMON_MOVE_WHITE + j);
	}
#endif //__SECOND_TRANSFORTER
	if( bWerWolf )
	{
		g_pPlayer->SetBodyColor1( g_pUserInformation->SkinColor );
		if( pItem != NULL )
			g_pPlayer->SetBodyColor2( pItem->GetItemOptionColorSet() );
		else
			g_pPlayer->SetBodyColor2( 377 );
	}
	

#endif

	__END_CATCH
}
