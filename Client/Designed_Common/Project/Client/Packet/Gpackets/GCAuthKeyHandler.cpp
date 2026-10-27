
//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAuthKeyHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "GCAuthKey.h"
#ifdef __GAME_CLIENT__ 
	#include "Client.h"
	//#include "nProtect.h"
#endif
//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////

void GCAuthKeyHandler::execute ( GCAuthKey * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
//		__BEGIN_DEBUG_EX
	
//#ifdef __GAME_CLIENT__ 
//
//#if !defined(OUTPUT_DEBUG) && !defined(_DEBUG) && !defined(__DEBUG_OUTPUT__)
//	Set_Auth(pPacket->getKey());
////	CT_Done("GCAuthKeyHandler받음1");
//#endif
////	CT_Done("GCAuthKeyHandler받음2");
////#ifdef __NPROTECT__
////	//std::cout << pPacket->toString() << std::endl;
////	Set_Auth(pPacket->getKey());
////	CT_Done("GCAuthKeyHandler받음1");
////#endif
////	CT_Done("GCAuthKeyHandler받음2");
//#endif

//	__END_DEBUG_EX
	__END_CATCH
}

