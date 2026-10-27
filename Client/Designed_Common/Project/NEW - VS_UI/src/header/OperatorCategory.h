/*-----------------------------------------------------------------------------

	OperatorCategory.h

	Operator Interface Category.

	2007.02.06 chyaya

-----------------------------------------------------------------------------*/

#ifndef __OPERATORCATEGORY_H__
#define __OPERATORCATEGORY_H__							

#include "VS_UI_Base.h"
#include "VS_UI_widget.h"

//-----------------------------------------------------------------------------
// COperatorCategory
//
//-----------------------------------------------------------------------------
class COperatorCategory : public CRect, public Exec, public ButtonVisual
{
public:
							COperatorCategory(LPCTSTR categoryName, Window* parentWindow, const CRect& position);
	virtual					~COperatorCategory() {}


public:
	virtual void			Start() = 0;
	virtual void			Finish() = 0;

	virtual void			ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button) = 0;
	virtual void			ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button) = 0;

	virtual void			Run(id_t id) = 0;
	virtual void			Process() = 0;

	virtual void			Show() = 0;
	
	virtual bool			MouseControl(UINT message, int _x, int _y) = 0;		

			LPCTSTR			GetCategoryName()				{ return m_CategoryName.c_str();	}

			const Window*	GetParentWindowPtr()			{ return m_pParentWindow;			}

private:	
			const Window*	m_pParentWindow;
			std::string		m_CategoryName;
};


//-----------------------------------------------------------------------------
// COperatorCategoryUtility
//
//-----------------------------------------------------------------------------
class COperatorCategoryUtility : public COperatorCategory
{
public:
	enum EXEC_ID
	{
//		EID_RELOAD_INFO_FILE,
//		EID_CLEAR_INVENTORY,
	};

public:
			COperatorCategoryUtility(LPCTSTR categoryName, Window* parentWindow, const CRect& position);
			~COperatorCategoryUtility();

public:
	void	Start();
	void	Finish();

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	Run(id_t id);
	void	Process();

	void	Show();
	
	bool	MouseControl(UINT message, int _x, int _y);

private:
	ButtonGroup*	m_pC_button_group;
	bool			m_bTest;
};


//-----------------------------------------------------------------------------
// COperatorCategoryShowInfo
//
//-----------------------------------------------------------------------------
class COperatorCategoryShowInfo : public COperatorCategory
{
public:
	enum EXEC_ID
	{
		TEST_ID,
	};

public:
			COperatorCategoryShowInfo(LPCTSTR categoryName, Window* parentWindow, const CRect& position);
			~COperatorCategoryShowInfo();

public:
	void	Start();
	void	Finish();

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	Run(id_t id);
	void	Process();

	void	Show();
	
	bool	MouseControl(UINT message, int _x, int _y);

private:
	ButtonGroup*	m_pC_button_group;
	bool			m_bTest;
};


//-----------------------------------------------------------------------------
// COperatorCategoryGameSetting
//
//-----------------------------------------------------------------------------
class COperatorCategoryGameSetting : public COperatorCategory
{
public:
	enum EXEC_ID
	{
		TEST_ID,
	};

public:
			COperatorCategoryGameSetting(LPCTSTR categoryName, Window* parentWindow, const CRect& position);
			~COperatorCategoryGameSetting();

public:
	void	Start();
	void	Finish();

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	Run(id_t id);
	void	Process();

	void	Show();
	
	bool	MouseControl(UINT message, int _x, int _y);

private:
	ButtonGroup*	m_pC_button_group;
	bool			m_bTest;
};



class COperatorCategorySimpleCommand: public COperatorCategory
{
public:
	typedef std::list<std::string> STRING_LIST;
	enum EXEC_ID
	{
		TEST_ID,
	};

public:
	COperatorCategorySimpleCommand(LPCTSTR categoryName, Window* parentWindow, const CRect& position);
	~COperatorCategorySimpleCommand();

public:
	void	Start();
	void	Finish();

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	Run(id_t id);
	void	Process();

	void	Show();
	
	bool	MouseControl(UINT message, int _x, int _y);


private:
	
	STRING_LIST		m_firstCommand;
	STRING_LIST		m_secondCommand;
	STRING_LIST		m_textList;

	C_VS_UI_LISTBOX* m_pfirstCommand;
	C_VS_UI_LISTBOX* m_psecondCommand;
	C_VS_UI_LISTBOX* m_ptextList;

	ButtonGroup*	m_pC_button_group;
	bool			m_bTest;
	static int		m_saveSelectIndexFirst;
	static int		m_saveSelectIndexSecond;
	static int		m_saveSelectIndexText;

};

#if __CONTENTS(__ITEM_SHOW)
//-----------------------------------------------------------------------------
// COperatorCategoryShowItemSetting
// Cheon-Iljea 2008.7.17
//-----------------------------------------------------------------------------
class COperatorCategoryShowItemSetting : public COperatorCategory
{
public:
	typedef std::list<std::string> STRING_LIST;
	enum EXEC_ID
	{
		TEST_ID,
	};

public:
	COperatorCategoryShowItemSetting(LPCTSTR categoryName, Window* parentWindow, const CRect& position);
	~COperatorCategoryShowItemSetting();

public:
	void	Start();
	void	Finish();

	void	ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button);
	void	ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button);

	void	Run(id_t id);
	void	Process();

	void	Show();
	
	bool	MouseControl(UINT message, int _x, int _y);
	bool	ShowItem_Find(string find_str);

private:
	ButtonGroup*		m_pC_button_group;
	bool				m_bTest;
	string				m_find_str;
	string				m_find_mark_str;
	int					m_find_mark_str_size;

	CRect				m_RectList;

	STRING_LIST			m_ItemClassListBox;
	STRING_LIST			m_ItemTypeListBox;

	C_VS_UI_LISTBOX*	m_pItemClassListBox;
	C_VS_UI_LISTBOX*	m_pItemTypeListBox;

	static int			m_saveSelectIndexItemClass;
	static int			m_saveSelectIndexItemType;

	// FAZ 4 (2026-05-15): Admin menu Item tab FPS optimization —
	// ShowItem_Find cache. Show() invokes ShowItem_Find every frame; the
	// existing m_find_str.size() heuristic only catches a narrow case.
	// Explicit cache_valid flag + last-string field so the O(N²) rebuild
	// (DelListNodeAll + per-class AddString loop over MAX_ITEM_CLASS) is
	// only executed when the search text actually changes or a click
	// invalidates the cache.
	string				m_last_find_str;
	bool				m_cache_valid;
};
#endif //__ITEM_SHOW

#endif	//__OPERATORCATEGORY_H__