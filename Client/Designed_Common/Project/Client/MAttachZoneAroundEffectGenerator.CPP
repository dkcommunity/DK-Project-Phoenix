//----------------------------------------------------------------------
// MAttachZoneEffectGenerator.cpp
//----------------------------------------------------------------------
// Tile과 맞붙은 Effect들을 생성한다.
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MAttachZoneAroundEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"
#include "MEventManager.h"

bool
MAttachZoneAroundEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	// 2026-05-11 audit: NULL/OOB guard + memory leak fix
	if (g_pTopView == NULL || g_pZone == NULL || g_pEffectSpriteTypeTable == NULL)
		return false;

	int est = egInfo.effectSpriteType;
	if (est < 0 || (size_t)est >= g_pEffectSpriteTypeTable->GetSize())
		return false;

	//POINT pixelPoint = { egInfo.x0, egInfo.y0 };
	TYPE_SECTORPOSITION tX,tY;

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;
	
	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
	tX = g_pTopView->PixelToMapX( egInfo.x1 );
	tY = g_pTopView->PixelToMapY( egInfo.y1 );

	// 2004, 11, 3, sobeit add start
	if(est == EFFECTSPRITETYPE_GREAT_RUFFIAN_1_AXE_GROUND)
	{
		int TempX[8][2] = { {0,0},	{0,7},	{-7,7},	{-7,0},	{0,0},	{-7,0},	{-7,7},	{0,7} };
		int TempY[8][2] = { {-7,7},	{-7,0},	{0,0},	{0,-7},	{-7,7},	{0,7},	{0,0},	{7,0} };
		
		for(int i = 0; i<3; i++)
		{
			MEffect*		pEffect;
			pEffect = new MEffect(bltType);
			pEffect->SetFrameID( frameID, maxFrame );
			pEffect->SetStepPixel(egInfo.step);		// 실제로 움직이지는 않지만, 다음 Effect를 위해서 대입해준다.
			if(i>0)
				pEffect->SetPosition( tX + TempX[egInfo.direction][i-1], tY + TempY[egInfo.direction][i-1]);
			else
				pEffect->SetPosition( tX, tY );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetDirection( egInfo.direction );
			pEffect->SetPower( egInfo.power );

			if (g_pZone->AddGroundEffect( pEffect ))
			{
				if (i == 0)
				{
					pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );
				}
				else
				{
					MEffectTarget *pTarget = egInfo.pEffectTarget;
					if( pTarget != NULL )
					{
						MEffectTarget *pEffectTarget = new MEffectTarget( *pTarget );
						pEffect->SetLink( egInfo.nActionInfo, pEffectTarget );
					}
				}
			}
			else
			{
				// 2026-05-20 DISABLED: 2026-05-12 commit 045040e3 yanlis "leak fix" eklenmis.
				// MZone::AddEffect false donerken zaten kendisi delete pNewEffect yapiyor
				// (MZone.cpp:4098 CanAddEffect fail + MZone.cpp:4130 outer zone fail).
				// Ek delete = double-free -> MEffect::~MEffect garbage member dereference -> AV
				// (BLOOD_DROP runtime crash 2026-05-20 empirical).
				// delete pEffect;
			}
		}
		MEvent event;
		event.eventID = EVENTID_METEOR_SHAKE;
		event.eventType = EVENTTYPE_ZONE;
		event.eventDelay = 500;
		event.eventFlag = EVENTFLAG_SHAKE_SCREEN;
		event.parameter3 = 3;
		g_pEventManager->AddEvent(event);
	
		return true;
	}
	// 2004, 11, 3, sobeit add end
	
	for(int y=-1;y<=1;y++)
	{
		for(int x=-1;x<=1;x++)
		{
			if( x == 0 && y == 0 )
				continue;

			MEffect*		pEffect;
			BYTE Direction;
			if( y == -1)
				Direction = 6-x;
			else
			if( y == 0)
				Direction = max(0,x) * 4;
			else
				Direction = x+2;

			pEffect = new MEffect(bltType);
			pEffect->SetFrameID( frameID, maxFrame );
			pEffect->SetStepPixel(egInfo.step);		// 실제로 움직이지는 않지만, 다음 Effect를 위해서 대입해준다.
			pEffect->SetPosition( tX + x, tY + y );
//			pEffect->SetPixelPosition( egInfo.x0 + x * 24 , egInfo.y0 + y * 12 , egInfo.z0);
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetDirection( Direction );
			pEffect->SetPower( egInfo.power );

			if( g_pZone->AddGroundEffect( pEffect ) )
			{
				pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );
			}
			else
			{
				// 2026-05-20 DISABLED: 2026-05-12 commit 045040e3 yanlis "leak fix" eklenmis.
				// MZone::AddEffect false donerken zaten kendisi delete pNewEffect yapiyor
				// (MZone.cpp:4098 CanAddEffect fail + MZone.cpp:4130 outer zone fail).
				// Ek delete = double-free -> MEffect::~MEffect garbage member dereference -> AV
				// (BLOOD_DROP runtime crash 2026-05-20 empirical).
				// delete pEffect;
			}
		}
	}

	return false;
}

