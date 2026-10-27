//-----------------------------------------------------------------------------
// MMoneyManager.h
//-----------------------------------------------------------------------------
// Money는 MoneyLimit를 넘으면 안된다.
//-----------------------------------------------------------------------------

#ifndef __MMONEYMANAGER_H__
#define __MMONEYMANAGER_H__

class MMoneyManager {

	public :
		MMoneyManager();
		MMoneyManager(const MMoneyManager& mm); 	
		~MMoneyManager();

		//-------------------------------------------------------
		// Money
		//-------------------------------------------------------		
		bool		SetMoney(int money);
		int			GetMoney() const		{ return m_Money; }

		bool		AddMoney(int money);
		bool		UseMoney(int money);

		// money만큼 돈을 add/use할 수 있나?
		bool		CanAddMoney(int money);
		bool		CanUseMoney(int money);

		//-------------------------------------------------------
		// DianJuan
		//-------------------------------------------------------		
		bool		SetDianJuan(int money);
		int			GetDianJuan() const		{ return m_DianJuan; }

		bool		AddDianJuan(int money);
		bool		UseDianJuan(int money);

		// money만큼 돈을 add/use할 수 있나?
		bool		CanAddDianJuan(int money);
		bool		CanUseDianJuan(int money);

		//-------------------------------------------------------
		// JiFen
		//-------------------------------------------------------		
		bool		SetJiFen(int money);
		int			GetJiFen() const		{ return m_JiFen; }

		bool		AddJiFen(int money);
		bool		UseJiFen(int money);

		// money만큼 돈을 add/use할 수 있나?
		bool		CanAddJiFen(int money);
		bool		CanUseJiFen(int money);

		//-------------------------------------------------------		
		// Money,DianJuan,JiFen Limit
		//-------------------------------------------------------		
		void		SetMoneyLimit(int limit)	{ m_MaxLimit = limit; }
		int			GetMoneyLimit() const		{ return m_MaxLimit;}
		int			GetMaxAddMoney() const		{ return m_MaxLimit - m_Money; }
		int			GetMaxAddDianJuan() const		{ return m_MaxLimit - m_DianJuan; }

	protected :
		int			m_MaxLimit;	// 가질 수 있는 돈의 한계
		int			m_Money;		// 돈 - -;
		int			m_DianJuan; //듐얩
		int			m_JiFen;
};

extern MMoneyManager*		g_pMoneyManager;

#endif

