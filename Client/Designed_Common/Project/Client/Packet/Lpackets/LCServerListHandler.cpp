//----------------------------------------------------------------------
//
// Filename    : LCServerListHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "client_PCH.h"
#include "LCServerList.h"
#include "DEServerInformation.h"
#include "ClientDef.h"
#include "UIFunction.h"

#ifdef __GAME_CLIENT__
	#include "ClientPlayer.h"	
#endif

//----------------------------------------------------------------------
// �����κ��� ĳ���� ����Ʈ�� �޾Ҵ�. 
// ���� ĳ���� ���� �������̽��� ������ ���� ���۹��� ���� ���� �־
// �������.
//----------------------------------------------------------------------
void LCServerListHandler::execute ( LCServerList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__


	//-----------------------------------------------------------
	// Server Information �ʱ�ȭ
	//-----------------------------------------------------------
	if (g_pServerInformation==NULL)
	{
		DEBUG_ADD("[Error] g_pServerInformation is NULL");

		return;
	}

	int currentID = pPacket->getCurrentServerGroupID();
	
	bool bExistDefault = false;

	//-----------------------------------------------------------
	// Server���� ����
	//-----------------------------------------------------------
	int groupID = g_pServerInformation->GetServerGroupID();
	ServerGroup* pServerGroup = g_pServerInformation->GetData( groupID );

	if (pServerGroup!=NULL)
	{
		int groupNum = pPacket->getListNum();
		int firstID = 0;

		for (int i=0; i<groupNum; i++)
		{
			ServerGroupInfo* pServerInfo = pPacket->popFrontListElement();

			if (pServerInfo!=NULL)
			{
				if (i==0)
				{
					firstID = pServerInfo->getGroupID();
				}

				if (pServerInfo->getGroupID()==currentID)
				{
					bExistDefault = true;
				}

				//--------------------------------------------------------------
				// ���ο� ServerGroup�� ���� ����
				//--------------------------------------------------------------
				DESERVER_INFO* pNewServer = pServerGroup->GetData( pServerInfo->getGroupID() );
				
				if (pNewServer==NULL)
				{
					pNewServer = new DESERVER_INFO;
					pServerGroup->AddData( pServerInfo->getGroupID(), pNewServer );
				}

				// Group�� ���� ����
				pNewServer->ServerName		= pServerInfo->getGroupName().c_str();
				pNewServer->ServerStatus	= (int)pServerInfo->getStat();
				pNewServer->ServerNonPK		= pServerInfo->isNonPKServer();

#if __CONTENTS(__SERVER_SELECT_REVEWAL)
				pNewServer->ServerSlayer	= pServerInfo->getSlayerNum();
				pNewServer->ServerVampire	= pServerInfo->getVampireNum();
				pNewServer->ServerOusters	= pServerInfo->getOustersNum();
#endif	// __SERVER_SELECT_REVEWAL			
				delete pServerInfo;
			}
			else
			{
				DEBUG_ADD( "[Error] ServerGroupInfo is NULL" );
			}	
		}

		// default ����		
		if (currentID==0 || !bExistDefault)
		{
			g_pServerInformation->SetServerID( firstID );
		}
		else
		{
			g_pServerInformation->SetServerID( currentID );
		}

		UI_SetServerList();

		SetMode( MODE_WAIT_SELECT_SERVER );
	}
	else
	{
		DEBUG_ADD_FORMAT("[Error] ServerGroup(%d) is NULL", groupID);
	}

#endif
		
	__END_CATCH
}
