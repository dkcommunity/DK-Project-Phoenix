//////////////////////////////////////////////////////////////////////////////
// Filename    : CLRegisterPlayerHandler.cpp
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CLRegisterPlayer.h"

#ifdef __LOGIN_SERVER__
	#include "LoginPlayer.h"
	#include "Assert.h"
	#include "DB.h"
    #include "GameServerGroupInfoManager.h"
	#include "Utility.h"

	#include "Lpackets/LCRegisterPlayerError.h"
	#include "Lpackets/LCRegisterPlayerOK.h"
#endif

#ifdef __LOGIN_SERVER__
namespace {

const unsigned int PHOENIX_REGISTER_ID_MIN = 4;
const unsigned int PHOENIX_REGISTER_ID_MAX = 8;
const unsigned int PHOENIX_REGISTER_PASSWORD_MIN = 6;
const unsigned int PHOENIX_REGISTER_PASSWORD_MAX = 8;

bool isPhoenixRegisterAlpha(char ch) throw()
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool isPhoenixRegisterDigit(char ch) throw()
{
	return ch >= '0' && ch <= '9';
}

bool isPhoenixRegisterAccountChar(char ch) throw()
{
	return isPhoenixRegisterAlpha(ch) || isPhoenixRegisterDigit(ch);
}

bool isPhoenixRegisterIDSafe(const string& value) throw()
{
	if (value.size() < PHOENIX_REGISTER_ID_MIN || value.size() > PHOENIX_REGISTER_ID_MAX)
		return false;
	if (!isPhoenixRegisterAlpha(value[0]))
		return false;
	for (string::const_iterator itr = value.begin(); itr != value.end(); ++itr)
	{
		if (!isPhoenixRegisterAccountChar(*itr))
			return false;
	}
	return true;
}

bool isPhoenixRegisterPasswordSafe(const string& value) throw()
{
	if (value.size() < PHOENIX_REGISTER_PASSWORD_MIN || value.size() > PHOENIX_REGISTER_PASSWORD_MAX)
		return false;
	for (string::const_iterator itr = value.begin(); itr != value.end(); ++itr)
	{
		if (!isPhoenixRegisterAccountChar(*itr))
			return false;
	}
	return true;
}

}
#endif


void CLRegisterPlayerHandler::execute (CLRegisterPlayer* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
		
#ifdef __LOGIN_SERVER__

	Assert(pPacket != NULL);
	Assert(pPlayer != NULL);

	__BEGIN_DEBUG

	LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

	cout << "Registering Player... " << endl;

	LCRegisterPlayerError lcRegisterPlayerError;

	try {



		if (pPacket->getID() == "") {
			lcRegisterPlayerError.setErrorID(EMPTY_ID);	
			throw string("ID field is empty");
		}

		if (pPacket->getID().size() < 4) {
			lcRegisterPlayerError.setErrorID(SMALL_ID_LENGTH);
			throw string("too small ID length");
		}

		if (!isPhoenixRegisterIDSafe(pPacket->getID())) {
			if (pPacket->getID().size() > PHOENIX_REGISTER_ID_MAX)
				lcRegisterPlayerError.setErrorID(SMALL_ID_LENGTH);
			else
				lcRegisterPlayerError.setErrorID(INVALID_ID_PASSWORD);
			throw string("invalid ID format");
		}

		if (pPacket->getPassword() == "")
		{
			lcRegisterPlayerError.setErrorID(EMPTY_PASSWORD);
			throw string("Password field is empty");
		}
		else
		{
			string password = pPacket->getPassword();
			if (password.find("\'") < password.size()) { lcRegisterPlayerError.setErrorID(INVALID_ID_PASSWORD); throw string("Invalid Password"); }
			if (password.find("'") < password.size()) { lcRegisterPlayerError.setErrorID(INVALID_ID_PASSWORD); throw string("Invalid Password"); }
			else if (password.find("\\") < password.size()) { lcRegisterPlayerError.setErrorID(INVALID_ID_PASSWORD); throw string("Invalid Password"); }
			else if (password.find("\"") < password.size()) { lcRegisterPlayerError.setErrorID(INVALID_ID_PASSWORD); throw string("Invalid Password"); }
			else if (password.find(";") < password.size()) { lcRegisterPlayerError.setErrorID(INVALID_ID_PASSWORD); throw string("Invalid Password"); }
		}

		if (pPacket->getPassword().size() < 6) {
			lcRegisterPlayerError.setErrorID(SMALL_PASSWORD_LENGTH);
			throw string("too small password length");
		}

		if (!isPhoenixRegisterPasswordSafe(pPacket->getPassword())) {
			if (pPacket->getPassword().size() > PHOENIX_REGISTER_PASSWORD_MAX)
				lcRegisterPlayerError.setErrorID(SMALL_PASSWORD_LENGTH);
			else
				lcRegisterPlayerError.setErrorID(INVALID_ID_PASSWORD);
			throw string("invalid password format");
		}

	} catch (string & errstr) {

		pLoginPlayer->sendPacket(&lcRegisterPlayerError);


		throw DisconnectException(lcRegisterPlayerError.toString());
	}
		



	Statement* pStmt = NULL;
	Result* pResult = NULL;
	string escapedID;
	bool playerInserted = false;
	bool userInfoInserted = false;

	try {

		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		escapedID               = ReplaceDBString(pPacket->getID());
		string escapedPassword  = ReplaceDBString(pPacket->getPassword());
		string registerIP = pLoginPlayer->getSocket() != NULL ? pLoginPlayer->getSocket()->getHost() : "";
		if (registerIP.empty())
		{
			lcRegisterPlayerError.setErrorID(ETC_ERROR);
			throw SQLQueryException("register IP is empty");
		}
		string escapedRegisterIP = ReplaceDBString(registerIP);

		pResult = pStmt->executeQuery("SELECT PlayerID FROM Player WHERE PlayerID = '%s'" , escapedID.c_str());

		if (pResult->getRowCount() != 0) {
			lcRegisterPlayerError.setErrorID(ALREADY_REGISTER_ID);
			throw DuplicatedException("db 11111111");
		}

		pResult = pStmt->executeQuery(
			"INSERT INTO Player (PlayerID , Password , oldplayerid , RegisterIP) VALUES ('%s' , MD5('%s') , '%s' , '%s')" ,
			escapedID.c_str() ,
			escapedPassword.c_str() ,
			escapedID.c_str() ,
			escapedRegisterIP.c_str()
		);
				
		Assert(pResult == NULL);
		Assert(pStmt->getAffectedRowCount() == 1);
		playerInserted = true;

		pResult = pStmt->executeQuery(
			"INSERT INTO userinfo (username, havecount, havejf, paymoney, zbmoney, regip, password) VALUES ('%s', 0, 0, 0, 0, '%s', '')",
			escapedID.c_str(),
			escapedRegisterIP.c_str()
		);
		Assert(pResult == NULL);
		if (pStmt->getAffectedRowCount() != 1)
		{
			lcRegisterPlayerError.setErrorID(ETC_ERROR);
			throw SQLQueryException("userinfo insert failed");
		}
		userInfoInserted = true;

		pResult = pStmt->executeQuery("SELECT CurrentWorldID, CurrentServerGroupID FROM Player WHERE PlayerID = '%s'" , escapedID.c_str());

		if (pResult->getRowCount() == 0) {
			lcRegisterPlayerError.setErrorID(ETC_ERROR);
			throw SQLQueryException("no sever");
		}

		WorldID_t 		 WorldID = 0;
		ServerGroupID_t  ServerGroupID = 0;

		if (!pResult->next()) throw SQLQueryException("no word.");

		WorldID = pResult->getInt(1);
		ServerGroupID = pResult->getInt(2);

		pLoginPlayer->setServerGroupID(ServerGroupID);

		LCRegisterPlayerOK lcRegisterPlayerOK;

		lcRegisterPlayerOK.setGroupName(g_pGameServerGroupInfoManager->getGameServerGroupInfo(ServerGroupID, WorldID)->getGroupName());

		lcRegisterPlayerOK.setAdult(true);

		pLoginPlayer->sendPacket(&lcRegisterPlayerOK);


		pLoginPlayer->setID(pPacket->getID());


		pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);

		SAFE_DELETE(pStmt);
	}
	catch (DuplicatedException & de)
	{
		SAFE_DELETE(pStmt);


		pLoginPlayer->sendPacket(&lcRegisterPlayerError);


		uint nFailed = pLoginPlayer->getFailureCount();

		//cout << pLoginPlayer->getID() << "'s Failure Count = " << ++nFailed << endl;

		if (nFailed > 3)
			throw DisconnectException("too many failure");

		pLoginPlayer->setFailureCount(nFailed);


		pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_REGISTER_PLAYER);

	}
	catch (SQLQueryException & sqe)
	{
		SAFE_DELETE(pStmt);

		if (playerInserted && !escapedID.empty())
		{
			Statement* pCleanupStmt = NULL;
			try
			{
				pCleanupStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
				if (userInfoInserted)
				{
					pCleanupStmt->executeQuery("DELETE FROM userinfo WHERE username='%s'", escapedID.c_str());
				}
				pCleanupStmt->executeQuery("DELETE FROM Player WHERE PlayerID='%s'", escapedID.c_str());
				SAFE_DELETE(pCleanupStmt);
			}
			catch (SQLQueryException& cleanupException)
			{
				SAFE_DELETE(pCleanupStmt);
				filelog("MarketCurrencyError.log", "[CLRegisterPlayerHandler] cleanup failed player=%s error=%s", escapedID.c_str(), cleanupException.toString().c_str());
			}
		}

		lcRegisterPlayerError.setErrorID(ETC_ERROR);

		pLoginPlayer->sendPacket(&lcRegisterPlayerError);


		uint nFailed = pLoginPlayer->getFailureCount();

		//cout << pLoginPlayer->getID() << "'s Failure Count = " << ++nFailed << endl;

		if (nFailed > 3) 
			throw DisconnectException("too many failure");

		pLoginPlayer->setFailureCount(nFailed);


		pLoginPlayer->setPlayerStatus(LPS_WAITING_FOR_CL_REGISTER_PLAYER);

	}

	__END_DEBUG

#endif
		
	__END_DEBUG_EX __END_CATCH
}
