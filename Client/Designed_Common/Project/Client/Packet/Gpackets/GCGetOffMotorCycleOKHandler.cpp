//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGetOffMotorCycleOKHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCGetOffMotorCycleOK.h"
#include "ClientDef.h"
#include "SoundDef.h"
#include "MCreatureWear.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCGetOffMotorCycleOKHandler::execute ( GCGetOffMotorCycleOK * pPacket, Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#if __GAME_CLIENT__

	
	//------------------------------------------
	// 내리는 소리 출력
	//------------------------------------------
	const TYPE_SOUNDID dismountSound =
		g_pPlayer->GetRideMotorcycleSoundID(MOTORCYCLE_SOUND_DISMOUNT);

	//------------------------------------------
	// 오토바이 그림을 없애준다.
	//------------------------------------------
	g_pPlayer->RemoveAddon( ADDON_MOTOR );

	//------------------------------------------
	// 움직이는 방법 바꿈
	//------------------------------------------
	g_pPlayer->SetMoveDevice( MCreature::MOVE_DEVICE_WALK );

	//------------------------------------------
	// 어쨋든 간에.. 검증이 되었다고 본다.
	//------------------------------------------
	g_pPlayer->SetWaitVerifyNULL();

	g_pPlayer->RemoveAttachEffect(EFFECTSPRITETYPE_SLAYER_WING_BIKE_MOVE);

	PlaySound( dismountSound );

#endif

	__END_CATCH
}
