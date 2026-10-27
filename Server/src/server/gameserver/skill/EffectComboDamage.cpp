//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectComboDamage.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectComboDamage.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Monster.h"
#include "DB.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectComboDamage::EffectComboDamage(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);
	m_CurrentStep = 0;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectComboDamage::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectComboDamage " << "begin begin" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectComboDamage " << "begin end" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectComboDamage::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	Creature* pAttacker = pZone->getCreature( m_UserObjectID );
	// 공격자가 없으면 끝낸다.
	if ( pAttacker == NULL )
	{
		setDeadline(0);
		return;
	}

	GCModifyInformation	gcMI;
	GCSkillToObjectOK2	_GCSkillToObjectOK2;
	GCSkillToObjectOK4	_GCSkillToObjectOK4;

	// 콤보 단계에 따른 설정
	Damage_t	damage	= m_Damage[m_CurrentStep];
	m_CurrentStep++;
	if ( m_CurrentStep > m_MaxStep )
	{
		setDeadline(0);
		return;
	}
	Turn_t		delay	= m_Delay[m_CurrentStep];

	// 데미지 딜링
	if ( pCreature->isPC() )
	{
		::setDamage( pCreature, damage, pAttacker, m_SkillType, &_GCSkillToObjectOK2, &gcMI );
	}
	else if ( pCreature->isMonster() )
	{
		Monster* pMonster = dynamic_cast<Monster*>(pCreature);

		::setDamage( pMonster, damage, pMonster, m_SkillType, NULL, &gcMI );

		pMonster->addEnemy( pAttacker );
	}

	if ( pCreature->isPC() )
	{
		_GCSkillToObjectOK2.setObjectID(1);
		_GCSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
		_GCSkillToObjectOK2.setDuration(0);

		pCreature->getPlayer()->sendPacket(&_GCSkillToObjectOK2);
	}

	_GCSkillToObjectOK4.setTargetObjectID( pCreature->getObjectID() );
	_GCSkillToObjectOK4.setSkillType(SKILL_ATTACK_MELEE);
	_GCSkillToObjectOK4.setDuration(0);

	pZone->broadcastPacket( pCreature->getX(), pCreature->getY(), &_GCSkillToObjectOK4, pCreature );

	setNextTime( delay );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectComboDamage::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG


	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectComboDamage::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectComboDamage::toString() const 
	throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectComboDamage("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH
}

