//////////////////////////////////////////////////////////////////////////////
// Filename    : BloodyWings.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "BloodyWings.h"
#include "EffectComa.h"
#include "EffectKillAftermath.h"
#include "EffectRequestResurrect.h"
#include "RankBonus.h"
#include "Properties.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffectToTile.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCStatusCurrentHP.h"
#include "Gpackets/GCAddEffect.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� ������Ʈ �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType BloodyWings::execute(Vampire* pVampire, ObjectID_t TargetObjectID, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
    throw(Error)
{
    __BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	//cout << "BloodyWings" << endl;

	try
	{
		Zone* pZone = pVampire->getZone();
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		//int NeedInt = pVampire->getINT();
		//bool canuse = false;
		//if(NeedInt>=400)
		//  canuse = true;		
		// NPC�� ����� ���� ���.
		// NoSuch����. by sigi. 2002.5.2
		if (pTargetCreature==NULL
				//|| canuse
				|| pTargetCreature->isNPC()
				// HIDE �� ���� �ǻ������ �̻��ϰ� �ȴ�. �ϴ� ���Ƴ���.
				// 2003. 1. 17. DEW
				|| pTargetCreature->isFlag(Effect::EFFECT_CLASS_HIDE)
				|| ( g_pConfig->hasKey("Hardcore") && g_pConfig->getPropertyInt("Hardcore")!=0 && pTargetCreature->isDead() ) )
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}


		ZoneCoord_t X = pTargetCreature->getX();
		ZoneCoord_t Y = pTargetCreature->getY();

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		// Knowledge of Innate �� �ִٸ� hit bonus 10
		int HitBonus = 0;
/*		if ( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE ) )
		{
			RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_INNATE );
			Assert( pRankBonus != NULL );

			HitBonus = pRankBonus->getPoint();
		}*/

		//bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		int RequiredMP = (int)pSkillInfo->getConsumeMP();
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = verifyDistance(pVampire, X, Y, pSkillInfo->getRange());
//		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, HitBonus);
		bool bHPCheck	 = hasEnoughMana(pVampire, RequiredMP);
		bool bEffect	 = pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA);

		Range_t Range = 1;

		if (bTimeCheck && bRangeCheck && bHPCheck && bEffect)
		{
			//cout << "BloodyWings Success" << endl;

			decreaseMana(pVampire, RequiredMP, _GCSkillToTileOK1);


			// �������� ���� �ð��� ����Ѵ�.
			SkillInput input(pVampire);
			SkillOutput output;
			computeOutput(input, output);

			output.Damage = (pVampire->getINT() / 5);

			// TargetCreature�� HP�� ä���
			if (pTargetCreature->isVampire())
			{
				Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTargetCreature);
				bool bCanResurrect = false;

				if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_COMA))
				{
					if( !pTargetCreature->isFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT) )
					{
						bCanResurrect = true;
					}

					if( bCanResurrect )
					{
						HP_t CurrentHP = min( (int)pTargetVampire->getHP(ATTR_MAX), getPercentValue(pTargetVampire->getHP(ATTR_MAX), output.Damage) );

						EffectRequestResurrect* pEffectRequestResurrect = new EffectRequestResurrect(pTargetVampire);
						pEffectRequestResurrect->setDeadline(100);
						pEffectRequestResurrect->setHP(CurrentHP);
						pTargetVampire->addEffect(pEffectRequestResurrect);
						pTargetVampire->setFlag(Effect::EFFECT_CLASS_REQUEST_RESURRECT);

						GCAddEffect gcAddEffect;
						gcAddEffect.setObjectID( pTargetVampire->getObjectID() );
						gcAddEffect.setEffectID( Effect::EFFECT_CLASS_REQUEST_RESURRECT );
						gcAddEffect.setDuration( 100 );

						pZone->broadcastPacket( pTargetVampire->getX(), pTargetVampire->getY(), &gcAddEffect);
					}
				}


				_GCSkillToTileOK1.addCListElement( pTargetVampire->getObjectID() );
				_GCSkillToTileOK4.addCListElement( pTargetVampire->getObjectID() );
				_GCSkillToTileOK5.addCListElement( pTargetVampire->getObjectID() );

				ZoneCoord_t myX = pVampire->getX();
				ZoneCoord_t myY = pVampire->getY();

				_GCSkillToTileOK1.setSkillType(SkillType);
				_GCSkillToTileOK1.setCEffectID(CEffectID);
				_GCSkillToTileOK1.setX(X);
				_GCSkillToTileOK1.setY(Y);
				_GCSkillToTileOK1.setDuration(output.Duration);

				_GCSkillToTileOK2.setSkillType(SkillType);
				_GCSkillToTileOK2.setObjectID(pVampire->getObjectID());
				_GCSkillToTileOK2.setX(X);
				_GCSkillToTileOK2.setY(Y);
				_GCSkillToTileOK2.setDuration(output.Duration);
				pTargetVampire->getPlayer()->sendPacket(&_GCSkillToTileOK2);

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

				Player* pPlayer = pVampire->getPlayer();
				Assert(pPlayer != NULL);

				// ��ų �� ó��
				postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToTileOK1 );

				pPlayer->sendPacket(&_GCSkillToTileOK1);

				list<Creature*> cList;
				cList.push_back(pTargetCreature);

				cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);

				pZone->broadcastPacket(myX, myY,  &_GCSkillToTileOK3 , cList);
				pZone->broadcastPacket(X, Y,  &_GCSkillToTileOK4 , cList);

				pVampireSkillSlot->setRunTime(output.Delay);
				
				return SKILL_RESULT_SUCCESS;
			} 
		}
		else 
		{
			//cout << "BloodyWings Failed" << endl;
			executeSkillFailNormal(pVampire, getSkillType(), NULL);
		}
	} 
	catch (Throwable & t) 
	{
		//cout << "BloodyWings Failed2" << endl;
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

    __END_CATCH
    
    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// �����̾� Ÿ�� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType BloodyWings::execute(Vampire* pVampire, ZoneCoord_t X, ZoneCoord_t Y, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;
    try
    {
		Zone* pZone = pVampire->getZone();
		Assert(pZone!=NULL);

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

		//int NeedInt = pVampire->getINT();
		//bool canuse = false;
		//if(NeedInt>=400)
		//  canuse = true;
		//if(canuse == false)
		//{
		//	executeSkillFailException(pVampire, getSkillType());
		//}

        if (rect.ptInRect(X, Y))
		{
			Tile& rTile = pZone->getTile(X, Y);

			if (rTile.hasWalkingCreature())
			{
				Creature* pCreature = rTile.getCreature( Creature::MOVE_MODE_WALKING );

				return execute( pVampire, pCreature->getObjectID(), pVampireSkillSlot, CEffectID);
			}
			else
			{
				executeSkillFailException(pVampire, getSkillType());
			}
		}
		else
		{
			executeSkillFailException(pVampire, getSkillType());
		}
	}
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
    }

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}


BloodyWings g_BloodyWings;
