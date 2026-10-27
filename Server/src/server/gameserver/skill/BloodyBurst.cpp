//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyBurst.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "BloodyBurst.h"
#include "EffectBloodyBurst.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// 몬스터 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
void BloodyBurst::execute(Monster* pMonster)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert(pMonster != NULL);

	try 
	{
		Zone* pZone = pMonster->getZone();

		Assert(pZone != NULL);

		SkillType_t	SkillType = SKILL_BLOODY_BURST;

		// 이미 폭주 상태이면 넘어간다~
		if( pMonster->isFlag(Effect::EFFECT_CLASS_BLOODY_BURST)  )
		{
			return;
		}

		// 지속 시간을 계산한다.
		SkillInput input(pMonster);
		SkillOutput output;
		computeOutput(input, output);

		// 이팩트 클래스를 만들어 붙인다.
		EffectBloodyBurst* pEffect = new EffectBloodyBurst(pMonster);
		pEffect->setDeadline(output.Duration);

		pMonster->addEffect(pEffect);
		pMonster->setFlag(Effect::EFFECT_CLASS_BLOODY_BURST);

		GCSkillToSelfOK2	_GCSkillToSelfOK2;

		_GCSkillToSelfOK2.setObjectID(pMonster->getObjectID());
		_GCSkillToSelfOK2.setSkillType(getSkillType());

		pZone->broadcastPacket(pMonster->getX(), pMonster->getY(),  &_GCSkillToSelfOK2, pMonster);

		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID(pMonster->getObjectID());
		gcAddEffect.setEffectID(pEffect->getEffectClass());
		pZone->broadcastPacket(pMonster->getX(), pMonster->getY(), &gcAddEffect, pMonster);
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
}

BloodyBurst g_BloodyBurst;
