//////////////////////////////////////////////////////////////////////////////
// Filename    : GearSlotInfo.h
// Written By  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GEAR_SLOT_INFO_H__
#define __GEAR_SLOT_INFO_H__

#include "Types.h"
#include "Exception.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "PCItemInfo.h"

//////////////////////////////////////////////////////////////////////////////
// Inventory 정보를 담고 있는 객체.
//////////////////////////////////////////////////////////////////////////////

class GearSlotInfo : public PCItemInfo 
{
public:
	void read ( SocketInputStream & iStream ) ;
	void write ( SocketOutputStream & oStream ) const ;

	uint getSize () 
	{ 
		DWORD	dwSize	= 0;
		dwSize	+= PCItemInfo::getSize();
		dwSize	+= szSlotID;

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GearSlotInfo m_bActiveSlot패킷 사이즈 추가
		dwSize	+= sizeof(bool);
#endif //__GEAR_SWAP_CHANGE
		return dwSize;
	}

	static uint getMaxSize () 
	{ 
		DWORD	dwSize	= 0;
		dwSize	+= PCItemInfo::getMaxSize();
		dwSize	+= szSlotID;

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GearSlotInfo m_bActiveSlot패킷 사이즈 추가
		dwSize	+= sizeof(bool);
#endif // __GEAR_SWAP_CHANGE
		return dwSize;
	}

	#ifdef __DEBUG_OUTPUT__
		std::string toString () const ;
	#endif

public:
	void setSlotID( SlotID_t SlotID )  { m_SlotID = SlotID; }
	SlotID_t getSlotID() const  { return m_SlotID; }

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GearSlotInfo setActiveSlot, getActiveSlot 함수 추가
//    void setActiveSlot(bool bActive) { m_bActiveSlot = bActive; }
    bool getActiveSlot() { return m_bActiveSlot; }
#endif // __GEAR_SWAP_CHANGE

private:
	SlotID_t	m_SlotID;

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GearSlotInfo 아이템이 착용 가능한지의 대해 서버에서 내려준다.
	bool		m_bActiveSlot;
#endif // __GEAR_SWAP_CHANGE
};

#endif
