//////////////////////////////////////////////////////////////////////////////
// Filename    : SharpHail.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "ADV3FROZENGROUND2.h"
#include "EffectADV3FROZENGROUND2.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffectToTile.h"

//////////////////////////////////////////////////////////////////////////////
// ŸÆ¿ìœºÅÍÁî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType FROZENGROUND2::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY


	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

    try
    {
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);


		if (pTargetCreature==NULL
			|| !canAttack( pOusters, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pOusters, getSkillType(), 0 );
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pOusters, pTargetCreature->getX(), pTargetCreature->getY(), pOustersSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), 0 );
    }

    __END_CATCH
    
    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// ŸÆ¿ìœºÅÍÁî ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType FROZENGROUND2::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pOustersSkillSlot->getSkillType(), 0 );
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pOustersSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		SkillInput input(pOusters, pOustersSkillSlot);
		SkillOutput output;
		computeOutput(input, output);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pOustersSkillSlot->getExpLevel()/3;
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, X, Y, pSkillInfo->getRange());
		bool bSatisfyRequire = pOusters->satisfySkillRequire( pSkillInfo );

		
		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bTileCheck && bSatisfyRequire)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToTileOK1);

			int oX, oY;
			int diff =2;

			for ( oX = X - diff ; oX <= X + diff ; ++oX )
			for ( oY = Y - diff ; oY <= Y + diff ; ++oY )
			{
				if (!rect.ptInRect(oX, oY)) continue;

				Tile&   tile  = pZone->getTile(oX, oY);
				if (!tile.canAddEffect()) continue;
		
				Creature* pTargetCreature = NULL;

	            if (tile.hasCreature(Creature::MOVE_MODE_WALKING))
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
				   
				Damage_t Damage = output.Damage;


				bool bCriticalHit = false;
				if( pTargetCreature )
					Damage += computeDamage(pOusters, pTargetCreature, 0, bCriticalHit);		
				
				// ÀÌÆåÆ® ¿ÀºêÁ§Æ®žŠ »ýŒºÇÑŽÙ.
				EffectFROZENGROUND2* pEffect = new EffectFROZENGROUND2(pZone, oX, oY);
				pEffect->setUserObjectID( pOusters->getObjectID() );
				pEffect->setDeadline(output.Duration);
				pEffect->setNextTime(10);
				pEffect->setTick(output.Tick);
				pEffect->setDamage(Damage);
				pEffect->setBroadcastingEffect(false);
				
				pEffect->setLevel(pOustersSkillSlot->getExpLevel());

				/*if ( Grade > 0 )
				{
					if ( Grade == 1 ) pEffect->setSendEffectClass( Effect::EFFECT_CLASS_SHARP_HAIL_2 );
					else pEffect->setSendEffectClass( Effect::EFFECT_CLASS_SHARP_HAIL_3 );
				}*/

				// ÅžÀÏ¿¡ ºÙÀº ÀÌÆåÆ®ŽÂ OIDžŠ ¹ÞŸÆŸß ÇÑŽÙ.
				ObjectRegistry & objectregister = pZone->getObjectRegistry();
				objectregister.registerObject(pEffect);
			
				// Áž ¹× ÅžÀÏ¿¡ŽÙ°¡ ÀÌÆåÆ®žŠ Ãß°¡ÇÑŽÙ.
				pZone->addEffect(pEffect);	
				tile.addEffect(pEffect);

			}

			ZoneCoord_t myX = pOusters->getX();
			ZoneCoord_t myY = pOusters->getY();

			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
//			_GCSkillToTileOK1.setGrade(Grade);
		
			_GCSkillToTileOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
//			_GCSkillToTileOK3.setGrade(Grade);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
//			_GCSkillToTileOK4.setGrade(Grade);
		
			_GCSkillToTileOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
//			_GCSkillToTileOK5.setGrade(Grade);

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			list<Creature*> cList;
			cList.push_back(pOusters);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL, 0 );
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType(), 0 );
		//cout << t.toString() << endl;
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

FROZENGROUND2 g_FROZENGROUND2;
