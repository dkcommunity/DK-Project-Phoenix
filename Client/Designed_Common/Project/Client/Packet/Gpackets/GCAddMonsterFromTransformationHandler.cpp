//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddMonsterFromTransformationHandler.cpp
// Written By  : elca@ewestsoft.com
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "crash_trace_tool.h"
#include "GCAddMonsterFromTransformation.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "MTopView.h"

extern "C" int Phoenix_CreatureDrawAudit_ShouldLog(int creatureType);
extern "C" void Phoenix_CreatureDrawAudit_Log(const char* format, ...);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddMonsterFromTransformationHandler::execute ( GCAddMonsterFromTransformation * pPacket , Player * pPlayer )

{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	EFFECTSTATUS	statusShadowCheck = EFFECTSTATUS_NULL;
	CT_BD("[ADDMON-XFORM] enter id=%d type=%d xy=%d,%d dir=%d hp=%d/%d zone=%p",
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
			"GCAddMonsterFromTransformation entry objectID=%d monsterType=%d name=%s x=%d y=%d dir=%d hp=%d/%d zonePtr=%p playerPtr=%p",
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
		#ifdef __DEBUG_OUTPUT__
			DEBUG_ADD_FORMAT("[AddMonster] type=%d, id=%d, xy=(%d,%d), dir=%d", 
								pPacket->getMonsterType(),
								pPacket->getObjectID(), 
								pPacket->getX(), pPacket->getY(), 
								pPacket->getDir());
		#endif

		//AfxMessageBox( pPacket->toString().c_str() );
		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());


#if __CONTENTS(__TIPOJYU_CASTLE)	//몬스터 반투명 그림자 설정
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
#endif // __TIPOJYU_CASTLE

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
			// Re-entry sirasinda dead monster (HP=0) GCAddMonsterFromTransformation
			// olarak gelirse default ACTION_STAND ile ayakta gosteriliyordu.
			// SetCorpse() ile m_bAlive=false + ACTION_DIE + ground state set.
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
#endif


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
		else
		{
			// creature가 아니면 지운다.
			bool reAdd = false;

			if (pCreature->GetClassType()!=MCreature::CLASS_CREATURE)
			{
				g_pZone->RemoveCreature( pCreature->GetID() );

				pCreature = new MCreature;

				pCreature->SetID(pPacket->getObjectID());

				pCreature->SetZone( g_pZone );
				
				pCreature->SetName( (*g_pCreatureTable)[pPacket->getMonsterType()].Name.GetString() );
				//pCreature->SetName( pPacket->getMonsterName().c_str() );

				pCreature->SetPosition( pPacket->getX(), pPacket->getY() );				

				reAdd = true;
			}
			else
			{
				pCreature->MovePosition( pPacket->getX(), pPacket->getY() );				
			}

			pCreature->SetCreatureType( pPacket->getMonsterType() );
			pCreature->SetGroundCreature();
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );
			pCreature->SetAction( ACTION_STAND );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetStatus( MODIFY_MAX_HP, pPacket->getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, pPacket->getCurrentHP() );

			// 2026-05-19 DEATH STATE FIX (bkz GCAddMonsterHandler same pattern)
			if (pPacket->getCurrentHP() == 0)
			{
				pCreature->SetCorpse();
			}

			// 임시로..
			pCreature->SetGuildNumber( 1 );
			
			if (reAdd)
			{
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
//					LoadCreatureType( pPacket->getMonsterType() );			
				}
			}
		}

		if (pCreature!=NULL)
		{
			// Zone preload listesinde olmayan transformation spawn'larının
			// sprite pack'i burada tamamlanır.
			CT_BD("[ADDMON-XFORM] before LoadCreatureType id=%d type=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType());
			BOOL loadCreatureResult = LoadCreatureType(pCreature->GetCreatureType());
			CT_BD("[ADDMON-XFORM] after LoadCreatureType id=%d type=%d result=%d",
				(int)pPacket->getObjectID(),
				(int)pCreature->GetCreatureType(),
				(int)loadCreatureResult);
			if (auditCreature2221)
			{
				Phoenix_CreatureDrawAudit_Log(
					"GCAddMonsterFromTransformation dynamic load result objectID=%d packetType=%d creatureType=%d result=%d action=%d dir=%d frame=%d pos=%d,%d",
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
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );

			//--------------------------------------------------
			// Burrow에서 빠져나오는 Effect == Burrowing하는 Effect
			//--------------------------------------------------		
			ExecuteActionInfoFromMainNode(
				RESULT_MAGIC_HIDE,										// 사용 기술 번호
			
				pCreature->GetX(), pCreature->GetY(), 0,
				pCreature->GetDirection(),														// 사용 방향
				
				OBJECTID_NULL,												// 목표에 대한 정보
				pCreature->GetX(), pCreature->GetY(), 0, 
				
				0,													// 기술의 (남은) 지속 시간		
				
				NULL,
				
				false);			// 기술 첨부터 시작한다.

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
