//-----------------------------------------------------------------------------
// MMoneyManager.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MMoneyManager.h"

#include "MHelpManager.h"
#include "MHelpDef.h"
//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
MMoneyManager*		g_pMoneyManager = NULL;

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
MMoneyManager::MMoneyManager()
{
	m_MaxLimit	= 2000000000;		// 20억
	m_Money			= 0;
	m_DianJuan		= 0;
	m_JiFen			= 0;
}

MMoneyManager::MMoneyManager(const MMoneyManager& mm)
{
	m_MaxLimit = mm.m_MaxLimit;
	m_Money = mm.m_Money;
	m_DianJuan = mm.m_DianJuan;
	m_JiFen = mm.m_JiFen;
}

MMoneyManager::~MMoneyManager()
{
}

//-----------------------------------------------------------------------------
//
// member functions
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Set Money
//-----------------------------------------------------------------------------
bool
MMoneyManager::SetMoney(int money)
{
	// 0이하거나 한계를 넘어가면 안된다.
	if (money<0 || money > m_MaxLimit)
	{
		return false; 
	}

	m_Money = money;

	// 2004, 5, 6, sobeit add start - 돈이 10 만이 넘으면 보관함 도움말
#ifdef __GAME_CLIENT__
	static bool CanBuyStorage = false;
	if( false == CanBuyStorage && m_Money>100000 )
	{
		ExecuteHelpEvent(HELP_EVENT_STORAGE_BUY);
		CanBuyStorage = true;
	}
#endif
	//2004, 5, 6 sobeit add end
	return true;
}

//-----------------------------------------------------------------------------
// Add Money
//-----------------------------------------------------------------------------
bool
MMoneyManager::AddMoney(int money)
{
	return SetMoney( m_Money + money );
}

//-----------------------------------------------------------------------------
// Remove Money
//-----------------------------------------------------------------------------
bool
MMoneyManager::UseMoney(int money)
{
	return SetMoney( m_Money - money );
}

//-----------------------------------------------------------------------------
// Can Add Money
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanAddMoney(int money)
{
	int left = m_Money + money;

	if (left > m_MaxLimit)
	{
		return false; 
	}

	return true;
}

//-----------------------------------------------------------------------------
// Can Use Money
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanUseMoney(int money)
{
	int left = m_Money - money;

	if (left<0)
	{
		return false; 
	}

	return true;
}


//--------------------------------------------------DianJuan---------------------------------------------------
//-----------------------------------------------------------------------------
// Set DianJuan
//-----------------------------------------------------------------------------
bool
MMoneyManager::SetDianJuan(int dianjuan)
{
	// 0이하거나 한계를 넘어가면 안된다.
	if (dianjuan<0 || dianjuan > m_MaxLimit)
	{
		return false; 
	}

	m_DianJuan = dianjuan;

	//2004, 5, 6 sobeit add end
	return true;
}

//-----------------------------------------------------------------------------
// Add DianJuan
//-----------------------------------------------------------------------------
bool
MMoneyManager::AddDianJuan(int dianjuan)
{
	return SetDianJuan( m_DianJuan + dianjuan );
}

//-----------------------------------------------------------------------------
// Remove DianJuan
//-----------------------------------------------------------------------------
bool
MMoneyManager::UseDianJuan(int dianjuan)
{
	return SetDianJuan( m_DianJuan - dianjuan );
}

//-----------------------------------------------------------------------------
// Can Add DianJuan
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanAddDianJuan(int dianjuan)
{
	int left = m_DianJuan + dianjuan;

	if (left > m_MaxLimit)
	{
		return false; 
	}

	return true;
}

//-----------------------------------------------------------------------------
// Can Use DianJuan
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanUseDianJuan(int dianjuan)
{
	int left = m_DianJuan - dianjuan;

	if (left<0)
	{
		return false; 
	}

	return true;
}




//--------------------------------------------------JiFen---------------------------------------------------
//-----------------------------------------------------------------------------
// Set JiFen
//-----------------------------------------------------------------------------
bool
MMoneyManager::SetJiFen(int jifen)
{
	// 0이하거나 한계를 넘어가면 안된다.
	if (jifen<0 || jifen > m_MaxLimit)
	{
		return false; 
	}

	m_JiFen = jifen;

	//2004, 5, 6 sobeit add end
	return true;
}

//-----------------------------------------------------------------------------
// Add DianJuan
//-----------------------------------------------------------------------------
bool
MMoneyManager::AddJiFen(int jifen)
{
	return SetJiFen( m_JiFen + jifen );
}

//-----------------------------------------------------------------------------
// Remove DianJuan
//-----------------------------------------------------------------------------
bool
MMoneyManager::UseJiFen(int jifen)
{
	return SetJiFen( m_JiFen - jifen );
}

//-----------------------------------------------------------------------------
// Can Add DianJuan
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanAddJiFen(int jifen)
{
	int left = m_JiFen + jifen;

	if (left > m_MaxLimit)
	{
		return false; 
	}

	return true;
}

//-----------------------------------------------------------------------------
// Can Use DianJuan
//-----------------------------------------------------------------------------
bool		
MMoneyManager::CanUseJiFen(int jifen)
{
	int left = m_JiFen - jifen;

	if (left<0)
	{
		return false; 
	}

	return true;
}
