//////////////////////////////////////////////////////////////////////////////
// Filename    : EarthForce.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "EarthForce.h"
#include "EffectSimplePassive.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"


//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType EarthForce::execute(Ousters* pOusters, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pOusters != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = getSkillType();

		ZoneCoord_t myX = pOusters->getX();
		ZoneCoord_t myY = pOusters->getY();

		bool bManaCheck  = pOusters->getAttackBloodBurstPoint() >= ATTACK_BLOOD_BURST_POINT_MAX;
		bool bEffected   = pOusters->isFlag(Effect::EFFECT_CLASS_EARTH_FORCE);

		if ( bManaCheck && !bEffected )
		{
			// Attack Blood Burst Point reset
			pOusters->resetAttackBloodBurstPoint();

			// 이펙트 오브젝트를 생성해서 붙인다.
			EffectSimplePassive* pEffect = new EffectSimplePassive(pOusters);
			pEffect->setEffectClass(Effect::EFFECT_CLASS_EARTH_FORCE);
			pEffect->setDeadline(300);
			pOusters->setFlag(Effect::EFFECT_CLASS_EARTH_FORCE);
			pOusters->addEffect(pEffect);
			
			// 패킷을 날린다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(Effect::EFFECT_CLASS_EARTH_FORCE);
			_GCSkillToSelfOK1.setDuration(300);
		
			_GCSkillToSelfOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(300);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToSelfOK1 );

			_GCSkillToSelfOK1.addShortData( MODIFY_ATTACK_BLOOD_BURST_POINT, pOusters->getAttackBloodBurstPoint() );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2, pOusters);

			// 이펙트가 붙었다고 알려준다.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID( pOusters->getObjectID() );
			gcAddEffect.setEffectID( Effect::EFFECT_CLASS_EARTH_FORCE );
			gcAddEffect.setDuration( 300 );
			pZone->broadcastPacket( pOusters->getX(), pOusters->getY(), &gcAddEffect );
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

EarthForce g_EarthForce;

