//////////////////////////////////////////////////////////////////////////////
// Filename    : HolyMace.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "HolyMace.h"
#include "EffectSimplePassive.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"


//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType HolyMace::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;

		SkillType_t       SkillType  = getSkillType();

		ZoneCoord_t myX = pSlayer->getX();
		ZoneCoord_t myY = pSlayer->getY();

		bool bManaCheck  = pSlayer->getAttackBloodBurstPoint() >= ATTACK_BLOOD_BURST_POINT_MAX;
		bool bEffected   = pSlayer->isFlag(Effect::EFFECT_CLASS_HOLY_MACE);

		if ( bManaCheck && !bEffected )
		{
			// Attack Blood Burst Point reset
			pSlayer->resetAttackBloodBurstPoint();

			// 이펙트 오브젝트를 생성해서 붙인다.
			EffectSimplePassive* pEffect = new EffectSimplePassive(pSlayer);
			pEffect->setEffectClass(Effect::EFFECT_CLASS_HOLY_MACE);
			pEffect->setDeadline(300);
			pSlayer->setFlag(Effect::EFFECT_CLASS_HOLY_MACE);
			pSlayer->addEffect(pEffect);
			
			// 패킷을 날린다.
			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(Effect::EFFECT_CLASS_HOLY_MACE);
			_GCSkillToSelfOK1.setDuration(300);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(300);

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			_GCSkillToSelfOK1.addShortData( MODIFY_ATTACK_BLOOD_BURST_POINT, pSlayer->getAttackBloodBurstPoint() );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(myX, myY, &_GCSkillToSelfOK2 , pSlayer);

			// 이펙트가 붙었다고 알려준다.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID( pSlayer->getObjectID() );
			gcAddEffect.setEffectID( Effect::EFFECT_CLASS_HOLY_MACE );
			gcAddEffect.setDuration( 300 );
			pZone->broadcastPacket( pSlayer->getX(), pSlayer->getY(), &gcAddEffect );
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
			
			return SKILL_RESULT_FAIL_UNKNOWN;
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	return SKILL_RESULT_FAIL_UNKNOWN;

	__END_CATCH
}

HolyMace g_HolyMace;

