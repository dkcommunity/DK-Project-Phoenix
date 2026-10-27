//////////////////////////////////////////////////////////////////////////////
// Filename    : CLQueryCharacterNameHandler.cpp
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CLQueryCharacterName.h"

#ifdef __LOGIN_SERVER__
	#include "LoginPlayer.h"

	#include "DatabaseManager.h"
	#include "Connection.h"
	#include "Statement.h"
	#include "Result.h"
	#include "Assert.h"
	#include "GameWorldInfoManager.h"

	#include "Lpackets/LCQueryResultCharacterName.h"
	#include "Utility.h"
#endif

bool isAvailableID(const char* pID);

//////////////////////////////////////////////////////////////////////////////
// DB 로부터 특정 플레이어 아이디를 찾아서 그 여부를 클라이언트로 리턴해준다.
//////////////////////////////////////////////////////////////////////////////
void CLQueryCharacterNameHandler::execute (CLQueryCharacterName* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __LOGIN_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

	WorldID_t WorldID = pLoginPlayer->getWorldID();

	Assert( WorldID <= g_pGameWorldInfoManager->getSize() );
	
	string connectIP = pLoginPlayer->getSocket()->getHost();
	string strCharacterName = pPacket->getCharacterName();
	
    // SQL Injection 방어
    if ( CheckSQLInjectionString(strCharacterName) )
    {
        filelog("Attack_SQLInjection.log", "%s CLQueryCharacterName : [PlayerID=%s CharacterName=%s]",
       		connectIP.c_str(),
       		pPlayer->getID().c_str(), 
       		strCharacterName.c_str()
		);
        
        return;
    }

	Statement* pStmt = NULL;

	try {

		pStmt = g_pDatabaseManager->getConnection( WorldID )->createStatement();

		Result* pResult = pStmt->executeQuery(
			"SELECT Name FROM Slayer WHERE Name = '%s'" , 
			pPacket->getCharacterName().c_str() 
		); 

		bool bExist = false;

		if (pResult->getRowCount() == 0)
		{
			bExist = false;
		}
		else
		{
			bExist = true;
		}

#ifdef __METRO_SERVER__
		// 서버 통합관련 임시 땜빵코드
		// 트랜실이 아닐경우 서버통합신청페이지에서도 새 캐릭터이름을 검사한다.
		if ( !bExist && WorldID != 2 )
		{
			pResult = pStmt->executeQuery( "SELECT FixName FROM NameChange WHERE FixName = '%s'", pPacket->getCharacterName().c_str() );
			
			if ( pResult->getRowCount() != 0 )
			{
				bExist = true;
			}

/*			pResult = pStmt->executeQuery( "SELECT NewName FROM ChangeWorld WHERE NewName = '%s'", pPacket->getCharacterName().c_str() );
			
			if ( pResult->getRowCount() != 0 )
			{
				bExist = true;
			}

			if ( !bExist )
			{
				pResult = pStmt->executeQuery( "SELECT NewName FROM WebTemp WHERE NewName = '%s'", pPacket->getCharacterName().c_str() );

				if ( pResult->getRowCount() != 0 )
				{
					bExist = true;
				}
			}*/
		}
#endif

		LCQueryResultCharacterName lcQueryResultCharacterName;
		lcQueryResultCharacterName.setCharacterName(pPacket->getCharacterName());
		lcQueryResultCharacterName.setExist( bExist );

		//--------------------------------------------------------------------------------
		// 운영자와 관련된 이름은 아닌지 검증한다.
		//--------------------------------------------------------------------------------
		// by sigi
		if (!isAvailableID(pPacket->getCharacterName().c_str()))
		{
			lcQueryResultCharacterName.setExist(true);
		}

		/*
		string text = pPacket->getCharacterName();

		list<string> InvalidTokenList;
		InvalidTokenList.push_back("관리자");
		InvalidTokenList.push_back("도우미");
		InvalidTokenList.push_back("담당자");
		InvalidTokenList.push_back("운영");
		InvalidTokenList.push_back("기획자");
		InvalidTokenList.push_back("개발자");
		InvalidTokenList.push_back("테스터");
		InvalidTokenList.push_back("직원");
		list<string>::const_iterator itr = InvalidTokenList.begin();
		for (; itr != InvalidTokenList.end(); itr++)
		{
			if (text.find(*itr) != string::npos)
			{
				lcQueryResultCharacterName.setExist(false);
			}
		}
		*/

		//cout << "exist..." << endl;
		
		pLoginPlayer->sendPacket(&lcQueryResultCharacterName);

		// 쿼리가 여러 번 올 수 있으므로, 여전히 LWFRP 이다.
		// 문제는 누군가가 프로그램을 짜서 DOS 공격을 할 수 있으므로,
		// 최소 시간을 둬야 한다는 점이다.
		pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);

		SAFE_DELETE(pStmt);
		//delete pStmt;		// 2002.1.16 by sigi
	} 
	catch (SQLQueryException & sqe) 
	{
		SAFE_DELETE(pStmt);
		throw;
	}
	
#endif
		
	__END_DEBUG_EX __END_CATCH
}
