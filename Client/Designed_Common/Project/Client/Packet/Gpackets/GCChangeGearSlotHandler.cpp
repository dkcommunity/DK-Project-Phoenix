//--------------------------------------------------------------------------------
//
// Filename    : GCChangeDarkLightHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCChangeGearSlot.h"
#include "Client.h"
#include "DarkAssert.h"

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// GCChangeGearSlotHandler.cpp
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
extern BloodBibleSignInfo*	g_pBloodBibleSignInfo;	// 전역 값으로 주고 이걸 멤버 변수로 주는거 이건 아닌거 같은데..
															// 서버 님이 너무 바빠 보이신다~! ㅜ.ㅜ
															// BloodBibleSignInfo 값을 패킷으로 받아야 할꺼 같은데..

void GCChangeGearSlotHandler::execute ( GCChangeGearSlot * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__	
	SetGearInfo( pPacket->getGearInfo(), g_pBloodBibleSignInfo, pPacket->getGearSlotID());

	// 계산을 다시해준다.
	g_pPlayer->CalculateStatus();
#endif

	__END_CATCH
}
#endif //__GEAR_SWAP_CHANGE
