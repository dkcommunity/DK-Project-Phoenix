////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "ADV2TORRENTIALLIFE.h"
#include "EffectComa.h"
#include "EffectKillAftermath.h"
#include "EffectADV2TORRENTIALLIFE.h"
#include "Properties.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "EffectRequestResurrect.h"

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî ¿ÀºêÁ§Æ® ÇÚµé·¯
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2TORRENTIALLIFE::execute(Ousters* pOusters, ObjectID_t TargetObjectID, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	// Slayer Object Assertion
	Assert(pOusters != NULL);
	Assert(pOustersSkillSlot != NULL);

    try
    {
		Zone* pZone = pOusters->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuchÁŠ°Å. by sigi. 2002.5.2
		if (pTargetCreature==NULL
			||  !pTargetCreature->isOusters()
			|| ( g_pConfig->hasKey("Hardcore") && g_pConfig->getPropertyInt("Hardcore")!=0 ) )
		{
			executeSkillFailException(pOusters, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}
		return execute(pOusters, pTargetCreature->getX(), pTargetCreature->getY(), pOustersSkillSlot, CEffectID);
    } 
	catch (Throwable & t) 
	{
		executeSkillFailException(pOusters, getSkillType());
        //cout << t.toString() << endl;
    }

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

    __END_CATCH
    
    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// ¹ìÆÄÀÌŸî ÅžÀÏ ÇÚµé·¯
//  ¹ìÆÄÀÌŸî°¡ Vigor Drop SkillÀ» Tile¿¡ »ç¿ëÇßÀ»¶§ »ç¿ëÇÏŽÂ Handler
//////////////////////////////////////////////////////////////////////////////
SkillResultType ADV2TORRENTIALLIFE::execute(Ousters* pOusters, ZoneCoord_t X, ZoneCoord_t Y, OustersSkillSlot* pOustersSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	try
	{
		Player* pPlayer = pOusters->getPlayer();
		Zone* pZone = pOusters->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

            Item* pWeapon = pOusters->getWearItem(Ousters::WEAR_RIGHTHAND);
		if (pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET || !pOusters->isRealWearingEx(Ousters::WEAR_RIGHTHAND))
		{
			executeSkillFailException(pOusters, pOustersSkillSlot->getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_ITEM;
		}

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType = pOustersSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo =  g_pSkillInfoManager->getSkillInfo(SkillType);
		//SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		ZoneCoord_t myX = pOusters->getX();
		ZoneCoord_t myY = pOusters->getY();

                int  RequiredMP  = (int)pSkillInfo->getConsumeMP() + pOustersSkillSlot->getExpLevel();
		bool bManaCheck  = hasEnoughMana(pOusters, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pOustersSkillSlot);
		bool bRangeCheck = verifyDistance(pOusters, X, Y, pSkillInfo->getRange());
		bool bHitRoll = HitRoll::isSuccessMagic(pOusters, pSkillInfo, pOustersSkillSlot);


		bool bTileCheck = false;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

		if(rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}


		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck)
		{
			decreaseMana(pOusters, RequiredMP, _GCSkillToTileOK1);

			// calculate damage and duration time
			SkillInput input(pOusters, pOustersSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			Creature* pTargetCreature;

			list<Creature*> cList;
			cList.push_back(pOusters);

			int oX, oY;
			bool bHit = false;

			int diff =2;

			if (pOusters->hasRankGem(NewRankGem::SKILL_ADV2_TORRENTIAL_LIFE_RANGE ) )
			{
				NewRankGem* pRankGemTORRENTIALRANGE = pOusters->getRankGem( NewRankGem::SKILL_ADV2_TORRENTIAL_LIFE_RANGE );
				Assert( pRankGemTORRENTIALRANGE != NULL );

				diff =3;
			}

			for(oX = -diff; oX <= diff; oX++)
			for(oY = -diff; oY <= diff; oY++)
			{
				int tileX = X+oX;
				int tileY = Y+oY;
				if (!rect.ptInRect(tileX, tileY)) continue;

				Tile& tile = pZone->getTile(tileX, tileY);
		    	if (!tile.canAddEffect()) continue; 

				pTargetCreature = NULL;
				if(tile.hasCreature(Creature::MOVE_MODE_WALKING))
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

				if(pTargetCreature != NULL && pTargetCreature->isOusters() && pTargetCreature->isFlag( Effect::EFFECT_CLASS_COMA ))
				{
						Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTargetCreature);


						HP_t CurrentHP = pTargetOusters->getHP(ATTR_MAX);
                                                MP_t CurrentMP = pTargetOusters->getMP(ATTR_MAX);

						if( !pTargetOusters->isFlag( Effect::EFFECT_CLASS_REQUEST_RESURRECT ) )
						{
							EffectRequestResurrect* pEffectRequestResurrect = new EffectRequestResurrect(pTargetOusters);
							pEffectRequestResurrect->setDuration(100);
							pEffectRequestResurrect->setHP(CurrentHP);
							pEffectRequestResurrect->setMP(CurrentMP);
							pTargetOusters->addEffect(pEffectRequestResurrect);
							pTargetOusters->setFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT);

							GCAddEffect gcAddEffect;
							gcAddEffect.setObjectID( pTargetOusters->getObjectID() );
							gcAddEffect.setEffectID( Effect::EFFECT_CLASS_REQUEST_RESURRECT );
							gcAddEffect.setDuration( 100 );

							pZone->broadcastPacket( pTargetOusters->getX(), pTargetOusters->getY(), &gcAddEffect );
						}

						_GCSkillToTileOK2.addCListElement( pTargetOusters->getObjectID() );
						_GCSkillToTileOK2.setObjectID(pOusters->getObjectID());
						_GCSkillToTileOK2.setSkillType(SkillType);
						_GCSkillToTileOK2.setX(X);
						_GCSkillToTileOK2.setY(Y);
						pTargetOusters->getPlayer()->sendPacket( &_GCSkillToTileOK2 );

						_GCSkillToTileOK1.addCListElement( pTargetOusters->getObjectID() );
						_GCSkillToTileOK4.addCListElement( pTargetOusters->getObjectID() );
						_GCSkillToTileOK5.addCListElement( pTargetOusters->getObjectID() );
						bHit = true; // 20080811
					}
				}


			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
		
			_GCSkillToTileOK3.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);

			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);

			_GCSkillToTileOK5.setObjectID(pOusters->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
	
			postSkillSuccessProcess( SkillType, pOusters, &_GCSkillToTileOK1 );

			pPlayer->sendPacket(&_GCSkillToTileOK1);

			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);
			
			pZone->broadcastPacket(myX, myY, &_GCSkillToTileOK3, cList);

			pZone->broadcastPacket(X, Y, &_GCSkillToTileOK4, cList);

			pOustersSkillSlot->setRunTime(output.Delay);
			
			return SKILL_RESULT_SUCCESS;

		}
		else
		{
			executeSkillFailNormal(pOusters, getSkillType(), NULL);
		}
	}
	catch(Throwable& t)
	{
		executeSkillFailException(pOusters, getSkillType());
	}

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

ADV2TORRENTIALLIFE g_ADV2TORRENTIALLIFE;
