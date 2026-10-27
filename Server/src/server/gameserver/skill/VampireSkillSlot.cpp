//////////////////////////////////////////////////////////////////////////////
// Filename    : VampireSkillSlot.cpp
// Written By  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "VampireSkillSlot.h"
#include "DBExecuteManager.h"

#include <sstream>

VampireSkillSlot::VampireSkillSlot()
	: SkillSlot()	
{

}

VampireSkillSlot::VampireSkillSlot(SkillType_t SkillType, ulong Interval, ulong CastingTime)
	: SkillSlot(SkillType, Interval, CastingTime)	
{

}

VampireSkillSlot::~VampireSkillSlot()
{

}

void VampireSkillSlot::create(const string & OwnerID)
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "INSERT INTO VampireSkillSave (OwnerID, SkillType, Delay, CastingTime, NextTime) VALUES ( '" 
		<< OwnerID.c_str() << "', " 
		<< m_SkillType << ", " 
		<< m_Interval << ", " 
		<< m_CastingTime << ", " 
		<< m_runTime.tv_sec << " )";
	
	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());
	
	setDirty(false);

	__END_CATCH
}

void VampireSkillSlot::save(const string & OwnerID)
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "UPDATE VampireSkillSave SET "
		<< "Delay=" << m_Interval 
		<< " WHERE OwnerID='" << OwnerID.c_str() << "' AND SkillType=" << m_SkillType;
	
	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());
	
	setDirty(false);
	
	__END_CATCH
}

