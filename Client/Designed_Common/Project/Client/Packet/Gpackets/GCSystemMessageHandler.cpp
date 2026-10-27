//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSystemMessageHandler.cc
// Written By  : elca
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSystemMessage.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "MGameStringTable.h"
#include "Client.h"

extern bool PhoenixAdminOnlinePlayers_HandleProtocolMessage(const char* message);
//////////////////////////////////////////////////////////////////////
//
// 클라이언트에서 서버로부터 메시지를 받았을때 실행되는 메쏘드이다.
//
//////////////////////////////////////////////////////////////////////
void GCSystemMessageHandler::execute ( GCSystemMessage * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
 //  	static char previous1[128] = { NULL, };
	const char* protocolMessage = pPacket->getMessage().c_str();
	if (PhoenixAdminOnlinePlayers_HandleProtocolMessage(protocolMessage))
		return;

	// FIX 2026-05-12: g_pZone NULL guard (login öncesi system message → NULL deref crash).
	if(g_pZone != NULL && g_pZone->GetID() == 1005 && pPacket->getType() != SYSTEM_MESSAGE_EVENT && pPacket->getType() != SYSTEM_MESSAGE_OPERATOR && pPacket->getType() != SYSTEM_MESSAGE_INFO)
		return;
	switch(pPacket->getType())
	{ 
		case SYSTEM_MESSAGE_HOLY_LAND :		// 아담의 성지 관련
			if(g_pUserOption->DoNotShowHolyLandMsg)
				return;
			break;

		case SYSTEM_MESSAGE_NORMAL:
			break;

//		case SYSTEM_MESSAGE_OPERATOR:	// 운영자 말씀
//			break;
	
		case SYSTEM_MESSAGE_MASTER_LAIR:	// 마스터 레어 관련
			if(g_pUserOption->DoNotShowLairMsg)
				return;
			break;

		case SYSTEM_MESSAGE_COMBAT:		// 전쟁 관련
			if(g_pUserOption->DoNotShowWarMsg)
				return;
			break;
	
		case SYSTEM_MESSAGE_INFO: 		// 특정한 정보 관련
			break;
			
		case SYSTEM_MESSAGE_RANGER_CHAT:
			{
				char* message = (char*)pPacket->getMessage().c_str();
				if(NULL != message)
				{
					UI_SetRangerChatString(message);
				}
			}
			return;
		case SYSTEM_MESSAGE_TIMER:
			{
				const char *tok = "[]";

				// FIX 2026-05-12: strtok NULL dönüş guard. Önceden strcpy(szDesc, NULL) / atoi(NULL) crash.
				char szBuf[256];
				strncpy(szBuf, pPacket->getMessage().c_str(), sizeof(szBuf) - 1);
				szBuf[sizeof(szBuf) - 1] = '\0';

				char szDesc[256] = { 0 };
				char* pDescTok = strtok(szBuf, tok);
				if (pDescTok == NULL)
					return;
				strncpy(szDesc, pDescTok, sizeof(szDesc) - 1);
				szDesc[sizeof(szDesc) - 1] = '\0';

				char *szTime = strtok(NULL, tok);
				if (szTime == NULL)
					return;
				DWORD dwSec = atoi(szTime) / 10;

				// Time이 0이면 닫으라는 소리다.
				if(dwSec > 0)
				{
					UI_RunTimeCount(dwSec, szDesc);
				}
				else
				{
					UI_CloseTimeCount();
				}
			}
			return;
		//2008.07.17 zzi
		case SYSTEM_MESSAGE_OPERATOR:	//test용..운영자 말씀을.. EventMessage로 출력해 보자.
			{
				char* message = (char*)pPacket->getMessage().c_str();
				if(NULL != message)
					g_pGMMessage->Add( message );
			}
			return;

		case SYSTEM_MESSAGE_EVENT:
			{
				static char cleaner[512] = { NULL, };
				//일단 무조건.. 밀어넣자.
				char* message = (char*)pPacket->getMessage().c_str();
				if(NULL != message)
				{
					if (!strcmp(message, cleaner))
					{
						g_pEventMessage->Clear();
						cleaner[0] = '\0';
					}
					else
					{
						g_pEventMessage->Add( message );
						strcpy (cleaner, message);
					}
				}
			}
			return;
		case SYSTEM_MESSAGE_PLAYER:	    // add by Coffee 2007-8-2 
			char* message = (char*)pPacket->getMessage().c_str();
			
			if (NULL != message)
			{
			//	message = (char*)pPacket->getMessage().c_str();
					//g_pPlayerMessage->Add( message );
					g_pWorldChatingMessage->Add( message );
			//		strcpy( previous1, pPacket->getMessage().c_str() );
			}
			return;
	}

	static char previous[128] = { NULL, };

	const char* message = pPacket->getMessage().c_str();
	
	std::string strMessage = message;
	int nPos = strMessage.find("@");

	if ( nPos != -1 )
	{
		g_pInfomationBord->Parse(message);
		return;
	}
	//--------------------------------------------------------------------
	// system message에 출력
	//--------------------------------------------------------------------
	if (strcmp(previous, message)==0)
	{
		BOOL bExist = FALSE;

		//--------------------------------------------------------------------
		// 이미 있는 메세지인지 검사한다.
		//--------------------------------------------------------------------
		for (int i=0; i<g_pSystemMessage->GetSize(); i++)
		{
			if (strcmp((*g_pSystemMessage)[i], message)==0)
			{
				bExist = TRUE;
			}
		}

		//--------------------------------------------------------------------
		// 없는거면 추가한다.		
		//--------------------------------------------------------------------
		if (!bExist)
		{
			g_pSystemMessage->Add( message );
		}
	}
	//--------------------------------------------------------------------
	// 새로운 메세지이면 추가한다.
	//--------------------------------------------------------------------
	else
	{
		g_pSystemMessage->Add( message );

		strcpy( previous, pPacket->getMessage().c_str() );
	}

#endif

	__END_CATCH
}
