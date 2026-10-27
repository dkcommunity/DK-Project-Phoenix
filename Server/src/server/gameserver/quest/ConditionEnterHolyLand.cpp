////////////////////////////////////////////////////////////////////////////////
// Filename    : ConditionEnterHolyLand.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "ConditionEnterHolyLand.h"
#include "PlayerCreature.h"
#include "FlagSet.h"

#include "Zone.h"
#include "PaySystem.h"
#include "WarSystem.h"
#include "GamePlayer.h"
#include "PacketUtil.h"
#include "ZoneInfoManager.h"
#include "ZoneGroupManager.h"
#include "ZoneUtil.h"
#include "VariableManager.h"
#include "DB.h"

////////////////////////////////////////////////////////////////////////////////
// is satisfied?
////////////////////////////////////////////////////////////////////////////////
bool ConditionEnterHolyLand::isSatisfied (Creature * pCreature1 , Creature * pCreature2, void* pParam) const 
{ 
	/*if (!g_pVariableManager->isActiveHolyLand())
	{
		return false;
	}*/
	// 나중에 전쟁중인지 체크해야 된다

	Assert(pCreature2 != NULL);
	Assert(pCreature2->isPC());

	//cout << "Condition: EnterHolyLand" << endl;

	bool bPayPlay = false;

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature2->getPlayer());
	Assert(pGamePlayer!=NULL);

#if defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
	// 이미 유료존에 있는 경우라면... 관계없겠지.
	// 패밀리 요금제 적용중일 경우
//  if( pGamePlayer->isPayPlaying() || pGamePlayer->isFamilyFreePass() )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//          20080227 유료존 티켓 추가하면서 코드 수정도 함께 한다.
//          기존 isPayPlaying() 은 모든 기간제 사용자는 다 체크하는 거라 유료존 티켓에는 맞지 않는다.
//          어차피 기간제 사용자가 프리미엄 사용자가 되기 땜시 isPremiumPlay로 바꾸고 유료존 티켓체크를 추가한다. isPayZoneTicket()
    if ( pGamePlayer->isPremiumPlay() || pGamePlayer->isPayZoneTicket() || pGamePlayer->isFamilyFreePass() )
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        bPayPlay = true;
    }
//  else
//  {
//      string connectIP = pGamePlayer->getSocket()->getHost();
//      if( pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID() ) )
//      {
//          sendPayInfo(pGamePlayer);
//          bPayPlay = true;
//      }
//  }
#else
	bPayPlay = true;
#endif

	// 돈 낸 사람만 castle 에 들어갈 수 있다.
	if (bPayPlay || g_pVariableManager->getVariable(OPEN_PAYZONE) )
	{
		// 종족 전쟁 중에.. 전쟁 참가 인원 제한을 한다면..
		if (g_pWarSystem->hasActiveRaceWar()
			&& g_pVariableManager->isActiveRaceWarLimiter())
		{
			Zone* pZone = getZoneByZoneID(m_TargetZoneID);
			Assert(pZone!=NULL);

			// 아담의 성지에 들어갈때
			if (!pZone->isHolyLand())
			{
				return true;
			}

			PlayerCreature* pPC = dynamic_cast<PlayerCreature*>( pCreature2 );

			// 전쟁 참가를 했었어야 한다.
			return pPC->isFlag( Effect::EFFECT_CLASS_RACE_WAR_JOIN_TICKET );
		}

		// 전쟁 중 아니면 그냥 들어간다.
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void ConditionEnterHolyLand::read (PropertyBuffer & propertyBuffer)
{
	try
	{
		// read turn
		m_TargetZoneID = propertyBuffer.getPropertyInt("TargetZoneID");
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}
}

////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ConditionEnterHolyLand::toString () const 
{ 
	__BEGIN_TRY

	StringStream msg;
	msg << "ConditionEnterHolyLand("
		<< "TargetZoneID:" << (int)m_TargetZoneID
		<< ")"; 
	return msg.toString();

	__END_CATCH
}
