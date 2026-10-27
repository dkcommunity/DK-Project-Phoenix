//-----------------------------------------------------------------------------
// CGameUpdate.h
//-----------------------------------------------------------------------------
// Game Loop
//-----------------------------------------------------------------------------

#ifndef	__CGAMEUPDATE_H__
#define	__CGAMEUPDATE_H__

#include "CWinUpdate.h"
#include "CDirectInput.h"
#include "TickTimer.h"

class CGameUpdate : public CWinUpdate {
	public :
		CGameUpdate();
		~CGameUpdate() {}

		void		Init();

		void		Update();	

		void		UpdateDraw();
		bool		GetForceAttack()					{ return m_bForceAttack; }
		void		SetForceAttack(bool bForceAttack)	{ m_bForceAttack = bForceAttack; }

	protected :
		void		ProcessInput();		
		void		UpdateDrawHelp();
		
		static void	DXMouseEvent(CDirectInput::E_MOUSE_EVENT event, int x, int y, int z);
		static void	DXKeyboardEvent(CDirectInput::E_KEYBOARD_EVENT, DWORD);

	private:
		bool		m_bForceAttack;
		CTickTimer	m_userGraddisplay;
};

extern CGameUpdate*		g_pCGameUpdate;

#endif
