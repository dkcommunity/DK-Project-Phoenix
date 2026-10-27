//////////////////////////////////////////////////////////////////////
//
// Filename    : GCBloodBibleSignInfoHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
// include files
#include "GCBloodBibleSignInfo.h"
#include "PacketFunction.h"
//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// BloodBibleSignInfo*	g_pBloodBibleSignInfo	= NULL;
BloodBibleSignInfo*	g_pBloodBibleSignInfo	= NULL;	//장착창이 바뀔때에 인수로 사용될 글로벌 값
															//SetGearInfo 2번째 인수가 NULL로 가니깐 피의 성서가 안보이는 현상이 발생.
															//GCChangeGearSlot에서 BloodBibleSignInfo 패킷도 같이 받으면 좋겠다. ㅜ.ㅜ
#endif // __GEAR_SWAP_CHANGE



void GCBloodBibleSignInfoHandler::execute ( GCBloodBibleSignInfo * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
	
#ifdef __GAME_CLIENT__
	SetBloodBibleSlot(pPacket->getSignInfo());

#if __CONTENTS(__GEAR_SWAP_CHANGE)	// 	g_pBloodBibleSignInfo	= pPacket->getSignInfo();
	g_pBloodBibleSignInfo	= pPacket->getSignInfo();
#endif	// __GEAR_SWAP_CHANGE

	//std::cout << pPacket->toString() << std::endl;
#endif

	 __END_CATCH
}
