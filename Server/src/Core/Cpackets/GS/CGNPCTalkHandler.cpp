//////////////////////////////////////////////////////////////////////////////
// Filename    : CGNPCTalkHandler.cpp
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "CGNPCTalk.h"

#ifdef __GAME_SERVER__
	#include "GamePlayer.h"
	#include "NPC.h"
	#include "StringPool.h"

	#include "quest/TriggerManager.h"
	#include "quest/Trigger.h"
	#include "quest/Condition.h"
	#include "quest/Action.h"

	#include "Gpackets/GCNPCSayDynamic.h"
	#include "Gpackets/GCNPCResponse.h"
	#include "Gpackets/GCInstanceDoungeonLobby.h"

	#include "PlayerCreature.h"
	#include "GQuestManager.h"
	#include "InstanceDoungeonManager.h"
#endif

const string DiffClanSpeech[] = 
{
	"���! ���! ���� ���丮�� �������� ħ���ؿԴ�.",
	"����? �̰���.. ���丮�� �������� �ǳ���! ���� �׳���� �������� �Ƚ�ó���� �������� �Ա���!!",
	"�� ���� ������? ���� ���Ⱑ �����.. Abscede Hinc!!"
};

//////////////////////////////////////////////////////////////////////////////
// �÷��̾ NPC�� Ŭ���� ���, Ŭ���̾�Ʈ�� CGNPCTalk ��Ŷ�� ������
// �����Ѵ�. ������ �� ��Ŷ�� �ڵ鸵�� ��, NPC�� CONDITION_TALKED_BY 
// ����� �÷��װ� ���� ������, Ʈ���Ÿ� Ž���ϸ鼭 ������ Ʈ���Ÿ� ã�Ƽ�
// ������ �׼��� �����Ѵ�.
//////////////////////////////////////////////////////////////////////////////
void CGNPCTalkHandler::execute (CGNPCTalk* pPacket , Player* pPlayer)
	 throw (ProtocolException , Error)
{
	__BEGIN_TRY __BEGIN_DEBUG_EX
	__BEGIN_DEBUG

#ifdef __GAME_SERVER__

	try
	{
		Assert(pPacket != NULL);
		Assert(pPlayer != NULL);

		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
		PlayerCreature*   pPC         = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
		Assert( pPC != NULL );
		Zone*       pZone       = pPC->getZone();

		// 2026-05-12 NPC click debug — kaynak (client send / trigger fail / shop drop) lokalize
		cout << "[NPCTALK] in: player=" << pPC->getName()
			 << " objectID=" << (int)pPacket->getObjectID()
			 << " zone=" << (int)pZone->getZoneID() << endl;

		// �ϴ� ���� �Ŵ� PC�� ���� ����ִ� ���¿��� �Ѵ�.
		// �� �˻縦 �ϴ� ������ ���� �ߵ��Ǿ� �ִ� �����̾, �ð� ���� ���ؼ�
		// NPC���� HEAL�� ��û�� ���Ŀ�, ���� ���ؼ� �׾������,
		// �ϴ� �ٴڿ� �������� �Ǵµ�, ���̾� NPC�� ���ؼ� ġ�Ḧ �ް� �ȴ�.
		// �� ���, HP�� Ǯ�ε� �ٴڿ� �������ְ� �ȴ�.
		// �� ���� PCManager::heartbeat()���� killCreature�� ���� �ʰ� �ȴٴ� ���̴�.
		// ��. �� ������ ���� �ʴµ�, �Ͽ�ư NPC���� ���� �ɷ��� �÷��̾�� ����־�� �Ѵ�.
		if (pPC->isDead())
		{
			return;
		}

		Creature* pNPCBase = NULL;

		pNPCBase = pZone->getCreature(pPacket->getObjectID()); 

		if (pNPCBase==NULL || !pNPCBase->isNPC())
		{
			cout << "[NPCTALK] reject: pNPCBase NULL or !isNPC (objectID=" << (int)pPacket->getObjectID() << ")" << endl;
			GCNPCResponse gcNPCAskAnswer;
			pPlayer->sendPacket(&gcNPCAskAnswer);

			return;
		}

		NPC* pNPC = dynamic_cast<NPC*>(pNPCBase);
		cout << "[NPCTALK] resolved NPC: name=" << pNPC->getName() << " npcID=" << (int)pNPC->getNPCID() << endl;

		// ����Ʈ�� ���� ���϶� ^^;
		if ( pPC->getGQuestManager()->metNPC(pNPC) )
		{
			// �ϴ� Ŭ���̾�Ʈ�� ���� OK��Ŷ�� �� ������.
			GCNPCResponse gcNPCAskAnswer;
			pPlayer->sendPacket(&gcNPCAskAnswer);

			return;
		}

		// �ƴ��� ���������� NPC�� NP�� ������ �����Ѵ�.
		// �������� ������ NPC�� �ٸ���.
		if ( !pZone->isHolyLand() 
			|| pZone->isCastle() )
		{
			// �����̾ �����̾� NPC���� ���� �ɾ��� ��쿡 �����Ѵ�.
			if (pNPC->getRace() == NPC_RACE_SLAYER && pPC->isVampire())
			{
				// �ϴ� Ŭ���̾�Ʈ�� ���� OK��Ŷ�� �� ������.
				GCNPCResponse gcNPCAskAnswer;
				pPlayer->sendPacket(&gcNPCAskAnswer);

				// �����̾ �����̾� NPC���� ���� �ɸ�...
				GCNPCSayDynamic saypkt;
				saypkt.setObjectID(pNPC->getObjectID());
				// Event �� NPC �� ��� �ٸ� ���� �Ѵ� ;;
				if ( pNPC->getNPCID() == 639 )
				{
					saypkt.setMessage( g_pStringPool->c_str( STRID_EVENT_NPC_1 ) );
				}
				else
				{
					saypkt.setMessage( g_pStringPool->c_str( STRID_ALERT_VAMPIRE ) );
				}
				pPlayer->sendPacket(&saypkt);
				return;
			}
			// �ƿ콺�ͽ��� �����̾� NPC���� ���� �ɾ��� ��쿡 �����Ѵ�.
			else if (pNPC->getRace() == NPC_RACE_SLAYER && pPC->isOusters())
			{
				// �ϴ� Ŭ���̾�Ʈ�� ���� OK��Ŷ�� �� ������.
				GCNPCResponse gcNPCAskAnswer;
				pPlayer->sendPacket(&gcNPCAskAnswer);

				// �����̾ �����̾� NPC���� ���� �ɸ�...
				GCNPCSayDynamic saypkt;
				saypkt.setObjectID(pNPC->getObjectID());
				saypkt.setMessage( g_pStringPool->c_str( STRID_ALERT_OUSTERS_2 ) );
				pPlayer->sendPacket(&saypkt);
				return;
			}
			// �����̾ �����̾� NPC���� ���� �ɾ �����Ѵ�.
			else if (pNPC->getRace() == NPC_RACE_VAMPIRE && pPC->isSlayer())
			{
				// �ϴ� Ŭ���̾�Ʈ�� ���� OK��Ŷ�� �� ������.
				GCNPCResponse gcNPCAskAnswer;
				pPlayer->sendPacket(&gcNPCAskAnswer);

				// �����̾ �����̾� NPC���� ���� �ɸ�...
				GCNPCSayDynamic saypkt;
				saypkt.setObjectID(pNPC->getObjectID());
				// Event �� NPC �� ��� �ٸ� ���� �Ѵ� ;;
				if ( pNPC->getNPCID() == 638 )
				{
					saypkt.setMessage( g_pStringPool->c_str( STRID_EVENT_NPC_2 ) );
				}
				else if ( pNPC->getNPCID() == 636 )
				{
					saypkt.setMessage( g_pStringPool->c_str( STRID_EVENT_NPC_3 ) );
				}
				else if ( pNPC->getNPCID() == 635 )
				{
					saypkt.setMessage( g_pStringPool->c_str( STRID_EVENT_NPC_4 ) );
				}
				else
				{
					saypkt.setMessage( g_pStringPool->c_str( STRID_ALERT_SLAYER ) );
				}
				pPlayer->sendPacket(&saypkt);
				return;
			}
			// �ƿ콺�ͽ��� �����̾� NPC���� ���� �ɾ �����Ѵ�.
			else if (pNPC->getRace() == NPC_RACE_VAMPIRE && pPC->isOusters())
			{
				// �ϴ� Ŭ���̾�Ʈ�� ���� OK��Ŷ�� �� ������.
				GCNPCResponse gcNPCAskAnswer;
				pPlayer->sendPacket(&gcNPCAskAnswer);

				GCNPCSayDynamic saypkt;
				saypkt.setObjectID(pNPC->getObjectID());
				saypkt.setMessage( g_pStringPool->c_str( STRID_ALERT_OUSTERS ) );
				pPlayer->sendPacket(&saypkt);
				return;
			}
			// �����̾ �ƿ콺�ͽ� NPC���� ���� �ɾ �����Ѵ�.
			else if (pNPC->getRace() == NPC_RACE_OUSTERS && pPC->isSlayer())
			{
				// �ϴ� Ŭ���̾�Ʈ�� ���� OK��Ŷ�� �� ������.
				GCNPCResponse gcNPCAskAnswer;
				pPlayer->sendPacket(&gcNPCAskAnswer);

				GCNPCSayDynamic saypkt;
				saypkt.setObjectID(pNPC->getObjectID());
				saypkt.setMessage( g_pStringPool->c_str( STRID_ALERT_SLAYER_2 ) );
				pPlayer->sendPacket(&saypkt);
				return;
			}
			// �����̾ �ƿ콺�ͽ� NPC���� ���� �ɾ �����Ѵ�.
			else if (pNPC->getRace() == NPC_RACE_OUSTERS && pPC->isVampire())
			{
				// �ϴ� Ŭ���̾�Ʈ�� ���� OK��Ŷ�� �� ������.
				GCNPCResponse gcNPCAskAnswer;
				pPlayer->sendPacket(&gcNPCAskAnswer);

				GCNPCSayDynamic saypkt;
				saypkt.setObjectID(pNPC->getObjectID());
				saypkt.setMessage( g_pStringPool->c_str( STRID_ALERT_VAMPIRE_2 ) );
				pPlayer->sendPacket(&saypkt);
				return;
			}
		}

		// ���� �����̾���� Ŭ���� �ٸ� NPC�� ��� �����Ѵ�.
		// �� �κ��� �ڵ�� ���߿� �ٸ� ������� ����ؾ� �� ���̴�.
		// ����� Ŭ���� ���� �� ���� ���̱� ������...
		// .....
		// Ŭ�� �Ѱ��� �ۿ� ����~~
/*		if (pPC->isVampire() && pNPC->getRace() == NPC_RACE_VAMPIRE)
		{
			if (pPC->getClanType() != pNPC->getClanType())
			{
				GCNPCResponse gcNPCAskAnswer;
				pPlayer->sendPacket(&gcNPCAskAnswer);

				GCNPCSayDynamic saypkt;
				saypkt.setObjectID(pNPC->getObjectID());
				saypkt.setMessage(DiffClanSpeech[rand()%3]);
				pPlayer->sendPacket(&saypkt);
				return;
			}
		}
*/
		// �÷��̾ ���� �ɾ��� ��쿡 �����ϴ� �׼��� �ִ���, �ִٸ� �ൿ�� ���ϵ��� ��
		bool _triggerActivated = pNPC->activateTrigger(pPC, Condition::CONDITION_TALKED_BY, Trigger::PASSIVE_TRIGGER);
		cout << "[NPCTALK] activateTrigger result=" << (_triggerActivated ? "TRUE" : "FALSE")
			 << " npc=" << pNPC->getName() << endl;
		if ( !_triggerActivated )
		{
			cout << "[NPCTALK] -> sending QUIT_DIALOGUE (no trigger matched)" << endl;
			// Ŭ���̾�Ʈ�� NPC�� Ŭ������ ���, ������ ������ ������ ��ٸ��� ������, 
			// �׷� �׼��� �������� ���� �� �ƹ� �͵� ���� ������, Ŭ���̾�Ʈ�� ������ �����(?)
			// ���¿� ������ �ȴ�. �װ��� �����ϱ� ���ؼ� ��ȭâ�� ������� �޼����� �����ش�.
			GCNPCResponse gcNPCResponse;
			gcNPCResponse.setCode(NPC_RESPONSE_QUIT_DIALOGUE);
			pPlayer->sendPacket(&gcNPCResponse);
			
			// �δ� ���� �����Ǿ� �ִ� ��� �δ� ���� ���� ����
			if ( pNPC->getInstanceDoungeonScript().length() > 0)
			{
				InstanceDoungeonUnit *pInstanceDoungeonUnit = g_InstanceDoungeonManager.GetDoungeonUnit(pNPC->getInstanceDoungeonScript());
				
				if ( pInstanceDoungeonUnit == NULL )
				{
					return;
				}
				
				TInstanceDoungeonItemList *pItemList = pInstanceDoungeonUnit->GetDoungeonItemList();
				
				GCInstanceDoungeonLobby gcInstanceDoungeonLobby;
				
				gcInstanceDoungeonLobby.setObjectID(pNPC->getObjectID());
				gcInstanceDoungeonLobby.setNPCID(pNPC->getNPCID());
				
				gcInstanceDoungeonLobby.setDoungeonTitle(pInstanceDoungeonUnit->GetTitle());
				gcInstanceDoungeonLobby.setDoungeonDesc(pInstanceDoungeonUnit->GetDesc());
				
				InstanceDoungeonInfo *pInstanceDoungeonInfo;
				
				TInstanceDoungeonItemList::iterator iter;
				iter = pItemList->begin();
				for( ; iter != pItemList->end(); ++iter)
				{
					pInstanceDoungeonInfo = new InstanceDoungeonInfo;
					
					pInstanceDoungeonInfo->setDoungeonIndex((*iter).GetIndex());
					pInstanceDoungeonInfo->setDoungeonName((*iter).GetName());
					
					gcInstanceDoungeonLobby.addDoungeonInfo(pInstanceDoungeonInfo);
				}
				
				pPlayer->sendPacket(&gcInstanceDoungeonLobby);
			}
		}
	} 
	catch (Throwable & t) 
	{
		//cerr << "���� NPC ������Ʈ ���̵� �̻��ϰ� �����ֳ�?" << endl;
		//cerr << t.toString() << endl;
	}
	
#endif

	__END_DEBUG
	__END_DEBUG_EX __END_CATCH
}
