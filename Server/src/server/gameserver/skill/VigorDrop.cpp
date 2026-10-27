////////////////////////////////////////////////////////////////////////////////
// Project     : DARKEDEN
// Module      : Skill - Effect
// File Name   : EffectVigorDrop.h
// Writer      : ��ȫâ
// Date        : 2002.3.28
// Description :
//               Vigor Drop�� --storm���� ���� ������ ������� �����Ǵ�
//               Slayer�� ����̴�.
//               ����� ����ϰ� �Ǹ�, ��� ���� ������ �߽����� 3x3�� ������
//               EffectVigorDrop Effect�� �ٰ� �ȴ�. EffectVigorDrop Effect
//               �� ���������� �������� ���ϴ� ����� �ƴ϶�.
//               �ش� ������ Creature���� EffectVigorDropToCreature Effect��
//               ���̰� ������. EffectVigorDropToCreature�� VigorDrop��
//               ��ü �������� 1/3�� �ش��ϴ� Damage�� 3�� �������� �ش� 
//               Creature���� ���ϰ� ������. 
//     
// History
//     DATE      WRITER         DESCRIPTION
// =========== =========== =====================================================
// 2002.3.28    ��ȫâ      header file �ۼ�
//
//
////////////////////////////////////////////////////////////////////////////////

#include "VigorDrop.h"
#include "EffectVigorDrop.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToTileOK1.h"
#include "Gpackets/GCSkillToTileOK2.h"
#include "Gpackets/GCSkillToTileOK3.h"
#include "Gpackets/GCSkillToTileOK4.h"
#include "Gpackets/GCSkillToTileOK5.h"
#include "Gpackets/GCSkillToTileOK6.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCAddEffectToTile.h"

//////////////////////////////////////////////////////////////////////////////
// �����̾� ������Ʈ �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
SkillResultType VigorDrop::execute(Slayer* pSlayer, ObjectID_t TargetObjectID, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
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

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch����. by sigi. 2002.5.2
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
        //cout << t.toString() << endl;
    }

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

    __END_CATCH
    
    return SKILL_RESULT_FAIL_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
// �����̾� Ÿ�� �ڵ鷯
//  �����̾ Vigor Drop Skill�� Tile�� ��������� ����ϴ� Handler
//////////////////////////////////////////////////////////////////////////////
SkillResultType VigorDrop::execute(Slayer* pSlayer, ZoneCoord_t X, ZoneCoord_t Y, SlayerSkillSlot* pSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;


	EffectVigorDrop * pEffect = NULL;
	EffectVigorDrop * pEffect2 = NULL;

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

		SkillType_t SkillType = pSkillSlot->getSkillType();
		SkillInfo*  pSkillInfo =  g_pSkillInfoManager->getSkillInfo(SkillType);
		SkillDomainType_t DomainType = pSkillInfo->getDomainType();

		ZoneCoord_t myX = pSlayer->getX();
		ZoneCoord_t myY = pSlayer->getY();

		int  RequiredMP = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck = hasEnoughMana(pSlayer, RequiredMP);
		bool bTimeCheck = verifyRunTime(pSkillSlot);
		bool bRangeCheck = verifyDistance(pSlayer, X, Y, pSkillInfo->getRange());
		bool bHitRoll = HitRoll::isSuccessMagic(pSlayer, pSkillInfo, pSkillSlot);

		bool bTileCheck = false;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

		if(rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}


		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && bTileCheck)
		{
			decreaseMana(pSlayer, RequiredMP, _GCSkillToTileOK1);

			// calculate damage and duration time
			SkillInput input(pSlayer, pSkillSlot);
			SkillOutput output;
			computeOutput(input, output);

			// Soul Smashing �� �ִٸ� ������ 10% ����
/*			if ( pSlayer->hasRankBonus( RankBonus::RANK_BONUS_SOUL_SMASHING ) )
			{
				RankBonus* pRankBonus = pSlayer->getRankBonus( RankBonus::RANK_BONUS_SOUL_SMASHING );
				Assert( pRankBonus != NULL );

				output.Damage += pRankBonus->getPoint();
			}*/


			Range_t Range = 3;

			// ������ ���� ����Ʈ�� Ÿ�Ͽ� �ִٸ� ����� ���� �����Ѵ�.
			Tile& tile = pZone->getTile(X, Y);
			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_ENERGY_DROP);
			if(pOldEffect != NULL)
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}

			// ����Ʈ ������Ʈ�� ���ؼ� Ÿ�Ͽ� ���δ�. 
			//cout << "make EffectObject to Tile" << X << " " << Y << endl;
			pEffect = new EffectVigorDrop(pZone, X, Y);

			//pEffect->setCasterName(pSlayer->getName());
			//pEffect->setPartyID(pSlayer->getPartyID());
			pEffect->setUserObjectID( pSlayer->getObjectID() );
			pEffect->setDeadline(output.Duration);
			pEffect->setNextTime(0);
			pEffect->setTick(output.Tick);
			pEffect->setDamage(output.Damage);
			pEffect->setLevel(pSkillInfo->getLevel()/2);

			//
			//ObjectRegistry& objectregister = pZone->getObjectRegistry();
			//objectregister.registerObject(pEffect);

			//
			//
			//pZone->addEffect(pEffect);
			//tile.addEffect(pEffect);
			// ����Ʈ ������Ʈ�� ���ؼ� Ÿ�Ͽ� ���δ�. 
			pEffect2 = new EffectVigorDrop(pZone, X, Y);
			pEffect2->setUserObjectID( pSlayer->getObjectID() );
			pEffect2->setDeadline(output.Duration);
			pEffect2->setNextTime(0);
			pEffect2->setTick(output.Tick);
			pEffect2->setDamage(output.Damage * 30 / 100 );
			pEffect2->setLevel(pSkillInfo->getLevel()/2);

			// ����Ʈ �������� ��� Creature���� effect�� �ٿ��ش�.
			// Slayer�� ����� ����� ��� ���� Slayer���Դ�
			// �ش����� �ʴ´�.
			bool bEffected = false;
			bool bHit = false;

			Creature* pTargetCreature;

			Level_t maxEnemyLevel = 0;
			uint EnemyNum = 0;

			list<Creature*> cList;
			cList.push_back(pSlayer);

			int oX, oY;

			for(oX = -2; oX <= 2; oX++)
			for(oY = -2; oY <= 2; oY++)
			{
				int tileX = X+oX;
				int tileY = Y+oY;
				if (!rect.ptInRect(tileX, tileY)) continue;

				Tile& tile = pZone->getTile(tileX, tileY);
		    	if (!tile.canAddEffect()) continue; 

				pTargetCreature = NULL;
				if(tile.hasCreature(Creature::MOVE_MODE_WALKING))
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

				EffectVigorDrop * pTempEffect = NULL;

				if( oX == 2 || oX == -2 || oY == 2 || oY == -2 ) pTempEffect = pEffect2;
				else pTempEffect = pEffect;

				if(pTargetCreature != NULL && canAttack( pSlayer, pTargetCreature ))
				{
					if(pTargetCreature->isVampire()||pTargetCreature->isOusters())
					{
						if(pTempEffect->affectCreature(pTargetCreature, false) == true)
						{
							//cout << "VigorDrop to Slayer Success" << endl;
							Player* pTargetPlayer = pTargetCreature->getPlayer();
							bEffected = true;

							bHit = true;

							bool bCanSee = canSee(pTargetCreature, pSlayer);

							_GCSkillToTileOK1.addCListElement(pTargetCreature->getObjectID());
							_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
							_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());

							cList.push_back(pTargetCreature);

							if (bCanSee)
							{
								// ����� ���� ������� 
								_GCSkillToTileOK2.setObjectID(pSlayer->getObjectID());
								_GCSkillToTileOK2.setSkillType(SkillType);
								_GCSkillToTileOK2.setX(X);
								_GCSkillToTileOK2.setY(Y);
								_GCSkillToTileOK2.setDuration(output.Duration);
								_GCSkillToTileOK2.setRange(Range);
								pTargetPlayer->sendPacket(&_GCSkillToTileOK2);
							}
						}
						else
						{
							//cout << "VigorDrop to Vampire fail" << endl;
						}
					}
					else if(pTargetCreature->isMonster())
					{
						if(pTempEffect->affectCreature(pTargetCreature, false) == true)
						{
							//cout << "VigorDrop to Monster Success" << endl;
							bHit = true;
							Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);
							pMonster->addEnemy(pSlayer);

							// ������ ���� �ְ� �����̾��� �����Ѵ�. by sigi. 2002.6.21
							pMonster->setLastHitCreatureClass(Creature::CREATURE_CLASS_SLAYER);
						}
						else
						{
							//cout << "VigorDrop to Monster Falis" << endl;
						}

					}
					else continue;

					if ( maxEnemyLevel < pTargetCreature->getLevel() ) maxEnemyLevel = pTargetCreature->getLevel();
					EnemyNum++;
				} // if(pTargetCreature!= NULL)
			}

			if(bHit)
			{
				//cout << "Skill Succesfully Attacked(" << output.Damage << ")" << endl;
				shareAttrExp(pSlayer, output.Damage, 1, 1, 8, _GCSkillToTileOK1);
				increaseDomainExp(pSlayer, DomainType, pSkillInfo->getPoint(), _GCSkillToTileOK1, maxEnemyLevel, EnemyNum);
				increaseSkillExp(pSlayer, DomainType, pSkillSlot, pSkillInfo, _GCSkillToTileOK1);
			}

			// ����� ����� ����鿡��
			_GCSkillToTileOK1.setSkillType(SkillType);
			_GCSkillToTileOK1.setCEffectID(CEffectID);
			_GCSkillToTileOK1.setX(X);
			_GCSkillToTileOK1.setY(Y);
			_GCSkillToTileOK1.setDuration(output.Duration);
			_GCSkillToTileOK1.setRange(Range);
		
			// ����� �� ����� �� �� �ִ� ����鿡��
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(X);
			_GCSkillToTileOK3.setY(Y);
			//_GCSkillToTileOK3.setDuration(output.Duration);
			//_GCSkillToTileOK3.setRange(Range);

			// ����� ���� ����� �� �� �ִ� ����鿡��
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);

			//����� �� ����� ���� ����� ��� �� �� �ִ� ����鿡��
			_GCSkillToTileOK5.setObjectID(pSlayer->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setRange(Range);
	
			// ��ų �� ó��
			postSkillSuccessProcess( SkillType, pSlayer, &_GCSkillToTileOK1 );

			// ����� ����� ������� packet ���
			pPlayer->sendPacket(&_GCSkillToTileOK1);

			// ����� �� ����� ���� ����� ��� �� �� �ִ� ����鿡�� broadcasing
			// broadcasting�� 5��OK�� ���� ����� ����Ѵ�.
			// ���⿡ ��ϵ� ����� ���� broadcasting���� ���ܵȴ�.
			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);
			
			// ����� �� ����� �� �� �ִ� ����鿡�� broadcasting
			pZone->broadcastPacket(myX, myY, &_GCSkillToTileOK3, cList);

			// ����� ���� ����� �� �� �ִ� ����鿡�� broadcasting
			pZone->broadcastPacket(X, Y, &_GCSkillToTileOK4, cList);

			// ��� delay setting
			pSkillSlot->setRunTime(output.Delay);

			return SKILL_RESULT_SUCCESS;

		}
		else
		{
			executeSkillFailNormal(pSlayer, getSkillType(), NULL);
		}

		SAFE_DELETE(pEffect);
		SAFE_DELETE(pEffect2);
	}
	catch(Throwable& t)
	{
		SAFE_DELETE(pEffect);
		SAFE_DELETE(pEffect2);
		executeSkillFailException(pSlayer, getSkillType());
	}

	return SKILL_RESULT_FAIL_UNKNOWN;

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// ���� Ÿ�� �ڵ鷯
//////////////////////////////////////////////////////////////////////////////
void VigorDrop::execute(Monster* pMonster, ZoneCoord_t X, ZoneCoord_t Y)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << "begin " << endl;
	EffectVigorDrop * pEffect = NULL;
	EffectVigorDrop * pEffect2 = NULL;

	try
	{
		Zone* pZone = pMonster->getZone();

		Assert(pZone != NULL);

		GCSkillToTileOK1 _GCSkillToTileOK1;
		GCSkillToTileOK2 _GCSkillToTileOK2;
		GCSkillToTileOK3 _GCSkillToTileOK3;
		GCSkillToTileOK4 _GCSkillToTileOK4;
		GCSkillToTileOK5 _GCSkillToTileOK5;
		GCSkillToTileOK6 _GCSkillToTileOK6;

		SkillType_t SkillType = SKILL_ENERGY_DROP;
		SkillInfo*  pSkillInfo =  g_pSkillInfoManager->getSkillInfo(SkillType);

		ZoneCoord_t myX = pMonster->getX();
		ZoneCoord_t myY = pMonster->getY();

		bool bRangeCheck = verifyDistance(pMonster, X, Y, pSkillInfo->getRange());
		bool bHitRoll = HitRoll::isSuccessMagic(pMonster, pSkillInfo);


		bool bTileCheck = false;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

		if(rect.ptInRect(X, Y))
		{
			Tile& tile = pZone->getTile(X, Y);
			if (tile.canAddEffect()) bTileCheck = true;
		}

		if (bRangeCheck && bHitRoll && bTileCheck)
		{
			// calculate damage and duration time
			SkillInput input(pMonster);
			SkillOutput output;
			computeOutput(input, output);

			Range_t Range = 3;

			// ������ ���� ����Ʈ�� Ÿ�Ͽ� �ִٸ� ����� ���� �����Ѵ�.
			Tile& tile = pZone->getTile(X, Y);
			Effect* pOldEffect = tile.getEffect(Effect::EFFECT_CLASS_ENERGY_DROP);
			if(pOldEffect != NULL)
			{
				ObjectID_t effectID = pOldEffect->getObjectID();
				pZone->deleteEffect(effectID);
			}

			// ����Ʈ ������Ʈ�� ���ؼ� Ÿ�Ͽ� ���δ�. 
			pEffect = new EffectVigorDrop(pZone, X, Y);
			pEffect->setDeadline(output.Duration);
			pEffect->setNextTime(0);
			pEffect->setTick(output.Tick);
			pEffect->setDamage(output.Damage);
			pEffect->setLevel(pSkillInfo->getLevel()/2);

			//
			//ObjectRegistry& objectregister = pZone->getObjectRegistry();
			//objectregister.registerObject(pEffect);

			//
			//
			//pZone->addEffect(pEffect);
			//tile.addEffect(pEffect);

			// ����Ʈ ������Ʈ�� ���ؼ� Ÿ�Ͽ� ���δ�. 
			pEffect2 = new EffectVigorDrop(pZone, X, Y);
			pEffect2->setDeadline(output.Duration);
			pEffect2->setNextTime(0);
			pEffect2->setTick(output.Tick);
			pEffect2->setDamage(output.Damage * 30 / 100 );
			pEffect2->setLevel(pSkillInfo->getLevel()/2);


			// ����Ʈ �������� ��� Creature���� effect�� �ٿ��ش�.
			// Slayer�� ����� ����� ��� ���� Slayer���Դ�
			// �ش����� �ʴ´�.
			bool bEffected = false;
			Creature* pTargetCreature;


			list<Creature*> cList;
			cList.push_back(pMonster);

			int oX, oY;

			for(oX = -2; oX <= 2; oX++)
			for(oY = -2; oY <= 2; oY++)
			{
				int tileX = X+oX;
				int tileY = Y+oY;

				EffectVigorDrop * pTempEffect = NULL;

				if( oX == 2 || oX == -2 || oY == 2 || oY == -2 ) pTempEffect = pEffect2;
				else pTempEffect = pEffect;

				if (!rect.ptInRect(tileX, tileY)) continue;

				Tile& tile = pZone->getTile(tileX, tileY);

				if (!tile.canAddEffect()) continue;

				pTargetCreature = NULL;
				if(tile.hasCreature(Creature::MOVE_MODE_WALKING))
					pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

				if(pTargetCreature != NULL)
				{
					if(pTargetCreature->isPC())
					{
						if(pTempEffect->affectCreature(pTargetCreature, false) == true)
						{
							Player* pTargetPlayer = pTargetCreature->getPlayer();
							bEffected = true;

							bool bCanSee = canSee(pTargetCreature, pMonster);

							_GCSkillToTileOK1.addCListElement(pTargetCreature->getObjectID());
							_GCSkillToTileOK4.addCListElement(pTargetCreature->getObjectID());
							_GCSkillToTileOK5.addCListElement(pTargetCreature->getObjectID());

							cList.push_back(pTargetCreature);

							if (bCanSee)
							{
								// ����� ���� ������� 
								_GCSkillToTileOK2.setObjectID(pMonster->getObjectID());
								_GCSkillToTileOK2.setSkillType(SkillType);
								_GCSkillToTileOK2.setX(X);
								_GCSkillToTileOK2.setY(Y);
								_GCSkillToTileOK2.setDuration(output.Duration);
								_GCSkillToTileOK2.setRange(Range);
								pTargetPlayer->sendPacket(&_GCSkillToTileOK2);
							}
						}
					}
				} // if(pTargetCreature!= NULL)
			}

			// ����� �� ����� �� �� �ִ� ����鿡��
			_GCSkillToTileOK3.setSkillType(SkillType);
			_GCSkillToTileOK3.setX(myX);
			_GCSkillToTileOK3.setY(myY);
			//_GCSkillToTileOK3.setDuration(output.Duration);
			//_GCSkillToTileOK3.setRange(Range);

			// ����� ���� ����� �� �� �ִ� ����鿡��
			_GCSkillToTileOK4.setSkillType(SkillType);
			_GCSkillToTileOK4.setX(X);
			_GCSkillToTileOK4.setY(Y);
			_GCSkillToTileOK4.setDuration(output.Duration);
			_GCSkillToTileOK4.setRange(Range);

			//����� �� ����� ���� ����� ��� �� �� �ִ� ����鿡��
			_GCSkillToTileOK5.setObjectID(pMonster->getObjectID());
			_GCSkillToTileOK5.setSkillType(SkillType);
			_GCSkillToTileOK5.setX(X);
			_GCSkillToTileOK5.setY(Y);
			_GCSkillToTileOK5.setDuration(output.Duration);
			_GCSkillToTileOK5.setRange(Range);
	
			// ����� �� ����� ���� ����� ��� �� �� �ִ� ����鿡�� broadcasing
			cList = pZone->broadcastSkillPacket(myX, myY, X, Y, &_GCSkillToTileOK5, cList);
			
			// ����� �� ����� �� �� �ִ� ����鿡�� broadcasting
			pZone->broadcastPacket(myX, myY, &_GCSkillToTileOK3, cList);

			// ����� ���� ����� �� �� �ִ� ����鿡�� broadcasting
			pZone->broadcastPacket(X, Y, &_GCSkillToTileOK4, cList);


		}
		else
		{
			executeSkillFailNormal(pMonster, getSkillType(), NULL);
		}
		SAFE_DELETE(pEffect);
		SAFE_DELETE(pEffect2);
	}
	catch(Throwable& t)
	{
		SAFE_DELETE(pEffect);
		SAFE_DELETE(pEffect2);
		executeSkillFailException(pMonster, getSkillType());
	}


	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " end " << endl;

	__END_CATCH
}
VigorDrop g_VigorDrop;
