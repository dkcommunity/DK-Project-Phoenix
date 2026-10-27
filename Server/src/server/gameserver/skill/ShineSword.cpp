//////////////////////////////////////////////////////////////////////////////
// Filename    : ShineSword.cpp
// Written by  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ShineSword.h"
#include "SimpleMeleeSkill.h"
#include "SimpleTileMissileSkill.h"
#include "EffectShineSword.h"
#include "ZoneUtil.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "SimpleMissileSkill.h"
#include <list>

SkillResultType ShineSword::execute(Slayer* pSlayer, ObjectID_t targetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID) throw(Error)
{
	SkillResultType skillResult;
	
	__BEGIN_TRY

	Zone* pZone = pSlayer->getZone();
	Assert(pZone != NULL);
	
	Creature* pTargetCreature = pZone->getCreature(targetObjectID);
	//Assert(pTargetCreature != NULL);

	// NoSuch제거. by sigi. 2002.5.2
	if (pTargetCreature==NULL)
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_TARGET;
	}

//	execute( pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID );
	SkillInput input(pSlayer, pSkillSlot);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT param;
	param.SkillType		= getSkillType();
	param.SkillDamage	= output.Damage;
	param.Delay			= output.Delay;
	param.ItemClass		= Item::ITEM_CLASS_SWORD;
	param.STRMultiplier = 8;
	param.DEXMultiplier = 1;
	param.INTMultiplier = 1;
	param.bMagicHitRoll = false;
	param.bMagicDamage	= false;
	param.bAdd			= true;
	param.bExpForTotalDamage = false;

	SIMPLE_SKILL_OUTPUT result;


	Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);
	// 무기가 없거나 착용 무기가 검이 아니면 이펙트를 주지 않는다.
	if( pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_SWORD ) 
	{
		executeSkillFailException(pSlayer, getSkillType());
		
		return SKILL_RESULT_FAIL_INVALID_ITEM;
	}
	Damage_t silverDamage = 0;

	// 은데미지 산출 공식
	silverDamage = min( 40, int(pSlayer->getSTR()/25 + pSlayer->getSTR()/40 + input.Range*1.5) );
	// 무기의 은의 포함량이 50 미만이면 발동 하지 않는다.
	//if( pWeapon->getSilver() < 50 )
	//{
	//	executeSkillFailException(pSlayer, getSkillType());
		
	//	return SKILL_RESULT_FAIL_NOT_ENOUGH_BULLET;
	//}
	
	skillResult = g_SimpleMissileSkill.execute( pSlayer, targetObjectID, pSkillSlot, param, result );

	if ( result.bSuccess )
	{
		//GCModifyInformation gcMI;
		//pWeapon->setSilver( pWeapon->getSilver() - 50 );
		//gcMI.addShortData( MODIFY_SILVER_DURABILITY, pWeapon->getSilver() );
		//pSlayer->getPlayer()->sendPacket(&gcMI);

		ZoneCoord_t tx = pTargetCreature->getX();
		ZoneCoord_t ty = pTargetCreature->getY();
		
		if( !isValidZoneCoord(pZone, tx, ty) ) 
		{
			return SKILL_RESULT_FAIL_IS_NOT_IN_RANGE;
		}
		
		Tile& rTile = pZone->getTile(tx, ty);
		if( !rTile.canAddEffect() ) 
		{
			return SKILL_RESULT_FAIL_CANNOT_OVERLAP_EFFECT;
		}

		EffectShineSword* pEffect = new EffectShineSword(pZone, tx, ty);
		pEffect->setDamage( silverDamage );
		pEffect->setDeadline( output.Duration );
		pEffect->setUserObjectID(pSlayer->getObjectID());
		pEffect->setTick(12);
		pZone->registerObject( pEffect );
		
		pEffect->setNextTime(12);

		GCAddEffectToTile gcAE;
		gcAE.setEffectID( pEffect->getSendEffectClass() );
		gcAE.setXY( tx, ty );
		gcAE.setObjectID( pEffect->getObjectID() );
		gcAE.setDuration( output.Duration );
		pZone->broadcastPacket( tx, ty, &gcAE );

		pZone->addEffect( pEffect );
		rTile.addEffect( pEffect );
	}

	__END_CATCH
	
	return skillResult;
}

ShineSword g_ShineSword;

