//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectBonusExp.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectBonusExp.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "DBExecuteManager.h"

#include <sstream>

#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectBonusExp::EffectBonusExp(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 

	m_fBonusRate = 2.0f;
	
	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBonusExp::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY 
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBonusExp::unaffect(Creature* pFromCreature)
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

	destroy( pPC->getName().c_str() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBonusExp::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBonusExp::create(const string & name) 
	throw(Error)
{
	__BEGIN_TRY

	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
	
	os << "INSERT INTO EffectBonusExp (Name, YearTime, DayTime, BonusRate) VALUES(" 
		<< "'" << name.c_str() 
		<< "', " << currentYearTime 
		<< ", " << m_Deadline.tv_sec 
		<< ", " << GetBonusRate()
		<< ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBonusExp::destroy(const string & name)
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
	
	os << "DELETE FROM EffectBonusExp WHERE Name = '" << name.c_str() << "'", 
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBonusExp::save(const string & name) 
	throw(Error)
{
	__BEGIN_TRY

	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
	
	os << "UPDATE EffectBonusExp SET "
		<< "YearTime=" << currentYearTime 
		<< ", DayTime=" << m_Deadline.tv_sec
		<< ", BonusRate=" << GetBonusRate() 
		<< " WHERE Name='" << name.c_str() << "'"; 
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectBonusExp::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "EffectBonusExp("
		<< "ObjectID:" << getObjectID()
		<< ",BonusRate:" << m_fBonusRate
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectBonusExpLoader::load(Creature* pCreature) 
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Statement* pStmt = NULL;

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		Result* pResult = pStmt->executeQuery( 
			"SELECT BonusRate, DayTime FROM EffectBonusExp WHERE Name='%s'", 
			pPC->getName().c_str()
		);

		EffectBonusExp* pEffectBonusExp = NULL;
		
		while(pResult->next())
		{
			float fBonusRate = (float)atof(pResult->getField(1));
			DWORD DayTime = pResult->getDWORD(2);

			Timeval currentTime;
			getCurrentTime(currentTime);

			pEffectBonusExp = new EffectBonusExp(pCreature);
	
			if ( pEffectBonusExp != NULL )
			{
				if (currentTime.tv_sec < DayTime) 
				{
					pEffectBonusExp->setDeadline((DayTime - currentTime.tv_sec) * 10);
					pEffectBonusExp->SetBonusRate(fBonusRate);
				} 
				else
				{
					pEffectBonusExp->setDeadline(0);
				}
				
				pCreature->addEffect(pEffectBonusExp);
				pCreature->setFlag(Effect::EFFECT_CLASS_BONUS_EXP);
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

EffectBonusExpLoader* g_pEffectBonusExpLoader = NULL;
