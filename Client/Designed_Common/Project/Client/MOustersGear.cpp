//----------------------------------------------------------------------
// MOustersGear.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItem.h"
#include "MOustersGear.h"
#include "MQuickSlot.h"

#ifdef __GAME_CLIENT__
	#include "UIFunction.h"
	#include "ClientFunction.h"
	#include "MPlayer.h"
#endif
#include "MTimeItemManager.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MOustersGear*		g_pOustersGear = NULL;
MOustersGear*		g_pOustersGearOther = NULL;

//----------------------------------------------------------------------
// static
//----------------------------------------------------------------------
const int MOustersGear::s_Slot_Offset_Cue = MOustersGear::GEAR_OUSTERS_CUE1 - MOustersGear::GEAR_OUSTERS_RING1;

MOustersGear::FUNCTION_GEARSLOT_CHECK

//nanomech 2006.03.08 Item수정 작업 갯수 보정 및 순서 교정
MOustersGear::s_GearSlotCheckTable[MAX_GEAR_OUSTERS] =
{
	IsGearSlotCirclet,			// 서클릿
	IsGearSlotCoat,				// 옷
	IsGearSlotLeftHand,			// 왼손
	IsGearSlotRightHand,		// 오른손
	IsGearSlotBoots,			// 신발
	IsGearSlotArmsBand,			// 암스밴드1
	IsGearSlotArmsBand,			// 암스밴드2
	IsGearSlotRing,				// 링1
	IsGearSlotRing,				// 링2
	IsGearSlotPendent,			// 목걸이1
	IsGearSlotPendent,			// 목걸이2
	IsGearSlotPendent,			// 목걸이3
	IsGearSlotStone,			// 정령석1
	IsGearSlotStone,			// 정령석2
	IsGearSlotStone,			// 정령석3
	IsGearSlotStone,			// 정령석4

	IsGearSlotCoreZap,			// 코어잽1
	IsGearSlotCoreZap,			// 코어잽2
	IsGearSlotCoreZap,			// 코어잽3
	IsGearSlotCoreZap,			// 코어잽4
	
	IsGearSlotFascia,			// 허리장식
	IsGearSlotMitten,			// 장갑

	IsGearSlotEtherealChain,	// 목걸이코어잽
	
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
bool		MOustersGear::IsGearSlotCirclet(MItem* pItem)		{ return pItem->IsGearSlotOustersCirclet(); }
bool		MOustersGear::IsGearSlotCoat(MItem* pItem)			{ return pItem->IsGearSlotOustersCoat(); }
bool		MOustersGear::IsGearSlotLeftHand(MItem* pItem)		{ return pItem->IsGearSlotOustersLeftHand(); }
bool		MOustersGear::IsGearSlotRightHand(MItem* pItem)		{ return pItem->IsGearSlotOustersRightHand(); }
bool		MOustersGear::IsGearSlotBoots(MItem* pItem)			{ return pItem->IsGearSlotOustersBoots(); }
bool		MOustersGear::IsGearSlotArmsBand(MItem* pItem)		{ return pItem->IsGearSlotOustersArmsBand(); }
bool		MOustersGear::IsGearSlotRing(MItem* pItem)			{ return pItem->IsGearSlotOustersRing(); }
bool		MOustersGear::IsGearSlotPendent(MItem* pItem)		{ return pItem->IsGearSlotOustersPendent(); }
bool		MOustersGear::IsGearSlotStone(MItem* pItem)			{ return pItem->IsGearSlotOustersStone(); }
bool		MOustersGear::IsGearSlotCoreZap(MItem* pItem)		{ return pItem->IsGearSlotOustersCoreZap(); }		// 코어잽
bool		MOustersGear::IsGearSlotFascia(MItem* pItem)		{ return pItem->IsGearSlotOustersFascia(); }
bool		MOustersGear::IsGearSlotMitten(MItem* pItem)		{ return pItem->IsGearSlotOustersMitten(); }
bool		MOustersGear::IsGearSlotBloodBible(MItem* pItem)	{ return pItem->IsGearSlotOustersBloodBible(); }	// 블러드 바이블
//nanomech 2006.03.08 Item수정 작업 start
bool		MOustersGear::IsGearSlotEtherealChain(MItem* pItem)	{ return pItem->IsGearSlotOustersEtherealChain(); }	// 목걸이 코어잽
//nanomech 2006.03.08 Item수정 작업 end
bool		MOustersGear::IsGearSlotCue(MItem* pItem)			{ return pItem->IsGearSlotOustersCue(); }			// 큐

//----------------------------------------------------------------------
//
// constructor
// 
//----------------------------------------------------------------------
MOustersGear::MOustersGear()
{
	g_pArmsBand1 = NULL;
	g_pArmsBand2 = NULL;
	m_bBloodBibleOpenCount = 0;
}

MOustersGear::~MOustersGear()
{
	g_pArmsBand1 = NULL;
	g_pArmsBand2 = NULL;
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init 
//----------------------------------------------------------------------
// MAX_GEAR_OUSTERS만큼의 Slot을 생성한다.
//----------------------------------------------------------------------
void
MOustersGear::Init()
{
	// slot 생성
	MPlayerGear::Init(MAX_GEAR_OUSTERS);
	//nanomech 2006.03.08 Item수정 작업 start
	m_Gilles_CoreZap = GEAR_OUSTERS::GEAR_OUSTERS_ZAP1 - GEAR_OUSTERS::GEAR_OUSTERS_STONE1;
	//m_Gilles_CoreZap = 4;
	//nanomech 2006.03.08 Item수정 작업 end
	m_bBloodBibleOpenCount = 0;
}

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------
// 특정 아이템의 착용 가능 여부를 체크한다.
//----------------------------------------------------------------------
void			
MOustersGear::CheckAffectStatus(MItem* pItem)
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
MOustersGear::AddItem(MItem* pItem)
{
	for (int i=0; i<MAX_GEAR_OUSTERS; i++)
	{
		// 추가했으면..
		if (AddItem( pItem, (enum GEAR_OUSTERS)i ))
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
MOustersGear::CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
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
#endif //__LIMITED_ITEM_UNISEX
	pOldItem = NULL;

	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsOustersItem())
	{
		return false;
	}
	if(n>=GEAR_OUSTERS_BLOOD_BIBLE1 && n<=GEAR_OUSTERS_BLOOD_BIBLE6) // 블러드 바이블은 집을수 없다.
		return false;

	
		
	// bycsm 2004.12.31

	/*
	if(n == GEAR_OUSTERS_FASCIA)
	{
		if(pItem->IsGearSlotOustersFascia() == true)
		{
			if (m_ItemSlot[GEAR_OUSTERS_FASCIA]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_OUSTERS_FASCIA];
				return true;
			}
		}

	}

	if(n == GEAR_OUSTERS_MITTEN)
	{
		if(pItem->IsGearSlotOustersMitten()== true)
		{
			
			if (m_ItemSlot[GEAR_OUSTERS_MITTEN]==NULL)
				return true;
			else
			{
				pOldItem = m_ItemSlot[GEAR_OUSTERS_MITTEN];
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
	if (n==GEAR_OUSTERS_LEFTHAND || n==GEAR_OUSTERS_RIGHTHAND)	
	{
		//-------------------------------------------------
		// [ 양손 Item ] 인 경우
		//-------------------------------------------------
		if (pItem->IsGearSlotTwoHand())
		{			
			//-------------------------------------------------
			// 양손의 Slot이 모두 비어있는 경우 
			//-------------------------------------------------			
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]==NULL)
			{
				return true;
			}
			
			//-------------------------------------------------
			// 양손에 다 뭔가 있는 경우
			//-------------------------------------------------
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				// 양손에 있는게 같다면..
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==m_ItemSlot[GEAR_OUSTERS_LEFTHAND])
				{
					pOldItem = m_ItemSlot[GEAR_OUSTERS_RIGHTHAND];
					return true;
				}
				
				return false;				
			}

			//-------------------------------------------------
			// 오른손에만 뭔가 있는 경우
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL)
			{
				pOldItem = m_ItemSlot[GEAR_OUSTERS_RIGHTHAND];
				return true;
			}

			//-------------------------------------------------
			// 왼손에만뭔가 있는 경우
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{			
				pOldItem = m_ItemSlot[GEAR_OUSTERS_LEFTHAND];
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
			pOldItem = m_ItemSlot[GEAR_OUSTERS_RIGHTHAND];
			return true;
		}
		
		//-------------------------------------------------				
		// [ 왼손 Item ] 인 경우
		//-------------------------------------------------
		else if (pItem->IsGearSlotLeftHand())
		{
			pOldItem = m_ItemSlot[GEAR_OUSTERS_LEFTHAND];
			return true;
		}

		// 손에 장착하는게 아닌 경우
		return false;
	}

	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if (n >= GEAR_OUSTERS_STONE1 && n <= GEAR_OUSTERS_STONE4)	
	{
		if (pItem->IsGearSlotOustersStone())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL) // 코어잽이 없어야 정령석을 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotOustersCoreZap())
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
	else if(n >= GEAR_OUSTERS_ZAP1 && n <= GEAR_OUSTERS_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
		return false;


	// nanomech 2006.03.08 Item 수정 작업 start
	//----------------------------------------------------
	// 에레리얼 체인
	//----------------------------------------------------
	if (n == GEAR_OUSTERS_PENDENT2 )
	{
		if (pItem->IsGearSlotOustersPendent())
		{			
			if (m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]==NULL) // 에레리얼 체인이 없어야 목걸이를 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotOustersEtherealChain())
		{			
			if (m_ItemSlot[n]!=NULL) // 에레리얼 체인은 목걸이가 있어야 놓을 수 있다. 
			{
				if(m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]!=NULL)
					pOldItem = m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN];
				else
					pOldItem = NULL;
				return true;
			}
			
		}
		return false;
	}
	else if(n == GEAR_OUSTERS_ETHEREALCHAIN )// 에레리얼 체인 슬롯은 보이지 않는다.
		return false;
	// nanomech 2006.03.08 Item 수정 작업 end


	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if (n >= GEAR_OUSTERS_RING1 && n <= GEAR_OUSTERS_RING2)	
	{
		if (pItem->IsGearSlotOustersRing())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]==NULL) // 큐가 없어야 링을 놓을 수 있음
			{
				pOldItem = m_ItemSlot[n];
				return true;
			}
		}
		else
		if (pItem->IsGearSlotOustersCue())
		{			
			if (m_ItemSlot[n]!=NULL) // 큐는 링이 있어야 놓을 수 있다. 
			{
				if (m_ItemSlot[n]!=NULL) // 큐는 링이 있어야 놓을 수 있다. 
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
	else if(n >= GEAR_OUSTERS_CUE1 && n <= GEAR_OUSTERS_CUE2) // 큐 슬롯은 보이지 않는다. -_-?? 
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
MOustersGear::AddItem(MItem* pItem, GEAR_OUSTERS n)
{	
	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------	
	if (n>=m_Size || !pItem->IsOustersItem())
	{
		return false;
	}

	//-------------------------------------------------
	// 양손 Item인 경우는 따로 체크해준다.
	//-------------------------------------------------
	if ((n==GEAR_OUSTERS_LEFTHAND || n==GEAR_OUSTERS_RIGHTHAND) &&
		pItem->IsGearSlotTwoHand())
	{
		// 양손의 Slot이 모두 비어있는 상태에만 추가 가능하다.
		if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==NULL
				&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]==NULL)
		{
			// 오른손에 추가한다.			
			if (MPlayerGear::AddItem( pItem, GEAR_OUSTERS_RIGHTHAND ))
			{
				// 왼손에도 추가해준다.
				m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;

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
	if (n >= GEAR_OUSTERS_STONE1 && n <= GEAR_OUSTERS_STONE4)	
	{
		if (pItem->IsGearSlotOustersStone())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]==NULL&&m_ItemSlot[n]==NULL) // 정령석도 없고 코어잽이 없어야 링을 놓을 수 있음
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
		if (pItem->IsGearSlotOustersCoreZap())
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
	if (n == GEAR_OUSTERS_PENDENT2 )
	{
		if (pItem->IsGearSlotOustersPendent())
		{			
			if (m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]==NULL&&m_ItemSlot[n]==NULL)
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
		if (pItem->IsGearSlotOustersEtherealChain())
		{
			if (m_ItemSlot[GEAR_OUSTERS_PENDENT2]!=NULL && m_ItemSlot[n]==NULL) // 해당위치에 링이 있고 코어잽이 없을 경우만 코어잽 추가
			{		
				if (MPlayerGear::AddItem( pItem, GEAR_OUSTERS_ETHEREALCHAIN ))// 코어잽 위치에 추가
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
	if (n >= GEAR_OUSTERS_RING1 && n <= GEAR_OUSTERS_RING2)	
	{
		if (pItem->IsGearSlotOustersRing())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]==NULL&&m_ItemSlot[n]==NULL) // 정령석도 없고 코어잽이 없어야 링을 놓을 수 있음
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
		if (pItem->IsGearSlotOustersCue())
		{
			if (m_ItemSlot[n-s_Slot_Offset_Cue]!=NULL && m_ItemSlot[n]==NULL) // 해당위치에 링이 있고 코어잽이 없을 경우만 코어잽 추가
			{		
				if (MPlayerGear::AddItem( pItem, n + s_Slot_Offset_Cue ))// 코어잽 위치에 추가
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


//	else if(n >= GEAR_OUSTERS_ZAP1 && n <= GEAR_OUSTERS_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;
	//----------------------------------------------------
	// 적절한 Slot인지 확인해야 한다.
	//----------------------------------------------------	
	if (m_ItemSlot[n]==NULL &&				// n번째 slot이 비어있는지 확인
		pItem->IsGearItem()	&&				// pItem이 GearItem인지 확인..
		(*s_GearSlotCheckTable[n])(pItem))	// 적절한 Slot인지 확인
	{
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

			if( n == GEAR_OUSTERS_ARMSBAND1	&& !m_bIsOtherGear)
			{
				g_pArmsBand1 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			else if ( n == GEAR_OUSTERS_ARMSBAND2 && !m_bIsOtherGear)
			{
				g_pArmsBand2 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			
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
MOustersGear::RemoveItem(GEAR_OUSTERS n)
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
		n = GEAR_OUSTERS_RIGHTHAND;
	}
	/***********************************************************	
				코어잽 관련 처리
	***********************************************************/
	if(n>=GEAR_OUSTERS_STONE1 && n<= GEAR_OUSTERS_STONE4) 
	{
		if(NULL != m_ItemSlot[n+m_Gilles_CoreZap]) // 코어잽이 붙어 있으면 코어잽을 뗀다
			n = (GEAR_OUSTERS)(n + m_Gilles_CoreZap); // 코어잽 위치
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if( n==GEAR_OUSTERS_PENDENT2 )
	{
		if(NULL != m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]) // 에테리얼 체인이 붙어 있으면 에테리얼 체인을 뗀다
			n = (GEAR_OUSTERS)(GEAR_OUSTERS_ETHEREALCHAIN); // 에테리얼 체인 위치
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if(n>=GEAR_OUSTERS_RING1 && n<= GEAR_OUSTERS_RING2) 
	{
		if(NULL != m_ItemSlot[n+s_Slot_Offset_Cue])			// 큐가 붙어 있으면 큐를 뗀다
			n = (GEAR_OUSTERS)(n + s_Slot_Offset_Cue);		// 큐 위치
	}

//	else if(n >= GEAR_OUSTERS_ZAP1 && n <= GEAR_OUSTERS_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;

	pItem = MPlayerGear::RemoveItem( (BYTE)n );

	//-----------------------------------------------------
	// 없는 경우
	//-----------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	if( n == GEAR_OUSTERS_ARMSBAND1 || n == GEAR_OUSTERS_ARMSBAND2 )
	{
		if( n == GEAR_OUSTERS_ARMSBAND1 )
			g_pArmsBand1 = NULL;
		else
			g_pArmsBand2 = NULL;
		
	}

	//-----------------------------------------------------
	// 양손 Item인 경우 양손의 Slot을 모두 제거해줘야 한다.
	//-----------------------------------------------------
	if (pItem->IsGearSlotTwoHand())
	{
		m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = NULL;
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
MOustersGear::RemoveItem(TYPE_OBJECTID id)
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
		m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = NULL;
		m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = NULL;
	}
	if( pItem->GetItemClass() == ITEM_CLASS_OUSTERS_ARMSBAND )
	{
		if( pItem->GetItemSlot()== GEAR_OUSTERS_ARMSBAND1 )
			g_pArmsBand1 = NULL;
		else
			g_pArmsBand2 = NULL;		
	}


	#ifdef __GAME_CLIENT__
		g_pPlayer->CalculateStatus();
	#endif

	// 있~으면...
	return pItem;
}

//----------------------------------------------------------------------
// ReplaceItem : pItem을 추가하고 딴게 있다면 Item교환
//----------------------------------------------------------------------
bool			
MOustersGear::ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem)
{
	//---------------------------------------------------------
	// ItemSlot 범위를 넘어가는 경우..
	//---------------------------------------------------------	

	if (n>=m_Size || !pItem->IsOustersItem())
	{
		return false;
	}


	/*
	if(n == GEAR_OUSTERS_FASCIA)
	{
		if(pItem->IsGearSlotOustersFascia() == true)
		{
			if (m_ItemSlot[GEAR_OUSTERS_FASCIA]==NULL)	
			{
				MPlayerGear::ReplaceItem( pItem, GEAR_OUSTERS_FASCIA, pOldItem );
				return true;
			}
			else
				return false;
		}
	}
	
	if(n == GEAR_OUSTERS_MITTEN)
	{
		if(pItem->IsGearSlotOustersMitten() == true)
		{
			if (m_ItemSlot[GEAR_OUSTERS_MITTEN]==NULL)
			{
				MPlayerGear::ReplaceItem( pItem, GEAR_OUSTERS_MITTEN, pOldItem );
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
	if (n==GEAR_OUSTERS_LEFTHAND || n==GEAR_OUSTERS_RIGHTHAND)	
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
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==NULL
					&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]==NULL)
			{
				// 그냥 추가한다.
				pOldItem = NULL;
				if (MPlayerGear::AddItem( pItem, GEAR_OUSTERS_RIGHTHAND ))
				{
					//-------------------------------------------------
					// item의 내구성 상태 설정
					//-------------------------------------------------
					CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );

					pItem->SetItemSlot( GEAR_OUSTERS_RIGHTHAND );
					m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;
					m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = pItem;

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
			if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL 
				&& m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{
				//-------------------------------------------------
				// 양손에 있는게 같다면..
				//-------------------------------------------------
				if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]==m_ItemSlot[GEAR_OUSTERS_LEFTHAND])
				{
					// 양손 무기와 교체하는 경우이다.
					if (MPlayerGear::ReplaceItem( pItem, 
													GEAR_OUSTERS_RIGHTHAND, 
													pOldItem ))
					{
						//-------------------------------------------------
						// item의 내구성 상태 설정
						//-------------------------------------------------
						CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );

						//pItem->SetItemSlot( GEAR_OUSTERS_RIGHTHAND );
						m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;
						//m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = pItem;

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
			else if (m_ItemSlot[GEAR_OUSTERS_RIGHTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_OUSTERS_RIGHTHAND, 
													pOldItem );
			}		

			//-------------------------------------------------
			// 왼손에 뭔가 있는 경우
			//-------------------------------------------------
			// 왼손의 Item과 Replace한다.
			//-------------------------------------------------
			else if (m_ItemSlot[GEAR_OUSTERS_LEFTHAND]!=NULL)
			{
				bAdd = MPlayerGear::ReplaceItem( pItem, 
													GEAR_OUSTERS_LEFTHAND, 
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
				CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );
				
				pItem->SetItemSlot( GEAR_OUSTERS_RIGHTHAND );
				m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = pItem;
				m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = pItem;
		
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
													GEAR_OUSTERS_RIGHTHAND, 
													pOldItem );

			//-------------------------------------------------
			// 교환되어 나온 Item이 양손인 경우
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					// 왼손을 비워둔다.
					m_ItemSlot[GEAR_OUSTERS_LEFTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				// item의 내구성 상태 설정
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_OUSTERS_RIGHTHAND );

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
													GEAR_OUSTERS_LEFTHAND, 
													pOldItem );
			//-------------------------------------------------
			// 교환되어 나온 Item이 양손인 경우
			//-------------------------------------------------
			if (pOldItem!=NULL)
			{
				if (pOldItem->IsGearSlotTwoHand())
				{
					// 오른손을 비워둔다.
					m_ItemSlot[GEAR_OUSTERS_RIGHTHAND] = NULL;
				}

			}

			if (bAdd)
			{
				//-------------------------------------------------
				// item 상태 설정
				//-------------------------------------------------
				CheckItemStatus( pItem, GEAR_OUSTERS_LEFTHAND );
				
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
	if (n >= GEAR_OUSTERS_STONE1 && n <= GEAR_OUSTERS_STONE4)	
	{
		if (pItem->IsGearSlotOustersStone())
		{			
			if (m_ItemSlot[n+m_Gilles_CoreZap]!=NULL) // 코어잽이 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotOustersCoreZap())
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
	//	return false;
	}

	// nanomech 2006.03.09 Item 수정 작업 start
	/***********************************************************	
				에테리얼 체인 관련 처리
	***********************************************************/
	if (n == GEAR_OUSTERS_PENDENT2 )
	{
		if (pItem->IsGearSlotOustersPendent())
		{			
			if (m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]!=NULL) // 에테리얼 체인이 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotOustersEtherealChain())
		{			
			if (m_ItemSlot[n]!=NULL) // 에테리얼 체인은 링이 있어야 놓을 수 있다. 
			{
				bool bAdd = false;
				if(m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN] == NULL)// 기존 에테리얼 체인이 없으면
				{
					pOldItem = NULL;
					bAdd = MPlayerGear::AddItem( pItem, GEAR_OUSTERS_ETHEREALCHAIN );	// 걍 추가
				}
				else // 기존 기존 에테리얼이 있으면 교환
				{
					bAdd = MPlayerGear::ReplaceItem( pItem, GEAR_OUSTERS_ETHEREALCHAIN, pOldItem );	
					
					if (bAdd)
					{
						CheckItemStatus( pItem, GEAR_OUSTERS_ETHEREALCHAIN );
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
	//	return false;
	}

	// nanomech 2006.03.09 Item 수정 작업 end


	/***********************************************************	
				큐 관련 처리
	***********************************************************/
	if (n >= GEAR_OUSTERS_RING1 && n <= GEAR_OUSTERS_RING2)	
	{
		if (pItem->IsGearSlotOustersRing())
		{			
			if (m_ItemSlot[n+s_Slot_Offset_Cue]!=NULL) // 큐가 있으면 못 놓음
				return false;
		}
		else
		if (pItem->IsGearSlotOustersCue())
		{			
			if (m_ItemSlot[n]!=NULL) // 큐는 링이 있어야 놓을 수 있다. 
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
	//	return false;
	}


//	else if(n >= GEAR_OUSTERS_ZAP1 && n <= GEAR_OUSTERS_ZAP4) // 코어잽 슬롯은 보이지 않는다. -_-?? 
//		return false;
	
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

			if( n == GEAR_OUSTERS_ARMSBAND1	&& !m_bIsOtherGear)
			{
				g_pArmsBand1 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			else if ( n == GEAR_OUSTERS_ARMSBAND2 && !m_bIsOtherGear)
			{
				g_pArmsBand2 = (MOustersArmsBand*)pItem;
				#ifdef __GAME_CLIENT__		
				UI_ResetQuickItemSlot();
				#endif
			}
			return true;
		}

		return false;
	}

	return false;
}

MItem*	MOustersGear::GetGearItem_PickUp(int &n) const	
{
	int Slot = n;
//	if(Slot>=GEAR_OUSTERS_BLOOD_BIBLE1 && Slot<=GEAR_OUSTERS_BLOOD_BIBLE6) // 블러드 바이블은 집을수 없다.
//		return NULL;
	if(Slot>=GEAR_OUSTERS_STONE1 && Slot<=GEAR_OUSTERS_STONE4)
	{
		if(m_ItemSlot[Slot+m_Gilles_CoreZap]!=NULL)
		{
			n = n+m_Gilles_CoreZap;
			return MSlotItemManager::GetItem( (BYTE)(Slot+m_Gilles_CoreZap) ); 
		}
	}
	
	// nanomech 2006.03.09 Item 수정 작업 start
	if( Slot==GEAR_OUSTERS_PENDENT2 )
	{
		if(m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]!=NULL)
		{
			n = GEAR_OUSTERS_ETHEREALCHAIN;
			return MSlotItemManager::GetItem( (BYTE)(GEAR_OUSTERS_ETHEREALCHAIN) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end


	if(Slot>=GEAR_OUSTERS_RING1 && Slot<=GEAR_OUSTERS_RING2)
	{
		if(m_ItemSlot[Slot+s_Slot_Offset_Cue]!=NULL)
		{
			n = n+s_Slot_Offset_Cue;
			return MSlotItemManager::GetItem( (BYTE)(Slot+s_Slot_Offset_Cue) ); 
		}
	}
	
	return MSlotItemManager::GetItem( (BYTE)Slot ); 
}


MItem*	MOustersGear::GetGearCoreZapItem(int slot) const	
{
	if(slot>=GEAR_OUSTERS_STONE1 && slot<=GEAR_OUSTERS_STONE4)
	{
		if(m_ItemSlot[slot+m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot+m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_CORE_ZAP)
				return MSlotItemManager::GetItem( (BYTE)(slot+m_Gilles_CoreZap) ); 
		}
	}
	
	// nanomech 2006.03.09 Item 수정 작업 start
	if( slot==GEAR_OUSTERS_PENDENT2 )
	{
		if(m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]!=NULL)
		{
			if(m_ItemSlot[GEAR_OUSTERS_ETHEREALCHAIN]->GetItemClass() == ITEM_CLASS_ETHEREAL_CHAIN )
				return MSlotItemManager::GetItem( (BYTE)(GEAR_OUSTERS_ETHEREALCHAIN) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	if(slot>=GEAR_OUSTERS_RING1 && slot<=GEAR_OUSTERS_RING2)
	{
		if(m_ItemSlot[slot+s_Slot_Offset_Cue]!=NULL)
		{
			if(m_ItemSlot[slot+s_Slot_Offset_Cue]->GetItemClass() == ITEM_CLASS_CUE_OF_ADAM)
				return MSlotItemManager::GetItem( (BYTE)(slot+s_Slot_Offset_Cue) ); 
		}
	}
	
	return NULL;
}


MItem*	MOustersGear::GetGearCoreZapedItem(int slot) const	
{
	if(slot>=GEAR_OUSTERS_ZAP1 && slot<=GEAR_OUSTERS_ZAP4)
	{
		if(slot-m_Gilles_CoreZap<0)
			return NULL;
		if(m_ItemSlot[slot-m_Gilles_CoreZap]!=NULL)
		{
			if(m_ItemSlot[slot-m_Gilles_CoreZap]->GetItemClass() == ITEM_CLASS_OUSTERS_STONE)
				return MSlotItemManager::GetItem( (BYTE)(slot-m_Gilles_CoreZap) ); 
		}
	}
	
	// nanomech 2006.03.09 Item 수정 작업 start
	if(slot==GEAR_OUSTERS_ETHEREALCHAIN )
	{
		if( GEAR_OUSTERS_PENDENT2<0)
			return NULL;
		if(m_ItemSlot[GEAR_OUSTERS_PENDENT2]!=NULL)
		{
			if(m_ItemSlot[GEAR_OUSTERS_PENDENT2]->GetItemClass() == ITEM_CLASS_OUSTERS_PENDENT)
				return MSlotItemManager::GetItem( (BYTE)(GEAR_OUSTERS_PENDENT2) ); 
		}
	}
	// nanomech 2006.03.09 Item 수정 작업 end

	if(slot>=GEAR_OUSTERS_CUE1 && slot<=GEAR_OUSTERS_CUE2)
	{
		if(slot-s_Slot_Offset_Cue<0)
			return NULL;
		if(m_ItemSlot[slot-s_Slot_Offset_Cue]!=NULL)
		{
			if(m_ItemSlot[slot-s_Slot_Offset_Cue]->GetItemClass() == ITEM_CLASS_OUSTERS_RING)
				return MSlotItemManager::GetItem( (BYTE)(slot-s_Slot_Offset_Cue) ); 
		}
	}
	
	return NULL;
}

const bool	MOustersGear::IsHasAllCoreZap(int CoreZapType) const	
{
	BYTE bType[4] = {0,};
	if(CoreZapType == 0) // 질드레 코어 잽 일 경우
	{
		MItem* pItem;
		for(int i = 0; i<4; i++)
		{
			pItem = m_ItemSlot[GEAR_OUSTERS_ZAP1+i];
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

const bool MOustersGear::IsHasAllCue(OUT bool CueOption[]) const
{
	ZeroMemory(CueOption, sizeof(bool)*4);
	
	for(int i = 0; i<2; i++)
	{
		MItem*	pItem		= m_ItemSlot[GEAR_OUSTERS_CUE1+i];
		
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

const bool	MOustersGear::IsCloseBloodBibleSlot(int slot) const	
{
	if(slot>=GEAR_OUSTERS_BLOOD_BIBLE1 && slot<=GEAR_OUSTERS_BLOOD_BIBLE6)
	{
		int TempSlot = slot - GEAR_OUSTERS_BLOOD_BIBLE1 + 1;
		if(TempSlot > m_bBloodBibleOpenCount)
			return true;
	}
	return false;
}
