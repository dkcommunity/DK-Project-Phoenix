//////////////////////////////////////////////////////////////////////////////
// Filename    : RankGem.cpp
// Written by  :
// Description :
//////////////////////////////////////////////////////////////////////////////


#include "NewRankGem.h"
#include "Exception.h"


NewRankGem::NewRankGem()
	throw()
{
	__BEGIN_TRY

	__END_CATCH
}

NewRankGem::NewRankGem( DWORD type, OptionClass optionclass, DWORD grade )
	throw()
{
	__BEGIN_TRY

	m_Type = type;
	m_OptionClass = optionclass;
	m_Grade = grade;

	__END_CATCH
}

NewRankGem::~NewRankGem()
	throw()
{
	__BEGIN_TRY

	__END_CATCH
}

