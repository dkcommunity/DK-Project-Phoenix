//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGetOffMotorCycleHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCGetOffMotorCycle.h"
#include "ClientDef.h"
#include "SoundDef.h"
#include "MCreatureWear.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCGetOffMotorCycleHandler::execute ( GCGetOffMotorCycle * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#if __GAME_CLIENT__

	
	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
	}
	//------------------------------------------------------
	// 정상.. 
	//
	// Creature가 오토바이를 장착해제 한다.
	//------------------------------------------------------
	else
	{
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		if (pCreature!=NULL)
		{
			// 장착가능한 creature인 경우
			if (pCreature->IsWear())
			{
				// FAZ 4 (2026-05-12): packet handler sweep — defansif dynamic_cast.
				// IsWear() virtual guard var; release build'de class hiyerarsi
				// mismatch korumasi olarak dynamic_cast da yapiyoruz.
				MCreatureWear* pCreatureWear = dynamic_cast<MCreatureWear*>(pCreature);
				if (pCreatureWear == NULL)
				{
					DEBUG_ADD_FORMAT("[Error] GetOffMotorCycle dynamic_cast<MCreatureWear*> failed id=%d", pPacket->getObjectID());
					return;
				}

				//------------------------------------------
				// 내리는 소리 출력
				//------------------------------------------
				const TYPE_SOUNDID dismountSound =
					pCreatureWear->GetRideMotorcycleSoundID(MOTORCYCLE_SOUND_DISMOUNT);

				//------------------------------------------
				// 오토바이 그림을 없애준다.
				//------------------------------------------
				pCreatureWear->RemoveAddon( ADDON_MOTOR );

				pCreatureWear->RemoveAttachEffect(EFFECTSPRITETYPE_SLAYER_WING_BIKE_MOVE);
				//------------------------------------------
				// 움직이는 방법 바꿈
				//------------------------------------------
				pCreatureWear->SetMoveDevice( MCreature::MOVE_DEVICE_WALK );

				PlaySound( dismountSound,
							false,
							g_pPlayer->GetX(),
							g_pPlayer->GetY());
			}
			else
			{
				DEBUG_ADD_FORMAT("[Error] Creature is not WearCreature id=%d", pPacket->getObjectID());
			}
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] Not Exist Creature id=%d", pPacket->getObjectID());
		}
	}

#endif

	__END_CATCH
}
