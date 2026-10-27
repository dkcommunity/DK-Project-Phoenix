//////////////////////////////////////////////////////////////////////////////
// Filename    : Untransform.cpp
// Written by  : elca@ewestsoft.com
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Untransform.h"
#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK3.h"
#include "Gpackets/GCDeleteObject.h"
#include "Gpackets/GCRemoveEffect.h"
#include "ZoneUtil.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SkillResultType Untransform::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pSlayer != NULL);

	try 
	{
		if ( pSlayer->isFlag(Effect::EFFECT_CLASS_INSTALL_TURRET) )
		{
			Zone* pZone = pSlayer->getZone();
			Assert(pZone != NULL);

			Effect* pEffect = pSlayer->findEffect(Effect::EFFECT_CLASS_INSTALL_TURRET);
			if ( pEffect != NULL ) pEffect->setDeadline(0);

			GCSkillToSelfOK1 gcOK1;

			gcOK1.setSkillType( SKILL_UN_TRANSFORM );

			// 스킬 후 처리
			postSkillSuccessProcess( getSkillType(), pSlayer, &gcOK1 );

			pSlayer->getPlayer()->sendPacket(&gcOK1);

			return SKILL_RESULT_SUCCESS;
		}
		else if ( pSlayer->isFlag(Effect::EFFECT_CLASS_ADV2HALCYONMK1) )
		{
			Zone* pZone = pSlayer->getZone();
			Assert(pZone != NULL);

			Effect* pEffect = pSlayer->findEffect(Effect::EFFECT_CLASS_ADV2HALCYONMK1);
			if ( pEffect != NULL ) pEffect->setDeadline(0);

			GCSkillToSelfOK1 gcOK1;

			gcOK1.setSkillType( SKILL_UN_TRANSFORM );

			// 스킬 후 처리
			postSkillSuccessProcess( getSkillType(), pSlayer, &gcOK1 );

			pSlayer->getPlayer()->sendPacket(&gcOK1);

			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailException(pSlayer, SKILL_UN_TRANSFORM);
		}
	} 
	catch(Throwable & t) 
	{
//		클라이언트에서 -_- 이런 거 보내지 말란다 흑 ㅠㅠ
//		executeSkillFailException(pSlayer, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SkillResultType Untransform::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);

	try 
	{
		if ( pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF) || 
			pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT) ||
			pVampire->isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF) )
		{
			Zone* pZone = pVampire->getZone();
			Assert(pZone != NULL);
			addUntransformCreature(pZone, pVampire, true);
			
			return SKILL_RESULT_SUCCESS;
		}
	} 
	catch(Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SkillResultType Untransform::execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pOusters != NULL);

	try 
	{
		if ( pOusters->isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH) )
		{
			Zone* pZone = pOusters->getZone();
			Assert(pZone != NULL);

			Effect* pEffect = pOusters->findEffect(Effect::EFFECT_CLASS_SUMMON_SYLPH);
			if ( pEffect != NULL ) pEffect->setDeadline(0);

			GCSkillToSelfOK1 gcOK1;

			gcOK1.setSkillType( SKILL_UN_TRANSFORM );

			// 스킬 후 처리
			postSkillSuccessProcess( getSkillType(), pOusters, &gcOK1 );

			pOusters->getPlayer()->sendPacket(&gcOK1);

			return SKILL_RESULT_SUCCESS;
		}
		else
		{
			executeSkillFailException(pOusters, SKILL_UN_TRANSFORM);
		}
	} 
	catch(Throwable & t) 
	{
//		클라이언트에서 -_- 이런 거 보내지 말란다 흑 ㅠㅠ
//		executeSkillFailException(pOusters, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void Untransform::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pMonster != NULL);

	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);
		addUntransformCreature(pZone, pMonster, true);
	} 
	catch(Throwable & t) 
	{
		//cout << t.toString() << endl;
	}
	
	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH

}

Untransform g_Untransform;
