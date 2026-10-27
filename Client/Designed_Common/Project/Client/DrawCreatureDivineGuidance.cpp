//#define __3D_IMAGE_OBJECT__					// by sonee
//----------------------------------------------------------------------
// MTopViewDraw.cpp1
//----------------------------------------------------------------------
//
// 복잡한 Draw 함수 부분만 따로 빼놓는다.
// 작업하기가 영 불편해서 
//----------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)

#include "DX3D.h"
#include <math.h>
#include <list>
#include <stdio.h>
#include <fstream>
#include "MZone.h"
#include "MCreature.h"
#include "MFakeCreature.h"
#include "MPlayer.h"
#include "MItem.h"
#include "MImageObject.h"
#include "MAnimationObject.h"
#include "MShadowAnimationObject.h"
#include "DarkAssert.h"

#include "MMovingEffect.h"
#include "MScreenEffect.h"
#include "MTopView.h"
#include "MItemTable.h"
#include "MCreatureTable.h"
#include "MWeather.h"
#include "CMessageArray.h"
#include "DXLib.h"
#include "SP.h"
#include "FL2.h"
//#include "2D.h"
#include "ClientConfig.h"
#include "VS_UI_BASE.h"
#include "VS_UI_Mouse_Pointer.h"
#include "UIFunction.h"
#include "AddonDef.h"
#include "MZoneTable.h"
#include "MGameTime.h"
#include "UserOption.h"
#include "UserInformation.h"
#include "SpriteIDDef.h"
#include "MGameStringTable.h"
//#include "MZoneInfo.h"
#include "MObjectSelector.h"
#include "MPortal.h"
#include "EffectSpriteTypeDef.h"
#include "MScreenEffectManager.h"
#include "MEffectSpriteTypeTable.h"
#include "Profiler.h"
//#include "MFileDef.h"
#include "Properties.h"
#include "MEventManager.h"
#include "DebugInfo.h"
#include "MItemOptionTable.h"
#include "Client.h"
#include "SkillDef.h"
#include "vs_ui_item.h"
#include "MTopViewDraw.inl"
#include "crash_trace_tool.h"

extern int defaultSkinColor;
extern int defaultCoatColor;
extern int defaultTrouserColor;

namespace {
	POINT pointTemp;
	RECT rect;

	bool Phoenix_DivineGuidanceLookupFrameArray(
		CCreatureFramePack& framePack,
		int body,
		int action,
		int direction,
		const char* context,
		const MCreature* pCreature,
		FRAME_ARRAY** ppFrames)
	{
		if (ppFrames != NULL)
			*ppFrames = NULL;

		const int creatureType = (pCreature != NULL) ? (int)pCreature->GetCreatureType() : -1;
		const int packSize = (int)framePack.GetSize();
		if (body < 0 || body >= packSize)
		{
			DEBUG_ADD_FORMAT("[%s] frame body out of range creatureType=%d body=%d packSize=%d action=%d direction=%d",
				context, creatureType, body, packSize, action, direction);
			CT_Done("%s frame body out of range creatureType=%d body=%d packSize=%d action=%d direction=%d",
				context, creatureType, body, packSize, action, direction);
			return false;
		}

		ACTION_FRAME_ARRAY& actions = framePack[(TYPE_FRAMEID)body];
		const int actionCount = (int)actions.GetSize();
		if (action < 0 || action >= actionCount)
		{
			DEBUG_ADD_FORMAT("[%s] frame action out of range creatureType=%d body=%d action=%d actionCount=%d direction=%d",
				context, creatureType, body, action, actionCount, direction);
			CT_Done("%s frame action out of range creatureType=%d body=%d action=%d actionCount=%d direction=%d",
				context, creatureType, body, action, actionCount, direction);
			return false;
		}

		DIRECTION_FRAME_ARRAY& directions = actions[(BYTE)action];
		const int directionCount = (int)directions.GetSize();
		if (direction < 0 || direction >= directionCount)
		{
			DEBUG_ADD_FORMAT("[%s] frame direction out of range creatureType=%d body=%d action=%d direction=%d directionCount=%d",
				context, creatureType, body, action, direction, directionCount);
			CT_Done("%s frame direction out of range creatureType=%d body=%d action=%d direction=%d directionCount=%d",
				context, creatureType, body, action, direction, directionCount);
			return false;
		}

		if (ppFrames != NULL)
			*ppFrames = &directions[(BYTE)direction];

		return true;
	}

	CIndexSprite* Phoenix_DivineGuidanceLookupIndexSprite(
		CIndexSpritePack& spritePack,
		int sprite,
		const char* context,
		const MCreature* pCreature,
		int body,
		int action,
		int direction)
	{
		const int creatureType = (pCreature != NULL) ? (int)pCreature->GetCreatureType() : -1;
		const int spriteCount = (int)spritePack.GetSize();
		if (sprite < 0 || sprite >= spriteCount)
		{
			DEBUG_ADD_FORMAT("[%s] sprite out of range creatureType=%d body=%d action=%d direction=%d sprite=%d spriteCount=%d",
				context, creatureType, body, action, direction, sprite, spriteCount);
			CT_Done("%s sprite out of range creatureType=%d body=%d action=%d direction=%d sprite=%d spriteCount=%d",
				context, creatureType, body, action, direction, sprite, spriteCount);
			return NULL;
		}

		CIndexSprite* pSprite = &spritePack[(TYPE_SPRITEID)sprite];
		if (pSprite == NULL || pSprite->IsNotInit())
		{
			DEBUG_ADD_FORMAT("[%s] sprite not initialized creatureType=%d body=%d action=%d direction=%d sprite=%d spriteCount=%d",
				context, creatureType, body, action, direction, sprite, spriteCount);
			CT_Done("%s sprite not initialized creatureType=%d body=%d action=%d direction=%d sprite=%d spriteCount=%d",
				context, creatureType, body, action, direction, sprite, spriteCount);
			return NULL;
		}

		return pSprite;
	}
};

extern int GetAdvancementPartFromItemClass( ITEM_CLASS itemClass , TYPE_FRAMEID frameID);
extern int GetAdvancementVampireAction171[];

void	MTopView::DrawDivineGuidanceSlayerCharacter( 
	POINT *pPoint, MCreature* pCreature, int direction )
{
	MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;
	
	// Print the add-on that matches the Creature's Action.
	//action = pCreature->GetAction();
	const int tempFrame = 30;
	
	WORD clothes;
	BYTE clothesType;
	
	for(int k = 0; k < 2; k++)
	{
		for (int i=0; i<ADDON_MAX; i++)
		{
			// Depending on Creature's current direction...
			// The order in which the clothes are printed may vary.
			clothesType = MCreatureWear::s_AddonOrder[pCreature->GetDirection()][i];
			
			// If you are wearing the i-th type of clothing, it will print the result.
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(clothesType);
			
			if (addonInfo.bAddon)
			{
				clothes = addonInfo.FrameID;

				CCreatureFramePack& addonFPK = pCreature->IsMale() ? m_AddonManFPK : m_AddonWomanFPK;
				CIndexSpritePack&	addonSPK = pCreature->IsMale() ? m_AddonManSPK : m_AddonWomanSPK;
				
				const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
				FRAME_ARRAY* pFA = NULL;
				if (!Phoenix_DivineGuidanceLookupFrameArray(addonFPK, clothes, ACTION_DRAINED, drawDirection,
					"DivineGuidanceSlayer", pCreature, &pFA))
					continue;
				FRAME_ARRAY& FA = *pFA;
				
				// If the action exists
				if (FA.GetSize() > tempFrame)
				{
					CFrame &Frame = FA[tempFrame];					
					int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
					int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
					int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();	

					CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(addonSPK, sprite,
						"DivineGuidanceSlayer", pCreature, clothes, ACTION_DRAINED, drawDirection);
					if (pSprite == NULL)
						continue;
					
					pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
					pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
					
					//---------------------------------------- 
					// Normal character output
					//---------------------------------------- 
					int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
					if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
					{
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
						if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
							colorSet1 = colorSet2;
					}
					
					if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					{
						colorSet2 = pCreature->GetItemOptionExtColorSet( colorSet2 );
						if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
							colorSet1 = colorSet2;
					}

					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
				}
			}
		}
	}
}

void	MTopView::DrawDivineGuidanceAdvancementClassSlayerCharacter( POINT *pPoint, MCreature* pCreature, int direction )
{
	MCreatureWear*	pCreatureWear = (MCreatureWear*)pCreature;
	
	// Outputs the add-on that matches the Creature's Action.
	//action = pCreature->GetAction();
	const int tempFrame = 30;
	
	WORD clothes;
	BYTE clothesType;
	
	CCreatureFramePack& slayerFPK = pCreature->IsMale() ? m_AdvancementSlayerManFPK : m_AdvancementSlayerWomanFPK;
	CIndexSpritePack& addonISPK = pCreature->IsMale() ? m_AdvancementSlayerManSPK : m_AdvancementSlayerWomanSPK;
			
	for(int k = 0; k < 2; k++)
	{
		for (int i=0; i<ADDON_MAX; i++)
		{
			// Depending on Creature's current direction...
			// The order in which the clothes are printed may vary.
			clothesType = MCreatureWear::s_ACAddonOrder[pCreature->GetDirection()][i];
			
			// If you are wearing the i-th type of clothing, it will print the result.
			const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetACAddonInfo(clothesType);
			
			if (addonInfo.bAddon)
			{
				clothes = GetAdvancementPartFromItemClass( addonInfo.ItemClass, addonInfo.FrameID );
				
				const int tempAction = ACTION_ADVANCEMENT_SLAYER_DRAINED-ADVANCEMENT_ACTION_START;
				const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
				FRAME_ARRAY* pFA = NULL;
				if (!Phoenix_DivineGuidanceLookupFrameArray(slayerFPK, clothes, tempAction, drawDirection,
					"DivineGuidanceAdvSlayer", pCreature, &pFA))
					continue;
				FRAME_ARRAY& FA = *pFA;
				
				
				// If the action exists
				if (FA.GetSize() > tempFrame)
				{
					CFrame &Frame = FA[tempFrame];					
					int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
					int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
					int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
					
					
					CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(addonISPK, sprite,
						"DivineGuidanceAdvSlayer", pCreature, clothes, tempAction, drawDirection);
					if (pSprite == NULL)
						continue;
					
					pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
					pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
					
					//---------------------------------------- 
					// Normal character output
					//---------------------------------------- 
					int colorSet1 = addonInfo.ColorSet1, colorSet2 = addonInfo.ColorSet2;
					if(colorSet2 == UNIQUE_ITEM_COLOR || colorSet2 == QUEST_ITEM_COLOR)
					{
						colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
						if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
							colorSet1 = colorSet2;
					}
					else if(colorSet2>= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					{
						colorSet2 = pCreature->GetItemOptionExtColorSet( colorSet2 );
						if(addonInfo.ItemClass != ITEM_CLASS_COAT && addonInfo.ItemClass != ITEM_CLASS_TROUSER)
							colorSet1 = colorSet2;
					}
					CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
					
					m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
				}
			}
		}
	}
}

void	MTopView::DrawDivineGuidanceVampireCharacter( 
			POINT *pPoint, MCreature* pCreature, int direction, int body )
{
	static int tempGuidance = 0;
	tempGuidance = (g_CurrentFrame)%20;
	const int tempFrame = 30;
	

	for(int k = 0; k < 2; k++)
	{
		const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
		FRAME_ARRAY* pFA = NULL;
		if (!Phoenix_DivineGuidanceLookupFrameArray(m_VampireFPK, body, ACTION_DRAINED, drawDirection,
			"DivineGuidanceVampire", pCreature, &pFA))
			continue;
		FRAME_ARRAY& FA = *pFA;
		
		if (FA.GetSize() > tempFrame)
		{
			CFrame& Frame =	FA[tempFrame];
			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			
			// Coordinate Calibration
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
			
			CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(m_VampireSPK, sprite,
				"DivineGuidanceVampire", pCreature, body, ACTION_DRAINED, drawDirection);
			if (pSprite == NULL)
				continue;
			
			int colorSet1 = pCreature->GetBodyColor1();
			int colorSet2 = pCreature->GetBodyColor2();
			
			if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
				colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
			else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
				colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;


			if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
				colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
			else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
				colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

			CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
			
			pointTemp.y += abs(10-tempGuidance);
			m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
		}
	}
}

void	MTopView::DrawDivineGuidanceCreature( 
			POINT *pPoint, MCreature* pCreature, int direction, int body )
{
	static int tempGuidance = 0;
	tempGuidance = (g_CurrentFrame)%20;
	const int tempFrame = 30;
	

	for(int k = 0; k < 2; k++)
	{
		const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
		FRAME_ARRAY* pFA = NULL;
		if (!Phoenix_DivineGuidanceLookupFrameArray(m_CreatureFPK, body, ACTION_DRAINED, drawDirection,
			"DivineGuidanceCreature", pCreature, &pFA))
			continue;
		FRAME_ARRAY& FA = *pFA;
		
		if (FA.GetSize() > tempFrame)
		{
			CFrame& Frame =	FA[tempFrame];
			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			
			// Coordinate Calibration
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
			
			CIndexSprite* pSprite = &GetCreatureSprite(sprite, body);
			if (pSprite == NULL || pSprite->IsNotInit())
			{
				DEBUG_ADD_FORMAT("[DivineGuidanceCreature] sprite not initialized creatureType=%d body=%d action=%d direction=%d sprite=%d",
					(int)pCreature->GetCreatureType(), body, ACTION_DRAINED, drawDirection, sprite);
				CT_Done("DivineGuidanceCreature sprite not initialized creatureType=%d body=%d action=%d direction=%d sprite=%d",
					(int)pCreature->GetCreatureType(), body, ACTION_DRAINED, drawDirection, sprite);
				continue;
			}
			
			int colorSet1 = pCreature->GetBodyColor1();
			int colorSet2 = pCreature->GetBodyColor2();
			
			if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
				colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
			else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
				colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;


			if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
				colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
			else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
				colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

			CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
			
			pointTemp.y += abs(10-tempGuidance);
			m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
		}
	}
}


void	MTopView::DrawDivineGuidanceAdvancementClassVampireCharacter( 
			POINT *pPoint, MCreature* pCreature, int direction, int body )
{
	static int tempGuidance = 0;
	tempGuidance = (g_CurrentFrame)%20;
	const int tempFrame = 30;

	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	bool bCoat = addonInfo.bAddon && addonInfo.ItemClass == ITEM_CLASS_VAMPIRE_COAT;
	bool bNewCoat = bCoat && (addonInfo.ItemType == 32 || addonInfo.ItemType == 33 || addonInfo.ItemType == 34 || addonInfo.ItemType == 35 || addonInfo.ItemType == 36 || addonInfo.ItemType == 37 || addonInfo.ItemType == 38 || addonInfo.ItemType == 39);
	bool b231NewCoat = bCoat && (addonInfo.ItemType == 40 || addonInfo.ItemType == 41 || addonInfo.ItemType == 42 || addonInfo.ItemType == 43);

	int tempAction = ACTION_ADVANCEMENT_DRAINED-ADVANCEMENT_ACTION_START;
	tempAction = GetAdvancementVampireAction171[tempAction];
	if (tempAction == -1)
		return;

	CCreatureFramePack& advanceVampireFPK = pCreature->IsMale() ? (b231NewCoat ? m_AdvancementVampireMan231FPK : (bNewCoat ? m_AdvancementVampireMan201FPK : m_AdvancementVampireMan171FPK)) : (b231NewCoat ? m_AdvancementVampireWoman231FPK : (bNewCoat ? m_AdvancementVampireWoman201FPK : m_AdvancementVampireWoman171FPK));
	CIndexSpritePack& advanceVampireSPK = pCreature->IsMale() ? (b231NewCoat ? m_AdvancementVampireMan231SPK : (bNewCoat ? m_AdvancementVampireMan201SPK : m_AdvancementVampireMan171SPK)) : (b231NewCoat ? m_AdvancementVampireWoman231SPK : (bNewCoat ? m_AdvancementVampireWoman201SPK : m_AdvancementVampireWoman171SPK));

	for(int k = 0; k < 2; k++)
	{
		const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
		FRAME_ARRAY* pFA = NULL;
		if (!Phoenix_DivineGuidanceLookupFrameArray(advanceVampireFPK, 0, tempAction, drawDirection,
			"DivineGuidanceAdvVampire", pCreature, &pFA))
			continue;
		FRAME_ARRAY& FA = *pFA;
		
		if (FA.GetSize() > tempFrame)
		{
			CFrame& Frame =	FA[tempFrame];
			int sprite = Frame.GetSpriteID(),	//m_CreatureFPK[body][action][direction][frame].GetSpriteID(),
				cx		= Frame.GetCX(),	//m_CreatureFPK[body][action][direction][frame].GetCX(),
				cy		= Frame.GetCY();	//m_CreatureFPK[body][action][direction][frame].GetCY();
			
			// Coordinate Calibration
			pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
			pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
			
			CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(advanceVampireSPK, sprite,
				"DivineGuidanceAdvVampire", pCreature, 0, tempAction, drawDirection);
			if (pSprite == NULL)
				continue;
			
			int colorSet1 = pCreature->GetBodyColor1();
			int colorSet2 = pCreature->GetBodyColor2();
			
			if( colorSet1 == QUEST_ITEM_COLOR || colorSet1 == UNIQUE_ITEM_COLOR )
				colorSet1 = MItem::GetSpecialColorItemColorset( colorSet1 );
			else if(colorSet1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
				colorSet1 =  pCreature->GetItemOptionExtColorSet(colorSet1) ;

			if( colorSet2 == QUEST_ITEM_COLOR || colorSet2 == UNIQUE_ITEM_COLOR )
				colorSet2 = MItem::GetSpecialColorItemColorset( colorSet2 );
			else if(colorSet2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
				colorSet2 =  pCreature->GetItemOptionExtColorSet(colorSet2) ;

			CIndexSprite::SetUsingColorSet( colorSet1, colorSet2 );
			
			pointTemp.y += abs(10-tempGuidance);
			m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
		}
	}
}

void	MTopView::DrawDivineGuidanceOustersCharacter( 
			POINT *pPoint, MCreature* pCreature, int direction )
{
	//  There is a chakram
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	const int tempFrame = 30;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	
	int tempAction = ACTION_DRAINED;
	
	// Hit the body, then hit with the chakram
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	const MCreatureWear::ADDON_INFO& bootsAddonInfo = pCreatureWear->GetAddonInfo(ADDON_TROUSER);
	
	for(int k = 0; k < 2; k++)
	{
		if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
		{
			int clothes = addonInfo.FrameID;			
			
			const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
			FRAME_ARRAY* pFA = NULL;
			if (!Phoenix_DivineGuidanceLookupFrameArray(m_OustersFPK, clothes, tempAction, drawDirection,
				"DivineGuidanceOusters", pCreature, &pFA))
				continue;
			FRAME_ARRAY& FA = *pFA;
			
			// If the action exists
			if (FA.GetSize() > tempFrame)
			{
				CFrame &Frame = FA[tempFrame];					
				int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
				int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
				int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
				
				CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(m_OustersSPK, sprite,
					"DivineGuidanceOusters", pCreature, clothes, tempAction, drawDirection);
				if (pSprite == NULL)
					continue;
				
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
				
				CIndexSprite::SetUsingColorSetOnly( 0, pCreature->GetBodyColor1() );	// Hair color
				
				int Colorset;
				
				Colorset = addonInfo.ColorSet2;
				
				if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
					Colorset = MItem::GetSpecialColorItemColorset( Colorset );
				else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
				CIndexSprite::SetUsingColorSetOnly( 1, Colorset );	// Giysi rengi
				
				if(bChakram == false)	// Listlet'i takmışsan
				{
					if(addonInfoChakram.bAddon)
					{
						Colorset = addonInfoChakram.ColorSet2;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
						
						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// kol
					}
					else
					{
						Colorset = defaultCoatColor;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// kol
					}
				}
				else
				{
					Colorset = defaultCoatColor;
					if(addonInfoChakram.bAddon)
						Colorset = addonInfoChakram.ColorSet2;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					
					CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// kol
					
				}
				
				if(bootsAddonInfo.bAddon)
				{
					Colorset = bootsAddonInfo.ColorSet2;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					
					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// Bot rengi
				}
				else
				{
					Colorset = defaultCoatColor ;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// kol
				}
				
				m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
				
			}
		}
		
		if (bChakram && !pCreatureWear->IsGhost(2))
		{
			int clothes = addonInfoChakram.FrameID;
			
			const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
			FRAME_ARRAY* pFA = NULL;
			if (!Phoenix_DivineGuidanceLookupFrameArray(m_OustersFPK, clothes, tempAction, drawDirection,
				"DivineGuidanceOustersChakram", pCreature, &pFA))
				continue;
			FRAME_ARRAY& FA = *pFA;
			
			// eğer böyle bir hareket varsa
			if (FA.GetSize() > tempFrame)
			{
				CFrame &Frame = FA[tempFrame];					
				int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
				int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
				int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
				
				CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(m_OustersSPK, sprite,
					"DivineGuidanceOustersChakram", pCreature, clothes, tempAction, drawDirection);
				if (pSprite == NULL)
					continue;
				
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
				
				int colorset1, colorset2;
				colorset1 = addonInfoChakram.ColorSet1;
				colorset2 = addonInfoChakram.ColorSet2;
				
				if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
					colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
				else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;

				if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
					colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
				else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;
				
				CIndexSprite::SetUsingColorSet( colorset1, colorset2 );
				
				m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
			}
		}
	}
}

void	MTopView::DrawDivineGuidanceAdvancementClassOustersCharacter( 
			POINT *pPoint, MCreature* pCreature, int direction )
{
	//  There is a chakram
	MCreatureWear *pCreatureWear = (MCreatureWear *)pCreature;
	const int tempFrame = 30;
	
	const MCreatureWear::ADDON_INFO& addonInfoChakram = pCreatureWear->GetAddonInfo(ADDON_RIGHTHAND);
	bool bChakram = addonInfoChakram.bAddon && addonInfoChakram.ItemClass == ITEM_CLASS_OUSTERS_CHAKRAM;
	
	int tempAction = ACTION_ADVANCEMENT_OUSTERS_DRAINED-ADVANCEMENT_ACTION_START;
	const MCreatureWear::ADDON_INFO& addonInfo = pCreatureWear->GetAddonInfo(ADDON_COAT);
	const MCreatureWear::ADDON_INFO& bootsAddonInfo = pCreatureWear->GetAddonInfo(ADDON_TROUSER);
	
	for(int k = 0; k < 2; k++)
	{
		if (addonInfo.bAddon && !pCreatureWear->IsGhost(1))
		{
			int clothes = 1;
			
			const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
			FRAME_ARRAY* pFA = NULL;
			if (!Phoenix_DivineGuidanceLookupFrameArray(m_AdvancementOustersFPK, clothes, tempAction, drawDirection,
				"DivineGuidanceAdvOusters", pCreature, &pFA))
				continue;
			FRAME_ARRAY& FA = *pFA;
			
			// If the action exists
			if (FA.GetSize() > tempFrame)
			{
				CFrame &Frame = FA[tempFrame];					
				int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
				int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
				int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
				
				CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(m_AdvancementOustersSPK, sprite,
					"DivineGuidanceAdvOusters", pCreature, clothes, tempAction, drawDirection);
				if (pSprite == NULL)
					continue;
				
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
				
				CIndexSprite::SetUsingColorSetOnly( 0, pCreature->GetBodyColor1() );	// Saç rengi
				
				int Colorset;
				
				Colorset = addonInfo.ColorSet2;
				
				if( Colorset == QUEST_ITEM_COLOR || Colorset == UNIQUE_ITEM_COLOR )
					Colorset = MItem::GetSpecialColorItemColorset( Colorset );
				else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

				CIndexSprite::SetUsingColorSetOnly( 1, Colorset );	// Giysi rengi
				
				if(bChakram == false)	// Listlet'i takmışsan
				{
					if(addonInfoChakram.bAddon)
					{
						Colorset = addonInfoChakram.ColorSet2;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
						
						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// 팔
					}
					else
					{
						Colorset = defaultCoatColor;
						if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
							Colorset = MItem::GetSpecialColorItemColorset( Colorset );
						else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
							Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

						CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// kol
					}
				}
				else
				{
					Colorset = defaultCoatColor;
					if(addonInfoChakram.bAddon)
						Colorset = addonInfoChakram.ColorSet2;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;
					
					CIndexSprite::SetUsingColorSetOnly( 2, Colorset );	// kol
					
				}
				
				if(bootsAddonInfo.bAddon)
				{
					Colorset = bootsAddonInfo.ColorSet2;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// Bot rengi
				}
				else
				{
					Colorset = defaultCoatColor ;
					if (Colorset == QUEST_ITEM_COLOR||Colorset == UNIQUE_ITEM_COLOR )
						Colorset = MItem::GetSpecialColorItemColorset( Colorset );
					else if(Colorset >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
						Colorset =  pCreature->GetItemOptionExtColorSet(Colorset) ;

					CIndexSprite::SetUsingColorSetOnly( 3, Colorset );	// kol
				}
				
				m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
				
			}
		}
		
		if (bChakram && !pCreatureWear->IsGhost(2))
		{
			int clothes = 0;
			
			const int drawDirection = (direction+g_CurrentFrame/2+k)%8;
			FRAME_ARRAY* pFA = NULL;
			if (!Phoenix_DivineGuidanceLookupFrameArray(m_AdvancementOustersFPK, clothes, tempAction, drawDirection,
				"DivineGuidanceAdvOustersChakram", pCreature, &pFA))
				continue;
			FRAME_ARRAY& FA = *pFA;
			
			// eğer böyle bir hareket varsa
			if (FA.GetSize() > tempFrame)
			{
				CFrame &Frame = FA[tempFrame];					
				int sprite	= Frame.GetSpriteID();	//m_AddonFPK[clothes][action][direction][frame].GetSpriteID();
				int cx		= Frame.GetCX();	//m_AddonFPK[clothes][action][direction][frame].GetCX();
				int cy		= Frame.GetCY();	//m_AddonFPK[clothes][action][direction][frame].GetCY();
				
				CIndexSprite* pSprite = Phoenix_DivineGuidanceLookupIndexSprite(m_AdvancementOustersSPK, sprite,
					"DivineGuidanceAdvOustersChakram", pCreature, clothes, tempAction, drawDirection);
				if (pSprite == NULL)
					continue;
				
				pointTemp.x = pPoint->x + cx;// + pCreature->GetSX();
				pointTemp.y = pPoint->y + cy-(*g_pCreatureTable)[pCreature->GetCreatureType()].Height;// + pCreature->GetSY();
				
				int colorset1, colorset2;
				colorset1 = addonInfoChakram.ColorSet1;
				colorset2 = addonInfoChakram.ColorSet2;
				
				if( colorset1 == QUEST_ITEM_COLOR || colorset1 == UNIQUE_ITEM_COLOR )
					colorset1 = MItem::GetSpecialColorItemColorset( colorset1 );
				else if(colorset1 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset1 =  pCreature->GetItemOptionExtColorSet(colorset1) ;
				if( colorset2 == QUEST_ITEM_COLOR || colorset2 == UNIQUE_ITEM_COLOR )
					colorset2 = MItem::GetSpecialColorItemColorset( colorset2 );
				else if(colorset2 >= OPTIONEXT_ITEM_COLOR)  // 2006.02.01
					colorset2 =  pCreature->GetItemOptionExtColorSet(colorset2) ;

				CIndexSprite::SetUsingColorSet( colorset1, colorset2 );

				
				m_pSurface->BltIndexSpriteDarkness(&pointTemp, pSprite, 2-k);
			}
		}
	}
}
