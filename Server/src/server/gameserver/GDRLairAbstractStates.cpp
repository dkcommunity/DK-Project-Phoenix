#include "GDRLairAbstractStates.h"
#include "StringPool.h"
#include "Gpackets/GCSay.h"
#include "Gpackets/GCGlobalChat.h"
#include "Zone.h"
#include "Monster.h"
#include "Utility.h"
#include "CreatureUtil.h"
#include "ZoneUtil.h"
#include "GDRLairManager.h"
#include "MonsterManager.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "skill/Skill.h"
#include "PCManager.h"
#include "ZoneGroup.h"

void GDRScene::start()
{
	getCurrentTime(m_NextActionTurn);
	m_NextAction = m_ActionList.begin();
}

DWORD GDRScene::heartbeat(Timeval currentTime)
{
	if ( m_NextAction == m_ActionList.end() )
		return m_NextState;

	if ( currentTime < m_NextActionTurn )
		return 0;

	Action* pAction = *m_NextAction;
	Monster* pTarget = (pAction != NULL) ? pAction->getTarget() : NULL;
	const int actionType = (pAction != NULL) ? (int)pAction->getActionType() : -1;
	filelog("GDRLair.log",
		"[SceneHB] before getZone target=%p actionType=%d",
		pTarget, actionType);

	Zone* pZone = (*m_NextAction)->getTarget()->getZone();
	Assert( pZone != NULL );

	filelog("GDRLair.log",
		"[SceneHB] zone=%p zg=%p before execute type=%d",
		pZone,
		(pZone != NULL) ? (void*)pZone->getZoneGroup() : NULL,
		actionType);

	Turn_t result;

	__ENTER_CRITICAL_SECTION( (*(pZone->getZoneGroup())) );

//	cout << (*m_NextAction)->getActionType() << " action execute.." << endl;
	filelog("GDRLair.log", "[SceneHB] locked, execute type=%d", actionType);
	result = (*m_NextAction)->execute();
	filelog("GDRLair.log", "[SceneHB] execute done result=%u", (uint)result);

	__LEAVE_CRITICAL_SECTION( (*(pZone->getZoneGroup())) );

	if ( result == 0 ) ++m_NextAction;
	else
	{
		getCurrentTime( m_NextActionTurn );
		m_NextActionTurn.tv_sec += result / 10;
		m_NextActionTurn.tv_usec += (result % 10) * 100000;
	}

	return 0;
}

Monster*		GDRScene::getGDR() const
{
	return GDRLairManager::Instance().m_pGDR;
}

void			GDRScene::setGDR(Monster* pGDR) const
{
	GDRLairManager::Instance().m_pGDR = pGDR;
}

// GCSay / GCGlobalChat mesaj uzunlugu BYTE: uygulama cap 255.
// 128 clamp GDR 342 sonunu kesiyordu; 255 asilirsa BYTE wrap olmasin diye filelog + kes.
static const uint GDR_CHAT_PACKET_MAX = 255;

static string clampGDRChatMessage(uint strID, const string& src, const char* packetName)
{
	if ( src.size() <= GDR_CHAT_PACKET_MAX )
		return src;

	filelog("GDRLair.log",
		"[ActionSay] strID=%u %s len=%u exceeds max=%u, truncating",
		strID, packetName, (uint)src.size(), GDR_CHAT_PACKET_MAX);
	return src.substr(0, GDR_CHAT_PACKET_MAX);
}

Turn_t GDRScene::ActionSay::execute()
{
	const string rawMsg = g_pStringPool->getString( m_strID );
	const string sayMsg = clampGDRChatMessage( m_strID, rawMsg, "GCSay" );
	const string globalMsg = clampGDRChatMessage( m_strID, string("GDR ") + rawMsg, "GCGlobalChat" );

	GCSay gcSay;
	gcSay.setObjectID( getTarget()->getObjectID() );
	gcSay.setMessage( sayMsg );
	gcSay.setColor( 0x00ffffff );

	cout << "GDR Says : " << gcSay.getMessage() << endl;

	const uint msgLen = (uint)rawMsg.size();
	filelog("GDRLair.log",
		"[ActionSay] after cout strID=%u msgLen=%u globalLen=%u oid=%d",
		m_strID, msgLen, (uint)(rawMsg.size() + 4), (int)getTarget()->getObjectID());

	filelog("GDRLair.log",
		"[ActionSay] before GCSay broadcast zone=%p",
		getTarget()->getZone());
	getTarget()->getZone()->broadcastPacket( getTarget()->getX(), getTarget()->getY(), &gcSay );
	filelog("GDRLair.log", "[ActionSay] after GCSay");

	GCGlobalChat gcGC;
	gcGC.setColor( 0x00ffffff );
	gcGC.setMessage( globalMsg );
	gcGC.setRace( RACE_VAMPIRE );

	filelog("GDRLair.log", "[ActionSay] before GCGlobalChat");
	getTarget()->getZone()->broadcastPacket( &gcGC );
	filelog("GDRLair.log", "[ActionSay] after GCGlobalChat ok");
	return 0;
}

Turn_t GDRScene::ActionWalk::execute()
{
	Monster* pTarget = getTarget();
	Assert( pTarget != NULL );

	Dir_t dir = computeDirection( pTarget->getX(), pTarget->getY(), m_TargetX, m_TargetY );
	ZoneCoord_t nx = 0;
	ZoneCoord_t ny = 0;

	Zone* pZone = pTarget->getZone();
	Assert( pZone != NULL );

	static int offset[5] = { 0, -1, 1, -2, 2 };
	int i = 0;
	
	for ( i = 0 ; i < 5 ; ++i )
	{
		nx = pTarget->getX() + dirMoveMask[dir+offset[i]].x;
		ny = pTarget->getY() + dirMoveMask[dir+offset[i]].y;

		// 2007 테스트 Code
/*		Dir_t old_dir = dir;
		const list<Object*>& oList = pZone->getTile(nx, ny).getObjectList();
		if( isValidZoneCoord( pZone, nx, ny) && i != 0)
		{
			for( list<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++ )
			{
				Object* pTargetObject = *itr;
				Creature* pTargetCreature = NULL;

				pTargetCreature = dynamic_cast<Creature*>(pTargetObject);
				if( pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE )
				{
					knockbackCreature( pTargetCreature->getZone(), pTargetCreature, pTarget->getX(), pTarget->getY() );
					knockbackCreature( pTargetCreature->getZone(), pTargetCreature, pTarget->getX(), pTarget->getY() );
				}
				dir += offset[i];
				break;
			}
			if( old_dir != dir )
				break;
		} */
		if ( isValidZoneCoord(pZone, nx, ny) && !pZone->getTile(nx, ny).isBlocked(pTarget->getMoveMode()) )
		{
			if ( i != 0 )
			{
				int temp;
				SWAP( offset[1], offset[i], temp );
				m_DodgeCount++;
				cout << "DodgeCount : " << m_DodgeCount << endl;
				filelog("GDRLair.log", "DodgeCount : %d", m_DodgeCount);
			}

			dir += offset[i];
			break;
		}
	}

	if ( i != 5 )
	{
		pZone->moveCreature( pTarget, nx, ny, dir );
		if ( nx == m_TargetX && ny == m_TargetY ) return 0;
	}
	else
	{
		m_DodgeCount++;
		cout << "DodgeCount : " << m_DodgeCount << endl;
		filelog("GDRLair.log", "DodgeCount : %d", m_DodgeCount);
	}

	if ( m_DodgeCount > 20 )
	{
		pZone->moveFastMonster( pTarget, pTarget->getX(), pTarget->getY(), m_TargetX, m_TargetY, SKILL_RAPID_GLIDING );
		return 0;
	}

	return m_MoveDelay;
}

Turn_t GDRScene::ActionEffect::execute()
{
	Monster* pTarget = getTarget();
	Assert( pTarget != NULL );
	Zone* pZone = pTarget->getZone();
	Assert( pZone != NULL );

	addSimpleCreatureEffect( pTarget, m_EffectClass, m_Duration, true );
	return 0;
}

Turn_t GDRScene::ActionRemoveEffect::execute()
{
	Monster* pTarget = getTarget();
	Assert( pTarget != NULL );

	Effect* pEffect = pTarget->findEffect( m_EffectClass );
	if ( pEffect != NULL ) pEffect->setDeadline(0);

	return 0;
}

Turn_t GDRScene::ActionHollywood::execute()
{
	Monster* pTarget = getTarget();
	Assert( pTarget != NULL );

	GCSkillToObjectOK4 gcOK;
	gcOK.setTargetObjectID( pTarget->getObjectID() );
	gcOK.setSkillType( SKILL_ATTACK_MELEE );

	pTarget->getZone()->broadcastPacket( pTarget->getX(), pTarget->getY(), &gcOK );
	return 0;
}

Turn_t GDRScene::ActionWarp::execute()
{
	Monster* pTarget = getTarget();
	Assert( pTarget != NULL );

	Zone* pZone = pTarget->getZone();
	Assert( pZone != NULL );

	pZone->moveFastMonster( pTarget, pTarget->getX(), pTarget->getY(), m_TargetX, m_TargetY, SKILL_TELEPORT );

	return 0;
}

void MonsterSummonState::start()
{
	m_bStartSummon = false;
	
	if ( m_TimeLimit ) TimerState::start();
}

bool MonsterSummonState::IsCurrentSummonMonster(MonsterType_t MType)
{
	list<SummonInfo*>::iterator itr;
	itr = (*m_CurrentSummonInfo)->getSummonInfos().begin();
	for( ; itr != (*m_CurrentSummonInfo)->getSummonInfos().end(); ++itr)
	{
		if ( (*itr)->getMonsterType() == MType )
		{
			return true;
		}
	}
	
	return false;

}
DWORD MonsterSummonState::heartbeat(Timeval currentTime)
{
	if ( m_TimeLimit )
	{
		DWORD nState = TimerState::heartbeat(currentTime);
		if ( nState != 0 ) return nState;
	}

	__ENTER_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )

	const PCManager* pPM = m_pZone->getPCManager();

	if ( pPM->getSize() < 1 )
	{
		m_pZone->getZoneGroup()->unlock();
		return m_FailState;
	}

	// SQL wave tablosu bos olabilir (operator 0 wave). Bos listede
	// m_CurrentSummonInfo == end(); IsCurrentSummonMonster deref UB olur.
	if ( m_GroupSummonInfos.empty() )
	{
		m_pZone->getZoneGroup()->unlock();
		return m_ClearState;
	}

	Timeval diffTime = timediff( m_previousTime, currentTime );
	
	if ( diffTime.tv_sec > 1 )
	{
		// 남아있는 소환 몬스터 수 검사(질드레에 의해 소환된 몬스터류만을 카운팅한다.)
		int iRemainSummonMonster = 0;
		MonsterManager* pMM = m_pZone->getMonsterManager();
		
		map< ObjectID_t, Creature* > Creatures;
		map< ObjectID_t, Creature* >::iterator itr;
		
		Creature *pCreature;
		Monster *pMonster;
		
		Creatures = pMM->getCreatures();
		itr = Creatures.begin();
		for(; itr != Creatures.end(); ++itr)
		{
			pCreature = (itr->second);
			if ( pCreature != NULL && !pCreature->isMonster() )
			{
				continue;
			}
			
			pMonster = dynamic_cast<Monster *>(pCreature);
				
			if ( IsCurrentSummonMonster(pMonster->getMonsterType()) )
			{
				++iRemainSummonMonster;	
			}
		}
		
		if ( iRemainSummonMonster < 1 )
		{
			if (m_bStartSummon )
			{
				++m_CurrentSummonInfo;
			}
			else
			{
				m_bStartSummon = true;
			}
			
			if ( m_CurrentSummonInfo == m_GroupSummonInfos.end() )
			{
				m_pZone->getZoneGroup()->unlock();
				return m_ClearState;
			}
			else
			{
				(*m_CurrentSummonInfo)->executeSummon( m_pZone );
			}
		}
		
		m_previousTime = currentTime;
	}

	__LEAVE_CRITICAL_SECTION( (*(m_pZone->getZoneGroup())) )

	return 0;
}

void MonsterSummonState::GroupSummonInfo::executeSummon( Zone* pZone )
{
	list<SummonInfo*>::iterator itr = m_SummonInfos.begin();
	list<SummonInfo*>::iterator endItr = m_SummonInfos.end();

	for ( ; itr != endItr ; ++itr )
	{
		for ( int i=0; i<(*itr)->m_MonsterNum; ++i )
		{
			cout << "Summon Monster " << (*itr)->m_MonsterType << endl;
			Monster* pMonster = new Monster( (*itr)->m_MonsterType );
			pMonster->setTreasure(true);
			try
			{
				pZone->addCreature( pMonster, (*itr)->m_X, (*itr)->m_Y, 0 );
			}
			catch( Exception& e )
			{
				cout << e.toString() << endl;
			}
		}
	}
}
