//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddSlyph.cpp
// Written By  : rappi76
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAddSylph.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "EffectSpriteTypeDef.h"
#include "MEffectSpriteTypeTable.h"

void GCAddSylphHandler ::execute ( GCAddSylph* pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
#if __CONTENTS(__FAST_TRANSFORTER||__SECOND_TRANSFORTER)
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
		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());
		if (pCreature==NULL)
		{
			//이런 경우라면 아우가 몬스터로 추가 될 경우이다.
		}
		else
		{
			pCreature->SetWingType(pPacket->getSylphType());
#if __CONTENTS(__SECOND_TRANSFORTER)	
			pCreature->SetWingItemType(pPacket->getWingItemType());
#endif //__SECOND_TRANSFORTER
			pCreature->SetWingColor(pPacket->getWingBodyColor());
			pCreature->SetWingEffectColor(pPacket->getWingEffectColor());

			switch(pCreature->GetWingType())
			{
			case 0:
			case 1:
			case 2:
				{
#if __CONTENTS(__SECOND_TRANSFORTER)
					switch(pCreature->GetWingItemType())
					{
					case 0:
#endif //__SECOND_TRANSFORTER
						pCreature->SetWingEffect1();
						pCreature->AddEffectStatus(EFFECTSTATUS_CLIENT_WING_SYLPH ,0xFFFFFFFA);	//지속시간은 거의 최대값이다.
						pCreature->SetAction(ACTION_OUSTERS_WING_STAND);
					break;
#if __CONTENTS(__SECOND_TRANSFORTER)
					case 1:
						pCreature->SetWingEffect2();
						pCreature->AddEffectStatus(EFFECTSTATUS_CLIENT_UNICORN ,0xFFFFFFFA); //지속시간은 거의 최대값이다.
						pCreature->SetAction(ACTION_OUSTERS_UNICORN_STAND);
					break;
					case 2:
						pCreature->AddEffectStatus(EFFECTSTATUS_OusterWings_New_1 ,0xFFFFFFFA); //침쥣劤麟폄
						pCreature->SetAction(ACTION_OUSTERS_UNICORN_STAND);
						//pCreature->SetWingEffect3(0);
						break;
					case 3:
						pCreature->AddEffectStatus(EFFECTSTATUS_OusterWings_New_2 ,0xFFFFFFFA); //침쥣劤麟폄
						pCreature->SetAction(ACTION_OUSTERS_UNICORN_STAND);
						break;

					}
#endif //__SECOND_TRANSFORTER
				}
			default:
				break;
			}
		}

	}
#endif //__FAST_TRANSFORTER||__SECOND_TRANSFORTER
	__END_CATCH

}
