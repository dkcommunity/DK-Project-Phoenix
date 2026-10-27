//----------------------------------------------------------------------
//
// Filename    : SkillParentInfo.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#include "SkillParentInfo.h"
#include "DB.h"
#include "Assert.h"
//#include <algo.h>

//----------------------------------------------------------------------
//
// constructor
//
//----------------------------------------------------------------------
SkillParentInfo::SkillParentInfo (SkillType_t SkillType) 
	throw () 
: m_SkillType(SkillType)
{
	__BEGIN_TRY
	init();
	__END_CATCH
}


//----------------------------------------------------------------------
//
// destructor
//
//----------------------------------------------------------------------
SkillParentInfo::~SkillParentInfo () 
    throw ()
{
	__BEGIN_TRY
	__END_CATCH
}


//----------------------------------------------------------------------
//
// SkillParentInfo::init()
//
//----------------------------------------------------------------------
void SkillParentInfo::init ()
	throw (Error)
{
	__BEGIN_TRY
	__END_CATCH
}


//----------------------------------------------------------------------
//
// SkillParentInfo::hasParent()
//
// 