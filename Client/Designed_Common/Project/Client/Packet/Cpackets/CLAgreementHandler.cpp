#ifndef __GAME_CLIENT__
//////////////////////////////////////////////////////////////////////////////

// Filename    : CLAgreementHandler.cpp

// Written By  : elca

// Description : 

//////////////////////////////////////////////////////////////////////////////



#include "CLAgreement.h"



#ifdef __LOGIN_SERVER__

	#include "LoginPlayer.h"

	#include "DB.h"

	#include "DarkAssert.h"

#endif



//////////////////////////////////////////////////////////////////////////////

// ì´ íŒ¨í‚·ì€ í´ë¼ì´ì–¸íŠ¸ê°€ ì•„ì´ë””ì™€ íŒ¨ìŠ¤ì›Œë“œë¥¼ ì•”í˜¸í™”í•´ì„œ 

// ë¡œê·¸ì¸ ì„œë²„ë¡œ ì „ì†¡í•œë‹¤. ë¡œê·¸ì¸ ì„œë²„ëŠ” ì´ íŒ¨í‚·ì„ ë°›ì•„ì„œ

// í”Œë ˆì´ì–´ì˜ ì•„ì´ë””ì™€ íŒ¨ìŠ¤ì›Œë“œê°€ ì •í™•í•œì§€ DBë¡œë¶€í„° ì½ì–´ì„œ

// ë¹„êµí•œ í›„, ë¡œê·¸ì¸ì˜ ì„±ê³µ ì—¬ë¶€ë¥¼ ì „ì†¡í•œë‹¤.

//////////////////////////////////////////////////////////////////////////////

void CLAgreementHandler::execute (CLAgreement* pPacket , Player* pPlayer)

	 

{

	__BEGIN_TRY __BEGIN_DEBUG_EX

		

#ifdef __LOGIN_SERVER__

#ifdef __NETMARBLE_SERVER__



	Assert(pPacket != NULL);

	Assert(pPlayer != NULL);



	LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

	Assert( pLoginPlayer != NULL );



	if ( pLoginPlayer->isAgree() )

	{

		Statement*   pStmt        = NULL;



		BEGIN_DB

		{

			//----------------------------------------------------------------------

			// ë„·ë§ˆë¸” ì•½ê´€ ë¯¸ë™ì˜ ë¦¬ìŠ¤íŠ¸ì—ì„œ ì‚­ì œ

			//----------------------------------------------------------------------

			pStmt   = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

			pStmt->executeQuery( "DELETE FROM PrivateAgreementRemain WHERE PlayerID = '%s'", pLoginPlayer->getID().c_str() );



			// ë‹¤ìŒ ë‹¨ê³„ë¡œ ì§„í–‰í•  ìˆ˜ ìžˆê²Œ ì„¤ì •

			pLoginPlayer->setAgree( true );



			SAFE_DELETE(pStmt);

		}

		END_DB(pStmt)

	}



#endif

#endif



	__END_DEBUG_EX __END_CATCH

}


#endif
