//////////////////////////////////////////////////////////////////////////////
// Filename    : ContributeInfo.h 
// Written by  : bigheart
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CONTRIBUTE_INFO_H__
#define __CONTRIBUTE_INFO_H__ 

#include "Types.h"
#include "Exception.h"

#define MAX_CONTRIBUTE_RANK 7
struct ContributeInfoStruct
{
	int m_Rank;
	int m_RankPoint;
};

class ContributeInfo
{
public:
	void load() throw(Error);
	int getRank(int RankPoint);
protected:
	ContributeInfoStruct m_ContributeRank[MAX_CONTRIBUTE_RANK];
};

extern ContributeInfo* g_pContributeInfo;

#endif
