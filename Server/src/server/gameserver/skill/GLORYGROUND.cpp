//////////////////////////////////////////////////////////////////////////////
// Filename    : GLORYGROUND.cpp
// Written by  : rallser
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "GLORYGROUND.h"
#include "EffectGLORYGROUND.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCDeleteEffectFromTile.h"
#include "NewRankGem.h"

//////////////////////////////////////////////////////////////////////////////
// 슬레이어 오브젝트 핸들러
//////////////////////////////////////////////////////////////////////////////
SkillResultType GLORYGROUND::execute( Slayer* pSlayer, ObjectID_t ObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID )
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	// Slayer Object Assertion
	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

    try
    {
		Zone* pZone = pSlayer->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(ObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch제거. by sigi. 2002.5.2
		if (pTargetCreature==NULL
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
        //cout << t.toString() << endl;
    }

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType GLORYGROUND::execute( Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID )
		throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayer)" << endl;

	Assert(pSlayer != NULL);
	Assert(pSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pSlayer->getPlayer();
		Zone* pZone = pSlayer->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToTileOK1 _GCSkillToTileOK1;
//		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
//		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t       SkillType  = pSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);
	//	SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pSlayer);
		bool bHitRoll    = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);
		bool bEffected   = pSlayer->isFlag(Effect::EFFECT_CLASS_REGENERATION);
		bool bTileCheck	 = false;

        VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);

			if (tile.canAddEffect()) bTileCheck = true;


			if( tile.getEffect(Effect::EFFECT_CLASS_SUMMON_CLAY) != NULL ) bTileCheck = false;
		}


		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && bTileCheck)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

			// 스킬 정보를 계산한다.
			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Tile& tile = pZone->getTile( X, Y );

			Range_t Range = 3;

                        int oX, oY;
			
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
				
			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_REGENERATION);
			if( pOldEffect != NULL )
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect( effectID );
			}

			// Holy Cross 이펙트가 걸려있으면
			if ( pSlayer->isFlag( Effect::EFFECT_CLASS_HOLY_CROSS ) )
			{
				// Regeneration 힐량 두배
				output.Damage = (Damage_t)(output.Damage * 1.5);
			}
			if (pSlayer->hasRankGem(NewRankGem::SKILL_GLORY_GROUND_DAM ) )
			{
				NewRankGem* pRankGemGLORYD = pSlayer->getRankGem( NewRankGem::SKILL_GLORY_GROUND_DAM );
				Assert( pRankGemGLORYD != NULL );

				output.Damage += pRankGemGLORYD->getGrade();
			}
			if (pSlayer->hasRankGem(NewRankGem::SKILL_GLORY_GROUND_DUR ) )
			{
				NewRankGem* pRankGemGENESISDUR = pSlayer->getRankGem( NewRankGem::SKILL_GLORY_GROUND_DUR );
				Assert( pRankGemGENESISDUR != NULL );

				output.Duration += pRankGemGENESISDUR->getGrade();
			}
			// 이팩트 클래스를 만들어 붙인다.
			EffectGLORYGROUND* pEffect = new EffectGLORYGROUND(pZone, X, Y);
			pEffect->setDeadline(output.Duration);
			pEffect->setDamage( output.Damage );
			pEffect->setDelay( output.Tick );
			pEffect->setNextTime( 0 );

			ObjectRegistry& objectRegister = pZone->getObjectRegistry();
			objectRegister.registerObject(pEffect);

			pZone->addEffect(pEffect);
			tile.addEffect(pEffect);


			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);

			_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setRange(Range);


			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);

			list<Creature*> cList;
			cList.push_back( pSlayer );

			cList = pZone->broadcastSkillPacket( X, Y, X, Y, &_GCSkillToTileOK5, cList );


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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayer)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}


GLORYGROUND g_GLORYGROUND;
