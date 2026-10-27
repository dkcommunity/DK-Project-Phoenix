//////////////////////////////////////////////////////////////////////////////
// Filename    : PCManager.cpp
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include <algorithm>		// find_if ()
#include <stdio.h>
#include "Assert.h"
#include "PCManager.h"
#include "GamePlayer.h"
#include "PlayerCreature.h"
#include "Creature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Tile.h"
#include "Zone.h"
#include "MonsterCorpse.h"
#include "GamePlayer.h"
#include "ZoneGroupManager.h"
#include "ZoneInfo.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "IncomingPlayerManager.h"
#include "TimeManager.h"
#include "Event.h"
#include "EventResurrect.h"
#include "ResurrectLocationManager.h"
#include "ItemUtil.h"
#include "AlignmentManager.h"
#include "ItemFactoryManager.h"
#include "SlayerCorpse.h"
#include "VampireCorpse.h"
#include "OustersCorpse.h"
#include "LogClient.h"
#include "Thread.h"
#include "TradeManager.h"
#include "SkillHandlerManager.h"
#include "Relic.h"
#include "RelicUtil.h"
#include "ItemInfoManager.h"
#include "Inventory.h"
#include "CreatureUtil.h"
#include "PacketUtil.h"
#include "ZoneUtil.h"
#include "PKZoneInfoManager.h"
#include "BloodBibleBonusManager.h"
#include "HolyLandManager.h"
//#include "SweeperBonusManager.h"
#include "LevelWarZoneInfoManager.h"
#include "war/WarSystem.h"
#include "EventZoneInfo.h"
#include "GQuestManager.h"
#include "SiegeManager.h"

#include "Properties.h"

#include "EffectComa.h"
#include "EventTransport.h"
#include "EffectPKZoneResurrection.h"

#include "EffectTryingPosition.h"

#include "EffectSummonCasket.h"
#include "skill/EffectHarpoonBomb.h"

#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCCreatureDied.h"
#include "Gpackets/GCGetOffMotorCycle.h"
#include "Gpackets/GCSystemMessage.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCRemoveFromGear.h"
#include "Gpackets/GCHolyLandBonusInfo.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffectToTile.h"
//#include "Gpackets/GCSweeperBonusInfo.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
PCManager::PCManager () 
	throw ()
{
    __BEGIN_TRY

	m_bRefreshHolyLandPlayer = false;
//	m_bRefreshLevelWarBonusZonePlayer = false;

    __END_CATCH
}

	
//////////////////////////////////////////////////////////////////////////////
// destructor
// 컨테이너의 모든 객체들을 DB에 저장한 후, 삭제한다.
//////////////////////////////////////////////////////////////////////////////
PCManager::~PCManager () 
	throw ()
{
    __BEGIN_TRY
    __END_CATCH
}
 

//////////////////////////////////////////////////////////////////////////////
// 존에 소속된 PC 들의 heartbeat 관련 메쏘드들을 실행해준다.
// 이때, PC가 죽었더라도 삭제하면 안된다. 왜냐하면, PC 가 PC 매니저에서
// 삭제되면 PC's EffectManager 의 heartbeat 메쏘드가 호출되지 않기 때문이다.
// 대신 CreatureDead::unaffect()에서 없에주면 딱 알맞겠다.
//
// 여기선 이펙트 아이템에 대한 처리를 프로세싱 하는 부분이다.
// 이 함수는 Zone의 Heart Beat에서 처리되는 루틴이다.
// 다른 쓰레드와의 관계를 살펴 볼때, ProcessCommand가 끝난 상황에서 처리된다.
// 그러나 다른 쓰레드에서 PCManager의 deleteCreature 또는 addCreature를
// 하게 되면 이 함수 또한 안전하지 못할 것이다.
//////////////////////////////////////////////////////////////////////////////
void PCManager::processCreatures ()
    throw (Error)
{
    __BEGIN_TRY

	__ENTER_CRITICAL_SECTION(m_Mutex)

	//cerr << "TID[" << Thread::self() << "]" << "PCM::process go" << endl;

	Timeval currentTime;
	getCurrentTime(currentTime);

	try
	{
		map< ObjectID_t , Creature* >::iterator before  = m_Creatures.end();
		map< ObjectID_t , Creature* >::iterator current = m_Creatures.begin(); 
		
		while (current != m_Creatures.end()) 
		{
			Creature* pCreature = current->second;
			
			if ( pCreature == NULL )
			{
				filelog("PCManagerBug.log", "ProcessCreatureBug : pCreature == NULL ");
				
				if (before == m_Creatures.end()) 	// first element
				{
					current = m_Creatures.begin();
				}
				else // !first element
				{
					current = before;
					current ++;
				}
				
				continue;
			}

			// 가지고 있는 아이템의 heartbeat.. Inventory, Gear 검색
			if (pCreature->isSlayer())
			{
				Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
				Assert(pSlayer != NULL);
				pSlayer->heartbeat(currentTime);

				/*
				// 어딘가에서 코마 상태에 걸려있는데 HP가 올라가는 현상이 발생하는 것 같다.
				// 그래서 코마 상태에 걸려있으면 일단 HP를 무조건 0으로 세팅하도록 변경한다.
				if (pSlayer->isFlag(Effect::EFFECT_CLASS_COMA))
					pSlayer->setHP(0, ATTR_CURRENT);
				*/
				if (pSlayer->isFlag(Effect::EFFECT_CLASS_COMA))
				{
					HP_t maxHP 		= pSlayer->getHP(ATTR_MAX);
	                                pSlayer->setHP( maxHP );
					if (pSlayer->isFlag(Effect::EFFECT_CLASS_DEPENDSOUL))
					{
						//cout << "Target HP is over 1/3" << endl;

						EffectComa* pEffectComa = dynamic_cast<EffectComa*>(pSlayer->findEffect(Effect::EFFECT_CLASS_COMA));
						Assert(pEffectComa != NULL);

						if (pEffectComa->canResurrect())
						{
							//cout << "Can Resurrect!" << endl;
                                                    if(!pSlayer->isFlag( Effect::EFFECT_CLASS_REQUEST_RESURRECT ) || !pSlayer->isFlag(Effect::EFFECT_CLASS_ETERNITY))
                                                      {
							// 타겟의 이펙트 매니저에서 코마 이펙트를 삭제한다.
							pSlayer->deleteEffect(Effect::EFFECT_CLASS_COMA);
							pSlayer->removeFlag(Effect::EFFECT_CLASS_COMA);
							pSlayer->deleteEffect(Effect::EFFECT_CLASS_DEPENDSOUL);
							pSlayer->removeFlag(Effect::EFFECT_CLASS_DEPENDSOUL);

							// 코마 이펙트가 날아갔다고 알려준다.
							GCRemoveEffect gcRemoveEffect;
							gcRemoveEffect.setObjectID(pSlayer->getObjectID());
							gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
							pSlayer->getZone()->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcRemoveEffect);

							GCAddEffectToTile gcAddEffect;
							gcAddEffect.setEffectID(Effect::EFFECT_CLASS_ETERNITY);
							gcAddEffect.setXY( pSlayer->getX(), pSlayer->getY() );
							gcAddEffect.setDuration(31);
							pSlayer->getZone()->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcAddEffect);

							GCStatusCurrentHP gcHP;
							gcHP.setObjectID( pSlayer->getObjectID() );
							gcHP.setCurrentHP( pSlayer->getHP() );
							pSlayer->getZone()->broadcastPacket(pSlayer->getX(), pSlayer->getY(), &gcHP);

							// 부활하고 나서 effect 붙여준다.
							pSlayer->getEffectManager()->sendEffectInfo(pSlayer, pSlayer->getZone(), pSlayer->getX(), pSlayer->getY());

							int CDR = 200;
							if (pSlayer->hasRankGem(NewRankGem::SKILL_DEPEND_SOUL_DAM ) )
							{
								NewRankGem* pRankGemDEPEND = pSlayer->getRankGem( NewRankGem::SKILL_DEPEND_SOUL_DAM );
								Assert( pRankGemDEPEND != NULL );

								CDR=150;
							}
                                                        addSimpleCreatureEffect(pSlayer, Effect::EFFECT_CLASS_GOLD_MEDAL, CDR);
                                                       }
						}
					}
				}
				// HolyLandRaceBonus 적용을 위해 initAllStat을 부른다.
				if ( m_bRefreshHolyLandPlayer && !g_pWarSystem->hasActiveRaceWar() )
				{
					pSlayer->initAllStatAndSend();
				}
			}
			else if (pCreature->isVampire())
			{
				Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
				Assert(pVampire != NULL);
				pVampire->heartbeat(currentTime);

				/*
				// 어딘가에서 코마 상태에 걸려있는데 HP가 올라가는 현상이 발생하는 것 같다.
				// 그래서 코마 상태에 걸려있으면 일단 HP를 무조건 0으로 세팅하도록 변경한다.
				if (pVampire->isFlag(Effect::EFFECT_CLASS_COMA))
					pVampire->setHP(0, ATTR_CURRENT);
				*/
				if (pVampire->isFlag(Effect::EFFECT_CLASS_COMA))
				{
					HP_t currentHP 	= pVampire->getHP(ATTR_CURRENT);
					HP_t maxHP 		= pVampire->getHP(ATTR_MAX);

					if (currentHP*3 >= maxHP)
					{
						//cout << "Target HP is over 1/3" << endl;

						EffectComa* pEffectComa = dynamic_cast<EffectComa*>(pVampire->findEffect(Effect::EFFECT_CLASS_COMA));
						Assert(pEffectComa != NULL);

						if (pEffectComa->canResurrect())
						{
							//cout << "Can Resurrect!" << endl;

							// 타겟의 이펙트 매니저에서 코마 이펙트를 삭제한다.
							pVampire->deleteEffect(Effect::EFFECT_CLASS_COMA);
							pVampire->removeFlag(Effect::EFFECT_CLASS_COMA);

							// 코마 이펙트가 날아갔다고 알려준다.
							GCRemoveEffect gcRemoveEffect;
							gcRemoveEffect.setObjectID(pVampire->getObjectID());
							gcRemoveEffect.addEffectList((EffectID_t)Effect::EFFECT_CLASS_COMA);
							pVampire->getZone()->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcRemoveEffect);

							// 부활하고 나서 effect 붙여준다.
							pVampire->getEffectManager()->sendEffectInfo(pVampire, pVampire->getZone(), pVampire->getX(), pVampire->getY());
						}
					}
				}

				// HolyLandRaceBonus 적용을 위해 initAllStat을 부른다.
				if ( m_bRefreshHolyLandPlayer && !g_pWarSystem->hasActiveRaceWar() )
				{
					pVampire->initAllStatAndSend();
				}
			}
			else if ( pCreature->isOusters() )
			{
				Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
				Assert(pOusters != NULL);
				pOusters->heartbeat(currentTime);
			}

			if (pCreature->isDead() || pCreature->isFlag(Effect::EFFECT_CLASS_COMA) )
			{
				if (!pCreature->isFlag(Effect::EFFECT_CLASS_COMA))
				{
					///////////////////////////////////////////////////////////////////
					// 죽을 때 Relic Item을 가지고 있다면 바닥에 떨어뜨린다.
					///////////////////////////////////////////////////////////////////
					dropRelicToZone(pCreature);
					dropFlagToZone(pCreature);
					dropSweeperToZone( pCreature );

					///////////////////////////////////////////////////////////////////
					// 죽을 때 성향에 따라서 아이템을 떨어뜨린다.
					///////////////////////////////////////////////////////////////////
					Zone* pZone = pCreature->getZone();
					Assert(pZone != NULL);

					Tile& rTile = pZone->getTile( pCreature->getX(), pCreature->getY() );
					EffectTryingPosition* pTryingTile = dynamic_cast<EffectTryingPosition*>(rTile.getEffect( Effect::EFFECT_CLASS_TRYING_POSITION ));
					if ( pTryingTile != NULL )
					{
						MonsterCorpse* pTower = pTryingTile->getTower();
						Assert( pTower != NULL );

						Effect* pTryingTower = pTower->getEffectManager().findEffect( Effect::EFFECT_CLASS_SLAYER_TRYING_1 );
						if ( pTryingTower != NULL ) pTryingTower->setDeadline(0);

						pTryingTower = pCreature->findEffect(Effect::EFFECT_CLASS_TRYING);
						if ( pTryingTower != NULL ) pTryingTower->setDeadline(0);
					}

					Slayer*  pSlayer = NULL;
					Vampire* pVampire= NULL;
					Ousters* pOusters= NULL;

					Creature::CreatureClass CClass = pCreature->getCreatureClass();

					if (CClass == Creature::CREATURE_CLASS_SLAYER)
					{
						pSlayer = dynamic_cast<Slayer*>(pCreature);
						//pCorpse = new SlayerCorpse(dynamic_cast<Slayer*>(pCreature));

						int SumAttr = pSlayer->getSTR(ATTR_BASIC) + pSlayer->getDEX(ATTR_BASIC) + pSlayer->getINT(ATTR_BASIC);

						// 섬이 40 이상이고, 일반 유저라면 죽을 때 아이템을 떨어뜨릴 확률이 있다.
						if (SumAttr > 40 && pSlayer->getCompetence() == 3)
						{
							Alignment_t alignment   = pSlayer->getAlignment();
							ItemNum_t   DropItemNum = g_pAlignmentManager->getDropItemNum(alignment, pSlayer->isPK());

							// 아이템을 떨어뜨리는 대신 입고 있는 아이템의 내구도를 모두 0으로 바꾼다.
							// 2006.02.27. 영등위 심의 관련 변경 by bezz
							if ( DropItemNum > 0 )
							{
								GCModifyInformation gcModifyInformation;
								ulong value = 0;

								for ( int i = 0; i < Slayer::WEAR_MAX; ++i )
								{
									Item* pItem = pSlayer->getWearItem( (Slayer::WearPart)i );

									if ( pItem != NULL
										&& canDecreaseDurability( pItem )
									   )
									{
										pItem->setDurability( 0 );

										value = (DWORD) i << 24 | (DWORD)(0);
										gcModifyInformation.addLongData( MODIFY_DURABILITY, value );
									}
								}

								SLAYER_RECORD prev;
								pSlayer->getSlayerRecord( prev );
								pSlayer->initAllStat();
								pSlayer->sendRealWearingInfo();
								pSlayer->addModifyInfo( prev, gcModifyInformation );

								pSlayer->getPlayer()->sendPacket( &gcModifyInformation );
							}
						}

						// PK 당한것을 재 셋팅 해 줘야 한다.
						pSlayer->setPK(false);
					}
					else if (CClass == Creature::CREATURE_CLASS_VAMPIRE)
					{
						pVampire = dynamic_cast<Vampire*>(pCreature);
						//pCorpse = new VampireCorpse(dynamic_cast<Vampire*>(pCreature));

						// 숨어있었을 경우 죽을때 튀어나온다.
						// 2003. 1. 17. Sequoia, DEW
						if( pVampire->isFlag( Effect::EFFECT_CLASS_HIDE ) )
						{
							// canUnburrow 에서 실패할경우 viewable hide 가 안 풀릴 수 있다.
							// canUnburrow 같은 타일에서 체크하는 경우이다. addUnburrowCreature 안에서 근처에 가능한 타일을 찾아서 처리한다.
							//if( canUnburrow( pZone, pVampire->getX(), pVampire->getY() ) )
							//{
							addUnburrowCreature( pZone, pVampire, pVampire->getX(), pVampire->getY(), pVampire->getDir() );
							//}
						}

						// 박쥐나 늑대 상태인 뱀파이어는 죽을 때 원래대로 돌아간다.
						if ( pVampire->isFlag( Effect::EFFECT_CLASS_TRANSFORM_TO_BAT ) ||
							 pVampire->isFlag( Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF ) ||
							 pVampire->isFlag( Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF ) )
						{
							Zone* pZone = pVampire->getZone();
							Assert(pZone != NULL);
							addUntransformCreature(pZone, pVampire, true);
						}

						// 관 안에 있던 뱀파이어는 죽을 때 관이 없어진다.
/*						if ( pVampire->isFlag( Effect::EFFECT_CLASS_CASKET ) )
						{
							EffectSummonCasket* pEffect = dynamic_cast<EffectSummonCasket*>( pVampire->findEffect( Effect::EFFECT_CLASS_CASKET ) );
							Assert( pEffect != NULL );

							pEffect->setDeadline(0);
						}*/

						// 성향에 따라서 돈과 아이템을 떨어트린다.
						if (pVampire->getLevel() > 10 && pVampire->getCompetence() == 3) 
						{
							Alignment_t alignment   = pVampire->getAlignment();
							ItemNum_t   DropItemNum = g_pAlignmentManager->getDropItemNum(alignment, pVampire->isPK());

							// 아이템을 떨어뜨리는 대신 입고 있는 아이템의 내구도를 모두 0으로 바꾼다.
							// 2006.02.27. 영등위 심의 관련 변경 by bezz
							if ( DropItemNum > 0 )
							{
								GCModifyInformation gcModifyInformation;
								ulong value = 0;

								for ( int i = 0; i < Vampire::VAMPIRE_WEAR_MAX; ++i )
								{
									Item* pItem = pVampire->getWearItem( (Vampire::WearPart)i );

									if ( pItem != NULL
										&& canDecreaseDurability( pItem )
									   )
									{
										pItem->setDurability( 0 );

										value = (DWORD) i << 24 | (DWORD)(0);
										gcModifyInformation.addLongData( MODIFY_DURABILITY, value );
									}
								}

								VAMPIRE_RECORD prev;
								pVampire->getVampireRecord( prev );
								pVampire->initAllStat();
								pVampire->sendRealWearingInfo();
								pVampire->addModifyInfo( prev, gcModifyInformation );

								pVampire->getPlayer()->sendPacket( &gcModifyInformation );
							}
						}

						SkillHandler * pSkillHandler = g_pSkillHandlerManager->getSkillHandler(SKILL_EXTREME);
						Assert( pSkillHandler != NULL );
						// 익스트림 걸어주기~ 분노~~
						pSkillHandler->execute( pVampire );

						// PK 당한것을 재 셋팅 해 줘야 한다.
						pVampire->setPK(false);
					}
					else if (CClass == Creature::CREATURE_CLASS_OUSTERS)
					{
						pOusters = dynamic_cast<Ousters*>(pCreature);

						// 성향에 따라서 돈과 아이템을 떨어트린다.
						if (pOusters->getLevel() > 10 && pOusters->getCompetence() == 3) 
						{
							Alignment_t alignment   = pOusters->getAlignment();
							ItemNum_t   DropItemNum = g_pAlignmentManager->getDropItemNum(alignment, pOusters->isPK());

							// 아이템을 떨어뜨리는 대신 입고 있는 아이템의 내구도를 모두 0으로 바꾼다.
							// 2006.02.27. 영등위 심의 관련 변경 by bezz
							if ( DropItemNum > 0 )
							{
								GCModifyInformation gcModifyInformation;
								ulong value = 0;

								for ( int i = 0; i < Ousters::OUSTERS_WEAR_MAX; ++i )
								{
									Item* pItem = pOusters->getWearItem( (Ousters::WearPart)i );

									if ( pItem != NULL
										&& canDecreaseDurability( pItem )
									   )
									{
										pItem->setDurability( 0 );

										value = (DWORD) i << 24 | (DWORD)(0);
										gcModifyInformation.addLongData( MODIFY_DURABILITY, value );
									}
								}

								OUSTERS_RECORD prev;
								pOusters->getOustersRecord( prev );
								pOusters->initAllStat();
								pOusters->sendRealWearingInfo();
								pOusters->addModifyInfo( prev, gcModifyInformation );

								pOusters->getPlayer()->sendPacket( &gcModifyInformation );
							}
						}

						// PK 당한것을 재 셋팅 해 줘야 한다.
						pOusters->setPK(false);
					}
					else
					{
						throw Error("invalid creature class");
					}

					// 제일 처음에 죽을 때에는 COMA가 걸려 있지 않으므로,
					// 이 부분에 걸려서 COMA가 걸리게 된다.
					EffectComa* pEffectComa = new EffectComa(pCreature);
					pEffectComa->setStartTime();
					if ( pTryingTile != NULL ) 
						pEffectComa->setDeadline(0);
					else if ( pCreature->isFlag( Effect::EFFECT_CLASS_HARPOON_BOMB ) )
					{
						// Harpoon Bomb 이펙트가 걸려있으면 코마상태 유지를 최소화하도록 하여
						// 죽었을때의 처리만하고 바로 PC 를 귀환시켜버린다.
//						pEffectComa->setDeadline(0);
					}
					else 
						pEffectComa->setDeadline(600);

					EffectManager* pEffectManager = pCreature->getEffectManager();
					Assert(pEffectManager != NULL);
					pEffectManager->addEffect(pEffectComa);

					pCreature->setFlag(Effect::EFFECT_CLASS_COMA);

					PlayerCreature* pPlayerCreature = dynamic_cast<PlayerCreature*>(pCreature);
					Assert(pPlayerCreature!=NULL);

					pPlayerCreature->getGQuestManager()->killed();
					pPlayerCreature->OnDead();

					// 바닥에 쓰러뜨리라고, 이펙트를 뿌려준다.
					GCAddEffect gcAddEffect;
					gcAddEffect.setObjectID(pCreature->getObjectID());
					gcAddEffect.setEffectID(Effect::EFFECT_CLASS_COMA);
					gcAddEffect.setDuration(300);
					pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcAddEffect);

					if ( g_pConfig->hasKey("Hardcore") && g_pConfig->getPropertyInt("Hardcore")!=0 )
					{
						PlayerCreature* pPC = dynamic_cast<PlayerCreature*>( pCreature );
						Assert( pPC != NULL );

						deletePC( pPC );
						GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPC->getPlayer());
						Assert( pGamePlayer != NULL );

						filelog("DeletePC.log", "죽어서 지워집니다 : %s", pPC->getName().c_str() );

						pGamePlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
						pGamePlayer->setItemRatioBonusPoint(1);
					}

					before = current ++;
				}
				else
				{
					EffectManager* pEffectManager = pCreature->getEffectManager();
					Assert(pEffectManager != NULL);

					EffectComa* pEffectComa = (EffectComa*)(pEffectManager->findEffect(Effect::EFFECT_CLASS_COMA));
					Assert(pEffectComa != NULL);

					if (pEffectComa->getDeadline() < currentTime)
					{
						// 먼저 이펙트를 삭제한다.
						pEffectManager->deleteEffect(pCreature, Effect::EFFECT_CLASS_COMA);
						pCreature->removeFlag(Effect::EFFECT_CLASS_COMA);

						// 사용자의 입력에 의해서 COMA 이펙트가 unaffect되면, 진짜로 죽은 것이 된다.
						// 그러므로, PC 를 죽이고, 플레이어를 ZPM -> IPM 으로 옮긴다.
						killCreature(pCreature);

						// PC 의 노드를 삭제한다.
						m_Creatures.erase(current);

						if (before == m_Creatures.end()) 	// first element
						{
							current = m_Creatures.begin();
						}
						else // !first element
						{
							current = before;
							current ++;
						}
					}
					else
					{
//						Effect* pHarpoonBomb = pEffectManager->findEffect( Effect::EFFECT_CLASS_HARPOON_BOMB );
//						if ( pHarpoonBomb != NULL && pHarpoonBomb->getNextTime() < currentTime )
//						{
//							pHarpoonBomb->affect();
//						}
//
						before = current ++;
					}
				}
			}
			else
			{
				before = current ++;
				
				// 크리처에 걸려 있는 이펙트들을 실행시킨다.
				pCreature->getEffectManager()->heartbeat( currentTime );
			}
		}

		// 이거 while 루프 안에 있었다 ㅜ.ㅠ
		// 전쟁중에는 안 보내주도록 수정
		if ( m_bRefreshHolyLandPlayer && !g_pWarSystem->hasActiveRaceWar() )
		{
			// 아담의 성지 전역에 피의 성서 보너스 정보를 뿌린다.
			GCHolyLandBonusInfo gcHolyLandBonusInfo;
			g_pBloodBibleBonusManager->makeHolyLandBonusInfo( gcHolyLandBonusInfo );
			g_pHolyLandManager->broadcast( &gcHolyLandBonusInfo );

		}

		/*
		if ( m_bRefreshLevelWarBonusZonePlayer && g_pSweeperBonusManager->isAble() )
		{
			GCSweeperBonusInfo gcSweeperBonusInfo;
			g_pSweeperBonusManager->makeSweeperBonusInfo( gcSweeperBonusInfo );
			g_pLevelWarZoneInfoManager->broadcast( &gcSweeperBonusInfo );
		}

		if ( m_bRefreshLevelWarBonusZonePlayer && !g_pSweeperBonusManager->isAble() )
		{
			GCSweeperBonusInfo gcSweeperBonusInfo;
			g_pSweeperBonusManager->makeVoidSweeperBonusInfo( gcSweeperBonusInfo );
			g_pLevelWarZoneInfoManager->broadcast( &gcSweeperBonusInfo );
		}
		*/

		m_bRefreshHolyLandPlayer = false;
	//	m_bRefreshLevelWarBonusZonePlayer = false;
		
	}
	catch (Throwable & t)
	{
		filelog("PCManagerBug.log", "ProcessCreatureBug : %s", t.toString().c_str());
		//cerr << t.toString() << endl;
	}

	//cerr << "TID[" << Thread::self() << "]" << "PCM::process end" << endl;
	
	__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// (1) 타일에서 크리처를 삭제한다.
// (2) 시체를 생성하고, 인벤토리를 시체로 옮긴다.
// (3) 타일에 아이템이 존재할 경우, 적절히 처리한다.
// (4) 시체를 타일에 추가한다. 중복시, 그 옆에 놓는다.
// (5) 부활 이펙트를 크리처에 추가한다.
//////////////////////////////////////////////////////////////////////////////
void PCManager::killCreature (Creature* pDeadCreature)
	throw (Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Assert(pDeadCreature != NULL);

	// transfusion때문에 죽은 뒤에도 HP찬다..
	// 무시. by sigi. 2002.10.8 
	//Assert(pDeadCreature->isDead());

	Zone* pZone = pDeadCreature->getZone();
	Assert(pZone != NULL);

	ZoneCoord_t cx = pDeadCreature->getX();
	ZoneCoord_t cy = pDeadCreature->getY();

	// Eternity
	pDeadCreature->removeFlag( Effect::EFFECT_CLASS_ETERNITY );

	// PK존에서 죽었을 때에는 살아날 때 이펙트가 붙는다.
	if ( g_pPKZoneInfoManager->isPKZone( pZone->getZoneID() ) )
	{
		EffectPKZoneResurrection* pEffect = new EffectPKZoneResurrection( pDeadCreature );
		pDeadCreature->addEffect( pEffect );
		pDeadCreature->setFlag( pEffect->getEffectClass() );
	}

	// 파티 초대중이라면 PartyInviteInfo를 삭제해준다.
	PartyInviteInfoManager* pPIIM = pZone->getPartyInviteInfoManager();
	pPIIM->cancelInvite(pDeadCreature);

	// 먼저 로컬 파티 매니저에서 죽은 놈을 지워줘야 한다.
	uint PartyID = pDeadCreature->getPartyID();
	if (PartyID != 0)
	{
		LocalPartyManager* pLPM = pZone->getLocalPartyManager();
		pLPM->deletePartyMember(PartyID, pDeadCreature);
	}

	// 트레이드 중이었다면 트레이드 관련 정보를 삭제해준다.
	TradeManager* pTradeManager = pZone->getTradeManager();
	TradeInfo* pInfo = pTradeManager->getTradeInfo(pDeadCreature->getName());
	if (pInfo != NULL)
	{
		pTradeManager->cancelTrade(pDeadCreature);
	}

	// 시체에 붙어있는 EFFECT_CLASS_CANNOT_ABSORB_SOUL 이펙트를 제거한다
	if (pDeadCreature->isFlag(Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL))
	{
		pDeadCreature->removeFlag(Effect::EFFECT_CLASS_CANNOT_ABSORB_SOUL);
	}

	bool addCorpse = true;
	if ( pDeadCreature->isFlag( Effect::EFFECT_CLASS_HARPOON_BOMB ) )
	{
		Effect* pEffect = pDeadCreature->findEffect( Effect::EFFECT_CLASS_HARPOON_BOMB );
		if ( pEffect == NULL ) addCorpse = false;
	}

	if ( addCorpse )
	{
		// 시체를 생성한다.
		Corpse*  pCorpse = NULL;

		if (pDeadCreature->isSlayer())
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pDeadCreature);
			pCorpse = new SlayerCorpse( pSlayer );
			pCorpse->setLevel( (int)(pSlayer->getHighestSkillDomainLevel()) );
			pCorpse->setExp( (Exp_t)computeCreatureExp(pSlayer, BLOODDRAIN_EXP) );
		}
		else if (pDeadCreature->isVampire())
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pDeadCreature);
			pCorpse = new VampireCorpse( pVampire );
			pCorpse->setLevel( (int)(pVampire->getLevel()) );
			pCorpse->setExp( (Exp_t)computeCreatureExp(pVampire, BLOODDRAIN_EXP) );
		}
		else if (pDeadCreature->isOusters())
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pDeadCreature);
			pCorpse = new OustersCorpse( pOusters );
			pCorpse->setLevel( (int)(pOusters->getLevel()) );
			pCorpse->setExp( (Exp_t)computeCreatureExp(pOusters, BLOODDRAIN_EXP) );
		}
		else
		{
			throw Error("PlayerCreature class type 오류");
		}

		// by sigi. 2002.12.12
		addCorpseToZone(pCorpse, pZone, cx, cy);

		if ( pDeadCreature->isFlag( Effect::EFFECT_CLASS_HARPOON_BOMB ) )
		{
			EffectHarpoonBomb* pEffect = dynamic_cast<EffectHarpoonBomb*>(pDeadCreature->findEffect( Effect::EFFECT_CLASS_HARPOON_BOMB ));
			if ( pEffect != NULL )
			{
				// 죽을때 Harpoon Bomb 이펙트가 걸려있으면
				// 존에 Harpoon Bomb 이펙트를 걸어준다. 이때 바로 affect 가 되도록 하여 토지도록한다.
				// affect 안에서 deadline 을 0으로 만들어 바로 지워지도록 한다.
				EffectHarpoonBomb* pZoneEffect = new EffectHarpoonBomb( pZone, pCorpse->getX(), pCorpse->getY() );
				pZoneEffect->setDamage( pEffect->getDamage() );
				pZoneEffect->setUserObjectID( pEffect->getUserObjectID() );
				pZoneEffect->setNextTime( 0 );
				pZoneEffect->setDeadline( 999999999 );
//				pDeadCreature->deleteEffect( Effect::EFFECT_CLASS_HARPOON_BOMB );
				pEffect->setDeadline(0);
				pZone->registerObject( pZoneEffect );
				pZone->getTile( pCorpse->getX(), pCorpse->getY() ).addEffect( pZoneEffect );
				pZone->addEffect( pZoneEffect );
			}
		}
	}

   	// 크리처가 죽었다고 주변에 알려준다.
    GCCreatureDied gcCreatureDied;
	gcCreatureDied.setObjectID(pDeadCreature->getObjectID());
	pDeadCreature->getPlayer()->sendPacket(&gcCreatureDied);
	pZone->broadcastPacket(cx , cy , &gcCreatureDied, pDeadCreature);

	// 타일에서 Creature를 지워준다. 
	// 단 PCManager 에서 삭제하면 PC's EM's heartbeat 가 호출되지 않으니 주의하기 바란다.
	Tile & tile = pZone->getTile(cx , cy);
	Assert(tile.getCreature(pDeadCreature->getMoveMode()) == pDeadCreature);
	tile.deleteCreature(pDeadCreature->getObjectID());

	/*
	// 밑으로 옮긴다.	by sigi. 2002.5.11
	// Resurrect 이벤트를 플레이어 객체에 연관시킨다.
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pDeadCreature->getPlayer());
	EventResurrect* pEventResurrect = new EventResurrect(pGamePlayer);
	pEventResurrect->setDeadline(0);

	// 원래 있었던 장소를 셋팅한다.
	pEventResurrect->setOldZone(pDeadCreature->getZone());

	// Player에 Event를 붙인다.
	pGamePlayer->addEvent(pEventResurrect);
	*/

	// *NOTE
	// 강제 접속 종료일 경우 목표 지점을 미리 지정해 놓기 위한 방법이다.
	// Resurrect처리 관계와 밀접한 부분이므로 이 부분이 바뀔 경우 Resurrect도 생각해야
	// 한다. 단순히 DB세이브만 하면 된다고 생각하기 쉽지만. GamePlayer가 Disconnect
	// 하면서 크리쳐를 세이브하기 때문에 다시 덮어서 세이브하게 되므로 성립이 되지 않는다.
	ZoneID_t     ZoneID = 0;
	ZoneCoord_t  ZoneX  = 0;
	ZoneCoord_t  ZoneY  = 0;
	ZONE_COORD   ResurrectCoord;
	Zone*		 pResurrectZone = NULL;

	// 도착존이 어느 서버, 어느 존그룹에 속하는지 알아본다.
	// 원래는 모든 존은 그 존에서 죽었을때 돌아가야 할 존을 명시해줘야 한다.
	ZoneInfo*  pZoneInfo  = NULL;
	ZoneGroup* pZoneGroup = NULL;

/*	if (pDeadCreature->isSlayer()) 
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pDeadCreature);
		Assert(pSlayer != NULL);

		pSlayer->setStashStatus(false);
		pSlayer->setHP(pSlayer->getHP(ATTR_MAX), ATTR_CURRENT);

		// 능력치 총합이 40이하라면, 무조건 야전사령부에서 태어나게 된다.
		int SumAttr = pSlayer->getSTR(ATTR_BASIC) + pSlayer->getDEX(ATTR_BASIC) + pSlayer->getINT(ATTR_BASIC);
		if (SumAttr <= 40)
		{
			pSlayer->setResurrectZoneID(2101);
		}

		/o
		try
		{
			ResurrectCoord = g_pResurrectLocationManager->getSlayerPosition(pSlayer->getResurrectZoneID());
			ZoneID         = ResurrectCoord.id;
			ZoneX          = ResurrectCoord.x;
			ZoneY          = ResurrectCoord.y;
		}
		catch (NoSuchElementException&)
		{
			try
			{
				// 만일 아직 부활 위치를 지정해 놓지 않았다면, 
				// 현재 존에서 디폴트로 설정되어 있는 부활존으로 간다.
				ResurrectCoord = g_pResurrectLocationManager->getSlayerPosition(pSlayer->getZone()->getZoneID());
				ZoneID         = ResurrectCoord.id; 
				ZoneX          = ResurrectCoord.x;
				ZoneY          = ResurrectCoord.y;
			}
			catch (NoSuchElementException&)
			{
				// 아, 비상이닷...
				throw Error("Critical Error : ResurrectInfo is not established!");
			}
		}
		o/

		// NoSuch제거. by sigi. 2002.5.9
		if (g_pResurrectLocationManager->getSlayerPosition(pSlayer->getResurrectZoneID(), ResurrectCoord))
		{
			ZoneID         = ResurrectCoord.id;
			ZoneX          = ResurrectCoord.x;
			ZoneY          = ResurrectCoord.y;
		}
		else
		{
			// 만일 아직 부활 위치를 지정해 놓지 않았다면, 
			// 현재 존에서 디폴트로 설정되어 있는 부활존으로 간다.
			if (g_pResurrectLocationManager->getSlayerPosition(pSlayer->getZone()->getZoneID(), ResurrectCoord))
			{
				ZoneID         = ResurrectCoord.id; 
				ZoneX          = ResurrectCoord.x;
				ZoneY          = ResurrectCoord.y;
			}
			else
			{
				// 아, 비상이닷...
				throw Error("Critical Error : ResurrectInfo is not established!");
			}
		}

		try 
		{
			pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneID);

#ifdef __PAY_SYSTEM_ZONE__
			GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pSlayer->getPlayer());
			Assert(pGamePlayer!=NULL);

			// 유료화 존이고 유료사용중이 아니면..
			if (pZoneInfo!=NULL
				&& (pZoneInfo->isPayPlay() || pZoneInfo->isPremiumZone())
				&& !pGamePlayer->isPayPlaying())
			{
			   	string connectIP = pGamePlayer->getSocket()->getHost();

				// 유료 서비스 사용이 가능한가?
				if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID()))
				{
					sendPayInfo(pGamePlayer);
				}
				else if (pZoneInfo->isPayPlay())
				{
					// 유료 서비스 사용 불가인 경우
					// 에슬남동에서 부활하는 곳으로 간다.
					if (g_pResurrectLocationManager->getSlayerPosition(13, ResurrectCoord))
					{
						ZoneID         = ResurrectCoord.id; 
						ZoneX          = ResurrectCoord.x;
						ZoneY          = ResurrectCoord.y;

						pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneID);
					}
					else
					{
						// 아, 비상이닷...
						throw Error("Critical Error : ResurrectInfo is not established!1");
					}
				}
			}
#endif
		} 
		catch (NoSuchElementException&) 
		{
			throw Error("Critical Error : 포탈에 지정된 존 아이디가 틀리거나, ZoneInfoManager에 해당 존이 존재하지 않습니다.");
		}
		
		try 
		{
			pZoneGroup = g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());
		} 
		catch (NoSuchElementException&) 
		{
			// 일단은 서버가 1대이므로.. 그대로 나간다...
			throw Error("Critical Error : 현재로는 게임 서버는 1대뿐이당..");
		}

		//pResurrectZone = pZoneGroup->getZone(ZoneID);
		//Assert(pResurrectZone != NULL);

		// EventResurrect에서 zone설정을 바꾼다.
		//pDeadCreature->setZone(pResurrectZone);	 // 주석처리 by sigi. 2002.5.11
		//pDeadCreature->setXY(ZoneX, ZoneY);
		//pDeadCreature->save();
	} 
	else if (pDeadCreature->isVampire())
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pDeadCreature);
		Assert(pVampire != NULL);

		pVampire->setStashStatus(false);
		pVampire->setHP(pVampire->getHP(ATTR_MAX), ATTR_CURRENT);


		/o
		try
		{
			ResurrectCoord = g_pResurrectLocationManager->getVampirePosition(pVampire->getResurrectZoneID());
			ZoneID         = ResurrectCoord.id;
			ZoneX          = ResurrectCoord.x;
			ZoneY          = ResurrectCoord.y;
		}
		catch (NoSuchElementException&)
		{
			try
			{
				// 만일 아직 부활 위치를 지정해 놓지 않았다면, 
				// 현재 존에서 디폴트로 설정되어 있는 부활존으로 간다.
				ResurrectCoord = g_pResurrectLocationManager->getVampirePosition(pVampire->getZone()->getZoneID());
				ZoneID         = ResurrectCoord.id; 
				ZoneX          = ResurrectCoord.x;
				ZoneY          = ResurrectCoord.y;
			}
			catch (NoSuchElementException&)
			{
				// 아, 비상이닷...
				throw Error("Critical Error : ResurrectInfo is not established!");
			}
		}
		o/

		// NoSuch제거. by sigi. 2002.5.9
		if (g_pResurrectLocationManager->getVampirePosition(pVampire->getResurrectZoneID(), ResurrectCoord))
		{
			ZoneID         = ResurrectCoord.id;
			ZoneX          = ResurrectCoord.x;
			ZoneY          = ResurrectCoord.y;
		}
		else
		{
			// 만일 아직 부활 위치를 지정해 놓지 않았다면, 
			// 현재 존에서 디폴트로 설정되어 있는 부활존으로 간다.
			if (g_pResurrectLocationManager->getVampirePosition(pVampire->getZone()->getZoneID(), ResurrectCoord))
			{
				ZoneID         = ResurrectCoord.id; 
				ZoneX          = ResurrectCoord.x;
				ZoneY          = ResurrectCoord.y;
			}
			else
			{
				// 아, 비상이닷...
				throw Error("Critical Error : ResurrectInfo is not established!");
			}
		}

		try 
		{
			pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneID);

#ifdef __PAY_SYSTEM_ZONE__
			GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pVampire->getPlayer());
			Assert(pGamePlayer!=NULL);

			// 유료화 존이고 유료사용자가 아니면..
			if (pZoneInfo!=NULL
				&& (pZoneInfo->isPayPlay() || pZoneInfo->isPremiumZone())
				&& !pGamePlayer->isPayPlaying())
			{
                string connectIP = pGamePlayer->getSocket()->getHost();

                // 유료 서비스 사용이 가능한가?
                if (pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID()))
				{
					sendPayInfo(pGamePlayer);
				}
				else if (pZoneInfo->isPayPlay())
				{
					// 유료 서비스 사용 불가인 경우
					// 림보남동에서 부활하는 곳으로 간다.
					if (g_pResurrectLocationManager->getVampirePosition(23, ResurrectCoord))
					{
						ZoneID         = ResurrectCoord.id; 
						ZoneX          = ResurrectCoord.x;
						ZoneY          = ResurrectCoord.y;

						pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneID);
					}
					else
					{
						// 아, 비상이닷...
						throw Error("Critical Error : ResurrectInfo is not established!2");
					}
				}
			}
#endif

		} 
		catch (NoSuchElementException&) 
		{
			throw Error("Critical Error : 포탈에 지정된 존 아이디가 틀리거나, ZoneInfoManager에 해당 존이 존재하지 않습니다.");
		}
		
		try 
		{
			pZoneGroup = g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());
		} 
		catch (NoSuchElementException&) 
		{
			// 일단은 서버가 1대이므로.. 그대로 나간다...
			throw Error("Critical Error : 현재로는 게임 서버는 1대뿐이당..");
		}

		//pResurrectZone = pZoneGroup->getZone(ZoneID);
		//Assert(pResurrectZone != NULL);

		// EventResurrect에서 zone설정을 바꾼다.
		//pDeadCreature->setZone(pResurrectZone);	 // 주석처리 by sigi. 2002.5.11
		//pDeadCreature->setXY(ZoneX, ZoneY);
		//pDeadCreature->save();
	}
	else Assert(false);
*/
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pDeadCreature);
	Assert( pPC != NULL );

	// 이벤트 존에서 죽었으면 이벤트 존 부활위치로..
	EventZoneInfo* pEventZoneInfo = EventZoneInfoManager::Instance().getEventZoneInfo( pPC->getZoneID() );
	if ( pEventZoneInfo != NULL )
	{
		ResurrectCoord.id = pPC->getZoneID();
		ResurrectCoord.x = pEventZoneInfo->getResurrectX();
		ResurrectCoord.y = pEventZoneInfo->getResurrectY();
	}
	// PK존에서 죽었을 경우
	// PK존의 부활 위치로 가야 한다.
	else if ( g_pPKZoneInfoManager->isPKZone( pPC->getZoneID() ) )
	{
		if ( !g_pPKZoneInfoManager->getResurrectPosition( pPC->getZoneID(), ResurrectCoord ) )
			g_pResurrectLocationManager->getPosition( pPC, ResurrectCoord );
	}
	// 일루젼스웨이1
	else if ( pPC->getZoneID() == 1410 )
	{
		ResurrectCoord.id = 1410;
		ResurrectCoord.x = 120;
		ResurrectCoord.y = 70;
	}
	else if ( pPC->getZoneID() == 1411 )
	{
		ResurrectCoord.id = 1411;
		ResurrectCoord.x = 126;
		ResurrectCoord.y = 60;
	}
	else if ( SiegeManager::Instance().isSiegeZone( pPC->getZoneID() ) )
	{
		ResurrectCoord.id = pPC->getZoneID();
		if ( pPC->isFlag( Effect::EFFECT_CLASS_SIEGE_DEFENDER ) || pPC->isFlag( Effect::EFFECT_CLASS_SIEGE_REINFORCE ) )
		{
			ResurrectCoord.x = 172;
			ResurrectCoord.y = 38;
		}
		else
		{
			ResurrectCoord.x = 20;
			ResurrectCoord.y = 232;
		}
	}
#ifdef __CONTRIBUTION_SYSTEM__
	else if( pPC->getZone()->hasLevelWar() && pPC->getZone()->isLevelWarZone() )
	{
		if ( pPC->isSlayer() )
		{
			ResurrectCoord.x = 12;
			ResurrectCoord.y = 9;
		}
		else if ( pPC->isVampire() )
		{
			ResurrectCoord.x = 117;
			ResurrectCoord.y = 8;
		}
		else if ( pPC->isOusters() )
		{
			ResurrectCoord.x = 9;
			ResurrectCoord.y = 111;
		}

		ResurrectCoord.id = pPC->getZoneID();
	}
#endif
	else
	{
		g_pResurrectLocationManager->getPosition( pPC, ResurrectCoord );
	}

	ZoneID	= ResurrectCoord.id; 
	ZoneX	= ResurrectCoord.x;
	ZoneY	= ResurrectCoord.y;

	pZoneInfo = g_pZoneInfoManager->getZoneInfo(ZoneID);
	pZoneGroup = g_pZoneGroupManager->getZoneGroup(pZoneInfo->getZoneGroupID());

	// Resurrect 이벤트를 플레이어 객체에 연관시킨다.
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pDeadCreature->getPlayer());
	EventResurrect* pEventResurrect = new EventResurrect(pGamePlayer);
	pEventResurrect->setDeadline(0);

	// 부활할 장소를 셋팅한다.	 by sigi. 2002.5.11
	pResurrectZone = pZoneGroup->getZone(ZoneID);
	Assert(pResurrectZone != NULL);

	if ( pZone->isHolyLand() != pResurrectZone->isHolyLand() )
	{
		// 아담의 성지에서 밖으로 나가면서 부활되는 경우
		// 성지 보너스를 다시 세팅하기 위해 initAllStat 을 불러줘야 한다.
		pDeadCreature->setFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
	}

	if ( g_pLevelWarZoneInfoManager->isCreatureBonusZone( pDeadCreature, pZone->getZoneID() )
		!= g_pLevelWarZoneInfoManager->isCreatureBonusZone( pDeadCreature, pResurrectZone->getZoneID() ) )
	{
		pDeadCreature->setFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
	}

	if ( pZone->isLevelWarZone() != pResurrectZone->isLevelWarZone() )
	{
		pDeadCreature->setFlag( Effect::EFFECT_CLASS_INIT_ALL_STAT );
	}

	pDeadCreature->setNewZone( pResurrectZone );
	pDeadCreature->setNewXY( ZoneX, ZoneY );

	// Player에 Event를 붙인다.
	pGamePlayer->addEvent(pEventResurrect);

	// DB 저장을 위해서.. EventResurrect에서 zone설정을 바꾼다.
	ZoneCoord_t  oldZoneX = pDeadCreature->getX();
	ZoneCoord_t  oldZoneY = pDeadCreature->getY();

	// 새 존의 정보를 저장해둔다.
	pDeadCreature->setZone(pResurrectZone);	
	pDeadCreature->setXY(ZoneX, ZoneY);

	pDeadCreature->save();
	// 다시 원래의 Zone 정보로 설정해준다.
	pDeadCreature->setZone(pZone);
	pDeadCreature->setXY(oldZoneX, oldZoneY);

	pGamePlayer->setPlayerStatus(GPS_IGNORE_ALL);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// transport All Creatures
//////////////////////////////////////////////////////////////////////////////
void PCManager::transportAllCreatures (ZoneID_t ZoneID, ZoneCoord_t ZoneX, ZoneCoord_t ZoneY, Race_t race, Turn_t delay) const
       throw (Error)
{
	__BEGIN_TRY

	vector< ObjectID_t > creatureIDs;

	Zone* pZone = NULL;

	// transportCreature - Zone::deleteCreature에서 다시 PCManager로 접근해서
	// SELF_DEAD_LOCK이 발생한다. -_--.. 그래서..
	// 일단 존에 있는 모든 Creature의 ID를 기억해두고...
	__ENTER_CRITICAL_SECTION(m_Mutex)

	map< ObjectID_t , Creature* >::const_iterator iCreature  = m_Creatures.begin();
	while (iCreature != m_Creatures.end()) 
	{
		Creature* pCreature = iCreature->second;
		Assert(pCreature->isPC());

		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

		// race==0xFF는 default로 모든 종족이라는 의미이고 -_-;
		// 특정 종족만 이동시킨다.
		if (race==defaultRaceValue 
			|| race==pPC->getRace())
		{
			creatureIDs.push_back( iCreature->first );

			pZone = pCreature->getZone();
		}

		iCreature++;
	}

	__LEAVE_CRITICAL_SECTION(m_Mutex)

	// 그 Creature들을 다른 곳으로 이동시킨다.
	try
	{
		vector< ObjectID_t >::const_iterator itr = creatureIDs.begin();
		
		while (itr != creatureIDs.end()) 
		{
			Creature* pCreature = pZone->getCreature( *itr );

			if (pCreature!=NULL)
			{
			//	transportCreature(pCreature, ZoneID, ZoneX, ZoneY, false);
				GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());
				EventTransport* pEventTransport = dynamic_cast<EventTransport*>(pGamePlayer->getEvent(Event::EVENT_CLASS_TRANSPORT));
				bool newEvent = false;
				if (pEventTransport==NULL)
				{
					pEventTransport = new EventTransport(pGamePlayer);
					newEvent = true;
				}

				if ( ZoneID == 0xffff )
				{
					ZONE_COORD   ResurrectCoord;
					PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
					Assert( pPC != NULL );
					g_pResurrectLocationManager->getPosition( pPC, ResurrectCoord );

					// 10초
					pEventTransport->setDeadline( 100 );
					pEventTransport->setZoneName( "" );
					pEventTransport->setTargetZone( ResurrectCoord.id, ResurrectCoord.x, ResurrectCoord.y );
				}
				else
				{
					ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo( ZoneID );
					Assert(pZoneInfo!=NULL);

					pEventTransport->setDeadline( delay*10 );
					pEventTransport->setZoneName( pZoneInfo->getFullName() );

					if ( ZoneX == 0xffff )
					{
						pEventTransport->setTargetZone( ZoneID, pCreature->getX(), pCreature->getY() );
					}
					else
					{
						pEventTransport->setTargetZone( ZoneID, ZoneX, ZoneY );
						// 몇 초후에 어디로 이동한다.고 보내준다.
//						pEventTransport->sendMessage();
					}
				}

				if ( newEvent ) pGamePlayer->addEvent(pEventTransport);
			}

			itr ++;
		}
	}
	catch (Throwable & t)
	{
		filelog("PCManagerBug.log", "transportAllCreaturesBUG : %s", t.toString().c_str());
	}
	

	__END_CATCH
}

vector<uint> PCManager::getPCNumByRace() const
{
	vector<uint> ret;
	ret.push_back(0);
	ret.push_back(0);
	ret.push_back(0);

	map<ObjectID_t, Creature*>::const_iterator itr = getCreatures().begin();

	for ( ; itr != getCreatures().end() ; ++itr )
	{
		PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(itr->second);
		if ( pPC != NULL )
		{
			ret[pPC->getRace()]++;
		}
	}

	return ret;
}

uint PCManager::getTotalPCNum() const
{
	return getCreatures().size();
}

//////////////////////////////////////////////////////////////////////////////
// get debug string
//////////////////////////////////////////////////////////////////////////////
string PCManager::toString () const
       throw ()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "PCManager("
		<< CreatureManager::toString()
		<< ")" ;

	return msg.toString();

	__END_CATCH
}
