//////////////////////////////////////////////////////////////////////////////
// Filename    : IceHorizon.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "DEMONICRESURRECTIO.h"
#include "EffectDEMONICRESURRECTIO.h"
#include "Creature.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"

DEMONICRESURRECTIO::DEMONICRESURRECTIO()
	throw()
{

};

SkillResultType DEMONICRESURRECTIO::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try
	{
		Zone* pZone = pVampire->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch°. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			|| !canAttack( pVampire, pTargetCreature )
			)
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		return execute(pVampire, pTargetCreature->getX(), pTargetCreature->getY(), pVampireSkillSlot, CEffectID);
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// ¹ìŸî ž µé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType DEMONICRESURRECTIO::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

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

		ZoneCoord_t myX = pVampire->getX();
		ZoneCoord_t myY = pVampire->getY();

		SkillInput input(pVampire);
		SkillOutput output;
		computeOutput(input, output);

		if (pVampire->hasRankGem(NewRankGem::SKILL_DEMONIC_RESURRECTIO_DAM ) )
		{
			NewRankGem* pRankGemDEMONICD = pVampire->getRankGem( NewRankGem::SKILL_DEMONIC_RESURRECTIO_DAM );
			Assert( pRankGemDEMONICD != NULL );

			output.Damage += pRankGemDEMONICD->getGrade();
		}

		if (pVampire->hasRankGem(NewRankGem::SKILL_DEMONIC_RESURRECTIO_DUR ) )
		{
			NewRankGem* pRankGemDEMONICDUR = pVampire->getRankGem( NewRankGem::SKILL_DEMONIC_RESURRECTIO_DUR );
			Assert( pRankGemDEMONICDUR != NULL );

			output.Duration += pRankGemDEMONICDUR->getGrade();
		}

		int  RequiredMP  = decreaseConsumeMP(pVampire, pSkillInfo);
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = verifyDistance(pVampire, X, Y, pSkillInfo->getRange());
		
		bool bTileCheck = false;
		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);
		if (rect.ptInRect(X, Y)) bTileCheck = true;

		if (bManaCheck && bTimeCheck && bRangeCheck && bTileCheck)
		{
			decreaseMana(pVampire, RequiredMP, _GCSkillToTileOK1);


			list<Creature*> cList;	// denier list
			
			int diff =2;

			if (pVampire->hasRankGem(NewRankGem::SKILL_DEMONIC_RESURRECTIO_RANGE ) )
			{
				NewRankGem* pRankGemDEMONICRANGE = pVampire->getRankGem( NewRankGem::SKILL_DEMONIC_RESURRECTIO_RANGE );
				Assert( pRankGemDEMONICRANGE != NULL );

				diff=3;
			}

			for (int i=-diff; i <= diff; i++)
			for (int j=-diff; j <= diff; j++)
			{
				int tileX = X+i;
				int tileY = Y+j;

				if (rect.ptInRect(tileX, tileY))
				{
					Tile& tile = pZone->getTile(tileX, tileY);
					if ( tile.getEffect(Effect::EFFECT_CLASS_TRYING_POSITION) ) continue;

					// öç ž¿¡Ž å®ž °¡ ö Žžé...
					if (tile.canAddEffect())
					{
						// °°º effect°¡ žžé ö¿îŽ.
						Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_DEMONICRESURRECTIO);
						if (pOldEffect != NULL)
						{
							ObjectID_t effectID = pOldEffect->getObjectID();
							pZone->deleteEffect(effectID);// fix me
						}

						// å® ¬·¡ºž »ýºŽ.
						EffectDEMONICRESURRECTIO* pEffect = new EffectDEMONICRESURRECTIO(pZone , tileX, tileY);
						pEffect->setCasterName( pVampire->getName() );
						pEffect->setCasterID( pVampire->getObjectID() );
						pEffect->setDeadline(output.Duration);
						pEffect->setIncreaseAmount(output.Damage);	
						pEffect->setNextTime(0);
						pEffect->setTick(output.Tick);

						if ( i != 0 || j !=  0 ) pEffect->setBroadcastingEffect(false);

						// Tile¿¡ ºŽ EffectŽ ObjectIDž µî·¹ŸŸ Ž.
						ObjectRegistry & objectregister = pZone->getObjectRegistry();
						objectregister.registerObject(pEffect);
						pZone->addEffect(pEffect);
						tile.addEffect(pEffect);

						const list<Object*>& oList = tile.getObjectList();
						for(list<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++) 
						{
							Object* pTarget = *itr;
							Creature* pTargetCreature = NULL;
							if ( pTarget->getObjectClass() == Object::OBJECT_CLASS_CREATURE 
								&& ( (pTargetCreature = dynamic_cast<Creature*>(pTarget))->isSlayer() || pTargetCreature->isOusters() )
								&& !checkZoneLevelToHitTarget( pTargetCreature )
							) 
							{
								cList.push_back(pTargetCreature);
								_GCSkillToTileOK2.addCListElement(pTargetCreature->getObjectID());
								_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
								_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());

								pEffect->affect(pTargetCreature);
							}
//								pEffect->affect(pTarget);
						}
					}	
				}	
			}

			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);

			_GCSkillToTileOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK2.setSkillType(SkillType);
			_GCSkillToTileOK2.setX(X);
			_GCSkillToTileOK2.setY(Y);
			_GCSkillToTileOK2.setDuration(output.Duration);
			//_GCSkillToTileOK2.addShortData(MODIFY_VISION, ICE_FIELD_SIGHT);

			_GCSkillToTileOK3.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);

			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);

			_GCSkillToTileOK5.setObjectID(pVampire->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);

			_GCSkillToTileOK6.setOrgXY(myX, myY);
			_GCSkillToTileOK6.setSkillType(SkillType);
			_GCSkillToTileOK6.setX(X);
			_GCSkillToTileOK6.setY(Y);
			_GCSkillToTileOK6.setDuration(output.Duration);
			//_GCSkillToTileOK6.addShortData(MODIFY_VISION, ICE_FIELD_SIGHT);

			for(list<Creature*>::const_iterator itr = cList.begin(); itr != cList.end(); itr++)
			{
				Creature* pTargetCreature = *itr;
				if (canSee(pTargetCreature, pVampire)) pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK2);
				else pTargetCreature->getPlayer()->sendPacket(&_GCSkillToTileOK6);
			}

			// º³  ³ž®
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);

			cList.push_back(pVampire);

			list<Creature*> watcherList = pZone->getWatcherList(myX, myY, pVampire);

			// watcherList¿¡­ cList¿¡ ö Ÿ°í, caster(pOusters)ž º ö ŸøŽ °æ¿ìŽ
			// OK4ž ºž³»°í.. cList¿¡ °¡Ž.
			for(list<Creature*>::const_iterator itr = watcherList.begin(); itr != watcherList.end(); itr++)
			{
				bool bBelong = false;
				for(list<Creature*>::const_iterator tItr = cList.begin(); tItr != cList.end(); tItr++)
					if (*itr == *tItr)
						bBelong = true;

				Creature* pWatcher = (*itr);
				if (bBelong == false && canSee(pWatcher, pVampire) == false)
				{
					if (!pWatcher->isPC())
					{
						continue;
					}
					pWatcher->getPlayer()->sendPacket(&_GCSkillToTileOK4);
					cList.push_back(*itr);
				}
			}
					
			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList, false);
			
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
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

DEMONICRESURRECTIO g_DEMONICRESURRECTIO;
