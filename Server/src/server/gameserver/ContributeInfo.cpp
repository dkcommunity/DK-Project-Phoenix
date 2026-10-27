//////////////////////////////////////////////////////////////////////////////
// Filename    : ContributeInfo.cpp 
// Written by  : bigheart
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ContributeInfo.h"
#include "DB.h"

void ContributeInfo::load()
	throw(Error)
{
	Statement* pStmt;

	BEGIN_DB 
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery(
			"SELECT `Rank`, RankPoint FROM ContributeInfo ORDER BY `Rank` ASC"
			);
		int nCount = 0;
		while (pResult->next()) 
		{
			uint i = 0;
			int Rank, RankPoint;
			Rank = pResult->getInt(++i);
			RankPoint = pResult->getInt(++i);
			m_ContributeRank[nCount].m_Rank = Rank;
			m_ContributeRank[nCount].m_RankPoint = RankPoint;			
			nCount++;
		}
		if(MAX_CONTRIBUTE_RANK != nCount)
			throw Error("MAX_CONTRIBUTE_RANK != nCount");

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
		
}

int ContributeInfo::getRank(int RankPoint)
{
	for(int i = 0; i < MAX_CONTRIBUTE_RANK; i++)
	{
		if(m_ContributeRank[i].m_RankPoint <= RankPoint)
			return m_ContributeRank[i].m_Rank;
		else
			continue;
	}	
	return 0;
}

ContributeInfo* g_pContributeInfo = NULL;