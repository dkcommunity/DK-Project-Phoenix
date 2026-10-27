//--------------------------------------------------------------------------
// TalkBox.cpp
//--------------------------------------------------------------------------
#include "Client_PCH.h"
#include "TalkBox.h"
#include "MStringList.h"
#include "UIDialog.h"
#include "MNPCScriptTable.h"
#include "SystemAvailabilities.h"
//--------------------------------------------------------------------------
// global
//--------------------------------------------------------------------------
MStringList	*	g_pNPCTalkBox = NULL;

PCTalkBox	*	g_pPCTalkBox = NULL;

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
PCTalkBox::PCTalkBox()
{ 
	m_Type		= NORMAL;
	m_NPCID		= 0;
	m_CreatureType = 0;
	m_ScriptID	= 0; 
	m_AnswerID	= 0;
}
void
PCTalkBox::SetNPCTalkBox(int talkBoxType,const char* content, unsigned int npcID, unsigned int creatureType, unsigned int sID)
{
	SetType((PCTalkBox::TALKBOX_TYPE)talkBoxType);
	SetContent(content);
	SetNPCID(npcID);
	SetCreatureType(creatureType);
	SetScriptID(sID);

	int contentSize = g_pNPCScriptTable->GetContentSize(sID);
	
	g_pPCTalkBox->m_AnswerIDMap.clear();
	
	int idnum=0;
	
	for (int i=0; i<contentSize; i++)
	{
		// g_PCTalkBox에 추가
		if(g_pSystemAvailableManager->ScriptFiltering(sID, i))
		{
 			g_pPCTalkBox->AddString(g_pNPCScriptTable->GetContent(sID, i));
			g_pPCTalkBox->m_AnswerIDMap.push_back(i);
		}				
	}
	
	DEBUG_ADD("SetContent");
	
	g_pUIDialog->PopupPCTalkDlg();
}
