//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectIllusionInversion.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectIllusionInversion.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

#ifdef VERSION_SKILL_1
	#include "HitRoll.h"
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectIllusionInversion::EffectIllusionInversion(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_Damage = 10;
	m_Tick = 10;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectIllusionInversion::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectIllusionInversion " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectIllusionInversion " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectIllusionInversion::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 스킬 사용자를 가져온다.
	// !! 이미 존을 나갔을 수 있으므로 NULL이 될 수 있다.
	// by bezz. 2003.1.4
	Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

//	Damage_t IllusionInversionDamage = m_Damage;
///////////////////////////////////////////////////////////////////////////////////////////////////////
//		저항공식 적용...
#ifndef VERSION_SKILL_1
	Damage_t IllusionInversionDamage = computeMagicDamage(pCreature, m_Damage, SKILL_ILLUSION_INVERSION);
#else
	Damage_t IllusionInversionDamage = m_Damage;

	bool bHitRoll = HitRoll::isSuccessMagic( pCastCreature, pCreature );
#endif

	if ( !(pZone->getZoneLevel() & COMPLETE_SAFE_ZONE) 
#ifdef VERSION_SKILL_1
		&& bHitRoll
#endif
			)
	{
		if (pCreature->isSlayer())
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

			GCModifyInformation gcMI;
			::setDamage( pSlayer, IllusionInversionDamage, pCastCreature, SKILL_ILLUSION_INVERSION, &gcMI, NULL, true, false );
			pSlayer->getPlayer()->sendPacket(&gcMI);
		}
		else if (pCreature->isVampire())
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

			GCModifyInformation gcMI;
			::setDamage( pVampire, IllusionInversionDamage, pCastCreature, SKILL_ILLUSION_INVERSION, &gcMI, NULL, true, false );
			pVampire->getPlayer()->sendPacket(&gcMI);
		}
		else if (pCreature->isOusters())
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

			GCModifyInformation gcMI;
			::setDamage( pOusters, IllusionInversionDamage, pCastCreature, SKILL_ILLUSION_INVERSION, &gcMI, NULL, true, false );
			pOusters->getPlayer()->sendPacket(&gcMI);
		}
		else if(pCreature->isMonster())
		{
			Monster* pMonster = dynamic_cast<Monster*>(pCreature);

			::setDamage( pMonster, IllusionInversionDamage, pCastCreature, SKILL_ILLUSION_INVERSION, NULL, NULL, true, false );
		}
	}
	else 
	{
		return;
	}

	GCStatusCurrentHP	pkt;
	Slayer* pSlayer = dynamic_cast<Slayer*>(pCastCreature);

	if( pSlayer == NULL )
		return;

	HP_t RemainHP = HP_t(pSlayer->getHP(ATTR_CURRENT)+(getHealDamage()));

	if( RemainHP >= pSlayer->getHP(ATTR_MAX) )
		RemainHP = pSlayer->getHP(ATTR_MAX);

	pSlayer->setHP( RemainHP, ATTR_CURRENT );

	pkt.setObjectID(pSlayer->getObjectID());
	pkt.setCurrentHP(RemainHP);
	pZone->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &pkt);
	
	setNextTime(m_Tick);

	//cout << "EffectIllusionInversion " << "end" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectIllusionInversion::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	Zone* pZone = pCreature->getZone();
	Assert( pZone != NULL );

	pCreature->removeFlag(Effect::EFFECT_CLASS_ILLUSION_INVERSION);

	// 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_ILLUSION_INVERSION);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectIllusionInversion::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectIllusionInversion::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectIllusionInversion("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

