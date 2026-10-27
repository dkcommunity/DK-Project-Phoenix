//////////////////////////////////////////////////////////////////////////////
// Filename    : SlayerSkillSlot.cpp
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SlayerSkillSlot.h"
#include "DBExecuteManager.h"

#include <sstream>

SlayerSkillSlot::SlayerSkillSlot()
	: SkillSlot()
{
	m_Exp = 0;
	m_ExpLevel = 0;
	m_Enable = true;
}

SlayerSkillSlot::SlayerSkillSlot(SkillType_t SkillType, DWORD Exp, ulong Interval)
	: SkillSlot(SkillType, Interval, 0)
{
	m_Exp = Exp;
	m_ExpLevel = 0;
	m_Enable = true;
}

SlayerSkillSlot::~SlayerSkillSlot()
{
	m_Exp = 0;
	m_ExpLevel = 0;
}

void SlayerSkillSlot::create(const string & OwnerID)
	throw(Error)
{
	__BEGIN_TRY

	ostringstream os;
	
	os << "INSERT INTO SkillSave (OwnerID , SkillType , SkillLevel , SkillExp , Delay , CastingTime , NextTime) "
		<< "VALUES ( '" 
		<< OwnerID.c_str() << "', " 
		<< m_SkillType << ", " 
		<< m_ExpLevel << ", " 
		<< m_Exp << ", " 
		<< m_Interval << ", " 
		<< m_CastingTime<< ", " 
		<< m_runTime.tv_sec<< " )";
	
	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());
	
	setDirty(false);
	
	__END_CATCH
}

void SlayerSkillSlot::save(const string & OwnerID)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "UPDATE SkillSave SET " 
		<< "SkillLevel=" << m_ExpLevel
		<< ", SkillExp=" << m_Exp
		<< ", Delay=" << m_Interval
		<< " WHERE OwnerID='" << OwnerID.c_str() << "' AND SkillType=" << m_SkillType;
	
	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());
	
	setDirty(false);

	__END_CATCH
}

//void SlayerSkillSlot::save()
//	throw(Error)
//{
//	__BEGIN_TRY
//	
////	ostringstream os;
////	
////	os <<  "UPDATE SkillSave SET " 
////		<< "SkillLevel=" << m_ExpLevel
////		<< ", SkillExp=" << m_Exp
////		<< ", Delay=" << m_Interval
////		<< " WHERE OwnerID='" << m_Name.c_str() << "' AND SkillType=" << m_SkillType;
////	
////	g_GameDBExecuteManager.RequestExecQuery(m_SkillType, os.str());
////	
////	setDirty(false);
//	
//	save(m_Name);
//
//	__END_CATCH
//}
