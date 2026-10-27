//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRemoveEffectHandler.cc
// Written By  : crazydog
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRemoveEffect.h"
#include "ClientDef.h"
#include "MEffectStatusDef.h"
#include "MEventManager.h"
#include "MSkillManager.h"
#include "SkillDef.h"
#include "MTopView.h"

#include "OperatorOption.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCRemoveEffectHandler::execute ( GCRemoveEffect * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
		

	//------------------------------------------------------
	// 디버그 메시지 보기
	//------------------------------------------------------
	if(g_pUserInformation->attrOperator.GetValue() &&
	   g_pOperatorOption->bShowAddEffect)
	{	
		char szBuf[32];
		sprintf(szBuf, "listSize=%d", pPacket->getListNum());
		UI_AddChatToHistory(szBuf, "Recv GCRemoveEffect", 6, RGB(255, 255, 0));
	}
		
	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
	}
	//------------------------------------------------------
	// 정상.. 
	//------------------------------------------------------
	else
	{
		int CreatureID = pPacket->getObjectID();
		
		MCreature* pCreature = g_pZone->GetCreature(CreatureID);

		// 성물보관대 얻어오기 위해서
		if (pCreature == NULL)
		{
			MItem *selectedItem = g_pZone->GetItem(CreatureID);
			if(selectedItem != NULL)
			{
				if(selectedItem->GetItemClass() == ITEM_CLASS_CORPSE)
				{
					MCorpse *pCorpse = (MCorpse *)selectedItem;
					pCreature = pCorpse->GetCreature();
				}
			}
		}

		if (pCreature==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] There is no such Creature. id=%d", pPacket->getObjectID());
		}
		else
		{	
			int size = pPacket->getListNum();

			for (int i=0; i<size; i++)
			{
				WORD effectStatus = pPacket->popFrontListElement();


				//------------------------------------------------------
				// 디버그 메시지 보기
				//------------------------------------------------------
				if(g_pUserInformation->attrOperator.GetValue() &&
					g_pOperatorOption->bShowAddEffect)
				{	
					char szBuf[128];
					
					const char* pName;
					if(pCreature)	pName = pCreature->GetName();
					else			pName = "?";
					
					sprintf(szBuf, "targetName=%s, cid=%d, eid=%d",
						pName, CreatureID, effectStatus);
					
					UI_AddChatToHistory(szBuf, "-> GCRemoveEffect List", 6, RGB(255, 255, 0));
				}


				switch( effectStatus )
				{
					case EFFECTSTATUS_KEEP_SWEEPER :                      // 304
					case EFFECTSTATUS_KEEP_SWEEPER_2 :                    // 305
					case EFFECTSTATUS_KEEP_SWEEPER_3 :                    // 306
					case EFFECTSTATUS_KEEP_SWEEPER_4 :                    // 307
					case EFFECTSTATUS_KEEP_SWEEPER_5 :                    // 308
					case EFFECTSTATUS_KEEP_SWEEPER_6 :                    // 309
					case EFFECTSTATUS_KEEP_SWEEPER_7 :                    // 310
					case EFFECTSTATUS_KEEP_SWEEPER_8 :                    // 311
					case EFFECTSTATUS_KEEP_SWEEPER_9 :                    // 312
					case EFFECTSTATUS_KEEP_SWEEPER_10 :                   // 313
					case EFFECTSTATUS_KEEP_SWEEPER_11 :                   // 314
					case EFFECTSTATUS_KEEP_SWEEPER_12 :                   // 315
						PlaySound( SOUND_SWEEPER_END );
						break;
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
					case EFFECTSTATUS_SUMMON_SYLPH_GREEN :
					case EFFECTSTATUS_SUMMON_SYLPH_ORANGE :
					case EFFECTSTATUS_SUMMON_SYLPH_RED :
					case EFFECTSTATUS_SUMMON_SYLPH_YELLOW :
					case EFFECTSTATUS_SUMMON_SYLPH_WHITE :
					case EFFECTSTATUS_SUMMON_SYLPH_BROWN :
					case EFFECTSTATUS_SUMMON_SYLPH_LILAC :
					case EFFECTSTATUS_SUMMON_SYLPH_BLACK :
					case EFFECTSTATUS_SUMMON_SYLPH:
					case EFFECTSTATUS_SUMMON_WISP_RED:
					case EFFECTSTATUS_SUMMON_WISP_YELLOW:
					case EFFECTSTATUS_SUMMON_WISP_GREEN:
						{
							if(pCreature->GetWingType() == 2)
							{
#if __CONTENTS(__SECOND_TRANSFORTER)
								if(pCreature->GetWingItemType() == 0)
								{
#endif //__SECOND_TRANSFORTER
									effectStatus = EFFECTSTATUS_CLIENT_WING_SYLPH;
									pCreature->SetWingType(0);
#if __CONTENTS(__SECOND_TRANSFORTER)
								}
								else if(pCreature->GetWingItemType() == 1)
								{
									effectStatus = EFFECTSTATUS_CLIENT_UNICORN;
									pCreature->SetWingType(0);
								}
								else if(pCreature->GetWingItemType() == 2)//침쥣劤麟폄
								{
									effectStatus = EFFECTSTATUS_OusterWings_New_1;
									pCreature->SetWingType(0);
								}
								else if(pCreature->GetWingItemType() == 3)//침쥣劤麟폄
								{
									effectStatus = EFFECTSTATUS_OusterWings_New_2;
									pCreature->SetWingType(0);
								}
#endif //__SECOND_TRANSFORTER
							}
						}
						break;
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
				}
				
				if (pCreature->RemoveEffectStatus( (EFFECTSTATUS)effectStatus ))
				{					
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] Failed to Remove EffectStatus %d", effectStatus);
				}
				// FIX 2026-05-12: g_pPlayer + pCreature + g_pEffectStatusTable NULL/bounds guard.
				if( g_pPlayer != NULL && pCreature != NULL
					&& g_pPlayer->GetID() == pCreature->GetID()
					&& g_pEffectStatusTable != NULL
					&& (size_t)effectStatus < g_pEffectStatusTable->GetSize() )
				{
					ACTIONINFO actionInfo = (ACTIONINFO) (*g_pEffectStatusTable)[effectStatus].OriginalActionInfo;
					switch(actionInfo)
					{
						//						case MAGIC_SACRIFICE :
					case MAGIC_AURA_SHIELD :
						if (g_pSkillInfoTable != NULL)
							g_pSkillInfoTable->SetAvailableTime(actionInfo);
						break;
					}
				}

				if( effectStatus == EFFECTSTATUS_FADE_OUT) 
					pCreature->SetVisibleSoon();
				
//				if( effectStatus == EFFECTSTATUS_REDIANCE && pCreature->IsSlayer() )
//				{
//					MCreatureWear* pCreatureWear = dynamic_cast<MCreatureWear*>( pCreature );
////					_MinTrace("Basic ActionInfo : %d %s\n",pCreatureWear->GetID(), pCreatureWear->GetName() );
//					
//					const MCreatureWear::ADDON_INFO &addonInfo = pCreatureWear->GetAddonInfo( ADDON_RIGHTHAND );
//					
//					if( addonInfo.bAddon )
//					{
////						_MinTrace("Basic ActionInfo : %d %d %d\n",addonInfo.ItemClass,addonInfo.ItemType);
//						if( addonInfo.ItemClass < g_pItemTable->GetSize() &&
//							addonInfo.ItemType < g_pItemTable[addonInfo.ItemClass].GetSize() &&
//							(*g_pItemTable)[addonInfo.ItemClass][addonInfo.ItemType].UseActionInfo != ACTIONINFO_NULL )
//							pCreature->SetBasicActionInfo( (*g_pItemTable)[addonInfo.ItemClass][addonInfo.ItemType].UseActionInfo );
//						else
//							pCreature->SetBasicActionInfo( SKILL_ATTACK_MELEE );
//					} else
//						pCreature->SetBasicActionInfo( SKILL_ATTACK_MELEE );
//				}
			}
		}
	}

#endif

	__END_CATCH
}
