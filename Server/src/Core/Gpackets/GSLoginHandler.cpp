//----------------------------------------------------------------------
//
// Filename    : GSLoginHandler.cpp
// Written By  : 
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GSLogin.h"
#include "Assert.h"

#ifdef __SHARED_SERVER__
	#include "GameServerManager.h"
	#include "FriendManager.h"

	#include "Spackets/SGFriendList.h"
	#include "Spackets/SGFriendUpdateConnect.h"
	#include "Spackets/SGFriendWaitList.h"

#endif

//----------------------------------------------------------------------
// 
// GSLoginHandler::execute()
// 
//----------------------------------------------------------------------
void GSLoginHandler::execute ( GSLogin* pPacket, Player* pPlayer )
	 throw ( ProtocolException , Error )
{
	__BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __SHARED_SERVER__

	Assert( pPacket != NULL );
	
	g_pGameServerManager->AddPlayer(pPacket->GetSender(), pPlayer);
	
	g_pFriendManager->LoadFriendList(pPacket->GetSender());
	FriendListManager *pFriendListManager = g_pFriendManager->GetFriendListManager(pPacket->GetSender());
	
	FriendInfo *pFriendInfo;
	
	pFriendInfo = g_pFriendManager->AddFriendInfo(pPacket->GetSender());
	pFriendInfo->SetName(pPacket->GetSender());
	pFriendInfo->SetConnect(true);
	pFriendInfo->SetServerName(pPacket->GetServerName());
	
	TFriendList *pFriendList = &pFriendListManager->GetFriendFWList();
	if ( pFriendList != NULL )
	{
		{
			SGFriendList sgFriendList;
			
			sgFriendList.SetReceiver(pPacket->GetSender());
			
			TFriendList::iterator iter;
			string strFriendName;
			
			FriendInfo *pFriendInfo;
			
			iter = pFriendList->begin();
			for(; iter != pFriendList->end() ; ++iter )
			{
				strFriendName = iter->second;
				
				pFriendInfo = g_pFriendManager->GetFriendInfo(strFriendName);
				
				FriendSimpleInfo friendSimpleInfo;
					
				friendSimpleInfo.SetFriendName(strFriendName);
				friendSimpleInfo.SetConnect(pFriendInfo->IsConnect());
				friendSimpleInfo.SetServerName(pFriendInfo->GetServerName());
				friendSimpleInfo.SetZoneID(pFriendInfo->GetZoneID());
				
				sgFriendList.AddFriendSimpleInfo(friendSimpleInfo);
			}
			
			FriendGlobalOptionInfo info;
			info.SetAllowToRequestAddition(pFriendListManager->GetGlobalOption().GetAllowToRequestAddition());
			
			sgFriendList.SetFriendGlobalOptionInfo(info);
			
			g_pGameServerManager->broadcast( &sgFriendList, pPlayer );
		}
		
		{
			TFriendList::iterator iter;
			string strFriendName;
			Player *pFriendPlayer;
			
			FriendInfo *pFriendInfo;
			pFriendInfo = g_pFriendManager->GetFriendInfo(pPacket->GetSender());
			
			iter = pFriendList->begin();
			for(; iter != pFriendList->end() ; ++iter )
			{
				strFriendName = iter->second;
				
				pFriendPlayer = g_pGameServerManager->GetPlayer(strFriendName);
				if ( pFriendPlayer != NULL )
				{
					SGFriendUpdateConnect sgFriendUpdateConnect;
					
					sgFriendUpdateConnect.SetReceiver(strFriendName);
					sgFriendUpdateConnect.SetPCName(pPacket->GetSender());
					sgFriendUpdateConnect.SetConnect(pFriendInfo->IsConnect());
					sgFriendUpdateConnect.SetServerName(pFriendInfo->GetServerName());
					
					g_pGameServerManager->broadcast( &sgFriendUpdateConnect, pFriendPlayer );
				}
			}
		}
	}
	
	///////////////////////////////////////////////////////////////////////
	
	// 模备 措扁 格废 傈价
	{
		TFriendList *pFriendWaitList = &pFriendListManager->GetFriendWaitList();
		
		SGFriendWaitList sgFriendWaitList;
		
		sgFriendWaitList.SetReceiver(pPacket->GetSender());
		
		TFriendList::iterator iter;
		string strFriendName;
		
		iter = pFriendWaitList->begin();
		for(; iter != pFriendWaitList->end() ; ++iter )
		{
			strFriendName = iter->second;
			
			FriendWaitInfo info;
			
			info.SetFriendName(strFriendName);
			
			sgFriendWaitList.AddFriendWaitInfo(info);
		}
		
		g_pGameServerManager->broadcast( &sgFriendWaitList, pPlayer );
	}

#endif
		
	__END_DEBUG_EX __END_CATCH
}
