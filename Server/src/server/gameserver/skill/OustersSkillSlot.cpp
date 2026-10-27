//////////////////////////////////////////////////////////////////////////////
// Filename    : OustersSkillSlot.cpp
// Written By  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "OustersSkillSlot.h"
#include "DBExecuteManager.h"

#include <sstream>

OustersSkillSlot::OustersSkillSlot()
	: SkillSlot()
{
	m_ExpLevel = 0;
}

OustersSkillSlot::OustersSkillSlot(SkillType_t SkillType, ulong Interval, ulong CastingTime)
: SkillSlot(SkillType, Interval, CastingTime)
{
	m_ExpLevel = 0;
}

OustersSkillSlot::~OustersSkillSlot()
{
	m_ExpLevel = 0;
}

void OustersSkillSlot::create(const string & OwnerID)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "INSERT INTO OustersSkillSave (OwnerID, SkillType, Delay, CastingTime, NextTime, SkillLevel) VALUES ( '" 
		<< OwnerID.c_str() << "', " 
		<< m_SkillType << ", "
		<< m_Interval<< ", " 
		<< m_CastingTime << ", " 
		<< m_runTime.tv_sec << ", " 
		<< m_ExpLevel << " )";
	
	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());

	__END_CATCH
}

void OustersSkillSlot::save(const string & OwnerID)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "UPDATE OustersSkillSave SET "
		<< "SkillLevel=" << m_ExpLevel 
		<< ", Delay=" << m_Interval 
		<< " WHERE OwnerID='" << OwnerID.c_str() << "' AND SkillType=" << m_SkillType;
	
	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());
	
	setDirty(false);

	__END_CATCH
}

void OustersSkillSlot::destroy(const string & OwnerID)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "DELETE FROM OustersSkillSave WHERE OwnerID='" << OwnerID.c_str() << "' AND SkillType=" << m_SkillType;

	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());
	
	setDirty(false);

	__END_CATCH
}

