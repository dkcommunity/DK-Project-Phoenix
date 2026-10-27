//////////////////////////////////////////////////////////////////////////////
// Filename    : Darkness.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "PULSEMAGNETICFIELD.h"
#include "EffectPULSEMAGNETICFIELD.h"
#include "RankBonus.h"

#include "MonsterCorpse.h"
#include "ctf/FlagManager.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCAddEffect.h"

SkillResultType PULSEMAGNETICFIELD::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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

		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}
		
		if( pZone->getZoneID() == 8001 ||
			pZone->getZoneID() == 1410 ||	//by viva
			pZone->getZoneID() == 1411 ||	//by viva
			pZone->getZoneID() == 1412 ||	//by viva
			pZone->getZoneID() == 1413 )	//by viva
		{
			executeSkillFailException(pSlayer, getSkillType());
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
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

//////////////////////////////////////////////////////////////////////////////
// ¹ìŸî ž µé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType PULSEMAGNETICFIELD::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

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

		ZoneCoord_t myX = pSlayer->getX();
		ZoneCoord_t myY = pSlayer->getY();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);

		bool bSlayerSafeZone = pZone->getZoneLevel( X, Y ) & SLAYER_SAFE_ZONE;
		bool bCompleteSafeZone = pZone->getZoneLevel( X, Y ) & COMPLETE_SAFE_ZONE;
		
		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y)) bTileCheck = true;

		bool bZoneCheck = true;
		if( pZone->getZoneID() == 8001 ||
			pZone->getZoneID() == 1410 ||	//by viva
			pZone->getZoneID() == 1411 ||	//by viva
			pZone->getZoneID() == 1412 ||	//by viva
			pZone->getZoneID() == 1413 )	//by viva
		{
			bZoneCheck = false;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bTileCheck && !bSlayerSafeZone && bZoneCheck && !bCompleteSafeZone)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);


			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);


			Range_t    Range    = 6;

			int oX, oY;
			
			list<Creature*> cList;

			for ( oX = X - 2 ; oX <= X + 2 ; ++oX )
			for ( oY = Y - 2 ; oY <= Y + 2 ; ++oY )
			{


				// oX/oY zaten hedef koordinatin etrafindaki gercek tile koordinatlaridir.
				int tileX = oX;
				int tileY = oY;
				if (rect.ptInRect(tileX, tileY))
				{
					Tile& tile = pZone->getTile(tileX, tileY);

					if ( tile.hasItem() )
					{
						Item* pItem = tile.getItem();
						if ( pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_CORPSE && pItem->getItemType() == MONSTER_CORPSE )
						{
							MonsterCorpse* pMonsterCorpse = dynamic_cast<MonsterCorpse*>(pItem);
							if ( g_pFlagManager->isFlagPole( pMonsterCorpse ) )
							{
								continue;
							}
						}
					}

					if ( tile.getEffect( Effect::EFFECT_CLASS_TRYING_POSITION ) != NULL ) continue;

					if (tile.canAddEffect())
					{
						if ( tile.getEffect(Effect::EFFECT_CLASS_MERCY_GROUND) != NULL ) continue;
						if ( tile.getEffect(Effect::EFFECT_CLASS_DARKNESS_FORBIDDEN) != NULL ) continue;
						if ( tile.getEffect(Effect::EFFECT_CLASS_HEAVEN_GROUND) != NULL ) continue;
						if ( tile.getEffect(Effect::EFFECT_CLASS_GLORYGROUND) != NULL ) continue;
						if ( tile.getEffect(Effect::EFFECT_CLASS_SUMMON_CLAY) != NULL ) continue;
						if ( tile.getEffect(Effect::EFFECT_CLASS_DARKNESS) != NULL ) continue;

						EffectPULSEMAGNETICFIELD* pEffect = new EffectPULSEMAGNETICFIELD(pZone, tileX, tileY);
						pEffect->setDeadline(output.Duration);
						pEffect->setDuration(output.Duration);
						pEffect->setStartTime();

						ObjectRegistry & objectregister = pZone->getObjectRegistry();
						objectregister.registerObject(pEffect);
						pZone->addEffect(pEffect);	
						tile.addEffect(pEffect);


						const list<Object*>& oList = tile.getObjectList();
						for(list<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++) 
						{
							Object* pTarget = *itr;
							Creature* pTargetCreature = NULL;
							if (pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE 
								&& ( (pTargetCreature = dynamic_cast<Creature*>(pTarget))->isVampire() || pTargetCreature->isOusters() )
							) 
							{
								cList.push_back(pTargetCreature);
								_GCSkillToTileOK2.addCListElement(pTargetCreature->getObjectID());
								_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
								_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());
							}

							pEffect->affectObject(pTarget, false);
						}
					}	
				}	
			}

			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);

			_GCSkillToTileOK2.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK2.setSkillType(SkillType);
			_GCSkillToTileOK2.setX(X);
			_GCSkillToTileOK2.setY(Y);
			_GCSkillToTileOK2.setDuration(output.Duration);
			_GCSkillToTileOK2.setRange(Range);

			_GCSkillToTileOK3.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);

			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setRange(Range);
			_GCSkillToTileOK4.setDuration(output.Duration);

			_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setRange(Range);
			_GCSkillToTileOK5.setDuration(output.Duration);

			_GCSkillToTileOK6.setOrgXY(myX, myY);
			_GCSkillToTileOK6.setSkillType(SkillType);
			_GCSkillToTileOK6.setX(X);
			_GCSkillToTileOK6.setY(Y);
			_GCSkillToTileOK6.setDuration(output.Duration);
			_GCSkillToTileOK6.setRange(Range);

			for(list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++)
			{
				Creature* pTargetCreature = *itr;
				if (canSee(pTargetCreature, pSlayer)) pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK2);
				else pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK6);
			}

			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			cList.push_back(pSlayer);

			list<Creature*> watcherList = pZone->getWatcherList(myX, myY, pSlayer);

			for(list<Creature*>::const_iterator itr = watcherList.begin(); itr != watcherList.end(); itr++)
			{
				bool bBelong = false;
				for(list<Creature*>::const_iterator tItr = cList.begin(); tItr != cList.end(); tItr++)
					if (*itr == *tItr)
						bBelong = true;

				Creature* pWatcher = (*itr);
				if (bBelong == false && canSee(pWatcher, pSlayer) == false)
				{
					if (!pWatcher->isPC())
					{
						executeSkillFailException(pSlayer, getSkillType());

						return SKILL_RESULT_FAIL_INVALID_TARGET;
					}
					pWatcher->getPlayer()->sendPacket(&_GCSkillToTileOK4);
					cList.push_back(*itr);
				}
			}

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

PULSEMAGNETICFIELD g_PULSEMAGNETICFIELD;

