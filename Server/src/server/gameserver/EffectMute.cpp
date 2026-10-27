//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectMute.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectMute.h"
#include "Creature.h"
#include "GamePlayer.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"
#include "DBExecuteManager.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectMute::EffectMute(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::affect() throw(Error)
{
	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);
}

void EffectMute::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 
	
	pCreature->setFlag(getEffectClass());

	GCAddEffect gcAddEffect;
	
	gcAddEffect.setObjectID( pCreature->getObjectID() );
	gcAddEffect.setEffectID( getSendEffectClass() );
	gcAddEffect.setDuration( getRemainDuration() );

	pCreature->getPlayer()->sendPacket( &gcAddEffect );
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::unaffect(Creature* pFromCreature)
	throw(Error)
{
	__BEGIN_TRY 

	Assert(pFromCreature != NULL);

	pFromCreature->removeFlag(getEffectClass());

	GCRemoveEffect gcRemoveEffect;
	
	gcRemoveEffect.setObjectID( pFromCreature->getObjectID() );
	gcRemoveEffect.addEffectList( getSendEffectClass() );
	pFromCreature->getPlayer()->sendPacket( &gcRemoveEffect );

	destroy( pFromCreature->getName() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectMute" << "unaffect BEGIN" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	//cout << "EffectMute" << "unaffect END" << endl;
						
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::create(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY
	
	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
		
	os << "INSERT INTO EffectMute (OwnerID , YearTime, DayTime) VALUES(" 
		<< "'" << ownerID.c_str()
		<< "', " << currentYearTime
		<< ", " << m_Deadline.tv_sec << ")";
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::destroy(const string & ownerID)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
		
	os << "DELETE FROM EffectMute WHERE OwnerID = '" << ownerID.c_str() << "'";
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());	
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMute::save(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY
	
	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
		
	os << "UPDATE EffectMute SET "
		<< "YearTime=" << currentYearTime
		<< ", DayTime=" << m_Deadline.tv_sec
		<< " WHERE OwnerID='" << ownerID.c_str() << "'";
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectMute::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectMute("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectMuteLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT DayTime FROM EffectMute WHERE OwnerID='%s'", 
												pCreature->getName().c_str());

		while(pResult->next())
		{
			uint i = 0;

			int DayTime = pResult->getDWORD(++i);

			Timeval currentTime;
			getCurrentTime(currentTime);

			EffectMute* pEffectMute = new EffectMute(pCreature);
	
			if (currentTime.tv_sec < DayTime) 
			{
				pEffectMute->setDeadline((DayTime - currentTime.tv_sec)* 10);

				pCreature->addEffect(pEffectMute);
				pCreature->setFlag(Effect::EFFECT_CLASS_MUTE);
			} 
			else 
			{
				pEffectMute->destroy( pCreature->getName() );
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

EffectMuteLoader* g_pEffectMuteLoader = NULL;
