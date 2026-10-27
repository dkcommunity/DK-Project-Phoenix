//----------------------------------------------------------------------
// MStopZoneRectRandomPixelEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MStopZoneRectRandomPixelEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MCreature.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"
#include "UserInformation.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MStopZoneRectRandomPixelEffectGenerator	g_StopZoneRectRandomPixelEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MStopZoneRectRandomPixelEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	const int RECT_WIDTH		= 3;
	const int RECT_HEIGHT		= 3;

	const int RECT_WIDTH_HALF	= RECT_WIDTH / 2;
	const int RECT_HEIGHT_HALF	= RECT_HEIGHT / 2;
	
	const int RECT_SIZE			= RECT_WIDTH * RECT_HEIGHT;


	//------------------------------------------------------------
	// 이번에는 시작좌표(sx,sy)에 Effect를 출력하고
	// 다음에는 (tx,ty)에 Effect를 출력하게 된다.
	// MEffectTarget을 수정하면 가능하다.
	//------------------------------------------------------------
	MEffectTarget* pTarget = egInfo.pEffectTarget;

	// 시작 좌표
	int startPixelX = egInfo.x0;
	int startPixelY = egInfo.y0;
	int startPixelZ = 0;//egInfo.z0;

	// 목표 좌표
	int targetPixelX = egInfo.x1; 
	int targetPixelY = egInfo.y1;
	int targetPixelZ = 0;//egInfo.z1;

	int est = egInfo.effectSpriteType;

	TYPE_SECTORPOSITION	targetSectorX, targetSectorY;
	targetSectorX = g_pTopView->PixelToMapX( targetPixelX );
	targetSectorY = g_pTopView->PixelToMapY( targetPixelY );

	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[est].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[est].FrameID;

	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

	int randWidth		= TILE_X * RECT_WIDTH;
	int randHeight		= TILE_Y * RECT_HEIGHT;
	int randWidthHalf	= randWidth / 2;
	int randHeightHalf	= randHeight / 2;

	int DelayCnt		= 0;

	int AddEffectCnt[RECT_HEIGHT][RECT_WIDTH] = {0, };

	for(int i = 0 ; i < 12; ++i)
	{
		const int step = i / RECT_SIZE;
		
		int randX, randY;

		do {
			randX = rand() % RECT_WIDTH;
			randY = rand() % RECT_HEIGHT;
		} while(AddEffectCnt[randY][randX] > step);

		++AddEffectCnt[randX][randY];

		TYPE_SECTORPOSITION newSectorX, newSectorY;
		newSectorX = targetSectorX + randX - RECT_WIDTH_HALF;
		newSectorY = targetSectorY + randY - RECT_HEIGHT_HALF;
		
		int newRandomPixelX, newRandomPixelY;
		newRandomPixelX = g_pTopView->MapToPixelX(newSectorX) + rand() % TILE_X - TILE_X_HALF;
		newRandomPixelY = g_pTopView->MapToPixelY(newSectorY) + rand() % TILE_Y - TILE_Y_HALF;

		//---------------------------------------------
		// Effect 생성
		//---------------------------------------------
		MEffect* pEffect = new MEffect(bltType);

		//pEffect->SetPosition(x, y);		// Sector 좌표(의미 없을 듯)

		pEffect->SetPixelPosition(newRandomPixelX, newRandomPixelY, targetPixelZ);		// pixel 좌표		
		
		// 방향 설정
		pEffect->SetDirection( egInfo.direction );

		// 딜레이
		pEffect->SetDelayFrame(++DelayCnt);

		pEffect->SetFrameID( frameID, maxFrame );

		//pEffect->SetZ( sz );			
		//pEffect->SetStepPixel(egInfo.step);		// 실제로 움직이지는 않지만, 다음 Effect를 위해서 대입해준다.
		pEffect->SetCount( egInfo.count+DelayCnt, egInfo.linkCount );			// 지속되는 Frame
		
		// 위력
		pEffect->SetPower(egInfo.power);
		
		g_pZone->AddEffect( pEffect );

//		if (g_pZone->AddEffect( pEffect ))
// 		{
// 			// 다음 Effect 생성 정보
// 			if (egInfo.pEffectTarget == NULL)
// 			{
// 				pEffect->SetLink( egInfo.nActionInfo, NULL );
// 			}
// 			else
// 			{
// 				MEffectTarget* pEffectTarget = new MEffectTarget(*egInfo.pEffectTarget);
// 				pEffect->SetLink( egInfo.nActionInfo, pEffectTarget );
// 				//pEffectTarget2->Set( x, y, egInfo.z0, egInfo.creatureID );
// 			}
// 			
// 		}
	}

	return true;
}
