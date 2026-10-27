//----------------------------------------------------------------------
// MAttackCreatureEffectGenerator.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MAttackCreatureEffectGenerator.h"
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
//MAttackCreatureEffectGenerator	g_AttackCreatureEffectGenerator;

//----------------------------------------------------------------------
// Generate
//----------------------------------------------------------------------
bool
MAttackCreatureEffectGenerator::Generate( const EFFECTGENERATOR_INFO& egInfo )
{
	BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].BltType;
	TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[egInfo.effectSpriteType].FrameID;

	bool  bShady = false;
	bool  bfireball = false;

	// creature의 좌표
	int cx, cy, cz, effectCount = 0, arrivedelay = 0;	

	if(egInfo.nActionInfo == SKILL_SHADY_DOUPLE)
	{
		bShady = true;
		effectCount = 3;
	}
	if(egInfo.nActionInfo == SKILL_SHADY_DOUPLE_2)
	{
		bShady = true;
		effectCount = 1;
	}
	if(egInfo.nActionInfo == SKILL_ADV2_INFINITY_BLAZE)
	{
		bfireball = true;
		effectCount = 4;
	}
	if( egInfo.nActionInfo == STEP_SKILL_KASAS_ARROW_2 || egInfo.nActionInfo == STEP_SKILL_BLAZE_BOLT_2 ||
		egInfo.nActionInfo == STEP_SKILL_EARTHS_TEETH_2 )
		effectCount = 1;
	else
	if( egInfo.nActionInfo == STEP_SKILL_KASAS_ARROW_3 || egInfo.nActionInfo == STEP_SKILL_BLAZE_BOLT_3 ||
		egInfo.nActionInfo == STEP_SKILL_EARTHS_TEETH_3 )
		effectCount = 2;

	if( egInfo.nActionInfo == SKILL_HETER_CHAKRAM || egInfo.nActionInfo == SKILL_HETER_CHAKRAM_CHAIN )
		arrivedelay = 3;

	// 목표 위치 Pixel좌표
	MCreature* pCreature = g_pZone->GetCreature( egInfo.creatureID );

	if (pCreature == NULL)
	{
		return false;
	}

	//-----------------------------------
	// [ TEST CODE ]
	//-----------------------------------
	//if (pCreature->GetID() != g_pPlayer->GetID())
	//{
	//	pCreature->PacketSpecialAction( nActionInfo, g_pPlayer->GetX(), g_pPlayer->GetY(), g_pPlayer->GetZ(), g_pPlayer->GetID());
	//}
	
	// Creture의 좌표로 목표좌표를 설정한다.
	cx = g_pTopView->MapToPixelX( pCreature->GetX() );
	cy = g_pTopView->MapToPixelY( pCreature->GetY() );
	cz = pCreature->GetZ();

	//MGuidanceEffect* pEffect = new MGuidanceEffect(bltType);	

	MGuidanceEffect* pEffect = new MGuidanceEffect(bltType);	
	
	int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);
	
	pEffect->SetFrameID( frameID, maxFrame );		// 0번 Effect, Max 3 Frame					

	// 발사 위치 Pixel좌표	
	pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0 );

	pEffect->SetDirection( egInfo.direction );
	
	// 목표 Creature
	pEffect->SetTraceCreatureID( egInfo.creatureID );
	if(bShady == true)
	{
	   pEffect->SetStepPixel( 35 );	  //	  例뱁錦릿
	}
	else if(bfireball == true)
	{
		pEffect->SetStepPixel(100);
	}
	else
	{
	pEffect->SetStepPixel( egInfo.step );
	}

	// 지속되는 Frame (목표가 있다면 별로 관계 없음 - -;)
	pEffect->SetCount( egInfo.count, egInfo.linkCount );

	// 위력
	pEffect->SetPower(egInfo.power);

	// 빛의 밝기
	//pEffect->SetLight( light );

	// 도착 후 잠시 대기하는 시간
	pEffect->SetArriveDelay( arrivedelay );

	if (g_pZone->AddEffect( pEffect ))
	{
		pEffect->SetLink( egInfo.nActionInfo, egInfo.pEffectTarget );

		if( effectCount == 0)
			return true;
	} else
		return false;
	if( effectCount > 0 && bShady == false && bfireball == false)
	{
		int z=-12;
		while(effectCount)
		{
			MGuidanceEffect* pEffect = new MGuidanceEffect(bltType);
			pEffect->SetFrameID( frameID, maxFrame );

//			if( egInfo.direction == 5 )
//			{
//				if(!( egInfo.nActionInfo == STEP_SKILL_EARTHS_TEETH_2 || egInfo.nActionInfo == STEP_SKILL_EARTHS_TEETH_3) )
//					pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0 + z );
//			} else
			if( egInfo.direction == 2 || egInfo.direction == 6 )
			{
				pEffect->SetPixelPosition( egInfo.x0+z, egInfo.y0, egInfo.z0 );
			} else
				pEffect->SetPixelPosition( egInfo.x0, egInfo.y0, egInfo.z0 - z );
			z+=24;
			pEffect->SetDirection( egInfo.direction );
			pEffect->SetTraceCreatureID( egInfo.creatureID );
			pEffect->SetStepPixel( egInfo.step );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetPower( egInfo.power );

			pEffect->SetArriveDelay( arrivedelay );

			if( g_pZone->AddEffect( pEffect ) )
			{
				if( egInfo.pEffectTarget != NULL )
				{
					MEffectTarget *pTarget = new MEffectTarget ( *egInfo.pEffectTarget );								
					pEffect->SetLink( egInfo.nActionInfo, pTarget );				
				}
			} else
				return false;
			effectCount--;
		}
		return true;
	}

	if( bShady == true)
	{
		int z=0;
		while(effectCount)
		{
			MGuidanceEffect* pEffect = new MGuidanceEffect(bltType);
			pEffect->SetFrameID( frameID, maxFrame );
		    if(effectCount ==2)
			{
				z = 20;
			}
			else if(effectCount == 1)
			{
				z = -20;
			}
			if( egInfo.direction == 2 || egInfo.direction == 6 )
			{
				pEffect->SetPixelPosition( egInfo.x0-z, egInfo.y0, egInfo.z0 );
			} 
			else
			{
			pEffect->SetPixelPosition( egInfo.x0-z, egInfo.y0-z, egInfo.z0);
			}
			
			pEffect->SetDirection( egInfo.direction );
			pEffect->SetTraceCreatureID( egInfo.creatureID );
			pEffect->SetStepPixel( 20 );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetPower( egInfo.power );
		//	pEffect->SetDelayFrame(timenum);
			pEffect->SetArriveDelay( 1 );

			if( g_pZone->AddEffect( pEffect) )
			{
				if( egInfo.pEffectTarget != NULL )
				{
					MEffectTarget *pTarget = new MEffectTarget ( *egInfo.pEffectTarget );								
					pEffect->SetLink( egInfo.nActionInfo, pTarget );				
				}
			} else
				return false;
			effectCount--;
		}
		return true;
	}

	if( bfireball == true)
	{
		while(effectCount)
		{
			MGuidanceEffect* pEffect = new MGuidanceEffect(bltType);
			pEffect->SetFrameID( frameID, maxFrame );
			int z=0;
			if(effectCount == 4)
			{
				pEffect->SetPixelPosition( egInfo.x0, egInfo.y0-20, egInfo.z0 );
			}
			if(effectCount == 3)
			{
				pEffect->SetPixelPosition( egInfo.x0+20, egInfo.y0, egInfo.z0 );
			}

			if(effectCount == 2)
			{
				pEffect->SetPixelPosition( egInfo.x0-20, egInfo.y0, egInfo.z0 );
			}
			if(effectCount == 1)
			{
				pEffect->SetPixelPosition( egInfo.x0, egInfo.y0+20, egInfo.z0 );
			}
			pEffect->SetDirection( egInfo.direction );
			pEffect->SetTraceCreatureID( egInfo.creatureID );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetStepPixel( 18 );
			pEffect->SetPower( egInfo.power );
			pEffect->SetArriveDelay( 1 );

			if( g_pZone->AddEffect( pEffect) )
			{
				if( egInfo.pEffectTarget != NULL )
				{
					MEffectTarget *pTarget = new MEffectTarget ( *egInfo.pEffectTarget );								
					pEffect->SetLink( egInfo.nActionInfo, pTarget );				
				}
			} 
		else
		{
			return false;
		}
		effectCount--;
		}
	/*	while(effectCount)
		{
			MGuidanceEffect* pEffect = new MGuidanceEffect(bltType);
			pEffect->SetFrameID( frameID, maxFrame );
			if(effectCount ==2)
			{
				z = 20;
			}
			else if(effectCount == 1)
			{
				z = -20;
			}
			if( egInfo.direction == 2 || egInfo.direction == 6 )
			{
				pEffect->SetPixelPosition( egInfo.x0-z, egInfo.y0, egInfo.z0 );
			} 
			else
			{
				pEffect->SetPixelPosition( egInfo.x0-z, egInfo.y0-z, egInfo.z0);
			}
			pEffect->SetDirection( egInfo.direction );
			pEffect->SetTraceCreatureID( egInfo.creatureID );
			pEffect->SetStepPixel( 1 );
			pEffect->SetCount( egInfo.count, egInfo.linkCount );
			pEffect->SetPower( egInfo.power );
			pEffect->SetArriveDelay( 1 );

			if( g_pZone->AddEffect( pEffect) )
			{
				if( egInfo.pEffectTarget != NULL )
				{
					MEffectTarget *pTarget = new MEffectTarget ( *egInfo.pEffectTarget );								
					pEffect->SetLink( egInfo.nActionInfo, pTarget );				
				}
			} else
				return false;
			effectCount--;
		}	*/
		return true;
	}
	return false;
}
