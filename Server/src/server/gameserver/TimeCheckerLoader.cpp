///////////////////////////////////////////////////////////////
// Filename		: TimeCheckerLoader.cpp
// Written by	: rappi76
// Description	:
///////////////////////////////////////////////////////////////

#include "TimeCheckerLoader.h"
#include "DB.h"

#include <stdio.h>

void TimeCheckerLoader::Load(TimeChecker *pTimeChecker)
{
	Statement* pStmt = NULL;
	Result*    pResult = NULL;
		
	pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
	pResult = pStmt->executeQuery(
			"SELECT TimePeriodType, StartTimeH, StartTimeM, EndTimeH, EndTimeM from TimePeriodSchdule");

	int iTimePeriodType;
	int iStartTimeH;
	int iStartTimeM;
	int iEndTimeH;
	int iEndTimeM;
	
	while( pResult->next() )
	{
		iTimePeriodType = pResult->getInt(1);
		
		iStartTimeH = pResult->getInt(2);
		iStartTimeM = pResult->getInt(3);
		
		iEndTimeH = pResult->getInt(4);
		iEndTimeM = pResult->getInt(5);
		
		pTimeChecker->addPeriod( 
			iTimePeriodType, 
			TimePeriod( 
				VSTime(iStartTimeH, iStartTimeM), 
				VSTime(iEndTimeH, iEndTimeM) 
			) 
		);
		
		printf("Add TimePeriod : %d %d %d %d %d\n", iTimePeriodType, iStartTimeH, iStartTimeM, iEndTimeH, iEndTimeM);
	}
}

