//////////////////////////////////////////////////////////////////////////////
// Filename    : HeavenGround.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "HeavenGround.h"
#include "EffectHeavenGround.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCDeleteEffectFromTile.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCModifyInformation.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType HeavenGround::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try
	{
		Zone* pZone = pSlayer->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch제거. by sigi. 2002.5.2
		if ( pTargetCreature==NULL )
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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 타일 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType HeavenGround::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

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
		bool bRangeCheck = verifyDistance(pSlayer, X, Y, pSkillInfo->getRange()) && checkZoneLevelToUseSkill(pSlayer);
		bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);

		//090305 wlzzi 무기착용 조건 검사
		bool bWeapon	 = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND)==NULL? false:true;
		
		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y)) bTileCheck = true;

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck && bWeapon)	//무기착용 조건 추가
		{
			// 이펙트의 지속시간을 계산한다.
			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Range_t     Range    = 3;
			Damage_t	skillDamage = output.Damage;

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

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_PROMINENCE);

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

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_HELLFIRE);

						if( pEffect != NULL )
						{
							effectList.push_back(pEffect);
							pEffect = NULL;
						}

						pEffect = tile.getEffect(Effect::EFFECT_CLASS_GROUND_ELEMENTAL_AURA);

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

						if( pEffect!= NULL )
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
						
						pEffect = tile.getEffect(Effect::EFFECT_CLASS_YELLOW_POISON);

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
						
						pEffect = tile.getEffect(Effect::EFFECT_CLASS_FURY_OF_GNOME);

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
						executeSkillFailNormal(pSlayer, getSkillType(), NULL);
						
						return SKILL_RESULT_FAIL_CANNOT_OVERLAP_EFFECT;
					}
				}
			}

			// 실패하면 마나가 줄면 안 되므로 여기서 줄여준다.
			decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

			Item* pWeapon = pSlayer->getWearItem(Slayer::WEAR_RIGHTHAND);

			if( pWeapon == NULL ) 
			{
				executeSkillFailNormal(pSlayer,getSkillType(), NULL);
				
				return SKILL_RESULT_FAIL_INVALID_ITEM;
			}

		/*	if( pWeapon->getSilver() < 40 )
			{
				executeSkillFailNormal(pSlayer,getSkillType(), NULL);
				
				return SKILL_RESULT_FAIL_NOT_ENOUGH_BULLET;
			}
			else
			{
				GCModifyInformation gcMI;
				pWeapon->setSilver(pWeapon->getSilver() - 40 );
				gcMI.addShortData( MODIFY_SILVER_DURABILITY, pWeapon->getSilver() );
				pSlayer->getPlayer()->sendPacket(&gcMI);
			}*/

			for(oY = -2; oY <= 2; oY++)
			for(oX = -2; oX <= 2; oX++)
			{
				int tileX = X+oX;
				int tileY = Y+oY;
				if (rect.ptInRect(tileX, tileY))
				{
					Tile& tile = pZone->getTile(tileX, tileY);
					Creature* pTargetCreature = NULL;
					GCModifyInformation gcMI;

					if( tile.hasCreature(Creature::MOVE_MODE_WALKING) )
					{
						pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);
					}

					if( pTargetCreature != NULL &&
						!pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA) &&
						!pTargetCreature->isDead() &&
						!pTargetCreature->isNPC() &&
						canAttack( pSlayer, pTargetCreature ) &&
						HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot) &&
						checkZoneLevelToHitTarget(pTargetCreature)
					  )
					{
						if( pTargetCreature->isVampire() )
						{
							Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
							::setDamage( pVampire, skillDamage, pSlayer, SKILL_HEAVEN_GROUND, &gcMI);
							pVampire->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isOusters() )
						{
							Ousters* pOusters = dynamic_cast<Ousters*>(pTargetCreature);
							::setDamage( pOusters, skillDamage, pSlayer, SKILL_HEAVEN_GROUND, &gcMI);
							pOusters->getPlayer()->sendPacket(&gcMI);
						}
						else if( pTargetCreature->isMonster() )
						{
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
							::setDamage( pMonster, skillDamage, pSlayer, SKILL_HEAVEN_GROUND, &gcMI);
						}

						if( pTargetCreature->isPC() && !pTargetCreature->isSlayer())
						{
							GCSkillToObjectOK2 gcSkillToObjectOK2;
							gcSkillToObjectOK2.setObjectID(1);
							gcSkillToObjectOK2.setSkillType(SKILL_ATTACK_MELEE);
							gcSkillToObjectOK2.setDuration(0);
							
							pTargetCreature->getPlayer()->sendPacket(&gcSkillToObjectOK2);
						}
					}

					// 현재 타일에다 이펙트를 추가할 수 있다면...
					//if (tile.canAddEffect())	// 위에서 체크했다.
					{
				
						Damage_t effectDamage = Damage_t(skillDamage*0.1);
						// 이펙트 클래스를 생성한다.
						EffectHeavenGround* pEffect = new EffectHeavenGround(pZone , tileX, tileY);
						pEffect->setUserObjectID( pSlayer->getObjectID() );
						pEffect->setDeadline(output.Duration);
						pEffect->setTick(12);
						pEffect->setDamage(effectDamage);
						pEffect->setNextTime(12);

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

							pZone->broadcastPacket(X, Y, &gcAddEffectToTile, pSlayer);

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

			// EXP UP!
			SkillDomainType_t DomainType = pSkillInfo->getDomainType();
			SkillGrade Grade = g_pSkillInfoManager->getGradeByDomainLevel( pSlayer->getSkillDomainLevel(DomainType));
			Exp_t ExpUp = 10* (Grade + 1);

			shareAttrExp(pSlayer, ExpUp, 1, 1, 8, _GCSkillToTileOK1);
			increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToTileOK1);
			increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToTileOK1);

			for(list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++)
			{
				Creature* pTargetCreature = *itr;
				if (canSee(pTargetCreature, pSlayer)) pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK2);
				else pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK6);
			}

			// 스킬 후 처리
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);

			cList.push_back(pSlayer);

			list<Creature*> watcherList = pZone->getWatcherList(myX, myY, pSlayer);

			// watcherList에서 cList에 속하지 않고, caster(pSlayer)를 볼 수 없는 경우는
			// OK4를 보내고.. cList에 추가한다.
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
					
			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList, false);

						
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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

HeavenGround g_HeavenGround;
