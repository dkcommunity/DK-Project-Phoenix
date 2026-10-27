//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectRankBonusExpBlue.cpp
// Written by  : Kim sung yong
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectRankBonusExpBlue.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "DBExecuteManager.h"

#include <sstream>

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectRankBonusExpBlue::EffectRankBonusExpBlue(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY 

		setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRankBonusExpBlue::affect(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRankBonusExpBlue::unaffect(Creature* pFromCreature)
throw(Error)
{
	__BEGIN_TRY 

		Assert(pFromCreature != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pFromCreature);
	Assert( pPC != NULL );

	Player* pPlayer = pPC->getPlayer();
	Assert( pPlayer != NULL );

	pPC->removeFlag( getEffectClass() );

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pPC->getObjectID() );
	gcRemoveEffect.addEffectList( getEffectClass() );

	pPlayer->sendPacket( &gcRemoveEffect );

	destroy( pPC->getPlayer()->getID() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRankBonusExpBlue::unaffect()
throw(Error)
{
	__BEGIN_TRY

		Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRankBonusExpBlue::create(const string & name) 
throw(Error)
{
	__BEGIN_TRY
	
	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
		
	os << "INSERT INTO EffectRankBonusExpBlue (Name, YearTime, DayTime) VALUES(" 
		<< "'" << name.c_str()
		<< "', " << currentYearTime
		<< ", " << m_Deadline.tv_sec << ")";
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRankBonusExpBlue::destroy(const string & name)
throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
		
	os << "DELETE FROM EffectRankBonusExpBlue WHERE Name = '" << name.c_str() << "'"; 
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());

		__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRankBonusExpBlue::save(const string & name) 
throw(Error)
{
	__BEGIN_TRY
	
	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
		
	os << "UPDATE EffectRankBonusExpBlue SET "
		<< "YearTime=" << currentYearTime
		<< ", DayTime=" << m_Deadline.tv_sec
		<< " WHERE Name='" << name.c_str() << "'";
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());

		__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectRankBonusExpBlue::toString()
const throw()
{
	__BEGIN_TRY

		StringStream msg;

	msg << "EffectRankBonusExpBlue("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectRankBonusExpBlueLoader::load(Creature* pCreature) 
throw(Error)
{
	__BEGIN_TRY

		Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( "SELECT DayTime FROM EffectRankBonusExpBlue WHERE Name='%s'", 
			pPC->getName().c_str());

		while(pResult->next())
		{
			uint i = 0;

			DWORD DayTime   = pResult->getDWORD(++i);

			Timeval currentTime;
			getCurrentTime(currentTime);

			EffectRankBonusExpBlue* pEffectRankBonusExpBlue = new EffectRankBonusExpBlue(pCreature);

			if (currentTime.tv_sec < DayTime) 
			{
				pEffectRankBonusExpBlue->setDeadline((DayTime - currentTime.tv_sec) * 10);

				pCreature->addEffect(pEffectRankBonusExpBlue);
				pCreature->setFlag(Effect::EFFECT_CLASS_RANK_BONUS_EXP_BLUE);

			} 
			else
			{
				pEffectRankBonusExpBlue->destroy( pPC->getName().c_str());
				SAFE_DELETE( pEffectRankBonusExpBlue );
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

		__END_CATCH
}

EffectRankBonusExpBlueLoader* g_pEffectRankBonusExpBlueLoader = NULL;
