//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginPayTypeManager.cpp
// Written by  : rappi76
// Description : 
//
//////////////////////////////////////////////////////////////////////

#include "LoginPayTypeManager.h"
#include "DB.h"

bool LoginPayTypeManager::IsAllow(WorldID_t WorldID, LoginPlayer* pLoginPlayer)
{
	int LoginPayType = LOGIN_PAYTYPE_ALL_ALLLOW;
	
	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection( "DARKEDEN" )->createStatement();
		Result* pResult = pStmt->executeQuery("SELECT PayType FROM LoginPayType WHERE WorldID=%d", WorldID);

		if ( pResult->next() )
		{
			LoginPayType = pResult->getInt(1);
		}
		else
		{
			LoginPayType = 0;
		}

		SAFE_DELETE( pStmt );
	}
	END_DB( pStmt )
	
	switch(LoginPayType)
	{
		case LOGIN_PAYTYPE_ALL_ALLLOW :	
		default :
			return true;
			
		case LOGIN_PAYTYPE_PREMIUM :	
		{
			if ( pLoginPlayer->isPremiumPlay() )
			{
				return true;
			}
			
			return false;
		}
	}
	
	return false;
	
}

LoginPayTypeManager * g_pLoginPayTypeManager = NULL;
