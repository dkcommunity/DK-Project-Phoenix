//----------------------------------------------------------------------
// MCreatureWear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "MCreatureWear.h"
#include "MItem.h"
#include "AddonDef.h"
#include "SkillDef.h"
#include "DebugInfo.h"
#include "CIndexSprite.h"
#include "ClientConfig.h"
#include "MEffectGeneratorTable.h"
#include "MEffectSpriteTypeTable.h"
#include "EffectSpriteTypeDef.h"
#include "MItemOptionTable.h"
#include "PacketFunction.h"
#include "MinTR.H"
#include "MTopView.h"
#include "MPlayer.h"
#include "SoundDef.h"
#include "ClientDef.h"

int defaultSkinColor		= 410;					
int defaultCoatColor		= 377;//368;	
int defaultTrouserColor	= 377;//489;	


#define	ATTACHEFFECTCOLOR_NULL		0xFFFF

//----------------------------------------------------------------------
// static member
//----------------------------------------------------------------------
// Creature�� ���� ���⿡ ����...
// ���� ������ִ� ������ �ٸ� �� �ִ�.
// �� ���⺰�� �� ������ִ� ������ ���ϴ� �κ��̴�.
int			MCreatureWear::s_AddonOrder[DIRECTION_MAX][ADDON_MAX] = 
{
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_RIGHTHAND, ADDON_LEFTHAND },	// LEFT
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_RIGHTHAND, ADDON_LEFTHAND },	// LEFTDOWN
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_LEFTHAND, ADDON_RIGHTHAND },	// DOWN	
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_LEFTHAND, ADDON_RIGHTHAND },	// RIGHTDOWN	
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_LEFTHAND, ADDON_RIGHTHAND },	// RIGHT
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_LEFTHAND, ADDON_RIGHTHAND },	// RIGHTUP
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_RIGHTHAND, ADDON_LEFTHAND },	// UP
	{ ADDON_MOTOR, ADDON_TROUSER, ADDON_COAT, ADDON_HAIR, ADDON_HELM, ADDON_RIGHTHAND, ADDON_LEFTHAND }		// LEFTUP
};

int	MCreatureWear::s_ACAddonOrder[ DIRECTION_MAX ][ AC_ADDON_MAX ] =
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


//----------------------------------------------------------------------
//
//  constructor/destructor
//
//----------------------------------------------------------------------
MCreatureWear::MCreatureWear()
{
	for (int i=0; i<ADDON_MAX; i++)
	{
		ClearAddonInfo( i );
	}

	m_HairFrameID = ADDONID_HAIR1;
	m_HairColorSet = 0;

	m_ShoulderAddon.bAddon = FALSE;
	m_ShoulderAddon.FrameID	= FRAMEID_NULL;		// ����

	m_ShoulderAddon.ItemClass	= ITEM_CLASS_NULL;	// item class
	m_ShoulderAddon.ItemType	= ITEMTYPE_NULL;			// item type
	m_ShoulderAddon.ColorSet1	= 0;						// colorset1
	m_ShoulderAddon.ColorSet2	= 0;						// colorset2
	m_ShoulderAddon.bEffectColor = FALSE;					// effect color�� �������� �����ΰ�?
	m_ShoulderAddon.EffectColorSet	= 0;				// effectcolorset
}

MCreatureWear::~MCreatureWear()
{
}

//----------------------------------------------------------------------
//
//  member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Clear AddonInfo
//----------------------------------------------------------------------
void			
MCreatureWear::ClearAddonInfo(int Addon)
{
	if(IsAdvancementClass() && Addon == ADDON_MAX)
	{
		ADDON_INFO& addon = m_ShoulderAddon;

		// �ʱ�ȭ
		addon.bAddon	= FALSE;			// �����߳�?	
		addon.FrameID	= FRAMEID_NULL;		// ����

		addon.ItemClass	= ITEM_CLASS_NULL;	// item class
		addon.ItemType	= ITEMTYPE_NULL;			// item type
		addon.ColorSet1	= 0;						// colorset1
		addon.ColorSet2	= 0;						// colorset2
		addon.bEffectColor = FALSE;					// effect color�� �������� �����ΰ�?
		addon.EffectColorSet	= 0;				// effectcolorset

	}
	else
	{
		ADDON_INFO& addon = m_Addon[Addon];

		// �ʱ�ȭ
		addon.bAddon	= FALSE;			// �����߳�?	
		addon.FrameID	= FRAMEID_NULL;		// ����

		addon.ItemClass	= ITEM_CLASS_NULL;	// item class
		addon.ItemType	= ITEMTYPE_NULL;			// item type
		addon.ColorSet1	= 0;						// colorset1
		addon.ColorSet2	= 0;						// colorset2
		addon.bEffectColor = FALSE;					// effect color�� �������� �����ΰ�?
		addon.EffectColorSet	= 0;				// effectcolorset
	}
}

//----------------------------------------------------------------------
// Set Same Wear
//----------------------------------------------------------------------
void				
MCreatureWear::SetSameWear(const MCreatureWear* pCreature)
{
	// ������ ����	
	for (int i=0; i<ADDON_MAX; i++)
	{
		m_Addon[i] = pCreature->m_Addon[i];
	}

	m_HairFrameID = pCreature->m_HairFrameID;
	m_HairColorSet = pCreature->m_HairColorSet;
}

//----------------------------------------------------------------------
// Set Addon ColorSet1
//----------------------------------------------------------------------
void			
MCreatureWear::SetAddonColorSet1(int Addon, WORD colorSet)	
{ 
	if (Addon >= ADDON_MAX)
	{
		return;
	}
	
	// 2004, 5, 4 sobeit add start - �ٸ� ĳ���� �ƿ콺���� ������ �����ϴ� �ڵ尡 ��� ������ �߰�
	if(Addon == ADDON_TROUSER)
		m_Addon[Addon].bAddon = true;
	// 2004, 5, 4 sobeit add end

	if (colorSet < MAX_COLORSET || colorSet == UNIQUE_ITEM_COLOR || colorSet == QUEST_ITEM_COLOR)
	{	
		m_Addon[Addon].ColorSet1 = colorSet; 		
	}
	else
	{
		DEBUG_ADD_FORMAT("[Error] Exceed Maximum ColorSet. addon=%d, set=%d", Addon, colorSet);		
	}
}

//----------------------------------------------------------------------
// Set Addon ColorSet2
//----------------------------------------------------------------------
void			
MCreatureWear::SetAddonColorSet2(int Addon, WORD colorSet)	
{ 
	if (Addon >= ADDON_MAX)
	{
		return;
	}

	// 2004, 5, 4 sobeit add start - �ٸ� ĳ���� �ƿ콺���� ������ �����ϴ� �ڵ尡 ��� ������ �߰�
	if(Addon == ADDON_TROUSER)
		m_Addon[Addon].bAddon = true;
	// 2004, 5, 4 sobeit add end
	
	if (colorSet < MAX_COLORSET || colorSet == UNIQUE_ITEM_COLOR || colorSet == QUEST_ITEM_COLOR)
	{	
		// ��ڴ� set1�̴�. T_T;;
		if (m_CreatureType==CREATURETYPE_SLAYER_OPERATOR
			&& Addon==ADDON_COAT)
		{
			m_ColorBody1 = m_ColorBody2 = colorSet;
		}
		
		m_Addon[Addon].ColorSet2 = colorSet; 		
	}
	else
	{
		DEBUG_ADD_FORMAT("[Error] Exceed Maximum ColorSet. addon=%d, set=%d", Addon, colorSet);
	}
}

//----------------------------------------------------------------------
// New Item From AddonInfo
//----------------------------------------------------------------------
// Addon��° ������ Item�� �����Ѵ�.
// �ܺο��� delete����� �Ѵ�.
//----------------------------------------------------------------------
MItem*
MCreatureWear::NewItemFromAddonInfo(int Addon)
{
	if(IsAdvancementClass() && Addon == ADDON_MAX)
	{
		ADDON_INFO& addon = m_ShoulderAddon;

		//--------------------------------------------------
		// item�� ������ ���
		//--------------------------------------------------
		if (!addon.bAddon || addon.ItemClass==ITEM_CLASS_NULL)
		{
			return NULL;
		}
		
		//--------------------------------------------------
		// item�� ������ �˱����ؼ� �����ؼ� �����Ѵ�.
		//--------------------------------------------------
		MItem* pItem = MItem::NewItem( addon.ItemClass );
		pItem->SetItemType( addon.ItemType );

		return pItem;
	}
	else
	{
		if (Addon >= ADDON_MAX)
		{
			return false;
		}

		ADDON_INFO& addon = m_Addon[Addon];

		//--------------------------------------------------
		// item�� ������ ���
		//--------------------------------------------------
		if (!addon.bAddon || addon.ItemClass==ITEM_CLASS_NULL)
		{
			return NULL;
		}
		
		//--------------------------------------------------
		// item�� ������ �˱����ؼ� �����ؼ� �����Ѵ�.
		//--------------------------------------------------
		MItem* pItem = MItem::NewItem( addon.ItemClass );
		pItem->SetItemType( addon.ItemType );

		return pItem;
	}
	return NULL;
}

//----------------------------------------------------------------------
// Remove Addon
//----------------------------------------------------------------------
// Addon�� ��ġ�� ������ ���ش�.
//----------------------------------------------------------------------
bool		
MCreatureWear::RemoveAddon(int Addon)
{ 
	DEBUG_ADD("MCreatureWear::RemoveAddon");
	
	// shoulder
	if(IsAdvancementClass() && Addon == ADDON_MAX)
	{
		ADDON_INFO& addon = m_ShoulderAddon;

		//--------------------------------------------------
		// �����Ѱ� ���� ��쿡�� �����. - -;
		//--------------------------------------------------
		if (addon.bAddon)
		{
			MItem* pItem = NewItemFromAddonInfo( Addon );
			
			if (pItem!=NULL)
			{
				//--------------------------------------------------
				// addon Item�� ���忡�� �����Ѵ�.
				//--------------------------------------------------
				bool bRemove = RemoveAddonItem( pItem );

				delete pItem;

				return bRemove;
			}

			//--------------------------------------------------
			// ���� ����..
			//--------------------------------------------------
			ClearAddonInfo( Addon );

			return true;
		}
	}
	else
	{

		if (Addon >= ADDON_MAX)
		{
			return false;
		}

		ADDON_INFO& addon = m_Addon[Addon];

		//--------------------------------------------------
		// �����Ѱ� ���� ��쿡�� �����. - -;
		//--------------------------------------------------
		if (addon.bAddon)
		{
			MItem* pItem = NewItemFromAddonInfo( Addon );
			
			if (pItem!=NULL)
			{
				//--------------------------------------------------
				// addon Item�� ���忡�� �����Ѵ�.
				//--------------------------------------------------
				bool bRemove = RemoveAddonItem( pItem );

				delete pItem;
				pItem = NULL;

				return bRemove;
			}

			//--------------------------------------------------
			// ���� ����..
			//--------------------------------------------------
			ClearAddonInfo( Addon );

			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
// Set AddonItem
//---------------------------------------------------------------------------
// �Ӹ�ī���� �����Ѵ�.
//---------------------------------------------------------------------------
void			
MCreatureWear::SetAddonHair(TYPE_FRAMEID id, WORD cs1)
{
	ADDON_INFO& addon = m_Addon[ADDON_HAIR];

	// �ʱ�ȭ
	addon.bAddon	= TRUE;			// �����߳�?	
	addon.FrameID	= id;		// ����

	addon.ItemClass	= ITEM_CLASS_NULL;	// item class

	addon.ItemType	= ITEMTYPE_NULL;			// item type
	addon.ColorSet1	= cs1;						// colorset1
	addon.ColorSet2	= 0;						// colorset2
	addon.bEffectColor = FALSE;
	addon.EffectColorSet = 0;

	// ������ �д�.
	m_HairFrameID = id;
	m_HairColorSet = cs1;
}

//---------------------------------------------------------------------------
// Set AddonItem
//---------------------------------------------------------------------------
// �����ؼ� ����� �ٲ�� item(AddonItem)�� ���
// Creature�� �����ϰ� ���¸� �ٲ۴�.
//---------------------------------------------------------------------------
bool
MCreatureWear::SetAddonItem(MItem* pItem)
{
	DEBUG_ADD("MCreatureWear::SetAddonItem");
	CT_Done("SAI: ENTER this=%p pItem=%p m_ID=%d m_Action=%d", (void*)this, (void*)pItem, (int)m_ID, (int)m_Action);

	// FAZ 4 (2026-05-03): May 2 defansif "return false" workaround KALDIRILDI.
	// Önceki ABI mismatch sorunları çözüldü: SetGearInfo gerçek impl
	// (PacketFunction.cpp:2143), CreatureTable real LoadFromFile (max=40/17
	// per tribe), ItemDescShow MItem method'ları stable çalışıyor (trace
	// teyit). Karakter eşyaları visible olmalı şimdi. Crash gelirse
	// pItem->Method() çağrılarına granular markers + race-specific fix.
	if (pItem==NULL)
	{
		DEBUG_ADD("The Item is NULL");

		return false;
	}
	CT_Done("SAI: post-NULLcheck classType=%d", (int)GetClassType());

	//-------------------------------------------------
	//
	// ������ ������ ��츸 ������ �ٲ� �� �ִ�.
	//
	//-------------------------------------------------
	// player�� �ƴ� ��쳪..
	// ������̸� Ÿ�� ����..
	// �׳� �����ϰ� �� ������. - -;
	if (GetClassType()!=MCreature::CLASS_PLAYER
		|| pItem->GetItemClass()==ITEM_CLASS_MOTORCYCLE)
	{
		SetAction( (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );
	}
	CT_Done("SAI: post-actionSet itemClass=%d", (int)pItem->GetItemClass());

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// MCreatureWear::SetAddonItem ����â ��ü ����

	bool	bItemActive	= true;
/*	DWORD	dwGetGearID	= 0;
	switch(g_pPlayer->GetRace())
	{
	case RACE_SLAYER:
		dwGetGearID	= g_pSlayerGear->GetSendGearID();
		break;

	case RACE_VAMPIRE:
		dwGetGearID	= g_pVampireGear->GetSendGearID();
		break;
		
	case RACE_OUSTERS:
		dwGetGearID	= g_pOustersGear->GetSendGearID();
		break;
	}
*/

	if(pItem->IsQuestItem() || pItem->IsAffectStatus())
		bItemActive	= true;
	else
		bItemActive	= false;
	CT_Done("SAI: bItemActive=%d isQuest=%d affect=%d", (int)bItemActive, (int)pItem->IsQuestItem(), (int)pItem->IsAffectStatus());

	if ((m_Action==ACTION_STAND
		|| m_Action	== ACTION_MOVE
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND
		|| IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		)
		&& bItemActive)
#else
	if (m_Action==ACTION_STAND 
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND
		|| IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND 
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		)
#endif	//__GEAR_SWAP_CHANGE
	{
		//-------------------------------------------------
		// �����ؼ� ����� �ٲ�� Item�� ��쿡�� ó���Ѵ�.
		//-------------------------------------------------
		CT_Done("SAI: entered action+active block isAddon=%d isAffect=%d", (int)pItem->IsAddonItem(), (int)pItem->IsAffectStatus());

		if (pItem->IsAddonItem() && pItem->IsAffectStatus())
//		if(pItem->IsAddonItem() && (pItem->IsAffectStatus() || 
//			(pItem->GetItemClass() == ITEM_CLASS_COAT &&
//			pItem->GetItemType() >=32 &&
//			pItem->GetItemType() <=35)))	//by viva
		{
			TYPE_FRAMEID	fid;
			ADDON			Addon = pItem->GetAddonSlot();
			CT_Done("SAI: addonSlot=%d", (int)Addon);

			//-------------------------------------------------
			// ������ �ٲ�� ��ġ�� �ִ� ���
			//-------------------------------------------------
			if (pItem->GetAddonSlot()==ADDON_NULL)
			{	
				// 2005, 1, 19, sobeit add start - �����̾� ��� �Ƹ� ó��
				if(pItem->GetItemClass() == ITEM_CLASS_SHOULDER_ARMOR)
					{
					if (IsMale())
					{
						fid = pItem->GetAddonMaleFrameID();
					}			
					else //if (pCreature->IsFemale())
					{
						fid = pItem->GetAddonFemaleFrameID();			
					}
					m_ShoulderAddon.bAddon = TRUE;
					m_ShoulderAddon.FrameID	= fid;		// ����

					m_ShoulderAddon.ItemClass	= ITEM_CLASS_SHOULDER_ARMOR;	// item class
					m_ShoulderAddon.ItemType	= pItem->GetItemType();			// item type
					m_ShoulderAddon.ColorSet1	= pItem->GetItemOptionColorSet();	// option
					m_ShoulderAddon.ColorSet2	= pItem->GetItemOptionColorSet();//pItem->GetItemColorSet();			// null
					m_ShoulderAddon.bEffectColor = FALSE;
					m_ShoulderAddon.EffectColorSet = 0;
				}
				// 2005, 1, 19, sobeit add end
				else
					DEBUG_ADD_FORMAT("[Error] Item doesn't have Addon Slot. id=%d, class=%d, type=%d", 
														pItem->GetID(), (int)pItem->GetItemClass(), (int)pItem->GetItemType());
				
			}
			else
			{	
				//-------------------------------------------------
				//
				//			�����ϴ� ��ġ�� ���� ó��..
				//
				//-------------------------------------------------
				switch (Addon)
				{
					//-------------------------------------------------
					// ������̸� ������ ���
					//-------------------------------------------------
					case ADDON_MOTOR :
						//------------------------------------------
						// �����̴� ��� �ٲ�
						//------------------------------------------					
						SetMoveDevice( MCreature::MOVE_DEVICE_RIDE );
					break;

					//-------------------------------------------------
					// �����ҷ��°� �޼չ���?�� ���
					//-------------------------------------------------
					case ADDON_LEFTHAND :
					{
						// ���йۿ� ����. - -;
						// �ʻ� �ϵ��ڵ�.. - -;;;;

						//-------------------------------------------------
						// ������ �ִ� �����չ��Ⱑ ��չ����̸� 
						// ������Addon�� �����Ѵ�.
						//-------------------------------------------------
						MItem* pOldItem = NewItemFromAddonInfo( ADDON_RIGHTHAND );

						// ������ item�� �ִ� ��� �����Ѵ�.
						if (pOldItem!=NULL)
						{	
							if (pOldItem->IsGearSlotTwoHand())
							{
								RemoveAddon( ADDON_RIGHTHAND );
							}

							delete pOldItem;
						}
					}
					break;

					//-------------------------------------------------
					// �����ҷ��°� �����չ���?�� ���
					//-------------------------------------------------
					case ADDON_RIGHTHAND :
					{
						// ��չ����̸� �޼��� ���ش�.
						if (pItem->IsGearSlotTwoHand())
						{
							RemoveAddon( ADDON_LEFTHAND );
						}				
					}	
					break;

					//-------------------------------------------------
					// ����
					//-------------------------------------------------
					// ������ ����
					// ª�� �Ӹ�ī���̶�� ���ش�.
					// ª�� �Ӹ� : ��12, ��12
					// �׷��� 1�� ���ڴ� ���� ���±� ������
					// �Ӹ�ī���� �ݵ�� ����ؾ���
					//-------------------------------------------------
					case ADDON_HELM :
						if(pItem->GetAddonMaleFrameID() != ADDONID_HELM1)
						{
							if(IsMale() && m_HairFrameID != ADDONID_HAIR3 ||
							   IsFemale() && m_HairFrameID != ADDONID_HAIR1)
							{
								if(!IsAdvancementClass())
									RemoveAddon( ADDON_HAIR );
							}
						}
						else
						{
							SetAddonHair(m_HairFrameID, m_HairColorSet);
						}
					break;

					//-------------------------------------------------
					// ����� ���.. �� �ٲٸ� �� ���� �ٲ��.
					//-------------------------------------------------
					case ADDON_COAT :
						CT_Done("SAI: case ADDON_COAT enter creatureType=%d", (int)m_CreatureType);
						if (m_CreatureType==CREATURETYPE_SLAYER_OPERATOR
							|| m_CreatureType==CREATURETYPE_VAMPIRE_OPERATOR
							|| m_CreatureType==CREATURETYPE_OUSTERS_OPERATOR)
						{
//							if(pItem->IsUniqueItem())
//								m_ColorBody1 = m_ColorBody2 = UNIQUE_ITEM_COLOR;
//							else
								m_ColorBody1 = m_ColorBody2 = pItem->GetItemOptionColorSet();
						}
					break;
				}
				CT_Done("SAI: post-switch Addon=%d", (int)Addon);

				//-------------------------------------------------
				//
				//			������ ���� FrameID
				//
				//-------------------------------------------------
				if (IsMale())
				{
					fid = pItem->GetAddonMaleFrameID();
				}
				else //if (pCreature->IsFemale())
				{
					fid = pItem->GetAddonFemaleFrameID();
				}
				CT_Done("SAI: post-fid fid=%d isSlayer=%d", (int)fid, (int)IsSlayer());
			
				//-------------------------------------------------	
				// Slayer�̸鼭 fid�� ���� ���..
				//-------------------------------------------------					
				if (fid == FRAMEID_NULL && IsSlayer())
				{					
					DEBUG_ADD_FORMAT("[Error] Item doesn't have FrameID. id=%d, class=%d, type=%d", 
										pItem->GetID(), (int)pItem->GetItemClass(), (int)pItem->GetItemType());					
				}
				//-------------------------------------------------					
				// �������̰ų�.. Vampire�� ���
				//-------------------------------------------------					
				else
				{	
					// 2005, 1, 19, sobeit add start - �����̾� ��� �Ƹ� ó��
//					if(pItem->GetItemClass() == ITEM_CLASS_SHOULDER_ARMOR)
//					{
//						m_ShoulderAddon.bAddon = TRUE;
//						m_ShoulderAddon.FrameID	= fid;		// ����
//
//						m_ShoulderAddon.ItemClass	= ITEM_CLASS_SHOULDER_ARMOR;	// item class
//						m_ShoulderAddon.ItemType	= pItem->GetItemType();			// item type
//						m_ShoulderAddon.ColorSet1	= pItem->GetItemOptionColorSet();	// option
//						m_ShoulderAddon.ColorSet2	= pItem->GetItemOptionColorSet();//pItem->GetItemColorSet();			// null
//						m_ShoulderAddon.bEffectColor = FALSE;
//						m_ShoulderAddon.EffectColorSet = 0;
//					}
					// 2005, 1, 19, sobeit add end
//					else
					{
						//-------------------------------------------------
						// �����Ѵ�.
						//-------------------------------------------------
						CT_Done("SAI: pre-m_Addon[%d] ref", (int)Addon);
						ADDON_INFO& addon = m_Addon[Addon];

						// �ʱ�ȭ
						addon.bAddon	= TRUE;
						addon.FrameID	= fid;
						CT_Done("SAI: post-bAddon/FrameID set");

						addon.ItemClass	= pItem->GetItemClass();
						addon.ItemType	= pItem->GetItemType();
						CT_Done("SAI: post-itemClass/Type set class=%d type=%d", (int)addon.ItemClass, (int)addon.ItemType);

						// ���� ���߰�
						if(pItem->GetItemClass() == ITEM_CLASS_VAMPIRE_COAT)
						{
							unsigned short nSpecialActionInfo = GetSpecialActionInfo();

							if(m_CreatureType != CREATURETYPE_VAMPIRE_OPERATOR)
							{
								//if (m_CreatureType != CREATURETYPE_WOLF && m_CreatureType != CREATURETYPE_BAT )
								if (m_CreatureType != CREATURETYPE_WOLF && m_CreatureType != CREATURETYPE_BAT  && m_CreatureType != CREATURETYPE_VAMPIRE_GHOST
#if __CONTENTS(__FAST_TRANSFORTER)
								&& m_CreatureType != CREATURETYPE_FLITTERMOUSE
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
								&& m_CreatureType != CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
									)
								{
									if( m_CreatureType == CREATURETYPE_WER_WOLF )
									{
										SetCreatureType( CREATURETYPE_WER_WOLF );
									}
									/*
									else
									{
										if (IsMale())
										{
											SetCreatureType(pItem->GetAddonMaleFrameID());
										}			
										else //if (pCreature->IsFemale())
										{
											SetCreatureType(pItem->GetAddonFemaleFrameID());
										}
									}
									*/
								}
	
								addon.ColorSet1			= m_ColorBody1;//defaultSkinColor;	// �Ǻ�
								addon.ColorSet2			= defaultCoatColor;

								addon.bEffectColor		= FALSE;
								addon.EffectColorSet	= 0;

								if(pItem->IsUniqueItem())
									m_ColorBody2 = UNIQUE_ITEM_COLOR;
								else
									m_ColorBody2	= pItem->GetItemOptionColorSet();			// option

								if( m_CreatureType == CREATURETYPE_WER_WOLF )
									m_ColorBody1 = m_ColorBody2;
									
							}
							else
							{
								m_ColorBody1 = m_ColorBody2 = pItem->GetItemOptionColorSet();
							}

							SetSpecialActionInfo(nSpecialActionInfo);
						}

						else
						if (Addon==ADDON_COAT || Addon==ADDON_TROUSER)
						{
							CT_Done("SAI: COAT/TROUSER branch isAdvClass=%d", (int)IsAdvancementClass());
							{

								// 2005, 2, 18, sobeit modify end
								//						if(pItem->IsUniqueItem())
								//							addon.ColorSet2	= UNIQUE_ITEM_COLOR;			// option
								//						else

								if(Addon == ADDON_COAT)
								{
									// 2005, 2, 18, sobeit modify start - ���� �����̾� ADDON_COAT�� ColorSet1�� �Ӹ� �����̴�.
									if(!IsAdvancementClass())
										addon.ColorSet1	= m_ColorBody1;//defaultSkinColor;	// �Ǻ�
									else
									{
										CT_Done("SAI: pre-HairAddon ref m_Addon ptr=%p", (void*)m_Addon);
										ADDON_INFO& HairAddon = m_Addon[ADDON_HAIR];
										CT_Done("SAI: post-HairAddon ref HairAddon.ColorSet1=%d", (int)HairAddon.ColorSet1);
										addon.ColorSet1	= HairAddon.ColorSet1;						// colorset1
										CT_Done("SAI: post-HairAddon copy");
									}

									// FAZ 4 RESTORE (2026-05-18): ABI mismatch hipotezi YANLIŞ
									// — GetItemOptionColorSet() satır 504/505/611/721/729'da
									// zaten aktif çağrılıyor. SKIP gereksiz paranoia idi.
									// 5 SAI SKIP restore edildi; SEH ile defansif sarmal eklendi.
									CT_Done("SAI: pre-pItem->GetItemOptionColorSet pItem=%p (RESTORED)", (void*)pItem);
									__try
									{
										addon.ColorSet2 = pItem->GetItemOptionColorSet();
										CT_Done("SAI: COAT addon.ColorSet2=%d (restored)", (int)addon.ColorSet2);
									}
									__except(EXCEPTION_EXECUTE_HANDLER)
									{
										CT_Done("SAI: COAT GetItemOptionColorSet SEH caught, fallback 0");
										addon.ColorSet2 = 0;
									}
									CT_Done("SAI: COAT colors set cs1=%d cs2=%d", (int)addon.ColorSet1, (int)addon.ColorSet2);
								}
								else
								{
									// FAZ 4 RESTORE (2026-05-18): non-COAT branch (TROUSER) restore
									CT_Done("SAI: GetItemOptionColorSet (else/TROUSER branch) RESTORED");
									__try
									{
										addon.ColorSet1 = pItem->GetItemOptionColorSet();
										CT_Done("SAI: TROUSER addon.ColorSet1=%d (restored)", (int)addon.ColorSet1);
									}
									__except(EXCEPTION_EXECUTE_HANDLER)
									{
										CT_Done("SAI: TROUSER GetItemOptionColorSet SEH caught, fallback 0");
										addon.ColorSet1 = 0;
									}
								}

								
								addon.bEffectColor = FALSE;
								addon.EffectColorSet = 0;
								
								// ������ ��츸 ���� �ٲ��.
								//if (Addon==ADDON_COAT)
								{
									//-------------------------------------------------
									// Vampire�� ����ȴ�.
									//-------------------------------------------------
									// FAZ 4 RESTORE (2026-05-18): ABI mismatch hipotezi YANLIŞ —
									// GetItemOptionColorSet zaten satır 504/611/721'de aktif.
									// SEH ile defansif sarmal.
									if (m_CreatureType==CREATURETYPE_VAMPIRE_OPERATOR)
									{
										CT_Done("SAI: VAMPIRE m_ColorBody RESTORED");
										__try
										{
											m_ColorBody1 = m_ColorBody2 = pItem->GetItemOptionColorSet();
											CT_Done("SAI: VAMPIRE m_ColorBody1/2=%d (restored)", (int)m_ColorBody1);
										}
										__except(EXCEPTION_EXECUTE_HANDLER)
										{
											CT_Done("SAI: VAMPIRE GetItemOptionColorSet SEH caught, fallback 0");
											m_ColorBody1 = m_ColorBody2 = 0;
										}
									}
									else
									{
										CT_Done("SAI: non-VAMPIRE m_ColorBody2 RESTORED");
										__try
										{
											m_ColorBody2 = pItem->GetItemOptionColorSet();
											CT_Done("SAI: non-VAMPIRE m_ColorBody2=%d (restored)", (int)m_ColorBody2);
										}
										__except(EXCEPTION_EXECUTE_HANDLER)
										{
											CT_Done("SAI: non-VAMPIRE GetItemOptionColorSet SEH caught, fallback 0");
											m_ColorBody2 = 0;
										}
									}
								}
							}
						}
						else
						{
	//						if(pItem->IsUniqueItem())
	//						{
	//							addon.ColorSet1	= UNIQUE_ITEM_COLOR;	// option
	//							addon.ColorSet2	= UNIQUE_ITEM_COLOR;//pItem->GetItemColorSet();			// null
	//						}
	//						else
							{
								// FAZ 4 RESTORE (2026-05-18): ABI mismatch hipotezi YANLIŞ
								// — GetItemOptionColorSet zaten dosyada aktif çağrılıyor.
								// SEH ile defansif sarmal.
								CT_Done("SAI: addon.ColorSet1/2 RESTORED (else-non-COAT/TROUSER branch)");
								__try
								{
									addon.ColorSet1	= pItem->GetItemOptionColorSet();	// option
									addon.ColorSet2	= pItem->GetItemOptionColorSet();
									CT_Done("SAI: else-non-COAT addon.ColorSet1=%d cs2=%d (restored)", (int)addon.ColorSet1, (int)addon.ColorSet2);
								}
								__except(EXCEPTION_EXECUTE_HANDLER)
								{
									CT_Done("SAI: else-non-COAT GetItemOptionColorSet SEH caught, fallback 0");
									addon.ColorSet1	= 0;
									addon.ColorSet2	= 0;
								}

#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
								//����, �����̾��� �ű��̵������̶��..... 
								//4�� 30�� �׼� / 5�� 7�� ���� ��ġ������ ���Ƿ� Color���� 0���� �����Ѵ�.
								//����, �ű��̵����ܿ� ���� Color Enchant����� �߰��� ��.. (����)�����̾� ����ũ�� ���ؼ� Coloró���� ������ �κ��� �����ϰ�, �� ���� �� �κ��� ó���� ������ �ϳ�.
								if (pItem->GetItemClass() == ITEM_CLASS_MOTORCYCLE)
								{
#if __CONTENTS(__FAST_TRANSFORTER)
									if(pItem->GetItemType () == 8)
									{
										addon.ColorSet1	= (WORD)379;
										addon.ColorSet2	= (WORD)379;
									}
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
									else if(pItem->GetItemType () == 9)
									{
										addon.ColorSet1	= (WORD)379;
										addon.ColorSet2	= (WORD)379;
									}
#endif //__SECOND_TRANSFORTER
								}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
							}
							addon.bEffectColor = FALSE;
							addon.EffectColorSet = 0;
						}					
					}
				}				
			}
		}			
		else
		{
			DEBUG_ADD_FORMAT("[Error] Item is Not Addon Item. id=%d, class=%d, type=%d", 
									pItem->GetID(), (int)pItem->GetItemClass(), (int)pItem->GetItemType());			
		}

		//-------------------------------------------------
		// ������ Item��
		// �⺻���ݿ� �������̶��...
		// �⺻����ActionInfo�� �ٲ��� �Ѵ�.
		//-------------------------------------------------
		// FAZ 4 (2026-05-11) Bug 6.b fix — Slayer 2H weapon (Mace/Blade/Cross/Gun)
		// equip ettiginde saldiri sirasinda yumruk animasyonu cikiyordu.
		// ROOT CAUSE: IsAffectStatus() gate'i bazi yollardan (GCChangeShape ile gelen
		// temporary pItem, SetGearInfo loop'undaki pItem'in default ctor durumu)
		// FALSE kalip BasicActionInfo guncellenmiyordu -> default SKILL_ATTACK_MELEE
		// kaliyor -> attack sirasinda yumruk sprite oynaniyor.
		// Fix: IsAffectStatus check'i kaldir. Basic weapon equipte BasicActionInfo
		// daima weapon class'a gore set edilmeli (silahla yumruk animasyonu mantiksiz).
		// 3 irk paritesi: ITEM_CLASS_BLADE/SWORD/MACE/CROSS/AR/SR/SG/SMG hepsi MItem
		// MWeaponItem hiyerarsinde — Slayer/Vampire/Ousters icin ayni fix.
		if (pItem->IsBasicWeapon())
		{
			TYPE_ACTIONINFO ai = pItem->GetUseActionInfo();

			//-------------------------------------------------
			// ������ actionInfo�� ���� ���
			//-------------------------------------------------
			if (ai==ACTIONINFO_NULL)
			{
				// �⺻ �Ǽ� ���� �������� ��ȯ�Ѵ�.
				SetBasicActionInfo( SKILL_ATTACK_MELEE );

				DEBUG_ADD("[Empty ActionInfo]This Creature's basic attack is Melee");
			}
			//-------------------------------------------------
			// ������ actionInfo�� �ִٸ�..
			//-------------------------------------------------
			else
			{
				// �⺻ ���� �������� �����Ѵ�.
				SetBasicActionInfo( ai );

				DEBUG_ADD_FORMAT("[Set BasicActionInfo] %d", ai);
			}
		}

		//-------------------------------------------------
		// �������� ���������..
		// ������ �Ǽ��̶�� ���� �ȴ�..
		// �ϵ� �ڵ�.. - -;;
		//-------------------------------------------------
		if (!m_Addon[ADDON_RIGHTHAND].bAddon)
		{
			DEBUG_ADD("[Empty RightHand]This Creature's basic attack is Melee");
			
			// �⺻ �Ǽ� ���� �������� ��ȯ�Ѵ�.
			SetBasicActionInfo( SKILL_ATTACK_MELEE );			
		}

		return true;	
		
	}

	//-------------------------------------------------
	//
	// ������ ���°� �ƴ� ���
	//
	//-------------------------------------------------
	DEBUG_ADD_FORMAT("[Error] Not STAND action. id=%d, action=%d", m_ID, m_Action);
	
	// ������ ������ ��, ������ �ٲٵ��� �Ѵ�..
	// �׷��� �Ǵµ�.. ������. - -;;

	return false;	
}

//---------------------------------------------------------------------------
// Remove AddonItem
//---------------------------------------------------------------------------
// AddonItem�� �������� �Ѵ�.
//---------------------------------------------------------------------------
bool
MCreatureWear::RemoveAddonItem( MItem* pItem )
{
	DEBUG_ADD("MCreatureWear::RemoveAddonItem");
	
	if (pItem==NULL)
	{
		DEBUG_ADD("The Item is NULL");
		
		return false;
	}

	//-------------------------------------------------
	//
	// ������ ������ ��츸 ������ �ٲ� �� �ִ�.
	//
	//-------------------------------------------------
	// player�� �ƴ� ��쳪..
	// ������̸� Ÿ�� ����..
	// �׳� �����ϰ� �� ������. - -;
	if (GetClassType()!=MCreature::CLASS_PLAYER
		|| pItem->GetItemClass()==ITEM_CLASS_MOTORCYCLE)
	{
		SetAction( (m_MoveAction==ACTION_SLAYER_MOTOR_MOVE)? ACTION_SLAYER_MOTOR_STAND : ACTION_STAND );
	}

#if __CONTENTS(__GEAR_SWAP_CHANGE)	//MCreatureWear::RemoveAddonItem ����â ���� ����
	if (m_Action==ACTION_STAND
		|| m_Action==ACTION_MOVE
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND
		|| IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		)
#else
	if (m_Action==ACTION_STAND
		|| IsSlayer() && m_Action==ACTION_SLAYER_MOTOR_STAND
		|| IsOusters() && m_Action==ACTION_OUSTERS_FAST_MOVE_STAND 
#if __CONTENTS(__FAST_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_WING_STAND
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| IsOusters() && m_Action==ACTION_OUSTERS_UNICORN_STAND
#endif //__SECOND_TRANSFORTER
		)
#endif	// __GEAR_SWAP_CHANGE

	{
		// ���� ���߰�
		if(pItem->GetItemClass() == ITEM_CLASS_VAMPIRE_COAT)
		{
			if(m_CreatureType != CREATURETYPE_VAMPIRE_OPERATOR)
			{
				//if (m_CreatureType != CREATURETYPE_WOLF && m_CreatureType != CREATURETYPE_BAT )
				if (m_CreatureType != CREATURETYPE_WOLF && m_CreatureType != CREATURETYPE_BAT && m_CreatureType != CREATURETYPE_VAMPIRE_GHOST
#if __CONTENTS(__FAST_TRANSFORTER)
								&& m_CreatureType != CREATURETYPE_FLITTERMOUSE
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
								&& m_CreatureType != CREATURETYPE_SHAPE_OF_DEMON
#endif //__SECOND_TRANSFORTER
					)
				{
					if( m_CreatureType == CREATURETYPE_WER_WOLF )
					{
						SetCreatureType( CREATURETYPE_WER_WOLF );
						m_ColorBody1 = defaultCoatColor;
					}
					/*
					else
					{
						if (IsMale())	// �ƾ� �ϵ� �ڵ� �ϸ� �ȴ�µ�-��-
						{
							SetCreatureType(2);
						}			
						else //if (pCreature->IsFemale())
						{
							SetCreatureType(3);
						}
					}
					*/
				}
				//						addon.ColorSet1	= m_ColorBody1;//defaultSkinColor;	// �Ǻ�

				m_ColorBody2	= defaultCoatColor;
			}
			else
			{
				m_ColorBody1	= m_ColorBody2	= defaultCoatColor;
			}
		}

		//-------------------------------------------------
		// �����ؼ� ����� �ٲ�� Item�ΰ�?
		//-------------------------------------------------
		// --> ���� ������ �˾Ƴ��� 
		//    �� ������ Addon�� ���־� �Ѵ�.
		//-------------------------------------------------
//		else
		if (pItem->IsAddonItem())
		{						
			enum ADDON add = pItem->GetAddonSlot();

			//-------------------------------------------------
			// ���� ������ Addon�� ���ش�.
			//-------------------------------------------------
			if (add == ADDON_NULL)
			{
				// 2005, 1, 19, sobeit add start - �����̾� ��� �Ƹ� ó��
				if(pItem->GetItemClass() == ITEM_CLASS_SHOULDER_ARMOR)
				{
					m_ShoulderAddon.bAddon = FALSE;
					m_ShoulderAddon.FrameID	= FRAMEID_NULL;		// ����

					m_ShoulderAddon.ItemClass	= ITEM_CLASS_NULL;	// item class
					m_ShoulderAddon.ItemType	= ITEMTYPE_NULL;			// item type
					m_ShoulderAddon.ColorSet1	= 0;						// colorset1
					m_ShoulderAddon.ColorSet2	= 0;						// colorset2
					m_ShoulderAddon.bEffectColor = FALSE;					// effect color�� �������� �����ΰ�?
					m_ShoulderAddon.EffectColorSet	= 0;				// effectcolorset
				}
				// 2005, 1, 19, sobeit add end
				else
				DEBUG_ADD_FORMAT("[Error] Item doesn't have Addon Slot. id=%d, class=%d, type=%d", 
													pItem->GetID(), (int)pItem->GetItemClass(), (int)pItem->GetItemType());				
			}
			else
			{
				//--------------------------------------------------
				// ������ ���� �Ѵ�.
				//--------------------------------------------------
				ClearAddonInfo( add );

				//-------------------------------------------------
				//
				//			�����ϴ� ������ ����
				//
				//-------------------------------------------------	
				switch (add)
				{
					//-------------------------------------------------
					// ���� : �Ǹ��� ��쿡�� �⺻������..
					//-------------------------------------------------
					case ADDON_COAT :
					{
						//MItem* pItem = MItem::NewItem( ITEM_CLASS_TROUSER );
						//pItem->SetItemType( 0 );

						//SetAddonItem( pItem );	

						//delete pItem;
						//-------------------------------------------------
						// �����Ѵ�.
						//-------------------------------------------------						
						ADDON_INFO& addon = m_Addon[ADDON_COAT];

						// �ʱ�ȭ
						addon.bAddon	= TRUE;

						if(GetRace() == RACE_OUSTERS)
						{
							addon.FrameID = 1;
							addon.ColorSet1 = defaultCoatColor;

							addon.ItemClass	= ITEM_CLASS_OUSTERS_COAT;
						}
						else if(GetRace() == RACE_VAMPIRE)
						{	
							//SetCreatureType(IsMale() ? 2 : 3);

							addon.FrameID	= IsMale() ? 2 : 3;
							addon.ColorSet1 = m_ColorBody1;

							addon.ItemClass	= ITEM_CLASS_VAMPIRE_COAT;
						}
						else
						{
							//addon.FrameID	= IsMale()? ADDONID_COAT0_MALE : ADDONID_COAT0_FEMALE;
							addon.FrameID	= ADDONID_COAT1;

							// 2005, 2, 18, sobeit modify start - ���� �����̾� ADDON_COAT�� ColorSet1�� �Ӹ� �����̴�.
							if(!IsAdvancementClass())
								addon.ColorSet1	= m_ColorBody1;//defaultSkinColor;	// �Ǻ�
							else
							{
								ADDON_INFO& HairAddon = m_Addon[ADDON_HAIR];
								addon.ColorSet1	= HairAddon.ColorSet1;						// colorset1
							}
							// 2005, 2, 18, sobeit modify end

							addon.ItemClass	= ITEM_CLASS_COAT;
						}

						addon.ItemType	= 0;
						addon.ColorSet2	= defaultCoatColor;
						addon.bEffectColor = FALSE;
						addon.EffectColorSet = 0;
						
						//-------------------------------------------------	
						// Vampire�� ����ȴ�.
						// ��ڵ� ����ǰ���..
						//-------------------------------------------------													
						if (m_CreatureType==CREATURETYPE_VAMPIRE_OPERATOR
							|| m_CreatureType==CREATURETYPE_SLAYER_OPERATOR
							|| m_CreatureType==CREATURETYPE_OUSTERS_OPERATOR
							)
						{
							m_ColorBody1 = m_ColorBody2 = defaultSkinColor;	
						}
						else
						{
							m_ColorBody2 = defaultCoatColor;
						}
						
					}
					break;

					//-------------------------------------------------
					// ���� : �Ǹ��� ��쿡�� �⺻������..
					//-------------------------------------------------
					case ADDON_TROUSER :
					{
						//MItem* pItem = MItem::NewItem( ITEM_CLASS_TROUSER );
						//pItem->SetItemType( 0 );

						//SetAddonItem( pItem );

						//delete pItem;
						//-------------------------------------------------
						// �����Ѵ�.
						//-------------------------------------------------
						ADDON_INFO& addon = m_Addon[ADDON_TROUSER];

						// �ʱ�ȭ
						addon.bAddon	= TRUE;
						//addon.FrameID	= IsMale()? ADDONID_TROUSER0_MALE : ADDONID_TROUSER0_FEMALE;
						addon.FrameID	= ADDONID_TROUSER1;

						addon.ItemClass	= ITEM_CLASS_TROUSER;
						addon.ItemType	= 0;
						addon.ColorSet1	= defaultTrouserColor;
						addon.ColorSet2	= m_ColorBody1;//defaultSkinColor;	// �⺻ �Ǻλ�
						addon.bEffectColor = FALSE;
						addon.EffectColorSet = 0;
					}
					break;

					//-------------------------------------------------
					// �������� ����..�̹Ƿ�..
					//-------------------------------------------------				
					case ADDON_RIGHTHAND :
						// �⺻ �Ǽ� ���� �������� ��ȯ�Ѵ�.
						SetBasicActionInfo( SKILL_ATTACK_MELEE );

						// Ư�� ���⿡�� �ɸ��� Effect
						switch (pItem->GetItemClass())
						{
							//-------------------------------------------------	
							// ��
							//-------------------------------------------------	
							case ITEM_CLASS_BLADE :
								if (HasEffectStatus(EFFECTSTATUS_BERSERKER))
								{
									RemoveEffectStatus(EFFECTSTATUS_BERSERKER);
								}
							break;

							//-------------------------------------------------	
							// ���ڰ�
							//-------------------------------------------------	
							case ITEM_CLASS_CROSS :
							case ITEM_CLASS_MACE :
								if (HasEffectStatus(EFFECTSTATUS_HYPNOSIS))
								{
									RemoveEffectStatus(EFFECTSTATUS_HYPNOSIS);
								}
							break;
						}
					break;

					//-------------------------------------------------
					// ������� ���� ����
					//-------------------------------------------------
					case ADDON_MOTOR :
						//------------------------------------------
						// �����̴� ��� �ٲ�
						//------------------------------------------					
						SetMoveDevice( MCreature::MOVE_DEVICE_WALK );
					break;

					//-------------------------------------------------
					// ����
					//-------------------------------------------------
					// ���ڸ� ������ ���� 
					// �Ӹ�ī���� �ٽ� �ٿ��ش�.
					//-------------------------------------------------
					case ADDON_HELM :
						SetAddonHair( m_HairFrameID, m_HairColorSet );
					break;
				}
				
			}

			return true;
		}

		// addon item�� �ƴ� ���
		return true;
	}

	//-------------------------------------------------
	//
	// ������ ���°� �ƴ� ���
	//
	//-------------------------------------------------
	DEBUG_ADD_FORMAT("[Error] Not STAND action. id=%d, action=%d", m_ID, m_Action);
	
	// ������ ������ ��, ������ �ٲٵ��� �Ѵ�..
	// �׷��� �Ǵµ�.. ������. - -;;


	return false;
}

//---------------------------------------------------------------------------
// Set Creature Type
//---------------------------------------------------------------------------
// �⺻ ������ ������.
//---------------------------------------------------------------------------
void
MCreatureWear::SetCreatureType(TYPE_CREATURETYPE type)
{
	//-------------------------------------------------
	// type����
	//-------------------------------------------------
	MCreature::SetCreatureType( type );

	//-------------------------------------------------
	// �� �����.
	//-------------------------------------------------
	for (int i=0; i<ADDON_MAX; i++)
	{
		ClearAddonInfo( i );
	}

	if(IsVampire())
	{
		//-------------------------------------------------
		// ���� : �Ǹ��� ��쿡�� �⺻������..
		//-------------------------------------------------
		ADDON_INFO& addon = m_Addon[ADDON_COAT];

		// �ʱ�ȭ
		addon.bAddon	= TRUE;

		// ���� ���ٸ� Vampire 1���ǻ����� ����
		if(type == FRAMEID_NULL)
		{
			addon.FrameID	= IsMale()? 2 : 3;
		}
		else
		{
			addon.FrameID	= type;
		}
		
		addon.ItemClass	= ITEM_CLASS_VAMPIRE_COAT;
		addon.ItemType	= 0;
		addon.ColorSet1	= m_ColorBody1;//defaultSkinColor;	// �⺻ �Ǻλ�
		addon.ColorSet2	= defaultCoatColor;	
		addon.bEffectColor = FALSE;
		addon.EffectColorSet = 0;
	}
	else
	if(IsOusters())
	{
		//-------------------------------------------------
		// ���� : �Ǹ��� ��쿡�� �⺻������..
		//-------------------------------------------------
		ADDON_INFO& addon = m_Addon[ADDON_COAT];
		
		// �ʱ�ȭ
		addon.bAddon	= TRUE;
		addon.FrameID	= 1;
		//		addon.FrameID	= IsMale()? ADDONID_COAT0_MALE : ADDONID_COAT0_FEMALE;
		
		addon.ItemClass	= ITEM_CLASS_OUSTERS_COAT;
		addon.ItemType	= 0;
		addon.ColorSet1	= m_ColorBody1;//defaultSkinColor;	// �⺻ �Ǻλ�
		addon.ColorSet2	= defaultCoatColor;	
		addon.bEffectColor = FALSE;
		addon.EffectColorSet = 0;
	}
	else
	{
		//-------------------------------------------------
		// ���� : �Ǹ��� ��쿡�� �⺻������..
		//-------------------------------------------------
		ADDON_INFO& addon = m_Addon[ADDON_COAT];

		// �ʱ�ȭ
		addon.bAddon	= TRUE;
		//addon.FrameID	= IsMale()? ADDONID_COAT0_MALE : ADDONID_COAT0_FEMALE;
		addon.FrameID	= ADDONID_COAT1;
		
		addon.ItemClass	= ITEM_CLASS_COAT;
		addon.ItemType	= 0;
		addon.ColorSet1	= m_ColorBody1;//defaultSkinColor;	// �⺻ �Ǻλ�
		addon.ColorSet2	= defaultCoatColor;	
		addon.bEffectColor = FALSE;
		addon.EffectColorSet = 0;
		
		//-------------------------------------------------
		// ���� : �Ǹ��� ��쿡�� �⺻������..
		//-------------------------------------------------
		ADDON_INFO& addon2 = m_Addon[ADDON_TROUSER];
		
		// �ʱ�ȭ
		addon2.bAddon	= TRUE;
		//addon2.FrameID	= IsMale()? ADDONID_TROUSER0_MALE : ADDONID_TROUSER0_FEMALE;
		addon2.FrameID	= ADDONID_TROUSER1;
		
		addon2.ItemClass	= ITEM_CLASS_TROUSER;
		addon2.ItemType	= 0;
		addon2.ColorSet1	= defaultTrouserColor;
		addon2.ColorSet2	= m_ColorBody1;//defaultSkinColor;	// �⺻ �Ǻλ�
		addon2.bEffectColor = FALSE;
		addon2.EffectColorSet = 0;
	}
}

//----------------------------------------------------------------------
// Remove EffectStatus
//----------------------------------------------------------------------
// Ư���� EffectSpriteType�� �����Ѵ�.
//----------------------------------------------------------------------
bool
MCreatureWear::RemoveEffectStatus(EFFECTSTATUS status)
{
	DEBUG_ADD_FORMAT("[RemoveEffectStatus] id=%d, status=%d", m_ID, (int)status);
	
	if (status>=g_pEffectStatusTable->GetSize())
	{
		DEBUG_ADD_FORMAT("[Error]EffectStatus exceed Max : %d", (int)status);
		
		return false;
	}

	if (!m_bEffectStatus[status])
	{
		return false;
	}

	m_bEffectStatus[status] = false;

	bool bUseEffectSprite = (*g_pEffectStatusTable)[status].bUseEffectSprite;

	TYPE_EFFECTSPRITETYPE type = (*g_pEffectStatusTable)[status].EffectSpriteType;
	//------------------------------------------------------------
	// effectStatus�� ����.
	//------------------------------------------------------------
	switch (status)
	{
		case EFFECTSTATUS_SUMMON_SYLPH_GREEN :
		case EFFECTSTATUS_SUMMON_SYLPH_ORANGE :
		case EFFECTSTATUS_SUMMON_SYLPH_RED :
		case EFFECTSTATUS_SUMMON_SYLPH_YELLOW :
		case EFFECTSTATUS_SUMMON_SYLPH_WHITE :
		case EFFECTSTATUS_SUMMON_SYLPH_BROWN :
		case EFFECTSTATUS_SUMMON_SYLPH_LILAC :
		case EFFECTSTATUS_SUMMON_SYLPH_BLACK :
		case EFFECTSTATUS_SUMMON_SYLPH:
		case EFFECTSTATUS_SUMMON_WISP_RED:
		case EFFECTSTATUS_SUMMON_WISP_YELLOW:
		case EFFECTSTATUS_SUMMON_WISP_GREEN:
			SetMoveDevice(MOVE_DEVICE_WALK);
			if(IsAdvancementClass())
			{
				type = Wisp_Sylph_Loopcolor_Kind(status);
				ExecuteActionInfoFromMainNode(MCreature::Wisp_Endcolor_Kind(status),GetX(), GetY(), 0,GetDirection(),	GetID(),	
					GetX(), GetY(), 0, 8, NULL, false);
			}
		break;
#if __CONTENTS(__FAST_TRANSFORTER)
		case EFFECTSTATUS_CLIENT_WING_SYLPH :
			{
				type = Seragon_Color_Kind(status);
				SetMoveDevice(MOVE_DEVICE_WALK);
			}
			break;
#endif //__FAST_TRANSFORTER
#if __CONTENTS(__SECOND_TRANSFORTER)
		case EFFECTSTATUS_CLIENT_UNICORN:
			{
				type = Unicorn_Color_Kind(status);
				SetMoveDevice(MOVE_DEVICE_WALK);
			}
			break;
#endif //__SECOND_TRANSFORTER

		case EFFECTSTATUS_BIKE_CRASH:
			SetMoveDevice(MOVE_DEVICE_WALK);
			break;
	
		//------------------------------------------------------------
		// ���� Ǯ�� ��
		//------------------------------------------------------------
		case EFFECTSTATUS_GUN_SHOT_GUIDANCE_AIM :
		case EFFECTSTATUS_SATELLITE_BOMB_AIM :
			if (m_bAlive)
			{
				SetStop();
				StopGunShotGuidance();
	#ifdef OUTPUT_DEBUG					
			DEBUG_ADD_FORMAT("[GSG] Stop");
	#endif
			}
		break;

		//------------------------------------------------------------
		// ���� Ǯ�� ��
		//------------------------------------------------------------
		case EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS :
		case EFFECTSTATUS_EXPLOSION_WATER :
			if (m_bAlive)
			{
				SetStop();
				StopCauseCriticalWounds();
	#ifdef OUTPUT_DEBUG					
			DEBUG_ADD_FORMAT("[CCW] Stop");
	#endif
			}
		break;

		case EFFECTSTATUS_CURSE_PARALYSIS :
			if (m_bAlive)
			{
				SetStop();
				Remove_Curse_Paralsis_Effect();
			}
		break;

		//------------------------------------------------------------
		// EFFECTSTATUS_SUMMON_CASKET [�����]
		//------------------------------------------------------------
		case EFFECTSTATUS_CASKET :
			// �ٷ� invisible�� �����.
			RemoveCasket();
		break;

		//------------------------------------------------------------
		// invisible ����
		//------------------------------------------------------------
		case EFFECTSTATUS_INVISIBILITY :
		case EFFECTSTATUS_SNIPPING_MODE :
			// invisible�� �����Ѵ�.
			SetVisible();
		break;

		//------------------------------------------------------------
		// EFFECTSTATUS_COMA
		//------------------------------------------------------------
		case EFFECTSTATUS_COMA :
			SetAlive();
			if(IsVampire())
			{
				ExecuteActionInfoFromMainNode(
							BLOOD_RESURRECT,										// ��� ��� ��ȣ
						
							m_X, m_Y, 0,
							(int)m_Direction,														// ��� ����
							
							m_ID,												// ��ǥ�� ���� ����
							m_X, m_Y, 0,
							
							2*16, //5*16, 
							
							NULL,
							
							false,
							
							(int)m_sX, (int)m_sY);	
			}

		case EFFECTSTATUS_GHOST:
			if (!(*g_pCreatureTable)[m_CreatureType].bFlyingCreature)	// ������ ���
				SetGroundCreature();
			break;
		break;
		case EFFECTSTATUS_GLACIER:	
			MCreature::RemoveGlacierEffect();
			break;

		case EFFECTSTATUS_INSTALL_TURRET:
			SetAction( ACTION_STAND );
			SetInstallTurretCount(0);
			SetInstallTurretDirect(2);
			SetDirection( 2 );
			SetCurrentDirection( 2 );
			break;
		case EFFECTSTATUS_GRAND_MASTER_SLAYER:
		case EFFECTSTATUS_GRAND_MASTER_VAMPIRE:
		case EFFECTSTATUS_GRAND_MASTER_OUSTERS:
		case EFFECTSTATUS_GRAND_MASTER_SLAYER_130:
		case EFFECTSTATUS_GRAND_MASTER_SLAYER_150:
		case EFFECTSTATUS_GRAND_MASTER_VAMPIRE_130:
		case EFFECTSTATUS_GRAND_MASTER_VAMPIRE_150:
		case EFFECTSTATUS_GRAND_MASTER_OUSTERS_130:
		case EFFECTSTATUS_GRAND_MASTER_OUSTERS_150:
			{
				type = GetMasterEffectType(status);
			}
			break;
		case EFFECTSTATUS_HYDRO_CONVERGENCE:
			type = EFFECTSPRITETYPE_HYDRO_CONVERGENCE_PILLAR_OF_WATER_REPEAT;
			break;	
		case EFFECTSTATUS_BLIND:
			if(g_pTopView != NULL && GetID() == g_pPlayer->GetID())
			{
				g_pTopView->SetFadeStart(1, 31, 1, 255,255,255);
				g_pTopView->SetBlindEffect(false);
			}
			break;
	}

	//------------------------------------------------------------
	//
	//			EffectSprite�� ǥ���ϴ� ���
	//
	//------------------------------------------------------------
	if (bUseEffectSprite)
	{
		//TYPE_EFFECTSPRITETYPE type = (*g_pEffectStatusTable)[status].EffectSpriteType;
		TYPE_EFFECTSPRITETYPE type2 = EFFECTSPRITETYPE_NULL;

		if(type == EFFECTSPRITETYPE_FAST_MOVE_STOP)
			type2 = EFFECTSPRITETYPE_FAST_MOVE_FLY;

		if(type == EFFECTSPRITETYPE_HYDRO_CONVERGENCE_PILLAR_OF_WATER_REPEAT)
			type2 = EFFECTSPRITETYPE_HYDRO_CONVERGENCE_WATER_GROUND_REPEAT;

		_MinTrace("remove effectsprite\n");

		//-------------------------------------------------------
		// Creature�� �پ� �ִ� Effect�߿���
		// EffectSpriteType�� type�� ���� �����Ѵ�.
		//-------------------------------------------------------
		if (type < g_pEffectSpriteTypeTable->GetSize())
		{
			if (IsFemale()
				&& (*g_pEffectSpriteTypeTable)[type].FemaleEffectSpriteType!=EFFECTSPRITETYPE_NULL)
			{
				type = (*g_pEffectSpriteTypeTable)[type].FemaleEffectSpriteType;
			}
			

			if (m_bAttachEffect[type])
			{
				//-------------------------------------------------------
				// ���� ���� ���
				//-------------------------------------------------------			
				ATTACHEFFECT_LIST::iterator iEffect = m_listEffect.begin();

				bool re = false;

				while (iEffect != m_listEffect.end())
				{
					MAttachEffect* pEffect = *iEffect;
					
					//-------------------------------------------------------
					// ���� type�� ã�´�.
					//-------------------------------------------------------
					if (pEffect->IsEffectSprite() 
						&& (pEffect->GetEffectSpriteType() == type ||
						type2 != EFFECTSPRITETYPE_NULL && pEffect->GetEffectSpriteType() == type2 ) )
					{						
						// �޸� ����
						delete pEffect;
						pEffect = NULL;
						
						ATTACHEFFECT_LIST::iterator dEffect = iEffect;
						iEffect--;

						// list���� ����
						m_listEffect.erase( dEffect );

						m_bAttachEffect[type] = false;
						
						re = true;
					}					
					
					iEffect++;
				}

				if(re)
					return true;

				//-------------------------------------------------------
				// �ٴڿ� ���� ���
				//-------------------------------------------------------
				iEffect = m_listGroundEffect.begin();

				while (iEffect != m_listGroundEffect.end())
				{
					MAttachEffect* pEffect = *iEffect;
					
					//-------------------------------------------------------
					// ���� type�� ã�´�.
					//-------------------------------------------------------
					if (pEffect->IsEffectSprite() && 
						( pEffect->GetEffectSpriteType() == type || type2 != EFFECTSPRITETYPE_NULL && 
						type2 == pEffect->GetEffectSpriteType()) )
					{
						// �޸� ����
						delete pEffect;

						// list���� ����
						m_listGroundEffect.erase( iEffect );

						m_bAttachEffect[type] = false;
				
						return true;
					} 
					iEffect++;
				}
			}
		}
	}
	//------------------------------------------------------------
	//
	//			EffectColor�� ǥ���ϴ� ���
	//
	//------------------------------------------------------------
	else
	{
		WORD colorSet = (*g_pEffectStatusTable)[status].EffectColor;

		//-------------------------------------------------------
		// Creature�� �پ� �ִ� Effect�߿���
		// EffectColor�� color�� ���� �����Ѵ�.
		//-------------------------------------------------------
		ATTACHEFFECT_LIST::iterator iEffect = m_listEffect.begin();

		while (iEffect != m_listEffect.end())
		{
			MAttachEffect* pEffect = *iEffect;
			
			//-------------------------------------------------------
			// ���� type�� ã�´�.
			//-------------------------------------------------------
			if (pEffect->IsEffectColor() && pEffect->GetEffectColor() == colorSet)
			{
				ADDON	part = pEffect->GetEffectColorPart();

				//------------------------------------------------------------
				// Ư���� ������ ������ �ٲ�� ��쵵 �ִ�.
				//------------------------------------------------------------
				if (part < ADDON_MAX)
				{
					m_Addon[part].bEffectColor = FALSE;
				}
				//------------------------------------------------------------
				// ��� ������ �������...
				//------------------------------------------------------------
				else
				{
					for (int p=0; p<ADDON_MAX; p++)
					{
						m_Addon[p].bEffectColor = FALSE;
					}
				}

				// �޸� ����
				delete pEffect;

				// list���� ����
				m_listEffect.erase( iEffect );

				return true;
			}
			
			iEffect++;
		}		
	}
	
	return false;
}

//----------------------------------------------------------------------
// Update Effect
//----------------------------------------------------------------------
// Creature�� ���� ��� Effect�� Frame�� �ٲ��ְ�..
// �����°� ������ list�� memory���� �����Ѵ�.
//----------------------------------------------------------------------
void		
MCreatureWear::UpdateAttachEffect()
{
	ATTACHEFFECT_LIST::iterator iEffect = m_listEffect.begin();
	ATTACHEFFECT_LIST::iterator iEffectTemp;

	// ���� 0
	//m_MaxEffectLight = 0;

	//---------------------------------------------------------------------
	// ĳ���� ���� �ٲٴ� ��..
	//---------------------------------------------------------------------
	// NULL���̴�.. �ܺο��� üũ�ؾ���..
	m_AttachEffectColor = m_ChangeColorSet;	//ATTACHEFFECTCOLOR_NULL;
	int bShowColor = (HasEffectStatus(EFFECTSTATUS_CURSE_PARALYSIS) || HasEffectStatus(EFFECTSTATUS_FREEZE)
						|| g_CurrentFrame % g_pClientConfig->FRAME_DRAW_ORIGINAL_SPRITE);	// �� frame���� �ѹ����� ���� ������ �����ش�.

	int bShowColorPart = (g_CurrentFrame & 0x00000004);	// 4 frame�� �����ư��� �� ���
	int numColors = 0;
	BOOL bChangeColor = FALSE;

	BOOL	bAddonColorPart[ADDON_MAX] = { FALSE, };
	WORD	addonColor[ADDON_MAX] = { m_ChangeColorSet, };

	//---------------------------------------------------------------------
	// ��� Effect�� Update�Ѵ�.
	//---------------------------------------------------------------------
	while (iEffect != m_listEffect.end())
	{
		MAttachEffect*	pEffect = *iEffect;
		bool			bErase = false;
		
		int x		= pEffect->GetX();
		int y		= pEffect->GetY();
		int light = pEffect->GetLight();		

		//---------------------------------------------------------------------
		// Update
		//---------------------------------------------------------------------
		// message : find path
		//#ifdef	OUTPUT_DEBUG
			//sprintf(g_pDebugMessage->GetCurrent(), "Before UpdateEffect : Type=%d", pEffect->GetFrameID());	
			//g_pDebugMessage->Next();
		//#endif

		if( pEffect->GetEffectSpriteType() == EFFECTSPRITETYPE_FAST_MOVE_FLY ||
			pEffect->GetEffectSpriteType() == EFFECTSPRITETYPE_FAST_MOVE_STOP //||
//			(
//			pEffect->GetEffectSpriteType() == EFFECTSPRITETYPE_SUMMON_SYLPH_GREEN_FLY &&
//			pEffect->GetEffectSpriteType() == EFFECTSPRITETYPE_SUMMON_SYLPH_BLACK_STOP
//			)
			)
		{		
			BLT_TYPE		bltType = (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_FAST_MOVE_STOP].BltType;
			TYPE_EFFECTSPRITETYPE		EffectSpriteType = pEffect->GetEffectSpriteType();
			bool			bNewSummonSylphEffect = false;
			bool			bNewSummonSylphEffectStop = false;
			
			if( EffectSpriteType >= EFFECTSPRITETYPE_SUMMON_SYLPH_GREEN_FLY &&
				EffectSpriteType <= EFFECTSPRITETYPE_SUMMON_SYLPH_BLACK_STOP )
			{
				bNewSummonSylphEffect = true;

				if( (EffectSpriteType - EFFECTSPRITETYPE_SUMMON_SYLPH_GREEN_FLY) & 0x1 )
					bNewSummonSylphEffectStop = true;
			}
			
			if( GetAction() == ACTION_OUSTERS_FAST_MOVE_STAND && (
				EffectSpriteType != EFFECTSPRITETYPE_FAST_MOVE_STOP ||
				( bNewSummonSylphEffect && !bNewSummonSylphEffectStop )
				)
			  )
			{
				TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_FAST_MOVE_STOP].FrameID;
				
				if( bNewSummonSylphEffect )
					frameID = (*g_pEffectSpriteTypeTable)[ EffectSpriteType + 1 ].FrameID;

				int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);			
			
				pEffect->SetEffectSprite( EFFECTSPRITETYPE_FAST_MOVE_STOP );
				if( bNewSummonSylphEffect )
					pEffect->SetEffectSprite( EffectSpriteType + 1 );
				pEffect->SetFrameID( frameID, maxFrame );
			}
			else
			if( GetAction() == ACTION_OUSTERS_FAST_MOVE_MOVE && 
				( pEffect->GetEffectSpriteType() != EFFECTSPRITETYPE_FAST_MOVE_FLY ||
				( bNewSummonSylphEffect && bNewSummonSylphEffectStop )
			  ) )
			{
				TYPE_FRAMEID	frameID	= (*g_pEffectSpriteTypeTable)[EFFECTSPRITETYPE_FAST_MOVE_FLY].FrameID;
				if( bNewSummonSylphEffect )
					frameID = (*g_pEffectSpriteTypeTable)[ EffectSpriteType - 1 ].FrameID;

				int maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);			

				pEffect->SetEffectSprite( EFFECTSPRITETYPE_FAST_MOVE_FLY );

				if( bNewSummonSylphEffect )
					pEffect->SetEffectSprite( EffectSpriteType - 1 );
				pEffect->SetFrameID( frameID, maxFrame );
			}
		} 
		else if( pEffect->GetEffectSpriteType() >= EFFECTSPRITETYPE_NEW_LAR_SLASH_MALE_FAST &&
				 pEffect->GetEffectSpriteType() <= EFFECTSPRITETYPE_NEW_LAR_SLASH_FEMALE_SLOW )
		{
			if(!(  GetAction() == ACTION_SLAYER_SWORD_2 || GetAction() == ACTION_SLAYER_SWORD_2_SLOW || GetAction() == ACTION_SLAYER_SWORD_2_FAST ) 
				&& m_RepeatCount <= 0 )
			{
				pEffect->SetCount(0);
			}
		}
		else if(		
					(pEffect->GetEffectSpriteType() >= EFFECTSPRITETYPE_REGEN_TOWER_THROW_LIGHT_START &&
					pEffect->GetEffectSpriteType() <= EFFECTSPRITETYPE_REGEN_TOWER_THROW_LIGHT_END &&
					!HasEffectStatus( EFFECTSTATUS_TRYING ) 
					)
			)
		{
			bErase = true;			
		}

		if (bErase == false && pEffect->Update())
		{
			// -_- ������ ���� �ȹٲ۴�..
			if( pEffect->GetEffectSpriteType() != EFFECTSPRITETYPE_ABSORB_SOUL )
				pEffect->SetDirection( m_CurrentDirection );
			// �ְ� ��⸦ ���� Effect�� ��⸦ �����Ѵ�.
//			if (m_MaxEffectLight < pEffect->GetLight())
//			{
//				m_MaxEffectLight = pEffect->GetLight();
//			}

			// ��ġ ����
			pEffect->SetPosition(m_X, m_Y);

			//---------------------------------------------------------------------
			// ��ġ�� �ٲ���ų�
			// ���� ũ��(�þ�)�� �ٲ� ���
			//---------------------------------------------------------------------
			if (x!=pEffect->GetX() || y!=pEffect->GetY()
				|| light != pEffect->GetLight())
			{				
				// �þ� �ٲٱ�
//				g_pZone->UnSetLight(x, y, light);
//				g_pZone->SetLight(pEffect->GetX(), pEffect->GetY(), pEffect->GetLight());

				/*
				// UnSetLightSight
				#ifdef	OUTPUT_DEBUG
					sprintf(g_pDebugMessage->GetCurrent(), "UnSetLightSight : Type=%d, Light=%d, (%d, %d)", pEffect->GetFrameID(), light, x,y);	
					g_pDebugMessage->Next();
				#endif

				// SetLightSight
				#ifdef	OUTPUT_DEBUG
					sprintf(g_pDebugMessage->GetCurrent(), "SetLightSight : Type=%d, Light=%d, (%d, %d)", pEffect->GetFrameID(), pEffect->GetLight(), pEffect->GetX(), pEffect->GetY());	
					g_pDebugMessage->Next();
				#endif
				*/
			}

			//---------------------------------------------------------------------
			// ĳ���� ���� �ٲٴ� Effect�̸�
			// �ϳ��� �����ؾ� �Ѵ�.
			//---------------------------------------------------------------------
			if (pEffect->IsEffectColor())
			{
				ADDON	part = pEffect->GetEffectColorPart();

				//--------------------------------------------------------
				// �κ������� ���� �ٲ�� effect
				//--------------------------------------------------------
				if (part < ADDON_MAX)
				{
					if (bShowColorPart)
					{
						bAddonColorPart[part] = TRUE;
						addonColor[part] = pEffect->GetEffectColor();

						bChangeColor = TRUE;
					}
				}
				//--------------------------------------------------------
				// ��ü ���� �ٲ�� effect
				//--------------------------------------------------------
				else if (bShowColor)
				{
					numColors++;
					//if (m_AttachEffectColor == ATTACHEFFECTCOLOR_NULL || rand()%numColors==0)
					if (m_AttachEffectColor == m_ChangeColorSet || rand()%numColors==0)
					{
						m_AttachEffectColor = pEffect->GetEffectColor();

						bChangeColor = TRUE;
					}
				}				
			}

			//-----------------------------------------------
			//
			// �� Effect�� ������ ���� LinkCount�� ���ؼ�
			// ���� ����Ǵ� Effect�� ������ �����ؾ� �Ѵ�.
			//
			// ����Frame�� EndLinkFrame�� �Ѿ ���
			//
			//-----------------------------------------------
			if (g_CurrentFrame >= pEffect->GetEndLinkFrame()
				&& pEffect->GetLinkSize() != 0)
			{
				// GenerateNext���� 
				// pEffect�� EffectTarget�� NULL�� ������ֱ� ������
				// ���⼭ ���� �ʿ� ����.
				g_pEffectGeneratorTable->GenerateNext( pEffect );

				// pEffect�� ������ �����ؾ� �ϹǷ� ����� �ȵȴ�.
			}

			// ����� �� ���
			iEffect++;
		}
		//---------------------------------------------------------------------
		// �ð��� �� �ż� ������ ���
		//---------------------------------------------------------------------
		else
		{
			bool bUseEffectSprite = pEffect->IsEffectSprite();

			// flag����
			if (bUseEffectSprite)
			{
				m_bAttachEffect[pEffect->GetEffectSpriteType()] = false;	// flag����
			}

			//m_bEffectStatus[pEffect->GetActionInfo()] = false;

			ADDON	part = pEffect->GetEffectColorPart();

			//------------------------------------------------------------
			// Ư���� ������ ������ �ٲ�� ��쵵 �ִ�.
			//------------------------------------------------------------
			if (part < ADDON_MAX)
			{
				m_Addon[part].bEffectColor = FALSE;
			}
			//------------------------------------------------------------
			// ������� ��� ������.
			//------------------------------------------------------------
			else
			{
				for (int p=0; p<ADDON_MAX; p++)
				{
					m_Addon[p].bEffectColor = FALSE;
				}
			}
		
			//---------------------------------------------------------------------
			//
			// ���� ����Ǵ� Effect�� ������ �����ؾ� �Ѵ�.
			//
			//---------------------------------------------------------------------
			if (pEffect->GetLinkSize() != 0)
			{				
				g_pEffectGeneratorTable->GenerateNext( pEffect );
			}

			// ������ Effect�� �þ߸� ������� �ؾ��Ѵ�.
			//if (pEffect->GetBltType()==BLT_EFFECT)
			{
				//m_nAlphaEffect --;

				// ��� ������Effect�� ��������� �þ߸� ���Ž�Ų��.
				//if (m_nAlphaEffect==0)
				{
//					g_pZone->UnSetLight(x, y, light);

					// UnSet LightSight
					/*
					#ifdef	OUTPUT_DEBUG
						sprintf(g_pDebugMessage->GetCurrent(), "RemoveEffect : Type=%d, Light=%d, (%d, %d)", pEffect->GetFrameID(), light, x,y);	
						g_pDebugMessage->Next();
					#endif
					*/
				}
			}

			DEBUG_ADD_FORMAT("[DeleteAttachEffect] id=%d, esType=%d", m_ID, pEffect->GetEffectSpriteType());
			
			// memory����, list����						
			delete pEffect;						// memory����
			
			DEBUG_ADD("[DeleteAttachEffect] OK");

			// list���� �����ϱ� ���ؼ�.. �ӽ÷� ����
			iEffectTemp = iEffect;

			iEffect++;
			m_listEffect.erase( iEffectTemp );	// list���� ����
		}		
	}

	//---------------------------------------------------------------------
	// ���� �ٴ� effect
	//---------------------------------------------------------------------
	// ��� Effect�� Update�Ѵ�.
	//---------------------------------------------------------------------
	iEffect = m_listGroundEffect.begin();
	
	while (iEffect != m_listGroundEffect.end())
	{
		MAttachEffect*	pEffect = *iEffect;
		
		int x		= pEffect->GetX();
		int y		= pEffect->GetY();
		int light = pEffect->GetLight();		

		//---------------------------------------------------------------------
		// Update
		//---------------------------------------------------------------------
		// message : find path
		//#ifdef	OUTPUT_DEBUG
			//sprintf(g_pDebugMessage->GetCurrent(), "Before UpdateEffect : Type=%d", pEffect->GetFrameID());	
			//g_pDebugMessage->Next();
		//#endif

		if (pEffect->Update())
		{
			// �ְ� ��⸦ ���� Effect�� ��⸦ �����Ѵ�.
//			if (m_MaxEffectLight < pEffect->GetLight())
//			{
//				m_MaxEffectLight = pEffect->GetLight();
//			}

			// ��ġ ����
			pEffect->SetPosition(m_X, m_Y);

			//---------------------------------------------------------------------
			// ��ġ�� �ٲ���ų�
			// ���� ũ��(�þ�)�� �ٲ� ���
			//---------------------------------------------------------------------
			if (x!=pEffect->GetX() || y!=pEffect->GetY()
				|| light != pEffect->GetLight())
			{				
				// �þ� �ٲٱ�
//				g_pZone->UnSetLight(x, y, light);
//				g_pZone->SetLight(pEffect->GetX(), pEffect->GetY(), pEffect->GetLight());

				/*
				// UnSetLightSight
				#ifdef	OUTPUT_DEBUG
					sprintf(g_pDebugMessage->GetCurrent(), "UnSetLightSight : Type=%d, Light=%d, (%d, %d)", pEffect->GetFrameID(), light, x,y);	
					g_pDebugMessage->Next();
				#endif

				// SetLightSight
				#ifdef	OUTPUT_DEBUG
					sprintf(g_pDebugMessage->GetCurrent(), "SetLightSight : Type=%d, Light=%d, (%d, %d)", pEffect->GetFrameID(), pEffect->GetLight(), pEffect->GetX(), pEffect->GetY());	
					g_pDebugMessage->Next();
				#endif
				*/
			}

			//-----------------------------------------------
			//
			// �� Effect�� ������ ���� LinkCount�� ���ؼ�
			// ���� ����Ǵ� Effect�� ������ �����ؾ� �Ѵ�.
			//
			// ����Frame�� EndLinkFrame�� �Ѿ ���
			//
			//-----------------------------------------------
			if (g_CurrentFrame >= pEffect->GetEndLinkFrame()
				&& pEffect->GetLinkSize() != 0)
			{
				// GenerateNext���� 
				// pEffect�� EffectTarget�� NULL�� ������ֱ� ������
				// ���⼭ ���� �ʿ� ����.
				g_pEffectGeneratorTable->GenerateNext( pEffect );

				// pEffect�� ������ �����ؾ� �ϹǷ� ����� �ȵȴ�.
			}

			// ����� �� ���
			iEffect++;
		}
		//---------------------------------------------------------------------
		// �ð��� �� �ż� ������ ���
		//---------------------------------------------------------------------
		else
		{
			bool bUseEffectSprite = pEffect->IsEffectSprite();

			// flag����
			if (bUseEffectSprite)
			{
				m_bAttachEffect[pEffect->GetEffectSpriteType()] = false;	// flag����
			}

			//---------------------------------------------------------------------
			//
			// ���� ����Ǵ� Effect�� ������ �����ؾ� �Ѵ�.
			//
			//---------------------------------------------------------------------
			if (pEffect->GetLinkSize() != 0)
			{				
				g_pEffectGeneratorTable->GenerateNext( pEffect );
			}

			// ������ Effect�� �þ߸� ������� �ؾ��Ѵ�.
			//if (pEffect->GetBltType()==BLT_EFFECT)
			{
				//m_nAlphaEffect --;

				// ��� ������Effect�� ��������� �þ߸� ���Ž�Ų��.
				//if (m_nAlphaEffect==0)
				{
//					g_pZone->UnSetLight(x, y, light);

					// UnSet LightSight
					/*
					#ifdef	OUTPUT_DEBUG
						sprintf(g_pDebugMessage->GetCurrent(), "RemoveEffect : Type=%d, Light=%d, (%d, %d)", pEffect->GetFrameID(), light, x,y);	
						g_pDebugMessage->Next();
					#endif
					*/
				}
			}

			DEBUG_ADD_FORMAT("[DeleteAttachEffect] id=%d, esType=%d", m_ID, pEffect->GetEffectSpriteType());
			

			// memory����, list����						
			delete pEffect;						// memory����
			
			
			DEBUG_ADD("[DeleteAttachEffect] OK");
			

			// list���� �����ϱ� ���ؼ�.. �ӽ÷� ����
			iEffectTemp = iEffect;

			iEffect++;
			m_listGroundEffect.erase( iEffectTemp );	// list���� ����
		}		
	}

	//---------------------------------------------------------------------
	// ���� �ٲ�� �� �����ִ� �����̸�..
	//---------------------------------------------------------------------
	if (bChangeColor)
	{
		//---------------------------------------------------------------------
		// ��ü ���� �ٲ�� Effect�� ���� ���
		//---------------------------------------------------------------------	
		if (m_AttachEffectColor == m_ChangeColorSet)//ATTACHEFFECTCOLOR_NULL)
		{
			for (int p=0; p<ADDON_MAX; p++)
			{
				if (bAddonColorPart[p])
				{
					// �� �κи� �ٲ�� �����..
					m_Addon[p].bEffectColor		= TRUE;
					m_Addon[p].EffectColorSet	= addonColor[p];
				}		
				else
				{
					// ���� �����..
					m_Addon[p].bEffectColor		= FALSE;
				}
			}
		}
		//---------------------------------------------------------------------
		// ��ü ���� �ٲ�� Effect�� �ִ� ���
		//---------------------------------------------------------------------
		else
		{
			for (int p=0; p<ADDON_MAX; p++)
			{
				m_Addon[p].bEffectColor		= TRUE;

				if (bAddonColorPart[p])
				{
					// �� �κи� �ٲ�� �����..			
					m_Addon[p].EffectColorSet	= addonColor[p];
				}		
				else
				{
					// ��ü �ٲ�� �����..				
					m_Addon[p].EffectColorSet	= m_AttachEffectColor;
				}
			}
		}
	}
	//---------------------------------------------------------------------
	// ���� �ٲ�°� �� �����ִ� ����...
	//---------------------------------------------------------------------
	else
	{
		for (int p=0; p<ADDON_MAX; p++)
		{
			m_Addon[p].bEffectColor		= FALSE;
		}
	}
}

const MCreatureWear::ADDON_INFO&	MCreatureWear::GetACAddonInfo(int Addon) const
{ 
	switch( Addon )
	{
	case AC_ADDON_BODY :
		return m_Addon[ ADDON_COAT ];
		
	case AC_ADDON_HELM :
		return m_Addon[ ADDON_HELM ];

	case AC_ADDON_LEFTHAND :
		return m_Addon[ ADDON_LEFTHAND ];

	case AC_ADDON_RIGHTHAND :
		return m_Addon[ ADDON_RIGHTHAND ];

	case AC_ADDON_MOTOR :
		return m_Addon[ ADDON_MOTOR ];
	}

	return m_ShoulderAddon;
}

TYPE_SOUNDID
MCreatureWear::ResolveMotorcycleSoundID(ITEM_CLASS itemClass, TYPE_ITEMTYPE itemType, MOTORCYCLE_SOUND_KIND kind)
{
	if (itemClass != ITEM_CLASS_MOTORCYCLE)
		return SOUNDID_NULL;

	const ITEMTABLE_INFO& info = (*g_pItemTable)[ITEM_CLASS_MOTORCYCLE][itemType];

	switch (kind)
	{
	case MOTORCYCLE_SOUND_MOUNT:
		if (info.GearSoundID != SOUNDID_NULL)
			return info.GearSoundID;
		if (info.UseSoundID != SOUNDID_NULL)
			return info.UseSoundID;
		return SOUND_WORLD_BIKE_GO;

	case MOTORCYCLE_SOUND_MOVE:
		if (info.UseSoundID != SOUNDID_NULL)
			return info.UseSoundID;
		return SOUNDID_NULL;

	case MOTORCYCLE_SOUND_DISMOUNT:
		if (info.TileSoundID != SOUNDID_NULL)
			return info.TileSoundID;
		return SOUND_WORLD_BIKE_STOP;

	default:
		return SOUNDID_NULL;
	}
}

TYPE_SOUNDID
MCreatureWear::GetRideMotorcycleSoundID(MOTORCYCLE_SOUND_KIND kind) const
{
	const ADDON_INFO& addon = m_Addon[ADDON_MOTOR];
	if (!addon.bAddon || addon.ItemClass != ITEM_CLASS_MOTORCYCLE)
		return SOUNDID_NULL;

	return ResolveMotorcycleSoundID(addon.ItemClass, addon.ItemType, kind);
}

#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
//�����̾ .. ������� ž�½�.. Addon�� ��ϵȴ�. ����, �� �Լ��� �����̾��� ��쿡�� ��ȿ�ϴ�.
bool	MCreatureWear::IsRide_Fast_Transforter()
{
	ADDON_INFO& addon = m_Addon[ADDON_MOTOR];
	if (addon.bAddon && (addon.ItemType == 8	//�ű� �̵������� �߰��Ǹ�..���⿡ ItemType�� �ϵ��ڵ��� ...
#if __CONTENTS(__SECOND_TRANSFORTER)
		|| addon.ItemType == 9
		|| addon.ItemType == 10
		|| addon.ItemType == 11
#endif //__SECOND_TRANSFORTER
		))
		return true;
	
	return false;
}

bool	MCreatureWear::IsRide_Air_Transporter()
{
	ADDON_INFO& addon = m_Addon[ADDON_MOTOR];
	if (!addon.bAddon)
		return false;

#if __CONTENTS(__SECOND_TRANSFORTER)
	return addon.ItemType == 9
		|| addon.ItemType == 11;
#else
	return false;
#endif //__SECOND_TRANSFORTER
}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
