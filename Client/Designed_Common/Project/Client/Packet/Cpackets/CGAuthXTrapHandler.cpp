#if 0

//////////////////////////////////////////////////////////////////////
//
// Filename    : CGAuthXTrapHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CGAuthXTrap.h"

#ifdef __GAME_SERVER__
#include "GamePlayer.h"
#include "EventKick.h"
#include "XTrap4Server.h"
#include "Gpackets/GCSystemMessage.h"
#endif

//////////////////////////////////////////////////////////////////////
//
// í´ë¼ì´ì–¸íŠ¸ì—ì„œ ì„œë²„ë¡œë¶€í„° ë©”ì‹œì§€ë¥¼ ë°›ì•˜ì„ë•Œ ì‹¤í–‰ë˜ëŠ” ë©”ì˜ë“œì´ë‹¤.
//
//////////////////////////////////////////////////////////////////////
void CGAuthXTrapHandler::execute ( CGAuthXTrap * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_SERVER__

/*	Assert( pPacket != NULL );

	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
	Assert( pGamePlayer != NULL );

	DWORD key = pPacket->getKey();
	if ( !pGamePlayer->getCSAuth().CheckAuthDword(key) )
	{
		filelog("CSAuth.log", "[%s] ì¸ì¦ ê°’ì´ ìž˜ëª»ë˜ì—ˆìŠµë‹ˆë‹¤.", pGamePlayer->getID().c_str());

		GCSystemMessage gcSystemMessage;
		gcSystemMessage.setMessage("nProtect GameGuard ì¸ì¦ì— ì‹¤íŒ¨í–ˆìŠµë‹ˆë‹¤. ì‹¤í–‰ íŒŒì¼ì´ ë³€ì¡°ë˜ê±°ë‚˜ GameGuard íŒŒì¼ì´ ì†ìƒë˜ì—ˆìŠµë‹ˆë‹¤.");
		pGamePlayer->sendPacket( &gcSystemMessage );

		EventKick* pKick = new EventKick( pGamePlayer );
		pKick->setDeadline(100);
//		pKick->setMessage("GameGuard ì¸ì¦ ì½”ë“œê°€ ìž˜ëª»ë˜ì—ˆìŠµë‹ˆë‹¤. 10ì´ˆ ë’¤ì— ì—°ê²°ì´ ì¢…ë£Œë©ë‹ˆë‹¤.");
		pKick->sendMessage();

		pGamePlayer->addEvent(pKick);
	}
	
	*/
	
	Assert(pPacket != NULL);
	GamePlayer * pGamePlayer = dynamic_cast<GamePlayer *> (pPlayer);
	Assert ( pGamePlayer != NULL);
	

	int ret = -1;

	switch( pPacket->m_shCmdFlag )
	{
		case XTRAP_CMD_STEP_TWO:
			{
				ret = XTrap_CS_Step3( pGamePlayer->getXTSessionData(), pPacket->getAuthData() );
				
				std::cout << "C-S XTrap_CS_Step3 Authencation Check!!ret=" <<ret << std::endl;
				if(XTRAP_API_RETURN_OK != ret)
				{					
					///< ì„¹ì…˜ ì¢…ë£Œì‹œí‚¤ë¼ë„¤...
					///< ìµœì‹  ë©”ë‰´ì–¼ì—ì„  ì„¹ì…˜ ì¢…ë£Œ ë§ë¼ë„¤..XTrap_CS_Step1ì—ì„œ ì¢…ë£Œ
					//filelog("CSAuth.log", "[%s] ì¸ì¦ ê°’ì´ ìž˜ëª»ë˜ì—ˆìŠµë‹ˆë‹¤.", pGamePlayer->getID().c_str());
					//GCSystemMessage gcSystemMessage;
					//gcSystemMessage.setMessage("XTrap GameGuard ì¸ì¦ì— ì‹¤íŒ¨í–ˆìŠµë‹ˆë‹¤. ì‹¤í–‰ íŒŒì¼ì´ ë³€ì¡°ë˜ê±°ë‚˜ GameGuard íŒŒì¼ì´ ì†ìƒë˜ì—ˆìŠµë‹ˆë‹¤.");
					//pGamePlayer->sendPacket( &gcSystemMessage );
					//EventKick* pKick = new EventKick( pGamePlayer );
					//pKick->setDeadline(100);
					//pKick->sendMessage();
					//pGamePlayer->addEvent(pKick);

				}
			}
			break;
		case XTRAP_MAP_FILE_CHANGE:
			{
			//	MapFileChange();
				std::cout << "XTRAP_MAP_FILE_CHANGE " << std::endl;
			}
			break;
	}

#endif
	
	__END_CATCH
}

#endif
