//////////////////////////////////////////////////////////////////////////////
// Filename    : SummonClay.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SummonClay.h"
#include "EffectSummonClay.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCDeleteEffectFromTile.h"

SkillResultType SummonClay::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pOusters != NULL);
	Assert(pSkillSlot != NULL);

	try
	{
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch제거. by sigi. 2002.5.2
		if ( pTargetCreature==NULL )
		{
			executeSkillFailException(pOusters, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pOusters, pTargetCreature->getX(), pTargetCreature->getY(), pSkillSlot, CEffectID);
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// 타일 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType SummonClay::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pOusters != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);
		
		Inventory *pInventory = pOusters->getInventory();

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t myX = pOusters->getX();
		ZoneCoord_t myY = pOusters->getY();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, X, Y, pSkillInfo->getRange()) && checkZoneLevelToUseSkill(pOusters);
		bool bHitRoll    = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pSkillSlot);
		//bool bSummonItem = pInventory->hasEnoughNumItem( Item::ITEM_CLASS_PUPA, 5, 3);

		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y)) bTileCheck = true;

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck) //&& bSummonItem)
		{
			//pInventory->decreaseNumItem( Item::ITEM_CLASS_PUPA, 5, 3, pPlayer );
			
			// 이펙트의 지속시간을 계산한다.
			SkillInput input(pOusters, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Range_t    Range    = 3;

			int oX, oY;
			
			list<Creature*> cList;
			
			for(oY = -2; oY <= 2; oY++)
			for(oX = -2; oX <= 2; oX++)
			{
				int tileX = X+oX;
				int tileY = Y+oY;
				if (rect.ptInRect(tileX, tileY))
				{
					Tile& tile = pZone->getTile(tileX, tileY);
					GCDeleteEffectFromTile	gcDeleteEffectFromTile;

					if (tile.canAddEffect())
					{
						list<Effect*> effectList;
						Effect* pEffect = NULL;

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_DARKNESS);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_REGENERATION);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_ACID_SWAMP);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_MERCY_GROUND);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_SWORD_OF_THOR);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_BLOODY_WALL);

						if( pEffect != NULL)
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_GRAY_DARKNESS);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_YELLOW_POISON);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_GREEN_POISON);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_ICE_FIELD);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_SANCTUARY);
						
						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_INSTALL_TURRET);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						list<Effect*>::iterator itr = effectList.begin();
						list<Effect*>::iterator endItr = effectList.end();

						for(; itr != endItr; ++itr)
						{
							Effect* pDeleteEffect = *itr;
							if( pDeleteEffect != NULL )
							{
								pDeleteEffect->setDeadline(0);

								gcDeleteEffectFromTile.setXY(tileX, tileY);
								gcDeleteEffectFromTile.setObjectID(pDeleteEffect->getObjectID());
								gcDeleteEffectFromTile.setEffectID(pDeleteEffect->getEffectClass());
								pZone->broadcastPacket(tileX, tileY, &gcDeleteEffectFromTile);
							}
						}
					}
					else
					{
						executeSkillFailNormal(pOusters, getSkillType(), NULL);
						
						return SKILL_RESULT_FAIL_CANNOT_OVERLAP_EFFECT;
					}
				}
			}

			// 실패하면 마나가 줄면 안 되므로 여기서 줄여준다.
			decreaseMana(pOusters, RequiredMP, _GCSkillToTileOK1);

			Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);

			if( pWeapon == NULL ) 
			{
				return SKILL_RESULT_FAIL_INVALID_ITEM;
			}

			for(oY = -2; oY <= 2; oY++)
			for(oX = -2; oX <= 2; oX++)
			{
				int tileX = X+oX;
				int tileY = Y+oY;
				if (rect.ptInRect(tileX, tileY))
				{
					Tile& tile = pZone->getTile(tileX, tileY);

					// 현재 타일에다 이펙트를 추가할 수 있다면...
					//if (tile.canAddEffect())	// 위에서 체크했다.
					{
				
						// 이펙트 클래스를 생성한다.
						EffectSummonClay* pEffect = new EffectSummonClay(pZone , tileX, tileY);
						pEffect->setUserObjectID( pOusters->getObjectID() );
						pEffect->setDeadline(output.Duration);

						// Tile에 붙이는 Effect는 ObjectID를 등록받아야 한다.
						pZone->registerObject(pEffect);
						pZone->addEffect(pEffect);
						tile.addEffect(pEffect);

						if (oX==0 && oY==0)
						{
							GCAddEffectToTile gcAddEffectToTile;
							gcAddEffectToTile.setEffectID(pEffect->getEffectClass());
							gcAddEffectToTile.setObjectID(pEffect->getObjectID());
							gcAddEffectToTile.setXY(X, Y);
							gcAddEffectToTile.setDuration(output.Duration);

							pZone->broadcastPacket(X, Y, &gcAddEffectToTile);

							pEffect->setBroadcastingEffect(true);
						}
						else
						{
							pEffect->setBroadcastingEffect(false);
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

			_GCSkillToTileOK2.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK2.setSkillType(SkillType);
			_GCSkillToTileOK2.setX(X);
			_GCSkillToTileOK2.setY(Y);
			_GCSkillToTileOK2.setDuration(output.Duration);
			_GCSkillToTileOK2.setRange(Range);
			//_GCSkillToTileOK2.addShortData(MODIFY_VISION, SANCTUARY_SIGHT);

			_GCSkillToTileOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);

			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setRange(Range);
			_GCSkillToTileOK4.setDuration(output.Duration);

			_GCSkillToTileOK5.setObjectID(pOusters->getObjectID());
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
			//_GCSkillToTileOK6.addShortData(MODIFY_VISION, SANCTUARY_SIGHT);

			for(list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++)
			{
				Creature* pTargetCreature = *itr;
				if (canSee(pTargetCreature, pOusters)) pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK2);
				else pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK6);
			}

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);

			cList.push_back(pOusters);

			list<Creature*> watcherList = pZone->getWatcherList(myX, myY, pOusters);

			// watcherList에서 cList에 속하지 않고, caster(pOusters)를 볼 수 없는 경우는
			// OK4를 보내고.. cList에 추가한다.
			for(list<Creature*>::const_iterator itr = watcherList.begin(); itr != watcherList.end(); itr++)
			{
				bool bBelong = false;
				for(list<Creature*>::const_iterator tItr = cList.begin(); tItr != cList.end(); tItr++)
					if (*itr == *tItr)
						bBelong = true;

				Creature* pWatcher = (*itr);
				if (bBelong == false && canSee(pWatcher, pOusters) == false)
				{
					if (!pWatcher->isPC())
					{
						executeSkillFailException(pOusters, getSkillType());

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
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SummonClay g_SummonClay;
