#if 0

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
#include "GCAuthXTrap.h"
#include "../Cpackets/CGAuthXTrap.h"
#include "ServerInfo.h"
#include "XTrap4Client.h"

#ifdef __GAME_CLIENT__
#include "ClientPlayer.h"

extern ClientPlayer*		g_pSocket;
#endif
//////////////////////////////////////////////////////////////////////
//
// í´ë¼ì´ì–¸íŠ¸ì—ì„œ ì„œë²„ë¡œë¶€í„° ë©”ì‹œì§€ë¥¼ ë°›ì•˜ì„ë•Œ ì‹¤í–‰ë˜ëŠ” ë©”ì˜ë“œì´ë‹¤.
//
//////////////////////////////////////////////////////////////////////
void GCAuthXTrapHandler::execute ( GCAuthXTrap * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

#if !defined(_DEBUG)

	//CT_Done("GCAuthKeyHandlerë°›ìŒ1");

	CGAuthXTrap SendPack;
	
	if(pPacket->m_shCmdFlag == XTRAP_CMD_STEP_ONE)
	{
		BYTE szTemp[128] = {0};
		int ret = XTrap_CS_Step2(pPacket->getAuthData(), (void*)szTemp, XTRAP_PROTECT_PE| XTRAP_PROTECT_TEXT |XTRAP_PROTECT_EXCEPT_VIRUS);
		SendPack.setKey(szTemp);
		SendPack.m_shCmdFlag = XTRAP_CMD_STEP_TWO;
		g_pSocket->sendPacket( &SendPack );
		
		if(ret == XTRAP_API_RETURN_OK )
		{			
		}
		else
		{
			//CT_Done("GCAuthKeyHandlerë³´ëƒ„2");
		}
	}

#endif	
#endif

	__END_CATCH
}

#endif
