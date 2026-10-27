//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddMonsterHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "GCAddMonster.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "PacketFunction.h"
#include "MTopView.h"

extern void Add_GDR_Effect(int nEffect, bool bAppearBossMonster);
extern void Add_Race_SlayerMonster(GCAddMonster * pPacket);
extern void Add_Race_OustersMonster(GCAddMonster * pPacket);
extern "C" int Phoenix_CreatureDrawAudit_ShouldLog(int creatureType);
extern "C" void Phoenix_CreatureDrawAudit_Log(const char* format, ...);
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddMonsterHandler::execute ( GCAddMonster * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
		

	/*
	if(pPacket->getMonsterType() == 814)
	{
		char szBuf[128];
		sprintf(szBuf, "[GCAddMonster] TYPE=%d, NAME=%s, X=%d, Y=%d\n",
			pPacket->getMonsterType(), pPacket->getMonsterName().c_str(), pPacket->getX(), pPacket->getY());
		OutputDebugString(szBuf);
	}
	*/

	//	std::cout << pPacket->toString() << std::endl;

	//AfxMessageBox( pPacket->toString().c_str() );
	CT_BD("[ADDMON] enter id=%d type=%d xy=%d,%d dir=%d hp=%d/%d from=%d zone=%p",
		(int)pPacket->getObjectID(),
		(int)pPacket->getMonsterType(),
		(int)pPacket->getX(),
		(int)pPacket->getY(),
		(int)pPacket->getDir(),
		(int)pPacket->getCurrentHP(),
		(int)pPacket->getMaxHP(),
		(int)pPacket->getFromFlag(),
		g_pZone);
	const bool auditCreature2221 = (Phoenix_CreatureDrawAudit_ShouldLog(pPacket->getMonsterType()) != 0);
	if (auditCreature2221)
	{
		Phoenix_CreatureDrawAudit_Log(
			"GCAddMonster entry objectID=%d monsterType=%d name=%s x=%d y=%d dir=%d hp=%d/%d zonePtr=%p playerPtr=%p",
			(int)pPacket->getObjectID(),
			(int)pPacket->getMonsterType(),
			pPacket->getMonsterName().c_str(),
			(int)pPacket->getX(),
			(int)pPacket->getY(),
			(int)pPacket->getDir(),
			(int)pPacket->getCurrentHP(),
			(int)pPacket->getMaxHP(),
			g_pZone,
			pPlayer);
	}
	//------------------------------------------------------
	// Zone이 아직 생성되지 않은 경우
	//------------------------------------------------------

	EFFECTSTATUS	statusShadowCheck = EFFECTSTATUS_NULL;

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
		#ifdef __DEBUG_OUTPUT__
			DEBUG_ADD_FORMAT("[AddMonster] type=%d, id=%d, xy=(%d,%d), dir=%d", 
								pPacket->getMonsterType(),
								pPacket->getObjectID(), 
								pPacket->getX(), pPacket->getY(), 
								pPacket->getDir());
		#endif

		//AfxMessageBox( pPacket->toString().c_str() );
		WORD wMonsterType = pPacket->getMonsterType();	
		// 각종족모습의 몬스터(뱀파이어 모습 몬스터는 따로 처리 안해도 되네..ㅎㅎ)

		if(wMonsterType == 735 || wMonsterType == 793 || wMonsterType == 796 || wMonsterType == 797)
		{
			Add_Race_SlayerMonster(pPacket);
			
			return;
		}
		else if(wMonsterType == 737 || wMonsterType == 795 || wMonsterType == 800)
		{
			Add_Race_OustersMonster(pPacket);
			return;
		}
		// 2004, 9, 1, sobeit add start - 공성전시 트랩일 경우 공격측에서는 보이지 않는다.
		else if( wMonsterType == 738 ||
			wMonsterType == 739 ||
			wMonsterType == 740 ||
			wMonsterType == 741 )
		{
			EffectInfo* pTempEffectInfo = pPacket->getEffectInfo();
			
			if(NULL != pTempEffectInfo)
			{
				if(pTempEffectInfo->getListNum()>0)
				{

					std::list<std::pair<WORD,DWORD> > TemList = pTempEffectInfo->GetList();
					std::list<std::pair<WORD,DWORD> >::iterator itr = TemList.begin();

					for(;itr != TemList.end(); ++itr)
					{
						EFFECTSTATUS	status		= (EFFECTSTATUS)itr->first;
						
						if(status == EFFECTSTATUS_HIDE_TO_ATTACKER)
						{
							if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) || 
								g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) || 
								g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) || 
								g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) || 
								g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) )
								return;
						}
					}
				}
			}
		}
#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명 그림자 설정
		else
		{
			EffectInfo* pTempEffectInfo = pPacket->getEffectInfo();
			
			if(NULL != pTempEffectInfo)
			{
				if(pTempEffectInfo->getListNum()>0)
				{

					std::list<std::pair<WORD,DWORD> > TemList = pTempEffectInfo->GetList();
					std::list<std::pair<WORD,DWORD> >::iterator itr = TemList.begin();

					for(;itr != TemList.end(); ++itr)
					{
						EFFECTSTATUS	status		= (EFFECTSTATUS)itr->first;
						if(status	== EFFECT_CLASS_TRANSLUCENCY)
							statusShadowCheck	= EFFECT_CLASS_TRANSLUCENCY;
					}	
				}
			}
		}
#endif // __TIPOJYU_CASTLE
	
		// 2004, 9, 1, sobeit add end - 공성전시 트랩일 경우 공격측에서는 보이지 않는다.
		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());

		//--------------------------------------------------
		// 새로운 Creature이면 추가
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreature;
			CT_Done("DIAG: GCAddMonster post new MCreature, monsterType=%d objectID=%d", (int)pPacket->getMonsterType(), (int)pPacket->getObjectID());

			pCreature->SetZone( g_pZone );
			CT_Done("DIAG: GCAddMonster post SetZone");

			//pCreature->SetCreatureType( 0 );
			pCreature->SetCreatureType( pPacket->getMonsterType() );
			pCreature->SetGroundCreature();
			pCreature->SetID(pPacket->getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			CT_Done("DIAG: GCAddMonster post position/dir setup");
			if( pPacket->getMonsterType() == 808 ||
				pPacket->getMonsterType() == 809
#if __CONTENTS(__ONIBLA)
				|| pPacket->getMonsterType() == 1071
#endif //__ONIBLA
				)
				pCreature->SetAction( ACTION_VAMPIRE_DRAIN );
			else	pCreature->SetAction( ACTION_STAND );

			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			// 2026-05-19 DEATH STATE FIX: Re-entry sirasinda dead monster
			// (HP=0) GCAddMonster paketi olarak gelirse default ACTION_STAND
			// set ediliyordu -> creature ayakta ama olu gosteriliyordu.
			// SetCorpse() ile m_bAlive=false + ACTION_DIE + ground state set.
			// (Phoenix server brief death-animation window'da Monster object
			// kalabiliyor; ya da bazi senaryolarda corpse'a donmeden gonderiyor.)
			if (pPacket->getCurrentHP() == 0)
			{
				pCreature->SetCorpse();
			}

			// [ TEST CODE ]
			// 이름 설정
			//char str[20];
			//sprintf(str, "ID=%d", pCreature->GetID());



			//by viva
			// FAZ 4 (2026-05-03): GM summon creature on player's tile crash —
			// unguarded g_pCreatureTable[monsterType] OOB. Same family as
			// GHOST mode entry crash. Fall back to packet's name if table
			// lookup is out-of-range.
			{
				WORD mt = pPacket->getMonsterType();
				if (g_pCreatureTable != NULL
					&& mt < g_pCreatureTable->GetSize())
				{
					pCreature->SetName((*g_pCreatureTable)[mt].Name.GetString());
				}
				else
				{
					pCreature->SetName(pPacket->getMonsterName().c_str());
				}
			}
			//end


			// 임시로..
			pCreature->SetGuildNumber( 1 );

			// 이름
			//pCreature->SetName( pPacket->getName().toString().c_str() );

#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명 그림자 설정
			if(statusShadowCheck == EFFECT_CLASS_TRANSLUCENCY)
			{
				SMonsterHalfAlpha	MonsterHalfAlpha;
				MonsterHalfAlpha.Release();
				MonsterHalfAlpha.bHalfAlpha			= true;
				MonsterHalfAlpha.iCreatureObjectID	= pPacket->getObjectID();
				MonsterHalfAlpha.iCreatureID		= pCreature->GetCreatureType();
				pCreature->SetAlpha(TRUE);

				if(g_pTopView)
				{
					g_pTopView->SetMonsterHalfAlpha(MonsterHalfAlpha);
				}
			}
#endif

			// 색깔 정보			

			CT_BD("[ADDMON] before AddCreature id=%d type=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType());
			if (!g_pZone->AddCreature( pCreature ))
			{
				CT_BD("[ADDMON] AddCreature FAILED id=%d type=%d",
					(int)pPacket->getObjectID(),
					(int)pPacket->getMonsterType());
				delete pCreature;
				pCreature = NULL;
			}
			else
			{
				CT_BD("[ADDMON] AddCreature OK id=%d type=%d",
					(int)pPacket->getObjectID(),
					(int)pPacket->getMonsterType());
				//------------------------------------------------------------
				// Load되지 않았으면 load한다.
				//------------------------------------------------------------
//				LoadCreatureType( pPacket->getMonsterType() );			
			}
		}
		else
		{
			if (pCreature->GetCreatureType()!=pPacket->getMonsterType())
			{
				pCreature->SetCreatureType(pPacket->getMonsterType());
			}

			pCreature->SetGroundCreature();
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			pCreature->SetAction( ACTION_STAND );

			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			// 2026-05-19 DEATH STATE FIX (re-entry — existing creature update branch).
			// Ayni mantik: HP=0 ise SetCorpse() ile death sprite + ground state.
			if (pPacket->getCurrentHP() == 0)
			{
				pCreature->SetCorpse();
			}

			// 임시로..
			pCreature->SetGuildNumber( 1 );
		}

		if (pCreature!=NULL)
		{
			// Zone preload listesinde olmayan dinamik summon/spawn yaratıklarının
			// sprite pack'i burada tamamlanır.
			CT_BD("[ADDMON] before LoadCreatureType id=%d type=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType());
			BOOL loadCreatureResult = LoadCreatureType(pCreature->GetCreatureType());
			CT_BD("[ADDMON] after LoadCreatureType id=%d type=%d result=%d action=%d dir=%d frame=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType(),
				(int)loadCreatureResult,
				(int)pCreature->GetAction(),
				(int)pCreature->GetDirection(),
				(int)pCreature->GetFrame());
			if (auditCreature2221)
			{
				Phoenix_CreatureDrawAudit_Log(
					"GCAddMonster dynamic load result objectID=%d packetType=%d creatureType=%d result=%d action=%d dir=%d frame=%d pos=%d,%d",
					(int)pPacket->getObjectID(),
					(int)pPacket->getMonsterType(),
					(int)pCreature->GetCreatureType(),
					(int)loadCreatureResult,
					(int)pCreature->GetAction(),
					(int)pCreature->GetDirection(),
					(int)pCreature->GetFrame(),
					(int)pCreature->GetX(),
					(int)pCreature->GetY());
			}

			//--------------------------------------------------
			// Effect 붙이기..
			//--------------------------------------------------
			bool bShowComeFromPotal = true;
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );
			// 2004, 6, 25, sobeit add start - 질드레 관련 맵 효과 

			TYPE_CREATURETYPE creatureType = pCreature->GetCreatureType();

			switch(creatureType)
			{
				case 724: // 뱀파이어 소환체 1
					Add_GDR_Effect(1, true);
					break;
				case 725: // // 뱀파이어 소환체 2
					Add_GDR_Effect(2, true);
					break;
				case 739: // 트랩 - 랜드 마인
					ExecuteActionInfoFromMainNode(SKILL_CLIENT_LAND_MINE_LIGHT,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	pCreature->GetID(),	
						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);			
					break;
				case 738: // 트랩 - 플레져 스테이션
					ExecuteActionInfoFromMainNode(SKILL_CLIENT_PLEASUER_STATION_LIGHT,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	pCreature->GetID(),	
						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);			
					break;
				case 753:
					bShowComeFromPotal = false;
					pCreature->SetAction(ACTION_VAMPIRE_DRAIN);  
					break;

				case 1358:

	              	bShowComeFromPotal = false;

					ExecuteActionInfoFromMainNode(SKILL_SKILL_GORE_GLAND_FIRE,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	pCreature->GetID(),	

						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);

					pCreature->SetAction(ACTION_VAMPIRE_DRAIN);  

					break;
				case 765: // 그레이트 러피언 몸에 붙는 이펙트
//				case 891: // 헬가든 그레이트 러피언
					ExecuteActionInfoFromMainNode(SKILL_CLIENT_GREAT_RUFFIAN_BODY,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	pCreature->GetID(),	
						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);			
					break;
				case 768: // miga
					bShowComeFromPotal = false;
					pCreature->AddEffectStatus((EFFECTSTATUS)(EFFECTSTATUS_SUMMON_MIGA_FIRE), 0xFFFF);
					break;
				case 786: // 장교
					pCreature->AddEffectStatus((EFFECTSTATUS)(EFFECTSTATUS_GRAND_MASTER_VAMPIRE), 0xFFFF);
					break;
				case 788: // 
					bShowComeFromPotal = false;
					pCreature->AddEffectStatus((EFFECTSTATUS)(EFFECTSTATUS_DRAGON_TORNADO), 0xFFFF);
					break;
				case 789:
					bShowComeFromPotal = false;
					pCreature->AddEffectStatus((EFFECTSTATUS)(EFFECTSTATUS_DRAGON_TORNADO_CHILD), 0xFFFF);
					break;
				case 793:
				case 794:
				case 795: // 제물들
					bShowComeFromPotal = false;
					pCreature->AddEffectStatus(EFFECTSTATUS_CAUSE_CRITICAL_WOUNDS, 0xffff);
					ExecuteActionInfoFromMainNode(RESULT_MAGIC_CAUSE_CRITICAL_WOUNDS,pCreature->GetX(), pCreature->GetY(), 0,2,	pCreature->GetID(),	
						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);		
					break;
				case 808:
				case 809:
					bShowComeFromPotal = false;
					break;
				case 814: // 가디언 리더
					ExecuteActionInfoFromMainNode(SKILL_CLIENT_GREAT_RUFFIAN_BODY,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(), pCreature->GetID(),	
						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);
					break;
				case 2004:
					pCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2688, 0xFFFF, 0,FALSE, MEffect::EFFECT_ATTACH, false);
					break;
				case 2005:
					pCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2691, 0xFFFF, 0,FALSE, MEffect::EFFECT_ATTACH, false);
					break;
				case 2006:
					pCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2689, 0xFFFF, 0,FALSE, MEffect::EFFECT_ATTACH, false);
					break;
				case 2007:
					pCreature->CreateAttachEffect(EFFECTSPRITETYPE_unknow_2692, 0xFFFF, 0,FALSE, MEffect::EFFECT_ATTACH, false);
					break;
			}

			// 몸에 이팩트가 붙는 Creature라면 이팩트를 붙여준다.
			if(creatureType < g_pCreatureTable->GetSize())
			{
				CREATURETABLE_INFO &creatureInfo = g_pCreatureTable->Get(creatureType);

				if(creatureInfo.EffectStatus != EFFECTSTATUS_NULL)
				{
					pCreature->AddEffectStatus((EFFECTSTATUS)creatureInfo.EffectStatus, 0xFFFF);
				}
			}


			// 2004, 6, 25, sobeit add end - 질드레 관련 
			//--------------------------------------------------
			// 정상일 경우에는 0, 포탈을 통했을 경우에는 1
			//--------------------------------------------------
			if (pPacket->getFromFlag()==1 && bShowComeFromPotal)
			{
//				if(pCreature->GetCreatureType() == 753)
//				{
//					pCreature->SetAction(ACTION_VAMPIRE_DRAIN);  
//				}
//				else
//				{
					CT_BD("[ADDMON] before VampireComeFromPortal id=%d type=%d",
						(int)pPacket->getObjectID(),
						(int)pCreature->GetCreatureType());
					g_pTopView->VampireComeFromPortal( pCreature );
					CT_BD("[ADDMON] after VampireComeFromPortal id=%d type=%d",
						(int)pPacket->getObjectID(),
						(int)pCreature->GetCreatureType());
					
					// 바닥 이펙트 붙이기
					int x			= pCreature->GetX();
					int y			= pCreature->GetY();
					DWORD delayFrame	= ConvertDurationToFrame( 18 );
					
					ExecuteActionInfoFromMainNode(
						RESULT_MAGIC_BLOODY_TUNNEL,		// 값 자체가 RESULT_ACTIONINFO이다.
						
						x, y, 0,
						DIRECTION_DOWN,
						
						OBJECTID_NULL,
						x, y, 0,
						
						delayFrame,	
						
						NULL
						);
//				}
			}
			
			
		}
	}
	
	// [도움말] 몹이 나타날때
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_CREATURE_APPEAR_MONSTER );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
