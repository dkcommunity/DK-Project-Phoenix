////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionCanEnterPayZone.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ConditionCanEnterPayZone.h"
#include "PlayerCreature.h"
#include "FlagSet.h"

#include "PaySystem.h"
#include "GamePlayer.h"
#include "PacketUtil.h"
#include "DB.h"

////////////////////////////////////////////////////////////////////////////////
// is satisfied?
////////////////////////////////////////////////////////////////////////////////
bool ConditionCanEnterPayZone::isSatisfied (Creature * pCreature1 , Creature * pCreature2, void* pParam) const 
{ 
	Assert(pCreature2 != NULL);
	Assert(pCreature2->isPC());

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature2->getPlayer());

	Assert(pGamePlayer!=NULL);

	// 이미 유료존에 있는 경우라면... 관계없겠지.
	// 패밀리 요금 적용중인 경우
//  if( pGamePlayer->isPayPlaying() || pGamePlayer->isFamilyFreePass() )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//          기존 isPayPlaying() 은 모든 기간제 사용자는 다 체크하는 거라 유료존 티켓에는 맞지 않는다.
//          어차피 기간제 사용자가 프리미엄 사용자가 되기 땜시 isPremiumPlay로 바꾸고 유료존 티켓체크를 추가한다. isPayZoneTicket()
    if ( pGamePlayer->isPremiumPlay() || pGamePlayer->isPayZoneTicket() || pGamePlayer->isFamilyFreePass() )
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		return true;
	}

//	string connectIP = pGamePlayer->getSocket()->getHost();
	
//	if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID()))
//	{
//		sendPayInfo(pGamePlayer);
//		return true;
//	}
//	else
//	{
		return false;
//	}
#else
	return true;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ConditionCanEnterPayZone::read (PropertyBuffer & propertyBuffer)
{
}

////////////////////////////////////////////////////////////////////////////////
	// get debug string
////////////////////////////////////////////////////////////////////////////////
string ConditionCanEnterPayZone::toString () const 
{ 
	__BEGIN_TRY

	StringStream msg;
	msg << "ConditionCanEnterPayZone("
		<< ")"; 
	return msg.toString();

	__END_CATCH
}
