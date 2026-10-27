//----------------------------------------------------------------------
// OperatorOption.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "OperatorOption.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
OperatorOption*		g_pOperatorOption = NULL;

//----------------------------------------------------------------------
// 
// constructor
//
//----------------------------------------------------------------------
OperatorOption::OperatorOption()
{	
	SetDefault();
}

OperatorOption::~OperatorOption()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

void OperatorOption::SetDefault()
{
	bShowModifyHP			= false;
	bShowCreatureInfo		= false;
	bShowImageObjectInfo	= false;
	bHideAttachEffect		= false;
	bHideSpeechBalloon		= false;
	bShowAddEffect			= false;
	bShowModifyEXP			= false;
	bSlideScreenShot		= false;
	bHideSpeechBalloon		= false;
	bShowSkillRange			= false;
	bShowModifyRANKEXP		= false;
	bRecordCharName			= false;
	bCanRootingGhost		= false;
}
	

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void	
OperatorOption::SaveToFile(const char* filename)
{
}


//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
bool	
OperatorOption::LoadFromFile(const char* filename)
{
	return false;
}
