//////////////////////////////////////////////////////////////////////////////
// Filename    : BLOODYSKULL.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "BLOODYSKULL.h"
#include "RankBonus.h"
#include "EffectBLOODYSKULL.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType BLOODYSKULL::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

    try
    {
		Zone* pZone = pVampire->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		if (pTargetCreature==NULL
			|| !canAttack( pVampire, pTargetCreature )
			|| pTargetCreature->isNPC())
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		execute(pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
    }

    __END_CATCH

    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType BLOODYSKULL::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int HitBonus = 0;

		int  RequiredMP  = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = verifyDistance(pVampire, X, Y, pSkillInfo->getRange());
		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, HitBonus);

		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToTileOK1);

			Tile&   tile  = pZone->getTile(X, Y);
			Range_t Range = 5;	// Ç×»ó 1ÀÌŽÙ.
			
			// °°Àº ÀÌÆåÆ®°¡ ÀÌ¹Ì ÁžÀçÇÑŽÙžé »èÁŠÇÑŽÙ.
			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_BLOODY_SKULL);
			if (pOldEffect != NULL)
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}

			checkMine( pZone, X, Y );
			
			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);

			if (pVampire->hasRankGem(NewRankGem::SKILL_BLOODY_SKULL_DAM ) )
			{
				NewRankGem* pRankGemSKULLD = pVampire->getRankGem( NewRankGem::SKILL_BLOODY_SKULL_DAM );
				Assert( pRankGemSKULLD != NULL );

				output.Damage += pRankGemSKULLD->getGrade();
			}

			if (pVampire->hasRankGem(NewRankGem::SKILL_BLOODY_SKULL_MCRI ) )
			{
				NewRankGem* pRankGemSKULLDMCRI = pVampire->getRankGem( NewRankGem::SKILL_BLOODY_SKULL_MCRI );
				Assert( pRankGemSKULLDMCRI != NULL );

				bool isCri = HitRoll::isMagicCriticalHit(pVampire, 200);
				if(isCri)
				output.Damage += output.Damage*0.5;
			}

			EffectBLOODYSKULL* pEffect = new EffectBLOODYSKULL(pZone, X, Y, true);
			pEffect->setUserObjectID( pVampire->getObjectID() );
			pEffect->setDamage(max(220,output.Damage));
			pEffect->setNextTime(output.Duration);

			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);
		
			pZone->addEffect(pEffect);	
			tile.addEffect(pEffect);

			bool bEffected = false;
			Creature* pTargetCreature = NULL;

			if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) 
				pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			ZoneCoord_t myX = pVampire->getX();
			ZoneCoord_t myY = pVampire->getY();

			if (pTargetCreature != NULL)
			{
				if (pTargetCreature->isSlayer()||pTargetCreature->isOusters())
				{


					bEffected = true;

					Player* pTargetPlayer = pTargetCreature->getPlayer();
					bool bCanSee = canSee(pTargetCreature, pVampire);

					if (bCanSee)
					{
						_GCSkillToTileOK2.setObjectID(pVampire->getObjectID());
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

					//pEffect->affect();
					bEffected = true;

					pMonster->addEnemy(pVampire);
				}
			}


			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);
		
			_GCSkillToTileOK3.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);
		
			_GCSkillToTileOK5.setObjectID(pVampire->getObjectID());
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

			// œºÅ³ ÈÄ Ã³ž®
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToTileOK1 );
		
			pPlayer->sendPacket(&_GCSkillToTileOK1);
		
			list<Creature*> cList;
			cList.push_back(pVampire);
			if (bEffected) cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

			pVampireSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;
		} 
		else 
		{
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
		//cout << t.toString() << endl;
	}

	__END_CATCH

	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// žóœºÅÍ ÅžÀÏ ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
void BLOODYSKULL::execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin(monster) " << endl;

	try 
	{
		Zone* pZone = pMonster->getZone();
		Assert(pZone != NULL);

		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = SKILL_BLOODY_SKULL;
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		bool bRangeCheck = verifyDistance(pMonster, X, Y, pSkillInfo->getRange());
		bool bHitRoll    = HitRoll::isSuccessMagic(pMonster, pSkillInfo);

		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bRangeCheck && bHitRoll && bTileCheck)
		{

			if( rand() % 100 < 90 ) {
				checkMine( pZone, X, Y );
			}

			Tile&   tile  = pZone->getTile(X, Y);
			Range_t Range = 5;	// Ç×»ó 1ÀÌŽÙ.
			
			// °°Àº ÀÌÆåÆ®°¡ ÀÌ¹Ì ÁžÀçÇÑŽÙžé »èÁŠÇÑŽÙ.
			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_BLOODY_SKULL);
			if (pOldEffect != NULL)
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}
			
			// µ¥¹ÌÁö¿Í ÁöŒÓ œÃ°£À» °è»êÇÑŽÙ.
			SkillInput input(pMonster);
			input.SkillLevel = pMonster->getLevel();
			SkillOutput output;
			computeOutput(input, output);

			// ÀÌÆåÆ® ¿ÀºêÁ§Æ®žŠ »ýŒºÇÑŽÙ.
			EffectBLOODYSKULL* pEffect = new EffectBLOODYSKULL(pZone, X, Y);
			pEffect->setNextTime(output.Duration);
			pEffect->setUserObjectID( pMonster->getObjectID() );
			//pEffect->setNextTime(0);
			//pEffect->setTick(output.Tick);
			pEffect->setDamage(output.Damage);
			//pEffect->setLevel(pSkillInfo->getLevel()/2);

			// ÅžÀÏ¿¡ ºÙÀº ÀÌÆåÆ®ŽÂ OIDžŠ ¹ÞŸÆŸß ÇÑŽÙ.
			ObjectRegistry & objectregister = pZone->getObjectRegistry();
			objectregister.registerObject(pEffect);
		
			// Áž ¹× ÅžÀÏ¿¡ŽÙ°¡ ÀÌÆåÆ®žŠ Ãß°¡ÇÑŽÙ.
			pZone->addEffect(pEffect);	
			tile.addEffect(pEffect);

			// ÅžÀÏ À§¿¡ Å©ž®ÃÄ°¡ ÀÖŽÙžé ¹Ù·Î ¿µÇâÀ» ÁÖµµ·Ï ÇÑŽÙ.
			bool bEffected = false;
			Creature* pTargetCreature = NULL;

			if (tile.hasCreature(Creature::MOVE_MODE_WALKING)) 
				pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			ZoneCoord_t myX = pMonster->getX();
			ZoneCoord_t myY = pMonster->getY();

			if (pTargetCreature != NULL)
			{
				if (pTargetCreature->isPC())
				{
					//pEffect->affect();

					Player* pTargetPlayer = pTargetCreature->getPlayer();
					bool bCanSee = canSee(pTargetCreature, pMonster);

					if (bCanSee)
					{
						_GCSkillToTileOK2.setObjectID(pMonster->getObjectID());
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
					Monster* pTargetMonster = dynamic_cast<Monster*>(pTargetCreature);
					//pEffect->affect();
					pTargetMonster->addEnemy(pMonster);
				}
			}

			_GCSkillToTileOK3.setObjectID(pMonster->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);
		
			_GCSkillToTileOK5.setObjectID(pMonster->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setRange(Range);

			if (bEffected) 
			{
				_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
				_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());
			}
		
			list<Creature*> cList;
			cList.push_back(pMonster);
			if (bEffected) cList.push_back(pTargetCreature);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

			pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
			pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);
		} 
		else 
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pMonster, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end(monster) " << endl;

	__END_CATCH
}

BLOODYSKULL g_BLOODYSKULL;
