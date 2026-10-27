//----------------------------------------------------------------------
// MMovingEffect.cpp
//----------------------------------------------------------------------

#include "MEffect.h"
#include "MSkipEffect.h"
#include "MTopView.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

MSkipEffect::MSkipEffect(BYTE bltType)
: MEffect(bltType)
{
	m_nSkipValue = 3;
}

MSkipEffect::~MSkipEffect()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------
// m_Count가 0일때까지 -1 해주면서 Frame을 바꾼다.
//----------------------------------------------------------------------
bool
MSkipEffect::Update()
{
	if (g_CurrentFrame < m_EndFrame-5)
	{
		if((rand()%m_nSkipValue))
			SetDrawSkip(true);
		else
			SetDrawSkip(false);
		
		NextFrame();
	
		if (m_BltType == BLT_EFFECT)
		{
			m_Light = g_pTopView->m_EffectAlphaFPK[m_FrameID][m_Direction][m_CurrentFrame].GetLight();
		}

		return true;
	}
	else if(g_CurrentFrame < m_EndFrame*2)
	{
		m_CurrentFrame = m_MaxFrame-5;
		m_PixelX ++;

		return true;
	}

	return false;
}
