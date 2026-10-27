//////////////////////////////////////////////////////////////////////////////
// Filename    : ADV2VIACRUCIS2.cpp
// Written by  :rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV2VIACRUCIS2.h"
#include "RankBonus.h"
#include "EffectADV2VIACRUCIS2.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "HitRoll.h"
#include "NewRankGem.h"

SkillResultType ADV2VIACRUCIS2::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

    try
    {
		Zone* pZone = pSlayer->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		if (pTargetCreature==NULL
			|| !canAttack( pSlayer, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pSlayer, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
    }

    __END_CATCH

    return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType ADV2VIACRUCIS2::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int HitBonus = 0;
		int CriBonus = 0;
	
		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
	//	bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
	       
		bool bTileCheck = false;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bManaCheck && bTimeCheck && bTileCheck)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Damage_t	skillDamage = output.Damage;

			if (pSlayer->hasRankGem(NewRankGem::SKILL_ADV2_VIA_CRUCIS_DAM ) )
			{
				NewRankGem* pRankGemVIAD = pSlayer->getRankGem( NewRankGem::SKILL_ADV2_VIA_CRUCIS_DAM );
				Assert( pRankGemVIAD != NULL );

				skillDamage += pRankGemVIAD->getGrade();
			}

			if (pSlayer->hasRankGem(NewRankGem::SKILL_ADV2_VIA_CRUCIS_MCRI ) )
			{
				NewRankGem* pRankGemVIAMCRI = pSlayer->getRankGem( NewRankGem::SKILL_ADV2_VIA_CRUCIS_MCRI );
				Assert( pRankGemVIAMCRI != NULL );

				bool isCri = HitRoll::isMagicCriticalHit(pSlayer, 200);
				if(isCri)
				skillDamage += skillDamage*0.5;
			}


			int oX, oY;
			Tile&   tile  = pZone->getTile(X, Y);
			Range_t Range = 5;


			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_ADV2VIACRUCIS2);
			if (pOldEffect != NULL)
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}

			Damage_t effectDamage = Damage_t(skillDamage*0.4);
			EffectADV2VIACRUCIS2* pEffect = new EffectADV2VIACRUCIS2(pZone, X, Y, true);
			pEffect->setUserObjectID( pSlayer->getObjectID() );
			pEffect->setDamage(effectDamage);
			pEffect->setNextTime(3);
			pEffect->setTick(3);
			pEffect->setDeadline( output.Duration );

			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);

			pZone->addEffect(pEffect);	
			tile.addEffect(pEffect);

			bool bEffected = false;
			Creature* pTargetCreature = NULL;

			if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) 
				pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			ZoneCoord_t myX = pSlayer->getX();
			ZoneCoord_t myY = pSlayer->getY();

			if (pTargetCreature != NULL)
			{
				if (pTargetCreature->isVampire()||pTargetCreature->isOusters())
				{
					bEffected = true;

					Player* pTargetPlayer = pTargetCreature->getPlayer();
					bool bCanSee = canSee(pTargetCreature, pSlayer);

					if (bCanSee)
					{
						_GCSkillToTileOK2.setObjectID(pSlayer->getObjectID());
						_GCSkillToTileOK2.setSkillType(SkillType);
						_GCSkillToTileOK2.setX(X);
						_GCSkillToTileOK2.setY(Y);
						_GCSkillToTileOK2.setDuration(output.Duration);
						_GCSkillToTileOK2.setRange(Range);
						pTargetPlayer->sendPacket(&_GCSkillToTileOK2);
					} 
					else 
					{
						_GCSkillToTileOK6.setOrgXY(myX, myY);
						_GCSkillToTileOK6.setSkillType(SkillType);
						_GCSkillToTileOK6.setX(X);
						_GCSkillToTileOK6.setY(Y);
						_GCSkillToTileOK6.setDuration(output.Duration);
						_GCSkillToTileOK6.setRange(Range);
						pTargetPlayer->sendPacket(&_GCSkillToTileOK6);
					}
				}
				else if (pTargetCreature->isMonster())
				{
					Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

					bEffected = true;

					pMonster->addEnemy(pSlayer);
				}
			}


			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);
		
			_GCSkillToTileOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);
		
			_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setRange(Range);

			if (bEffected) 
			{
				_GCSkillToTileOK1.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());
			}

			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToTileOK1 );
		
			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			list<Creature*> cList;
			cList.push_back(pSlayer);
			if (bEffected) cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pSlayer, getSkillType());
	}

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;
}


ADV2VIACRUCIS2 g_ADV2VIACRUCIS2;
