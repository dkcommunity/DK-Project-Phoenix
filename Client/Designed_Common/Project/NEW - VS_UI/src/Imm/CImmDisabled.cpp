#include "client_PCH.h"

#include "CImm.h"

// TouchSense/Immersion artik kullanilmiyor. Orijinal CImm.cpp kaynak dosyasi
// korunur, fakat modern build bu no-op implementasyonu linkler.
CImm::CImm()
{
	m_bPlay = false;
	m_pDevice = NULL;
	m_ProjectAction = NULL;
	m_ProjectSkill = NULL;
	m_ProjectInventory = NULL;
	m_ProjectUseItem = NULL;
}

CImm::~CImm()
{
}

void CImm::Enable()
{
	m_bPlay = false;
}

void CImm::Disable()
{
	m_bPlay = false;
}

void CImm::ForceUI(const unsigned int) const
{
}

void CImm::ForceAction(const int) const
{
}

void CImm::ForceSkill(const int) const
{
}

void CImm::ForceInventory(const int) const
{
}

void CImm::ForceUseItem(const int) const
{
}
