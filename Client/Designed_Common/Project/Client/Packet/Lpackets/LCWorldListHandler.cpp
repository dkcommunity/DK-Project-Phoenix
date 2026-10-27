//----------------------------------------------------------------------
//
// Filename    : LCWorldListHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "client_PCH.h"
#include "LCWorldList.h"
#include "ClientDef.h"
#include "DEServerInformation.h"
#include "DebugInfo.h"
#include "UIFunction.h"

//----------------------------------------------------------------------
// �����κ��� ĳ���� ����Ʈ�� �޾Ҵ�. 
// ���� ĳ���� ���� �������̽��� ������ ���� ���۹��� ���� ���� �־
// �������.
//----------------------------------------------------------------------
void LCWorldListHandler::execute ( LCWorldList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	//-----------------------------------------------------------
	// Server Information �ʱ�ȭ
	//-----------------------------------------------------------
	if (g_pServerInformation==NULL)
	{
		g_pServerInformation = new DEServerInformation;
	}
	else
	{
		g_pServerInformation->Release();
	}

	int currentID = pPacket->getCurrentWorldID();
	
	bool bExistDefault = false;

	//-----------------------------------------------------------
	// Server���� ����
	//-----------------------------------------------------------
	int groupNum = pPacket->getListNum();
	int firstID = 0;

	for (int i=0; i<groupNum; i++)
	{
		WorldInfo* pWorldInfo = pPacket->popFrontListElement();

		if (pWorldInfo!=NULL)
		{
			if( pWorldInfo->getStat() == 0 ||
				pWorldInfo->getStat() == 1 )
			{
				if (i==0)
				{
					firstID = pWorldInfo->getID();
				}

				if (pWorldInfo->getID()==currentID)
				{
					bExistDefault = true;
				}

				//--------------------------------------------------------------
				// ���ο� ServerGroup�� ���� ����
				//--------------------------------------------------------------
				ServerGroup* pNewGroup = g_pServerInformation->GetData( pWorldInfo->getID() );
				
				if (pNewGroup==NULL)
				{
					pNewGroup = new ServerGroup;
					g_pServerInformation->AddData( pWorldInfo->getID(), pNewGroup );
				}

				// Group�� ���� ����			
				pNewGroup->SetGroupName( pWorldInfo->getName().c_str() );
				pNewGroup->SetGroupStatus( (int)pWorldInfo->getStat() );
				
				//--------------------------------------------------------------
				// ������ Server�� ���� ���� ����
				//--------------------------------------------------------------
				/*
				int serverNum = pWorldInfo->getListNum();

				for (int j=0; j<serverNum; j++)
				{
					SubServerInfo* pServerInfo = pWorldInfo->popFrontListElement();
					
					if (pServerInfo!=NULL)
					{
						//--------------------------------------------------------------
						// ServerGroup�� Server �߰�
						//--------------------------------------------------------------
						DESERVER_INFO* pNewServer = new DESERVER_INFO;

						pNewServer->ServerName		= pServerInfo->getServerName().c_str();
						pNewServer->ServerStatus	= (DESERVER_INFO::SERVER_STATUS)pServerInfo->getServerStat();
						
						pNewGroup->AddData( pServerInfo->getServerID(), pNewServer );					

						delete pServerInfo;
					}
					else
					{
						DEBUG_ADD( "[Error] SubServerGroupInfo is NULL" );
					}
				}
				*/

			}

			delete pWorldInfo;
			pWorldInfo = NULL;
		}
		else
		{
			DEBUG_ADD( "[Error] ServerGroupInfo is NULL" );
		}	
	}

	// default ����
	if (currentID==0 || !bExistDefault)
	{
		g_pServerInformation->SetServerGroupID( firstID );
	}
	else
	{
		g_pServerInformation->SetServerGroupID( currentID );
	}

	UI_SetWorldList();

	SetMode( MODE_WAIT_SELECT_WORLD );

#endif
		
	__END_CATCH
}
