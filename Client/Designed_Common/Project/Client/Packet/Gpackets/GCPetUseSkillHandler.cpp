//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPetUseSkillHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCPetUseSkill.h"
#include "MZone.h"
#include "MFakeCreature.h"
#include "DebugInfo.h"
#include "SkillDef.h"
#include "PacketFunction.h"
#include "ClientDef.h"
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCPetUseSkillHandler::execute ( GCPetUseSkill * pGCPetUseSkill , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
	TYPE_OBJECTID attackerID = pGCPetUseSkill->getAttacker();
	TYPE_OBJECTID targetID = pGCPetUseSkill->getTarget();

	MCreature *pAttackerCreature = g_pZone->GetCreature(attackerID);
	MCreature *pTargetCreature = g_pZone->GetCreature(targetID);

	if( pAttackerCreature == NULL )
	{
		DEBUG_ADD_FORMAT("[GCPetUseSkillHandler] 어태커(%d) 없음-_-;", attackerID);
	}
	else
	{
		if( pTargetCreature == NULL )
		{
			DEBUG_ADD_FORMAT("[GCPetUseSkillHandler] 타겟(%d) 없음-_-;", targetID);
		}
		else
		{
//			pTargetCreature->SetAction( ACTION_DAMAGED );		//종족펫 버그 코드 타 캐릭터가 패스트 무브 중일때 발생 할 수 있음.
		
			MFakeCreature *pPet = dynamic_cast<MFakeCreature *>(g_pZone->GetFakeCreature(pAttackerCreature->GetPetID()));

			if( pPet == NULL )
			{
				DEBUG_ADD_FORMAT("[GCPetUseSkillHandler] 펫(%d) 없음-_-;", pAttackerCreature->GetPetID());
			}
			else
			{
				int skillID = SKILL_STEP_NULL;
				if(pPet->GetPetItem() != NULL)
				{
					int petItemType = pPet->GetPetItem()->GetItemType();
					int petLevel = pPet->GetPetItem()->GetNumber();

					TYPE_SOUNDID soundID = (*g_pCreatureTable)[pPet->GetCreatureType()].GetActionSound( ACTION_ATTACK );

					if (soundID!=SOUNDID_NULL  )
					{
						PlaySound( soundID,
									false,
									pPet->GetX(), pPet->GetY());
					}

					switch(petItemType)
					{
					// 탱크-_-;
					case 3:
					case 27:	   //XK2譴옹
						{
							ExecuteActionInfoFromMainNode(
								SKILL_CLIENT_TANK_ATTACKED,										// 사용 기술 번호
								
								0, 0, 0,
								DIRECTION_DOWN, // 사용 방향
								
								pTargetCreature->GetID(),												// 목표에 대한 정보
								0, 0, 0, 
								
								0,													// 기술의 (남은) 지속 시간		
								
								NULL,
								
								false);	
						}

						if(petLevel >= 30 && petLevel < 40)
							skillID = SKILL_CLIENT_TANK_ATTACK_3;
						else if(petLevel >= 40 && petLevel < 50)
							skillID = SKILL_CLIENT_TANK_ATTACK_4;
						else if(petLevel >= 50)
							skillID = SKILL_CLIENT_TANK_ATTACK_5;
						break;
					case 4:	// stirge
					case 28://펜쳄윗
						if(petLevel<20)
							skillID = SKILL_CLIENT_STIRGE_BLOOD_DRAIN_1;
						else if(petLevel >= 20 && petLevel < 30)
							skillID = SKILL_CLIENT_STIRGE_BLOOD_DRAIN_2;
						else if(petLevel >= 30 && petLevel < 40)
							skillID = SKILL_CLIENT_STIRGE_BLOOD_DRAIN_3;
						else if(petLevel >= 40 && petLevel < 50)
							skillID = SKILL_CLIENT_STIRGE_BLOOD_DRAIN_4;
						else if(petLevel >= 50)
							skillID = SKILL_CLIENT_STIRGE_BLOOD_DRAIN_5;
						
						{
							ExecuteActionInfoFromMainNode(
								skillID,										// 사용 기술 번호
								
								0, 0, 0,
								DIRECTION_DOWN, // 사용 방향
								
								pTargetCreature->GetID(),												// 목표에 대한 정보
								0, 0, 0, 
								
								0,													// 기술의 (남은) 지속 시간		
								
								NULL,
								
								true);	
						}
						break;
					case 25:	// add by rallser 녘膠세콘
					case 26:	 //聃聞의
					case 30:
					case 31:
					case 32:
					case 33:
						{
							{
							PlaySound( SOUND_CABERUS_SKILL, false, pPet->GetX(), pPet->GetY());

							ExecuteActionInfoFromMainNode(
								SKILL_CLIENT_INCUBUS_ATTACK, 									// 사용 기술 번호
								0, 0, 0,
								DIRECTION_DOWN, // 사용 방향
								pTargetCreature->GetID(),										// 목표에 대한 정보
								0, 0, 0, 
								0,																// 기술의 (남은) 지속 시간		
								NULL,								
								false);

								skillID = REFLECT_AURA_PRISM;

							BYTE IncubusDirect = pPet->GetDirectionToPosition(pTargetCreature->GetX(), pTargetCreature->GetY());

							pPet->SetDirection(IncubusDirect);
							pPet->SetAction(ACTION_DRAINED);
							}
						}
						break;
					case 5:	// pixie
					case 29:	  //빻鉤綾
						if(petLevel<20)
							skillID = SKILL_CLIENT_PIXIE_ABSORB_SOUL_1;
						else if(petLevel >= 20 && petLevel < 30)
							skillID = SKILL_CLIENT_PIXIE_ABSORB_SOUL_2;
						else if(petLevel >= 30 && petLevel < 40)
							skillID = SKILL_CLIENT_PIXIE_ABSORB_SOUL_3;
						else if(petLevel >= 40 && petLevel < 50)
							skillID = SKILL_CLIENT_PIXIE_ABSORB_SOUL_4;
						else if(petLevel >= 50)
							skillID = SKILL_CLIENT_PIXIE_ABSORB_SOUL_5;

						ExecuteActionInfoFromMainNode(
							skillID,										// 사용 기술 번호
							
							0, 0, 0,
							DIRECTION_DOWN, // 사용 방향
							
							pTargetCreature->GetID(),												// 목표에 대한 정보
							0, 0, 0, 
							
							0,													// 기술의 (남은) 지속 시간		
							
							NULL,
							
							false);	
						break;
#if __CONTENTS(__NEW_PET_INCUBUS)
					case 11: 	// 인큐버스 공격
						{
//							pPet->SetAction( ACTION_ATTACK );
//							pPet->SyncTurretDirection();
							{
							PlaySound( SOUND_CABERUS_SKILL, false, pPet->GetX(), pPet->GetY());

							ExecuteActionInfoFromMainNode(
								SKILL_CLIENT_INCUBUS_ATTACK, 									// 사용 기술 번호
								0, 0, 0,
								DIRECTION_DOWN, // 사용 방향
								pTargetCreature->GetID(),										// 목표에 대한 정보
								0, 0, 0, 
								0,																// 기술의 (남은) 지속 시간		
								NULL,								
								false);


						//	if(petLevel >= 30 && petLevel < 40)
						//		skillID = SKILL_CLIENT_TANK_ATTACK_3;
						//	else if(petLevel >= 40 && petLevel < 50)
						//		skillID = SKILL_CLIENT_TANK_ATTACK_4;
						//	else if(petLevel >= 50)
								skillID = REFLECT_AURA_PRISM;

							BYTE IncubusDirect = pPet->GetDirectionToPosition(pTargetCreature->GetX(), pTargetCreature->GetY());

							pPet->SetDirection(IncubusDirect);
							pPet->SetAction(ACTION_DRAINED);
							}
						}
						break;
#endif //__NEW_PET_INCUBUS
#if __CONTENTS(__UPGRADE_MONSTER_PET)
				/*	case 28: case 29: case 34: case 35:
						{
							PlaySound( SOUND_CABERUS_SKILL, false, pPet->GetX(), pPet->GetY());

							ExecuteActionInfoFromMainNode(
														SKILL_CLIENT_INCUBUS_ATTACK, 	// 사용 기술 번호
														0, 0, 0,
														DIRECTION_DOWN,					// 사용 방향
														pTargetCreature->GetID(),		// 목표에 대한 정보
														0, 0, 0, 
														0,								// 기술의 (남은) 지속 시간
														NULL,								
														false);

							skillID = REFLECT_AURA_PRISM;

							BYTE IncubusDirect = pPet->GetDirectionToPosition(pTargetCreature->GetX(), pTargetCreature->GetY());

							pPet->SetDirection(IncubusDirect);
							pPet->SetAction(ACTION_DRAINED);
						}
						break;*/
#endif //__UPGRADE_MONSTER_PET
					}
				}

				pPet->SetTurretDelay( 3 );
//				pPet->SetDirectionToPosition(pTargetCreature->GetX(), pTargetCreature->GetY());
				int sx = 0, sy = 0;
				BYTE petDirect = pPet->GetDirectionToPosition(pTargetCreature->GetX(), pTargetCreature->GetY());

				// 2004, 12, 21, sobeit add start - 센타우로 터렛위치 세팅
				if( pPet->GetCreatureType() == 702 || pPet->GetCreatureType() == 703 || pPet->GetCreatureType() == 704 || pPet->GetCreatureType() == 1562 )
					pPet->SetTurretFinalDirection(petDirect);
				// 2004, 12, 21, sobeit add end

				if( pPet->GetCreatureType() == 702  || pPet->GetCreatureType() == 1562 )
				{
					POINT pointGap[8] = 
					{
						{ 11, 6 },	// 좌
						{ 8, 2 },	// 좌하
						{ 0, 0 },	// 하
						{ -7, 2 },	// 우하
						{ -10, 6 },	// 우
						{ -7, 10 },	// 우상
						{ 0, 12 },	// 상
						{ 8, 10 },	// 좌상
					};
					sx = pointGap[pPet->GetDirection()].x-pointGap[petDirect].x;
					sy = pointGap[pPet->GetDirection()].y-pointGap[petDirect].y;
				}

				ExecuteActionInfoFromMainNode(
					skillID,										// 사용 기술 번호
					
					pPet->GetX(), pPet->GetY(), 0,
					petDirect, // 사용 방향
					
					pPet->GetID(),												// 목표에 대한 정보
					pPet->GetX(), pPet->GetY(), 0, 
					
					0,													// 기술의 (남은) 지속 시간		
					
					NULL,
					
					false, sx, sy);	
				
			}
		}
	}

	//std::cout << pGCPetUseSkill->toString() << std::endl;
	
#elif __WINDOWS__

#endif

	__END_CATCH
}
