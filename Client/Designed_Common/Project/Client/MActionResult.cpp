//----------------------------------------------------------------------
// MActionResult.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MActionResult.h"
#include "MTopView.h"
#include "ClientDef.h"
#include "MEffectGeneratorTable.h"
#include "UIFunction.h"
#include "MCreatureTable.h"
#include "VS_UI.h"
#include "MEventManager.h"
#include "MItemOptionTable.h"

#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

#define SAFE_DELETE(x)		{ if(x!=NULL) delete x; x=NULL; }

extern int g_MorphCreatureType;

// [»õ±â¼ú]
void		SkillShadowDancing(MCreature* pUserCreature, MCreature* pTargetCreature, int skillID);

//----------------------------------------------------------------------
// 
// ActionResultNode :: constructor/destructor
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//
// ActionResultNode - ActionInfo
//
//----------------------------------------------------------------------
MActionResultNodeActionInfo::MActionResultNodeActionInfo(TYPE_ACTIONINFO nActionInfo, TYPE_OBJECTID uid, TYPE_OBJECTID tid, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY, DWORD delayFrame, BYTE temp)
{ 
	m_UserID		= uid;
	m_TargetID		= tid;
	m_nActionInfo	= nActionInfo;
	m_DelayFrame	= delayFrame;

	m_sX			= sX;
	m_sY			= sY;

	m_Temp			= temp;
}

//----------------------------------------------------------------------
// ActionResultNode - ActionInfo::Execute
//----------------------------------------------------------------------
void
MActionResultNodeActionInfo::Execute()
{
	DEBUG_ADD_FORMAT("ActionResultNodeAIExecute. u=%d, t=%d", m_UserID, m_TargetID);
	///*
	//POINT point;
	if (g_pZone==NULL)
	{
		return;
	}


	//--------------------------------------------------------
	// ¸ñÇ¥ À§Ä¡ PixelÁÂÇ¥
	//--------------------------------------------------------
	MCreature*	pTargetCreature = g_pZone->GetCreature( m_TargetID );
	MCreature*	pUserCreature	= g_pZone->GetCreature( m_UserID );

	// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ³¡~
	if (pTargetCreature==NULL)// || pUserCreature==NULL)
	{
		//MItem* pItem = g_pZone->GetItem( m_TargetID );	
		
		return;		
	}

	TYPE_ACTIONINFO nUsedActionInfo = m_nActionInfo;

/*
	point = MTopView::MapToPixel(pTargetCreature->GetX(), pTargetCreature->GetY());

	
	if ((*g_pActionInfoTable)[nUsedActionInfo].GetSize()!=0)
	{
		//--------------------------------------------------------
		//
		//					Effect ¸ñÇ¥ ¼³Á¤
		//
		//--------------------------------------------------------
		MEffectTarget* pEffectTarget = new MEffectTarget( (*g_pActionInfoTable)[nUsedActionInfo].GetSize() );

		pEffectTarget->Set( point.x, point.y, pTargetCreature->GetZ(), pTargetCreature->GetID() );		

		//--------------------------------------------------------
		//
		//					½ÃÀÛ À§Ä¡¸¦ °áÁ¤ÇÑ´Ù.
		//
		//--------------------------------------------------------
		int x,y,z, direction;

		//--------------------------------------------------------
		// User À§Ä¡¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
		//--------------------------------------------------------
		if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartUser())
		{
			point = MTopView::MapToPixel(pUserCreature->GetX(), pUserCreature->GetY());

			x			= point.x;
			y			= point.y;
			z			= pUserCreature->GetZ();//+60;
		}
		//--------------------------------------------------------
		// Target À§Ä¡¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
		//--------------------------------------------------------
		else if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartTarget())
		{
			x			= point.x;
			y			= point.y;
			z			= pUserCreature->GetZ();//+60;			
		}

		//--------------------------------------------------------
		// °øÁß¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
		//--------------------------------------------------------
		if ((*g_pActionInfoTable)[nUsedActionInfo].IsStartSky())
		{
			z	= pUserCreature->GetZ() + (*g_pActionInfoTable)[nUsedActionInfo].GetValue();

			direction	= DIRECTION_DOWN;
		}
		//--------------------------------------------------------
		// Áö»ó¿¡¼­ ½ÃÀÛÇÏ´Â °æ¿ì
		//--------------------------------------------------------
		else
		{
			direction	= pUserCreature->GetDirection();
		}

		//--------------------------------------------------------
		//
		//                   Effect»ý¼º		
		//
		//--------------------------------------------------------
		g_EffectGeneratorTable.Generate(
				x,y,z,				// ½ÃÀÛ À§Ä¡
				direction, 			// ¹æÇâ
				1,					// power
				nUsedActionInfo,	//	ActionInfoTableÁ¾·ù,
				pEffectTarget		// ¸ñÇ¥ Á¤º¸
		);
	}
	*/
	//--------------------------------------------------------
	// ´ë»óÀÌ µÇ´Â Ä³¸¯ÅÍ´Â Æ¯Á¤ÇÑ ActionÀ» ÃëÇÏ°Ô µÈ´Ù.
	//--------------------------------------------------------
	//pTargetCreature->SetStop();
	//pTargetCreature->SetAction( (*g_pActionInfoTable)[nUsedActionInfo].GetAction() );

	int resultActionInfo;
	resultActionInfo = nUsedActionInfo + (*g_pActionInfoTable).GetMinResultActionInfo();	// °á°ú

	switch( nUsedActionInfo )
	{
	case SKILL_SHADOW_DANCING :
		resultActionInfo = SKILL_SINGLE_BLOW + (*g_pActionInfoTable).GetMinResultActionInfo();
		break;

	case SKILL_ILLENDUE :
		resultActionInfo = SKILL_LIGHT_BALL + (*g_pActionInfoTable).GetMinResultActionInfo();
		break;
//	case SKILL_BAT_BREAKER :
//		resultActionInfo = RESULT_SKILL_BAT_BREAKER + (*g_pActionInfoTable).GetMinResultActionInfo();
//		break;
	case SKILL_SPECTOR_INVERSE :
		resultActionInfo =SKILL_SPECTOR_INVERSE + (*g_pActionInfoTable).GetMinResultActionInfo();
		break;
#if __CONTENTS(__ONIBLA)
	case SKILL_AGONY_CHILD_SHADOW_FOUR :
		resultActionInfo = SKILL_ATTACK_MELEE + (*g_pActionInfoTable).GetMinResultActionInfo();
		break;
	case SKILL_VILLAIN_LADY_GHOST_SHADOW :
		resultActionInfo = MAGIC_RAPID_GLIDING + (*g_pActionInfoTable).GetMinResultActionInfo();
		break;
#endif //__ONIBLA
	case SKILL_BLAZE_WALK :
		resultActionInfo = SKILL_BLAZE_WALK_ATTACK + (*g_pActionInfoTable).GetMinResultActionInfo();
		break;
	}

	if( resultActionInfo >= g_pActionInfoTable->GetSize() )
	{
		DEBUG_ADD_FORMAT("[Execute-ResultActionInfo] exceed resultAction %d/%d",nUsedActionInfo,
			resultActionInfo);
		return;
	}
	
	#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			DEBUG_ADD_FORMAT("[Execute-ResultActionInfo] ID=%d, ai=%d", m_TargetID, resultActionInfo);
		}
	#endif

	// [»õ±â¼ú]

	
	pTargetCreature->SetEffectDelayFrame(resultActionInfo, m_DelayFrame );

 	if( pUserCreature != NULL)
 	{
		if(nUsedActionInfo == MAGIC_BLOODY_ZENITH)
			pUserCreature->SetBloodyZenith(8);
//		else if( nUsedActionInfo == SKILL_SET_AFIRE)
//			pUserCreature->SetBloodyZenith(20);
//		pUserCreature->SetActionDelay( 16 );
//		if( pUserCreature == g_pPlayer )
//			g_pPlayer->SetDelay( 2000 );
 	}
	
	if(nUsedActionInfo == SKILL_BLITZ_SLIDING_ATTACK )
	{
		if( pUserCreature != NULL )
		{
			pUserCreature->PacketSpecialActionResult( resultActionInfo, m_TargetID, m_sX, m_sY );
		}
	}
	else
	{
		if( (*g_pActionInfoTable)[nUsedActionInfo].IsUseActionGrade() && m_Temp != 0 )
			pTargetCreature->PacketSpecialActionResult( resultActionInfo, m_TargetID, m_sX, m_sY, m_Temp );
		else
			pTargetCreature->PacketSpecialActionResult( resultActionInfo, m_TargetID, m_sX, m_sY);
	}
	
	//--------------------------------------------------------
	// ±â¼úÀÇ µ¿ÀÛ¿¡ ¸Â´Â sound¸¦ Ãâ·ÂÇØÁØ´Ù.
	//--------------------------------------------------------
	//g_Sound.Play( g_SoundTable[(*g_pActionInfoTable)[nUsedActionInfo].GetSoundID()].pDSBuffer );
	//PlaySound( (*g_pActionInfoTable)[nUsedActionInfo].GetSoundID(),
	//			false,
	//			pTargetCreature->GetX(),
	//			pTargetCreature->GetY());

	//--------------------------------------------------------
	// EffectºÙÀÌ´Â °á°úÀÌ¸é ..
	//--------------------------------------------------------
	/*
	EFFECTSTATUS	status		= (*g_pActionInfoTable)[resultActionInfo].GetEffectStatus();

	if (status!=EFFECTSTATUS_NULL)
	{
		// ½Ã°£ÀÌ ³²¾Æ ÀÖ´Â °æ¿ì¿¡¸¸...
		if (m_DelayFrame > 0)
		{
			pTargetCreature->AddEffectStatus( status, m_DelayFrame );
		}
	}
	*/
}


//----------------------------------------------------------------------
//
// ActionResultNode - ChangePosition
//
//----------------------------------------------------------------------
MActionResultNodeChangePosition::MActionResultNodeChangePosition(TYPE_OBJECTID uid, TYPE_SECTORPOSITION sX, TYPE_SECTORPOSITION sY)
{ 
	m_UserID	= uid;
	m_X			= sX;
	m_Y			= sY;
}

//----------------------------------------------------------------------
// ActionResultNode - ChangePosition::Execute
//----------------------------------------------------------------------
void
MActionResultNodeChangePosition::Execute()
{
	if (g_pPlayer==NULL
		|| g_pZone==NULL)
	{
		return;
	}

	//-----------------------------------------------------------
	// PlayerÀÎ °æ¿ì
	//-----------------------------------------------------------
	if (m_UserID==g_pPlayer->GetID())
	{
		g_pPlayer->MovePosition( m_X, m_Y );
		g_pPlayer->SetServerPosition( m_X, m_Y );
	}
	//-----------------------------------------------------------
	// ´Ù¸¥ ¾ÖµéÀÎ °æ¿ì
	//-----------------------------------------------------------
	else
	{
		MCreature* pCreature = g_pZone->GetCreature( m_UserID );

		// playerÀÎ °æ¿ì¸¦ µû·Î Ã¼Å©ÇØ¾ß ÇÒ±î??

		if (pCreature!=NULL)
		{
			//------------------------------------------------
			// ±âÁ¸ÀÇ MoveBuffer¸¦ ´Ù Á¦°Å½ÃÅ²´Ù.
			//------------------------------------------------
			pCreature->ReleaseMoveBuffer();

			// ÁÂÇ¥¸¦ ¹Ù²Û´Ù.
			pCreature->MovePosition( m_X, m_Y );
			pCreature->SetServerPosition( m_X, m_Y );
			//pCreature->SetStop();
		}
	}
}

//----------------------------------------------------------------------
//
// ActionResultNode - ChangeStatus
//
//----------------------------------------------------------------------
MActionResultNodeChangeStatus::MActionResultNodeChangeStatus(TYPE_OBJECTID tid, MStatus* pStatus)
{
	m_TargetID = tid;
	m_pStatus = pStatus;
}

MActionResultNodeChangeStatus::~MActionResultNodeChangeStatus()
{
	if (m_pStatus != NULL)
	{
		delete m_pStatus;
	}
		
}

//----------------------------------------------------------------------
// ActionResultNode - ChangeStatus::Execute
//----------------------------------------------------------------------
void
MActionResultNodeChangeStatus::Execute()
{
	if (g_pPlayer==NULL)
	{
		return;
	}

	//---------------------------------------------------------
	// Player¸¸ MStatus¸¦ °¡Áö°í ÀÖ´Ù.. - -;;
	// ±×·¸´Ù¸é.. m_TargetID´Â ÇÊ¿ä¾ø´Âµ¥.. À½.. - -;; 
	//---------------------------------------------------------
	if (m_TargetID==g_pPlayer->GetID()
		&& m_pStatus!=NULL)
	{
		g_pPlayer->ApplyStatus( *m_pStatus );		
	}
}

//----------------------------------------------------------------------
// ActionResultNodeCreatureDie - CreatureDie::Execute
//----------------------------------------------------------------------
void
MActionResultNodeCreatureDie::Execute()
{
	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

	//---------------------------------------------------------
	// Player ÀÎ °æ¿ì
	//---------------------------------------------------------
	if (m_TargetID == g_pPlayer->GetID())
	{
		g_pPlayer->SetDead();
	}
	//---------------------------------------------------------
	// ´Ù¸¥ CreatureÀÎ °æ¿ì
	//---------------------------------------------------------
	else
	{
		MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );

		//---------------------------------------------------------
		// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ÀÇ¹Ì¾ø°ÚÁö..
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			return;
		}
		
		//---------------------------------------------------------
		// Á×ÀÎ´Ù. - -;
		//---------------------------------------------------------		
		pCreature->SetDead();	
	}
}

//----------------------------------------------------------------------
// ActionResultNodeCreatureBurrow - CreatureBurrow::Execute
//----------------------------------------------------------------------
void
MActionResultNodeCreatureBurrow::Execute()
{
	if (g_pPlayer==NULL
		|| g_pZone==NULL)
	{
		return;
	}

	//---------------------------------------------------------
	// Player ÀÎ °æ¿ì
	//---------------------------------------------------------
	if (m_TargetID == g_pPlayer->GetID())
	{
		// Underground Creature·Î ¸¸µç´Ù.
		g_pPlayer->SetUndergroundCreature();

		// [µµ¿ò¸»] burrow
//		__BEGIN_HELP_EVENT
//			ExecuteHelpEvent( HE_EFFECT_BURROW );
//		__END_HELP_EVENT
	}
	//---------------------------------------------------------
	// ´Ù¸¥ CreatureÀÎ °æ¿ì
	//---------------------------------------------------------
	else
	{
		MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );

		//---------------------------------------------------------
		// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ÀÇ¹Ì¾ø°ÚÁö..
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			return;
		}
		
		//---------------------------------------------------------
		// ¼û±ä´Ù. 
		//---------------------------------------------------------		
		pCreature->SetUndergroundCreature();
	}
}

//----------------------------------------------------------------------
// ActionResultNodeCreatureInvisible - CreatureInvisible::Execute
//----------------------------------------------------------------------
void
MActionResultNodeCreatureInvisible::Execute()
{
	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

	//---------------------------------------------------------
	// Player ÀÎ °æ¿ì
	//---------------------------------------------------------
	if (m_TargetID == g_pPlayer->GetID())
	{
		// Underground Creature·Î ¸¸µç´Ù.
		g_pPlayer->SetInvisible();

		// [µµ¿ò¸»] burrow
//		__BEGIN_HELP_EVENT
//			ExecuteHelpEvent( HE_EFFECT_BURROW );
//		__END_HELP_EVENT
	}
	//---------------------------------------------------------
	// ´Ù¸¥ CreatureÀÎ °æ¿ì
	//---------------------------------------------------------
	else
	{
		MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );

		//---------------------------------------------------------
		// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ÀÇ¹Ì¾ø°ÚÁö..
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			return;
		}
		
		//---------------------------------------------------------
		// invisible
		//---------------------------------------------------------		
		pCreature->SetInvisible();
	}
}

//----------------------------------------------------------------------
// ActionResultNodeCreatureVisible - CreatureVisible::Execute
//----------------------------------------------------------------------
void
MActionResultNodeCreatureVisible::Execute()
{
	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

	//---------------------------------------------------------
	// Player ÀÎ °æ¿ì
	//---------------------------------------------------------
	if (m_TargetID == g_pPlayer->GetID())
	{
		// Underground Creature·Î ¸¸µç´Ù.
		g_pPlayer->SetVisible();

		// [µµ¿ò¸»] burrow
//		__BEGIN_HELP_EVENT
//			//ExecuteHelpEvent( HE_EFFECT_BURROW );
//		__END_HELP_EVENT
	}
	//---------------------------------------------------------
	// ´Ù¸¥ CreatureÀÎ °æ¿ì
	//---------------------------------------------------------
	else
	{
		MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );

		//---------------------------------------------------------
		// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ÀÇ¹Ì¾ø°ÚÁö..
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			return;
		}
		
		//---------------------------------------------------------
		// invisible
		//---------------------------------------------------------		
		pCreature->SetVisible();
	}
}

//----------------------------------------------------------------------
// ActionResultNodeCreatureSnipping - CreatureSnipping::Execute
//----------------------------------------------------------------------
void
MActionResultNodeCreatureSnipping::Execute()
{
	#ifdef OUTPUT_DEBUG
		if (g_pDebugMessage)
		{
			DEBUG_ADD_FORMAT("[Execute-CreatureSnipping] ID=%d", m_TargetID);
		}
	#endif

	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

	//---------------------------------------------------------
	// Player ÀÎ °æ¿ì
	//---------------------------------------------------------
	if (m_TargetID == g_pPlayer->GetID())
	{		
		g_pPlayer->AddEffectStatus( EFFECTSTATUS_SNIPPING_MODE, 0xFFFF );
		g_pPlayer->SetInvisible();

		// [µµ¿ò¸»] burrow
//		__BEGIN_HELP_EVENT
//			//ExecuteHelpEvent( HE_EFFECT_BURROW );
//		__END_HELP_EVENT
	}
	//---------------------------------------------------------
	// ´Ù¸¥ CreatureÀÎ °æ¿ì
	//---------------------------------------------------------
	else
	{
		MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );

		//---------------------------------------------------------
		// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ÀÇ¹Ì¾ø°ÚÁö..
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			return;
		}
		
		//---------------------------------------------------------
		// invisible
		//---------------------------------------------------------		
		pCreature->AddEffectStatus( EFFECTSTATUS_SNIPPING_MODE, 0xFFFF );
		pCreature->SetInvisible();
	}
}
void
MActionResultNodeCreatureSnipping2::Execute()
{
#ifdef OUTPUT_DEBUG
	if (g_pDebugMessage)
	{
		DEBUG_ADD_FORMAT("[Execute-CreatureSnipping2] ID=%d", m_TargetID);
	}
#endif

	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

	if (m_TargetID == g_pPlayer->GetID())
	{		
		g_pPlayer->AddEffectStatus( EFFECTSTATUS_SNIPPING_MODE_2, 0xFFFF );
		g_pPlayer->SetInvisible();

	}

	else
	{
		MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );

		if (pCreature==NULL)
		{
			return;
		}

		//---------------------------------------------------------
		// invisible
		//---------------------------------------------------------		
		pCreature->AddEffectStatus( EFFECTSTATUS_SNIPPING_MODE_2, 0xFFFF );
		pCreature->SetInvisible();
	}
}
//----------------------------------------------------------------------
// ActionResultNodeCreatureTurning - CreatureTurning::Execute
//----------------------------------------------------------------------
void
MActionResultNodeCreatureTurning::Execute()
{
	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

	//---------------------------------------------------------
	// Player ÀÎ °æ¿ì
	//---------------------------------------------------------
	if (m_TargetID == g_pPlayer->GetID())
	{
		// Underground Creature·Î ¸¸µç´Ù.
		g_pPlayer->SetTurning( m_TurnFrame );
	}
	//---------------------------------------------------------
	// ´Ù¸¥ CreatureÀÎ °æ¿ì
	//---------------------------------------------------------
	else
	{
		MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );

		//---------------------------------------------------------
		// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ÀÇ¹Ì¾ø°ÚÁö..
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			return;
		}
		
		//---------------------------------------------------------
		// invisible
		//---------------------------------------------------------		
		pCreature->SetTurning( m_TurnFrame );
	}
}

//----------------------------------------------------------------------
// ActionResultNodeAddEffectStatus
//----------------------------------------------------------------------
void
MActionResultNodeAddEffectStatus::Execute()
{
	if (g_pZone==NULL)
	{
		return;
	}

	MCreature*	pTargetCreature = g_pZone->GetCreature( m_TargetID );

	// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ³¡~
	if (pTargetCreature==NULL)// || pUserCreature==NULL)
		return;

	//--------------------------------------------------------
	// EffectStatus¸¦ ºÙÀÎ´Ù.
	//--------------------------------------------------------
	pTargetCreature->AddEffectStatus(m_EffectStatus, m_DelayFrame);	
}

//----------------------------------------------------------------------
//
// ActionResultNode - ChangeCreatureType
//
//----------------------------------------------------------------------
MActionResultNodeChangeCreatureType::MActionResultNodeChangeCreatureType(TYPE_OBJECTID tid, int ctype)
{
	m_TargetID = tid;
	m_CreatureType = ctype;
}

MActionResultNodeChangeCreatureType::~MActionResultNodeChangeCreatureType()
{
}

//----------------------------------------------------------------------
// ActionResultNode - ChangeStatus::Execute
//----------------------------------------------------------------------
void
MActionResultNodeChangeCreatureType::Execute()
{
	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

	MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );
	
	// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ³¡~
	if (pCreature==NULL)
		return;


	if (pCreature->IsUndergroundCreature())
	{
		// ±×³É ¹¯¾îµÐ´Ù. - -;
	}
	else
	{
		// ¼ºº°À» ´Ù½Ã ¼³Á¤ÇØÁà¾ß ÇÑ´Ù.
		bool bMale = pCreature->IsMale();
		WORD cs1 = pCreature->GetBodyColor1();
		WORD cs2 = pCreature->GetBodyColor2();

		pCreature->SetCreatureType( m_CreatureType );

		pCreature->SetMale( bMale );
		if( m_CreatureType == CREATURETYPE_WER_WOLF )//±äÀÇºóÑÕÉ«
			pCreature->SetBodyColor1( cs2 );
		else
			pCreature->SetBodyColor1( cs1 );

		pCreature->SetBodyColor2( cs2 );

		// ¹«±â Â÷°í ÀÖ´ÂÁö Ã¼Å©
		MCreatureWear *pCreatureWear = dynamic_cast<MCreatureWear*>(pCreature);

		if(pCreatureWear)
		{
			MItem *pCoatItem = g_pVampireGear->GetItem(MVampireGear::GEAR_VAMPIRE_COAT);
			if(pCoatItem)
			{
//20080904 º¯½Å,º¯½ÅÇØÁ¦ ¿ëÀ¸·Î ¹ìÆÄÀÇ °æ¿ì Å©¸®Ã³¿Í ¹ìÆÄ ÆÑÀÌ ³ª´¸À¸·Î ÇØ¼­ Æ¨±æ¼ö ÀÖ´Ù
//µû¶ó¼­ Àû¿ë ºÒ°¡´ÉÇÑ ¾ÆÀÌÅÛÀº ¿ÜÇü Àû¿ëµÇ µÇÁö ¾ÊÀ¸¹Ç·Î °Á ¹þ°Ü ¹ö¸°´Ù. 
//¾Æ·¡ ÄÚµå¸¸À¸·Ð ³²¿¡°Ô´Â º¸ÀÌÁö ¾ÊÀ¸¸ç ÀçÀåºñ ÇÒ°æ¿ì ÀÚ±â ÀÚ½Å¿¡°Ô¸¸ º¸ÀÎ´Ù.
//add start
				if(!pCoatItem->IsAffectStatus())
				{
					pCreatureWear->RemoveAddon(ADDON_COAT);
				}
//add end				
				pCreatureWear->SetAddonItem(pCoatItem);
			}

			MItem *pRightItem = g_pVampireGear->GetItem(MVampireGear::GEAR_VAMPIRE_RIGHTHAND);

			if(pRightItem && pRightItem->GetItemClass() == ITEM_CLASS_VAMPIRE_WEAPON)
			{
				pCreatureWear->SetAddonItem(pRightItem);
			}
		}

		// ÀÌµ¿¹æ¹ý¿¡ µû¶ó..
		if ((*g_pCreatureTable)[m_CreatureType].IsFlyingCreature())
		{
			pCreature->SetFlyingCreature();
		}
		else
		{
			pCreature->SetGroundCreature();
		}
	}

	// playerÀÎ °æ¿ì´Â skillÃë¼Ò
	if (m_TargetID == g_pPlayer->GetID())
	{
		int selectSkill = ACTIONINFO_NULL;

		switch (m_CreatureType)
		{
			case CREATURETYPE_BAT :
			case CREATURETYPE_VAMPIRE_GHOST :
			case CREATURETYPE_FLITTERMOUSE :
			case CREATURETYPE_NEDE:
			case CREATURETYPE_KLTL:
#if __CONTENTS(__SECOND_TRANSFORTER)
			case CREATURETYPE_SHAPE_OF_DEMON :
#endif //__SECOND_TRANSFORTER
				selectSkill = MAGIC_UN_TRANSFORM;
			break;
		
			case CREATURETYPE_WOLF :
				selectSkill = MAGIC_EAT_CORPSE;
			break;

			case CREATURETYPE_WER_WOLF ://±äÀÇºóÊ×Ñ¡¼¼ÄÜ
				selectSkill = MAGIC_UN_TRANSFORM;
			break;
			
			// 2004, 9, 13, sobeit add start - ÃÑ½½ 130 skill
			case CREATURETYPE_INSTALL_TURRET:
				selectSkill = MAGIC_UN_TRANSFORM;
			break;

			case CREATURETYPE_INSTALL_MK1:
				selectSkill = MAGIC_UN_TRANSFORM;
			break;

			// 2004, 9, 13, sobeit add end
		}
		
		if (selectSkill!=ACTIONINFO_NULL)
		{
			gC_vs_ui.SelectSkill( selectSkill );
		}
		else
		{
			gC_vs_ui.UnselectSkill();
		}

		// ÀÌÀü¿¡ ¼³Á¤ÇØµÐ°Å ¾ø¾Ø´Ù.
//		if (g_MorphCreatureType==m_CreatureType)
		{
			g_MorphCreatureType = 0;
		}
	}
}


//----------------------------------------------------------------------
// [»õ±â¼ú]
//----------------------------------------------------------------------
MActionResultNodeSummonCasket::MActionResultNodeSummonCasket(TYPE_OBJECTID tid, int casketType)
{
	m_TargetID = tid;
	m_CasketType = casketType;
}

MActionResultNodeSummonCasket::~MActionResultNodeSummonCasket()
{
}

//----------------------------------------------------------------------
// ActionResultNode - SummonCasket::Execute
//----------------------------------------------------------------------
void
MActionResultNodeSummonCasket::Execute()
{
	if (g_pZone==NULL)
	{
		return;
	}

	MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );
	
	// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ³¡~
	if (pCreature==NULL)
		return;

	//
	if (!pCreature->IsInCasket())
	{
		pCreature->SetStop();
		pCreature->MovePosition( pCreature->GetServerX(), pCreature->GetServerY() );
		pCreature->AddCasket( m_CasketType );
	}
}

//----------------------------------------------------------------------
// ActionResultNode - OpenCasket::Execute
//----------------------------------------------------------------------
void
MActionResultNodeOpenCasket::Execute()
{
	if (g_pZone==NULL)
	{
		return;
	}

	MCreature*	pCreature = g_pZone->GetCreature( m_TargetID );
	
	// Ä³¸¯ÅÍ°¡ ¾øÀ¸¸é... ³¡~
	if (pCreature==NULL)
		return;

	if (pCreature->IsInCasket())
	{
		pCreature->RemoveCasket();
	}
}

//----------------------------------------------------------------------
// ActionResultNode - OpenCasket::Execute
//----------------------------------------------------------------------
void
MActionResultNodeRemoveItemInInventory::Execute()
{
	if (g_pInventory==NULL)
	{
		return;
	}

	MItem* pRemovedItem = g_pInventory->RemoveItem( m_TargetID );
	SAFE_DELETE( pRemovedItem );

	MItem*	pMouseItem = NULL;

	if(g_pPlayer->IsItemCheckBufferItemToItem())
	{

		pMouseItem = g_pPlayer->GetItemCheckBuffer();

		g_pPlayer->ClearItemCheckBuffer();

		if(pMouseItem != NULL)
			delete pMouseItem;
	}
	
}

//----------------------------------------------------------------------
// ActionResultNode - OpenCasket::Execute
//----------------------------------------------------------------------
void
MActionResultNodeAddItemOptionThreeInInventory::Execute()//ÈýÊôÐÔ
{
	if (g_pInventory==NULL)
	{
		return;
	}

	MItem*	pInvenItem = g_pInventory->GetItem(m_TargetID);
	MItem*	pMouseItem = NULL;

	bool bItemOptChange	= false;

	bool bItemOptChangeTwoOpt	= false;

	bool bGaiZao = false;

	if(g_pPlayer->IsItemCheckBufferItemToItem())
	{

		pMouseItem = g_pPlayer->GetItemCheckBuffer();

		g_pPlayer->ClearItemCheckBuffer();

		if(pMouseItem != NULL)
		{

			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() == 68)
			{
				bItemOptChange = true;
			}
			delete pMouseItem;
		}
	}
	if(g_pPlayer->IsItemCheckBufferItemToItem())
	{

		pMouseItem = g_pPlayer->GetItemCheckBuffer();

		g_pPlayer->ClearItemCheckBuffer();

		if(pMouseItem != NULL)
		{

			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() == 95)
			{
				bGaiZao = true;
			}
			delete pMouseItem;
		}
	}
	if (pInvenItem==NULL)
		return;

	bool bSucces = false;

	if(bItemOptChange)
	{
	//	pInvenItem->ClearItemOption();
		pInvenItem->AddItemOption(m_Type);
		bSucces	= true;
	}
	if(bGaiZao)
	{
	//	pInvenItem->ClearItemOption();
		pInvenItem->AddItemGTwoOption(m_Type);
		bSucces	= true;
	}
	if(bSucces)
		PlaySound(SOUND_EVENT_FANFARE);

}

//----------------------------------------------------------------------
// ActionResultNode - OpenCasket::Execute
//----------------------------------------------------------------------
void
MActionResultNodeChangeItemOptionInInventory::Execute()
{
	if (g_pInventory==NULL)
	{
		return;
	}

	MItem*	pInvenItem = g_pInventory->GetItem(m_TargetID);
	MItem*	pMouseItem = NULL;

	bool bSecondPetEnchant = false;
	bool bRevivalPet = false;
	bool bRemovePetOption = false;
	bool bGradePlus = false;
	bool bPetNoChange = false;
	bool bGradeMinus = false;
	bool bItemOptChange	= false;

	bool bItemOptChangeTwoOpt	= false;
	bool bGaiZaoChange = false;

#if __CONTENTS(__TUNING_ITEM)
	bool bItemTuning = false;
#endif //__TUNING_ITEM

	if(g_pPlayer->IsItemCheckBufferItemToItem())
	{

		pMouseItem = g_pPlayer->GetItemCheckBuffer();

		g_pPlayer->ClearItemCheckBuffer();

		if(pMouseItem != NULL)
		{
			if(pMouseItem->GetItemClass() == ITEM_CLASS_PET_ENCHANT_ITEM)
			{
				if(pMouseItem->GetItemType() == 13 ||
					pMouseItem->GetItemType() == 14)	// Æê ºÎÈ° Â¥ÀÜ~
				{
					bRevivalPet = true;
				}
				else
				if(pMouseItem->GetItemType() == 15 ||	// ·¹µå ¹öµå
				   pMouseItem->GetItemType() == 26)		// ·¹µå ¹öµå 2
				{
					bSecondPetEnchant = true;
				}
				else
				if(pMouseItem->GetItemType() >= 27 &&	// Ä¸½¶
				   pMouseItem->GetItemType() <= 29)
				{
					bPetNoChange = true;
				}
			}
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() >=54 && 
				pMouseItem->GetItemType() <=56)
			{
				bGradeMinus = true;
			}


			if(pMouseItem->GetItemClass() == ITEM_CLASS_MIXING_ITEM &&
				((pMouseItem->GetItemType() >= 9 && pMouseItem->GetItemType() <= 18) || pMouseItem->GetItemType() == 25))
			{
				bRemovePetOption = true;
			}

			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && 
				(pMouseItem->GetItemType() == 22 || pMouseItem->GetItemType() == 48 ))// ¿»·Î¿ì µå·Ó
			{
				bGradePlus = true;
			}
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && (pMouseItem->GetItemType() == 57 || pMouseItem->GetItemType() == 69 || pMouseItem->GetItemType() == 91))
			{		//¿»·Î¿ì µå·Ó 3
				bGradePlus = true;
			}
#if __CONTENTS(__2008_FIRST_CHARGE_ITEM)
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() == 58 )
			{		//Crystal — 1. opt reroll
				bItemOptChange = true;
			}
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() == 95)
			{		//Å©¸®½ºÅ»
				bGaiZaoChange = true;
			}
#endif	//__2008_FIRST_CHARGE_ITEM
			
#if __CONTENTS(__TUNING_ITEM)
			if(pMouseItem->IsTuningItem())
			{
				bItemTuning = true;
			}
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && (pMouseItem->GetItemType()>=87 && pMouseItem->GetItemType()<=90))
			{
				bItemTuning = true;
			}
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() ==92)
			{
				bItemTuning = true;
			}
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() ==93)
			{
				bItemTuning = true;
			}
			if(pMouseItem->GetItemClass() == ITEM_CLASS_EVENT_STAR && pMouseItem->GetItemType() ==94)
			{
				bItemTuning = true;
			}
#endif	//__TUNING_ITEM

			delete pMouseItem;
		}
	}
	
	// ¾ÆÀÌÅÛÀÌ ¾øÀ¸¸é... ³¡~
	if (pInvenItem==NULL)
		return;

	bool bSucces = false;

	if(bRemovePetOption)
	{
		pInvenItem->ClearItemOption();
		bSucces = true;
	}
	else if(bGradePlus)
	{
		pInvenItem->SetGrade(m_Type);
		bSucces = true;
	}
	else if(bGradeMinus)
	{
		pInvenItem->SetGrade(m_Type);
		bSucces = false;
	}

	else if(pInvenItem->GetItemClass() == ITEM_CLASS_PET_ITEM)
	{
		if(bPetNoChange)
		{
			bSucces = true;
		}
		else
		if(bRevivalPet == true)
		{
			pInvenItem->SetCurrentDurability(m_Type);

			bSucces = true;
		}
		else
		if(bSecondPetEnchant == false && pMouseItem->GetItemType() != 24)
		{
			// Æê 1Â÷ ÀÎÃ¾Æ® ÇÊ»ì! ¼Ó¼º ºÙÀÌ±â
#if __CONTENTS(!__PET_VISION_AMPLE)
			WORD AttrType = HIWORD(m_Type);
			WORD AttrLevel = LOWORD(m_Type);
			pInvenItem->SetEnchantLevel(AttrType);	// Attr Type
			pInvenItem->SetSilver(AttrLevel);	// AttrLevel
			pInvenItem->SetItemColorSet(0xFFFF);
#endif //__PET_VISION_AMPLE
			bSucces = true;
		}
		else
		{
			// Æê 2Â÷ ÀÎÃ¾Æ® ¿É¼Ç ºÙÀÌ±â, ¾Æ·¡ ÄÚµå ±×´ë·Î ½áµµ µÉ°Å °°Àºµ¥ ¾ç±ºÀÌ ÂÌ¶ú´Ù.-¤µ-
#if __CONTENTS(!__PET_VISION_AMPLE)
			pInvenItem->ClearItemOption();	// ÀÏ´Ü ¸Ó°¡ ÀÖÀ»Áö ¸ð¸£´Ï Áö¿ì°í º»´Ù-0-
			pInvenItem->AddItemOption(m_Type);
#endif //__PET_VISION_AMPLE
			bSucces = true;
		}
	}
	else if(bItemOptChange)
	{
		pInvenItem->ClearItemOption();
		pInvenItem->AddItemOption(m_Type);
		bSucces	= true;
	}
	else if(bGaiZaoChange)
	{
		pInvenItem->ClearGTwoItemOption();
		pInvenItem->AddItemGTwoOption(m_Type);
		bSucces	= true;
	}
	else if(bItemOptChangeTwoOpt)
	{
	//	pInvenItem->ClearItemOption();
		pInvenItem->AddItemOption(m_Type);
		bSucces	= true;
	}
#if __CONTENTS(__TUNING_ITEM)
	else if(bItemTuning)
	{
		pInvenItem->SetItemType(m_Type);
		bSucces	= true;
	}
#endif //__TUNING_ITEM
	else
	{

		DWORD Option = m_Type;
	//	int Option2 = LOWORD(m_Type);

	//	if(oriOption < newOption)
		
		for(; Option != NULL; Option >>= 16)	// 2¹ÙÀÌÆ® ½¬ÇÁÆ® ÇÏ¸é LOWORD, HIWORD¼øÀ¸·Î µÇ³ª
		{
			int oriOption = (Option & 0xff00) >> 8;
			int newOption = (Option & 0xff);

			if(newOption == NULL && oriOption == NULL)
				continue;

	//			continue;
			
			if(oriOption)
				pInvenItem->ChangeItemOption(oriOption, newOption);
			else
				pInvenItem->AddItemOption(newOption);

			if((*g_pItemOptionTable)[oriOption].UpgradeOptionType == newOption)
				bSucces |= true;
		}
	}

	if(bSucces)
		PlaySound(SOUND_EVENT_FANFARE);

}

MActionResultNodeChangeItemThirdOptionInInventory::MActionResultNodeChangeItemThirdOptionInInventory(tag_ThirdOptionData* type, TYPE_OBJECTID tid)
{
	m_TargetID = tid;
	m_ThirdOptionList = *(type->thirdOptionList);
	m_ThirdOptionType	 = type->thirdOptionType;
}

void
MActionResultNodeChangeItemThirdOptionInInventory::SetOption(tag_ThirdOptionData* type)
{
	m_ThirdOptionList = *(type->thirdOptionList);
	m_ThirdOptionType	 = type->thirdOptionType;
}


void 
MActionResultNodeChangeItemThirdOptionInInventory::Execute()
{
	if (g_pInventory==NULL)
	{
		return;
	}

	MItem*	pInvenItem = g_pInventory->GetItem(m_TargetID);
	MItem*	pMouseItem = NULL;

	//Ä«ÇÇ¿£ ÆÐÀÌ½ºÆ®¿¡ ºÙ¾î ¿Ô´Ù. ³ªÁß¿¡ ÇÊ¿ä ÇÒ¼öµµ ÀÖÀ¸´Ï °Á µÐ´Ù.
	bool bSecondPetEnchant = false;
	bool bRevivalPet = false;
	bool bRemovePetOption = false;
	bool bGradePlus = false;
	bool bPetNoChange = false;
	bool bThirdOptionSecces = false;

	if(m_ThirdOptionList.size())
	{
		pInvenItem->ChangeItemThirdOption(m_ThirdOptionList);
		pInvenItem->SetItemThirdOptionType(m_ThirdOptionType);
		bThirdOptionSecces = true;
	}
	else
	{
		pInvenItem->ClearItemThirdOption();
		pInvenItem->SetItemThirdOptionType(99);
	}
	if(bThirdOptionSecces)
	{
		PlaySound(SOUND_EVENT_FANFARE);
	}
}
//----------------------------------------------------------------------
// ActionResultNodeFakeDie - FakeDie::Execute
//----------------------------------------------------------------------
void
MActionResultNodeFakeDie::Execute()
{
	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

}

//----------------------------------------------------------------------
// ActionResultNodeEvent - Event::Execute
//----------------------------------------------------------------------
void
MActionResultNodeEvent::Execute()
{
	if (g_pZone==NULL
		|| g_pPlayer==NULL)
	{
		return;
	}

}

//----------------------------------------------------------------------
// 
// MActionResult :: constructor/destructor
//
//----------------------------------------------------------------------
MActionResult::MActionResult()
{
}

MActionResult::~MActionResult()
{
	ACTIONRESULTNODE_LIST::iterator	iNode = m_List.begin();

	// ¸ðµç node¸¦ deleteÇØÁØ´Ù.
	while (iNode != m_List.end())
	{
		MActionResultNode* pResultNode = *iNode;
		
		// °á°ú ½ÇÇà..
		//pResultNode->Execute();

		delete pResultNode;
		
		iNode++;
	}

	m_List.clear();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MActionResult::Release()
{
	ACTIONRESULTNODE_LIST::iterator	iNode = m_List.begin();

	// ¸ðµç node¸¦ deleteÇØÁØ´Ù.
	while (iNode != m_List.end())
	{
		MActionResultNode* pResultNode = *iNode;
		
		delete pResultNode;
		
		iNode++;
	}

	m_List.clear();
}

//----------------------------------------------------------------------
// Add : °á°ú ÇÏ³ª¸¦ Ãß°¡ÇÑ´Ù.
//----------------------------------------------------------------------
void		
MActionResult::Add(MActionResultNode* pNode)
{
	if (pNode==NULL)
		return;

	// list¿¡ Ãß°¡
	m_List.push_back( pNode );
}

//----------------------------------------------------------------------
// ExecuteResult
//----------------------------------------------------------------------
// ActionInfo(Effect)¿¡ µû¸¥ °á°ú¸¦ ½ÇÇàÇÑ´Ù.
//----------------------------------------------------------------------
void
MActionResult::Execute()
{
	//------------------------------------------------
	// ¸ðµÎ~~ Ã³¸®ÇÑ´Ù.
	//------------------------------------------------
	while (!m_List.empty())
	{	
		MActionResultNode* pResultNode = m_List.front();
		
		m_List.pop_front();

		if (pResultNode!=NULL)
		{
			pResultNode->Execute();

			delete pResultNode;
		}
	}
}

//----------------------------------------------------------------------
// ActionResultNode - OpenCasket::Execute
//----------------------------------------------------------------------
void
MActionResultNodeChangeItemGenderInInventory::Execute()
{
	if (g_pInventory==NULL)
	{
		return;
	}

	MItem*	pInvenItem = g_pInventory->GetItem(m_TargetID);
	MItem*	pMouseItem = NULL;

	if(g_pPlayer->IsItemCheckBufferItemToItem())
	{
		pMouseItem = g_pPlayer->GetItemCheckBuffer();

		g_pPlayer->ClearItemCheckBuffer();
		if(pMouseItem != NULL)
			delete pMouseItem;
	}
	
	// ¾ÆÀÌÅÛÀÌ ¾øÀ¸¸é... ³¡~
	if (pInvenItem==NULL)
		return;

	pInvenItem->SetItemType( pInvenItem->GetItemType() ^ 0x1 );

	g_pInventory->CheckAffectStatus( pInvenItem );
	PlaySound(SOUND_EVENT_FANFARE);
}
