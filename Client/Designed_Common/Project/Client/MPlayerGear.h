//----------------------------------------------------------------------
// MPlayerGear.cpp
//----------------------------------------------------------------------
// Player의 Gear에 대한 class
//----------------------------------------------------------------------

#ifndef __MPLAYERGEAR_H__
#define __MPLAYERGEAR_H__

#include "MSlotItemManager.h"

class MPlayerGear : public MSlotItemManager {
	public :
		enum ITEM_STATUS
		{
			ITEM_STATUS_OK,					// 좋은 상태
			ITEM_STATUS_SOMEWHAT_BROKEN,		// 약간 부서짐 - -;
			ITEM_STATUS_ALMOST_BROKEN		// 많이 부서짐
		};

	public :
		MPlayerGear();
		~MPlayerGear();

		//------------------------------------------------------
		// Init / Release
		//------------------------------------------------------
		void			Init(BYTE size);
		void			Release();

		//------------------------------------------------------
		// Add / Get / Remove
		//------------------------------------------------------		
		bool			AddItem(MItem* pItem, BYTE n);		// slot(n)에 pItem을 추가한다.

		MItem*			GetItem(BYTE n) const;				// slot(n)의 item을 읽어온다.
		
		MItem*			RemoveItem(BYTE n);					// slot(n)의 item을 제거한다.
		MItem*			RemoveItem(TYPE_OBJECTID id);		// id가 같은 item을 제거한다.

		bool			ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);	// (n)위치의 Item교환

		//------------------------------------------------------
		// Item 변화
		//------------------------------------------------------
		bool			ModifyDurability(BYTE n, int changeValue);

		//------------------------------------------------------
		// 부서질려는 아이템이 있는가?
		//------------------------------------------------------
		ITEM_STATUS		GetItemStatus(int slot) const	{ return m_pItemStatus[slot]; }
		BOOL			HasBrokenItem() const			{ return m_nBrokenItem!=0; }
		void			CheckItemStatusAll();

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// MPlayerGear 서버에서 받은 장착창 ID와 보낼 장착창 ID의 값을 설정, 얻는 함수 추가
		void			SetAccepGearID(DWORD	dwGearID)	//서버에서 받은 값을 설정해 준다.
		{ 
			m_dwAccepGearSlotID	= dwGearID; 
		}

		DWORD			GetAccepGearID()					//서버에서 받은 값을 넘겨준다.					
		{ 
			return m_dwAccepGearSlotID; 
		}				

		void			SetSendGearID(DWORD	dwGearID)		//서버에 보낼 값을 설정해 준다.
		{ 
			m_dwSendGearSlotID	= dwGearID; 
		}		

		DWORD			GetSendGearID()						//서버에 보낼 값을 얻는다.		
		{ 
			return m_dwSendGearSlotID; 
		}

		void			SetActive(bool bActive)				//아이템 착용이 가능한지 설정해 준다.
		{
			m_bActive	= bActive;
		}

		bool			GetActive()							//아이템 착용이 가능한지 알려준다.
		{
			return m_bActive;
		}

		void			SetProcessingGearChange(bool bGearChagne)
		{
			m_bGearChagne	= bGearChagne;
		}

		bool			GetProcessingGearChange()
		{
			return	m_bGearChagne;
		}

		void			SetProcessingSkill(bool bUseSkill)
		{
			m_bUseSkill	= bUseSkill;
		}

		bool			GetProcessingSkill()
		{
			return	m_bUseSkill;
		}

		void			SetIsOtherGear(bool bSet) 
		{ 
			m_bIsOtherGear = bSet; 
		}
#endif //__GEAR_SWAP_CHANGE

	protected :
		//virtual void	CheckItemStatusAll() = 0;	// 필요 없을 듯..
		void			SetItemStatusOK(int slot);	// slot번째는 굿~으로 만든다.
		void			CheckItemStatus(const MItem* pItem, int slot);
		
	protected :
		int				m_nBrokenItem;	// 부서진 item의 개수
		ITEM_STATUS*	m_pItemStatus;
#if __CONTENTS(__GEAR_SWAP_CHANGE)	// MPlayerGear 서버에서 받은 장착창 ID와 보낼 장착창 ID의 변수를 추가
		DWORD			m_dwAccepGearSlotID;	//서버에서 받은 장착창 ID
		DWORD			m_dwSendGearSlotID;		//서버에 보낼 장착창 ID
		bool			m_bActive;				//아이템 착용이 가능한지 확인 한다.
		bool			m_bGearChagne;			//이중 장착창이 교체 중일때에는 아이템을 교체 할 수 없다.
		bool			m_bUseSkill;			//스킬을 사용중인지 확인 한다.
#endif //__GEAR_SWAP_CHANGE
		bool		m_bIsOtherGear;
};

#endif

