//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBloodDrain.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectBloodDrain.h"
#include "Creature.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "EventMorph.h"
#include "PCManager.h"
#include "GamePlayer.h"
#include "Gpackets/GCMorph1.h"
#include "Gpackets/GCMorphVampire2.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCChangeDarkLight.h"
#include "Gpackets/GCRemoveEffect.h"
#include "PCFinder.h"
#include "DBExecuteManager.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBloodDrain::EffectBloodDrain(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::unaffect(Creature* pFromCreature)
	throw(Error)
{
	__BEGIN_TRY 

	//cout << "EffectBloodDrain" << "unaffect BEGIN" << endl;
	Assert(pFromCreature != NULL);

	if ( pFromCreature->isSlayer() )
	{

		// 2007 08 13 슬레이어 뱀파이어로 변환 되지 않음
		// 요기부터
		Player* pPlayer = pFromCreature->getPlayer();

		Assert(pPlayer != NULL);

		// 20070822 add
		pFromCreature->removeFlag( Effect::EFFECT_CLASS_BLOOD_DRAIN );

		// GamePlayer에 Event를 붙여서 heartbeat를 다 수행한후 지워준다.
		// 동기화 문제가 없을까? -_-; 다이어그램 상으론 문제가 없쥐만 -_-;
		GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);

		destroy(pFromCreature->getName()); 

		GCRemoveEffect gcRemoveEffect;
		gcRemoveEffect.setObjectID(pFromCreature->getObjectID());
		gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_BLOOD_DRAIN);
//		pZone->broadcastPacket(pFromCreature->getX(), pFromCreature->getY(), &gcRemoveEffect);
		pPlayer->sendPacket(&gcRemoveEffect);

	}
	else
	{
		// 시야 복구.
		Assert(pFromCreature->isOusters());

		Player* pPlayer = pFromCreature->getPlayer();
		Assert(pPlayer != NULL);

		pFromCreature->removeFlag( Effect::EFFECT_CLASS_BLOOD_DRAIN );

		Sight_t oldSight = pFromCreature->getSight();
		Sight_t newSight = pFromCreature->getEffectedSight();

		if ( oldSight != newSight )
		{
			GCModifyInformation gcMI;
			pFromCreature->setSight(newSight);
			pFromCreature->getZone()->updateScan(pFromCreature, oldSight, pFromCreature->getSight());
			gcMI.addShortData(MODIFY_VISION, pFromCreature->getSight());
			pFromCreature->getPlayer()->sendPacket(&gcMI);

			GCChangeDarkLight gcChangeDarkLight;
			gcChangeDarkLight.setDarkLevel(13);
			gcChangeDarkLight.setLightLevel(min(6,(int)newSight));
			pFromCreature->getPlayer()->sendPacket(&gcChangeDarkLight);
		}

		// DB에서 지워뿐다.
		destroy( pFromCreature->getName() );
	}

	//cout << "EffectBloodDrain" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectBloodDrain" << "unaffect BEGIN" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	//cout << "EffectBloodDrain" << "unaffect END" << endl;
						
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::create(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY

	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
	
	os << "INSERT INTO EffectBloodDrain (OwnerID , YearTime, DayTime, Level) VALUES("
		<< "'" << ownerID.c_str() 
		<< "', " << currentYearTime
		<< ", " << m_Deadline.tv_sec
		<< ", " << (int)m_Level << ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::destroy(const string & ownerID)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "DELETE FROM EffectBloodDrain WHERE OwnerID = '" << ownerID.c_str() << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrain::save(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY
	
	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);

	ostringstream os;
	
	os << "UPDATE EffectBloodDrain SET "
		<< "YearTime=" << currentYearTime
		<< ", DayTime=" << m_Deadline.tv_sec
		<< ", Level=" << (int)m_Level
		<< " WHERE OwnerID='" << ownerID.c_str() << "'"; 
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBloodDrain::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectBloodDrain("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBloodDrainLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);
	if ( !pCreature->isSlayer() && !pCreature->isOusters() ) return;

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		/*
		StringStream sql;

		sql << "SELECT DayTime, Level FROM EffectBloodDrain"
			<< " WHERE OwnerID = '" << pCreature->getName() 
			<< "'";

		Result* pResult = pStmt->executeQuery(sql.toString());
		*/

		Result* pResult = pStmt->executeQuery( "SELECT DayTime, Level FROM EffectBloodDrain WHERE OwnerID='%s'", 
												pCreature->getName().c_str());

		while(pResult->next())
		{
			uint i = 0;

			int DayTime = pResult->getDWORD(++i);

			Timeval currentTime;
			getCurrentTime(currentTime);

			EffectBloodDrain* pEffectBloodDrain = new EffectBloodDrain(pCreature);
	
			if (currentTime.tv_sec + 600 < DayTime) 
			{
				pEffectBloodDrain->setDeadline((DayTime - currentTime.tv_sec)* 10);
//				2007 08 13 무한대로 설정
//				pEffectBloodDrain->setDeadline(99999999);
				pEffectBloodDrain->setLevel(pResult->getBYTE(++i));

				pCreature->addEffect(pEffectBloodDrain);
				pCreature->setFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
			} 
			else 
			{
				//pEffectBloodDrain->setDeadline(6000);
				pEffectBloodDrain->setDeadline(6000);
				pEffectBloodDrain->setLevel(pResult->getBYTE(++i));

				pCreature->addEffect(pEffectBloodDrain);
				pCreature->setFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

EffectBloodDrainLoader* g_pEffectBloodDrainLoader = NULL;
