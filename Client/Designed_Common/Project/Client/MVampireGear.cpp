//----------------------------------------------------------------------
// MVampireGear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItem.h"
#include "MVampireGear.h"

#ifdef __GAME_CLIENT__
	#include "ClientFunction.h"
	#include "MPlayer.h"
#endif
#include "MTimeItemManager.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MVampireGear*		g_pVampireGear = NULL;
MVampireGear*		g_pVampireGearOther = NULL;

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
const int MVampireGear::s_Slot_Offset_Cue = MVampireGear::GEAR_VAMPIRE_CUE1 - MVampireGear::GEAR_VAMPIRE_BRACELET1;

MVampireGear::FUNCTION_GEARSLOT_CHECK

//nanomech 2006.03.08 Item수정 작업 순서 보정 및 갯수 조정
MVampireGear::s_GearSlotCheckTable[MAX_GEAR_VAMPIRE] =
{
	IsGearSlotNecklace,			// 목걸이
	IsGearSlotCoat,				// 상의
	IsGearSlotBracelet,			// 팔찌1
	IsGearSlotBracelet,			// 팔찌2
	IsGearSlotRing,				// 반지1
	IsGearSlotRing,				// 반지2
	IsGearSlotRing,				// 반지3
	IsGearSlotRing,				// 반지4
	IsGearSlotEarRing,			// 귀걸이1
	IsGearSlotEarRing,			// 귀걸이2
	IsGearSlotLeftHand,			// 왼손
	IsGearSlotRightHand,		// 오른손
	IsGearSlotAmulet,			// 부적1
	IsGearSlotAmulet,			// 부적2
	IsGearSlotAmulet,			// 부적3
	IsGearSlotAmulet,			// 부적4

	IsGearSlotCoreZap,			// 코어잽1
	IsGearSlotCoreZap,			// 코어잽2
	IsGearSlotCoreZap,			// 코어잽3
	IsGearSlotCoreZap,			// 코어잽4

	IsGearSlotDermis,			// 승직용 문신?
	IsGearSlotPersona,			// 승직용 페르소나

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
bool		MVampireGear::IsGearSlotNecklace(MItem* pItem) 	{ return pItem->IsGearSlotVampireNecklace(); }	// 목걸이
bool		MVampireGear::IsGearSlotCoat(MItem* pItem) 		{ return pItem->IsGearSlotVampireCoat(); }		// 상의
bool		MVampireGear::IsGearSlotBracelet(MItem* pItem) 	{ return pItem->IsGearSlotVampireBracelet(); }	// 팔찌
bool		MVampireGear::IsGearSlotRing(MItem* pItem) 		{ return pItem->IsGearSlotVampireRing(); }		// 반지
bool		MVampireGear::IsGearSlotEarRing(MItem* pItem) 	{ return pItem->IsGearSlotVampireEarRing(); }	// 귀걸이
bool		MVampireGear::IsGearSlotLeftHand(MItem* pItem) 	{ return pItem->IsGearSlotVampireLeftHand(); }	// 왼손
bool		MVampireGear::IsGearSlotRightHand(MItem* pItem) { return pItem->IsGearSlotVampireRightHand(); }	// 오른손
bool		MVampireGear::IsGearSlotAmulet(MItem* pItem)	{ return pItem->IsGearSlotVampireAmulet(); }	// 오른손
bool		MVampireGear::IsGearSlotCoreZap(MItem* pItem)	{ return pItem->IsGearSlotVampireCoreZap(); }	// 코어잽
bool		MVampireGear::IsGearSlotDermis(MItem* pItem)	{ return pItem->IsGearSlotVampireDermis(); }	// 문신
bool		MVampireGear::IsGearSlotPersona(MItem* pItem)	{ return pItem->IsGearSlotVampirePersona(); }	// 페르소
bool		MVampireGear::IsGearSlotBloodBible(MItem* pItem){ return pItem->IsGearSlotVampireBloodBible(); }	// 블러디 바이블
//nanomech 2006.03.08 Item수정 작업 start
bool		MVampireGear::IsGearSlotEtherealChain(MItem* pItem)	{ return pItem->IsGearSlotVampireEtherealChain(); }	// 목걸이 코어잽
//nanomech 2006.03.08 Item수정 작업 end
bool		MVampireGear::IsGearSlotCue(MItem* pItem)		{ return pItem->IsGearSlotVampireCue(); }	// 목걸이 코어잽

//----------------------------------------------------------------------
//
// constructor
// 
//----------------------------------------------------------------------
MVampireGear::MVampireGear()
{
	m_bBloodBibleOpenCount = 0;
}

MVampireGear::~MVampireGear()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init 
//----------------------------------------------------------------------
// MAX_GEAR_VAMPIRE만큼의 Slot을 생성한다.
//----------------------------------------------------------------------
void
MVampireGear::Init()
{
	// slot 생성
	MPlayerGear::Init(MAX_GEAR_VAMPIRE);
	//nanomech 2006.03.08 Item수정 작업 start
	m_Gilles_CoreZap = GEAR_VAMPIRE::GEAR_VAMPIRE_ZAP1 - GEAR_VAMPIRE::GEAR_VAMPIRE_RING1;
	//m_Gilles_CoreZap = 12;
	//nanomech 2006.03.08 Item수정 작업 end
	m_bBloodBibleOpenCount = 0;
}

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------
// 특정 아이템의 착용 가능 여부를 체크한다.
//----------------------------------------------------------------------
void			
MVampireGear::CheckAffectStatus(MItem* pItem)
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
MVampireGear::AddItem(MItem* pItem)
{
	for (int i=0; i<MAX_GEAR_VAMPIRE; i++)
	{
		// 추가했으면..
		if (AddItem( pItem, (enum GEAR_VAMPIRE)i ))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Can Replace Item ( pItem, n )
//----------------------------------------------------------------------
// slot(n)에 pItem을 추가한다.
// 추가할 수 있는 slot인지 확인해야 한다.
//
// pOldItem에는 교체될 것이 있는 경우에 그게 들어간다.
//----------------------------------------------------------------------
bool			
MVampireGear::CanReplaceItem(MItem* pItem, GEAR_VAMPIRE n, MItem*& pOldItem)
{	
	// 수치 적용되지 않는 아이템은 착용되지 않는다
	if(pItem->IsAffectStatus() == false&& !pItem->IsQuestItem())
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
	if (n>=m_Size || !pItem->IsVampireItem())
	{
		return false;
	}

	if(n>=GEAR_VAMPIRE_BLOOD_BIBLE1 && n<=GEAR_VAMPIRE_BLOOD_BIBLE6) // 블러드 바이블은 집을수 없다.
		return false;
	//-------------------------------------------------
	//
	// 손에 장착할려는 경우
	//
	//-------------------------------------------------
	if (n==GEAR_VAMPIRE_LEFTHAND || n==GEAR_VAMPIRE_RIGHTHAND)	
	{
		//-------------------------------------------------
		// [ 양손 Item ] 인 경우
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			// 양손의 Slot이 모두 비어있는 경우 
			//-------------------------------------------------			
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]==NULL)
			{
				return true;
			}
			
			//-------------------------------------------------
			// 양손에 다 뭔가 있는 경우
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				// 양손에 있는게 같다면..
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==m_ItemSlot[GEAR_VAMPIRE_LEFTHAND])
				{
					pOldItem = m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND];
					return true;
				}
				
				return false;				
			}

			//-------------------------------------------------
			// 오른손에만 뭔가 있는 경우
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL)
			{
				pOldItem = m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND];
				return true;
			}

			//-------------------------------------------------
			// 왼손에만뭔가 있는 경우
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{			
				pOldItem = m_ItemSlot[GEAR_VAMPIRE_LEFTHAND];
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
			pOldItem = m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND];
			return true;
		}
		
		//-------------------------------------------------				
		// [ 왼손 Item ] 인 경우
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			pOldItem = m_ItemSlot[GEAR_VAMPIRE_LEFTHAND];
			return true;
		}

		// 손에 장착하는게 아닌 경우
		return false;
	}
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if (n >= GEAR_VAMPIRE_RING1 && n <= GEAR_VAMPIRE_RING4)	
	{
		if (pItem->IsGearSlotVampireRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL) // 코어잽이 없어야 링을 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotVampireCoreZap())
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
		return false;
	}
	else if(n >= GEAR_VAMPIRE_ZAP1 && n <= GEAR_VAMPIRE_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
		return false;

	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if ( n == GEAR_VAMPIRE_NECKLACE )
	{
		if (pItem->IsGearSlotVampireNecklace())
		{			
			if (m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]==NULL) // 에테리얼 체인이 없어야 목거리를 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotVampireEtherealChain())
		{			
			if (m_ItemSlot[n]!=NULL) // 에테리얼 체인은 목걸이가 있어야 놓을 수 있다. 
			{
				if(m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]!=NULL)
					pOldItem = m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN];
				else
					pOldItem = NULL;
				return true;
			}
		}
		return false;
	}
	else if( n == GEAR_VAMPIRE_ETHEREAL_CHAIN )
		return false;
	// nanomech 2006.03.09 Item 수정 작업 end


	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if (n >= GEAR_VAMPIRE_BRACELET1 && n <= GEAR_VAMPIRE_BRACELET2)	
	{
		if (pItem->IsGearSlotVampireBracelet())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]==NULL) // 큐가 없어야 팔찌를 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotVampireCue())
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
		return false;
	}
	else if(n >= GEAR_VAMPIRE_CUE1 && n <= GEAR_VAMPIRE_CUE2) // 큐 슬롯은 보이지 않는다. -_-?? 
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
		// n번째 slot에 pItem을 추가한다.
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
MVampireGear::AddItem(MItem* pItem, GEAR_VAMPIRE n)
{	
	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsVampireItem())
	{
		return false;
	}

	//-------------------------------------------------
	// 양손 Item인 경우는 따로 체크해준다.
	//-------------------------------------------------
	if ((n==GEAR_VAMPIRE_LEFTHAND || n==GEAR_VAMPIRE_RIGHTHAND) &&
		pItem->IsGearSlotTwoHand())
	{
		// 양손의 Slot이 모두 비어있는 상태에만 추가 가능하다.
		if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==NULL
				&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]==NULL)
		{
			// 오른손에 추가한다.			
			if (MPlayerGear::AddItem( pItem, GEAR_VAMPIRE_RIGHTHAND ))
			{
				// 왼손에도 추가해준다.
				m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;

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

		return false;
	}
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if (n >= GEAR_VAMPIRE_RING1 && n <= GEAR_VAMPIRE_RING4)	
	{
		if (pItem->IsGearSlotVampireRing())
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
		if (pItem->IsGearSlotVampireCoreZap())
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
	if (n == GEAR_VAMPIRE_NECKLACE )
	{
		if (pItem->IsGearSlotVampireNecklace())
		{			
			if (m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]==NULL&&m_ItemSlot[n]==NULL) // 목걸이도 없고 에테리얼 체인이 없어야 목걸이를 놓을 수 있음
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
		if (pItem->IsGearSlotVampireEtherealChain())
		{
			if (m_ItemSlot[GEAR_VAMPIRE_NECKLACE]!=NULL && m_ItemSlot[n]==NULL) // 해당위치에 목걸이가 있고 에테리얼 체인이 없을 경우만 에테리얼 체인 추가
			{		
				if (MPlayerGear::AddItem( pItem, GEAR_VAMPIRE_ETHEREAL_CHAIN ))// 에테리얼 체인 위치에 추가
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
	if (n >= GEAR_VAMPIRE_BRACELET1 && n <= GEAR_VAMPIRE_BRACELET2)	
	{
		if (pItem->IsGearSlotVampireBracelet())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]==NULL&&m_ItemSlot[n]==NULL) // 팔찌도 없고 큐도 없어야 큐를 놓을 수 있음
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
		if (pItem->IsGearSlotVampireCue())
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


//	else if(n >= GEAR_VAMPIRE_ZAP1 && n <= GEAR_VAMPIRE_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;
	//----------------------------------------------------
	// 적절한 Slot인지 확인해야 한다.
	//----------------------------------------------------	
	if (m_ItemSlot[n]==NULL &&				// n번째 slot이 비어있는지 확인
		pItem->IsGearItem()	&&				// pItem이 GearItem인지 확인..
		(*s_GearSlotCheckTable[n])(pItem))	// 적절한 Slot인지 확인
	{
		// slot만 맞으면 된다.
		// n번째 slot에 pItem을 추가한다.
		bool bAdd = MPlayerGear::AddItem( pItem, n );

		//-------------------------------------------------
		// 제대로 추가된 경우 --> sound출력
		//-------------------------------------------------
		if (bAdd)
		{
			#ifdef __GAME_CLIENT__		
				PlaySound( pItem->GetGearSoundID() );
				g_pPlayer->CalculateStatus();
			#endif

			return true;
		}		

		return false;
	}

	return false;
}

//----------------------------------------------------------------------
// Remove Item ( n)
//----------------------------------------------------------------------
MItem*			
MVampireGear::RemoveItem(GEAR_VAMPIRE n)
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
		n = GEAR_VAMPIRE_RIGHTHAND;
	}
	
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if(n>=GEAR_VAMPIRE_RING1 && n<= GEAR_VAMPIRE_RING4) 
	{
		if(NULL != m_ItemSlot[n+m_Gilles_CoreZap]) // 코어잽이 붙어 있으면 코어잽을 뗀다
			n = (GEAR_VAMPIRE)(n + m_Gilles_CoreZap); // 코어잽 위치
	}
//	else if(n >= GEAR_VAMPIRE_ZAP1 && n <= GEAR_VAMPIRE_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;

	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if( n==GEAR_VAMPIRE_NECKLACE )
	{
		if(NULL != m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]) // 에테리얼 체인이 붙어 있으면 에테리얼 체인을 뗀다
			n = (GEAR_VAMPIRE)(GEAR_VAMPIRE_ETHEREAL_CHAIN); // 에테리얼 체인 위치
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if(n>=GEAR_VAMPIRE_BRACELET1 && n<= GEAR_VAMPIRE_BRACELET2) 
	{
		if(NULL != m_ItemSlot[n+s_Slot_Offset_Cue])		// 큐가 붙어 있으면 큐를 뗀다
			n = (GEAR_VAMPIRE)(n + s_Slot_Offset_Cue);	// 큐 위치
	}

	pItem = MPlayerGear::RemoveItem( (BYTE)n );

	//-----------------------------------------------------
	// 없는 경우
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//-----------------------------------------------------
	// 양손 Item인 경우 양손의 Slot을 모두 제거해줘야 한다.
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = NULL;
	}
	
	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	// 있으면...
	return pItem;
}

//----------------------------------------------------------------------
// RemoveItem : id가 같은 item을 제거한다.
//----------------------------------------------------------------------
MItem*			
MVampireGear::RemoveItem(TYPE_OBJECTID id)
{ 
	MItem* pItem = MPlayerGear::RemoveItem( id ); 

	//-----------------------------------------------------
	// 없는 경우
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//-----------------------------------------------------
	// 양손 Item인 경우 양손의 Slot을 모두 제거해줘야 한다.
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = NULL;
	}


	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	// 있으면...
	return pItem;
}

//----------------------------------------------------------------------
// ReplaceItem : pItem을 추가하고 딴게 있다면 Item교환
//----------------------------------------------------------------------
bool			
MVampireGear::ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsVampireItem())
	{
		return false;
	}

	//-------------------------------------------------
	//
	// 손에 장착할려는 경우
	//
	//-------------------------------------------------
	if (n==GEAR_VAMPIRE_LEFTHAND || n==GEAR_VAMPIRE_RIGHTHAND)	
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
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]==NULL)
			{
				// 그냥 추가한다.
				pOldItem = NULL;
				if (MPlayerGear::AddItem( pItem, GEAR_VAMPIRE_RIGHTHAND ))
				{
					//-------------------------------------------------
					// item의 내구성 상태 설정
					//-------------------------------------------------
					CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );

					pItem->SetItemSlot( GEAR_VAMPIRE_RIGHTHAND );
					m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;
					m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = pItem;

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
			if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				// 양손에 있는게 같다면..
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]==m_ItemSlot[GEAR_VAMPIRE_LEFTHAND])
				{
					// 양손 무기와 교체하는 경우이다.
					if (MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_RIGHTHAND, 
													pOldItem ))
					{
						//-------------------------------------------------
						// item의 내구성 상태 설정
						//-------------------------------------------------
						CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );

						//pItem->SetItemSlot( GEAR_VAMPIRE_RIGHTHAND );
						m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;
						//m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = pItem;

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
			else if (m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_RIGHTHAND, 
													pOldItem );
			}		

			//-------------------------------------------------
			// 왼손에 뭔가 있는 경우
			//-------------------------------------------------
			// 왼손의 Item과 Replace한다.
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_VAMPIRE_LEFTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_VAMPIRE_LEFTHAND, 
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
				CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );
				
				pItem->SetItemSlot( GEAR_VAMPIRE_RIGHTHAND );
				m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = pItem;
				m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = pItem;
		
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
													GEAR_VAMPIRE_RIGHTHAND, 
													pOldItem );

			//-------------------------------------------------
			// 교환되어 나온 Item이 양손인 경우
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					// 왼손을 비워둔다.
					m_ItemSlot[GEAR_VAMPIRE_LEFTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				// item의 내구성 상태 설정
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_VAMPIRE_RIGHTHAND );

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
													GEAR_VAMPIRE_LEFTHAND, 
													pOldItem );
			//-------------------------------------------------
			// 교환되어 나온 Item이 양손인 경우
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					// 오른손을 비워둔다.
					m_ItemSlot[GEAR_VAMPIRE_RIGHTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				// item 상태 설정
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_VAMPIRE_LEFTHAND );
				
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
	if (n >= GEAR_VAMPIRE_RING1 && n <= GEAR_VAMPIRE_RING4)	
	{
		if (pItem->IsGearSlotVampireRing())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]!=NULL) // 코어잽이 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotVampireCoreZap())
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
//	else if(n >= GEAR_VAMPIRE_ZAP1 && n <= GEAR_VAMPIRE_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;
	
	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if (n == GEAR_VAMPIRE_NECKLACE )
	{
		if (pItem->IsGearSlotVampireNecklace())
		{			
			if (m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]!=NULL) // 에테리얼 체인이 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotVampireEtherealChain())
		{			
			if (m_ItemSlot[n]!=NULL) // 에테리얼 체인은 링이 있어야 놓을 수 있다. 
			{
				bool bAdd = false;
				if(m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN] == NULL)// 기존 에테리얼 체인이 없으면
				{
					pOldItem = NULL;
					bAdd = MPlayerGear::AddItem( pItem, GEAR_VAMPIRE_ETHEREAL_CHAIN );	// 걍 추가
				}
				else // 기존 에테리얼 체인이 있으면 교환
				{
					bAdd = MPlayerGear::ReplaceItem( pItem, GEAR_VAMPIRE_ETHEREAL_CHAIN, pOldItem );	
					
					if (bAdd)
					{
						CheckItemStatus( pItem, GEAR_VAMPIRE_ETHEREAL_CHAIN );
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
	if (n >= GEAR_VAMPIRE_BRACELET1 && n <= GEAR_VAMPIRE_BRACELET2)	
	{
		if (pItem->IsGearSlotVampireBracelet())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]!=NULL) // 큐가 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotVampireCue())
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
	if (//m_ItemSlot[n]==NULL &&				// n번째 slot이 비어있는지 확인
		pItem->IsGearItem()	&&				// pItem이 GearItem인지 확인..
		(*s_GearSlotCheckTable[n])(pItem))	// 적절한 Slot인지 확인
	{
		// n번째 slot에 pItem을 추가한다.
		if (MPlayerGear::ReplaceItem( pItem, n, pOldItem ))
		{
			//-------------------------------------------------
			// item 상태 설정
			//-------------------------------------------------
			CheckItemStatus( pItem, n );			

			//-------------------------------------------------
			// 제대로 추가된 경우 --> sound출력
			//-------------------------------------------------
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetGearSoundID() );
			#endif

			#ifdef __GAME_CLIENT__
				g_pPlayer->CalculateStatus();
			#endif

			return true;
		}

		return false;
	}

	return false;
}


MItem*	MVampireGear::GetGearItem_PickUp(int &n) const	
{
	int Slot = n;
//	if(Slot>=GEAR_VAMPIRE_BLOOD_BIBLE1 && Slot<=GEAR_VAMPIRE_BLOOD_BIBLE6) // 블러드 바이블은 집을수 없다.
//		return NULL;
	if(Slot>=GEAR_VAMPIRE_RING1 && Slot<=GEAR_VAMPIRE_RING4)
	{
		if(m_ItemSlot[Slot+m_Gilles_CoreZap]!=NULL)
		{
			n = n+m_Gilles_CoreZap;
			return MSlotItemManager::GetItem( (BYTE)(Slot+m_Gilles_CoreZap) ); 
		}
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	if( Slot==GEAR_VAMPIRE_NECKLACE )
	{
		if(m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]!=NULL)
		{
			n = GEAR_VAMPIRE_ETHEREAL_CHAIN;
			return MSlotItemManager::GetItem( (BYTE)(GEAR_VAMPIRE_ETHEREAL_CHAIN) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end


	if(Slot>=GEAR_VAMPIRE_BRACELET1 && Slot<=GEAR_VAMPIRE_BRACELET2)
	{
		if(m_ItemSlot[Slot+s_Slot_Offset_Cue]!=NULL)
		{
			n = n+s_Slot_Offset_Cue;
			return MSlotItemManager::GetItem( (BYTE)(Slot+s_Slot_Offset_Cue) ); 
		}
	}
	
	return MSlotItemManager::GetItem( (BYTE)Slot ); 
}



MItem*	MVampireGear::GetGearCoreZapItem(int slot) const	
{
	if(slot>=GEAR_VAMPIRE_RING1 && slot<=GEAR_VAMPIRE_RING4)
	{
		if(m_ItemSlot[slot+m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot+m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_CORE_ZAP)
				return MSlotItemManager::GetItem( (BYTE)(slot+m_Gilles_CoreZap) ); 
		}
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	if( slot==GEAR_VAMPIRE_NECKLACE )
	{
		if(m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]!=NULL)
		{
			if(m_ItemSlot[GEAR_VAMPIRE_ETHEREAL_CHAIN]->GetItemClass() == ITEM_CLASS_ETHEREAL_CHAIN)
				return MSlotItemManager::GetItem( (BYTE)(GEAR_VAMPIRE_ETHEREAL_CHAIN) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	if(slot>=GEAR_VAMPIRE_BRACELET1 && slot<=GEAR_VAMPIRE_BRACELET2)
	{
		if(m_ItemSlot[slot+s_Slot_Offset_Cue]!=NULL)
		{
			if(m_ItemSlot[slot+s_Slot_Offset_Cue]->GetItemClass() == ITEM_CLASS_CUE_OF_ADAM)
				return MSlotItemManager::GetItem( (BYTE)(slot+s_Slot_Offset_Cue) ); 
		}
	}
	
	return NULL;
}


MItem*	MVampireGear::GetGearCoreZapedItem(int slot) const	
{
	if(slot>=GEAR_VAMPIRE_ZAP1 && slot<=GEAR_VAMPIRE_ZAP4)
	{
		if(slot-m_Gilles_CoreZap<0)
			return NULL;
		if(m_ItemSlot[slot-m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot-m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_VAMPIRE_RING)
				return MSlotItemManager::GetItem( (BYTE)(slot-m_Gilles_CoreZap) ); 
		}
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	if( slot==GEAR_VAMPIRE_ETHEREAL_CHAIN )
	{
		if(GEAR_VAMPIRE_NECKLACE<0)
			return NULL;
		if(m_ItemSlot[GEAR_VAMPIRE_NECKLACE]!=NULL)
		{
			if(m_ItemSlot[GEAR_VAMPIRE_NECKLACE]->GetItemClass() == ITEM_CLASS_VAMPIRE_NECKLACE)
				return MSlotItemManager::GetItem( (BYTE)(GEAR_VAMPIRE_NECKLACE) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end


	if(slot>=GEAR_VAMPIRE_CUE1 && slot<=GEAR_VAMPIRE_CUE2)
	{
		if(slot-s_Slot_Offset_Cue<0)
			return NULL;
		if(m_ItemSlot[slot-s_Slot_Offset_Cue]!=NULL)
		{
			if(m_ItemSlot[slot-s_Slot_Offset_Cue]->GetItemClass() == ITEM_CLASS_VAMPIRE_BRACELET)
				return MSlotItemManager::GetItem( (BYTE)(slot-s_Slot_Offset_Cue) ); 
		}
	}
	
	return NULL;
}

const bool	MVampireGear::IsHasAllCoreZap(int CoreZapType) const	
{
	BYTE bType[4] = {0,};
	if(CoreZapType == 0) // 질드레 코어 잽 일 경우
	{
		MItem* pItem;
		for(int i = 0; i<4; i++)
		{
			pItem = m_ItemSlot[GEAR_VAMPIRE_ZAP1+i];
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

const bool MVampireGear::IsHasAllCue(OUT bool CueOption[]) const
{
	ZeroMemory(CueOption, sizeof(bool)*4);
	
	for(int i = 0; i<2; i++)
	{
		MItem*	pItem		= m_ItemSlot[GEAR_VAMPIRE_CUE1+i];
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

const bool	MVampireGear::IsCloseBloodBibleSlot(int slot) const	
{
	if(slot>=GEAR_VAMPIRE_BLOOD_BIBLE1 && slot<=GEAR_VAMPIRE_BLOOD_BIBLE6)
	{
		int TempSlot = slot - GEAR_VAMPIRE_BLOOD_BIBLE1 + 1;
		if(TempSlot > m_bBloodBibleOpenCount)
			return true;
	}
	return false;
}
