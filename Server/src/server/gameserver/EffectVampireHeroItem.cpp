//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectVampireHeroItem.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectVampireHeroItem.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectVampireHeroItem::EffectVampireHeroItem(Creature* pCreature)
throw(Error)
: EffectHasRelic( pCreature )
{
	__BEGIN_TRY

		bHPBonus=true;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectVampireHeroItem::EffectVampireHeroItem(Item* pItem)
throw(Error)
: EffectHasRelic( pItem )
{
	__BEGIN_TRY
		bHPBonus=true;
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVampireHeroItem::affect(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY

		// HP 가 두배~
		if ( pCreature->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
			SLAYER_RECORD prev;
			pSlayer->getSlayerRecord(prev);
			pSlayer->initAllStat();
			pSlayer->sendModifyInfo(prev);
		}
		else if ( pCreature->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
			VAMPIRE_RECORD prev;
			pVampire->getVampireRecord(prev);
			pVampire->initAllStat();
			pVampire->sendModifyInfo(prev);
		}
		else if ( pCreature->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
			OUSTERS_RECORD prev;
			pOusters->getOustersRecord(prev);
			pOusters->initAllStat();
			pOusters->sendModifyInfo(prev);
		}

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVampireHeroItem::unaffect(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY

		// HP 원래대로
		if ( pCreature->isSlayer() )
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
			SLAYER_RECORD prev;
			pSlayer->getSlayerRecord(prev);
			pSlayer->initAllStat();
			pSlayer->sendModifyInfo(prev);
		}
		else if ( pCreature->isVampire() )
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
			VAMPIRE_RECORD prev;
			pVampire->getVampireRecord(prev);
			pVampire->initAllStat();
			pVampire->sendModifyInfo(prev);
		}
		else if ( pCreature->isOusters() )
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
			OUSTERS_RECORD prev;
			pOusters->getOustersRecord(prev);
			pOusters->initAllStat();
			pOusters->sendModifyInfo(prev);
		}

		EffectHasRelic::unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectVampireHeroItem::affect(Item* pItem)
throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectVampireHeroItem::toString()
const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectVampireHeroItem("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

