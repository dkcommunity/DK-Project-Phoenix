#include "EffectPremiumService.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "DB.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectPremiumService::EffectPremiumService(Creature* pCreature)
throw(Error)
{
    __BEGIN_TRY

        setTarget(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPremiumService::affect(Creature* pCreature)
throw(Error)
{
    __BEGIN_TRY
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPremiumService::unaffect(Creature* pFromCreature)
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
void EffectPremiumService::unaffect()
throw(Error)
{
    __BEGIN_TRY

    Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
    unaffect(pCreature);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
string EffectPremiumService::toString()
const throw()
{
    __BEGIN_TRY

        StringStream msg;

    msg << "EffectPremiumService("
        << "ObjectID:" << getObjectID()
        << ")";

    return msg.toString();

    __END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectPremiumServiceLoader::load(Creature* pCreature)
throw(Error)
{
    __BEGIN_TRY
/*
        Assert(pCreature != NULL);

    Statement* pStmt = NULL;

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);

    BEGIN_DB
    {
        pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

        Result* pResult = pStmt->executeQuery( "SELECT DayTime FROM EffectPremiumService WHERE Name='%s'",
            pPC->getName().c_str());

        while(pResult->next())
        {
            uint i = 0;

            DWORD DayTime = pResult->getDWORD(++i);

            Timeval currentTime;
            getCurrentTime(currentTime);

            EffectPremiumService* pEffectPremiumService = new EffectPremiumService(pCreature);

            if (currentTime.tv_sec < DayTime)
            {
                pEffectPremiumService->setDeadline((DayTime - currentTime.tv_sec) * 10);

                pCreature->addEffect(pEffectPremiumService);
                pCreature->setFlag(Effect::EFFECT_CLASS_JOYMAX_PREMIUM_SERVICE);
            }
            else
            {
                pEffectPremiumService->destroy( pPC->getName().c_str());
                SAFE_DELETE( pEffectPremiumService );
            }
        }

        SAFE_DELETE(pStmt);
    }
    END_DB(pStmt)
*/
        __END_CATCH
}

EffectPremiumServiceLoader* g_pEffectPremiumServiceLoader = NULL;

