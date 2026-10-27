#ifndef __GAME_CLIENT__
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAddItemToCodeSheetHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////
/*

// í•œ ì•„ì´í…œì„ ë“¤ì–´ì„œ ë‹¤ë¥¸ ì•„ì´í…œì— ë„£ëŠ”ë‹¤.
 [Client]                                               [Server]
           -- CGAddItemToCodeSheet
              (mouseItem_ObjectID, invenX, invenY) 
			  =(ë§ˆìš°ìŠ¤ì— ë“¤ê³ ìžˆëŠ” ë³„, upgradeí•  item XY) -->

            <-- GCAddItemToCodeSheetVerify(ê²°ê³¼ì½”ë“œ, ê´€ë ¨ê°’) ---
   
//-------------+---------------------------------------------
// ì‚¬ìš© packet | client ì²˜ë¦¬
//-------------+---------------------------------------------
// ë¶ˆê°€ packet | ë¶ˆê°€ message ì¶œë ¥
// ì‹¤íŒ¨ packet | ì‹¤íŒ¨ message ì¶œë ¥, ë³„ ì‚­ì œ, [ì•„ì´í…œ ì œê±°]
// ì„±ê³µ packet | ë³„ ì‚­ì œ, optionë³€ê²½
//-------------+---------------------------------------------
*/

#include "Client_PCH.h"
#include "CGAddItemToCodeSheet.h"

#ifdef __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGAddItemToCodeSheetHandler::execute (CGAddItemToCodeSheet* pPacket , Player* pPlayer)
	 
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __GAME_SERVER__

#endif	// __GAME_SERVER__
		
	__END_DEBUG_EX __END_CATCH
}

#endif	// __GAME_SERVER__

#endif
