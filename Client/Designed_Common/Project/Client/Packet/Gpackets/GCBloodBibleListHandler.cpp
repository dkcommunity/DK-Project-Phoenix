//////////////////////////////////////////////////////////////////////
//
// Filename    : GCBloodBibleListHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#pragma warning(disable:4786)
// include files
#include "GCBloodBibleList.h"


#include "ClientDef.h"
#include "TalkBox.h"
#include "UIDialog.h"
#include "SystemAvailabilities.h"
#include "MGameStringTable.h"
#include "TempInformation.h"

//////////////////////////////////////////////////////////////////////
//
// Å¬¶óÀÌ¾ðÆ®¿¡¼­ ¼­¹ö·ÎºÎÅÍ ¸Þ½ÃÁö¸¦ ¹Þ¾ÒÀ»¶§ ½ÇÇàµÇ´Â ¸Þ½îµåÀÌ´Ù.
//
//////////////////////////////////////////////////////////////////////
void GCBloodBibleListHandler::execute ( GCBloodBibleList * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY 
	
#ifdef __GAME_CLIENT__
	if (g_pPlayer==NULL
		|| g_pZone==NULL
		|| g_pUIDialog==NULL
		|| g_pPCTalkBox==NULL)
	{
		DEBUG_ADD("[Error] Some Object is NULL");
		return;
	}

	g_pPlayer->SetWaitVerifyNULL();

	g_pPCTalkBox->Release();		// 

	std::vector<ItemType_t>	BloodBibleList = pPacket->getList();

	g_pPCTalkBox->SetType( PCTalkBox::BLOOD_BIBLE_SIGN );

	char str[512];
	char str2[512];

	int bibletype;
	for(int i = 0; i< BloodBibleList.size(); i++)
	{
		bibletype = BloodBibleList[i];
		if(bibletype < 12)
		{
			sprintf(str2, (*g_pGameStringTable)[UI_STRING_MESSAGE_RENT_BLOOD_BIBLE2].GetString(), 
				(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_ARMEGA+bibletype].GetString(), 
				(*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+bibletype].GetString());
		}
#if __CONTENTS(__CONTRIBUTE_SYSTEM)
		else
		{
			//sprintf(str2, (*g_pGameStringTable)[UI_STRING_MESSAGE_RENT_BLOOD_BIBLE3].GetString(),
			//	(*g_pGameStringTable)[UI_STRING_MESSAGE_GRUN+(bibletype/12 - 1)].GetString(), 
			//	(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_ARMEGA+bibletype%12].GetString(), 
			//	(*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+bibletype%12].GetString(),
			//	(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_BONUS_GRUN_ARMEGA+(bibletype-12)].GetString()
			//	);
			sprintf(str2, "Borrow %s%s's holy tome (%s,%s)",
				(*g_pGameStringTable)[UI_STRING_MESSAGE_GRUN+(bibletype/12 - 1)].GetString(), 
				(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_ARMEGA+bibletype%12].GetString(), 
				(*g_pGameStringTable)[STRING_MESSAGE_BLOOD_BIBLE_BONUS_ARMEGA+bibletype%12].GetString(),
				(*g_pGameStringTable)[UI_STRING_MESSAGE_BLOOD_BIBLE_BONUS_GRUN_ARMEGA+(bibletype-12)].GetString()
				);
		}
#endif //__CONTRIBUTE_SYSTEM
		sprintf(str, "%3d %s", bibletype, str2);
		g_pPCTalkBox->AddString( str );
	}
	
	// ³¡³»±â Ãß°¡
	std::string szMsg;
	szMsg += "999";
	szMsg += "We are not lending out the sacred texts at the moment";
	g_pPCTalkBox->AddString( szMsg.c_str() );


	//std::string szMsg3;
	//szMsg3 += "555";
	//szMsg3 += "555&&ÉÏÒ»Ò³";
	//g_pPCTalkBox->AddString( szMsg3.c_str() );

	//std::string szMsg2;
	//szMsg2 += "666";
	//szMsg2 += "666&&ÏÂÒ»Ò³";
	//g_pPCTalkBox->AddString( szMsg2.c_str() );


	strcpy(str, (*g_pGameStringTable)[UI_STRING_MESSAGE_RENT_BLOOD_BIBLE].GetString());

	g_pPCTalkBox->SetContent( str );

	g_pUIDialog->PopupPCTalkDlg();
	g_pTempInformation->SetMode(TempInformation::MODE_SKILL_LEARN);

#endif

	__END_CATCH
}
