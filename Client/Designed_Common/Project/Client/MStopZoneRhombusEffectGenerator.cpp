//----------------------------------------------------------------------
// MStopZoneRhombusEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MStopZoneRhombusEffectGenerator.h"
#include "MEffect.h"
#include "MTopView.h"
#include "MZone.h"
#include "MEffectSpriteTypeTable.h"

#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
//MStopZoneRhombusEffectGenerator	g_StopZoneRhombusEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MStopZoneRhombusEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	const int EFFECTSHAPE_SIZE		= 5;
	const int EFFECTSHAPE_SIZE_HALF = EFFECTSHAPE_SIZE/2;
	const int EffectShape[EFFECTSHAPE_SIZE][EFFECTSHAPE_SIZE] = {
		{ -1, -1, +2, -1, -1 },
		{ -1, +2, +1, +2, -1 },
		{ +2, +1, +0, +1, +2 },
		{ -1, +2, +1, +2, -1 },
		{ -1, -1, +2, -1, -1 }
	};

	bool bOK = false;
	//---------------------------------------------
	// pixel좌표를 Map의 좌표로 바꿔준다.
	//---------------------------------------------
	volatile int	sX, sY;
	sX = g_pTopView->PixelToMapX(egInfo.x0);
	sY = g_pTopView->PixelToMapY(egInfo.y0);
	
	for(int iy = 0; iy < EFFECTSHAPE_SIZE; ++iy)
		for(int ix = 0; ix < EFFECTSHAPE_SIZE; ++ix)
		{
			const int nOffsetIdx = EffectShape[iy][ix];
			if ( nOffsetIdx == -1 ) continue;
			//---------------------------------------------
			// Effect 생성
			//---------------------------------------------
			const int		NewIndex	= egInfo.effectSpriteType+nOffsetIdx;
			BLT_TYPE		bltType		= (*g_pEffectSpriteTypeTable)[NewIndex].BltType;
			TYPE_FRAMEID	frameID		= (*g_pEffectSpriteTypeTable)[NewIndex].FrameID;
			int				maxFrame	= g_pTopView->GetMaxEffectFrame(bltType, frameID);

			const POINT pt = { ix - EFFECTSHAPE_SIZE_HALF, iy - EFFECTSHAPE_SIZE_HALF };
			
			MEffect*	pEffect = new MEffect(bltType);

			pEffect->SetFrameID( frameID, maxFrame );
			pEffect->SetPosition(sX + pt.x, sY + pt.y);			// Sector 좌표						
			pEffect->SetZ(egInfo.z0);			
			pEffect->SetStepPixel(egInfo.step);		// 실제로 움직이지는 않지만, 다음 Effect를 위해서 대입해준다.	
			pEffect->SetCount( egInfo.count , egInfo.linkCount );			// 지속되는 Frame

			// 방향 설정
			pEffect->SetDirection( egInfo.direction );

			// 위력
			pEffect->SetPower(egInfo.power);

			// 빛의 밝기
			//pEffect->SetLight( light );

			// Zone에 추가한다.

			if (g_pZone->AddEffect( pEffect ) && pt.x == 0 && pt.y == 0)
			{
				// 다음 Effect 생성 정보
				if (egInfo.pEffectTarget == NULL)
				{
					pEffect->SetLink( egInfo.nActionInfo, NULL );
				}
				else
				{
					MEffectTarget*	pEffectTarget2 = new MEffectTarget(*egInfo.pEffectTarget);
					pEffect->SetLink( egInfo.nActionInfo, pEffectTarget2 );
				}
			}

		}


	return bOK;
}