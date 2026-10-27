/////////////////////////////////////////////////////////////////////////////
// Filename : Mofus.cpp
// Desc		:
/////////////////////////////////////////////////////////////////////////////

#include "Mofus.h"
#include "DB.h"


int loadPowerPoint( const string& name )
{
    __BEGIN_TRY

	int powerpoint = 0;

#ifdef __MOFUS__


    Statement* pStmt = NULL;
    Result* pResult = NULL;

    try
    {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pResult = pStmt->executeQuery( "SELECT Point FROM MofusPowerPoint WHERE OwnerID='%s'", name.c_str() );

        if ( pResult->next() )
        {
            powerpoint = pResult->getInt(1);
        }
    }
    catch( SQLQueryException& sql )
    {
        // SQL 에러는 무시한다.
    }
    catch( ... )
    {
        SAFE_DELETE( pStmt );

        throw;
    }

    SAFE_DELETE( pStmt );

#endif /*  __MOFUS__ */

	return powerpoint;

    __END_CATCH
}

int savePowerPoint( const string& name, int amount )
{
    __BEGIN_TRY

	int powerpoint = 0;

#ifdef __MOFUS__

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    try
    {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQuery( "Update MofusPowerPoint SET Point = Point + %d WHERE OwnerID='%s'", amount, name.c_str() );

        if ( pStmt->getAffectedRowCount() == 0 )
        {
            pStmt->executeQuery( "Insert Into MofusPowerPoint Values ('%s',%d)", name.c_str(), amount );
        }

        pResult = pStmt->executeQuery( "SELECT Point FROM MofusPowerPoint WHERE OwnerID='%s'", name.c_str() );

        if ( pResult->next() )
        {
            powerpoint = pResult->getInt(1);
        }
    }
    catch( SQLQueryException& sql )
    {
        // SQL 에러는 무시한다.
    }
    catch( ... )
    {
        SAFE_DELETE( pStmt );

        throw;
    }

    SAFE_DELETE( pStmt );

#endif /*  __MOFUS__ */

	return powerpoint;

    __END_CATCH
}

void logPowerPoint( const string& name, int recvPoint, int savePoint )
{
#ifdef __MOFUS__
    __BEGIN_TRY

    Statement* pStmt = NULL;

    try
    {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pStmt->executeQuery( "INSERT INTO MofusLog (OwnerID, SaveTime, RecvPoint, SavePoint) VALUES ('%s', now(), %u, %u)",
				name.c_str(), recvPoint, savePoint );
    }
    catch( SQLQueryException& sql )
    {
        // SQL 에러는 무시한다.
    }
    catch( ... )
    {
        SAFE_DELETE( pStmt );

        throw;
    }

    SAFE_DELETE( pStmt );

    __END_CATCH
#endif /*  __MOFUS__ */
}

