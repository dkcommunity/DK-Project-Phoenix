//////////////////////////////////////////////////////////////////////////////
// Filename    : DarkForce.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DarkForce.h"
#include "EffectSimplePassive.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"


//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType DarkForce::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = getSkillType();

		ZoneCoord_t myX = pVampire->getX();
		ZoneCoord_t myY = pVampire->getY();

		bool bManaCheck  = pVampire->getAttackBloodBurstPoint() >= ATTACK_BLOOD_BURST_POINT_MAX;
		bool bEffected   = pVampire->isFlag(Effect::EFFECT_CLASS_DARK_FORCE);

		if ( bManaCheck && !bEffected )
		{
			// Attack Blood Burst Point reset
			pVampire->resetAttackBloodBurstPoint();

			// 이펙트 오브젝트를 생성해서 붙인다.
			EffectSimplePassive* pEffect = new EffectSimplePassive(pVampire);
			pEffect->setEffectClass(Effect::EFFECT_CLASS_DARK_FORCE);
			pEffect->setDeadline(300);
			pVampire->setFlag(Effect::EFFECT_CLASS_DARK_FORCE);
			pVampire->addEffect(pEffect);
			
			// 패킷을 날린다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(Effect::EFFECT_CLASS_DARK_FORCE);
			_GCSkillToSelfOK1.setDuration(300);
		
			_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(300);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

			_GCSkillToSelfOK1.addShortData( MODIFY_ATTACK_BLOOD_BURST_POINT, pVampire->getAttackBloodBurstPoint() );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2, pVampire);

			// 이펙트가 붙었다고 알려준다.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID( pVampire->getObjectID() );
			gcAddEffect.setEffectID( Effect::EFFECT_CLASS_DARK_FORCE );
			gcAddEffect.setDuration( 300 );
			pZone->broadcastPacket( pVampire->getX(), pVampire->getY(), &gcAddEffect );
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

DarkForce g_DarkForce;

