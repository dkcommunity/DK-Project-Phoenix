#include "client_PCH.h"
#include "VS_UI.h"

#include "VS_UI_filepath.h"
#include "VS_UI_Message.h"
#include "VS_UI_Mouse_pointer.h"
#include "VS_UI_GameCommon.h"
#include "UserOption.h"
#include "ClientConfig.h"

//#include "ExperienceTable.h"	//char_info지울때 지울것
#include "MGuildMarkManager.h"
#include "KeyAccelerator.h"
#include "UserInformation.h"
#include <time.h>
#include "MItemOptionTable.H"
#include "mgamestringtable.H"
#include "SkinManager.h"
#include "MTypeDef.h"
#include "AddonDef.h"
#include "MCreatureWear.h"

int	g_ACAddonOrder[ DIRECTION_MAX ][ AC_ADDON_MAX ] =
{
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_RIGHTHAND, AC_ADDON_LEFTHAND },
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_RIGHTHAND, AC_ADDON_LEFTHAND },
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_LEFTHAND, AC_ADDON_RIGHTHAND },
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_LEFTHAND, AC_ADDON_RIGHTHAND },
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_LEFTHAND, AC_ADDON_RIGHTHAND },
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_LEFTHAND, AC_ADDON_RIGHTHAND },
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_RIGHTHAND, AC_ADDON_LEFTHAND },
	{ AC_ADDON_MOTOR, AC_ADDON_BODY, AC_ADDON_HELM, AC_ADDON_SHOULDER, AC_ADDON_RIGHTHAND, AC_ADDON_LEFTHAND },
};

int GetWeaponPartFromType( int type )
{
	switch( type )
	{
	case M_SWORD :
		return AC_SWORD;

	case M_BLADE :
		return AC_BLADE;

	case M_AR :
		return AC_GUN_AR;

	case M_SR :
	case M_SMG :
	case M_SG :
		return AC_GUN_SR;

	case M_MACE :
		return AC_MACE;

	case M_CROSS :
		return AC_CROSS;
	}
	return -1;
}			

int GetAdvancementSlayerMoveActionFromType( int type )
{
	switch( type )
	{
	case M_SWORD :
		return ACTION_ADVANCEMENT_SLAYER_MOVE_SWORD;

	case M_BLADE :
		return ACTION_ADVANCEMENT_SLAYER_MOVE_BLADE;

	case M_SR :
	case M_SMG :
	case M_SG :
	case M_AR :
		return ACTION_ADVANCEMENT_SLAYER_MOVE_GUN;
	}

	return ACTION_ADVANCEMENT_SLAYER_MOVE_MACE_AND_CROSS;
}

int	GetAdvancementSlayerStandActionFromType( int type )
{
	switch( type )
	{
	case M_SWORD :
		return ACTION_ADVANCEMENT_SLAYER_STOP_SWORD;

	case M_BLADE :
		return ACTION_ADVANCEMENT_SLAYER_STOP_BLADE;

	case M_SR :
	case M_SMG :
	case M_SG :
	case M_AR :
		return ACTION_ADVANCEMENT_SLAYER_STOP_GUN;		
	}

	return ACTION_ADVANCEMENT_SLAYER_STOP_MACE_AND_CROSS;
}









#if __CONTENTS(__TITLE_UI_RENWEAL)
void	C_VS_UI_NEWCHAR::_ShowCharacterOusters( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;
	int coat = p_slot->man_info.coat;
	int weapon = p_slot->man_info.right;
	
	int action = 0;
	if(enable)	// 걷기
	{
		if(weapon == 0)	// 무기 없다
			action = ACTION_OUSTERS_MOVE;
		else
			action = ACTION_MOVE;
	}
	else
	{
		if(weapon == 0)	// 무기 없다
			action = ACTION_OUSTERS_STAND;
		else
			action = ACTION_STAND;
	}
	
	index %= m_ousters_cfpk[coat][action][m_chDirection].GetSize();
	
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
	
	int weapon_color = p_slot->skin_color;
	if(weapon_color == QUEST_ITEM_COLOR )
		weapon_color = MItem::GetQuestItemColorset();
	else if(weapon_color == UNIQUE_ITEM_COLOR)
		weapon_color = MItem::GetUniqueItemColorset();
	else if( weapon_color >= OPTIONEXT_ITEM_COLOR )
		weapon_color = MItem::GetItemOptionExtColorSet(weapon_color);
	
	int hair_color = p_slot->hair_color;
	if(hair_color == QUEST_ITEM_COLOR )
		hair_color = MItem::GetQuestItemColorset();
	else if(hair_color == UNIQUE_ITEM_COLOR)
		hair_color = MItem::GetUniqueItemColorset();
	else if( hair_color >= OPTIONEXT_ITEM_COLOR )
		hair_color = MItem::GetItemOptionExtColorSet(hair_color);
	
	int trouser_color = p_slot->trouser_color;
	
	if(trouser_color == QUEST_ITEM_COLOR )
		trouser_color = MItem::GetQuestItemColorset();
	else if(trouser_color == UNIQUE_ITEM_COLOR)
		trouser_color = MItem::GetUniqueItemColorset();
	else if( trouser_color >= OPTIONEXT_ITEM_COLOR )
		trouser_color = MItem::GetItemOptionExtColorSet(trouser_color);
	
	
	
	CIndexSprite::SetUsingColorSet(hair_color, coat_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
	if(weapon)
		CIndexSprite::SetUsingColorSetOnly(2, 377);
	else
		CIndexSprite::SetUsingColorSetOnly(2, weapon_color);
	CIndexSprite::SetUsingColorSetOnly(3, trouser_color);
	
	point.x = _x+m_ousters_cfpk[coat][action][m_chDirection][index].GetCX();
	point.y = _y+m_ousters_cfpk[coat][action][m_chDirection][index].GetCY();
	
	const int sprite_id = m_ousters_cfpk[coat][action][m_chDirection][index].GetSpriteID();
	
	if(dark)
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_ousters_ispk[sprite_id], DARK_BIT);
	else
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_ousters_ispk[sprite_id]);
	
	if(weapon)	// 챠크람 찍기
	{
		index %= m_ousters_cfpk[0][action][m_chDirection].GetSize();
		
		point.x = _x+m_ousters_cfpk[0][action][m_chDirection][index].GetCX();
		point.y = _y+m_ousters_cfpk[0][action][m_chDirection][index].GetCY();
		
		const int sprite_id = m_ousters_cfpk[0][action][m_chDirection][index].GetSpriteID();
		
		CIndexSprite::SetUsingColorSet(weapon_color, weapon_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_ousters_ispk[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_ousters_ispk[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterSlayer( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	// 2007.09.06 에이 그냥 다시 만들자 - by chyaya
	const int addonOrder[] = {
		ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR,
		ADDON_HELM, ADDON_LEFTHAND, ADDON_RIGHTHAND,
	};
	
	const int DARK_BIT = 1;
	CCreatureFramePack &addonFPK = p_slot->bl_female ? m_slayer_woman_cfpk : m_slayer_man_cfpk;
	CIndexSpritePack &addonSPK = p_slot->bl_female ? m_slayer_woman_ispk : m_slayer_man_ispk;
	MAN_INFO &manInfo = p_slot->man_info;

	// MAN_INFO을 ADDON_INFO로 컨버팅
	MCreatureWear::ADDON_INFO addonInfoArray[ADDON_MAX];
	memset(addonInfoArray, 0, sizeof(MCreatureWear::ADDON_INFO) * ADDON_MAX);

	addonInfoArray[ADDON_HAIR].bAddon			= manInfo.hair != M_NO_WEAR;
	addonInfoArray[ADDON_HAIR].FrameID			= manInfo.hair;
	addonInfoArray[ADDON_HAIR].ColorSet1		= p_slot->hair_color;

	addonInfoArray[ADDON_HELM].bAddon			= manInfo.helmet != M_NO_WEAR;
	addonInfoArray[ADDON_HELM].FrameID			= manInfo.helmet;
	addonInfoArray[ADDON_HELM].ColorSet1		= p_slot->helmet_color;
	
	addonInfoArray[ADDON_COAT].bAddon			= manInfo.coat != M_NO_WEAR;
	addonInfoArray[ADDON_COAT].FrameID			= manInfo.coat;
	addonInfoArray[ADDON_COAT].ColorSet1		= p_slot->skin_color;
	addonInfoArray[ADDON_COAT].ColorSet2		= p_slot->coat_color;
	
	addonInfoArray[ADDON_TROUSER].bAddon		= manInfo.trouser != M_NO_WEAR;
	addonInfoArray[ADDON_TROUSER].FrameID		= manInfo.trouser;
	addonInfoArray[ADDON_TROUSER].ColorSet1		= p_slot->trouser_color;

	addonInfoArray[ADDON_LEFTHAND].bAddon		= manInfo.left != M_NO_WEAR;
	addonInfoArray[ADDON_LEFTHAND].FrameID		= manInfo.left;
	addonInfoArray[ADDON_LEFTHAND].ColorSet1	= p_slot->left_color;

	addonInfoArray[ADDON_RIGHTHAND].bAddon		= manInfo.right != M_NO_WEAR;
	addonInfoArray[ADDON_RIGHTHAND].FrameID		= manInfo.right;
	addonInfoArray[ADDON_RIGHTHAND].ColorSet1	= p_slot->right_color;

	const bool useGun = manInfo.right == M_SR || manInfo.right == M_AR ||
						manInfo.right == M_SG || manInfo.right == M_SMG;

	int clothes = ADDONID_COAT1;
	int action = enable ? ACTION_MOVE : ACTION_STAND;
	int direction = DOWN;

	// 총은 걷는게 다르다.
	if(action == ACTION_MOVE && useGun)
	{
		action = ACTION_SLAYER_MOVE_GUN;
	}

	CPoint drawPoint;

	// COAT1을 기준으로 프레임을 맞춘다.
	index %= addonFPK[ADDONID_COAT1][action][m_chDirection].GetSize();

	// 본격적인 그리기 시작
	for(int i = 0; i < ADDON_MAX; ++i)
	{
		// 방향별 그리는 순서에 따라 그려준다.
		int clothesType = addonOrder[i];

		const MCreatureWear::ADDON_INFO& addonInfo = addonInfoArray[clothesType];
		
		if (addonInfo.bAddon)
		{
			clothes = addonInfo.FrameID;

			const FRAME_ARRAY &FA = addonFPK[clothes][action][m_chDirection];

			if(FA.GetSize() == 0) continue;
			
			const CFrame &Frame = FA[index];					
			int sprite	= Frame.GetSpriteID();
			int cx		= Frame.GetCX();
			int cy		= Frame.GetCY();
			
			drawPoint.x = _x + cx;
			drawPoint.y = _y + cy;
			
			CIndexSprite* pSprite = &addonSPK[sprite];
			
			// 칼라셋 설정
			int colorSet1 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet1);
			int colorSet2 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet2);
			
			CIndexSprite::SetUsingColorSet(colorSet1, colorSet2);
			
			if(dark)
			{
				gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&drawPoint, pSprite, DARK_BIT);
			}
			else
			{
				gpC_base->m_p_DDSurface_back->BltIndexSprite(&drawPoint, pSprite);
			}
		}
	}
}
			

void	C_VS_UI_NEWCHAR::_ShowCharacterVampire( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark  )
{	
	const int DARK_BIT = 1;
	POINT point;

	int coat	= 0;
	int right	= 0;
	
	coat	= p_slot->man_info.coat;
	right	= p_slot->man_info.right;
	
	int body	= (coat * 2) + p_slot->bl_female;
	int action	= enable;
	int weapon	= -1;

	if(right != M_NO_WEAR && right == 1)
	{
		weapon = p_slot->bl_female;
	}
	
	FRAME_ARRAY &coatFrame = m_vampire_cfpk[body][action][m_chDirection];

	index %= coatFrame.GetSize();
	
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
	
	int skin_color = p_slot->skin_color;
	if(skin_color == QUEST_ITEM_COLOR )
		skin_color = MItem::GetQuestItemColorset();
	else if(skin_color == UNIQUE_ITEM_COLOR)
		skin_color = MItem::GetUniqueItemColorset();
	else if( skin_color >= OPTIONEXT_ITEM_COLOR )
		skin_color = MItem::GetItemOptionExtColorSet(skin_color);
	
	// 아직 머리 색상은 안들어갔다 일단 검은색으로 처리
	//int hair_color = p_slot->hair_color;
	int hair_color = 390;
	if(hair_color == QUEST_ITEM_COLOR )
		hair_color = MItem::GetQuestItemColorset();
	else if(hair_color == UNIQUE_ITEM_COLOR)
		hair_color = MItem::GetUniqueItemColorset();
	else if( hair_color >= OPTIONEXT_ITEM_COLOR )
		hair_color = MItem::GetItemOptionExtColorSet(hair_color);
	
	int right_color = p_slot->right_color;
	if(right_color == QUEST_ITEM_COLOR )
		right_color = MItem::GetQuestItemColorset();
	else if(right_color == UNIQUE_ITEM_COLOR)
		right_color = MItem::GetUniqueItemColorset();
	else if( right_color >= OPTIONEXT_ITEM_COLOR )
		right_color = MItem::GetItemOptionExtColorSet(right_color);
	
	CIndexSprite::SetUsingColorSetOnly(0, skin_color);
	CIndexSprite::SetUsingColorSetOnly(1, coat_color);
	CIndexSprite::SetUsingColorSetOnly(2, hair_color);
	
	point.x = _x + coatFrame[index].GetCX();
	point.y = _y + coatFrame[index].GetCY();


	const int sprite_id = coatFrame[index].GetSpriteID();
	
	if(dark)
	{
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_vampire_ispk[sprite_id], DARK_BIT);
	}
	else
	{
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_vampire_ispk[sprite_id]);
	}
	
	if(weapon >= 0)	// 무기 찍기
	{
		FRAME_ARRAY &weaponFrame = m_vampire_cfpk[weapon][action][m_chDirection];

		index %= weaponFrame.GetSize();
		
		point.x = _x + weaponFrame[index].GetCX();
		point.y = _y + weaponFrame[index].GetCY();
		
		const int sprite_id = weaponFrame[index].GetSpriteID();
		
		CIndexSprite::SetUsingColorSet(right_color, right_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.

		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_vampire_ispk[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_vampire_ispk[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterACOusters( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;
	int coat = 1;//p_slot->man_info.coat;
	int weapon = p_slot->man_info.right;
	
	int action = 0;

	if(enable)	// 걷기
	{
		if(weapon == 0)	// 무기 없다
		{
			bool bFirstAction = index < m_AdvancementOustersCfpk[coat][ACTION_ADVANCEMENT_OUSTERS_MAGIC-ADVANCEMENT_ACTION_START][2].GetSize();
	
			if( bFirstAction )
				action = ACTION_ADVANCEMENT_OUSTERS_MAGIC;
			else
				action = ACTION_ADVANCEMENT_OUSTERS_WRISTLET_MOVE;
		}
		else
		{
			bool bFirstAction = index < m_AdvancementOustersCfpk[coat][ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST-ADVANCEMENT_ACTION_START][2].GetSize();
			//int i =  ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST-ADVANCEMENT_ACTION_START  ; 
			//bool bFirstAction = index < m_AdvancementOustersCfpk[coat][ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST-ADVANCEMENT_ACTION_START][2].GetSize();     

			if( bFirstAction )
				action = ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST ; //ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST ; //ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST ;
			else
				action = ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_MOVE;
		}
	}
	else
	{
		if(weapon == 0)	// 무기 없다
		{
			action = ACTION_ADVANCEMENT_OUSTERS_WRISTLET_STOP;
		}
		else
			action = ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP;
	}

	action-=ADVANCEMENT_ACTION_START;
	index %= m_AdvancementOustersCfpk[coat][action][2].GetSize();
	
	
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
	
	int weapon_color = p_slot->skin_color;
	if(weapon_color == QUEST_ITEM_COLOR )
		weapon_color = MItem::GetQuestItemColorset();
	else if(weapon_color == UNIQUE_ITEM_COLOR)
		weapon_color = MItem::GetUniqueItemColorset();
	else if( weapon_color >= OPTIONEXT_ITEM_COLOR )
		weapon_color = MItem::GetItemOptionExtColorSet(weapon_color);
	
	int hair_color = p_slot->hair_color;
	if(hair_color == QUEST_ITEM_COLOR )
		hair_color = MItem::GetQuestItemColorset();
	else if(hair_color == UNIQUE_ITEM_COLOR)
		hair_color = MItem::GetUniqueItemColorset();
	else if( hair_color >= OPTIONEXT_ITEM_COLOR )
		hair_color = MItem::GetItemOptionExtColorSet(hair_color);
	
	int trouser_color = p_slot->trouser_color;
	
	if(trouser_color == QUEST_ITEM_COLOR )
		trouser_color = MItem::GetQuestItemColorset();
	else if(trouser_color == UNIQUE_ITEM_COLOR)
		trouser_color = MItem::GetUniqueItemColorset();
	else if( trouser_color >= OPTIONEXT_ITEM_COLOR )
		trouser_color = MItem::GetItemOptionExtColorSet(trouser_color);
	
	
	CIndexSprite::SetUsingColorSet(hair_color, coat_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
	if(weapon)
		CIndexSprite::SetUsingColorSetOnly(2, 377);
	else
		CIndexSprite::SetUsingColorSetOnly(2, weapon_color);
	CIndexSprite::SetUsingColorSetOnly(3, trouser_color);
	
	point.x = _x+m_AdvancementOustersCfpk[coat][action][2][index].GetCX();
	point.y = _y+m_AdvancementOustersCfpk[coat][action][2][index].GetCY();
	
	const int sprite_id = m_AdvancementOustersCfpk[coat][action][2][index].GetSpriteID();
	
	if(dark)
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_AdvancementOustersIspk[sprite_id], DARK_BIT);
	else
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_AdvancementOustersIspk[sprite_id]);
	
	if(weapon)	// 챠크람 찍기
	{
		index %= m_AdvancementOustersCfpk[0][action][2].GetSize();
		
		point.x = _x+m_AdvancementOustersCfpk[0][action][2][index].GetCX();
		point.y = _y+m_AdvancementOustersCfpk[0][action][2][index].GetCY();
		
		const int sprite_id = m_AdvancementOustersCfpk[0][action][2][index].GetSpriteID();
		
		CIndexSprite::SetUsingColorSet(weapon_color, weapon_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_AdvancementOustersIspk[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_AdvancementOustersIspk[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterACVampire( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;
	int coat = 0;//p_slot->man_info.coat;
	int weapon = p_slot->man_info.right;

	CCreatureFramePack* pCFPK = NULL;
	CIndexSpritePack* pISPK = NULL;
	
	if( p_slot->bl_female )
	{
		pCFPK = &m_AdvancementVampireWomanCfpk;
		pISPK = &m_AdvancementVampireWomanIspk;
	}
	else
	{
		pCFPK = &m_AdvancementVampireManCfpk;
		pISPK = &m_AdvancementVampireManIspk;
	}
	
	int action = 0;

	if(enable)	// 걷기
	{
		if(weapon == 0)	// 무기 없다
		{
			bool bFirstAction = index < (*pCFPK)[coat][ACTION_ADVANCEMENT_SPECIAL-ADVANCEMENT_ACTION_START][2].GetSize();
	
			if( bFirstAction )
				action = ACTION_ADVANCEMENT_SPECIAL;
			else
				action = ACTION_ADVANCEMENT_MOVE;
		}
		else
		{
			bool bFirstAction = index < (*pCFPK)[coat][ACTION_ADVANCEMENT_SKILL_FAST-ADVANCEMENT_ACTION_START][2].GetSize();

			//bool bFirstAction = index < (*pCFPK)[coat][ACTION_ADVANCEMENT_COMBO_FAST-ADVANCEMENT_ACTION_START][2].GetSize();

			if( bFirstAction )
				action = ACTION_ADVANCEMENT_SKILL_FAST ; //ACTION_ADVANCEMENT_COMBO_FAST ; //ACTION_ADVANCEMENT_SKILL_FAST ; 
			else
				action = ACTION_ADVANCEMENT_MOVE;
		}
	}
	else
	{
		if(weapon == 0)	// 무기 없다
		{
			action = ACTION_ADVANCEMENT_STOP;
		}
		else
			action = ACTION_ADVANCEMENT_STOP;
	}

	action-=ADVANCEMENT_ACTION_START;
	index %= (*pCFPK)[coat][action][2].GetSize();
		
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
		
	point.x = _x+(*pCFPK)[coat][action][2][index].GetCX();
	point.y = _y+(*pCFPK)[coat][action][2][index].GetCY();
	
	const int sprite_id = (*pCFPK)[coat][action][2][index].GetSpriteID();
	
	if(dark)
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &(*pISPK)[sprite_id], DARK_BIT);
	else
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &(*pISPK)[sprite_id]);
	
	if(weapon)	// 챠크람 찍기
	{
		index %= (*pCFPK)[1][action][2].GetSize();
		
		point.x = _x+(*pCFPK)[1][action][2][index].GetCX();
		point.y = _y+(*pCFPK)[1][action][2][index].GetCY();
		
		const int sprite_id = (*pCFPK)[1][action][2][index].GetSpriteID();
		
		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &(*pISPK)[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &(*pISPK)[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterACSlayer( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;

	MAN_INFO *info = &p_slot->man_info;
	
	// 그냥 편하게 둘다 바지색으로 설정.. - -; by sigi
	int item_color = p_slot->trouser_color;
	if(item_color == QUEST_ITEM_COLOR )
		item_color = MItem::GetQuestItemColorset();
	else if(item_color == UNIQUE_ITEM_COLOR)
		item_color = MItem::GetUniqueItemColorset();
	else if( item_color >= OPTIONEXT_ITEM_COLOR )
		item_color = MItem::GetItemOptionExtColorSet(item_color);

	CIndexSprite::SetUsingColorSet(p_slot->skin_color, item_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.

	CCreatureFramePack& slayerFPK = !p_slot->bl_female ? m_AdvancementSlayerManCfpk : m_AdvancementSlayerWomanCfpk;
	CIndexSpritePack& addonISPK = !p_slot->bl_female ? m_AdvancementSlayerManIspk : m_AdvancementSlayerWomanIspk;

	int action = 0;
	if(enable)	// 걷기
	{
		action = GetAdvancementSlayerMoveActionFromType( info->right );
	}
	else
	{
		action = GetAdvancementSlayerStandActionFromType( info->right );
	}

	action -= ADVANCEMENT_ACTION_START;

	for( int i = 0; i < AC_ADDON_MAX; ++i )
	{
		int clothes = g_ACAddonOrder[ 2 ][ i ];
		if( clothes == AC_ADDON_MOTOR )
			continue;

		switch( clothes )
		{
		case AC_ADDON_BODY :
			item_color=p_slot->coat_color;
			if(item_color == QUEST_ITEM_COLOR )
				item_color = MItem::GetQuestItemColorset();
			else if(item_color == UNIQUE_ITEM_COLOR)
				item_color = MItem::GetUniqueItemColorset();
			else if( item_color >= OPTIONEXT_ITEM_COLOR )
				item_color = MItem::GetItemOptionExtColorSet(item_color);

			CIndexSprite::SetUsingColorSet(p_slot->skin_color, item_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
			clothes = AC_BODY;
			break;

		case AC_ADDON_HELM :
			if( info->helmet == M_NO_WEAR )
				continue;

			item_color=p_slot->helmet_color;
			if(item_color == QUEST_ITEM_COLOR)
				item_color = MItem::GetQuestItemColorset();
			else if(item_color == UNIQUE_ITEM_COLOR)
				item_color = MItem::GetUniqueItemColorset();
			else if( item_color >= OPTIONEXT_ITEM_COLOR )
				item_color = MItem::GetItemOptionExtColorSet(item_color);

			CIndexSprite::SetUsingColorSet(item_color, 0); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
			clothes = AC_HELMET;
			break;

		case AC_ADDON_LEFTHAND :
			if( info->left == M_NO_WEAR )
				continue;
		case AC_ADDON_RIGHTHAND :
			if( clothes == AC_ADDON_RIGHTHAND && info->right == M_NO_WEAR )
				continue;

			if( clothes == AC_ADDON_RIGHTHAND )
			{
				item_color=p_slot->right_color;
				clothes = GetWeaponPartFromType( info->right );
				if( clothes == -1 )
					continue;
			}
			else 
			{
				item_color=p_slot->left_color;
				clothes = AC_SHIELD;
			}

			if(item_color == QUEST_ITEM_COLOR )				
				item_color = MItem::GetQuestItemColorset();
			else if(item_color == UNIQUE_ITEM_COLOR)
				item_color = MItem::GetUniqueItemColorset();
			else if( item_color >= OPTIONEXT_ITEM_COLOR )
				item_color = MItem::GetItemOptionExtColorSet(item_color);
				
			CIndexSprite::SetUsingColorSet(item_color, 0); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.

			break;

		case AC_ADDON_SHOULDER :
			clothes = AC_SHOULDER;
			break;
		}

		index %= slayerFPK[ clothes ][ action ][ 2 ].GetSize();

		int sprite_id = slayerFPK[ clothes ][ action ][ 2 ][ index ].GetSpriteID();
		point.x = _x + slayerFPK[ clothes ][ action ][ 2 ][ index ].GetCX();
		point.y = _y + slayerFPK[ clothes ][ action ][ 2 ][ index ].GetCY();

		if( dark )
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness( &point, &addonISPK[ sprite_id ], DARK_BIT );
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite( &point, &addonISPK[ sprite_id ] );
	}
}








































#else //__TITLE_UI_RENWEAL































void	C_VS_UI_NEWCHAR::_ShowCharacterOusters( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;
	int coat = p_slot->man_info.coat;
	int weapon = p_slot->man_info.right;
	
	int action = 0;
	if(enable)	// 걷기
	{
		if(weapon == 0)	// 무기 없다
			action = ACTION_OUSTERS_MOVE;
		else
			action = ACTION_MOVE;
	}
	else
	{
		if(weapon == 0)	// 무기 없다
			action = ACTION_OUSTERS_STAND;
		else
			action = ACTION_STAND;
	}
	
	index %= m_ousters_cfpk[coat][action][2].GetSize();
	
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
	
	int weapon_color = p_slot->skin_color;
	if(weapon_color == QUEST_ITEM_COLOR )
		weapon_color = MItem::GetQuestItemColorset();
	else if(weapon_color == UNIQUE_ITEM_COLOR)
		weapon_color = MItem::GetUniqueItemColorset();
	else if( weapon_color >= OPTIONEXT_ITEM_COLOR )
		weapon_color = MItem::GetItemOptionExtColorSet(weapon_color);
	
	int hair_color = p_slot->hair_color;
	if(hair_color == QUEST_ITEM_COLOR )
		hair_color = MItem::GetQuestItemColorset();
	else if(hair_color == UNIQUE_ITEM_COLOR)
		hair_color = MItem::GetUniqueItemColorset();
	else if( hair_color >= OPTIONEXT_ITEM_COLOR )
		hair_color = MItem::GetItemOptionExtColorSet(hair_color);
	
	int trouser_color = p_slot->trouser_color;
	
	if(trouser_color == QUEST_ITEM_COLOR )
		trouser_color = MItem::GetQuestItemColorset();
	else if(trouser_color == UNIQUE_ITEM_COLOR)
		trouser_color = MItem::GetUniqueItemColorset();
	else if( trouser_color >= OPTIONEXT_ITEM_COLOR )
		trouser_color = MItem::GetItemOptionExtColorSet(trouser_color);
	
	
	
	CIndexSprite::SetUsingColorSet(hair_color, coat_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
	if(weapon)
		CIndexSprite::SetUsingColorSetOnly(2, 377);
	else
		CIndexSprite::SetUsingColorSetOnly(2, weapon_color);
	CIndexSprite::SetUsingColorSetOnly(3, trouser_color);
	
	point.x = _x+m_ousters_cfpk[coat][action][2][index].GetCX();
	point.y = _y+m_ousters_cfpk[coat][action][2][index].GetCY();
	
	const int sprite_id = m_ousters_cfpk[coat][action][2][index].GetSpriteID();
	
	if(dark)
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_ousters_ispk[sprite_id], DARK_BIT);
	else
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_ousters_ispk[sprite_id]);
	
	if(weapon)	// 챠크람 찍기
	{
		index %= m_ousters_cfpk[0][action][2].GetSize();
		
		point.x = _x+m_ousters_cfpk[0][action][2][index].GetCX();
		point.y = _y+m_ousters_cfpk[0][action][2][index].GetCY();
		
		const int sprite_id = m_ousters_cfpk[0][action][2][index].GetSpriteID();
		
		CIndexSprite::SetUsingColorSet(weapon_color, weapon_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_ousters_ispk[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_ousters_ispk[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterSlayer( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	// 2007.09.06 에이 그냥 다시 만들자 - by chyaya
	const int addonOrder[] = {
		ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR,
		ADDON_HELM, ADDON_LEFTHAND, ADDON_RIGHTHAND,
	};
	
	const int DARK_BIT = 1;
	CCreatureFramePack &addonFPK = p_slot->bl_female ? m_slayer_woman_cfpk : m_slayer_man_cfpk;
	CIndexSpritePack &addonSPK = p_slot->bl_female ? m_slayer_woman_ispk : m_slayer_man_ispk;
	MAN_INFO &manInfo = p_slot->man_info;

	// MAN_INFO을 ADDON_INFO로 컨버팅
	MCreatureWear::ADDON_INFO addonInfoArray[ADDON_MAX];
	memset(addonInfoArray, 0, sizeof(MCreatureWear::ADDON_INFO) * ADDON_MAX);

	addonInfoArray[ADDON_HAIR].bAddon			= manInfo.hair != M_NO_WEAR;
	addonInfoArray[ADDON_HAIR].FrameID			= manInfo.hair;
	addonInfoArray[ADDON_HAIR].ColorSet1		= p_slot->hair_color;

	addonInfoArray[ADDON_HELM].bAddon			= manInfo.helmet != M_NO_WEAR;
	addonInfoArray[ADDON_HELM].FrameID			= manInfo.helmet;
	addonInfoArray[ADDON_HELM].ColorSet1		= p_slot->helmet_color;
	
	addonInfoArray[ADDON_COAT].bAddon			= manInfo.coat != M_NO_WEAR;
	addonInfoArray[ADDON_COAT].FrameID			= manInfo.coat;
	addonInfoArray[ADDON_COAT].ColorSet1		= p_slot->skin_color;
	addonInfoArray[ADDON_COAT].ColorSet2		= p_slot->coat_color;
	
	addonInfoArray[ADDON_TROUSER].bAddon		= manInfo.trouser != M_NO_WEAR;
	addonInfoArray[ADDON_TROUSER].FrameID		= manInfo.trouser;
	addonInfoArray[ADDON_TROUSER].ColorSet1		= p_slot->trouser_color;

	addonInfoArray[ADDON_LEFTHAND].bAddon		= manInfo.left != M_NO_WEAR;
	addonInfoArray[ADDON_LEFTHAND].FrameID		= manInfo.left;
	addonInfoArray[ADDON_LEFTHAND].ColorSet1	= p_slot->left_color;

	addonInfoArray[ADDON_RIGHTHAND].bAddon		= manInfo.right != M_NO_WEAR;
	addonInfoArray[ADDON_RIGHTHAND].FrameID		= manInfo.right;
	addonInfoArray[ADDON_RIGHTHAND].ColorSet1	= p_slot->right_color;

	const bool useGun = manInfo.right == M_SR || manInfo.right == M_AR ||
						manInfo.right == M_SG || manInfo.right == M_SMG;

	int clothes = ADDONID_COAT1;
	int action = enable ? ACTION_MOVE : ACTION_STAND;
	int direction = DOWN;

	// 총은 걷는게 다르다.
	if(action == ACTION_MOVE && useGun)
	{
		action = ACTION_SLAYER_MOVE_GUN;
	}

	CPoint drawPoint;

	// COAT1을 기준으로 프레임을 맞춘다.
	index %= addonFPK[ADDONID_COAT1][action][direction].GetSize();

	// 본격적인 그리기 시작
	for(int i = 0; i < ADDON_MAX; ++i)
	{
		// 방향별 그리는 순서에 따라 그려준다.
		int clothesType = addonOrder[i];

		const MCreatureWear::ADDON_INFO& addonInfo = addonInfoArray[clothesType];
		
		if (addonInfo.bAddon)
		{
			clothes = addonInfo.FrameID;

			const FRAME_ARRAY &FA = addonFPK[clothes][action][direction];

			if(FA.GetSize() == 0) continue;
			
			const CFrame &Frame = FA[index];					
			int sprite	= Frame.GetSpriteID();
			int cx		= Frame.GetCX();
			int cy		= Frame.GetCY();
			
			drawPoint.x = _x + cx;
			drawPoint.y = _y + cy;
			
			CIndexSprite* pSprite = &addonSPK[sprite];
			
			// 칼라셋 설정
			int colorSet1 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet1);
			int colorSet2 = MItem::GetSpecialColorItemColorset(addonInfo.ColorSet2);
			
			CIndexSprite::SetUsingColorSet(colorSet1, colorSet2);
			
			if(dark)
			{
				gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&drawPoint, pSprite, DARK_BIT);
			}
			else
			{
				gpC_base->m_p_DDSurface_back->BltIndexSprite(&drawPoint, pSprite);
			}
		}
	}
}
			

void	C_VS_UI_NEWCHAR::_ShowCharacterVampire( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark  )
{	
	const int DARK_BIT = 1;
	POINT point;

	int coat	= 0;
	int right	= 0;
	
	coat	= p_slot->man_info.coat;
	right	= p_slot->man_info.right;
	
	int body	= (coat * 2) + p_slot->bl_female;
	int action	= enable;
	int weapon	= -1;

	if(right != M_NO_WEAR && right == 1)
	{
		weapon = p_slot->bl_female;
	}
	
	FRAME_ARRAY &coatFrame = m_vampire_cfpk[body][action][2];

	index %= coatFrame.GetSize();
	
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
	
	int skin_color = p_slot->skin_color;
	if(skin_color == QUEST_ITEM_COLOR )
		skin_color = MItem::GetQuestItemColorset();
	else if(skin_color == UNIQUE_ITEM_COLOR)
		skin_color = MItem::GetUniqueItemColorset();
	else if( skin_color >= OPTIONEXT_ITEM_COLOR )
		skin_color = MItem::GetItemOptionExtColorSet(skin_color);
	
	// 아직 머리 색상은 안들어갔다 일단 검은색으로 처리
	//int hair_color = p_slot->hair_color;
	int hair_color = 390;
	if(hair_color == QUEST_ITEM_COLOR )
		hair_color = MItem::GetQuestItemColorset();
	else if(hair_color == UNIQUE_ITEM_COLOR)
		hair_color = MItem::GetUniqueItemColorset();
	else if( hair_color >= OPTIONEXT_ITEM_COLOR )
		hair_color = MItem::GetItemOptionExtColorSet(hair_color);
	
	int right_color = p_slot->right_color;
	if(right_color == QUEST_ITEM_COLOR )
		right_color = MItem::GetQuestItemColorset();
	else if(right_color == UNIQUE_ITEM_COLOR)
		right_color = MItem::GetUniqueItemColorset();
	else if( right_color >= OPTIONEXT_ITEM_COLOR )
		right_color = MItem::GetItemOptionExtColorSet(right_color);
	
	CIndexSprite::SetUsingColorSetOnly(0, skin_color);
	CIndexSprite::SetUsingColorSetOnly(1, coat_color);
	CIndexSprite::SetUsingColorSetOnly(2, hair_color);
	
	point.x = _x + coatFrame[index].GetCX();
	point.y = _y + coatFrame[index].GetCY();



	const int sprite_id = coatFrame[index].GetSpriteID();
	
	if(dark)
	{
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_vampire_ispk[sprite_id], DARK_BIT);
	}
	else
	{
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_vampire_ispk[sprite_id]);
	}
	
	if(weapon >= 0)	// 무기 찍기
	{
		FRAME_ARRAY &weaponFrame = m_vampire_cfpk[weapon][action][2];

		index %= weaponFrame.GetSize();
		
		point.x = _x + weaponFrame[index].GetCX();
		point.y = _y + weaponFrame[index].GetCY();
		
		const int sprite_id = weaponFrame[index].GetSpriteID();
		
		CIndexSprite::SetUsingColorSet(right_color, right_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.

		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_vampire_ispk[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_vampire_ispk[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterACOusters( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;
	int coat = 1;//p_slot->man_info.coat;
	int weapon = p_slot->man_info.right;
	
	int action = 0;

	if(enable)	// 걷기
	{
		if(weapon == 0)	// 무기 없다
		{
			bool bFirstAction = index < m_AdvancementOustersCfpk[coat][ACTION_ADVANCEMENT_OUSTERS_MAGIC-ADVANCEMENT_ACTION_START][2].GetSize();
	
			if( bFirstAction )
				action = ACTION_ADVANCEMENT_OUSTERS_MAGIC;
			else
				action = ACTION_ADVANCEMENT_OUSTERS_WRISTLET_MOVE;
		}
		else
		{
			bool bFirstAction = index < m_AdvancementOustersCfpk[coat][ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST-ADVANCEMENT_ACTION_START][2].GetSize();
			//int i =  ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST-ADVANCEMENT_ACTION_START  ; 
			//bool bFirstAction = index < m_AdvancementOustersCfpk[coat][ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST-ADVANCEMENT_ACTION_START][2].GetSize();     

			if( bFirstAction )
				action = ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST ; //ACTION_ADVANCEMENT_OUSTERS_COMBO_FAST ; //ACTION_ADVANCEMENT_OUSTERS_SKILL_FAST ;
			else
				action = ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_MOVE;
		}
	}
	else
	{
		if(weapon == 0)	// 무기 없다
		{
			action = ACTION_ADVANCEMENT_OUSTERS_WRISTLET_STOP;
		}
		else
			action = ACTION_ADVANCEMENT_OUSTERS_CHAKRAM_STOP;
	}

	action-=ADVANCEMENT_ACTION_START;
	index %= m_AdvancementOustersCfpk[coat][action][2].GetSize();
	
	
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
	
	int weapon_color = p_slot->skin_color;
	if(weapon_color == QUEST_ITEM_COLOR )
		weapon_color = MItem::GetQuestItemColorset();
	else if(weapon_color == UNIQUE_ITEM_COLOR)
		weapon_color = MItem::GetUniqueItemColorset();
	else if( weapon_color >= OPTIONEXT_ITEM_COLOR )
		weapon_color = MItem::GetItemOptionExtColorSet(weapon_color);
	
	int hair_color = p_slot->hair_color;
	if(hair_color == QUEST_ITEM_COLOR )
		hair_color = MItem::GetQuestItemColorset();
	else if(hair_color == UNIQUE_ITEM_COLOR)
		hair_color = MItem::GetUniqueItemColorset();
	else if( hair_color >= OPTIONEXT_ITEM_COLOR )
		hair_color = MItem::GetItemOptionExtColorSet(hair_color);
	
	int trouser_color = p_slot->trouser_color;
	
	if(trouser_color == QUEST_ITEM_COLOR )
		trouser_color = MItem::GetQuestItemColorset();
	else if(trouser_color == UNIQUE_ITEM_COLOR)
		trouser_color = MItem::GetUniqueItemColorset();
	else if( trouser_color >= OPTIONEXT_ITEM_COLOR )
		trouser_color = MItem::GetItemOptionExtColorSet(trouser_color);
	
	
	CIndexSprite::SetUsingColorSet(hair_color, coat_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
	if(weapon)
		CIndexSprite::SetUsingColorSetOnly(2, 377);
	else
		CIndexSprite::SetUsingColorSetOnly(2, weapon_color);
	CIndexSprite::SetUsingColorSetOnly(3, trouser_color);
	
	point.x = _x+m_AdvancementOustersCfpk[coat][action][2][index].GetCX();
	point.y = _y+m_AdvancementOustersCfpk[coat][action][2][index].GetCY();
	
	const int sprite_id = m_AdvancementOustersCfpk[coat][action][2][index].GetSpriteID();
	
	if(dark)
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_AdvancementOustersIspk[sprite_id], DARK_BIT);
	else
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_AdvancementOustersIspk[sprite_id]);
	
	if(weapon)	// 챠크람 찍기
	{
		index %= m_AdvancementOustersCfpk[0][action][2].GetSize();
		
		point.x = _x+m_AdvancementOustersCfpk[0][action][2][index].GetCX();
		point.y = _y+m_AdvancementOustersCfpk[0][action][2][index].GetCY();
		
		const int sprite_id = m_AdvancementOustersCfpk[0][action][2][index].GetSpriteID();
		
		CIndexSprite::SetUsingColorSet(weapon_color, weapon_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &m_AdvancementOustersIspk[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &m_AdvancementOustersIspk[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterACVampire( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;
	int coat = 0;//p_slot->man_info.coat;
	int weapon = p_slot->man_info.right;

	CCreatureFramePack* pCFPK = NULL;
	CIndexSpritePack* pISPK = NULL;
	
	if( p_slot->bl_female )
	{
		pCFPK = &m_AdvancementVampireWomanCfpk;
		pISPK = &m_AdvancementVampireWomanIspk;
	}
	else
	{
		pCFPK = &m_AdvancementVampireManCfpk;
		pISPK = &m_AdvancementVampireManIspk;
	}
	
	int action = 0;

	if(enable)	// 걷기
	{
		if(weapon == 0)	// 무기 없다
		{
			bool bFirstAction = index < (*pCFPK)[coat][ACTION_ADVANCEMENT_SPECIAL-ADVANCEMENT_ACTION_START][2].GetSize();
	
			if( bFirstAction )
				action = ACTION_ADVANCEMENT_SPECIAL;
			else
				action = ACTION_ADVANCEMENT_MOVE;
		}
		else
		{
			bool bFirstAction = index < (*pCFPK)[coat][ACTION_ADVANCEMENT_SKILL_FAST-ADVANCEMENT_ACTION_START][2].GetSize();

			//bool bFirstAction = index < (*pCFPK)[coat][ACTION_ADVANCEMENT_COMBO_FAST-ADVANCEMENT_ACTION_START][2].GetSize();

			if( bFirstAction )
				action = ACTION_ADVANCEMENT_SKILL_FAST ; //ACTION_ADVANCEMENT_COMBO_FAST ; //ACTION_ADVANCEMENT_SKILL_FAST ; 
			else
				action = ACTION_ADVANCEMENT_MOVE;
		}
	}
	else
	{
		if(weapon == 0)	// 무기 없다
		{
			action = ACTION_ADVANCEMENT_STOP;
		}
		else
			action = ACTION_ADVANCEMENT_STOP;
	}

	action-=ADVANCEMENT_ACTION_START;
	index %= (*pCFPK)[coat][action][2].GetSize();
		
	int coat_color = p_slot->coat_color;
	if(coat_color == QUEST_ITEM_COLOR )
		coat_color = MItem::GetQuestItemColorset();
	else if(coat_color == UNIQUE_ITEM_COLOR)
		coat_color = MItem::GetUniqueItemColorset();
	else if( coat_color >= OPTIONEXT_ITEM_COLOR )
		coat_color = MItem::GetItemOptionExtColorSet(coat_color);
		
	point.x = _x+(*pCFPK)[coat][action][2][index].GetCX();
	point.y = _y+(*pCFPK)[coat][action][2][index].GetCY();
	
	const int sprite_id = (*pCFPK)[coat][action][2][index].GetSpriteID();
	
	if(dark)
		gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &(*pISPK)[sprite_id], DARK_BIT);
	else
		gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &(*pISPK)[sprite_id]);
	
	if(weapon)	// 챠크람 찍기
	{
		index %= (*pCFPK)[1][action][2].GetSize();
		
		point.x = _x+(*pCFPK)[1][action][2][index].GetCX();
		point.y = _y+(*pCFPK)[1][action][2][index].GetCY();
		
		const int sprite_id = (*pCFPK)[1][action][2][index].GetSpriteID();
		
		if(dark)
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness(&point, &(*pISPK)[sprite_id], DARK_BIT);
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite(&point, &(*pISPK)[sprite_id]);
	}
}

void	C_VS_UI_NEWCHAR::_ShowCharacterACSlayer( int _x, int _y, S_SLOT* p_slot, int index, int enable, int dark )
{
	const int DARK_BIT = 1;
	POINT point;

	MAN_INFO *info = &p_slot->man_info;
	
	// 그냥 편하게 둘다 바지색으로 설정.. - -; by sigi
	int item_color = p_slot->trouser_color;
	if(item_color == QUEST_ITEM_COLOR )
		item_color = MItem::GetQuestItemColorset();
	else if(item_color == UNIQUE_ITEM_COLOR)
		item_color = MItem::GetUniqueItemColorset();
	else if( item_color >= OPTIONEXT_ITEM_COLOR )
		item_color = MItem::GetItemOptionExtColorSet(item_color);

	CIndexSprite::SetUsingColorSet(p_slot->skin_color, item_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.

	CCreatureFramePack& slayerFPK = !p_slot->bl_female ? m_AdvancementSlayerManCfpk : m_AdvancementSlayerWomanCfpk;
	CIndexSpritePack& addonISPK = !p_slot->bl_female ? m_AdvancementSlayerManIspk : m_AdvancementSlayerWomanIspk;

	int action = 0;
	if(enable)	// 걷기
	{
		action = GetAdvancementSlayerMoveActionFromType( info->right );
	}
	else
	{
		action = GetAdvancementSlayerStandActionFromType( info->right );
	}

	action -= ADVANCEMENT_ACTION_START;

	for( int i = 0; i < AC_ADDON_MAX; ++i )
	{
		int clothes = g_ACAddonOrder[ 2 ][ i ];
		if( clothes == AC_ADDON_MOTOR )
			continue;

		switch( clothes )
		{
		case AC_ADDON_BODY :
			item_color=p_slot->coat_color;
			if(item_color == QUEST_ITEM_COLOR )
				item_color = MItem::GetQuestItemColorset();
			else if(item_color == UNIQUE_ITEM_COLOR)
				item_color = MItem::GetUniqueItemColorset();
			else if( item_color >= OPTIONEXT_ITEM_COLOR )
				item_color = MItem::GetItemOptionExtColorSet(item_color);

			CIndexSprite::SetUsingColorSet(p_slot->skin_color, item_color); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
			clothes = AC_BODY;
			break;

		case AC_ADDON_HELM :
			if( info->helmet == M_NO_WEAR )
				continue;

			item_color=p_slot->helmet_color;
			if(item_color == QUEST_ITEM_COLOR)
				item_color = MItem::GetQuestItemColorset();
			else if(item_color == UNIQUE_ITEM_COLOR)
				item_color = MItem::GetUniqueItemColorset();
			else if( item_color >= OPTIONEXT_ITEM_COLOR )
				item_color = MItem::GetItemOptionExtColorSet(item_color);

			CIndexSprite::SetUsingColorSet(item_color, 0); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.
			clothes = AC_HELMET;
			break;

		case AC_ADDON_LEFTHAND :
			if( info->left == M_NO_WEAR )
				continue;
		case AC_ADDON_RIGHTHAND :
			if( clothes == AC_ADDON_RIGHTHAND && info->right == M_NO_WEAR )
				continue;

			if( clothes == AC_ADDON_RIGHTHAND )
			{
				item_color=p_slot->right_color;
			clothes = GetWeaponPartFromType( info->right );
				if( clothes == -1 )
					continue;
			}
			else 
			{
				item_color=p_slot->left_color;
				clothes = AC_SHIELD;
			}

			if(item_color == QUEST_ITEM_COLOR )				
				item_color = MItem::GetQuestItemColorset();
			else if(item_color == UNIQUE_ITEM_COLOR)
				item_color = MItem::GetUniqueItemColorset();
			else if( item_color >= OPTIONEXT_ITEM_COLOR )
				item_color = MItem::GetItemOptionExtColorSet(item_color);
				
			CIndexSprite::SetUsingColorSet(item_color, 0); // blt와 한쌍을 이뤄야 함. 출력직전에 설정.

			break;

		case AC_ADDON_SHOULDER :
			clothes = AC_SHOULDER;
			break;
		}

		index %= slayerFPK[ clothes ][ action ][ 2 ].GetSize();

		int sprite_id = slayerFPK[ clothes ][ action ][ 2 ][ index ].GetSpriteID();
		point.x = _x + slayerFPK[ clothes ][ action ][ 2 ][ index ].GetCX();
		point.y = _y + slayerFPK[ clothes ][ action ][ 2 ][ index ].GetCY();

		if( dark )
			gpC_base->m_p_DDSurface_back->BltIndexSpriteDarkness( &point, &addonISPK[ sprite_id ], DARK_BIT );
		else
			gpC_base->m_p_DDSurface_back->BltIndexSprite( &point, &addonISPK[ sprite_id ] );
	}
}





#endif //__TITLE_UI_RENWEAL