//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddEffectToScreenHandler.cpp
// Written By  : rappi@darkeden.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddEffectToScreen.h"
#include "ClientDef.h"
#include "TalkBox.h"
#include "UIDialog.h"
#include "MNPCScriptTable.h"
#include "TempInformation.h"
#include "SystemAvailabilities.h"

#include "ScreenEffectDef.h"		//화면 효과 Def

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#if __CONTENTS(__HIDDEN_BOSS_SYSTEM	)
void	SetScreenEffect_Dark_FadeOut(GCAddEffectToScreen *pGCAddEffectToScreen,		MEvent	&event);
#endif //__HIDDEN_BOSS_SYSTEM

#if __CONTENTS(__TIPOJYU_CASTLE)
void	SetScreenEffect_Shake(GCAddEffectToScreen *pGCAddEffectToScreen, 	MEvent	&event);
void	SetScreenEffect_Dark_FadeIn(GCAddEffectToScreen *pGCAddEffectToScreen,		MEvent	&event);

void GCAddEffectToScreenHandler::execute ( GCAddEffectToScreen *pGCAddEffectToScreen , Player *pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	//std::cout << pGCAddEffectToScreen->toString() << std::endl;
	MEvent	event;

	switch(pGCAddEffectToScreen->getEffectID())
	{
		case SCREEN_EFFECT_SHAKE:
			SetScreenEffect_Shake(pGCAddEffectToScreen, event);
		break;

		case SCREEN_EFFECT_DARK:
			SetScreenEffect_Dark_FadeIn(pGCAddEffectToScreen, event);
		break;
		case SCREEN_EFFECT_RECOVERY:
			SetScreenEffect_Dark_FadeOut(pGCAddEffectToScreen, event);
		break;
	}
	
	g_pEventManager->AddEvent(event);
#endif

	__END_CATCH
}



void	SetScreenEffect_Shake(GCAddEffectToScreen *pGCAddEffectToScreen,	MEvent &event)
{
	event.eventID = EVENTID_NULL;
	event.eventType = EVENTTYPE_ZONE;
	event.eventDelay = pGCAddEffectToScreen->getDuration();
	event.eventFlag = EVENTFLAG_SHAKE_SCREEN;
	event.parameter3 = 3;
}


void	SetScreenEffect_Dark_FadeIn(GCAddEffectToScreen *pGCAddEffectToScreen,	MEvent &event)
{
	event.eventID = EVENTID_GDR_PRESENT;
	event.eventType = EVENTTYPE_ZONE;
	event.eventDelay = pGCAddEffectToScreen->getDuration();
	event.eventFlag = EVENTFLAG_FADE_SCREEN;
	event.parameter3 = 3;
}

#endif //__TIPOJYU_CASTLE
#if __CONTENTS(__HIDDEN_BOSS_SYSTEM	)
void	SetScreenEffect_Dark_FadeOut(GCAddEffectToScreen *pGCAddEffectToScreen,	MEvent &event)
{
	event.eventID = EVENTID_FADE_OUT;
	event.eventType = EVENTTYPE_ZONE;
	event.eventDelay = pGCAddEffectToScreen->getDuration();
	event.eventFlag = EVENTFLAG_FADE_SCREEN;
	event.parameter3 = 3;
}
#endif //__HIDDEN_BOSS_SYSTEM	
