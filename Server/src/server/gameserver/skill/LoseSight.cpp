//////////////////////////////////////////////////////////////////////////////
// Filename    : LoseSight.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "LoseSight.h"
#include "EffectSimplePassive.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCStatusCurrentHP.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 셀프 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType LoseSight::execute(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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

		SkillType_t SkillType = getSkillType();

		bool bManaCheck	= pSlayer->getDefenseBloodBurstPoint() >= DEFENSE_BLOOD_BURST_POINT_MAX;

		ZoneCoord_t X = pSlayer->getX();
		ZoneCoord_t Y = pSlayer->getY();

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

		if ( bManaCheck )
		{
			// reset Defense Blood Burst Point
			pSlayer->resetDefenseBloodBurstPoint();

			for( int ox = -2 ; ox <= 2 ; ox++ )
			for( int oy = -2 ; oy <= 2 ; oy++ )
			{
				int tileX = X+ox;
				int tileY = Y+oy;

				if( !rect.ptInRect(tileX,tileY) ) continue;

				Tile& tile = pZone->getTile( tileX, tileY );
				if ( tile.getEffect( Effect::EFFECT_CLASS_TRYING_POSITION ) ) continue;

				list<Creature*> targetList;
				if (tile.hasCreature(Creature::MOVE_MODE_WALKING))
				{
					Creature* pCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
					targetList.push_back(pCreature);
				}
				if (tile.hasCreature(Creature::MOVE_MODE_FLYING))
				{
					Creature* pCreature = tile.getCreature(Creature::MOVE_MODE_FLYING);
					targetList.push_back(pCreature);
				}
				if (tile.hasCreature(Creature::MOVE_MODE_BURROWING))
				{
					Creature* pCreature = tile.getCreature(Creature::MOVE_MODE_BURROWING);
					targetList.push_back(pCreature);
				}

				list<Creature*>::iterator itr = targetList.begin();
				for ( ; itr != targetList.end() ; itr++ )
				{
					Creature* pTargetCreature = (*itr);
					Assert( pTargetCreature != NULL );

					// 안 맞는넘 체크
					if ( 
						pTargetCreature->isNPC()
						|| pTargetCreature->isSlayer()
						|| !checkZoneLevelToHitTarget(pTargetCreature)
				  		|| !canAttack( pSlayer, pTargetCreature )
						|| pTargetCreature->isFlag( Effect::EFFECT_CLASS_COMA )
						|| pTargetCreature->getZoneID() == 1130
					)
						continue;

					// 이미 LOSE SIGHT 이펙트가 걸려있으면 먼저 걸려있던 이펙트를 삭제한다.
					if ( pTargetCreature->isFlag( Effect::EFFECT_CLASS_LOSE_SIGHT ) )
					{
						pTargetCreature->deleteEffect( Effect::EFFECT_CLASS_LOSE_SIGHT );
					}

					Turn_t duration;
					if ( pTargetCreature->isPC() )
						duration = 50;
					else
						duration = 100;

					// 이펙트를 생성해서 붙인다.
					EffectSimplePassive* pEffect = new EffectSimplePassive( pTargetCreature );
					pEffect->setEffectClass( Effect::EFFECT_CLASS_LOSE_SIGHT );
					pEffect->setDeadline(duration);
					pTargetCreature->setFlag( Effect::EFFECT_CLASS_LOSE_SIGHT );
					pTargetCreature->addEffect( pEffect );

					GCAddEffect gcAddEffect;
					gcAddEffect.setObjectID( pTargetCreature->getObjectID() );
					gcAddEffect.setEffectID( pEffect->getEffectClass() );
					gcAddEffect.setDuration( duration );
					pZone->broadcastPacket( tileX, tileY, &gcAddEffect );
				}
			}

			_GCSkillToSelfOK1.setSkillType(SkillType);
			_GCSkillToSelfOK1.setCEffectID(CEffectID);
			_GCSkillToSelfOK1.setDuration(0);
		
			_GCSkillToSelfOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToSelfOK2.setSkillType(SkillType);
			_GCSkillToSelfOK2.setDuration(0);
		
			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToSelfOK1 );

			_GCSkillToSelfOK1.addShortData( MODIFY_DEFENSE_BLOOD_BURST_POINT, pSlayer->getDefenseBloodBurstPoint() );

			pPlayer->sendPacket(&_GCSkillToSelfOK1);
			pZone->broadcastPacket(pSlayer->getX(), pSlayer->getY(),  &_GCSkillToSelfOK2, pSlayer);
			
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

LoseSight g_LoseSight;

