//----------------------------------------------------------------------
// MSlayerGear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "MItem.h"
#include "MSlayerGear.h"
#include "MQuickSlot.h"

#ifdef __GAME_CLIENT__
	#include "UIFunction.h"
	#include "ClientFunction.h"
	#include "MPlayer.h"
	#include "DebugInfo.h"
#endif
#include "MTimeItemManager.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MSlayerGear*		g_pSlayerGear = NULL;
MSlayerGear*		g_pSlayerGearOther = NULL;

MMagazine*		g_pCurrentMagazine = NULL;

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
const int MSlayerGear::s_Slot_Offset_Cue = MSlayerGear::GEAR_SLAYER_CUE1 - MSlayerGear::GEAR_SLAYER_BRACELET1;

MSlayerGear::FUNCTION_GEARSLOT_CHECK

//nanomech 2006.03.08 Item수정 작업 (Index수정)
MSlayerGear::s_GearSlotCheckTable[MAX_GEAR_SLAYER] =
{
	IsGearSlotHelm,				// 모자
	IsGearSlotNecklace,			// 목걸이
	IsGearSlotCoat,				// 상의
	IsGearSlotLeftHand,			// 왼손
	IsGearSlotRightHand,		// 오른손
	IsGearSlotGlove,			// 장갑
	IsGearSlotBelt,				// 혁대
	IsGearSlotTrouser,			// 하의			
	IsGearSlotBracelet,			// 팔찌1
	IsGearSlotBracelet,			// 팔찌2
	IsGearSlotRing,				// 반지1
	IsGearSlotRing,				// 반지2
	IsGearSlotRing,				// 반지3
	IsGearSlotRing,				// 반지4
	IsGearSlotShoes,			// 신발

	IsGearSlotCoreZap,			// 코어잽1
	IsGearSlotCoreZap,			// 코어잽2
	IsGearSlotCoreZap,			// 코어잽3
	IsGearSlotCoreZap,			// 코어잽4

	IsGearSlotPDA,				// PDA
	IsGearSlotShoulder,			// 어깨

	IsGearSlotEtherealChain,	// 목걸이 코어잽

	IsGearSlotCue,				// 큐1
	IsGearSlotCue,				// 큐2

	IsGearSlotBloodBible,		// 블러드 바이블 1
	IsGearSlotBloodBible,		// 블러드 바이블 2
	IsGearSlotBloodBible,		// 블러드 바이블 3
	IsGearSlotBloodBible,		// 블러드 바이블 4
	IsGearSlotBloodBible,		// 블러드 바이블 5
	IsGearSlotBloodBible,		// 블러드 바이블 6
};

//----------------------------------------------------------------------
// 특정 부위에 pItem을 장착할 수 있는지를 check한다.
//----------------------------------------------------------------------
bool		MSlayerGear::IsGearSlotHelm(MItem* pItem)		{ return pItem->IsGearSlotHelm(); }		// 모자
bool		MSlayerGear::IsGearSlotNecklace(MItem* pItem) 	{ return pItem->IsGearSlotNecklace(); }	// 목걸이
bool		MSlayerGear::IsGearSlotCoat(MItem* pItem) 		{ return pItem->IsGearSlotCoat(); }		// 상의
bool		MSlayerGear::IsGearSlotLeftHand(MItem* pItem)	{ return pItem->IsGearSlotLeftHand(); }	// 왼손	
bool		MSlayerGear::IsGearSlotRightHand(MItem* pItem)	{ return pItem->IsGearSlotRightHand(); }	// 오른손
bool		MSlayerGear::IsGearSlotGlove(MItem* pItem) 		{ return pItem->IsGearSlotGlove(); }	// 장갑		
bool		MSlayerGear::IsGearSlotBelt(MItem* pItem) 		{ return pItem->IsGearSlotBelt(); }		// 혁대
bool		MSlayerGear::IsGearSlotTrouser(MItem* pItem) 	{ return pItem->IsGearSlotTrouser(); }	// 하의		
bool		MSlayerGear::IsGearSlotBracelet(MItem* pItem) 	{ return pItem->IsGearSlotBracelet(); }	// 팔찌
bool		MSlayerGear::IsGearSlotRing(MItem* pItem) 		{ return pItem->IsGearSlotRing(); }		// 반지
bool		MSlayerGear::IsGearSlotShoes(MItem* pItem)		{ return pItem->IsGearSlotShoes(); }	// 신발	
bool		MSlayerGear::IsGearSlotCoreZap(MItem* pItem)	{ return pItem->IsGearSlotCoreZap(); }	// 코어잽
bool		MSlayerGear::IsGearSlotPDA(MItem* pItem)		{ return pItem->IsGearSlotPDA(); }// PDA
bool		MSlayerGear::IsGearSlotShoulder(MItem* pItem)	{ return pItem->IsGearSlotShoulder(); }// Shoulder

bool		MSlayerGear::IsGearSlotBloodBible(MItem* pItem)	{ return pItem->IsGearSlotBloodBible(); }// 블러드 바이블
//nanomech 2006.03.08 Item수정 작업 start
bool		MSlayerGear::IsGearSlotEtherealChain(MItem* pItem)	{ return pItem->IsGearSlotEtherealChain(); }	// 목걸이 코어잽
//nanomech 2006.03.08 Item수정 작업 end

bool		MSlayerGear::IsGearSlotCue(MItem* pItem)		{ return pItem->IsGearSlotCue(); }	// 목걸이 코어잽


//----------------------------------------------------------------------
//
// constructor
// 
//----------------------------------------------------------------------
MSlayerGear::MSlayerGear()
{
	g_pQuickSlot = NULL;
	m_bBloodBibleOpenCount = 0;
}

MSlayerGear::~MSlayerGear()
{
	g_pQuickSlot = NULL;
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init 
//----------------------------------------------------------------------
// MAX_GEAR_SLAYER만큼의 Slot을 생성한다.
//----------------------------------------------------------------------
void
MSlayerGear::Init()
{
	// slot 생성
	MPlayerGear::Init(MAX_GEAR_SLAYER);

	if ( !m_bIsOtherGear )
		g_pCurrentMagazine = NULL;
	//nanomech 2006.03.08 Item수정 작업 start
	//m_Gilles_CoreZap = 5;
	m_Gilles_CoreZap = GEAR_SLAYER::GEAR_SLAYER_ZAP1-GEAR_SLAYER::GEAR_SLAYER_RING1;
	//nanomech 2006.03.08 Item수정 작업 end
	m_bBloodBibleOpenCount = 0;
}


//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------
// 특정 아이템의 착용 가능 여부를 체크한다.
//----------------------------------------------------------------------
void			
MSlayerGear::CheckAffectStatus(MItem* pItem)
{
	#ifdef __GAME_CLIENT__

		g_pPlayer->CheckAffectStatus( pItem );

	#endif
}

//----------------------------------------------------------------------
// Add Item ( pItem )
//----------------------------------------------------------------------
// pItem을 적절한(?) slot에 추가시킨다.
//----------------------------------------------------------------------
bool			
MSlayerGear::AddItem(MItem* pItem)
{
	for (int i=0; i<MAX_GEAR_SLAYER; i++)
	{
		// 추가했으면..
		if (AddItem( pItem, (enum GEAR_SLAYER)i ))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Can Replace Item
//----------------------------------------------------------------------
// 실제로는 Add or Replace를 의미한다.
// (n)에 pItem을 넣을 수 있거나
// (n)에 놓았을 때, 다른 것 하나와 교체가 될 경우는 true이다.
// pOldItem에는 교체될 것이 있는 경우에 그게 들어간다.
//----------------------------------------------------------------------
bool			
MSlayerGear::CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	// 수치 적용되지 않는 아이템은 착용되지 않는다
	if(pItem->IsAffectStatus() == false && !pItem->IsQuestItem() )
		return false;
	//20080225 리미티드의 경우에 성별이 다른 코트 아이템을 장착한 채로 변신을 할경우 변신을 풀 
	//아이템이 외형 값을 잘못 설정해주게 되고 크리처 팩 참조를  캐릭터 팩 참조로 변경해야 하나 그 작업을 못해
	//일어나는 현상을 수정하기 위해서 리미티드의 경우에도 성별이 다르면 입지 못하게 한다.
#if __CONTENTS(__LIMITED_ITEM_UNISEX)
	if((pItem->IsGenderForFemale() && g_pPlayer->IsMale()) ||
		(pItem->IsGenderForMale() && g_pPlayer->IsFemale()) )
		return false;
#endif	//__LIMITED_ITEM_UNISEX
	pOldItem = NULL;

	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsSlayerItem())
	{
		return false;
	}
	
	if(n>=GEAR_SLAYER_BLOOD_BIBLE1 && n<=GEAR_SLAYER_BLOOD_BIBLE6) // 블러드 바이블은 집을수 없다.
		return false;

	/*
	if(n == GEAR_SLAYER_PDA)
	{
		if(pItem->IsGearSlotPDA() == true
#ifdef __GAME_CLIENT__	
			&& g_pPlayer->IsAdvancementClass()
#endif	
			)
		{
			if (m_ItemSlot[GEAR_SLAYER_PDA]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_SLAYER_PDA];
				return true;
			}
		}

	}

	if(n == GEAR_SLAYER_SHOULDER)
	{
		if(pItem->IsGearSlotShoulder()== true
#ifdef __GAME_CLIENT__	
			&& g_pPlayer->IsAdvancementClass()
#endif	
			)
		{
			
			if (m_ItemSlot[GEAR_SLAYER_SHOULDER]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_SLAYER_SHOULDER];
				return true;
			}
		}
		
	}
*/

	//-------------------------------------------------
	//
	// 손에 장착할려는 경우
	//
	//-------------------------------------------------
	if (n==GEAR_SLAYER_LEFTHAND || n==GEAR_SLAYER_RIGHTHAND)	
	{
		//-------------------------------------------------
		// [ 양손 Item ] 인 경우
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			// 양손의 Slot이 모두 비어있는 경우 
			//-------------------------------------------------			
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]==NULL)
			{
				return true;
			}
			
			//-------------------------------------------------
			// 양손에 다 뭔가 있는 경우
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				// 양손에 있는게 같다면..
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==m_ItemSlot[GEAR_SLAYER_LEFTHAND])
				{
					pOldItem = m_ItemSlot[GEAR_SLAYER_RIGHTHAND];
					return true;
				}
				
				return false;				
			}

			//-------------------------------------------------
			// 오른손에만 뭔가 있는 경우
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL)
			{
				pOldItem = m_ItemSlot[GEAR_SLAYER_RIGHTHAND];
				return true;
			}

			//-------------------------------------------------
			// 왼손에만뭔가 있는 경우
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{			
				pOldItem = m_ItemSlot[GEAR_SLAYER_LEFTHAND];
				return true;
			}

			// 이런 경우가 있을 수 있을까? - -;;
			return false;
		}

		//-------------------------------------------------
		// [ 오른손 Item ] 인 경우
		//-------------------------------------------------				
		else if (pItem->IsGearSlotRightHand())
		{
			// NULL이거나 말거나 그냥 넣어주면 된다.
			pOldItem = m_ItemSlot[GEAR_SLAYER_RIGHTHAND];
			return true;
		}
		
		//-------------------------------------------------				
		// [ 왼손 Item ] 인 경우
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			pOldItem = m_ItemSlot[GEAR_SLAYER_LEFTHAND];
			return true;
		}

		// 손에 장착하는게 아닌 경우
		return false;
	}
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if (n >= GEAR_SLAYER_RING1 && n <= GEAR_SLAYER_RING4)	
	{
		if (pItem->IsGearSlotRing())
		{
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL) // 코어잽이 없어야 링을 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotCoreZap())
		{			
			if (m_ItemSlot[n]!=NULL) // 코어잽은 링이 있어야 놓을 수 있다. 
			{
				if (m_ItemSlot[n]!=NULL) // 코어잽은 링이 있어야 놓을 수 있다. 
				{
					if(m_ItemSlot[n+m_Gilles_CoreZap]!=NULL)
						pOldItem = m_ItemSlot[n+m_Gilles_CoreZap];
					else
						pOldItem = NULL;
					return true;
				}
			}
		}
		return false;
	}
	else if(n >= GEAR_SLAYER_ZAP1 && n <= GEAR_SLAYER_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
		return false;

	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if (n == GEAR_SLAYER_NECKLACE )
	{
		if (pItem->IsGearSlotNecklace())
		{
			if (m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]==NULL) // 에테리얼 체인이 없어야 목걸이를 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotEtherealChain())
		{			
			if (m_ItemSlot[n]!=NULL) // 에테리얼 체인은 목걸이가 있어야 놓을 수 있다. 
			{
				if (m_ItemSlot[n]!=NULL) // 에테리얼 체인은 목걸이가 있어야 놓을 수 있다. 
				{
					if(m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]!=NULL)
						pOldItem = m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN];
					else
						pOldItem = NULL;
					return true;
				}
			}
		}
		return false;
	}
	else if(n == GEAR_SLAYER_ETHEREAL_CHAIN )
		return false;
	// nanomech 2006.03.09 Item 수정 작업 end



	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if (n >= GEAR_SLAYER_BRACELET1 && n <= GEAR_SLAYER_BRACELET2)	
	{
		if (pItem->IsGearSlotBracelet())
		{
			if (m_ItemSlot[n+s_Slot_Offset_Cue]==NULL) // 큐기 없어야 팔찌 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotCue())
		{			
			if (m_ItemSlot[n]!=NULL) // 큐는 팔찌가 있어야 놓을 수 있다. 
			{
				if (m_ItemSlot[n]!=NULL) // 큐는 팔찌가 있어야 놓을 수 있다. 
				{
					if(m_ItemSlot[n+s_Slot_Offset_Cue]!=NULL)
						pOldItem = m_ItemSlot[n+s_Slot_Offset_Cue];
					else
						pOldItem = NULL;
					return true;
				}
			}
		}
		return false;
	}
	else if(n >= GEAR_SLAYER_CUE1 && n <= GEAR_SLAYER_CUE2) // 큐 슬롯은 보이지 않는다. -_-?? 
		return false;

	
	//----------------------------------------------------
	// 적절한 Slot인지 확인해야 한다.
	//----------------------------------------------------	
	if (pItem->IsGearItem()	&&				// pItem이 GearItem인지 확인..
		(*s_GearSlotCheckTable[n])(pItem)	// 적절한 Slot인지 확인
#ifdef __GAME_CLIENT__	// 승직 아이템 체크
		&& (pItem->IsAdvancementClassItem() && g_pPlayer->IsAdvancementClass() ||
			!pItem->IsAdvancementClassItem())
#endif	
		)	
	{
		// 위치만 맞으면 교환되거나 추가될 수 있다.
		pOldItem = m_ItemSlot[n];
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Add Item ( pItem, n )
//----------------------------------------------------------------------
// slot(n)에 pItem을 추가한다.
// 추가할 수 있는 slot인지 확인해야 한다.
//----------------------------------------------------------------------
bool
MSlayerGear::AddItem(MItem* pItem, GEAR_SLAYER n)
{
	CT_Done("MSGAI: ENTER this=%p pItem=%p n=%d m_Size=%d", (void*)this, (void*)pItem, (int)n, (int)m_Size);
	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------
	if (n>=m_Size) {
		CT_Done("MSGAI: n>=m_Size return false");
		return false;
	}
	CT_Done("MSGAI: pre IsSlayerItem (virtual)");
	bool bSlayerItem = pItem->IsSlayerItem();
	CT_Done("MSGAI: post IsSlayerItem=%d", (int)bSlayerItem);
	if (!bSlayerItem)
	{
		CT_Done("MSGAI: !IsSlayerItem return false");
		return false;
	}

	//-------------------------------------------------
	// 양손 Item인 경우는 따로 체크해준다.
	//-------------------------------------------------
	CT_Done("MSGAI: pre LEFT/RIGHT check");
	bool bIsLeftOrRight = (n==GEAR_SLAYER_LEFTHAND || n==GEAR_SLAYER_RIGHTHAND);
	CT_Done("MSGAI: bIsLeftOrRight=%d, pre IsGearSlotTwoHand (virtual)", (int)bIsLeftOrRight);
	bool bTwoHand = bIsLeftOrRight ? pItem->IsGearSlotTwoHand() : false;
	CT_Done("MSGAI: bTwoHand=%d", (int)bTwoHand);
	if (bIsLeftOrRight && bTwoHand)
	{
		CT_Done("MSGAI: TwoHand branch");
		// 양손의 Slot이 모두 비어있는 상태에만 추가 가능하다.
		CT_Done("MSGAI-TH: pre check both slots NULL");
		if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==NULL
				&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]==NULL)
		{
			CT_Done("MSGAI-TH: both slots NULL, pre MPlayerGear::AddItem(RIGHTHAND)");
			// 오른손에 추가한다.
			if (MPlayerGear::AddItem( pItem, GEAR_SLAYER_RIGHTHAND ))
			{
				CT_Done("MSGAI-TH: post MPlayerGear::AddItem(RIGHTHAND) OK, pre m_ItemSlot[LEFTHAND]=pItem");
				// 왼손에도 추가해준다.
				m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;
				CT_Done("MSGAI-TH: post m_ItemSlot[LEFTHAND]=pItem assign");

				//-------------------------------------------------
				// 총이면 현재 탄창 설정
				//-------------------------------------------------
				CT_Done("MSGAI-TH: pre IsGunItem (virtual)");
				bool bGun = pItem->IsGunItem();
				CT_Done("MSGAI-TH: post IsGunItem=%d m_bIsOtherGear=%d", (int)bGun, (int)m_bIsOtherGear);
				if (bGun && !m_bIsOtherGear)
				{
					CT_Done("MSGAI-TH: gun branch, pre GetMagazine");
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
					CT_Done("MSGAI-TH: post GetMagazine");
				}

				//-------------------------------------------------
				// 제대로 추가된 경우 --> sound출력
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					CT_Done("MSGAI-TH: pre GetGearSoundID (virtual)");
					int soundID = pItem->GetGearSoundID();
					CT_Done("MSGAI-TH: post GetGearSoundID=%d, pre PlaySound", soundID);
					PlaySound( soundID );
					CT_Done("MSGAI-TH: post PlaySound, pre g_pPlayer->CalculateStatus");
					g_pPlayer->CalculateStatus();
					CT_Done("MSGAI-TH: post CalculateStatus");
				#endif

				CT_Done("MSGAI-TH: return true");
				return true;
			}
			CT_Done("MSGAI-TH: MPlayerGear::AddItem(RIGHTHAND) failed, return false");

			return false;
		}
		CT_Done("MSGAI-TH: slots not both NULL, return false");

		return false;
	}
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if (n >= GEAR_SLAYER_RING1 && n <= GEAR_SLAYER_RING4)	
	{
		if (pItem->IsGearSlotRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL&&m_ItemSlot[n]==NULL) // 링도 없고 코어잽이 없어야 링을 놓을 수 있음
			{
				if (MPlayerGear::AddItem( pItem, n ))	
				{
					//-------------------------------------------------
					// 제대로 추가된 경우 --> sound출력
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}
			}
		}
		else 
		if (pItem->IsGearSlotCoreZap())
		{
			if (m_ItemSlot[n-m_Gilles_CoreZap]!=NULL && m_ItemSlot[n]==NULL) // 해당위치에 링이 있고 코어잽이 없을 경우만 코어잽 추가
			{		
				if (MPlayerGear::AddItem( pItem, n + m_Gilles_CoreZap ))// 코어잽 위치에 추가
				{
					//-------------------------------------------------
					// 제대로 추가된 경우 --> sound출력
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}

			}
		}

		return false;
	}
	
	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if (n == GEAR_SLAYER_NECKLACE )
	{
		if (pItem->IsGearSlotNecklace())
		{			
			if (m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]==NULL&&m_ItemSlot[n]==NULL) // 목걸이도 없고 에테리얼 체인이 없어야 링을 놓을 수 있음
			{
				if (MPlayerGear::AddItem( pItem, n ))	
				{
					//-------------------------------------------------
					// 제대로 추가된 경우 --> sound출력
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}
			}
		}
		else 
		if (pItem->IsGearSlotEtherealChain())
		{
			if (m_ItemSlot[GEAR_SLAYER_NECKLACE]!=NULL && m_ItemSlot[n]==NULL) // 해당위치에 목걸이가 있고 에테리얼 체인이 없을 경우만 코어잽 추가
			{		
				if (MPlayerGear::AddItem( pItem, GEAR_SLAYER_ETHEREAL_CHAIN ))// 에테리얼 체인 위치에 추가
				{
					//-------------------------------------------------
					// 제대로 추가된 경우 --> sound출력
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}

			}
		}

		return false;
	}
	// nanomech 2006.03.09 Item 수정 작업 end


	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if (n >= GEAR_SLAYER_BRACELET1 && n <= GEAR_SLAYER_BRACELET2)	
	{
		if (pItem->IsGearSlotBracelet())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]==NULL&&m_ItemSlot[n]==NULL) // 팔찌도 없고 큐가 없어야 팔찌를 놓을 수 있음
			{
				if (MPlayerGear::AddItem( pItem, n ))	
				{
					//-------------------------------------------------
					// 제대로 추가된 경우 --> sound출력
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}
			}
		}
		else 
		if (pItem->IsGearSlotCue())
		{
			if (m_ItemSlot[n-s_Slot_Offset_Cue]!=NULL && m_ItemSlot[n]==NULL)	// 해당위치에 팔찌가 있고 큐가 없을 경우만 큐 추가
			{		
				if (MPlayerGear::AddItem( pItem, n + s_Slot_Offset_Cue ))		// 큐 위치에 추가
				{
					//-------------------------------------------------
					// 제대로 추가된 경우 --> sound출력
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();	
					#endif

					return true;
				}

			}
		}

		return false;
	}


//	else if(n >= GEAR_SLAYER_ZAP1 && n <= GEAR_SLAYER_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;
	//----------------------------------------------------
	// 적절한 Slot인지 확인해야 한다.
	//----------------------------------------------------
	CT_Done("MSGAI: generic branch, pre m_ItemSlot[n]==NULL check");
	bool bSlotEmpty = (m_ItemSlot[n]==NULL);
	CT_Done("MSGAI: bSlotEmpty=%d, pre IsGearItem (virtual)", (int)bSlotEmpty);
	bool bGearItem = bSlotEmpty ? pItem->IsGearItem() : false;
	CT_Done("MSGAI: bGearItem=%d, pre s_GearSlotCheckTable[%d] fn-ptr call", (int)bGearItem, (int)n);
	bool bSlotCheck = (bSlotEmpty && bGearItem) ? (*s_GearSlotCheckTable[n])(pItem) : false;
	CT_Done("MSGAI: bSlotCheck=%d", (int)bSlotCheck);
	if (bSlotEmpty && bGearItem && bSlotCheck)
	{
		CT_Done("MSGAI: pre MPlayerGear::AddItem(n=%d)", (int)n);
		// n번째 slot에 pItem을 추가한다.
		bool bAdd = MPlayerGear::AddItem( pItem, n );
		CT_Done("MSGAI: post MPlayerGear::AddItem bAdd=%d", (int)bAdd);

		//--------------------------------------------
		// 제대로 넣었으면..
		//--------------------------------------------
		if (bAdd)
		{
			//--------------------------------------------
			// belt인 경우
			//--------------------------------------------
			// QuickSlot이 생긴 경우이다.
			//--------------------------------------------
			if (n==GEAR_SLAYER_BELT)
			{
				if (pItem->GetItemClass()==ITEM_CLASS_BELT && !m_bIsOtherGear)
				{
					g_pQuickSlot = (MBelt*)pItem;
					
					#ifdef __GAME_CLIENT__
						UI_ResetQuickItemSlot();
					#endif
				}
				else
				{
					// 이런 경우는 있을 수 없지 싶다.					
				}
			}
			else
			{
				//-------------------------------------------------
				// 총이면 현재 탄창 설정
				//-------------------------------------------------
				if (pItem->IsGunItem() && !m_bIsOtherGear)
				{
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
				}
			}

			//-------------------------------------------------
			// 제대로 추가된 경우 --> sound출력
			//-------------------------------------------------
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetGearSoundID() );
				g_pPlayer->CalculateStatus();	
			#endif

			return true;
		}
		
		//--------------------------------------------
		// 실패한 경우..
		//--------------------------------------------
		return false;		
	}

	return false;
}

//----------------------------------------------------------------------
// Remove Item ( n)
//----------------------------------------------------------------------
MItem*			
MSlayerGear::RemoveItem(GEAR_SLAYER n)
{ 
	MItem* pItem = m_ItemSlot[n];
		
	//-----------------------------------------------------
	// 없는 경우
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//-----------------------------------------------------
	// 양손 무기이면 오른손으로 설정한다.
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		n = GEAR_SLAYER_RIGHTHAND;
	}
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if(n>=GEAR_SLAYER_RING1 && n<= GEAR_SLAYER_RING4) 
	{
		if(NULL != m_ItemSlot[n+m_Gilles_CoreZap]) // 코어잽이 붙어 있으면 코어잽을 뗀다
			n = (GEAR_SLAYER)(n + m_Gilles_CoreZap); // 코어잽 위치
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if( n==GEAR_SLAYER_NECKLACE )
	{
		if(NULL != m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]) // 에테리얼 체인이 붙어 있으면 에테리얼 체인을 뗀다
			n = (GEAR_SLAYER)(GEAR_SLAYER_ETHEREAL_CHAIN); // 에테리얼 체인 위치
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if(n>=GEAR_SLAYER_BRACELET1 && n<= GEAR_SLAYER_BRACELET2) 
	{
		if(NULL != m_ItemSlot[n+s_Slot_Offset_Cue]) // 코어잽이 붙어 있으면 코어잽을 뗀다
			n = (GEAR_SLAYER)(n + s_Slot_Offset_Cue); // 코어잽 위치
	}

//	else if(n >= GEAR_SLAYER_ZAP1 && n <= GEAR_SLAYER_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;

	pItem = MPlayerGear::RemoveItem( (BYTE)n );

	//-----------------------------------------------------
	// 없는 경우
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//-----------------------------------------------------
	// Belt인 경우
	//-----------------------------------------------------
	// QuickSlot이 제거된 경우이다.
	//-----------------------------------------------------
	if (n==GEAR_SLAYER_BELT)
	{
		g_pQuickSlot = NULL;

		#ifdef __GAME_CLIENT__
			g_pPlayer->CalculateStatus();
		#endif

		return pItem;
	}

	//-----------------------------------------------------
	// 양손 Item인 경우 양손의 Slot을 모두 제거해줘야 한다.
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_SLAYER_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = NULL;
	}
	
	//-------------------------------------------------
	// 총이면 현재 탄창 제거
	//-------------------------------------------------
	if (pItem->IsGunItem())
	{
		g_pCurrentMagazine = NULL;
	}

	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	return pItem;
}

//----------------------------------------------------------------------
// RemoveItem : id가 같은 item을 제거한다.
//----------------------------------------------------------------------
MItem*			
MSlayerGear::RemoveItem(TYPE_OBJECTID id)
{ 
	MItem* pItem = MPlayerGear::RemoveItem( id ); 

	//-----------------------------------------------------
	// 없는 경우
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	// 있으면...

	//-----------------------------------------------------
	// Belt인 경우
	//-----------------------------------------------------
	// QuickSlot이 제거된 경우이다.
	//-----------------------------------------------------
	if (pItem->GetItemClass()==ITEM_CLASS_BELT)
	{
		g_pQuickSlot = NULL;

		#ifdef __GAME_CLIENT__
			g_pPlayer->CalculateStatus();
		#endif

		return pItem;
	}

	//-----------------------------------------------------
	// 양손 Item인 경우 양손의 Slot을 모두 제거해줘야 한다.
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_SLAYER_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = NULL;
	}

	//-------------------------------------------------
	// 총이면 현재 탄창 제거
	//-------------------------------------------------
	if (pItem->IsGunItem())
	{
		g_pCurrentMagazine = NULL;
	}

	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	return pItem;
}

//----------------------------------------------------------------------
// ReplaceItem : pItem을 추가하고 딴게 있다면 Item교환
//----------------------------------------------------------------------
bool			
MSlayerGear::ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsSlayerItem())
	{
		pOldItem = NULL;
		return false;
	}

	/*
	if(n == GEAR_SLAYER_PDA)
	{
		if(pItem->IsGearSlotPDA()== true)
		{
		if (m_ItemSlot[GEAR_SLAYER_PDA]==NULL)
		{
			MPlayerGear::ReplaceItem( pItem, GEAR_SLAYER_PDA, pOldItem );
			return true;
		}
		else
			return false;
		}
	}

	if(n == GEAR_SLAYER_SHOULDER)
	{
		if(pItem->IsGearSlotShoulder()== true)
		{
			if (m_ItemSlot[GEAR_SLAYER_SHOULDER]==NULL)
			{
				MPlayerGear::ReplaceItem( pItem, GEAR_SLAYER_SHOULDER, pOldItem );
				return true;
			}
			else
				return false;
		}	
	}
	*/
	
	//-------------------------------------------------
	//
	// 손에 장착할려는 경우
	//
	//-------------------------------------------------
	if (n==GEAR_SLAYER_LEFTHAND || n==GEAR_SLAYER_RIGHTHAND)	
	{
		bool bAdd = false;
			
		//-------------------------------------------------
		// [ 양손 Item ] 인 경우
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			// 양손의 Slot이 모두 비어있는 경우
			//-------------------------------------------------
			// 그냥 추가한다.
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]==NULL)
			{
				// 그냥 추가한다.
				pOldItem = NULL;
				if (MPlayerGear::AddItem( pItem, GEAR_SLAYER_RIGHTHAND ))
				{
					//-------------------------------------------------
					// item의 내구성 상태 설정
					//-------------------------------------------------
					CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );

					pItem->SetItemSlot( GEAR_SLAYER_RIGHTHAND );
					m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;
					m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = pItem;

					//-------------------------------------------------
					// 총이면 현재 탄창 설정
					//-------------------------------------------------
					if (pItem->IsGunItem() && !m_bIsOtherGear)
					{
						MGunItem* pGunItem = (MGunItem*)pItem;
						g_pCurrentMagazine = pGunItem->GetMagazine();

						//------------------------------------------
						// 총알이 없는 경우
						//------------------------------------------
						if (g_pCurrentMagazine==NULL)
						{
							MMagazine* pMagazine = (MMagazine*)MItem::NewItem( (ITEM_CLASS)ITEM_CLASS_MAGAZINE );

							// 의미 없음 - -;
							pMagazine->SetID( 0 );

							// 이거는 총에 맞춰서 해줘야된다.
							for (int j=0; j<(*g_pItemTable)[ITEM_CLASS_MAGAZINE].GetSize(); j++)			
							{
								pMagazine->SetItemType(	j );

								if (pMagazine->IsInsertToItem( pItem ))
								{
									break;
								}
							}

							// 의미 없음
							pMagazine->ClearItemOption();
						
							// 탄창 개수
							pMagazine->SetNumber( 0 );

							// 탄창 설정
							pGunItem->SetMagazine( pMagazine );
						}
					}				

					//-------------------------------------------------
					// 제대로 추가된 경우 --> sound출력
					//-------------------------------------------------
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );						
						g_pPlayer->CalculateStatus();			
					#endif

					return true;
				}

				return false;
			}
			
			//-------------------------------------------------
			// 양손에 다 뭔가 있는 경우
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				// 양손에 있는게 같다면..
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]==m_ItemSlot[GEAR_SLAYER_LEFTHAND])
				{
					// 양손 무기와 교체하는 경우이다.
					if (MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_RIGHTHAND, 
													pOldItem ))
					{
						//-------------------------------------------------
						// item의 내구성 상태 설정
						//-------------------------------------------------
						CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );

						//pItem->SetItemSlot( GEAR_SLAYER_RIGHTHAND );
						m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;
						//m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = pItem;

						//-------------------------------------------------
						// 제거된 것이 총인 경우
						//-------------------------------------------------
						if (pOldItem!=NULL && pOldItem->IsGunItem())
						{
							g_pCurrentMagazine = NULL;
						}

						//-------------------------------------------------
						// 총이면 현재 탄창 설정
						//-------------------------------------------------
						if (pItem->IsGunItem() && !m_bIsOtherGear)
						{
							MGunItem* pGunItem = (MGunItem*)pItem;
							g_pCurrentMagazine = pGunItem->GetMagazine();
						}
					
						//-------------------------------------------------
						// 제대로 추가된 경우 --> sound출력
						//-------------------------------------------------
						#ifdef __GAME_CLIENT__
							PlaySound( pItem->GetGearSoundID() );
							g_pPlayer->CalculateStatus();	
						#endif	

						return true;
					}

					return false;
				}
				
				//-------------------------------------------------
				// 양손에 있는게 다르다면..
				//-------------------------------------------------
				pOldItem = NULL;
				return false;
				
			}

			//-------------------------------------------------
			// 오른손에 뭔가 있는 경우
			//-------------------------------------------------
			// 오른손의 Item과 Replace한다.
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_SLAYER_RIGHTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_RIGHTHAND, 
													pOldItem );
			}		

			//-------------------------------------------------
			// 왼손에 뭔가 있는 경우
			//-------------------------------------------------
			// 왼손의 Item과 Replace한다.
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_SLAYER_LEFTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_LEFTHAND, 
													pOldItem );					
			}

			//-----------------------------------------------------
			// 양손의 Slot을 모두 설정..
			//-----------------------------------------------------
			if (bAdd)
			{
				//-------------------------------------------------
				// item의 내구성 상태 설정
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );
				
				pItem->SetItemSlot( GEAR_SLAYER_RIGHTHAND );
				m_ItemSlot[GEAR_SLAYER_LEFTHAND] = pItem;
				m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = pItem;

				//-------------------------------------------------
				// 제거된 것이 총인 경우
				//-------------------------------------------------
				if (pOldItem!=NULL && pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}

				//-------------------------------------------------
				// 총이면 현재 탄창 설정
				//-------------------------------------------------
				if (pItem->IsGunItem() && !m_bIsOtherGear)
				{
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
				}				
		
				//-------------------------------------------------
				// 제대로 추가된 경우 --> sound출력
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					PlaySound( pItem->GetGearSoundID() );
					g_pPlayer->CalculateStatus();
				#endif

				return true;
			}

			return false;
		}
		//-------------------------------------------------
		// [ 오른손 Item ] 인 경우
		//-------------------------------------------------		
		else if (pItem->IsGearSlotRightHand())
		{
			bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_RIGHTHAND, 
													pOldItem );

			//-------------------------------------------------
			// 교환되어 나온 Item이 양손인 경우
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					// 왼손을 비워둔다.
					m_ItemSlot[GEAR_SLAYER_LEFTHAND] = NULL;
				}

				//-------------------------------------------------
				// 총이면 현재 탄창 제거
				//-------------------------------------------------
				if (pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}
			}

			if (bAdd)
			{
				//-------------------------------------------------
				// item의 내구성 상태 설정
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_SLAYER_RIGHTHAND );

				//-------------------------------------------------
				// 제대로 추가된 경우 --> sound출력
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					PlaySound( pItem->GetGearSoundID() );
					g_pPlayer->CalculateStatus();
				#endif

				return true;
			}

			return false;
		}
		//-------------------------------------------------
		// [ 왼손 Item ] 인 경우
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_SLAYER_LEFTHAND, 
													pOldItem );
			//-------------------------------------------------
			// 교환되어 나온 Item이 양손인 경우
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					// 오른손을 비워둔다.
					m_ItemSlot[GEAR_SLAYER_RIGHTHAND] = NULL;
				}

				//-------------------------------------------------
				// 총이면 현재 탄창 제거
				//-------------------------------------------------
				if (pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}
			}

			if (bAdd)
			{
				//-------------------------------------------------
				// item 상태 설정
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_SLAYER_LEFTHAND );
				
				//-------------------------------------------------
				// 제대로 추가된 경우 --> sound출력
				//-------------------------------------------------
				#ifdef __GAME_CLIENT__
					PlaySound( pItem->GetGearSoundID() );
					g_pPlayer->CalculateStatus();
				#endif	

				return true;
			}

			return false;
		}
		
		//-------------------------------------------------
		// 손에 장착할려는데
		// 손에 장착할 수 있는 Item이 아닌 경우
		//-------------------------------------------------
		pOldItem = NULL;
		return false;
		
	}
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if (n >= GEAR_SLAYER_RING1 && n <= GEAR_SLAYER_RING4)	
	{
		if (pItem->IsGearSlotRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]!=NULL) // 코어잽이 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotCoreZap())
		{			
			if (m_ItemSlot[n]!=NULL) // 코어잽은 링이 있어야 놓을 수 있다. 
			{
				bool bAdd = false;
				if(m_ItemSlot[n+m_Gilles_CoreZap] == NULL)// 기존 코어잽이 없으면
				{
					pOldItem = NULL;
					bAdd = MPlayerGear::AddItem( pItem, n+m_Gilles_CoreZap );	// 걍 추가
				}
				else // 기존 코어잽이 있으면 교환
				{
					bAdd = MPlayerGear::ReplaceItem( pItem, n+m_Gilles_CoreZap, pOldItem );	
					
					if (bAdd)
					{
						CheckItemStatus( pItem, n+m_Gilles_CoreZap );
					}			
				}

				if (bAdd) // 잘 추가 됐으면 
				{
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();
					#endif
					return true;
				}
				else 
					return false;
			}
			return false;
		}
	
	}
//	else if(n >= GEAR_SLAYER_ZAP1 && n <= GEAR_SLAYER_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;
	
	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if (n == GEAR_SLAYER_NECKLACE )
	{
		if (pItem->IsGearSlotNecklace())
		{			
			if (m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]!=NULL) // 에테리얼 체인이 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotEtherealChain())
		{			
			if (m_ItemSlot[n]!=NULL) // 에테리얼 체인은 목걸이가 있어야 놓을 수 있다. 
			{
				bool bAdd = false;
				if(m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN] == NULL)// 기존 에테리얼 체인이 없으면
				{
					pOldItem = NULL;
					bAdd = MPlayerGear::AddItem( pItem, GEAR_SLAYER_ETHEREAL_CHAIN );	// 걍 추가
				}
				else // 기존 에테리얼 체인이 있으면 교환
				{
					bAdd = MPlayerGear::ReplaceItem( pItem, GEAR_SLAYER_ETHEREAL_CHAIN, pOldItem );	
					
					if (bAdd)
					{
						CheckItemStatus( pItem, GEAR_SLAYER_ETHEREAL_CHAIN );
					}			
				}

				if (bAdd) // 잘 추가 됐으면 
				{
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();
					#endif
					return true;
				}
				else 
					return false;
			}
			return false;
		}
	
	}
	// nanomech 2006.03.09 Item 수정 작업 end
	

	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if (n >= GEAR_SLAYER_BRACELET1 && n <= GEAR_SLAYER_BRACELET2)	
	{
		if (pItem->IsGearSlotBracelet())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]!=NULL) // 큐가 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotCue())
		{			
			if (m_ItemSlot[n]!=NULL) // 큐는 팔찌가 있어야 놓을 수 있다. 
			{
				bool bAdd = false;
				if(m_ItemSlot[n+s_Slot_Offset_Cue] == NULL)// 기존 큐가 없으면
				{
					pOldItem = NULL;
					bAdd = MPlayerGear::AddItem( pItem, n+s_Slot_Offset_Cue );	// 걍 추가
				}
				else // 기존 큐가 있으면 교환
				{
					bAdd = MPlayerGear::ReplaceItem( pItem, n+s_Slot_Offset_Cue, pOldItem );	
					
					if (bAdd)
					{
						CheckItemStatus( pItem, n+s_Slot_Offset_Cue );
					}			
				}

				if (bAdd) // 잘 추가 됐으면 
				{
					#ifdef __GAME_CLIENT__
						PlaySound( pItem->GetGearSoundID() );
						g_pPlayer->CalculateStatus();
					#endif
					return true;
				}
				else 
					return false;
			}
			return false;
		}
	
	}


	//----------------------------------------------------
	// 적절한 Slot인지 확인해야 한다.
	//----------------------------------------------------	
	if (pItem->IsGearItem()	&&				// pItem이 GearItem인지 확인..
		(*s_GearSlotCheckTable[n])(pItem))	// 적절한 Slot인지 확인
	{
		bool bAdd;
		//----------------------------------------------------
		// 다른 Item이 없는 경우 --> 그냥 추가
		//----------------------------------------------------	
		if (m_ItemSlot[n]==NULL)				// n번째 slot이 비어있는지 확인
		{
			pOldItem = NULL;
			// n번째 slot에 pItem을 추가한다.
			bAdd = MPlayerGear::AddItem( pItem, n );			
		}
		//----------------------------------------------------	
		// 교환 해야 되는 경우
		//----------------------------------------------------	
		else
		{
			//-----------------------------------------------------
			// n번째 slot의 Item과 교환한다.
			//-----------------------------------------------------
			bAdd = MPlayerGear::ReplaceItem( pItem, n, pOldItem );	
			
			//-------------------------------------------------
			// item의 내구성 상태 설정
			//-------------------------------------------------
			if (bAdd)
			{
				CheckItemStatus( pItem, n );
			}			
		}

		//--------------------------------------------
		// 제대로 넣었으면..
		//--------------------------------------------
		if (bAdd)
		{
			//--------------------------------------------
			// belt인 경우
			//--------------------------------------------
			// QuickSlot이 생긴 경우이다.
			//--------------------------------------------
			if (n==GEAR_SLAYER_BELT)
			{
				if (pItem->GetItemClass()==ITEM_CLASS_BELT && !m_bIsOtherGear)
				{
					g_pQuickSlot = (MBelt*)pItem;				
				}
				else
				{
					// 이런 경우는 있을 수 없지 싶다.					
				}
			}
			else
			{
				//-------------------------------------------------
				// 총을 제거한 경우
				// 위에서 손은 따로 체크하므로..
				// 음.. 가능성이 없는 코드당.. - -;;
				//-------------------------------------------------
				if (pOldItem!=NULL && pOldItem->IsGunItem())
				{
					g_pCurrentMagazine = NULL;
				}

				//-------------------------------------------------
				// 총이면 현재 탄창 설정
				//-------------------------------------------------
				if (pItem->IsGunItem() && !m_bIsOtherGear)
				{
					MGunItem* pGunItem = (MGunItem*)pItem;
					g_pCurrentMagazine = pGunItem->GetMagazine();
				}	
			}

			//-------------------------------------------------
			// 제대로 추가된 경우 --> sound출력
			//-------------------------------------------------
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetGearSoundID() );
				g_pPlayer->CalculateStatus();
			#endif

			return true;
		}
	}

	return false;
}

MItem*	MSlayerGear::GetGearItem_PickUp(int &n) const	
{
	int Slot = n;
//	if(Slot>=GEAR_SLAYER_BLOOD_BIBLE1 && Slot<=GEAR_SLAYER_BLOOD_BIBLE6) // 블러드 바이블은 집을수 없다.
//		return NULL;
	if(Slot>=GEAR_SLAYER_RING1 && Slot<=GEAR_SLAYER_RING4)
	{
		if(m_ItemSlot[Slot+m_Gilles_CoreZap]!=NULL)
		{
			n = n+m_Gilles_CoreZap;
			return MSlotItemManager::GetItem( (BYTE)(Slot+m_Gilles_CoreZap) ); 
		}
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	if( Slot==GEAR_SLAYER_NECKLACE )
	{
		if(m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]!=NULL)
		{
			n = GEAR_SLAYER_ETHEREAL_CHAIN;
			return MSlotItemManager::GetItem( (BYTE)(GEAR_SLAYER_ETHEREAL_CHAIN) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	if(Slot>=GEAR_SLAYER_BRACELET1 && Slot<=GEAR_SLAYER_BRACELET2)
	{
		if(m_ItemSlot[Slot+s_Slot_Offset_Cue]!=NULL)
		{
			n = n+s_Slot_Offset_Cue;
			return MSlotItemManager::GetItem( (BYTE)(Slot+s_Slot_Offset_Cue) ); 
		}
	}

	return MSlotItemManager::GetItem( (BYTE)Slot ); 
}


MItem*	MSlayerGear::GetGearCoreZapItem(int slot) const	
{
	if(slot>=GEAR_SLAYER_RING1 && slot<=GEAR_SLAYER_RING4)
	{
		if(m_ItemSlot[slot+m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot+m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_CORE_ZAP)
				return MSlotItemManager::GetItem( (BYTE)(slot+m_Gilles_CoreZap) ); 
		}
	}
	
	// nanomech 2006.03.09 Item 수정 작업 start
	if( slot==GEAR_SLAYER_NECKLACE )
	{
		if(m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]!=NULL)
		{
			if(m_ItemSlot[GEAR_SLAYER_ETHEREAL_CHAIN]->GetItemClass() == ITEM_CLASS_ETHEREAL_CHAIN)
				return MSlotItemManager::GetItem( (BYTE)(GEAR_SLAYER_ETHEREAL_CHAIN) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	if(slot>=GEAR_SLAYER_BRACELET1 && slot<=GEAR_SLAYER_BRACELET2)
	{
		if(m_ItemSlot[slot+s_Slot_Offset_Cue]!=NULL)
		{
			if(m_ItemSlot[slot+s_Slot_Offset_Cue]->GetItemClass() == ITEM_CLASS_CUE_OF_ADAM)
				return MSlotItemManager::GetItem( (BYTE)(slot+s_Slot_Offset_Cue) ); 
		}
	}
	
	return NULL;
}



MItem*	MSlayerGear::GetGearCoreZapedItem(int slot) const	
{
	if(slot>=GEAR_SLAYER_ZAP1 && slot<=GEAR_SLAYER_ZAP4)
	{
		if(slot-m_Gilles_CoreZap<0)
			return NULL;
		if(m_ItemSlot[slot-m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot-m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_RING)
				return MSlotItemManager::GetItem( (BYTE)(slot-m_Gilles_CoreZap) ); 
		}
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	if( slot==GEAR_SLAYER_ETHEREAL_CHAIN )
	{
		if(GEAR_SLAYER_NECKLACE<0)
			return NULL;
		if(m_ItemSlot[GEAR_SLAYER_NECKLACE]!=NULL)
		{
			if(m_ItemSlot[GEAR_SLAYER_NECKLACE]->GetItemClass() == ITEM_CLASS_NECKLACE)
				return MSlotItemManager::GetItem( (BYTE)(GEAR_SLAYER_NECKLACE) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	if(slot>=GEAR_SLAYER_CUE1 && slot<=GEAR_SLAYER_CUE2)
	{
		if(slot-s_Slot_Offset_Cue<0)
			return NULL;
		if(m_ItemSlot[slot-s_Slot_Offset_Cue]!=NULL)
		{
			if(m_ItemSlot[slot-s_Slot_Offset_Cue]->GetItemClass() == ITEM_CLASS_BRACELET)
				return MSlotItemManager::GetItem( (BYTE)(slot-s_Slot_Offset_Cue) ); 
		}
	}

	return NULL;
}

const bool	MSlayerGear::IsHasAllCoreZap(int CoreZapType) const	
{
	BYTE bType[4] = {0,};
	if(CoreZapType == 0) // 질드레 코어 잽 일 경우
	{
		MItem* pItem;
		for(int i = 0; i<4; i++)
		{
			pItem = m_ItemSlot[GEAR_SLAYER_ZAP1+i];
			if(pItem != NULL)
			{
				if(pItem->GetItemType()>3)
					return false;
				if(bType[pItem->GetItemType()]!= 0)
					return false;
				bType[pItem->GetItemType()] = 1;
			}
			else
				return false;
		}
		if(bType[0]+bType[1]+bType[2]+bType[3] == 4)
			return true;
	}
	
	return false;
}

const bool MSlayerGear::IsHasAllCue(OUT bool CueOption[]) const
{
	ZeroMemory(CueOption, sizeof(bool)*4);
	
	for(int i = 0; i<2; i++)
	{
		MItem*	pItem		= m_ItemSlot[GEAR_SLAYER_CUE1+i];
		if(pItem != NULL)
		{
			int		nItemType	= pItem->GetItemType();

			if(nItemType>3)
				return false;
			if(CueOption[pItem->GetItemType()] != false)
				return false;
			CueOption[pItem->GetItemType()] = true;
		}
		else
			return false;
	}
	if(CueOption[0]+CueOption[1]+CueOption[2]+CueOption[3] == 2)
		return true;
	
	return false;
}

const bool	MSlayerGear::IsCloseBloodBibleSlot(int slot) const	
{
	if(slot>=GEAR_SLAYER_BLOOD_BIBLE1 && slot<=GEAR_SLAYER_BLOOD_BIBLE6)
	{
			int TempSlot = slot - GEAR_SLAYER_BLOOD_BIBLE1 + 1;
		if(TempSlot > m_bBloodBibleOpenCount)
			return true;
	}
	return false;
}
