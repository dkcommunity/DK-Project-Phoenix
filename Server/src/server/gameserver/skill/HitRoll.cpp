//////////////////////////////////////////////////////////////////////////////
// Filename    : HitRoll.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "HitRoll.h"
#include "SkillInfo.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "LogClient.h"
#include "RankBonus.h"
#include "Skill.h"
#include "Zone.h"
//#include <math.h>

#include "EffectPrecedence.h"
#include "EffectHymn.h"

// 20070919
#include "Effect.h"

#include "Gpackets/GCSystemMessage.h"
#include "GamePlayer.h"
//////////////////////////////////////////////////////////////////////////////
// �Ϲ� ���� hitroll �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccess(Creature* pAttacker, Creature* pDefender, int ToHitBonus)
{
	Assert(pAttacker != NULL);
	Assert(pDefender != NULL);

	Zone* pZone = pAttacker->getZone();
	Assert( pZone != NULL );

	if (pDefender->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
	{
		return false;
	}


	ToHit_t   ToHit    = 0;
	Defense_t Defense  = 0;
	uint      timeband = pZone->getTimeband();

	bool 	  isMonster = false;
	
	if (pAttacker->isSlayer())
	{
		Slayer* pSlayerAttacker = dynamic_cast<Slayer*>(pAttacker);
		Assert(pSlayerAttacker != NULL);

		ToHit = pSlayerAttacker->getToHit();

	} 
	else if (pAttacker->isVampire())
	{
		Vampire* pVampireAttacker = dynamic_cast<Vampire*>(pAttacker);
		Assert(pVampireAttacker != NULL);

		ToHit = pVampireAttacker->getToHit();
		ToHit = (ToHit_t)getPercentValue(ToHit, VampireTimebandFactor[timeband]);

		if (pAttacker->isFlag(Effect::EFFECT_CLASS_HYMN))
		{
			EffectHymn* pHymn = dynamic_cast<EffectHymn*>( pAttacker->getEffectManager()->findEffect( Effect::EFFECT_CLASS_HYMN ) );	

			ToHit = ToHit*(100-pHymn->getToHitPenalty())/100;
		}
	} 
	else if (pAttacker->isOusters())
	{
		Ousters* pOustersAttacker = dynamic_cast<Ousters*>(pAttacker);
		Assert(pOustersAttacker != NULL);

		ToHit = pOustersAttacker->getToHit();
	} 
	else if (pAttacker->isMonster())
	{
		Monster* pMonsterAttacker = dynamic_cast<Monster*>(pAttacker);
		Assert(pMonsterAttacker != NULL);

		ToHit = pMonsterAttacker->getToHit();
		ToHit = (ToHit_t)getPercentValue(ToHit, MonsterTimebandFactor[timeband]);
		isMonster = true;

		if (pAttacker->isFlag(Effect::EFFECT_CLASS_HYMN))
		{
			EffectHymn* pHymn = dynamic_cast<EffectHymn*>( pAttacker->getEffectManager()->findEffect( Effect::EFFECT_CLASS_HYMN ) );

			ToHit = ToHit*(100-pHymn->getToHitPenalty())/100;
		}

	}
	else
	{
		return false;
	}
	
	if (pDefender->isSlayer())
	{
		Slayer* pSlayerDefender = dynamic_cast<Slayer*>(pDefender);
		Defense = pSlayerDefender->getDefense();

	} 
	else if (pDefender->isVampire())
	{
		Vampire* pVampireDefender = dynamic_cast<Vampire*>(pDefender);
		Defense = pVampireDefender->getDefense();
		Defense = (Defense_t)getPercentValue(Defense, VampireTimebandFactor[timeband]);
	} 
	else if (pDefender->isOusters())
	{
		Ousters* pOustersDefender = dynamic_cast<Ousters*>(pDefender);
		Defense = pOustersDefender->getDefense();
	} 
	else if (pDefender->isMonster())
	{
		Monster* pMonsterDefender= dynamic_cast<Monster*>(pDefender);
		Defense = pMonsterDefender->getDefense();
		Defense = (Defense_t)getPercentValue(Defense, MonsterTimebandFactor[timeband]);
#ifndef __CHINA_SERVER__
		isMonster = true;
#endif
	}
	else
	{
		return false;
	}

	int RandValue = Random(0, 100);
	int Result    = 0;

	if (ToHit >= Defense) 
	{
#ifdef __CHINA_SERVER__
		Result = min(90, (int)(((ToHit - Defense) / 1.5) + 60) + ToHitBonus);
#else
		if ( isMonster ) 
		{
			Result = min(95, (int)(((ToHit - Defense) / 3) + 50) + ToHitBonus/2);
		}
		else
		{
			Result = min(90, (int)(((ToHit - Defense) / 3.8) + 50) + ToHitBonus/2);
		}
#endif
	} 
	else 
	{
#ifdef __CHINA_SERVER__
		if( isMonster ) {
			Result = max(10, (int)(60 - ((Defense - ToHit) / 1.5) + ToHitBonus));
		} else {
			Result = max(20, (int)(60 - ((Defense - ToHit) / 1.5) + ToHitBonus));
		}
#else
		if( isMonster ) {
			Result = max(5, (int)(50 - ((Defense - ToHit) / 3) + ToHitBonus/2));
		} else {

			Result = max(10, min(90, (int)(50 - ((Defense - ToHit) / 3.8) + ToHitBonus/2)));
		}
#endif
	}

	if (RandValue <= Result) return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// �Ϲ� ���� hitroll �Լ�
//////////////////////////////////////////////////////////////////////////////
/*bool HitRoll::isSuccess(ToHit_t ToHit, Creature* pDefender, int ToHitBonus)
{
	Assert(pDefender != NULL);

	Zone* pZone = pDefender->getZone();
	Assert( pZone != NULL );

	// �������� üũ. by sigi. 2002.9.5
	if (pDefender->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
	{
		return false;
	}


	Defense_t Defense  = 0;
	uint      timeband = pZone->getTimeband();

	// �������� ���潺�� ������ �ش�.
	if (pDefender->isSlayer())
	{
		Slayer* pSlayerDefender = dynamic_cast<Slayer*>(pDefender);
		Defense = pSlayerDefender->getDefense();
	} 
	else if (pDefender->isVampire())
	{
		Vampire* pVampireDefender = dynamic_cast<Vampire*>(pDefender);
		Defense = pVampireDefender->getDefense();
		Defense = (Defense_t)getPercentValue(Defense, VampireTimebandFactor[timeband]);
	} 
	else if (pDefender->isOusters())
	{
		Ousters* pOustersDefender = dynamic_cast<Ousters*>(pDefender);
		Defense = pOustersDefender->getDefense();
	} 
	else if (pDefender->isMonster())
	{
		Monster* pMonsterDefender= dynamic_cast<Monster*>(pDefender);
		Defense = pMonsterDefender->getDefense();
		Defense = (Defense_t)getPercentValue(Defense, MonsterTimebandFactor[timeband]);
	}
	else
	{
		// ���� ũ������ Ŭ������ Slayer, Vampire, Monster, NPC ���ε�...
		// �̱��� �Դٴ� ���� �����ڰ� NPC���� ������.
		// NPC AI�� �������� ���� �����̹Ƿ�, ������ �����̴�.
		return false;
	}

	int RandValue = Random(0, 100);
	int Result    = 0;

	if (ToHit >= Defense) 
	{
		// ������ ���潺���� ���� ���쿡�� ���� Ȯ���� ��...����.
#ifdef __CHINA_SERVER__
		Result = min(90, (int)(((ToHit - Defense) / 1.5) + 60) + ToHitBonus);
#else
		Result = min(95, (int)(((ToHit - Defense) / 3) + 50) + ToHitBonus/2);
#endif
	} 
	else 
	{
		// ������ ���潺���� ���� ���쿡�� ���� Ȯ���� ���� ��������.
#ifdef __CHINA_SERVER__
		Result = max(20, (int)(60 - ((Defense - ToHit) / 1.5) + ToHitBonus));
#else
		Result = max(5, (int)(50 - ((Defense - ToHit) / 3) + ToHitBonus/2));
#endif
	}

	if (RandValue <= Result) return true;

	return false;
}*/

//#ifndef VERSION_SKILL_1
//////////////////////////////////////////////////////////////////////////////
// �����̾��� ���� hitroll �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Slayer* pSlayer, SkillInfo* pSkillInfo, SlayerSkillSlot* pSkillSlot)
{
	Assert(pSlayer != NULL);
	Assert(pSkillInfo != NULL);
	Assert(pSkillSlot != NULL);

	int RandValue = Random (1, 100);
#ifdef __CHINA_SERVER__
	int SuccessRatio = (int)(45 - pSkillInfo->getLevel() / 2 + (int)((pSlayer->getINT() + pSkillSlot->getExpLevel()) / 2.5));
#else
	int SuccessRatio = (int)(60 - pSkillInfo->getLevel() / 3 + (int)((pSlayer->getINT() + pSkillSlot->getExpLevel()) / 2.5));
#endif

	// �����̾� ���� ��ų�� ���� �ּ� Ȯ���� 50%�̴�.
	if ( isSlayerSelfSkill( pSkillSlot->getSkillType() ) )
		SuccessRatio = max( 50, SuccessRatio );

	if (RandValue < SuccessRatio ) return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// �����̾��� ���� hitroll �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Vampire* pVampire, SkillInfo* pSkillInfo, VampireSkillSlot* pVampireSkillSlot, int BonusPoint)
{
	Assert(pVampire != NULL);
	Assert(pSkillInfo != NULL);
	Assert(pVampireSkillSlot != NULL);

	int RandValue = Random(1, 100); 
	int Success = (int)(45 - pSkillInfo->getLevel() / 2 + (pVampire->getINT() + pVampire->getLevel()) / 4);

	if ( BonusPoint != 0 )
	{
		Success = getPercentValue( Success, 100 + BonusPoint );
	}
	
	if ( RandValue < Success ) return true;
	
//	if (RandValue < (int)(45 - pSkillInfo->getLevel() / 2 + (pVampire->getINT() + pVampire->getLevel()) / 4)) return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// �ƿ콺�ͽ��� ���� hitroll �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Ousters* pOusters, SkillInfo* pSkillInfo, OustersSkillSlot* pOustersSkillSlot, int BonusPoint)
{
	Assert(pOusters != NULL);
	Assert(pSkillInfo != NULL);
	Assert(pOustersSkillSlot != NULL);

	int RandValue = Random(1, 100); 
//	int Success = (int)( 45 - pSkillInfo->getLevel() / 2 + (pOusters->getINT() + pOusters->getLevel())/10 );
	int Success = (int)( 45 + (pOusters->getINT() + pOusters->getLevel())/4 + pOustersSkillSlot->getExpLevel()/3 );

	if ( isOustersSelfSkill( pOustersSkillSlot->getSkillType() ) )
	{
		Success = max( Success, 60 );
	}

	if ( BonusPoint != 0 )
	{
		Success = getPercentValue( Success, 100 + BonusPoint );
	}
	
	if ( RandValue < Success ) return true;
	
	return false;
}

 
//////////////////////////////////////////////////////////////////////////////
// �����Ϳ� �Ϲ� ���� ���� ���� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagic(Monster* pMonster, SkillInfo* pSkillInfo)
{
	Assert(pMonster != NULL);
	Assert(pSkillInfo != NULL);

	int ratio = (int)(45 - pSkillInfo->getLevel() / 2 + (pMonster->getINT() + pMonster->getLevel()) / 4);
	if (rand()%100 < ratio) return true;

	return false;
}
//#endif

//////////////////////////////////////////////////////////////////////////////
// �����̾� �� �����Ϳ� ���� ���� ���� �Լ�
//////////////////////////////////////////////////////////////////////////////
// multiplier�� ������ 3�̴�. ��.. 33%���ϰ� ���� ���ɼ�ġ�ε�..
// �������� ���쿡�� multiplier�� 2�� �ؼ�.. 50% ���ϰ� ���� ���ɼ�ġ�� ����
// by sigi. 2002.9.16
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessBloodDrain(Creature* pAttacker, Creature* pDefender, int multiplier)
{
	Assert(pAttacker != NULL);
	Assert(pDefender != NULL);

	Zone* pZone = pAttacker->getZone();
	Assert( pZone != NULL );

	// �������� üũ. by sigi. 2002.9.5
	if (pDefender->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
	{
		return false;
	}


	const int normalMultiplier = 3;	// master flag�� parameter�� �ް� �̰� �����Ѵ�. - -;
	bool bHPCheck  = false;
	bool bEffected = false;
	uint timeband  = pZone->getTimeband();

	int ToHit      = 0;
	int Defense    = 0;
	int VampLevel  = 0;
	int OtherLevel = 0;
	int ratio      = 0;

	// Ÿ���� �Ǹ� �� �� �ִ� ��������,
	// ���潺�� ��������, ������ ���������� �����Ѵ�.
	if (pDefender->isSlayer())
	{
		Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pDefender);
		int MaxHP = pTargetSlayer->getHP(ATTR_MAX);
		int CurHP = pTargetSlayer->getHP(ATTR_CURRENT);	

		bHPCheck  = (CurHP*multiplier <= MaxHP) ? true : false;
		bEffected = (normalMultiplier==multiplier) && pTargetSlayer->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
		Defense   = pTargetSlayer->getDefense();

		OtherLevel = 
			pTargetSlayer->getSTR(ATTR_CURRENT) +
			pTargetSlayer->getDEX(ATTR_CURRENT) +
			pTargetSlayer->getINT(ATTR_CURRENT) +
			pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_BLADE) +
			pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_SWORD) +
			pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_GUN) +
			pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_HEAL) +
			pTargetSlayer->getSkillDomainLevel(SKILL_DOMAIN_ENCHANT);

		OtherLevel = (int)(OtherLevel/350);

		// ü���� ���� 1/3�� �Ѱų�,
		// �̹� ������ ���� ���¶��� ������ �� �� ����.
		if (!bHPCheck || bEffected) return false;
	}
	else if (pDefender->isVampire())
	{
		Vampire* pTargetVampire = dynamic_cast<Vampire*>(pDefender);
		int MaxHP = pTargetVampire->getHP(ATTR_MAX);
		int CurHP = pTargetVampire->getHP(ATTR_CURRENT);		

		bHPCheck   = (CurHP*multiplier <= MaxHP) ? true : false;
		//bEffected  = pTargetVampire->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
		bEffected = (normalMultiplier==multiplier) && pTargetVampire->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
		Defense    = pTargetVampire->getDefense() + pTargetVampire->getLevel() / 5;
		OtherLevel = pTargetVampire->getLevel();

		Defense = (Defense_t)getPercentValue(Defense, VampireTimebandFactor[timeband]);

		// ü���� ���� 1/3�� �Ѱų�,
		// �̹� ������ ���� ���¶��� ������ �� �� ����.
		if (!bHPCheck || bEffected) return false;
	}
	else if (pDefender->isOusters())
	{
		Ousters* pTargetOusters = dynamic_cast<Ousters*>(pDefender);
		int MaxHP = pTargetOusters->getHP(ATTR_MAX);
		int CurHP = pTargetOusters->getHP(ATTR_CURRENT);		

		bHPCheck   = (CurHP*multiplier <= MaxHP) ? true : false;
		//bEffected  = pTargetOusters->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
		bEffected = (normalMultiplier==multiplier) && pTargetOusters->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
		Defense    = pTargetOusters->getDefense() + pTargetOusters->getLevel() / 5;
		OtherLevel = pTargetOusters->getLevel();

		// ü���� ���� 1/3�� �Ѱų�,
		// �̹� ������ ���� ���¶��� ������ �� �� ����.
		if (!bHPCheck || bEffected) return false;
	}
	else if (pDefender->isMonster())
	{
		Monster* pTargetMonster = dynamic_cast<Monster*>(pDefender);
		int MaxHP = pTargetMonster->getHP(ATTR_MAX);
		int CurHP = pTargetMonster->getHP(ATTR_CURRENT);		

		bHPCheck   = (CurHP*multiplier <= MaxHP) ? true : false;
		//bEffected  = pTargetMonster->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
		bEffected = (normalMultiplier==multiplier) && pTargetMonster->isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN);
		Defense    = pTargetMonster->getDefense() + pTargetMonster->getLevel() / 5;
		OtherLevel = pTargetMonster->getLevel();

		Defense = (Defense_t)getPercentValue(Defense, MonsterTimebandFactor[timeband]);

		// ü���� ���� 1/3�� �Ѱų�,
		// �̹� ������ ���� ���¶��� ������ �� �� ����.
		if (!bHPCheck || bEffected) return false;

		// ���� �켱���� �̹� ���õǾ� �ִ� �����Ͷ���...
		// ������ �� �ִ� ������ �켱���� ���� ���̰ų�, �켱���� ���� ��Ƽ�� ���� �ڿ��� �Ѵ�.
		if (pTargetMonster->isFlag(Effect::EFFECT_CLASS_PRECEDENCE))
		{
			EffectPrecedence* pEffectPrecedence = dynamic_cast<EffectPrecedence*>(pTargetMonster->findEffect(Effect::EFFECT_CLASS_PRECEDENCE));
			Assert(pEffectPrecedence != NULL);

			// �켱���� ���� �ڰ� �ƴ϶���...
			if (pAttacker->getName() != pEffectPrecedence->getHostName()) 
			{
				// �ƹ� ��Ƽ���� ���ԵǾ� ���� �ʰų�,
				// ��Ƽ�� ���ԵǾ� �ִµ�, ȣ��Ʈ ��Ƽ ID�� �ƴ϶���, ������ �� ����.
				if (pAttacker->getPartyID() == 0 || pAttacker->getPartyID() != pEffectPrecedence->getHostPartyID())
				{
					return false;
				}
			}
		}
	}
	else Assert(false);
     
	// �������� ���� �� ������ �����Ѵ�.
	if (pAttacker->isVampire())
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);

		ToHit     = pVampire->getToHit();
		VampLevel = pVampire->getLevel();

		ToHit = (ToHit_t)getPercentValue(ToHit, VampireTimebandFactor[timeband]);
	}
	else if (pAttacker->isMonster())
	{
		Monster* pMonster = dynamic_cast<Monster*>(pAttacker);

		ToHit     = pMonster->getToHit();
		VampLevel = pMonster->getLevel();

		ToHit = (ToHit_t)getPercentValue(ToHit, MonsterTimebandFactor[timeband]);
	}

//	cout << pAttacker->getName() << "�� ���� : " << ToHit << endl;
//	cout << pDefender->getName() << "�� ���� : " << Defense << endl;

	if (ToHit >= Defense)
	{
		// ������ ���潺���� ���� ���쿡�� ���� Ȯ���� ��...����.
		ratio = min(90, (ToHit - Defense) / 2 + 70);
	}
	else
	{
		// ������ ���潺���� ���� ���쿡�� ���� Ȯ���� ���� ��������.
		ratio = max(10, 70 - (Defense - ToHit) / 2);
	}

	if ((rand()%100) < ratio)
	{
//		cout << "���� ����" << endl;
		return true;
	}

//	cout << "���� ����" << endl;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ���� ���� ��Ʈ�� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessCurse(int MagicLevel, Resist_t resist)
{
	// MagicLevel�� �����̾ �� ������ ������ �� �ʿ��� �����̴�.
	// �� ���� ������ ���� �����ϼ��� MagicLevel�� ��������.
	//
	// MagicLevel�� 30�̰�, ������ 20�̶���...
	// curse_prob = 110�̰�, ���ִ� �׻� �����ϰ� �ȴ�.
	// MagicLevel�� 30�̰�, ������ 100�̶���...
	// curse_prob = 30�̰�, ������ 70% Ȯ���� �����ϰ� �ȴ�.
#ifdef __CHINA_SERVER__
	int prob_penalty = (int)(MagicLevel - resist);
	int curse_prob   = 65 + prob_penalty;
#else
	int prob_penalty = (int)(MagicLevel*2/1.5 - resist);
	int curse_prob   = 75 + prob_penalty;
	curse_prob = max( 5, curse_prob );
#endif
	int randomValue  = rand()%100;

	// ��, ����. ���� �ɷȴ�.
	if (randomValue < curse_prob) return true;

	// ���׷¿� ���ؼ� �ɸ��� �ʾҴ�. �ƽθ�...
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// ���� ���� ��Ʈ�� �Լ� - �����̾ ���ָ� �ɶ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessVampireCurse(int MagicLevel, Resist_t resist)
{
	// MagicLevel�� �����̾ �� ������ ������ �� �ʿ��� ������ 2�� ����
	// ���̴�. �� ���� ������ ���� �����ϼ��� MagicLevel�� ��������.
	//
	// MagicLevel�� 30�̰�, ������ 20�̶���...
	// curse_prob = 110�̰�, ���ִ� �׻� �����ϰ� �ȴ�.
	// MagicLevel�� 30�̰�, ������ 100�̶���...
	// curse_prob = 30�̰�, ������ 70% Ȯ���� �����ϰ� �ȴ�.
	int prob_penalty = (int)( (int)(MagicLevel/1.5) - resist);
	int curse_prob   = 75 + prob_penalty;
	curse_prob = max( 5, curse_prob );

	int randomValue  = rand()%100;

	// ��, ����. ���� �ɷȴ�.
	if (randomValue < curse_prob) return true;

	// ���׷¿� ���ؼ� �ɸ��� �ʾҴ�. �ƽθ�...
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// CurePoison ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessCurePoison(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio)
{
	int ratio = Base + SkillLevel - Difficulty - MagicLevel;

	// �ּ�Ȯ���� �����־���. by Sequoia 2003. 3. 20
	ratio = max( MinRatio, ratio );

	if (rand()%100 < ratio) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// CurePoison ���߱��� �Լ�
// 2005. 11. 30 ���� �ص� ������ ������ ���� �ٲ۴�. by bezz
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessNewCurePoison( int INTE, int skillLevel, int enemyINT )
{
	int ratio = (int)( ( INTE * 1.4 ) + skillLevel - ( enemyINT / 3 ) );

	if (rand()%100 < ratio)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// CurePoison ���߱��� �Լ� �ƿ콺������
// 2005. 11. 30 ���� �ص� ������ ������ ���� �ٲ۴�. by bezz
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessNewCurePoisonForOusters( int INTE, int skillLevel, int enemyINT )
{
	int max = 0;
	double multiplier = 1.0;

	if ( skillLevel <= 15 )
	{
		max = 60;
	}
	else if ( skillLevel <= 29 )
	{
		max = 70;
	}
	else // 30 ����
	{
		max = 85;
		multiplier = 1.1;
	}

	int ratio = min( max, (int)( ( ( INTE * 1.4 ) + ( skillLevel * 0.6 ) ) * multiplier - ( enemyINT / 3 ) ) );

	if (rand()%100 < ratio)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// Flare ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessFlare(Creature* pTargetCreature, int SkillLevel)
{
	Assert(pTargetCreature != NULL);

	// �������� üũ. by sigi. 2002.9.5
	if (pTargetCreature->isFlag(Effect::EFFECT_CLASS_NO_DAMAGE))
		return false;

	int ratio = 0;

	if (pTargetCreature->isPC())
	{
//		Vampire* pVampire = dynamic_cast<Vampire*>(pTargetCreature);
		ratio = 75 + SkillLevel - pTargetCreature->getLevel();
	}
	else if (pTargetCreature->isMonster())
	{
		Monster* pMonster = dynamic_cast<Monster*>(pTargetCreature);

		// 
		// by sigi. 2002.10.30
		if (pMonster->isMaster()
#ifdef __UNDERWORLD__
				|| pMonster->isUnderworld() || pMonster->getMonsterType() == 599
#endif
		)
		{
			return false;
		}

		ratio = 75 + SkillLevel - pMonster->getLevel();
	}

	if (rand()%100 < ratio) return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// RemoveCurse ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessRemoveCurse(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio)
{
	int ratio = Base + SkillLevel - Difficulty - MagicLevel;

	// �ּ�Ȯ���� �����־���. by Sequoia 2003. 3. 20
	ratio = max( MinRatio, ratio );

	if (rand()%100 < ratio) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// Rebuke ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessRebuke(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, Creature* pDefender)
{
	if( pDefender->isSlayer() ) return false;

	// 80���� �̻��� �����̾��� ������ �ʴ´�.
	if ( pDefender->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pDefender);
		if ( pVampire->getLevel() >= 80 )
			return false;
	}
	else if ( pDefender->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pDefender);
		if ( pOusters->getLevel() >= 80 )
			return false;
	}

	// ������ (���丮, ������)�� ������ �ʴ´�.
	if( pDefender->isMonster() )
	{
		Monster* pMonster = dynamic_cast<Monster*>(pDefender);
		if( pMonster->isMaster()
#ifdef __UNDERWORLD__
				|| pMonster->isUnderworld() || pMonster->getMonsterType() == 599 
#endif
		)
			return false;
	}

	Attr_t			INTE		= pSlayer->getINT(ATTR_CURRENT);
	SkillLevel_t	SkillLevel	= pSkillSlot->getExpLevel();

//	int ratio = ( INTE / 10 ) + ( SkillLevel / 3 );
	int ratio = 20 + ( INTE / 10 ) + ( SkillLevel / 2 );
	if (rand()%100 < ratio ) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// Magic Elusion ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessMagicElusion(Slayer* pSlayer)
{
	Attr_t	SUM		=	pSlayer->getTotalAttr( ATTR_CURRENT );
	int		Ratio	=	50 + (SUM/15);

	return (rand()%100) < Ratio;
}

//////////////////////////////////////////////////////////////////////////////
// Posion Mesh ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessPoisonMesh(Vampire* pVampire)
{
	int		Ratio	=	30 + (pVampire->getLevel()/5);

	return (rand()%100) < Ratio;
}

//////////////////////////////////////////////////////////////////////////////
// Illusion Of Avenge ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessIllusionOfAvenge(Slayer* pSlayer)
{
	Attr_t	SUM		=	pSlayer->getTotalAttr( ATTR_CURRENT );
	int		Ratio	=	50 + (SUM/15);

	return (rand()%100) < Ratio;
}

//////////////////////////////////////////////////////////////////////////////
// Will Of Life ���߱��� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSuccessWillOfLife(Vampire* pVampire)
{
	int		Ratio	=	50 + pVampire->getLevel()/5;

	return (rand()%100) < Ratio;
}

bool HitRoll::isSuccessRAGEOFBLOOD(Vampire* pVampire)
{
	int		Ratio	=	50 + pVampire->getLevel()/5;

	return (rand()%100) < Ratio;
}
bool HitRoll::isSuccessCHAINOFDEMON(Vampire* pVampire)
{
	int		Ratio	=	100;

	return (rand()%100) < Ratio;
}
//////////////////////////////////////////////////////////////////////////////
// ũ��Ƽ�� ��Ʈ ���� �����Ѵ�.
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isCriticalHit(Creature* pCreature, int CriticalBonus)
{
	Assert(pCreature != NULL);

	int CriticalRatio = pCreature->getCriticalRatio() + CriticalBonus;

	if (rand()%1000 < CriticalRatio) return true;

	return false;
}

bool HitRoll::isMagicCriticalHit(Creature* pCreature, int Bonus)
{
	Assert(pCreature != NULL);

	int CriticalRatio = Bonus;

	if (rand()%1000 < CriticalRatio) return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////////
// �����̾� ���� ��ų�ΰ��� üũ�ϴ� �Լ�
//////////////////////////////////////////////////////////////////////////////
bool HitRoll::isSlayerSelfSkill( SkillType_t skillType )
{
	switch ( skillType )
	{
		case SKILL_DANCING_SWORD:
		case SKILL_CROSS_COUNTER:
		case SKILL_EXPANSION:
		case SKILL_SHARP_SHIELD:

		case SKILL_GHOST_BLADE:
		case SKILL_POTENTIAL_EXPLOSION:
		case SKILL_CHARGING_POWER:
		case SKILL_BERSERKER:
		case SKILL_AIR_SHIELD:

		case SKILL_SNIPING:
		case SKILL_OBSERVING_EYE:

		case SKILL_CREATE_HOLY_WATER:
		case SKILL_LIGHT:
		case SKILL_DETECT_HIDDEN:
		case SKILL_BLESS:
		case SKILL_STRIKING:
		case SKILL_DETECT_INVISIBILITY:
		case SKILL_AURA_SHIELD:
		case SKILL_REFLECTION:
		case SKILL_REBUKE:
		case SKILL_SPIRIT_GUARD:

		case SKILL_CURE_LIGHT_WOUNDS:
		case SKILL_CURE_POISON:
		case SKILL_PROTECTION_FROM_POISON:
		case SKILL_CURE_SERIOUS_WOUNDS:
		case SKILL_REMOVE_CURSE:
		case SKILL_PROTECTION_FROM_CURSE:
		case SKILL_CURE_CRITICAL_WOUNDS:
		case SKILL_PROTECTION_FROM_ACID:
		case SKILL_CURE_ALL:
		case SKILL_PEACE:
		case SKILL_ACTIVATION:
		case SKILL_REGENERATION:
		case SKILL_TURN_UNDEAD:
			return true;

		default:
			return false;
	}
}

bool HitRoll::isOustersSelfSkill( SkillType_t skillType )
{
	switch ( skillType )
	{
		case SKILL_EVADE:
		case SKILL_BLUNTING:
		case SKILL_CROSS_GUARD:
		case SKILL_SHARP_CHAKRAM:
			return true;

		default:
			return false;
	}
}
bool HitRoll::isSuccessHallucination( Vampire* pAttacker, Creature* pTarget )
{
	if ( pTarget->isMonster() ) return true;

	Attr_t attackTotalAttr = pAttacker->getSTR() + pAttacker->getDEX() + pAttacker->getINT();
	Attr_t targetTotalAttr = 0;
	int minRatio = 0, maxRatio = 100;

	if ( pTarget->isSlayer() )
	{
		Slayer* pTargetSlayer = dynamic_cast<Slayer*>(pTarget);
		targetTotalAttr = pTargetSlayer->getTotalAttr();
		minRatio = 30;
		maxRatio = 60;
	}
	else if ( pTarget->isVampire() )
	{
		Vampire* pTargetVampire = dynamic_cast<Vampire*>(pTarget);
		targetTotalAttr = pTargetVampire->getSTR() + pTargetVampire->getDEX() + pTargetVampire->getINT();
		minRatio = 10;
		maxRatio = 40;
	}
	else if ( pTarget->isOusters() )
	{
		Ousters* pTargetOusters = dynamic_cast<Ousters*>(pTarget);
		targetTotalAttr = pTargetOusters->getSTR() + pTargetOusters->getDEX() + pTargetOusters->getINT();
		minRatio = 10;
		maxRatio = 50;

	}
	else return false;

	int Ratio = attackTotalAttr - targetTotalAttr;
	Ratio = max( minRatio, Ratio );
	Ratio = min( maxRatio, Ratio );

	return ( rand() % 100 ) < Ratio;
}

bool HitRoll::isSuccessBackStab( Ousters* pAttacker )
{
	int Ratio = min( 50, (pAttacker->getINT()/5) + (pAttacker->getDEX()/5) );
	if ( rand() % 100 < Ratio ) return true;
	return false;
}

#ifdef VERSION_SKILL_1
// 20070919 ���� ���߷� �Լ�
bool HitRoll::isSuccessMagic( Creature* pAttacker, Creature* pDefender, int BonusPoint )
{
	ToHit_t toHit = 0;
	ToHit_t bonusMagicDefense = 0;
	ToHit_t	magicToHit = 0;
	Defense_t magicDefense = 0;

	// ������ �Ǵ� �����ڰ� NULL�� ������ ������... ����!
	if( pAttacker == NULL || pDefender == NULL )
		return false;

	if( pDefender->isSlayer() )
	{
		Slayer* pSlayer = dynamic_cast<Slayer*>(pDefender);
		Assert( pSlayer != NULL );

		if( pSlayer->hasSkill(438) )
		{
			bonusMagicDefense = 30;
		}
		else if( pSlayer->hasSkill(437) )
		{
			bonusMagicDefense = 20;
		}
		else if( pSlayer->hasSkill(436) )
		{
			bonusMagicDefense = 10;
		}
	}
	else if( pDefender->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pDefender);
		Assert( pVampire != NULL );

		if( pVampire->hasSkill(438) )
		{
			bonusMagicDefense = 30;
		}
		else if( pVampire->hasSkill(437) )
		{
			bonusMagicDefense = 20;
		}
		else if( pVampire->hasSkill(436) )
		{
			bonusMagicDefense = 10;
		}
	}
	else if( pDefender->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pDefender);
		Assert( pOusters != NULL );

		if( pOusters->hasSkill(438) )
		{
			bonusMagicDefense = 30;
		}
		else if( pOusters->hasSkill(437) )
		{
			bonusMagicDefense = 20;
		}
		else if( pOusters->hasSkill(436) )
		{
			bonusMagicDefense = 10;
		}
	}

	if( pAttacker->isSlayer() )
	{
		Slayer* pSlayer =  dynamic_cast<Slayer*>(pAttacker);
		Assert( pSlayer != NULL );

		magicToHit = pSlayer->getMagicToHit();
		magicDefense = pDefender->getLevel();

//		toHit = ((magicToHit - magicDefense) + 120 - bonusMagicDefense) * ( 100 + BonusPoint ) / 100;
		toHit = (magicToHit - magicDefense) * 0.5 + 125 - bonusMagicDefense;
		toHit = max( 100 - bonusMagicDefense, (int)toHit );
	}
	else if( pAttacker->isVampire() )
	{
		Vampire* pVampire = dynamic_cast<Vampire*>(pAttacker);
		Assert( pVampire != NULL );

		magicToHit = pVampire->getMagicToHit();
		magicDefense = pDefender->getLevel();

		toHit = ((magicToHit - magicDefense) + 120 - bonusMagicDefense) * ( 100 + BonusPoint ) / 100;
		toHit = max( 100 - bonusMagicDefense, (int)toHit );
	}
	else if( pAttacker->isOusters() )
	{
		Ousters* pOusters = dynamic_cast<Ousters*>(pAttacker);
		Assert( pOusters != NULL );

		magicToHit = pOusters->getMagicToHit();
		magicDefense = pDefender->getLevel();

//		toHit = (magicToHit - magicDefense) * 0.5 + 125 - bonusMagicDefense;
		toHit = ((magicToHit - magicDefense) + 120 - bonusMagicDefense) * ( 100 + BonusPoint ) / 100;
		toHit = max( 100 - bonusMagicDefense, (int)toHit );
	}
	else
	{
		toHit = 100;
	}

	return ( (rand() % 100) + 1) < toHit;
}
#endif
