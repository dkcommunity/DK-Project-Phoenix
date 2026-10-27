////////////////////////////////////////////////////////////////////////////////
// Filename    : ActionAttendCheck.cpp
// Written By  : 
// Description :
////////////////////////////////////////////////////////////////////////////////
#include "ActionAttendCheck.h"
#include "Item.h"
#include "Slayer.h"
#include "Vampire.h"
#include "NPC.h"
#include "PlayerCreature.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "ItemFactoryManager.h"
#include "ItemUtil.h"
#include "ItemInfoManager.h"
#include "OptionInfo.h"
#include "PacketUtil.h"
#include "Zone.h"
#include "VariableManager.h"
#include "NicknameBook.h"
#include "FlagSet.h"
#include "DB.h"
#include "item/PetItem.h"

#include "Gpackets/GCNPCResponse.h"
#include "Gpackets/GCNPCAsk.h"

#include <list>

////////////////////////////////////////////////////////////////////////////////
// ActionAttendCheck
////////////////////////////////////////////////////////////////////////////////
ActionAttendCheck::ActionAttendCheck()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
ActionAttendCheck::~ActionAttendCheck()
{
}

////////////////////////////////////////////////////////////////////////////////
// 
////////////////////////////////////////////////////////////////////////////////
void ActionAttendCheck::read(PropertyBuffer & propertyBuffer)
{
    __BEGIN_TRY

	try
	{
	}
	catch (NoSuchElementException & nsee)
	{
		throw Error(nsee.toString());
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////////////
// 액션을 실행한다.
////////////////////////////////////////////////////////////////////////////////
void ActionAttendCheck::execute(Creature * pCreature1 , Creature * pCreature2)
{
	__BEGIN_TRY

	Assert( pCreature1 != NULL );
	Assert( pCreature2 != NULL );
	Assert( pCreature1->isNPC() );
	Assert( pCreature2->isPC() );

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature2);
	Assert( pPC != NULL );
		
	Player* pPlayer = pPC->getPlayer();
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);	
	Assert( pPlayer != NULL );

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		// 게임디비쪽으로 접속을 하는 것이다. getDisConnection은 로그인디비다.
		pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
		Result* pResult = pStmt->executeQuery( "SELECT CheckDate, CheckCount FROM AttendCheck WHERE PlayerID = '%s'",
				pPlayer->getID().c_str() );

		// Result 값이 존재 하면 대상자 인걸로 간주 하여 출석 체크 하는 함수로 들어가게 해준다.
		// 위의 함수로 들어 갔을 경우에 CheckDate를 보고 같은 날인지를 체크하여 아닐 경우에만 CheckDate와 CheckCount를 증가 하고 디비에 저장한다.
		// 아닐 경우는 해당 경고 메시지를 클라이언트에게 보내준 후 QUIT하여 대화창을 닫아 준다.
		//	 NPC_RESPONSE_DENY_ATTEND,                          // 이벤트 아이템이 부족합니다
		//   NPC_RESPONSE_TODAY_NOT_ATTEND,                       // 아이템이 부족합니다.
		//	 NPC_RESPONSE_CHECK_COUNT_ATTEND					
		if(pResult->next())
		{
			const string DBCheckDate = pResult->getString(1);
			VSDateTime checkTime = VSDateTime(DBCheckDate);
			VSDateTime currentTime = VSDateTime::currentDateTime();
			checkTime.setTime(VSTime(0,0,0,0));
			currentTime.setTime(VSTime(23,59,59,0));
//			cout << "checkTime daysTo" << checkTime.daysTo(currentTime) << endl;
			if(checkTime.daysTo(currentTime) == 0)
			{
				GCNPCResponse	response;
				response.setCode( NPC_RESPONSE_TODAY_NOT_ATTEND );
				pPlayer->sendPacket( &response );
//				cout << "이미 출석했다. ID : " << pGamePlayer->getID().c_str() << endl;
			}
			else
			{
				int checkCount = pResult->getInt(2);
				checkCount++;
				pResult = pStmt->executeQuery( "UPDATE AttendCheck SET CheckDate = now(), CheckCount = %d WHERE PlayerID = '%s'",
					checkCount, pGamePlayer->getID().c_str() );

				GCNPCResponse	response;
				response.setCode( NPC_RESPONSE_CHECK_COUNT_ATTEND );
				response.setParameter( checkCount );				
				pPlayer->sendPacket( &response );
//				cout << "출석!!!. ID : " << pGamePlayer->getID().c_str() << "checkDate : " << checkTime.date().toString().c_str() << 
//					"checkCount : " << checkCount << endl;
			}
		}
		else
		{
			GCNPCResponse	response;
			response.setCode( NPC_RESPONSE_DENY_ATTEND );
			pPlayer->sendPacket( &response );
//			cout << "신규 유저가 아니다. ID : " << pGamePlayer->getID().c_str() << endl;
			
		}
		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////////////
// get debug string
////////////////////////////////////////////////////////////////////////////////
string ActionAttendCheck::toString () const
{
	__BEGIN_TRY

	StringStream msg;
	msg << "ActionAttendCheck("
	    << ")";
	return msg.toString();

	__END_CATCH
}
