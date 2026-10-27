//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectPromotionBatWing.cpp
// Written by  : svi
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectPromotionBatWing.h"
#include "PlayerCreature.h"
#include "Zone.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Timeval.h"
#include "DBExecuteManager.h"

#include <sstream>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectPromotionBatWing::EffectPromotionBatWing(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	
	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWing::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	__END_CATCH
}

bool EffectPromotionBatWing::isPossibleAffect()
	throw(Error)
{
	__BEGIN_TRY
//	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
//	PlayerCreature* pPC   = dynamic_cast<PlayerCreature*>(pCreature);
	
	return true;
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWing::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert( pPC != NULL );

	//pPC->initAllStatAndSend();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWing::unaffect() 
	throw(Error)
{
	__BEGIN_TRY	

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWing::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert( pPC != NULL );

	Zone* pZone = pPC->getZone();
	Assert( pZone != NULL );

	pPC->removeFlag( getEffectClass() );
	//pPC->initAllStatAndSend();

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pCreature->getObjectID() );
	gcRemoveEffect.addEffectList( getEffectClass() );
	pZone->broadcastPacket( pPC->getX(), pPC->getY(), &gcRemoveEffect );

	destroy( pPC->getName() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWing::create( const string& ownerID )
	throw(Error)
{
	__BEGIN_TRY

	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
	
	os << "INSERT INTO EffectPromotionBatWing (Name, YearTime, DayTime) VALUES(" 
		<< "'" << ownerID.c_str() 
		<< "', " << currentYearTime 
		<< ", " << m_Deadline.tv_sec 
		<< ")";
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWing::destroy( const string& ownerID )
	throw(Error)
{
	__BEGIN_TRY
	
	ostringstream os;
		
	os << "DELETE FROM EffectPromotionBatWing WHERE Name = '" << ownerID.c_str() << "'";
	 
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWing::save( const string& ownerID )
	throw(Error)
{
	__BEGIN_TRY

	Turn_t currentYearTime;
	getCurrentYearTime(currentYearTime);
	
	ostringstream os;
	
	os << "UPDATE EffectPromotionBatWing SET "
		<< "YearTime=" << currentYearTime 
		<< ", DayTime=" << m_Deadline.tv_sec
		<< " WHERE Name='" << ownerID.c_str() << "'"; 
	
	g_GameDBExecuteManager.RequestExecQuery(getEffectClass(), os.str());
	
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectPromotionBatWing::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectPromotionBatWing("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPromotionBatWingLoader::load( Creature* pCreature )
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
			"SELECT DayTime FROM EffectPromotionBatWing WHERE Name='%s'", 
			pPC->getName().c_str()
		);

		EffectPromotionBatWing* pEffect = NULL;
		
		while(pResult->next())
		{
			DWORD DayTime = pResult->getDWORD(1);

			Timeval currentTime;
			getCurrentTime(currentTime);

			pEffect = new EffectPromotionBatWing(pCreature);
	
			if ( pEffect != NULL )
			{
				if (currentTime.tv_sec < DayTime) 
				{
					pEffect->setDeadline((DayTime - currentTime.tv_sec) * 10);
				} 
				else
				{
					pEffect->setDeadline(0);
				}
				
				pCreature->addEffect(pEffect);
				pCreature->setFlag( pEffect->getEffectClass() );
			}
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)
	
	__END_CATCH
}

EffectPromotionBatWingLoader* g_pEffectPromotionBatWingLoader = NULL;

