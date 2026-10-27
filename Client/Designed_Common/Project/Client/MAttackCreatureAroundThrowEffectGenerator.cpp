//----------------------------------------------------------------------
// MAttackCreatureAroundThrowEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MAttackCreatureAroundThrowEffectGenerator.h"
#include "MGuidanceEffect.h"
#include "MTopView.h"
#include "MCreature.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugInfo.h"
#include "SkillDef.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//#include "MPlayer.h"
//extern	MPlayer	g_pPlayer->

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MAttackCreatureAroundThrowEffectGenerator	g_AttackCreatureAroundThrowEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------

enum { TYPE_MONSTER, TYPE_SLAYER, TYPE_VAMPIRE, TYPE_OUSTERS, TYPE_MAX };

bool
MAttackCreatureAroundThrowEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	bool bOK = false;

	int est = egInfo.effectSpriteType;

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;
	
	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);		

	MEffectTarget* pTarget = egInfo.pEffectTarget;

	// 시작 좌표
	int startPixelX = egInfo.x0;
	int startPixelY = egInfo.y0;

	//---------------------------------------------
	// pixel좌표를 Map의 좌표로 바꿔준다.
	//---------------------------------------------
	TYPE_SECTORPOSITION	startSectorX, startSectorY;
	startSectorX = g_pTopView->PixelToMapX( startPixelX );
	startSectorY = g_pTopView->PixelToMapY( startPixelY );

	//------------------------------------------------------------
	// 목표까지가는게 아니라.. 일정한 pixel수 만큼 가야한다?
	//------------------------------------------------------------
	// 한 단계 이동..
	for (int iy = 0; iy < 5; ++iy)
		for(int ix = 0; ix < 5; ++ix)
		{
			int targetSectorX = startSectorX + ix - 2;
			int targetSectorY = startSectorY + iy - 2;

			TYPE_OBJECTID targetCreatureID = g_pZone->GetCreatureID(targetSectorX, targetSectorY);

			if(targetCreatureID == OBJECTID_NULL) continue;

			int movePixel = egInfo.step;// * egInfo.count;
														
			//---------------------------------------------
			// 나가는 방향을 결정한다.
			//---------------------------------------------
			BYTE moveDirection = g_pTopView->GetDirectionToPosition(
				startSectorX, startSectorY, targetSectorX, targetSectorY);

			//---------------------------------------------
			// (x,y)를 다시 pixel좌표로 바꾼다.
			//---------------------------------------------
			int targetPixelX = g_pTopView->MapToPixelX(targetSectorX);
			int targetPixelY = g_pTopView->MapToPixelY(targetSectorY);

			//---------------------------------------------
			// Effect 생성
			//---------------------------------------------
			MGuidanceEffect* pEffect = new MGuidanceEffect(bltType);

			pEffect->SetFrameID(frameID, maxFrame);

			pEffect->SetPixelPosition(startPixelX, startPixelY, 0);
			pEffect->SetTraceCreatureID(targetCreatureID);
			//pEffect->SetTarget(targetPixelX, targetPixelY, 0, egInfo.step);

			pEffect->SetStepPixel(egInfo.step);							// 실제로 움직이지는 않지만, 다음 Effect를 위해서 대입해준다.
			pEffect->SetCount(egInfo.count, egInfo.linkCount);			// 지속되는 Frame

			// 방향 설정
			pEffect->SetDirection(moveDirection);

			// 위력
			pEffect->SetPower(egInfo.power);

			// 빛의 밝기
			//pEffect->SetLight( light );

			if (g_pZone->AddEffect(pEffect))
			{
				MEffectTarget* pEffectTarget = new MEffectTarget((*g_pActionInfoTable)[egInfo.nActionInfo].GetSize());
				pEffect->SetLink(egInfo.nActionInfo, pEffectTarget);
				pEffectTarget->NewEffectID();
				pEffectTarget->Set(targetPixelX, targetPixelY, 0, targetCreatureID);
			}			
		}



	return bOK;
}
