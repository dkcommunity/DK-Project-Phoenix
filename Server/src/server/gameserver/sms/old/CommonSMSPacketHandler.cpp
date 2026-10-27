//////////////////////////////////////////////////////////////////////////////
// Filename    : CommonSMSPacketHandler.cc
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CommonSMSPacket.h"
#include "SMSPlayer.h"

#ifdef __GAME_SERVER__
	#include "PlayerCreature.h"
	#include "GamePlayer.h"
	#include "PCFinder.h"
	#include "EventKick.h"
	#include "EventSystemMessage.h"
	#include "VSDateTime.h"
	#include "StringPool.h"
	#include "ResurrectLocationManager.h"
	#include "ZoneUtil.h"
	#include "SMSResultLoginErrorMessage.h"
#endif

#include "Gpackets/GCSystemMessage.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CommonSMSPacketHandler::execute (CommonSMSPacket* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

	// 다 로그를 남기자
	filelog(LOGFILE_SMS_PACKET, "%s", pPacket->toString().c_str());

#endif

	__END_DEBUG_EX __END_CATCH
}

