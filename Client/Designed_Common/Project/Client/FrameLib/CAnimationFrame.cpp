//----------------------------------------------------------------------
// CAnimationFrame.cpp
//----------------------------------------------------------------------

#include "CAnimationFrame.h"
#include <fstream>

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CAnimationFrame::CAnimationFrame(BYTE bltType)
{
	m_FrameID		= 0;

	m_MaxFrame		= 0;
	m_CurrentFrame	= 0;

	m_BltType		= bltType;
}

CAnimationFrame::~CAnimationFrame()
{
}


//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void	
CAnimationFrame::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&m_FrameID, SIZE_FRAMEID);
	file.write((const char*)&m_MaxFrame, 1);
	//file.write((const char*)&m_CurrentFrame, 1);
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void	
CAnimationFrame::LoadFromFile(ivfstream& file)
{
	file.read((char*)&m_FrameID, SIZE_FRAMEID);
	file.read((char*)&m_MaxFrame, 1);
	//file.write((const char*)&m_CurrentFrame, 1);
}
