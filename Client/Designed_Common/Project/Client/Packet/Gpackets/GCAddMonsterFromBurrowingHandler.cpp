//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddMonsterFromBurrowingHandler.cpp
// Written By  : elca@ewestsoft.com
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "GCAddMonsterFromBurrowing.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "MTopView.h"

extern "C" int Phoenix_CreatureDrawAudit_ShouldLog(int creatureType);
extern "C" void Phoenix_CreatureDrawAudit_Log(const char* format, ...);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddMonsterFromBurrowingHandler::execute ( GCAddMonsterFromBurrowing * pPacket , Player * pPlayer )
	 
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	EFFECTSTATUS	statusShadowCheck = EFFECTSTATUS_NULL;
	CT_BD("[ADDMON-BURROW] enter id=%d type=%d xy=%d,%d dir=%d hp=%d/%d zone=%p",
		(int)pPacket->getObjectID(),
		(int)pPacket->getMonsterType(),
		(int)pPacket->getX(),
		(int)pPacket->getY(),
		(int)pPacket->getDir(),
		(int)pPacket->getCurrentHP(),
		(int)pPacket->getMaxHP(),
		g_pZone);
	const bool auditCreature2221 = (Phoenix_CreatureDrawAudit_ShouldLog(pPacket->getMonsterType()) != 0);
	if (auditCreature2221)
	{
		Phoenix_CreatureDrawAudit_Log(
			"GCAddMonsterFromBurrowing entry objectID=%d monsterType=%d name=%s x=%d y=%d dir=%d hp=%d/%d zonePtr=%p playerPtr=%p",
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
		//AfxMessageBox( pPacket->toString().c_str() );
		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());

		// 2004, 9, 1, sobeit add start - 공성전시 트랩일 경우 공격측에서는 보이지 않는다.
		WORD wMonsterType = pPacket->getMonsterType();
		if( wMonsterType == 738 ||
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
		//--------------------------------------------------
		// 새로운 Creature이면 추가
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreature;

			pCreature->SetZone( g_pZone );

			//pCreature->SetCreatureType( 0 );
			pCreature->SetCreatureType( pPacket->getMonsterType() );
			pCreature->SetGroundCreature();
			pCreature->SetID(pPacket->getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			pCreature->SetAction( ACTION_STAND );

			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			// 2026-05-19 DEATH STATE FIX (bkz GCAddMonsterHandler same pattern)
			if (pPacket->getCurrentHP() == 0)
			{
				pCreature->SetCorpse();
			}

			// [ TEST CODE ]
			// 이름 설정
			//char str[20];
			//sprintf(str, "ID=%d", pCreature->GetID());
			// FAZ 4 (2026-05-03): bounds guard — see GCAddMonsterHandler note
			{
				WORD mt = pPacket->getMonsterType();
				if (g_pCreatureTable != NULL && mt < g_pCreatureTable->GetSize())
					pCreature->SetName( (*g_pCreatureTable)[mt].Name.GetString() );
				else
					pCreature->SetName( pPacket->getMonsterName().c_str() );
			}

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
#endif //__TIPOJYU_CASTLE
			

			// 색깔 정보
			

			if (!g_pZone->AddCreature( pCreature ))
			{
				delete pCreature;
				pCreature = NULL;
			}
			else
			{				
				//------------------------------------------------------------
				// Load되지 않았으면 load한다.
				//------------------------------------------------------------
//				LoadCreatureType( pPacket->getMonsterType() );			
			}
		}
		//--------------------------------------------------
		// 이미 있는 Creature인 경우
		//--------------------------------------------------
		else
		{
			//pCreature->SetCreatureType( 0 );
			pCreature->SetCreatureType( pPacket->getMonsterType() );
			pCreature->SetGroundCreature();
			pCreature->SetID(pPacket->getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			pCreature->SetAction( ACTION_STAND );

			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			// 2026-05-19 DEATH STATE FIX (bkz GCAddMonsterHandler same pattern)
			if (pPacket->getCurrentHP() == 0)
			{
				pCreature->SetCorpse();
			}

			// [ TEST CODE ]
			// 이름 설정
			//char str[20];
			//sprintf(str, "ID=%d", pCreature->GetID());
			// FAZ 4 (2026-05-03): bounds guard — see GCAddMonsterHandler note
			{
				WORD mt = pPacket->getMonsterType();
				if (g_pCreatureTable != NULL && mt < g_pCreatureTable->GetSize())
					pCreature->SetName( (*g_pCreatureTable)[mt].Name.GetString() );
				else
					pCreature->SetName( pPacket->getMonsterName().c_str() );
			}

			// 임시로..
			pCreature->SetGuildNumber( 1 );

			// 이름
			//pCreature->SetName( pPacket->getName().toString().c_str() );

			// 색깔 정보

			//------------------------------------------------------------
			// Load되지 않았으면 load한다.
			//------------------------------------------------------------
//			LoadCreatureType( pPacket->getMonsterType() );			
		}

		//--------------------------------------------------
		// Effect 붙이기..
		//--------------------------------------------------
		if (pCreature!=NULL)
		{
			// Zone preload listesinde olmayan dinamik burrow spawn'larının
			// sprite pack'i burada tamamlanır.
			CT_BD("[ADDMON-BURROW] before LoadCreatureType id=%d type=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType());
			BOOL loadCreatureResult = LoadCreatureType(pCreature->GetCreatureType());
			CT_BD("[ADDMON-BURROW] after LoadCreatureType id=%d type=%d result=%d action=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType(),
				(int)loadCreatureResult,
				(int)pCreature->GetAction());
			if (auditCreature2221)
			{
				Phoenix_CreatureDrawAudit_Log(
					"GCAddMonsterFromBurrowing dynamic load result objectID=%d packetType=%d creatureType=%d result=%d action=%d dir=%d frame=%d pos=%d,%d",
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

			SetEffectInfo( pCreature, pPacket->getEffectInfo() );
			// 2004, 8, 31, sobeit add start - 트랩에 이펙트 붙임
			switch(pCreature->GetCreatureType())
			{
				case 739: // 트랩 - 랜드 마인
					ExecuteActionInfoFromMainNode(SKILL_CLIENT_LAND_MINE_LIGHT,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	pCreature->GetID(),	
						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);			
					break;
				case 738: // 트랩 - 플레져 스테이션
					ExecuteActionInfoFromMainNode(SKILL_CLIENT_PLEASUER_STATION_LIGHT,pCreature->GetX(), pCreature->GetY(), 0,pCreature->GetDirection(),	pCreature->GetID(),	
						pCreature->GetX(), pCreature->GetY(), 0, 0xffff, NULL, false);			
					break;
			}
			// 2004, 8, 31, sobeit add end -
			//--------------------------------------------------
			// Burrow에서 빠져나오는 Effect == Burrowing하는 Effect
			//--------------------------------------------------		
			CT_BD("[ADDMON-BURROW] before RESULT_MAGIC_HIDE id=%d type=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType());
			ExecuteActionInfoFromMainNode(
				RESULT_MAGIC_HIDE,										// 사용 기술 번호
			
				pCreature->GetX(), pCreature->GetY(), 0,
				pCreature->GetDirection(),														// 사용 방향
				
				OBJECTID_NULL,												// 목표에 대한 정보
				pCreature->GetX(), pCreature->GetY(), 0, 
				
				0,													// 기술의 (남은) 지속 시간		
				
				NULL,
				
				false);			// 기술 첨부터 시작한다.
			CT_BD("[ADDMON-BURROW] after RESULT_MAGIC_HIDE id=%d type=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType());


			TYPE_CREATURETYPE	creatureType = pCreature->GetCreatureType();

			// 몸에 이팩트가 붙는 Creature라면 이팩트를 붙여준다.
			if(creatureType < g_pCreatureTable->GetSize())
			{
				CREATURETABLE_INFO &creatureInfo = g_pCreatureTable->Get(creatureType);

				if(creatureInfo.EffectStatus != EFFECTSTATUS_NULL)
				{
					pCreature->AddEffectStatus((EFFECTSTATUS)creatureInfo.EffectStatus, 0xFFFF);
				}

				if(creatureInfo.EffectSpriteType != MAX_EFFECTSPRITETYPE)
				{
					pCreature->CreateAttachEffect((ENUM_EFFECTSPRITETYPE)creatureInfo.EffectSpriteType, 0xFFFF, 0);
				}
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
