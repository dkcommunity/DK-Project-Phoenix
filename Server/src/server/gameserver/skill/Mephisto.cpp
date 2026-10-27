//////////////////////////////////////////////////////////////////////////////
// Filename    : Mephisto.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////
#include "Mephisto.h"
#include "EffectMephisto.h"
#include "RankBonus.h"

#include "Gpackets/GCSkillToSelfOK1.h"
#include "Gpackets/GCSkillToSelfOK2.h"
#include "Gpackets/GCSkillToObjectOK1.h"
#include "Gpackets/GCSkillToObjectOK2.h"
#include "Gpackets/GCSkillToObjectOK3.h"
#include "Gpackets/GCSkillToObjectOK4.h"
#include "Gpackets/GCSkillToObjectOK5.h"
#include "Gpackets/GCAddEffect.h"
#include "HitRoll.h"

SkillResultType Mephisto::execute(Vampire* pVampire, VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayerself)" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();

		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();

		if ( !hasEnoughMana(pVampire, RequiredMP) )
		{
			executeSkillFailNormal(pVampire, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_MP;
		}
		
		if ( !checkZoneLevelToUseSkill(pVampire) )
		{
			executeSkillFailNormal(pVampire, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_CANNOT_USE_IN_ZONE;
		}
		
		if ( !verifyRunTime(pVampireSkillSlot) )
		{
			executeSkillFailNormal(pVampire, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_TIME;
		}
		
		// Knowledge of Curse �� �ִٸ� hit bonus 10
		int HitBonus = 0;
/*		if ( pVampire->hasRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_CURSE ) )
		{
			RankBonus* pRankBonus = pVampire->getRankBonus( RankBonus::RANK_BONUS_KNOWLEDGE_OF_CURSE );
			Assert( pRankBonus != NULL );

			HitBonus = pRankBonus->getPoint();
		}*/

		if ( !HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot, HitBonus) )
		{
			executeSkillFailNormal(pVampire, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_NOT_ENOUGH_HITROLL;
		}
		
		if ( pVampire->isFlag(Effect::EFFECT_CLASS_MEPHISTO) )
		{
			executeSkillFailNormal(pVampire, SkillType, NULL);
			
			return SKILL_RESULT_FAIL_CANNOT_OVERLAP_EFFECT;
		}

		GCSkillToSelfOK1 _GCSkillToSelfOK1;
		GCSkillToSelfOK2 _GCSkillToSelfOK2;
			
		decreaseMana(pVampire, RequiredMP, _GCSkillToSelfOK1);

		// ��ų ������ ��� ������ ���ʽ��� �޶����.
		SkillInput input( pVampire );
		SkillOutput output;
		input.SkillLevel = pVampire->getSTR()+pVampire->getDEX()+pVampire->getINT();
		input.DomainLevel = pVampire->getLevel();
		computeOutput(input, output);

		// set Next Run Time
		pVampireSkillSlot->setRunTime(output.Delay);
			
		// ����Ʈ Ŭ������ ����� ���δ�.
		EffectMephisto* pEffect = new EffectMephisto(pVampire);
		pEffect->setDeadline(output.Duration);
		pEffect->setToHitBonus( min(432, (int)( (pVampire->getSTR()/3.5)+(pVampire->getDEX()/3.5)+(pVampire->getINT()/3.5) ) ) );
		pEffect->setDefenseBonus( min( 420, (int)( (pVampire->getSTR()/3.5)+(pVampire->getDEX()/3.5)+(pVampire->getINT()/3.5) ) ) );
		pEffect->setProtectionBonus( min( 420, (int)( (pVampire->getSTR()/3.5)+(pVampire->getDEX()/3.5)+(pVampire->getINT()/3.5) ) ) );
		pVampire->addEffect(pEffect);
		pVampire->setFlag(Effect::EFFECT_CLASS_MEPHISTO);

		// �̷� ���Ͽ� �ٲ�� �ɷ�ġ�� ������.
		pVampire->initAllStatAndSend();

		// ��Ŷ�� ����� ������.
		_GCSkillToSelfOK1.setSkillType(SkillType);
		_GCSkillToSelfOK1.setCEffectID(CEffectID);
		_GCSkillToSelfOK1.setDuration(output.Duration);
		
		_GCSkillToSelfOK2.setObjectID(pVampire->getObjectID());
		_GCSkillToSelfOK2.setSkillType(SkillType);
		_GCSkillToSelfOK2.setDuration(output.Duration);
		
		// ��ų �� ó��
		postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToSelfOK1 );

		pPlayer->sendPacket(&_GCSkillToSelfOK1);
		
		pZone->broadcastPacket(pVampire->getX(), pVampire->getY(),  &_GCSkillToSelfOK2, pVampire);

		// ����Ʈ�� �پ�ٰ� �˷��ش�.
		GCAddEffect gcAddEffect;
		gcAddEffect.setObjectID(pVampire->getObjectID());
		gcAddEffect.setEffectID(Effect::EFFECT_CLASS_MEPHISTO);
		gcAddEffect.setDuration(output.Duration);
		pZone->broadcastPacket(pVampire->getX(), pVampire->getY(), &gcAddEffect);

		return SKILL_RESULT_SUCCESS;
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerself)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

SkillResultType Mephisto::execute(Vampire* pVampire, ObjectID_t TargetObjectID,  VampireSkillSlot* pVampireSkillSlot, CEffectID_t CEffectID)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayerself)" << endl;

	Assert(pVampire != NULL);
	Assert(pVampireSkillSlot != NULL);


	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);

		Creature* pTargetCreature = pZone->getCreature(TargetObjectID);
		//Assert(pTargetCreature != NULL);

		// NoSuch����. by sigi. 2002.5.2
		if (pTargetCreature==NULL)
		{
			executeSkillFailException(pVampire, getSkillType());
			
			return SKILL_RESULT_FAIL_INVALID_TARGET;
		}

		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;

		SkillType_t       SkillType  = pVampireSkillSlot->getSkillType();
		SkillInfo*        pSkillInfo = g_pSkillInfoManager->getSkillInfo(SkillType);

		int  RequiredMP  = (int)pSkillInfo->getConsumeMP();
		bool bManaCheck  = hasEnoughMana(pVampire, RequiredMP);
		bool bTimeCheck  = verifyRunTime(pVampireSkillSlot);
		bool bRangeCheck = checkZoneLevelToUseSkill(pVampire);
		bool bHitRoll    = HitRoll::isSuccessMagic(pVampire, pSkillInfo, pVampireSkillSlot);
		bool bEffected   = pTargetCreature->isFlag(Effect::EFFECT_CLASS_MEPHISTO);

		ZoneCoord_t myX = pVampire->getX();
		ZoneCoord_t myY = pVampire->getY();

		if (bManaCheck && bTimeCheck && bRangeCheck && bHitRoll && !bEffected && pTargetCreature->isVampire() )
		{
			Vampire* pTargetVampire= dynamic_cast<Vampire*>(pTargetCreature);

			ZoneCoord_t X   = pTargetVampire->getX();
			ZoneCoord_t Y   = pTargetVampire->getY();

			decreaseMana(pVampire, RequiredMP, _GCSkillToObjectOK1);

			// ��ų ������ ��� ������ ���ʽ��� �޶����.
			SkillInput input(pVampire);
			SkillOutput output;
			input.SkillLevel = pVampire->getSTR()+pVampire->getDEX()+pVampire->getINT();
			input.DomainLevel = pVampire->getLevel();
			computeOutput(input, output);

			// ����Ʈ Ŭ������ ����� ���δ�.
			EffectMephisto* pEffect = new EffectMephisto(pTargetVampire);
			pEffect->setDeadline(output.Duration);
			pEffect->setToHitBonus( min( 180, (int)( (pVampire->getSTR()/3.5)+(pVampire->getDEX()/3.5)+(pVampire->getINT()/6) ) ) );
			pEffect->setDefenseBonus( min( 150, (int)( (pVampire->getSTR()/6)+(pVampire->getDEX()/3.5)+(pVampire->getINT()/6) ) ) );
			pEffect->setProtectionBonus( min( 150, (int)( (pVampire->getSTR()/3.5)+(pVampire->getDEX()/6)+(pVampire->getINT()/6) ) ) );
			pTargetVampire->addEffect(pEffect);
			pTargetVampire->setFlag(Effect::EFFECT_CLASS_MEPHISTO);

			// �̷� ���Ͽ� �ٲ�� �ɷ�ġ�� ������.
			pTargetVampire->initAllStatAndSend();

			// ��Ŷ�� ����� ������.
			_GCSkillToObjectOK1.setSkillType(SkillType);
			_GCSkillToObjectOK1.setCEffectID(CEffectID);
			_GCSkillToObjectOK1.setDuration(output.Duration);
		
			_GCSkillToObjectOK2.setObjectID(pVampire->getObjectID());
			_GCSkillToObjectOK2.setSkillType(SkillType);
			_GCSkillToObjectOK2.setDuration(output.Duration);

			_GCSkillToObjectOK3.setObjectID(pVampire->getObjectID());
		    _GCSkillToObjectOK3.setSkillType(SkillType);
		    _GCSkillToObjectOK3.setTargetXY (X, Y);

			// ��ų �� ó��
			postSkillSuccessProcess( SkillType, pVampire, &_GCSkillToObjectOK1 );

			pPlayer->sendPacket(&_GCSkillToObjectOK1);

			if (pTargetCreature->isPC())
			{
				Player* pTargetPlayer = pTargetCreature->getPlayer();
				Assert(pTargetPlayer != NULL);
	
				_GCSkillToObjectOK2.setObjectID(pVampire->getObjectID());
	
				pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
			}
			else
			{
				Assert(false);
			}

			list<Creature *> cList;
			cList.push_back(pTargetCreature);
			cList.push_back(pVampire);
			pZone->broadcastPacket(myX, myY, &_GCSkillToObjectOK3, cList);

			// ����Ʈ�� �پ�ٰ� �˷��ش�.
			GCAddEffect gcAddEffect;
			gcAddEffect.setObjectID(pTargetVampire->getObjectID());
			gcAddEffect.setEffectID(Effect::EFFECT_CLASS_MEPHISTO);
			gcAddEffect.setDuration(output.Duration);
			pZone->broadcastPacket(pTargetVampire->getX(), pTargetVampire->getY(), &gcAddEffect);

			// set Next Run Time
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

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerself)" << endl;

	__END_CATCH
	
	return SKILL_RESULT_FAIL_UNKNOWN;
}

void Mephisto::execute(Vampire* pVampire)
	throw(Error)
{
	__BEGIN_TRY

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " Begin(slayerself)" << endl;

	Assert(pVampire != NULL);

	if( !pVampire->hasSkill(SKILL_MEPHISTO) ) return;

	try 
	{
		Player* pPlayer = pVampire->getPlayer();
		Zone* pZone = pVampire->getZone();
		Assert(pPlayer != NULL);
		Assert(pZone != NULL);


		GCSkillToObjectOK1 _GCSkillToObjectOK1;
		GCSkillToObjectOK2 _GCSkillToObjectOK2;
		GCSkillToObjectOK3 _GCSkillToObjectOK3;

		ZoneCoord_t myX = pVampire->getX();
		ZoneCoord_t myY = pVampire->getY();

		int oX, oY;

		VSRect rect(0, 0, pZone->getWidth()-1, pZone->getHeight()-1);

		for(oX = -4; oX <= 4; oX++)
		for(oY = -4; oY <= 4; oY++)
		{
			int tileX = myX+oX;
			int tileY = myY+oY;
			if (!rect.ptInRect(tileX, tileY)) continue;

			// Ÿ�� ����! �����̾ �ִ��� ����!
			Tile& tile = pZone->getTile(tileX, tileY);
			Creature * pTargetCreature = NULL;
			if(tile.hasCreature(Creature::MOVE_MODE_WALKING)) pTargetCreature = tile.getCreature(Creature::MOVE_MODE_WALKING);

			if( pTargetCreature != NULL && pTargetCreature != pVampire && pTargetCreature->isVampire() ) {

				bool bEffected   = pTargetCreature->isFlag(Effect::EFFECT_CLASS_MEPHISTO);

				if( bEffected ) continue;

				Vampire* pTargetVampire= dynamic_cast<Vampire*>(pTargetCreature);

				// ��ų ������ ��� ������ ���ʽ��� �޶����.
				SkillInput input(pVampire);
				SkillOutput output;
				input.SkillLevel = pVampire->getSTR()+pVampire->getDEX()+pVampire->getINT();
				input.DomainLevel = pVampire->getLevel();
				computeOutput(input, output);

				// ����Ʈ Ŭ������ ����� ���δ�.
				EffectMephisto* pEffect = new EffectMephisto(pTargetVampire);
				pEffect->setDeadline(output.Duration);
				pEffect->setToHitBonus( min( 180, (int)( (pVampire->getSTR()/3.5)+(pVampire->getDEX()/3.5)+(pVampire->getINT()/6) ) ) );
				pEffect->setDefenseBonus( min( 150, (int)( (pVampire->getSTR()/6)+(pVampire->getDEX()/3.5)+(pVampire->getINT()/6) ) ) );
				pEffect->setProtectionBonus( min( 150, (int)( (pVampire->getSTR()/3.5)+(pVampire->getDEX()/6)+(pVampire->getINT()/6) ) ) );
				pTargetVampire->addEffect(pEffect);
				pTargetVampire->setFlag(Effect::EFFECT_CLASS_MEPHISTO);

				// �̷� ���Ͽ� �ٲ�� �ɷ�ġ�� ������.
				pTargetVampire->initAllStatAndSend();

				if (pTargetCreature->isPC())
				{
					Player* pTargetPlayer = pTargetCreature->getPlayer();
					Assert(pTargetPlayer != NULL);

					pTargetPlayer->sendPacket(&_GCSkillToObjectOK2);
				}
				else
				{
					Assert(false);
				}

				// ����Ʈ�� �پ�ٰ� �˷��ش�.
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(pTargetVampire->getObjectID());
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_MEPHISTO);
				gcAddEffect.setDuration(output.Duration);
				pZone->broadcastPacket(pTargetVampire->getX(), pTargetVampire->getY(), &gcAddEffect);
			}
		}
	} 
	catch (Throwable & t) 
	{
		executeSkillFailException(pVampire, getSkillType());
	}

	//cout << "TID[" << Thread::self() << "]" << getSkillHandlerName() << " End(slayerself)" << endl;

	__END_CATCH
}


Mephisto g_Mephisto;
