//----------------------------------------------------------------------
// MPlayerGear.cpp
//----------------------------------------------------------------------
// Player의 Gear에 대한 class
//----------------------------------------------------------------------

#ifndef __MILITARYSLOT_H__
#define __MILITARYSLOT_H__

#include "MSlotItemManager.h"


class MilitarySlot : public MSlotItemManager 
{

	public :
		MilitarySlot();
		~MilitarySlot();

		enum E_Level
		{
			L_P1_0,
			L_P1_1,
			L_P1_2,
			L_P1_3,
			L_P1_4,
			L_P1_5,
			L_P1_6,
			L_P1_7,
			L_P1_8,
			L_P1_9,
			L_P1_10,
			L_P1_11,
			L_P1_12,
			L_P1_13,
			L_P1_14,
			L_P1_15,
			L_P1_16,
			L_P1_17,

			L_P2_0,
			L_P2_1,
			L_P2_2,
			L_P2_3,
			L_P2_4,
			L_P2_5,
			L_P2_6,
			L_P2_7,
			L_P2_8,
			L_P2_9,
			L_P2_10,
			L_P2_11,
			L_P2_12,
			L_P2_13,
			L_P2_14,
			L_P2_15,
			L_P2_16,
			L_P2_17,
			L_P2_18,
			L_P2_19,

			L_P3_0,
			L_P3_1,
			L_P3_2,
			L_P3_3,
			L_P3_4,
			L_P3_5,
			L_P3_6,
			L_P3_7,
			L_P3_8,
			L_P3_9,
			L_P3_10,
			L_P3_11,
			L_P3_12,
			L_P3_13,
			L_P3_14,
			L_P3_15,
			L_P3_16,
			L_P3_17,
			
			L_MAX,
		};

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
};

extern MilitarySlot*		g_pMilitarySlot;

#endif

