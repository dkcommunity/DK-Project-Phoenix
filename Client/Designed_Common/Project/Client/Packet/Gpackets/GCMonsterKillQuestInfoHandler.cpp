//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMonsterKillQuestInfoHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMonsterKillQuestInfo.h"
#include "MCreatureTable.h"
#include "MMonsterKillQuestInfo.h"


//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////
void GCMonsterKillQuestInfoHandler::execute(GCMonsterKillQuestInfo *pPacket,
                                            Player *pPlayer)

{
  __BEGIN_TRY

#ifdef __GAME_CLIENT__

  while (!pPacket->empty()) {
    GCMonsterKillQuestInfo::QuestInfo *pInfo = pPacket->popQuestInfo();
    std::string str;
    str = (*g_pCreatureTable)[pInfo->sType].Name;
    g_pQuestInfoManager->SetInfo(pInfo->questID, pInfo->goal, pInfo->timeLimit,
                                 str);
    delete pInfo;
  }

#endif

  __END_CATCH
}
