//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectRestore.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectRestore.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "DBExecuteManager.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectRestore::EffectRestore(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	// 서버 전용 Effect이다. by sigi. 2002.11.14
    m_bBroadcastingEffect = false;

	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::affect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	//cout << "EffectRestore" << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);
	destroy(pCreature->getName());

	//cout << "EffectRestore" << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectRestore" << "unaffect BEGIN" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	//cout << "EffectRestore" << "unaffect END" << endl;
						
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)
	throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::create(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY
	
	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
	
	os << "INSERT INTO EffectRestore "
		<< "(OwnerID, YearTime, DayTime)"
		<< " VALUES('" << ownerID
		<< "' , " << currentYearTime
		<< " , " << m_Deadline.tv_sec
		<< ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::destroy(const string & ownerID)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "DELETE FROM EffectRestore WHERE OwnerID = '" << ownerID << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());	
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestore::save(const string & ownerID) 
	throw(Error)
{
	__BEGIN_TRY

	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
	
	os << "UPDATE EffectRestore SET "
		<< "YearTime = " << currentYearTime
		<< ",DayTime = " << m_Deadline.tv_sec
		<< " WHERE OwnerID = '" << ownerID << "'";
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectRestore::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectRestore("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRestoreLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		StringStream sql;
		sql << "SELECT DayTime FROM EffectRestore"
			<< " WHERE OwnerID = '" << pCreature->getName() << "'";

		Result* pResult = pStmt->executeQuery(sql.toString());

		while(pResult->next())
		{
			if (pCreature->isSlayer()) 
			{
				Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

				uint i = 0;
				int DayTime = pResult->getDWORD(++i);

				Timeval currentTime;
				getCurrentTime(currentTime);

				EffectRestore* pEffectRestore = new EffectRestore(pCreature);
		
				if (currentTime.tv_sec < DayTime) 
				{
					pEffectRestore->setDeadline((DayTime - currentTime.tv_sec)*10);
	
					pSlayer->addEffect(pEffectRestore);
					pSlayer->setFlag(Effect::EFFECT_CLASS_RESTORE);
				} 
				else 
				{
					pEffectRestore->setDeadline(6000);
	
					pSlayer->addEffect(pEffectRestore);
					pSlayer->setFlag(Effect::EFFECT_CLASS_RESTORE);
				}
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

EffectRestoreLoader* g_pEffectRestoreLoader = NULL;
