#include "client_PCH.h"
#include "crash_trace_tool.h"
#include "VS_UI_GameCommon.h"
#include "OperatorCategory.h"
#include "OperatorOption.h"
#include "VS_UI_widget.h"
#include "VS_UI.h"
#include "UIDialog.h"
#include "MGameStringTable.h"

#if __CONTENTS(__ITEM_SHOW)
#include "ClientFunction.h"
#include <sstream>
#endif //__ITEM_SHOW

int COperatorCategorySimpleCommand:: m_saveSelectIndexFirst = -1;
int COperatorCategorySimpleCommand:: m_saveSelectIndexSecond = -1;
int COperatorCategorySimpleCommand:: m_saveSelectIndexText = -1;

#if __CONTENTS(__ITEM_SHOW)
int COperatorCategoryShowItemSetting:: m_saveSelectIndexItemClass = -1;
int COperatorCategoryShowItemSetting:: m_saveSelectIndexItemType = -1;

using namespace std;

// ==========================================================================
// FAZ 4 (2026-05-04 oturum 3) — Defansif yardimcilar (audit sonucu)
//
// VS6 mirasi admin menu kodu su pattern'lere defansif degil:
//   1) (*g_pItemTable)[N][M] — bound check yok (table size=0 veya OOB → crash)
//   2) HName/EName.GetString() — NULL dondurebilir → ostream<<NULL undefined
//   3) Loop conditional'da virtual call (chained virtual call pattern,
//      project_inventory_loop_virtual_call.md)
//   4) sscanf("%s", buf[N]) — field width yok → buffer overflow potansiyeli
//   5) strcpy(buf, src) — src uzunluk kontrolu yok
//
// Asagidaki helper'lar bu pattern'leri tek noktada centralize eder.
// 5 risk noktasinin tamami bunlara cevrildi (constructor + Show + ShowItem_Find
// 3 nokta).
// ==========================================================================

// Safe wrapper: g_pItemTable[classN].GetSize() OOB-protected.
// Returns 0 if NULL/OOB instead of crashing.
static inline int SafeItemClassSize(int classN)
{
    if (g_pItemTable == NULL) return 0;
    if (classN < 0 || classN >= (int)(*g_pItemTable).GetSize()) return 0;
    return (int)(*g_pItemTable)[classN].GetSize();
}

// Safe wrapper: g_pItemTable[classN][typeN].HName/EName.GetString() with
// NULL guard + 80-char clamp into caller-provided buffers (avoids ostream<<
// NULL UB and glibc fortify_fail on corrupt CHS strings).
//
// Caller responsibility: pass `char[128]` (or larger) buffers. Will write
// at most 80 chars + null terminator.
static inline void SafeItemTableEntry(int classN, int typeN,
                                       char* hbuf, char* ebuf)
{
    hbuf[0] = '\0';
    ebuf[0] = '\0';
    if (g_pItemTable == NULL) return;
    if (classN < 0 || classN >= (int)(*g_pItemTable).GetSize()) return;
    if (typeN  < 0 || typeN  >= (int)(*g_pItemTable)[classN].GetSize()) return;
    const char* h = (*g_pItemTable)[classN][typeN].HName.GetString();
    const char* e = (*g_pItemTable)[classN][typeN].EName.GetString();
    if (h) { strncpy(hbuf, h, 80); hbuf[80] = '\0'; }
    if (e) { strncpy(ebuf, e, 80); ebuf[80] = '\0'; }
}

#endif //__ITEM_SHOW

//-----------------------------------------------------------------------------
// COperatorCategory
//
//-----------------------------------------------------------------------------
COperatorCategory::COperatorCategory(LPCTSTR categoryName, Window* parentWindow, const CRect& position)
	: m_CategoryName(categoryName), m_pParentWindow(parentWindow), CRect(position)
{

}


//-----------------------------------------------------------------------------
// COperatorCategoryUtility
//
//-----------------------------------------------------------------------------
COperatorCategoryUtility::COperatorCategoryUtility(LPCTSTR categoryName, Window* parentWindow, const CRect& position)
	: COperatorCategory(categoryName, parentWindow, position)
{
	m_pC_button_group = new ButtonGroup(this);
	m_pC_button_group->Init();

	CPoint ptButton(15, 15);
	
//	m_pC_button_group->Add(new C_VS_UI_EVENT_PUSH_BUTTON(CRect(CPoint(ptButton.x, ptButton.y), CSize(200, 30)),
//		EID_RELOAD_INFO_FILE, this, "Reload all info files"));

//	ptButton.y += 40;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bSlideScreenShot,
		"Slide screenshot (Shift+Drag: select area)"));

}


COperatorCategoryUtility::~COperatorCategoryUtility()
{

	DeleteNew(m_pC_button_group);
}


void COperatorCategoryUtility::Start()
{

}


void COperatorCategoryUtility::Finish()
{
	
}


void COperatorCategoryUtility::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	C_VS_UI_EVENT_STRING_BUTTON* pStringButton = dynamic_cast<C_VS_UI_EVENT_STRING_BUTTON*>(p_button);
	
	if(pStringButton)
	{
		CPoint showOffset(TopLeft());
		showOffset.x += GetParentWindowPtr()->x;
		showOffset.y += GetParentWindowPtr()->y;
		pStringButton->Show(showOffset);
	}
}


void COperatorCategoryUtility::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{

}


//-----------------------------------------------------------------------------
// Empty by design (2026-05-17): operator menu uses C_VS_UI_EVENT_CHECK_BUTTON
// with bool* binding (SyncOuterState) -- Run(id) dispatch not wired here.
// PUSH_BUTTON support reserved for future EID_RELOAD_INFO_FILE etc.
//-----------------------------------------------------------------------------
void COperatorCategoryUtility::Run(id_t id)
{
//	switch(id)
//	{
// 	case EID_RELOAD_INFO_FILE:
// 		gpC_base->SendMessage(UI_RELOAD_INFO_FILE);
// 		break;
//	}
}


void COperatorCategoryUtility::Process()
{
	m_pC_button_group->Process();
}


void COperatorCategoryUtility::Show()
{
	CT_Done("WnSh: COperatorCategoryUtility::Show ENTER pthis=%p", (void*)this);
	m_pC_button_group->Show();
}


bool COperatorCategoryUtility::MouseControl(UINT message, int _x, int _y)
{
	m_pC_button_group->MouseControl(message, _x, _y);

	return true;
}


//-----------------------------------------------------------------------------
// COperatorCategoryShowInfo
//
//-----------------------------------------------------------------------------
COperatorCategoryShowInfo::COperatorCategoryShowInfo(LPCTSTR categoryName, Window* parentWindow, const CRect& position)
	: COperatorCategory(categoryName, parentWindow, position)
{
	m_pC_button_group = new ButtonGroup(this);
	m_pC_button_group->Init();

	CPoint ptButton(15, 15);
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bShowModifyHP, "Show HP"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bShowModifyEXP, "Show EXP"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bShowCreatureInfo, "Show monster info"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bShowImageObjectInfo, "Show map image"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bShowAddEffect, "Show effects"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bShowSkillRange, "Skill range"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bShowModifyRANKEXP, "Show class EXP"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bRecordCharName, "Record character name"));

		ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bCanRootingGhost, "Hide"));

}


COperatorCategoryShowInfo::~COperatorCategoryShowInfo()
{
	DeleteNew(m_pC_button_group);
}


void COperatorCategoryShowInfo::Start()
{

}


void COperatorCategoryShowInfo::Finish()
{
	
}


void COperatorCategoryShowInfo::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	C_VS_UI_EVENT_STRING_BUTTON* pStringButton = dynamic_cast<C_VS_UI_EVENT_STRING_BUTTON*>(p_button);
	
	if(pStringButton)
	{
		CPoint showOffset(TopLeft());
		showOffset.x += GetParentWindowPtr()->x;
		showOffset.y += GetParentWindowPtr()->y;
		pStringButton->Show(showOffset);
	}
}


void COperatorCategoryShowInfo::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{

}


//-----------------------------------------------------------------------------
// Empty by design (2026-05-17): operator menu uses C_VS_UI_EVENT_CHECK_BUTTON
// with bool* binding (SyncOuterState) -- Run(id) dispatch not wired here.
// PUSH_BUTTON support reserved for future EID_RELOAD_INFO_FILE etc.
//-----------------------------------------------------------------------------
void COperatorCategoryShowInfo::Run(id_t id)
{

}


void COperatorCategoryShowInfo::Process()
{
	m_pC_button_group->Process();
}


void COperatorCategoryShowInfo::Show()
{
	CT_Done("WnSh: COperatorCategoryShowInfo::Show ENTER pthis=%p", (void*)this);
	m_pC_button_group->Show();
}


bool COperatorCategoryShowInfo::MouseControl(UINT message, int _x, int _y)
{
	m_pC_button_group->MouseControl(message, _x, _y);

	return true;
}



//-----------------------------------------------------------------------------
// COperatorCategoryGameSetting
//
//-----------------------------------------------------------------------------
COperatorCategoryGameSetting::COperatorCategoryGameSetting(LPCTSTR categoryName, Window* parentWindow, const CRect& position)
	: COperatorCategory(categoryName, parentWindow, position)
{
	m_pC_button_group = new ButtonGroup(this);
	m_pC_button_group->Init();

	CPoint ptButton(15, 15);
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bHideAttachEffect, "Hide attached effects"));

	ptButton.y += 25;
	m_pC_button_group->Add(new C_VS_UI_EVENT_CHECK_BUTTON(ptButton,
		-1, this, &g_pOperatorOption->bHideSpeechBalloon, "Speech Balloon"));
}


COperatorCategoryGameSetting::~COperatorCategoryGameSetting()
{
	DeleteNew(m_pC_button_group);
}


void COperatorCategoryGameSetting::Start()
{

}


void COperatorCategoryGameSetting::Finish()
{
	
}


void COperatorCategoryGameSetting::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	C_VS_UI_EVENT_STRING_BUTTON* pStringButton = dynamic_cast<C_VS_UI_EVENT_STRING_BUTTON*>(p_button);
	
	if(pStringButton)
	{
		CPoint showOffset(TopLeft());
		showOffset.x += GetParentWindowPtr()->x;
		showOffset.y += GetParentWindowPtr()->y;
		pStringButton->Show(showOffset);
	}
}


void COperatorCategoryGameSetting::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{

}


//-----------------------------------------------------------------------------
// Empty by design (2026-05-17): operator menu uses C_VS_UI_EVENT_CHECK_BUTTON
// with bool* binding (SyncOuterState) -- Run(id) dispatch not wired here.
// PUSH_BUTTON support reserved for future EID_RELOAD_INFO_FILE etc.
//-----------------------------------------------------------------------------
void COperatorCategoryGameSetting::Run(id_t id)
{

}


void COperatorCategoryGameSetting::Process()
{
	m_pC_button_group->Process();
}


void COperatorCategoryGameSetting::Show()
{
	CT_Done("WnSh: COperatorCategoryGameSetting::Show ENTER pthis=%p", (void*)this);
	m_pC_button_group->Show();
}


bool COperatorCategoryGameSetting::MouseControl(UINT message, int _x, int _y)
{
	m_pC_button_group->MouseControl(message, _x, _y);

	return true;
}
//////////////////////////////////////////////////////////////////////////
//��� ���� ���ɾ�
//////////////////////////////////////////////////////////////////////////

COperatorCategorySimpleCommand::COperatorCategorySimpleCommand(LPCTSTR categoryName, Window* parentWindow, const CRect& position)
	: COperatorCategory(categoryName, parentWindow, position)
	, m_pfirstCommand(NULL), m_psecondCommand(NULL), m_ptextList(NULL), m_pC_button_group(NULL)
{
	ivfstream file;
	//���ʿ� �ü� �ִ� ���ɾ�
	file.open("Operator\\firstCommand.txt", 0);

	char tempstring[256];

	if(file.is_open())
	{
		while(!file.eof())
		{
			file.getline(tempstring, 256);
			m_firstCommand.push_back(tempstring);
		}
		file.close();
	}
	
	//���ʿ� �ü� �ִ� ���ɾ�
	file.open("Operator\\secondCommand.txt", 0);
	if(file.is_open())
	{
		while(!file.eof())
		{
			file.getline(tempstring, 256);
			m_secondCommand.push_back(tempstring);
		}
		file.close();
	}
	
	//���ɾ� �ڿ� �� text
	file.open("Operator\\textList.txt", 0);
	if(file.is_open())
	{
		while(!file.eof())
		{
			file.getline(tempstring, 256);
			m_textList.push_back(tempstring);
		}
		file.close();
	}

	CRect firstListBOX(CPoint(5, 5), CSize(position.Width()/2 - 5, position.Height()/3 - 5));
	
	CRect secondListBOX(CPoint(position.Width()/2 + 5, 5), CSize(position.Width()/2 - 5, position.Height()/3 - 5));

	CRect thirdListBOX(CPoint(5, position.Height()/3 + 5), CSize(position.Width() - 10, position.Height() * 2 / 3 - 35));

	m_pC_button_group = new ButtonGroup(this);
	m_pC_button_group->Init();


	m_pfirstCommand = new C_VS_UI_LISTBOX(m_firstCommand, firstListBOX, -1, this, &gpC_base->m_chatting_pi);
	m_psecondCommand = new C_VS_UI_LISTBOX(m_secondCommand, secondListBOX, -1, this, &gpC_base->m_chatting_pi);
	m_ptextList = new C_VS_UI_LISTBOX(m_textList, thirdListBOX, -1, this, &gpC_base->m_chatting_pi);
	m_pC_button_group->Add(m_pfirstCommand);
	m_pC_button_group->Add(m_psecondCommand);
	m_pC_button_group->Add(m_ptextList);
	m_pC_button_group->Add(new C_VS_UI_EVENT_PUSH_BUTTON(CRect(CPoint(25,482), CSize(position.Width()/5, position.Height()/20)),
		1 , this, "Input to chat box"));
	m_pC_button_group->Add(new C_VS_UI_EVENT_PUSH_BUTTON(CRect(CPoint(165,482), CSize(position.Width()/5, position.Height()/20)),
		2 , this, "Clear selection"));

	m_pfirstCommand->SetSelectListIndex(m_saveSelectIndexFirst);
	m_psecondCommand->SetSelectListIndex(m_saveSelectIndexSecond);
	m_ptextList->SetSelectListIndex(m_saveSelectIndexText);
}

COperatorCategorySimpleCommand::~COperatorCategorySimpleCommand()
{
	if(m_pfirstCommand)
		m_saveSelectIndexFirst = m_pfirstCommand->GetSelectListIndex();

	if(m_psecondCommand)
		m_saveSelectIndexSecond = m_psecondCommand->GetSelectListIndex();

	if(m_ptextList)
		m_saveSelectIndexText = m_ptextList->GetSelectListIndex();

	DeleteNew(m_pC_button_group);
}

void
COperatorCategorySimpleCommand::Start()
{
}

void
COperatorCategorySimpleCommand::Finish()
{
}

void
COperatorCategorySimpleCommand::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	C_VS_UI_EVENT_STRING_BUTTON* pStringButton = dynamic_cast<C_VS_UI_EVENT_STRING_BUTTON*>(p_button);
	C_VS_UI_LISTBOX* pListString = dynamic_cast<C_VS_UI_LISTBOX*>(p_button);
	
	if(pStringButton)
	{
		CPoint showOffset(TopLeft());
		showOffset.x += GetParentWindowPtr()->x;
		showOffset.y += GetParentWindowPtr()->y;
		pStringButton->Show(showOffset);
	}
	if(pListString)
	{
		CPoint showOffset(TopLeft());
		showOffset.x += GetParentWindowPtr()->x;
		showOffset.y += GetParentWindowPtr()->y;
		pListString->Show(showOffset);
	}

}
void
COperatorCategorySimpleCommand::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{
}

void
COperatorCategorySimpleCommand::Run(id_t id)
{

	std::string finalstring ="";

	switch(id)
	{
	case 1:
		if(m_pfirstCommand->GetSelectListString(m_pfirstCommand->GetSelectListIndex()))
		{
			finalstring += m_pfirstCommand->GetSelectListString(m_pfirstCommand->GetSelectListIndex());
			finalstring += " ";
		}
		if(m_psecondCommand->GetSelectListString(m_psecondCommand->GetSelectListIndex()))
		{
			finalstring += m_psecondCommand->GetSelectListString(m_psecondCommand->GetSelectListIndex());
			finalstring += " ";
		}
		if(m_ptextList->GetSelectListString(m_ptextList->GetSelectListIndex()))
		{
			finalstring += m_ptextList->GetSelectListString(m_ptextList->GetSelectListIndex());
		}
		if(finalstring.size()<=256)
		{
			gC_vs_ui.SetInputString(finalstring.c_str());
		}
		else
		{
#ifdef __GAME_CLIENT__			
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_USE].GetString());
#endif
		}
		
		break;

	case 2:	
		m_pfirstCommand->SetSelectListIndex(-1);
		m_psecondCommand->SetSelectListIndex(-1);
		m_ptextList->SetSelectListIndex(-1);
		break;
	}
}

void
COperatorCategorySimpleCommand::Process()
{
	m_pC_button_group->Process();
}

void
COperatorCategorySimpleCommand::Show()
{
	m_pC_button_group->Show();
}
	
bool
COperatorCategorySimpleCommand::MouseControl(UINT message, int _x, int _y)
{
	m_pC_button_group->MouseControl(message, _x, _y);
	return true;
}

#if __CONTENTS(__ITEM_SHOW)
//-----------------------------------------------------------------------------
// COperatorCategoryShowItemSetting
// Cheon-Iljea 2008.7.17
//-----------------------------------------------------------------------------
COperatorCategoryShowItemSetting::COperatorCategoryShowItemSetting(LPCTSTR categoryName, Window* parentWindow, const CRect& position)
	: COperatorCategory(categoryName, parentWindow, position)
	, m_pItemClassListBox(NULL), m_pItemTypeListBox(NULL)
{
	m_RectList = position;
	m_find_str = "";

	// FAZ 4 (2026-05-15): ShowItem_Find cache init.
	m_last_find_str = "";
	m_cache_valid   = false;

	// ���
	CRect rc_ItemClassListBOX(CPoint(5, 5), CSize(position.Width()/2 - 5, position.Height() - 10));
	CRect rc_ItemTypeListBOX(CPoint(position.Width()/2 + 5, 5), CSize(position.Width()/2 - 10, position.Height() - 10));

	m_pItemClassListBox = new C_VS_UI_LISTBOX(m_ItemClassListBox, rc_ItemClassListBOX, -1, this, &gpC_base->m_chatting_pi);
	m_pItemTypeListBox = new C_VS_UI_LISTBOX(m_ItemTypeListBox, rc_ItemTypeListBOX, -1, this, &gpC_base->m_chatting_pi);

	m_pC_button_group = new ButtonGroup(this);
	m_pC_button_group->Init();

	m_pC_button_group->Add(m_pItemClassListBox);
	m_pC_button_group->Add(m_pItemTypeListBox);

	for(int i=0;i<MAX_ITEM_CLASS;i++)
	{
		ostringstream ostr;
		ostr<<i<<". "<<ITEM_SHOW_NAME[i];

		m_pItemClassListBox->AddString(ostr.str());
	}
	m_pItemClassListBox->AddString("<<Clear inventory>");
	m_pItemClassListBox->AddString("");
	m_pItemClassListBox->AddString("create by rallser");
	m_pItemClassListBox->AddString("");

	// FAZ 4 (audit): SafeItemClassSize + SafeItemTableEntry (helper'lar)
	int item_size = SafeItemClassSize(0);  // MOTORCYCLE = 0

	for(int i=0;i<item_size;i++)
	{
		ostringstream ostr;
		char hbuf[128], ebuf[128];
		SafeItemTableEntry(0, i, hbuf, ebuf);
		ostr<<0<<"-"<<i<<". "<<hbuf<<"("<<ebuf<<")";

		m_pItemTypeListBox->AddString(ostr.str());
	}

	m_find_mark_str = "This Class! >";
	m_find_mark_str_size = m_find_mark_str.size();

	m_pItemClassListBox->SetSelectListIndex(0);
	m_pItemTypeListBox->SetSelectListIndex(m_saveSelectIndexItemType);
}

COperatorCategoryShowItemSetting::~COperatorCategoryShowItemSetting()
{
	if(m_pItemClassListBox)
		m_saveSelectIndexItemClass = m_pItemClassListBox->GetSelectListIndex();
	if(m_pItemTypeListBox)
		m_saveSelectIndexItemType = m_pItemTypeListBox->GetSelectListIndex();

	DeleteNew(m_pC_button_group);
	gC_vs_ui.SetInputString("");
}


void COperatorCategoryShowItemSetting::Start()
{
	
}

void COperatorCategoryShowItemSetting::Finish()
{
	
}

void COperatorCategoryShowItemSetting::ShowButtonWidget(C_VS_UI_EVENT_BUTTON * p_button)
{
	C_VS_UI_LISTBOX* pListString = dynamic_cast<C_VS_UI_LISTBOX*>(p_button);

	if(pListString)
	{
		CPoint showOffset(TopLeft());
		showOffset.x += GetParentWindowPtr()->x;
		showOffset.y += GetParentWindowPtr()->y;
		pListString->Show(showOffset);
	}
}

void COperatorCategoryShowItemSetting::ShowButtonDescription(C_VS_UI_EVENT_BUTTON *p_button)
{

}

//-----------------------------------------------------------------------------
// Empty by design (2026-05-17): operator menu uses C_VS_UI_EVENT_CHECK_BUTTON
// with bool* binding (SyncOuterState) -- Run(id) dispatch not wired here.
// PUSH_BUTTON support reserved for future EID_RELOAD_INFO_FILE etc.
//-----------------------------------------------------------------------------
void COperatorCategoryShowItemSetting::Run(id_t id)
{
}


void COperatorCategoryShowItemSetting::Process()
{
	m_pC_button_group->Process();
}

void COperatorCategoryShowItemSetting::Show()
{
	CT_Done("WnSh: COperatorCategoryShowItemSetting::Show ENTER pthis=%p", (void*)this);
	m_pC_button_group->Show();

	const char *Find_Str = gC_vs_ui.GetNewInputString();
	char find_str[256];
	find_str[0] = '\0';
	if(Find_Str!=NULL)
	{
		// FAZ 4 (audit): strncpy + null terminator (find_str[256] overflow guard)
		strncpy(find_str, Find_Str, sizeof(find_str)-1);
		find_str[sizeof(find_str)-1] = '\0';
		delete (char *)Find_Str;
		Find_Str = NULL;
	}

	// ���ڿ� �������ֱ�
	string find_str_delete = "Search";
	string find_str_str	= find_str;

	if( lstrlen("Please enter search term.")!=find_str_str.size() )
	{
		if( find_str_delete.size()!=0 && find_str_str.find(find_str_delete.c_str()) != std::string::npos )
		{
			if(lstrlen("Please enter search term.")<find_str_str.size())
			{
				string str = &find_str_str[find_str_str.size()-1];
				gC_vs_ui.SetInputString(str.c_str());
				// FAZ 4 (audit): strncpy guard
				strncpy(find_str, str.c_str(), sizeof(find_str)-1);
				find_str[sizeof(find_str)-1] = '\0';
			}
			else
			{
				gC_vs_ui.SetInputString("");
				find_str[0] = '\0';
			}
		}
	}
	
	ShowItem_Find(find_str);
}

bool COperatorCategoryShowItemSetting::MouseControl(UINT message, int _x, int _y)
{
	m_pC_button_group->MouseControl(message, _x, _y);
	
	CPoint ptLocalMouse(_x , _y );
//	if(!m_RectList.PtInRect(ptLocalMouse))
//		message = M_LEFTBUTTON_UP; // ��ũ�ѹ� ��ũ�� �ϸ鼭 ������ ����� ���콺���� ��� ��ũ�� �Ǵ°� ��ĥ�����ߴ���.

	bool ClassList_bt = m_pItemClassListBox->MouseControl(message, _x, _y);
	bool TypeList_bt = m_pItemTypeListBox->MouseControl(message, _x, _y);
	
	bool bClickMessage = (message==M_LEFTBUTTON_DOWN || message==M_LB_DOUBLECLICK);
	
	// FAZ 4 (2026-05-04 oturum 2): Admin operator menu item-click crash
	// hardening. Three OOB / NULL paths converged here:
	//   1) Class_Num from atoi() can be negative or > g_pItemTable->GetSize()
	//      when the user clicks the "create by rallser" / blank lines (rows
	//      MAX_ITEM_CLASS+1..3 in m_pItemClassListBox).
	//   2) HName/EName MString::GetString() can return NULL for items with
	//      empty strings on certain CHS/KOR builds — ostream<< a NULL char*
	//      is undefined behaviour and crashes immediately.
	//   3) g_pItemTable itself may have GetSize()==0 if the table never
	//      loaded ("Empty global table OOB pattern" — same root as the May 3
	//      g_pCreatureTable / g_pItemOptionTable crashes).
	if(!ClassList_bt&&bClickMessage&&m_pItemClassListBox->GetSelectListIndex()<m_pItemClassListBox->ListNodeMax()&&m_pItemClassListBox->GetSelectListIndex()<=MAX_ITEM_CLASS||m_pItemClassListBox->GetSelectListIndex()==MAX_ITEM_CLASS+2)
	{
		// FAZ 4 (2026-05-15): Class selection changed → invalidate
		// ShowItem_Find cache so list rebuilds with new context.
		m_cache_valid = false;
		m_pItemTypeListBox->DelListNodeAll();

		if(m_pItemClassListBox->GetSelectListIndex()==MAX_ITEM_CLASS)
		{
			gpC_base->SendMessage(UI_ITEM_SHOW_AND_CLEAR_ITEM, 0, 0);
		}
		else
		{
			char src_str[20] = {0};
			if(m_pItemClassListBox->IsFindStringOK())
			{
				char mark_str[256];
				string _find_str;

				// FAZ 4 (audit): strncpy guard + sscanf field width
				const char* selStr = m_pItemClassListBox->GetSelectListString(m_pItemClassListBox->GetSelectListIndex());
				if (selStr)
				{
					strncpy(mark_str, selStr, sizeof(mark_str)-1);
					mark_str[sizeof(mark_str)-1] = '\0';
				}
				_find_str = mark_str;

				// find mark..
				if( m_find_mark_str.size()!=0 && _find_str.find(m_find_mark_str.c_str()) != std::string::npos )
					sscanf(&mark_str[m_find_mark_str_size],"%19s",src_str);
				else if (selStr)
					sscanf(selStr,"%19s",src_str);
			}
			else
			{
				const char* selStr = m_pItemClassListBox->GetSelectListString(m_pItemClassListBox->GetSelectListIndex());
				if (selStr)
					sscanf(selStr,"%19s",src_str);
			}

			int Class_Num = atoi(src_str);

			// Guard: Class_Num must be inside the loaded ItemTable. Empty
			// table (size==0) and OOB indices both crash the [][] access.
			CT_Done("OPMENU: ClassClick Class_Num=%d g_pItemTable=%p size=%d",
				Class_Num, (void*)g_pItemTable,
				g_pItemTable ? (int)(*g_pItemTable).GetSize() : -1);
			if (g_pItemTable == NULL ||
				Class_Num < 0 ||
				Class_Num >= (int)(*g_pItemTable).GetSize())
			{
				CT_Done("OPMENU: ClassClick OOB skip — Class_Num=%d", Class_Num);
				return true;
			}

			int item_size = (*g_pItemTable)[Class_Num].GetSize();
			CT_Done("OPMENU: ClassClick item_size=%d, pre AddString loop", item_size);

			for(int i=0;i<item_size;i++)
			{
				ostringstream ostr;
				// HName / EName GetString() defensive — undefined behaviour
				// to ostream<< NULL. Empty string fallback keeps the menu
				// rendering even for half-populated item rows.
				const char* pHName = (*g_pItemTable)[Class_Num][i].HName.GetString();
				const char* pEName = (*g_pItemTable)[Class_Num][i].EName.GetString();
				if (pHName == NULL) pHName = "";
				if (pEName == NULL) pEName = "";

				// FAZ 4 (2026-05-04 oturum 3): defensive truncation — bazi
				// HName/EName CHS dosyasinda corrupt bytes olabilir, ostream
				// uzun string'lerde glibc fortify_fail tetikleyebiliyor.
				// 80 karakterle clamp + per-iter trace.
				char hbuf[128] = {0};
				char ebuf[128] = {0};
				strncpy(hbuf, pHName, 80);
				strncpy(ebuf, pEName, 80);
				hbuf[80] = '\0';
				ebuf[80] = '\0';

				ostr<<Class_Num<<"-"<<i<<". "
					<<hbuf<<"("
					<<ebuf<<")";

				m_pItemTypeListBox->AddString(ostr.str());

				if ((i & 0x1F) == 0)  // her 32 iter'de bir trace (spam'i azaltir)
					CT_Done("OPMENU: ClassClick AddString iter[%d]/%d", i, item_size);
			}
			CT_Done("OPMENU: ClassClick AddString loop done (%d items)", item_size);
			m_pItemTypeListBox->SetSelectListIndex(-1);
		}
	}

	// create item..
	if(!TypeList_bt&&bClickMessage&&m_pItemTypeListBox->GetSelectListIndex()!=-1&&m_pItemTypeListBox->GetSelectListIndex()<m_pItemTypeListBox->ListNodeMax())
	{
		// FAZ 4 (2026-05-15): Type click triggers a create-item dispatch
		// — also invalidate cache to keep state consistent.
		m_cache_valid = false;
		char src_str[20] = {0};
		// FAZ 4 (audit): sscanf field width "%19s" (src_str[20] overflow guard)
		const char* typeSelStr = m_pItemTypeListBox->GetSelectListString(m_pItemTypeListBox->GetSelectListIndex());
		if (typeSelStr)
			sscanf(typeSelStr,"%19s",src_str);

		int Class_Num = atoi(src_str);

		// Find '-' separator, then read Type_Num. Guard against missing '-'
		// (would walk past end of src_str into garbage memory).
		char* ptr = src_str;
		while (*ptr != '\0' && *ptr != '-') ptr++;
		if (*ptr == '\0')
		{
			CT_Done("OPMENU: TypeClick parse fail — src='%s' no '-' separator", src_str);
			return true;
		}
		ptr++;  // skip '-'

		int Type_Num = atoi(ptr);

		// Guard before dispatching to server: bad Class/Type triggers the
		// same DB INSERT failure path that crashed gameserver in the
		// *create varchar(10) overflow incident.
		CT_Done("OPMENU: TypeClick dispatch UI_ITEM_SHOW_AND_CREATE_ITEM Class=%d Type=%d",
			Class_Num, Type_Num);
		if (g_pItemTable == NULL ||
			Class_Num < 0 ||
			Class_Num >= (int)(*g_pItemTable).GetSize() ||
			Type_Num < 0 ||
			Type_Num >= (int)(*g_pItemTable)[Class_Num].GetSize())
		{
			CT_Done("OPMENU: TypeClick OOB skip — Class=%d Type=%d",
				Class_Num, Type_Num);
			return true;
		}

		gpC_base->SendMessage(UI_ITEM_SHOW_AND_CREATE_ITEM, Class_Num, Type_Num);
	}

	return true;
}

bool COperatorCategoryShowItemSetting::ShowItem_Find(string find_str)
{
	if(m_pItemClassListBox->ListNodeMax()==0 || m_pItemTypeListBox->ListNodeMax()==0)
		return false;

	// FAZ 4 (2026-05-15): Admin menu Item tab FPS optimization —
	// short-circuit when the search input is unchanged. Show() calls this
	// every frame; without the cache it would DelListNodeAll +
	// AddString-per-class on MAX_ITEM_CLASS classes every frame
	// (~1M char ops/frame). Cache is invalidated by Class/Type click
	// handlers in MouseControl so the list rebuilds only on real input.
	if (m_cache_valid && m_last_find_str == find_str)
	{
		return true;
	}
	m_last_find_str = find_str;
	m_cache_valid   = true;

	if(m_find_str.size()==find_str.size())
	{
		if( find_str.size()==0 || find_str.size()>30 || (find_str.size()!=0 && m_find_str.find(find_str.c_str()) != std::string::npos) )
			return false;
	}

	m_find_str = find_str;
	m_pItemClassListBox->DelListNodeAll();

	for(int i=0;i<MAX_ITEM_CLASS;i++)
	{
		ostringstream ostr;
		string _str;

		//////////////////////////////////////////////////////////////////////////
		// ItemType Find.. ij-ch
		// FAZ 4 (audit): SafeItemClassSize + SafeItemTableEntry
		int item_size = SafeItemClassSize(i);
		m_pItemTypeListBox->DelListNodeAll();

		for(int j=0;j<item_size;j++)
		{
			ostringstream ostr;
			char hbuf[128], ebuf[128];
			SafeItemTableEntry(i, j, hbuf, ebuf);
			ostr<<i<<"-"<<j<<". "<<hbuf<<"("<<ebuf<<")";

			m_pItemTypeListBox->AddString(ostr.str());
		}

		m_pItemTypeListBox->SetFindString(find_str);
		m_pItemTypeListBox->SetSelectListIndex(-1);
		//////////////////////////////////////////////////////////////////////////

		ostr<<i<<". "<<ITEM_SHOW_NAME[i];
		_str = ostr.str();

		if( m_pItemTypeListBox->IsFindStringOK())
		{
			ostr.str("");
			ostr<<m_find_mark_str<<i<<". "<<ITEM_SHOW_NAME[i];
		}
		
		m_pItemClassListBox->AddString(ostr.str());
	}
	m_pItemClassListBox->AddString("<<Clear inventory>>");
	m_pItemClassListBox->AddString("");
	m_pItemClassListBox->AddString("create by rallser");
	m_pItemClassListBox->AddString("");
	
	m_pItemClassListBox->SetFindString(m_find_mark_str);

	// ���õ� ������ Ÿ�Ը���Ʈ
	//////////////////////////////////////////////////////////////////////////
	// FAZ 4 (audit): src_str[20] sscanf field width "%19s" + strncpy mark_str.
	char src_str[20] = {0};
	if(m_pItemClassListBox->IsFindStringOK())
	{
		char mark_str[256] = {0};
		string _find_str;

		const char* selStr = m_pItemClassListBox->GetSelectListString(m_pItemClassListBox->GetSelectListIndex());
		if (selStr)
		{
			strncpy(mark_str, selStr, sizeof(mark_str)-1);
			mark_str[sizeof(mark_str)-1] = '\0';
		}
		_find_str = mark_str;

		// find mark..
		if( m_find_mark_str.size()!=0 && _find_str.find(m_find_mark_str.c_str()) != std::string::npos )
			sscanf(&mark_str[m_find_mark_str_size],"%19s",src_str);
		else if (selStr)
			sscanf(selStr,"%19s",src_str);
	}
	else
	{
		const char* selStr = m_pItemClassListBox->GetSelectListString(m_pItemClassListBox->GetSelectListIndex());
		if (selStr)
			sscanf(selStr,"%19s",src_str);
	}

	//////////////////////////////////////////////////////////////////////////
	int Class_Num = atoi(src_str);

	// FAZ 4 (audit): SafeItemClassSize + SafeItemTableEntry
	int item_size = SafeItemClassSize(Class_Num);
	m_pItemTypeListBox->DelListNodeAll();

	for(int i=0;i<item_size;i++)
	{
		ostringstream ostr;
		char hbuf[128], ebuf[128];
		SafeItemTableEntry(Class_Num, i, hbuf, ebuf);
		ostr<<Class_Num<<"-"<<i<<". "<<hbuf<<"("<<ebuf<<")";

		m_pItemTypeListBox->AddString(ostr.str());
	}
	return true;
}
#endif //__ITEM_SHOW
