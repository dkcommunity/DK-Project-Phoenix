//-----------------------------------------------------------------------------
// CGameUpdate.h
//-----------------------------------------------------------------------------
// Game Loop
//-----------------------------------------------------------------------------

#ifndef	__CGAMEHOOK_H__
#define	__CGAMEHOOK_H__

#include "CDirectInput.h"
#include "TickTimer.h"

class MPlayer;
class MObject;

class CGameHook
{
	public :
		CGameHook();
		~CGameHook() {}

		void		Init(MPlayer* pMainPlayer);
		void		Update();

	protected:
		MObject* FindNearMonster();  //Ѱ�Ҹ����Ĺ�
		void     AttackMonster(MObject* pMonster);


	private:
		bool		m_bForceAttack;
		CTickTimer	m_userGraddisplay;
		MPlayer*	m_pPlayer;
};

extern CGameHook*		g_pCGameHook;

#endif
