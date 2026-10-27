//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectFeebleVirus.cpp
// Written by  : elca
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EffectFeebleVirus.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "Player.h"
#include "SkillUtil.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCRemoveEffect.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectFeebleVirus::EffectFeebleVirus(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	m_UserObjectID = 0;
	setTarget(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFeebleVirus::affect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectFeebleVirus " << "affect BEGIN" << endl;

	Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	affect(pCreature);

	//cout << "EffectFeebleVirus " << "affect END" << endl;

	__END_CATCH 

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFeebleVirus::affect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectFeebleVirus " << "affect BEGIN" << endl;

	Assert(pCreature != NULL);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 기술 사용자를 가져온다.
	// !! 이미 존을 나갔을 수 있으므로 NULL 이 될 수 있다.
	// by bezz. 2003.1.4
	Creature* pCastCreature = pZone->getCreature( m_UserObjectID );

	if ( pCastCreature == NULL )
	{
		setNextTime(m_Tick);

		return;
	}

	// 무적상태 체크. by sigi. 2002.9.5
	if ( canAttack( pCastCreature, pCreature )
		 && !pCreature->isFlag(Effect::EFFECT_CLASS_COMA)
		)
	{
		// 슬레이어일 경우에만 독 데미지가 존재한다.
		if (pCreature->isSlayer())
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);
			Assert(pSlayer != NULL);

			GCModifyInformation gcMI;
			::setDamage( pSlayer, m_Damage, pCastCreature, SKILL_FEEBLE_VIRUS, &gcMI );

			Player* pPlayer = pCreature->getPlayer();
			Assert(pPlayer != NULL);
			pPlayer->sendPacket(&gcMI);
		}
		else if (pCreature->isVampire())
		{
			Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);
			Assert(pVampire != NULL);

			GCModifyInformation gcMI;
			::setDamage( pVampire, m_Damage, pCastCreature, SKILL_FEEBLE_VIRUS, &gcMI );

			Player* pPlayer = pCreature->getPlayer();
			Assert(pPlayer != NULL);
			pPlayer->sendPacket(&gcMI);
		}
		else if (pCreature->isOusters())
		{
			Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);
			Assert(pOusters != NULL);

			GCModifyInformation gcMI;
			::setDamage( pOusters, m_Damage, pCastCreature, SKILL_FEEBLE_VIRUS, &gcMI );

			Player* pPlayer = pCreature->getPlayer();
			Assert(pPlayer != NULL);
			pPlayer->sendPacket(&gcMI);
		}
		else if (pCreature->isMonster())
		{
			Monster* pMonster = dynamic_cast<Monster*>(pCreature);
			Assert(pMonster != NULL);

			::setDamage( pMonster, m_Damage, pCastCreature, SKILL_FEEBLE_VIRUS );
		}
	}

	setNextTime(m_Tick);

	//cout << "EffectFeebleVirus " << "affect END" << endl;

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFeebleVirus::unaffect(Creature* pCreature)
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	//cout << "EffectFeebleVirus " << "unaffect BEGIN" << endl;

	Assert(pCreature != NULL);

	// 크리쳐에게서 플래그를 제거한다.
	pCreature->removeFlag(Effect::EFFECT_CLASS_FEEBLE_VIRUS);

	Zone* pZone = pCreature->getZone();
	Assert(pZone != NULL);

	// 존에서 이펙트가 사라졌다고 알려준다.
	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID(pCreature->getObjectID());
	gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_FEEBLE_VIRUS);
	pZone->broadcastPacket(pCreature->getX(), pCreature->getY(), &gcRemoveEffect);

	//cout << "EffectFeebleVirus " << "unaffect END" << endl;

	__END_DEBUG
	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectFeebleVirus::unaffect()
	throw(Error)
{
	__BEGIN_TRY

	//cout << "EffectFeebleVirus " << "unaffect BEGIN" << endl;

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	unaffect(pCreature);

	//cout << "EffectFeebleVirus " << "unaffect END" << endl;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectFeebleVirus::toString()
	const throw()
{
	__BEGIN_TRY

	StringStream msg;
	msg << "EffectFeebleVirus("
		<< "ObjectID:" << getObjectID()
		<< ")";
	return msg.toString();

	__END_CATCH

}

