//////////////////////////////////////////////////////////////////////////////
// Filename    : CGLogoutHandler.cc
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGLogout.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "Creature.h"
	#include "Zone.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// 클라이언트가 CGLogout 패킷을 보내면, 게임 서버는 크리처를 존에서 삭제하고,
// 크리처와 아이템 정보를 DB에 저장한 후, 접속을 종료한다.
//////////////////////////////////////////////////////////////////////////////
void CGLogoutHandler::execute (CGLogout* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

	Assert(pPlayer != NULL);
	
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

	Creature* pCreature = pGamePlayer->getCreature();

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();

	Assert(pZone != NULL);
	
	pZone->pushLogoutPC(pCreature);

#endif

	__END_DEBUG_EX __END_CATCH
}


