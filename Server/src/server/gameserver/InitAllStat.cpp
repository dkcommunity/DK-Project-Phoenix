//////////////////////////////////////////////////////////////////////////////
// Filename    : InitAllStat.cpp
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "Monster.h"
#include "GamePlayer.h"
#include "Zone.h"
#include "AbilityBalance.h"
#include "Effect.h"
#include "OptionInfo.h"
#include "ItemUtil.h"
#include "ItemInfo.h"
#include "ItemInfoManager.h"
#include "Player.h"
#include "SkillInfo.h"
#include "CombatInfoManager.h"
#include "VariableManager.h"
#include "RankBonus.h"
#include "HolyLandRaceBonus.h"
#include "BloodBibleBonus.h"
#include "BloodBibleBonusManager.h"
#include "WarSystem.h"
#include "CastleInfoManager.h"
#include "DefaultOptionSetInfo.h"
#include "LevelWarZoneInfoManager.h"
#include "SweeperBonus.h"
#include "SweeperBonusManager.h"
#include "BloodBibleSignInfo.h"

#include "PetInfo.h"
#include "CoreZap.h"

#include "item/OustersWristlet.h"
#include "item/OustersStone.h"
#include "item/CueOfAdam.h"

#include "skill/CastleSkillSlot.h"
#include "skill/VampireCastleSkillSlot.h"
#include "skill/OustersCastleSkillSlot.h"
#include "skill/EffectBless.h"
#include "skill/EffectBless2.h"
#include "skill/EffectDancingSword.h"
#include "skill/EffectGhostBlade.h"
#include "skill/EffectChargingPower.h"
#include "skill/EffectPotentialExplosion.h"
#include "skill/EffectParalyze.h"
#include "skill/EffectDoom.h"
#include "skill/EffectSeduction.h"
#include "skill/EffectProtectionFromPoison.h"
#include "skill/EffectProtectionFromCurse.h"
#include "skill/EffectProtectionFromAcid.h"
#include "skill/EffectProtectionFromBlood.h"
#include "skill/EffectTransformToWolf.h"
#include "skill/EffectTransformToBat.h"
#include "skill/EffectMindControl.h"
#include "skill/EffectExpansion.h"
#include "skill/EffectBerserker.h"
#include "skill/EffectGLADIATOR.h"
#include "skill/EffectStriking.h"
#include "skill/EffectStriking2.h"
#include "skill/EffectExtreme.h"
#include "skill/EffectDeath.h"
#include "skill/EffectMephisto.h"
//#include "skill/EffectRAGEOFBLOOD.h"
#include "skill/EffectObservingEye.h"
#include "skill/EffectSummonCasket.h"
#include "skill/EffectConcealment.h"
#include "skill/EffectEvade.h"
#include "skill/EffectALLYSTERWIND.h"
#include "skill/EffectCrossGuard.h"
#include "skill/EffectBlunting.h"
#include "skill/EffectHandsOfFire.h"
#include "skill/EffectRingOfFlare.h"
#include "skill/EffectWaterBarrier.h"
#include "skill/EffectGnomesWhisper.h"
#include "skill/EffectRediance.h"
#include "skill/EffectHolyArmor.h"
#include "skill/EffectHolyArmor2.h"
#include "skill/EffectTransformToWerwolf.h"
#include "skill/EffectReactiveArmor.h"
#include "skill/EffectGroundBless.h"
#include "skill/EffectSharpChakram.h"
#include "skill/EffectWhitsuntide.h"
#include "skill/EffectIntimateGrail.h"
#include "skill/EffectIntimateGrail2.h"
#include "skill/EffectPartyAura.h"
#include "skill/EffectBloodsSymposionAttack.h"
#include "skill/EffectBloodyShout.h"
#include "skill/EffectADV2TRUECLOLOROFVAMPLOAD.h"
#include "skill/EffectADV2THEROAROFAWAKENWARRIOR.h"
#include "skill/EffectADV2THERAGEOFGUARDIANSPRIT.h"
#include "skill/EffectWOLFROAR.h"
#include "skill/EffectBREATHOFDRYAD.h"

#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCAddEffect.h"
#include "Gpackets/GCRemoveEffect.h"
#include "Gpackets/GCChangeShape.h"
#include "Gpackets/GCBloodBibleSignInfo.h"
#include "BloodBibleSignInfo.h"
#include "ContractOfBloodUtil.h"

#include "skill/EffectBloodCurse.h"
#include "EffectMysteryPotion.h"
#include "EffectDragonEye.h"
#include "EffectSlayerHeroItem.h"
#include "EffectSlayerLevelWarHeroItem.h"
#include "EffectVampireHeroItem.h"
#include "EffectVampireLevelWarHeroItem.h"
#include "EffectOustersHeroItem.h"
#include "EffectOustersLevelWarHeroItem.h"

#include "EffectSquallyBarrier1.h"
#include "EffectSquallyBarrier2.h"
#include "EffectRADCHIA.h"
#include "EffectADV2CYCLINGWARROCK.h"
#include "EffectEVGENEIABELLUM.h"
#include "EffectFAVOROFLUNA.h"
#include "skill/EffectOUTRAGEBOOSTER2.h"
#include "skill/EffectADV2GRACEOFELQUINES.h"
#include "NewRankGem.h"
#include "skill/EffectADV2StealInt.h"
#include "skill/EffectTaoZhuang.h"
#include "skill/EffectWaterBarrier2.h"
#include "skill/EffectRingOfFlare2.h"
#include "skill/EffectReactiveArmor2.h"
#include "skill/EffectPotentialExplosion2.h"
//////////////////////////////////////////////////////////////////////////////
//
// ����
//
//////////////////////////////////////////////////////////////////////////////
void PlayerCreature::applyBloodBibleSign()
{
	int openNum = getBloodBibleSignOpenNum();
	getBloodBibleSign()->setOpenNum(openNum);
//	int openNum = getBloodBibleSign()->getOpenNum();

	int applyCount = 0;
	vector<ItemType_t>::iterator bItr = getBloodBibleSign()->getList().begin();
	for ( ; bItr != getBloodBibleSign()->getList().end(); ++bItr )
	{
		if ( applyCount >= openNum ) break;
		BloodBibleBonus* pBonus = g_pBloodBibleBonusManager->getBloodBibleBonus( *bItr );
		if ( pBonus != NULL )
		{
			OptionTypeList optionTypes = pBonus->getOptionTypeList();
			OptionTypeListConstItor optionItr;
//			cout << getName() << "���� " << pBonus->getName() << " �� ����մϴ�." << endl;
			++applyCount;
			
			for ( optionItr = optionTypes.begin(); optionItr != optionTypes.end(); optionItr++ )
			{
				computeOptionStat( *optionItr );
			}
		}
	}

}

//////////////////////////////////////////////////////////////////////////////
//
// �����̾�
//
//////////////////////////////////////////////////////////////////////////////

void Slayer::initCastleSkill() throw()
{
	__BEGIN_TRY

	removeAllCastleSkill();

	if ( !getZone()->isHolyLand() ) return;

	list<CastleInfo*> pCastleInfoList = g_pCastleInfoManager->getGuildCastleInfos( getGuildID() );
	if ( pCastleInfoList.empty() ) return;

	list<CastleInfo*>::iterator itr = pCastleInfoList.begin();

	for ( ; itr != pCastleInfoList.end() ; itr++ )
	{
		SkillType_t CastleSkillType = g_pCastleInfoManager->getCastleSkillType( (*itr)->getZoneID(), getGuildID() );
		if ( CastleSkillType == SKILL_MAX ) 
			continue;

		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(CastleSkillType);
		Assert( pSkillInfo != NULL );

		Turn_t Delay = pSkillInfo->getMaxDelay();

		CastleSkillSlot* pCastleSkillSlot = new CastleSkillSlot();

		pCastleSkillSlot->setName( m_Name );
		pCastleSkillSlot->setSkillType( CastleSkillType );
		pCastleSkillSlot->setInterval(Delay);
		pCastleSkillSlot->setExpLevel(0);
		pCastleSkillSlot->setExp(1);
		pCastleSkillSlot->setRunTime();

		addSkill( pCastleSkillSlot );
	}

	__END_CATCH
}

void Slayer::initAllStat(int numPartyMember) throw()
{
	__BEGIN_TRY

	BASIC_ATTR attr;
	Creature::CreatureClass CClass = getCreatureClass();

	m_Resist[MAGIC_DOMAIN_NO_DOMAIN] = 0;
	m_Resist[MAGIC_DOMAIN_POISON]    = 0;
	m_Resist[MAGIC_DOMAIN_ACID]      = 0;
	m_Resist[MAGIC_DOMAIN_CURSE]     = 0;
	m_Resist[MAGIC_DOMAIN_BLOOD]     = 0;
////////////////////////////////////////////////////////////////////////
//	������ ��� ���� �ʱ�ȭ	
	m_Resist[MAGIC_DOMAIN_ALLRES]    = 0;

	// BloodBible ��� ���ʽ� ��ġ�� �ʱ�ȭ
	m_ConsumeMPRatio	= 0;
	m_GamblePriceRatio	= 0;
	m_PotionPriceRatio	= 0;
	m_MagicBonusDamage	= 0;
	m_PhysicBonusDamage	= 0;
	m_MagicDamageReduce = 0;
	m_PhysicDamageReduce= 0;

	//////////////////////////////////////////////////////////////////////////////
	// ���� ���� �⺻ �ɷ�ġ�� �ʱ�ȭ��Ű��, 
	// �⺻ �ɷ�ġ�� ������ �ִ� ����Ʈ�� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	m_STR[ATTR_CURRENT] = m_STR[ATTR_MAX] = m_STR[ATTR_BASIC] = m_pAttrs[ATTR_KIND_STR]->getLevel();
	m_DEX[ATTR_CURRENT] = m_DEX[ATTR_MAX] = m_DEX[ATTR_BASIC] = m_pAttrs[ATTR_KIND_DEX]->getLevel();
	m_INT[ATTR_CURRENT] = m_INT[ATTR_MAX] = m_INT[ATTR_BASIC] = m_pAttrs[ATTR_KIND_INT]->getLevel();

	//20070911
	m_STR[ATTR_CURRENT] += m_AdvancedSTR[ATTR_BASIC];
	m_DEX[ATTR_CURRENT] += m_AdvancedDEX[ATTR_BASIC];
	m_INT[ATTR_CURRENT] += m_AdvancedINT[ATTR_BASIC];
	m_AdvancedSTR[ATTR_CURRENT] = m_AdvancedSTR[ATTR_BASIC];
	m_AdvancedDEX[ATTR_CURRENT] = m_AdvancedDEX[ATTR_BASIC];
	m_AdvancedINT[ATTR_CURRENT] = m_AdvancedINT[ATTR_BASIC];

	if (isFlag(Effect::EFFECT_CLASS_BLESS))
	{
		EffectBless* pBless = dynamic_cast<EffectBless*>(findEffect(Effect::EFFECT_CLASS_BLESS));
		if (pBless != NULL)
		{
			// STR, DEX�� �ø���.
//			m_STR[ATTR_CURRENT] += pBless->getSTRBonus();
//			m_DEX[ATTR_CURRENT] += pBless->getDEXBonus();
			//int bbSTR = pBless->getSTRBonus();
			//int bbDEX = pBless->getDEXBonus();
		//	int bbINT = pBless->getINTBonus();
			//cout << "Bless ���ʽ� �� - STR = " << bbSTR << ", DEX = " << bbDEX << ", INT = " << bbINT << endl; 
			//cout << "BLESSȿ�� ����� - STR = " << m_STR[ATTR_CURRENT] << ", DEX = " << m_DEX[ATTR_CURRENT] << ", INT = " << m_INT[ATTR_CURRENT] << endl;
			m_STR[ATTR_CURRENT] += getPercentValue( m_STR[ATTR_CURRENT], pBless->getSTRBonus() );
			m_DEX[ATTR_CURRENT] += getPercentValue( m_DEX[ATTR_CURRENT], pBless->getDEXBonus() );
			//2008.1202 wlzzi - ���׼���
			m_INT[ATTR_CURRENT] += getPercentValue( m_INT[ATTR_CURRENT], pBless->getINTBonus() );
			//cout << "BLESSȿ�� ����� - STR =" << m_STR[ATTR_CURRENT] << ", DEX = " << m_DEX[ATTR_CURRENT] << ", INT = " << m_INT[ATTR_CURRENT] << endl;
			//-
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_BLESS_2))
	{
		EffectBless2* pBless2 = dynamic_cast<EffectBless2*>(findEffect(Effect::EFFECT_CLASS_BLESS_2));
		if (pBless2 != NULL)
		{
			//int bbSTR = pBless2->getSTRBonus();
			//int bbDEX = pBless2->getDEXBonus();
			//int bbINT = pBless2->getINTBonus();
			//cout << "Bless2 ���ʽ� �� - STR = " << bbSTR << ", DEX = " << bbDEX << ", INT = " << bbINT << endl; 
			//cout << "BLESS�2��� ����� - STR = " << m_STR[ATTR_CURRENT] << ", DEX = " << m_DEX[ATTR_CURRENT] << ", INT = " << m_INT[ATTR_CURRENT] << endl;
			m_STR[ATTR_CURRENT] += getPercentValue( m_STR[ATTR_CURRENT], pBless2->getSTRBonus() );
			m_DEX[ATTR_CURRENT] += getPercentValue( m_DEX[ATTR_CURRENT], pBless2->getDEXBonus() );

			m_INT[ATTR_CURRENT] += getPercentValue( m_INT[ATTR_CURRENT], pBless2->getINTBonus() );
			//cout << "BLESS�2��� ����� - STR =" << m_STR[ATTR_CURRENT] << ", DEX = " << m_DEX[ATTR_CURRENT] << ", INT = " << m_INT[ATTR_CURRENT] << endl;
			//-
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_ADV2StealInt))
	{
		EffectADV2StealInt* pStealInt = dynamic_cast<EffectADV2StealInt*>(findEffect(Effect::EFFECT_CLASS_ADV2StealInt));
		if (pStealInt != NULL)
		{

			int bINT = pStealInt->getReflection();

			m_INT[ATTR_CURRENT] += bINT;
			cout << "EffectADV2StealInt - INTE =" << m_INT[ATTR_CURRENT] <<endl;
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_POTENTIAL_EXPLOSION))
	{
		EffectPotentialExplosion* pPotentialExplosion = dynamic_cast<EffectPotentialExplosion*>(findEffect(Effect::EFFECT_CLASS_POTENTIAL_EXPLOSION));
		if (pPotentialExplosion!= NULL)
		{
			// STR, DEX�� �ø���.
			m_STR[ATTR_CURRENT] += pPotentialExplosion->getDiffSTR();
			m_DEX[ATTR_CURRENT] += pPotentialExplosion->getDiffDEX();
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_POTENTIAL_EXPLOSION2))
	{
		EffectPotentialExplosion2* pPotentialExplosion = dynamic_cast<EffectPotentialExplosion2*>(findEffect(Effect::EFFECT_CLASS_POTENTIAL_EXPLOSION2));
		if (pPotentialExplosion!= NULL)
		{
			m_STR[ATTR_CURRENT] += pPotentialExplosion->getDiffSTR();
			m_DEX[ATTR_CURRENT] += pPotentialExplosion->getDiffDEX();
		}
	}
//	20070917
/*	if (isFlag(Effect::EFFECT_CLASS_REDIANCE))
	{
		EffectRediance* pRediance = dynamic_cast<EffectRediance*>(findEffect(Effect::EFFECT_CLASS_REDIANCE));
		if (pRediance!= NULL)
		{
			Item* pWeapon = m_pWearItem[Slayer::WEAR_RIGHTHAND];
			if ( pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_SWORD || !isRealWearing( WEAR_RIGHTHAND ) )
			{
				pRediance->setDeadline(0);
			}
			else
			{
				m_DEX[ATTR_CURRENT] += pRediance->getDexBonus();
			}
		}
	}*/

	//////////////////////////////////////////////////////////////////////////////
	// �ɷ�ġ ����� ���� �Ķ���͵��� �ʱ�ȭ�Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	attr.nSTR    = m_STR[ATTR_CURRENT];
	attr.nDEX    = m_DEX[ATTR_CURRENT];
	attr.nINT    = m_INT[ATTR_CURRENT];
	attr.pWeapon = m_pWearItem[WEAR_RIGHTHAND];

	m_HPStealAmount = 0;
	m_MPStealAmount = 0;
	m_HPStealRatio  = 0;
	m_MPStealRatio  = 0;
	m_HPRegen       = 0;
	m_MPRegen       = 0;
	m_Luck			= m_BaseLuck;
//	cout << getName() << "�� �⺻ ��� : " << m_Luck << endl;

	for (int i=0; i<SKILL_DOMAIN_MAX; i++)
		attr.pDomainLevel[i] = m_SkillDomainLevels[i];

	//////////////////////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ���� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	//m_HP[ATTR_MAX]                = computeHP(CClass, &attr);
	m_HP[ATTR_MAX]                = computeTestModeHP(this, &attr);
	m_HP[ATTR_BASIC]              = 0;
	m_MP[ATTR_MAX]                = computeMP(CClass, &attr);
	m_MP[ATTR_BASIC]              = 0;
	m_ToHit[ATTR_CURRENT]         = computeToHit(CClass, &attr);
	m_ToHit[ATTR_MAX]             = 0;
	m_Defense[ATTR_CURRENT]       = computeDefense(CClass, &attr);
	m_Defense[ATTR_MAX]           = 0;
	m_Protection[ATTR_CURRENT]    = computeProtection(CClass, &attr);
	m_Protection[ATTR_MAX]        = 0;
	m_Damage[ATTR_CURRENT]        = computeMinDamage(CClass, &attr);
	m_Damage[ATTR_MAX]            = computeMaxDamage(CClass, &attr);
	m_Damage[ATTR_BASIC]          = 0;
	setAttackSpeed(computeAttackSpeed(CClass, &attr), ATTR_CURRENT);
	setAttackSpeed(0, ATTR_MAX);
	m_CriticalRatio[ATTR_CURRENT] = computeCriticalRatio(CClass, &attr);
	m_CriticalRatio[ATTR_MAX]     = 0;
	// 20070918
	m_MagicDefense				  = computeMagicDefense(CClass, &attr);
	m_MagicToHit				  = computeMagicToHit(CClass, &attr);

	//////////////////////////////////////////////////////////////////////////////
	// �ɷ�ġ�� ���� Damage �� ������ �д�. BERSER ����� ���ʽ� ����� ���ؼ�.
	//////////////////////////////////////////////////////////////////////////////
	Damage_t AttrMinDamage = m_Damage[ATTR_CURRENT];
	Damage_t AttrMaxDamage = m_Damage[ATTR_MAX];

	int DefBonus = 0;
	int ProBonus = 0;

	int RaceWarHPBonus = 0;

	if ( m_pZone->isHolyLand() || m_pZone->isLevelWarZone() )
	{
		RaceWarHPBonus = getPercentValue( m_HP[ATTR_MAX], g_pVariableManager->getRaceWarHPBonus() );
	}

	int DragonEyeHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_DRAGON_EYE ) )
	{
		// HP ���ʽ��� �ι�
		DragonEyeHPBonus = m_HP[ATTR_MAX];
	}

#ifdef __CONTRIBUTION_SYSTEM__
	int HeroItemHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_SLAYER_HERO_ITEM ) )
	{
		// HP ���ʽ��� �ټ��� 
        	  HeroItemHPBonus = m_HP[ATTR_MAX] * 2; // by svi 4->2
	}

	int LevelHeroItemHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_SLAYER_LEVELWAR_HERO_ITEM ) )
	{
		// HP ���ʽ��� �ټ��� 
		LevelHeroItemHPBonus = m_HP[ATTR_MAX] * 2;
	}
#endif

	// Passive Skill : Will of Iron (��� HP * 1.15)
	SlayerSkillSlot* pFabulousSoul = getSkill(SKILL_FABULOUS_SOUL);
	SlayerSkillSlot* pWillOfIron = getSkill(SKILL_WILL_OF_IRON);

	int HPBonus_WillOfIron = 0;

	if( (pFabulousSoul != NULL && pFabulousSoul->canUse()) ||	
		(pWillOfIron != NULL && pWillOfIron->canUse()) )
	{
		HPBonus_WillOfIron = m_HP[ATTR_MAX] * 0.15;	
	}

	SlayerSkillSlot* pLiveness = getSkill(SKILL_LIVENESS);
	int HPBonusPercent = 0,
		LivenessHPBonus =0,
		LivenessDefenseBonus = 0;
		
	if(pLiveness != NULL && pLiveness->canUse())
	{
		int level = m_SkillDomainLevels[SKILL_DOMAIN_GUN];

#ifdef __CHINA_SERVER__
		switch (g_pSkillInfoManager->getGradeByDomainLevel(level))
		{
			case SKILL_GRADE_APPRENTICE:   HPBonusPercent = 0; 
										   LivenessDefenseBonus = 0;
										   break;
			case SKILL_GRADE_ADEPT:        HPBonusPercent = 10;
										   LivenessDefenseBonus = 10;
										   break;
			case SKILL_GRADE_EXPERT:       HPBonusPercent = 25;
										   LivenessDefenseBonus = 35;
										   break;
			case SKILL_GRADE_MASTER:       HPBonusPercent = 40;
										   LivenessDefenseBonus = 60;
										   break;
			case SKILL_GRADE_GRAND_MASTER: HPBonusPercent = 100;
										   LivenessDefenseBonus = 100;
										   break;
			default:                       break;
		}

//		if ( level >= 125 ) HPBonusPercent = 50;
#else
		switch (g_pSkillInfoManager->getGradeByDomainLevel(level))
		{
			case SKILL_GRADE_APPRENTICE:   HPBonusPercent = 0; 
										   LivenessDefenseBonus = 0;
										   break;
			case SKILL_GRADE_ADEPT:        //HPBonusPercent = 10;
										   HPBonusPercent = 15;
										   LivenessDefenseBonus = 10;
										   break;
			case SKILL_GRADE_EXPERT:       //HPBonusPercent = 20;
										   HPBonusPercent = 20;
										   LivenessDefenseBonus = 35;
										   break;
			case SKILL_GRADE_MASTER:       //HPBonusPercent = 30;
										   HPBonusPercent = 25;
										   LivenessDefenseBonus = 60;
										   break;
			case SKILL_GRADE_GRAND_MASTER: //HPBonusPercent = 40;
										   HPBonusPercent = 30;
										   LivenessDefenseBonus = 100;
										   break;
			default:                       break;
		}

		//if ( level >= 125 ) HPBonusPercent = 50;
		if ( level >= 125 ) HPBonusPercent = 30;
#endif

		LivenessHPBonus = getPercentValue( m_HP[ATTR_MAX], HPBonusPercent);
		//m_Defense[ATTR_CURRENT]  = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + LivenessDefenseBonus);
	}


	////	// ���� ���ʽ�
	// ������ ���� ���п� ������ ������̵� ���ʽ��� ���� �� �ִ�. by sigi
	//if ( g_pCombatInfoManager->isSlayerBonus() )
	int HPBonus = 0;
	{
		int bonusRatio = g_pVariableManager->getCombatSlayerHPBonusRatio();
		//g_pCombatInfoManager->getSlayerHPModify();

		if (bonusRatio > 0)
		{
			HPBonus = getPercentValue( m_HP[ATTR_MAX], bonusRatio);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// �ϴ� ��� üũ ������ �ʱ�ȭ�ؼ� ��� ��� �� ���� ������ �����ϰ� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	bool pOldRealWearingCheck[WEAR_MAX];	// by sigi. 2002.10.31
	for (int i=0; i<WEAR_MAX; i++) 
	{
		pOldRealWearingCheck[i] = m_pRealWearingCheck[i];
		m_pRealWearingCheck[i] = false;
	}

	//////////////////////////////////////////////////////////////////////////////
	// ���� ������ ������ ���ʽ� �ɼ��� �ް� �ȴ�
	//////////////////////////////////////////////////////////////////////////////
	// Blood Bible ������ ���ʽ� �ɼ��� �޴� �ɷ� ���ƴ�.
/*	if (m_pZone->isHolyLand() )
	{
		const list<OptionType_t>& optionType = g_pHolyLandRaceBonus->getSlayerOptionTypeList();
		list<OptionType_t>::const_iterator itr;
		for (itr=optionType.begin(); itr!=optionType.end(); itr++)
		{
			computeOptionStat( *itr );
		}
	}*/

	//////////////////////////////////////////////////////////////////////////////
	// Blood Bible ������ ���ʽ� �ɼ��� �޴´�.
	//////////////////////////////////////////////////////////////////////////////
/*	if ( m_pZone->isHolyLand() && !g_pWarSystem->hasActiveRaceWar() )
	{
		const BloodBibleBonusHashMap& bloodBibleBonus = g_pBloodBibleBonusManager->getBloodBibleBonuses();
		BloodBibleBonusHashMapConstItor itr;
		for (itr=bloodBibleBonus.begin(); itr!=bloodBibleBonus.end(); itr++)
		{
			if ( itr->second->getRace() == RACE_SLAYER )
			{
				OptionTypeList optionTypes = itr->second->getOptionTypeList();
				OptionTypeListConstItor optionItr;
				
				for ( optionItr = optionTypes.begin(); optionItr != optionTypes.end(); optionItr++ )
				{
					computeOptionStat( *optionItr );
				}
			}
		}
	}*/

	if ( g_pSweeperBonusManager->isAble( getZoneID() ) && g_pLevelWarZoneInfoManager->isCreatureBonusZone( this, getZoneID() ))
//	2007 01 03
//	if ( g_pLevelWarZoneInfoManager->isCreatureBonusZone( this, getZoneID() ))
	{
		const SweeperBonusHashMap& sweeperBonuses = g_pSweeperBonusManager->getSweeperBonuses();

		SweeperBonusHashMapConstItor itr = sweeperBonuses.begin();
		SweeperBonusHashMapConstItor endItr = sweeperBonuses.end();

		for (; itr!=endItr; itr++)
		{
			if ( itr->second->getRace() == RACE_SLAYER && itr->second->getLevel() == g_pLevelWarZoneInfoManager->getCreatureLevelGrade( this ) )
			{
				OptionTypeList optionTypes = itr->second->getOptionTypeList();
				OptionTypeListConstItor optionItr;
				
				for ( optionItr = optionTypes.begin(); optionItr != optionTypes.end(); optionItr++ )
				{
					computeOptionStat( *optionItr );
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////
	// �⺻������ ������ �ִ� �ɼǵ��� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	list<DefaultOptionSetType_t>::iterator itr = m_DefaultOptionSet.begin();
	for ( ; itr != m_DefaultOptionSet.end(); itr++ )
	{
		DefaultOptionSetInfo* pDefaultOptionSetInfo = g_pDefaultOptionSetInfoManager->getDefaultOptionSetInfo( (*itr) );
		if ( pDefaultOptionSetInfo != NULL )
		{
			const list<OptionType_t>& optionList = pDefaultOptionSetInfo->getOptionTypeList();
			list<OptionType_t>::const_iterator citr;
			for ( citr = optionList.begin(); citr != optionList.end(); citr++ )
			{
				computeOptionStat( *citr );
			}
		}
	}

	list<OptionType_t>::iterator oitr = m_DefaultOptions.begin();
	list<OptionType_t>::iterator oend = m_DefaultOptions.end();
	for ( ; oitr != oend; ++oitr )
	{
		computeOptionStat( *oitr );
	}

	HashMapObjectOptionConstItor ooitr = m_EffectOptions.begin();
	HashMapObjectOptionConstItor ooendItr = m_EffectOptions.end();

	for ( ; ooitr != ooendItr; ++ooitr )
	{
		computeOptionStat( ooitr->second );
	}

	//////////////////////////////////////////////////////////////////////////////
	// ���� �ִ� ���ʽ��� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	if ( m_pPetInfo != NULL )
	{
		if ( m_pPetInfo->getPetAttr() != 0xff ) computeOptionClassStat( (OptionClass)m_pPetInfo->getPetAttr(), (int)m_pPetInfo->getPetAttrLevel() );
		if ( m_pPetInfo->getPetOption() != 0 ) computeOptionStat( m_pPetInfo->getPetOption() );
		
		//1203 ~ 1204 wlzzi Pet's OptionType �߰�
		const list<OptionType_t>& optionList = m_pPetInfo->getOptionType();
		list<OptionType_t>::const_iterator itr;
		for ( itr = optionList.begin(); itr != optionList.end(); itr++ )
		{
			computeOptionStat( *itr );
		} 
		if ( m_pPetInfo->getPetOption2() != 0 ) computeOptionStat( m_pPetInfo->getPetOption2() );
		if ( m_pPetInfo->getPetOption3() != 0 ) computeOptionStat( m_pPetInfo->getPetOption3() );
		//-
		//20090109 wlzzi - Ǫ��������
		switch (m_pPetInfo->getFunctionPetfoodType ())
		{
		case FOOD_FUNCTION_LUCKY:
			computeOptionClassStat ((OptionClass)21, 10);	//���+10 .. ���� �� ������.. �Ѥ�;�۾����� �ϴ� �ּ�ȭ �Ѵ�.
			break;
		}
		//-
		
		const list<OptionType_t>& MixoptionList = m_pPetInfo->getMixOptionType();
		list<OptionType_t>::const_iterator itr2;
		for ( itr2 = MixoptionList.begin(); itr2 != MixoptionList.end(); itr2++ )
		{
			computeOptionStat( *itr2 );
		} 
	}

	/*if ( hasRankBonus( RankBonus::RANK_BONUS_PERCEPTION ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_PERCEPTION );
		Assert( pRankBonus != NULL );

		computeOptionClassStat( OPTION_ALL_ATTR, pRankBonus->getPoint() );
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_STONE_OF_SAGE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_STONE_OF_SAGE );
		Assert( pRankBonus != NULL );

		computeOptionClassStat( OPTION_INT, pRankBonus->getPoint() );
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_FOOT_OF_RANGER ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_FOOT_OF_RANGER );
		Assert( pRankBonus != NULL );

		computeOptionClassStat( OPTION_DEX, pRankBonus->getPoint() );
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_WARRIORS_FIST ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_WARRIORS_FIST );
		Assert( pRankBonus != NULL );

		computeOptionClassStat( OPTION_STR, pRankBonus->getPoint() );
	}*/
	for(int i=NewRankGem::SMALL_RED_STR;i<NewRankGem::SKILL_BLADE_STORM_DAM;i++)
   	{
		if(hasRankGem((NewRankGem::RankGemType)i))
		{
			NewRankGem* pRankGem = getRankGem((NewRankGem::RankGemType)i);
			Assert( pRankGem != NULL );
			computeOptionClassStat(pRankGem->getOptionClass(), pRankGem->getGrade());
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_TaoZhuang))//s
	{
		EffectTaoZhuang* pEffect = dynamic_cast<EffectTaoZhuang*>(findEffect(Effect::EFFECT_CLASS_TaoZhuang));
		if (pEffect!= NULL)
		{

			int tz211[6][2] =
				{
					{23,20},//+5
					{3,5000},//HP+1000
					{32,30},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz221[6][2] =
				{
					{23,30},//+5
					{3,6000},//HP+1000
					{32,40},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz231[6][2] =
				{
					{23,40},//+5
					{3,7000},//HP+1000
					{32,50},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz241[6][2] =
				{
					{23,50},//+5
					{3,8000},//HP+1000
					{32,60},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz251[6][2] =
				{
					{23,60},//+5
					{3,9000},//HP+1000
					{32,70},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int TzGourp = pEffect->getTzGroup();
			int Num 	= pEffect->getNum();
				for(int i = 0;i<(Num-1);i++)
				{
					if(tz211[i][0]>0 && TzGourp == 1)
					{
					  computeOptionClassStat ((OptionClass)tz211[i][0], tz211[i][1]);
					}
					if(tz221[i][0]>0 && TzGourp == 2)
					{
					  computeOptionClassStat ((OptionClass)tz221[i][0], tz221[i][1]);
					}
					if(tz231[i][0]>0 && TzGourp == 3)
					{
					  computeOptionClassStat ((OptionClass)tz231[i][0], tz231[i][1]);
					}
					if(tz241[i][0]>0 && TzGourp == 4)
					{
					  computeOptionClassStat ((OptionClass)tz241[i][0], tz241[i][1]);
					}
					if(tz251[i][0]>0 && TzGourp == 5)
					{
					  computeOptionClassStat ((OptionClass)tz251[i][0], tz251[i][1]);
					}
				}
		}
	}
	if ( isFlag(Effect::EFFECT_CLASS_STRENGTHEN_INCENSE) )
	{
		computeOptionClassStat( OPTION_ALL_ATTR, 3 );
		HPBonus += 20;
	}

	// ���� ���� ��� ���
	applyBloodBibleSign();
	CheckValidWearingItem();
	// 2006.09.08 by Shinobi
	//list<OptionType_t> ContractOptionList = getContractOfBloodKindByItemType( (PlayerCreature*)this );
	list<OptionType_t> ContractOptionList = getContractOfBloodPositionByItemType( getInventory() );
	list<OptionType_t>::iterator ContractOptionIter = ContractOptionList.begin();

	for(; ContractOptionIter != ContractOptionList.end(); ++ContractOptionIter) 
	{
		computeOptionStat(*ContractOptionIter);
	}

	// by sigi. 2002.11.6
	bool bSendPacket = false;

	if (m_pPlayer!=NULL)
	{	
		bSendPacket = (dynamic_cast<GamePlayer*>(m_pPlayer)->getPlayerStatus()==GPS_NORMAL);
	}

	SendChangeShapes(bSendPacket);

	///////////////////////////////////////////////////////////////////////////////
	// ��� ���ʽ��� ����Ѵ�.
	///////////////////////////////////////////////////////////////////////////////
	// ��ƿ Ȯ���� ����ϱ� �� �̸� ����Ѵ�.
	///////////////////////////////////////////////////////////////////////////////
	/*if ( hasRankBonus( RankBonus::RANK_BONUS_WIGHT_HAND ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_WIGHT_HAND );
		Assert( pRankBonus != NULL );

		int StealBonus = pRankBonus->getPoint();

		m_HPStealAmount += StealBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_SEIREN_HAND ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SEIREN_HAND );
		Assert( pRankBonus != NULL );

		int StealBonus = pRankBonus->getPoint();

		m_MPStealAmount += StealBonus;
	}*/

	//////////////////////////////////////////////////////////////////////////////
	// HP, MP ��ƿ Ȯ���� ����� �д�.
	//////////////////////////////////////////////////////////////////////////////
	m_HPStealRatio = computeStealRatio(CClass, m_HPStealAmount, &attr);
	m_MPStealRatio = computeStealRatio(CClass, m_MPStealAmount, &attr);

	Item* pWeapon = m_pWearItem[Slayer::WEAR_RIGHTHAND];
	Item* pShield = m_pWearItem[Slayer::WEAR_LEFTHAND];

	// 2006.06.19 ��̼ҵ�, ����Ŀ, ��¡�Ŀ�, �?Ʈ �?�̵�, ���͸� �׷���, ��Ʈ����ŷ
	// STR + 15
	if( isFlag(Effect::EFFECT_CLASS_GREYFELL_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_STR, 15 );
	}
	// DEX + 15
	if( isFlag(Effect::EFFECT_CLASS_SOVIUS_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_DEX, 15 );
	}
	// INT + 15
	if( isFlag(Effect::EFFECT_CLASS_MIR_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_INT, 15 );
	}
	// 20081028 wlzzi - Extreme Force Scroll ������
	if ( isFlag(Effect::EFFECT_CLASS_EXTREME_FORCE_SCROLL))
	{
		computeOptionClassStat (OPTION_ALL_ATTR, 8);
	}
	// 2007 01 16 �̽��׸� ���� ����Ʈ ��� �߰�
	if (isFlag(Effect::EFFECT_CLASS_MYSTERY_POTION))
	{
		EffectMysteryPotion* pEffect = dynamic_cast<EffectMysteryPotion*>(findEffect(Effect::EFFECT_CLASS_MYSTERY_POTION));
		if( pEffect != NULL )
		{
			computeOptionStat( 185 );		// ��� + 3
			HP_t bonusHP = 50;
			Defense_t bonusDef = 10; 
			m_Defense[ATTR_MAX]  = max( 0, m_Defense[ATTR_MAX] + bonusDef );
			m_Defense[ATTR_CURRENT]  = m_Defense[ATTR_MAX];
			m_HP[ATTR_MAX]		 = max((HP_t)0, (HP_t)(m_HP[ATTR_MAX] + bonusHP ));
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ�� ���� �����ϴ� ����Ʈ�� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////

	if ( isFlag( Effect::EFFECT_CLASS_CONCEALMENT ) )
	{
		EffectConcealment* pEffect = dynamic_cast<EffectConcealment*>( findEffect(Effect::EFFECT_CLASS_CONCEALMENT) );
		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || !isArmsWeapon( attr.pWeapon ) )
			{
				pEffect->setDeadline(0);
			}
			else if ( isRealWearing( WEAR_RIGHTHAND ) )
			{
				DefBonus = (int)( (getDEX()/20) * ( 1.0f + ((float)pEffect->getLevel()/25.0f) ) );
				ProBonus = (int)( (getSTR()/10) * ( 1.0f + ((float)pEffect->getLevel()/25.0f) ) );
			}
		}
	}
	
	// 20070917 ��ġ�� �����Ͽ���.
	if (isFlag(Effect::EFFECT_CLASS_REDIANCE))
	{
		EffectRediance* pRediance = dynamic_cast<EffectRediance*>(findEffect(Effect::EFFECT_CLASS_REDIANCE));
		if (pRediance!= NULL)
		{
			Item* pWeapon = m_pWearItem[Slayer::WEAR_RIGHTHAND];
			if ( pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_SWORD || !isRealWearing( WEAR_RIGHTHAND ) )
			{
				pRediance->setDeadline(0);
			}
			else
			{
				m_DEX[ATTR_CURRENT] += pRediance->getDexBonus();
				computeStatOffset();
			}
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_STRIKING))
	{
		EffectStriking* pStriking = dynamic_cast<EffectStriking*>(findEffect(Effect::EFFECT_CLASS_STRIKING));
		if (pStriking != NULL)
		{
			Damage_t   DamageBonus  = pStriking->getDamageBonus();

			if (pWeapon != NULL && pStriking->isTargetItem( pWeapon ) )
			{
				m_Damage[ATTR_CURRENT] = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + DamageBonus);
				m_Damage[ATTR_MAX]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + DamageBonus);

				// ItemOID�� ��ġ�Ѵٴ� ���� ��� ��Ʈ����ŷ�� �ɷȰų�,
				// �ٸ� ���⸦ ���ٰ�, �ٽ� ��Ʈ����ŷ�� �ɸ� ���⸦
				// ���ٴ� ���̴�. �׷��Ƿ� ����Ʈ�� ���̶�� ������� �Ѵ�.
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(m_ObjectID);
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_STRIKING);
				gcAddEffect.setDuration(pStriking->getRemainDuration());
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
			}
			else
			{
				// ItemOID�� ��ġ���� �ʴ´ٴ� ���� ��Ʈ����ŷ�� �ɸ� ���¿���
				// �ٸ� ���⸦ ���ٴ� ���̴�. �׷��Ƿ� ������ �����̾�Դ�
				// ��Ʈ����ŷ�� �ɷ��ִ�. �� ����Ʈ�� ��������� �ϹǷ�...
				GCRemoveEffect gcRemoveEffect;
				gcRemoveEffect.setObjectID(m_ObjectID);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_STRIKING);
				m_pZone->broadcastPacket(getX(), getY(), &gcRemoveEffect);
			}
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_STRIKING_2))
	{
		EffectStriking2* pStriking2 = dynamic_cast<EffectStriking2*>(findEffect(Effect::EFFECT_CLASS_STRIKING_2));
		if (pStriking2 != NULL)
		{
			Damage_t   DamageBonus  = pStriking2->getDamageBonus();

			if (pWeapon != NULL && pStriking2->isTargetItem( pWeapon ) )
			{
				m_Damage[ATTR_CURRENT] = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + DamageBonus);
				m_Damage[ATTR_MAX]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + DamageBonus);

				// ItemOID�� ��ġ�Ѵٴ� ���� ��� ��Ʈ����ŷ�� �ɷȰų�,
				// �ٸ� ���⸦ ���ٰ�, �ٽ� ��Ʈ����ŷ�� �ɸ� ���⸦
				// ���ٴ� ���̴�. �׷��Ƿ� ����Ʈ�� ���̶�� ������� �Ѵ�.
				GCAddEffect gcAddEffect;
				gcAddEffect.setObjectID(m_ObjectID);
				gcAddEffect.setEffectID(Effect::EFFECT_CLASS_STRIKING_2);
				gcAddEffect.setDuration(pStriking2->getRemainDuration());
				m_pZone->broadcastPacket(m_X, m_Y, &gcAddEffect);
			}
			else
			{
				// ItemOID�� ��ġ���� �ʴ´ٴ� ���� ��Ʈ����ŷ�� �ɸ� ���¿���
				// �ٸ� ���⸦ ���ٴ� ���̴�. �׷��Ƿ� ������ �����̾�Դ�
				// ��Ʈ����ŷ�� �ɷ��ִ�. �� ����Ʈ�� ��������� �ϹǷ�...
				GCRemoveEffect gcRemoveEffect;
				gcRemoveEffect.setObjectID(m_ObjectID);
				gcRemoveEffect.addEffectList(Effect::EFFECT_CLASS_STRIKING_2);
				m_pZone->broadcastPacket(getX(), getY(), &gcRemoveEffect);
			}
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_BLOOD_DRAIN))
	{
		int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT],    20);
		int ToHitPenalty      = getPercentValue(m_ToHit[ATTR_CURRENT],      20);
		int ProtectionPenalty = getPercentValue(m_Protection[ATTR_CURRENT], 20);
		int DamagePenalty1    = getPercentValue(m_Damage[ATTR_CURRENT],     20);
		int DamagePenalty2    = getPercentValue(m_Damage[ATTR_MAX],         20);

		m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT]    - DefensePenalty);
		m_ToHit[ATTR_CURRENT]      = max(0, m_ToHit[ATTR_CURRENT]      - ToHitPenalty);
		m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
		m_Damage[ATTR_CURRENT]     = max(0, m_Damage[ATTR_CURRENT]     - DamagePenalty1);
		m_Damage[ATTR_MAX]         = max(0, m_Damage[ATTR_MAX]         - DamagePenalty2);
	}

	if (isFlag(Effect::EFFECT_CLASS_DOOM))
	{
		EffectDoom* pDoom = dynamic_cast<EffectDoom*>(findEffect(Effect::EFFECT_CLASS_DOOM));
		if (pDoom!= NULL)
		{
			int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT], pDoom->getDefensePenalty());
			int ProtectionPenalty = getPercentValue(m_Protection[ATTR_CURRENT], pDoom->getProtectionPenalty());

			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT]    - DefensePenalty);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_SEDUCTION))
	{
		EffectSeduction* pSeduction = dynamic_cast<EffectSeduction*>(findEffect(Effect::EFFECT_CLASS_SEDUCTION));
		if (pSeduction!= NULL)
		{
			int ToHitPenalty   = getPercentValue(m_ToHit[ATTR_CURRENT],  pSeduction->getToHitPenalty());
			int DamagePenalty1 = getPercentValue(m_Damage[ATTR_CURRENT], pSeduction->getDamagePenalty());
			int DamagePenalty2 = getPercentValue(m_Damage[ATTR_MAX],     pSeduction->getDamagePenalty());

			m_ToHit[ATTR_CURRENT]  = max(0, m_ToHit[ATTR_CURRENT] - ToHitPenalty);
			m_Damage[ATTR_CURRENT] = max(0, m_Damage[ATTR_CURRENT] - DamagePenalty1);
			m_Damage[ATTR_MAX]     = max(0, m_Damage[ATTR_MAX] - DamagePenalty2);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2))//slayer
	{
		EffectOUTRAGEBOOSTER2* pOUTRAGEBOOSTER2 = dynamic_cast<EffectOUTRAGEBOOSTER2*>(findEffect(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2));
		if (pOUTRAGEBOOSTER2!= NULL)
		{
			int STRPenalty   = getPercentValue(m_STR[ATTR_CURRENT],  pOUTRAGEBOOSTER2->getSTRPenalty());
                        int DEXPenalty   = getPercentValue(m_DEX[ATTR_CURRENT],  pOUTRAGEBOOSTER2->getDEXPenalty());
                        int INTPenalty   = getPercentValue(m_INT[ATTR_CURRENT],  pOUTRAGEBOOSTER2->getINTPenalty());
			m_STR[ATTR_CURRENT]     = max(0, m_STR[ATTR_CURRENT] - STRPenalty);
			m_DEX[ATTR_CURRENT]     = max(0, m_DEX[ATTR_CURRENT] - DEXPenalty);
			m_INT[ATTR_CURRENT]     = max(0, m_INT[ATTR_CURRENT] - INTPenalty);
		}
	}
/*	if (isFlag(Effect::EFFECT_CLASS_PARALYZE))
	{
		EffectParalyze* pParalyze = dynamic_cast<EffectParalyze*>(findEffect(Effect::EFFECT_CLASS_PARALYZE));
		if (pParalyze != NULL)
		{
			int DefensePenalty = getPercentValue(m_Defense[ATTR_CURRENT], pParalyze->getDefensePenalty());
			m_Defense[ATTR_CURRENT] = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
		}
	}*/
	if (isFlag(Effect::EFFECT_CLASS_CHARGING_POWER))
	{
		EffectChargingPower* pChargingPower= dynamic_cast<EffectChargingPower*>(findEffect(Effect::EFFECT_CLASS_CHARGING_POWER));
		if (pChargingPower != NULL)
		{
			if (!isRealWearing(Slayer::WEAR_RIGHTHAND) || pWeapon == NULL || pWeapon->getItemClass() != Item::ITEM_CLASS_BLADE)
			{
				pChargingPower->setDeadline(0);
			}
			else 
	//		if (pChargingPower != NULL)
			{
				int DamageBonus = pChargingPower->getDamageBonus();

				m_Damage[ATTR_CURRENT] = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + DamageBonus);
				m_Damage[ATTR_MAX]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + DamageBonus);
				m_ToHit[ATTR_CURRENT] = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + 200);
				m_ToHit[ATTR_MAX] = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_MAX] + 200);
			}
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_DANCING_SWORD))
	{
		EffectDancingSword* pDancingSword = dynamic_cast<EffectDancingSword*>(findEffect(Effect::EFFECT_CLASS_DANCING_SWORD));
		if (pDancingSword!= NULL)
		{
			m_ToHit[ATTR_CURRENT] = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + pDancingSword->getToHitBonus());
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_GHOST_BLADE))
	{
		EffectGhostBlade* pGhostBlade = dynamic_cast<EffectGhostBlade*>(findEffect(Effect::EFFECT_CLASS_GHOST_BLADE));
		if (pGhostBlade != NULL)
		{
			m_ToHit[ATTR_CURRENT] = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + pGhostBlade->getToHitBonus());
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_MIND_CONTROL))
	{
		EffectMindControl* pMindControl = dynamic_cast<EffectMindControl*>(findEffect(Effect::EFFECT_CLASS_MIND_CONTROL));
		if (pMindControl != NULL)
		{
			m_ToHit[ATTR_CURRENT]   = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + pMindControl->getToHitBonus());
			m_Defense[ATTR_CURRENT] = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + pMindControl->getDefenseBonus());
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_PROTECTION_FROM_POISON))
	{
		EffectProtectionFromPoison* pProtectionFromPoison = dynamic_cast<EffectProtectionFromPoison*>(findEffect(Effect::EFFECT_CLASS_PROTECTION_FROM_POISON));
		if (pProtectionFromPoison != NULL)
		{
			m_Resist[MAGIC_DOMAIN_POISON] += pProtectionFromPoison->getResist();
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_PROTECTION_FROM_CURSE))
	{
		EffectProtectionFromCurse* pProtectionFromCurse = dynamic_cast<EffectProtectionFromCurse*>(findEffect(Effect::EFFECT_CLASS_PROTECTION_FROM_CURSE));
		if (pProtectionFromCurse != NULL)
		{
			m_Resist[MAGIC_DOMAIN_CURSE] += pProtectionFromCurse->getResist();
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_PROTECTION_FROM_ACID))
	{
		EffectProtectionFromAcid* pProtectionFromAcid = dynamic_cast<EffectProtectionFromAcid*>(findEffect(Effect::EFFECT_CLASS_PROTECTION_FROM_ACID));
		if (pProtectionFromAcid != NULL)
		{
			m_Resist[MAGIC_DOMAIN_ACID] += pProtectionFromAcid->getResist();
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_PROTECTION_FROM_BLOOD))
	{
		EffectProtectionFromBlood* pProtectionFromBlood = dynamic_cast<EffectProtectionFromBlood*>(findEffect(Effect::EFFECT_CLASS_PROTECTION_FROM_BLOOD));
		if (pProtectionFromBlood != NULL)
		{
			m_Resist[MAGIC_DOMAIN_BLOOD] += pProtectionFromBlood->getResist();
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_EXPANSION))
	{
		EffectExpansion* pExpansion = dynamic_cast<EffectExpansion*>(findEffect(Effect::EFFECT_CLASS_EXPANSION));
		if (pExpansion != NULL)
		{
			int Bonus = pExpansion->getHPBonus();
			// ü���� ��Ƣ�� ���ش�...
			m_HP[ATTR_MAX] = m_HP[ATTR_MAX] + Bonus;
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_BERSERKER))
	{
		EffectBerserker* pBerserker= dynamic_cast<EffectBerserker*>(findEffect(Effect::EFFECT_CLASS_BERSERKER));
		if (pBerserker != NULL)
		{
			Damage_t BladeMinDamage = 0;
			Damage_t BladeMaxDamage = 0;

			if ( attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_BLADE )
			{
				BladeMinDamage = attr.pWeapon->getMinDamage();
				BladeMaxDamage = attr.pWeapon->getMaxDamage();
			}

			// ������ �� ���� ���ʽ�, ���潺 �� �����ؼ� ���Ƽ�� �ۼ�Ʈ ���̴�.
			// �������� �ɷ�ġ�� ���� �������� ����(��) �������� �տ� ���� �����̴�.
			// �ٸ� ����Ʈ�� ���� �߰� �������� �� ��꿡�� �����Ѵ�.
			int ToHitBonus        = getPercentValue(m_ToHit[ATTR_CURRENT], pBerserker->getToHitBonus());
			int MinDamageBonus    = getPercentValue(AttrMinDamage+BladeMinDamage, pBerserker->getDamageBonus());
			int MaxDamageBonus    = getPercentValue(AttrMaxDamage+BladeMaxDamage, pBerserker->getDamageBonus());
			int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT], pBerserker->getDefensePenalty());
//			int ProtectionPenalty = getPercentValue(m_Defense[ATTR_CURRENT], pBerserker->getProtectionPenalty());
//			2007 03 14 �и� m_Defense�� �ƴϰ�... m_Protection�� ���̶�� ������ ���ƴ�...!
			int ProtectionPenalty = getPercentValue(m_Protection[ATTR_CURRENT], pBerserker->getProtectionPenalty());

/*			cout << "Damage Bonus : " << (int)pBerserker->getDamageBonus()
				 << " MinDamage : " << (int)(AttrMinDamage+BladeMinDamage)
				 << " MaxDamage : " << (int)(AttrMaxDamage+BladeMaxDamage)
				 << " MinDamageBonus : " << (int)MinDamageBonus
				 << " MaxDamageBonus : " << (int)MaxDamageBonus
				 << endl;
*/
			m_ToHit[ATTR_CURRENT]      = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
			m_Damage[ATTR_CURRENT]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + MinDamageBonus);
			m_Damage[ATTR_MAX]         = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + MaxDamageBonus);
			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_GLADIATOR))
	{
		EffectGLADIATOR* pGLADIATOR= dynamic_cast<EffectGLADIATOR*>(findEffect(Effect::EFFECT_CLASS_GLADIATOR));
		if (pGLADIATOR != NULL)
		{
			//Damage_t BladeMinDamage = 0;
			//Damage_t BladeMaxDamage = 0;

			//if ( attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_BLADE )
			//{
				//BladeMinDamage = attr.pWeapon->getMinDamage();
				//BladeMaxDamage = attr.pWeapon->getMaxDamage();

			int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT], pGLADIATOR->getDefensePenalty());

			m_ToHit[ATTR_CURRENT]      = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + 200);
			m_Damage[ATTR_CURRENT]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + 120);
			m_Damage[ATTR_MAX]         = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + 120);
			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT] + DefensePenalty);
			//}
		}
	}
	if ( isFlag( Effect:: EFFECT_CLASS_ADV2THEROAROFAWAKENWARRIOR) )
	{
		EffectADV2THEROAROFAWAKENWARRIOR* pEffect = dynamic_cast<EffectADV2THEROAROFAWAKENWARRIOR*>(findEffect(Effect::EFFECT_CLASS_ADV2THEROAROFAWAKENWARRIOR));
		if ( pEffect != NULL )
		{
			//HP_t hpBonus = pEffect->getHPBonus();
                       HP_t hpBonus = 2225;
                       int     bonus= pEffect->getSTRBonus();
                       // int     DEXBonus= pEffect->getDEXBonus();
                       // int     INTBonus= pEffect->getINTBonus();
			
                        m_STR[ATTR_CURRENT] += bonus;
                        m_DEX[ATTR_CURRENT] += bonus;
                        m_INT[ATTR_CURRENT] += bonus;
                        m_STR[ATTR_MAX] += bonus;
                        m_DEX[ATTR_MAX] += bonus;
                        m_INT[ATTR_MAX] += bonus; 
			m_ToHit[ATTR_CURRENT]      += 35;
			m_Damage[ATTR_CURRENT]     += 5;
			m_Damage[ATTR_MAX]         += 8;
			m_Defense[ATTR_CURRENT]	   += 15;
                        m_Protection[ATTR_CURRENT] += 30;
			m_HP[ATTR_MAX]             += 225;
                        
		     //   computeStatOffset();
                       m_HP[ATTR_MAX] 			   = min((HP_t)SLAYER_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + hpBonus));
		//       m_HP[ATTR_CURRENT] += hpBonus ;
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_DEATH))
	{
		EffectDeath* pDeath = dynamic_cast<EffectDeath*>(findEffect(Effect::EFFECT_CLASS_DEATH));
		if (pDeath != NULL)
		{
			for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
			{
				m_Resist[i] -= pDeath->getResistPenalty();
//				if ( m_Resist[i] < 0 ) m_Resist[i] = 0;
			}
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_BLUNTING))
	{
		EffectBlunting* pBlunting = dynamic_cast<EffectBlunting*>(findEffect(Effect::EFFECT_CLASS_BLUNTING));
		if ( pBlunting != NULL )
		{
			int DefensePenalty = pBlunting->getDefensePenalty();
			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_HOLY_ARMOR))
	{
		EffectHolyArmor* pHolyArmor = dynamic_cast<EffectHolyArmor*>(findEffect(Effect::EFFECT_CLASS_HOLY_ARMOR));

		if ( pHolyArmor != NULL )
		{
			m_Defense[ATTR_CURRENT] = m_Defense[ATTR_CURRENT] + pHolyArmor->getDefBonus();
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_HOLY_ARMOR_2))
	{
		EffectHolyArmor2* pHolyArmor2 = dynamic_cast<EffectHolyArmor2*>(findEffect(Effect::EFFECT_CLASS_HOLY_ARMOR_2));

		if ( pHolyArmor2 != NULL )
		{
			m_Defense[ATTR_CURRENT] = m_Defense[ATTR_CURRENT] + pHolyArmor2->getDefBonus();
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_WHITSUNTIDE))
	{
		EffectWhitsuntide* pWhitsuntide = dynamic_cast<EffectWhitsuntide*>(findEffect(Effect::EFFECT_CLASS_WHITSUNTIDE));

		if ( pWhitsuntide != NULL )
		{
			m_Resist[MAGIC_DOMAIN_POISON] += pWhitsuntide->getBonus();
			m_Resist[MAGIC_DOMAIN_ACID] += pWhitsuntide->getBonus();
			m_Resist[MAGIC_DOMAIN_CURSE] += pWhitsuntide->getBonus();
			m_Resist[MAGIC_DOMAIN_BLOOD] += pWhitsuntide->getBonus();

//			m_MagicDamageReduce += pWhitsuntide->getBonus();
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_INTIMATE_GRAIL))
	{
		EffectIntimateGrail* pIntimateGrail = dynamic_cast<EffectIntimateGrail*>(findEffect(Effect::EFFECT_CLASS_INTIMATE_GRAIL));

		if ( pIntimateGrail != NULL )
		{
			// �����̾�� �ູ
/*			int hpratio = 15 + (int)(pIntimateGrail->getSkillLevel() / 6.6);
			m_HP[ATTR_MAX] += getPercentValue(m_HP[ATTR_MAX], hpratio);
			m_MP[ATTR_MAX] += getPercentValue(m_MP[ATTR_MAX], hpratio);
			
			int defratio = 10 + (pIntimateGrail->getSkillLevel() / 10);
			m_Defense[ATTR_CURRENT] += getPercentValue(m_Defense[ATTR_CURRENT] , defratio);
			m_Protection[ATTR_CURRENT] += getPercentValue(m_Protection[ATTR_CURRENT], defratio);*/

			m_HP[ATTR_MAX] += pIntimateGrail->getHPBonus();
			m_MP[ATTR_MAX] += pIntimateGrail->getHPBonus();

			m_Defense[ATTR_CURRENT] += pIntimateGrail->getDefenseBonus();
			m_Protection[ATTR_CURRENT] += pIntimateGrail->getDefenseBonus();
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_INTIMATE_GRAIL_2))
	{
		EffectIntimateGrail2* pIntimateGrail2 = dynamic_cast<EffectIntimateGrail2*>(findEffect(Effect::EFFECT_CLASS_INTIMATE_GRAIL_2));

		if ( pIntimateGrail2 != NULL )
		{


			m_HP[ATTR_MAX] += pIntimateGrail2->getHPBonus();
			m_MP[ATTR_MAX] += pIntimateGrail2->getHPBonus();

			m_Defense[ATTR_CURRENT] += pIntimateGrail2->getDefenseBonus();
			m_Protection[ATTR_CURRENT] += pIntimateGrail2->getDefenseBonus();
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_PARTY_AURA ) )
	{
		EffectPartyAura* pPartyAura = dynamic_cast<EffectPartyAura*>(findEffect(Effect::EFFECT_CLASS_PARTY_AURA));

		if ( pPartyAura != NULL )
		{
			Level_t level = getLevel();
			size_t partySize = pPartyAura->getPartySize();

			if ( pPartyAura->isHP() )
			{
				HP_t amount = (HP_t)( ( 65 + ( level / 3 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_HP[ATTR_MAX] = min( (HP_t)SLAYER_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + amount ));
			}
			if ( pPartyAura->isDefense() )
			{
				Defense_t amount = (Defense_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Defense[ATTR_CURRENT] = min( SLAYER_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + amount );
			}
			if ( pPartyAura->isToHit() )
			{
				ToHit_t amount = (ToHit_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_ToHit[ATTR_CURRENT] = min( SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + amount );
			}
			if ( pPartyAura->isLuck() )
			{
				Luck_t amount = (Luck_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Luck += amount;
			}
			if ( pPartyAura->isDamage() )
			{
				Damage_t amount = (Damage_t)( ( 15 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Damage[ATTR_CURRENT] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + amount );
				m_Damage[ATTR_MAX] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + amount );
			}
			if ( pPartyAura->isResist() )
			{
				Resist_t amount = (Resist_t)( ( 15 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Resist[MAGIC_DOMAIN_POISON] += amount;
				m_Resist[MAGIC_DOMAIN_ACID] += amount;
				m_Resist[MAGIC_DOMAIN_CURSE] += amount;
				m_Resist[MAGIC_DOMAIN_BLOOD] += amount;
			}
		}
	}
/*
	// 2006.05.29
	// STR + 15
	if( isFlag(Effect::EFFECT_CLASS_GREYFELL_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_STR, 15 );
	}
	// DEX + 15
	if( isFlag(Effect::EFFECT_CLASS_SOVIUS_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_DEX, 15 );
	}
	// INT + 15
	if( isFlag(Effect::EFFECT_CLASS_MIR_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_INT, 15 );
	}
*/
	if ( isFlag(Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL) )
	{
		m_Defense[ATTR_CURRENT] += 30;
	}

	if ( isFlag(Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL) )
	{
		m_Protection[ATTR_CURRENT] += 30;
	}

	if ( isFlag(Effect::EFFECT_CLASS_FURIOUS_INCENSE) )
	{
		m_Damage[ATTR_CURRENT] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + 3 );
		m_Damage[ATTR_MAX] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + 3 );
	}

	// 20070828
	if ( isFlag(Effect::EFFECT_CLASS_EAGLES_EYE1) )
	{
		int ToHitBonus = getPercentValue(m_ToHit[ATTR_CURRENT], 5);
		m_ToHit[ATTR_CURRENT] += ToHitBonus; 
	}
	else if( isFlag(Effect::EFFECT_CLASS_EAGLES_EYE2) )
	{
		int ToHitBonus = getPercentValue(m_ToHit[ATTR_CURRENT], 10);
		m_ToHit[ATTR_CURRENT] += ToHitBonus; 
	}

	// �нú� ��� ���� �ö󰡴� �ɷ�ġ�� ����Ѵ�.
	if (pWeapon != NULL)
	{
		Item::ItemClass IClass = pWeapon->getItemClass();
		int DamageBonus = 0;
		int ToHitBonus = 0;
		int CriticalRatioBonus = 0;

		// ���� ���.. ObservingEyeüũ. by sigi. 2002.6.19
		if (pWeapon->isGun() && isFlag(Effect::EFFECT_CLASS_OBSERVING_EYE))
		{
			EffectObservingEye* pObservingEye= dynamic_cast<EffectObservingEye*>(findEffect(Effect::EFFECT_CLASS_OBSERVING_EYE));
			if (pObservingEye != NULL)
			{
				int DamageBonus = pObservingEye->getDamageBonus();

				m_Damage[ATTR_CURRENT] = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + DamageBonus);
				m_Damage[ATTR_MAX]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + DamageBonus);

				CriticalRatioBonus += pObservingEye->getCriticalHitBonus();

//				m_CriticalRatio[ATTR_CURRENT] = m_CriticalRatio[ATTR_CURRENT] + CriticalRatioBonus;
//				m_CriticalRatio[ATTR_MAX]     = m_CriticalRatio[ATTR_MAX] + CriticalRatioBonus;

				//int VisionBonus = pObservingEye->getVisionBonus();
				// �̰Ŵ� client���� ó���ϵ��� �Ѵ�. 
			}
		}

		// Liveness ���ʽ� �����ֱ�
		if ( pLiveness != NULL && pWeapon->isGun() )
		{
			m_HP[ATTR_MAX] = m_HP[ATTR_MAX] + LivenessHPBonus;
			m_Defense[ATTR_CURRENT]  = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + LivenessDefenseBonus);
		}

		// Passive Skill : Will of Iron �����ֱ� : SWORD or BLADE �϶�
		if ( (pFabulousSoul != NULL && pWeapon->getItemClass() == Item::ITEM_CLASS_SWORD) ||
			 (pWillOfIron != NULL && pWeapon->getItemClass() == Item::ITEM_CLASS_BLADE) )
		{
			m_HP[ATTR_MAX] += HPBonus_WillOfIron;
		}

		

		if ( pWeapon->isGun() )
		{
			DamageBonus += getSkillDomainLevel( SKILL_DOMAIN_GUN ) / 10;
		}

		SlayerSkillSlot* pArmsMastery1 = getSkill(SKILL_ARMS_MASTERY_1);
		SlayerSkillSlot* pArmsMastery2 = getSkill(SKILL_ARMS_MASTERY_2);

		if (IClass == Item::ITEM_CLASS_AR)
		{
			SlayerSkillSlot* pARMastery = getSkill(SKILL_AR_MASTERY);
			if (pARMastery != NULL && pARMastery->canUse())
			{
				DamageBonus += 3;
				ToHitBonus += 5;
			}

			if ( pArmsMastery2 != NULL && pArmsMastery2->canUse() )
			{
				ToHitBonus += 6;
				DamageBonus += 5;
				CriticalRatioBonus += 6;
			}
			else if ( pArmsMastery1 != NULL && pArmsMastery1->canUse() )
			{
				ToHitBonus += 4;
				DamageBonus += 3;
				CriticalRatioBonus += 4;
			}

			/*if (isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2))
			{
				SlayerSkillSlot* pSniping = getSkill(SKILL_SNIPING2);

				if (pSniping!=NULL)
				{
					//int level = 120;

					int damage = m_Damage[ATTR_CURRENT];
					int toHit  = m_ToHit[ATTR_CURRENT];

					//int damageBonusPercent = m_STR[ATTR_CURRENT]/20 * level/20;
					//int toHitBonusPercent = m_DEX[ATTR_CURRENT]/10 * level/20;

					DamageBonus += damage * 1;//damageBonusPercent/100;
					ToHitBonus += toHit * 5;//toHitBonusPercent/100;
				}

			}*/

		}
		else if (IClass == Item::ITEM_CLASS_SMG)
		{
			SlayerSkillSlot* pSMGMastery = getSkill(SKILL_SMG_MASTERY);
			if (pSMGMastery != NULL && pSMGMastery->canUse())
			{
				DamageBonus += 3;
				ToHitBonus += 5;
			}

			if ( pArmsMastery2 != NULL && pArmsMastery2->canUse() )
			{
				ToHitBonus += 5;
				DamageBonus += 5;
				CriticalRatioBonus += 7;
			}
			else if ( pArmsMastery1 != NULL && pArmsMastery1->canUse() )
			{
				ToHitBonus += 3;
				DamageBonus += 3;
				CriticalRatioBonus += 5;
			}

		}
		else if (IClass == Item::ITEM_CLASS_SG)
		{
			SlayerSkillSlot* pSGMastery = getSkill(SKILL_SG_MASTERY);
			if (pSGMastery != NULL && pSGMastery->canUse())
			{
				DamageBonus += 3;
				ToHitBonus += 5;
			}

			if ( pArmsMastery2 != NULL && pArmsMastery2->canUse() )
			{
				ToHitBonus += 5;
				DamageBonus += 6;
				CriticalRatioBonus += 10;
			}
			else if ( pArmsMastery1 != NULL && pArmsMastery1->canUse() )
			{
				ToHitBonus += 3;
				DamageBonus += 5;
				CriticalRatioBonus += 9;
			}
		}
		else if (IClass == Item::ITEM_CLASS_SR)
		{
			SlayerSkillSlot* pSRMastery = getSkill(SKILL_SR_MASTERY);
			if (pSRMastery != NULL && pSRMastery->canUse())
			{
				DamageBonus += 3;
				ToHitBonus += 5;
			}

			if ( pArmsMastery2 != NULL && pArmsMastery2->canUse() )
			{
				ToHitBonus += 11;
				DamageBonus += 7;
				CriticalRatioBonus += 5;
			}
			else if ( pArmsMastery1 != NULL && pArmsMastery1->canUse() )
			{
				ToHitBonus += 9;
				DamageBonus += 5;
				CriticalRatioBonus += 3;
			}

			if (isFlag(Effect::EFFECT_CLASS_SNIPING_MODE))
			{
				//DamageBonus += 3;
				//ToHitBonus += 5;

				// by sigi. 2002.12.3
				SlayerSkillSlot* pSniping = getSkill(SKILL_SNIPING);

				if (pSniping!=NULL)
				{
					//int level = pSniping->getExpLevel();

					int damage = m_Damage[ATTR_CURRENT];
					int toHit  = m_ToHit[ATTR_CURRENT];

					//int damageBonusPercent = m_STR[ATTR_CURRENT]/20 * level/20;
					//int toHitBonusPercent = m_DEX[ATTR_CURRENT]/10 * level/20;

					DamageBonus += damage * 1;//damageBonusPercent/100;
					ToHitBonus += toHit * 3;//toHitBonusPercent/100;
				}

			}
			if (isFlag(Effect::EFFECT_CLASS_SNIPING_MODE_2))
			{
				SlayerSkillSlot* pSniping = getSkill(SKILL_SNIPING2);

				if (pSniping!=NULL)
				{
					//int level = 120;

					int damage = m_Damage[ATTR_CURRENT];
					int toHit  = m_ToHit[ATTR_CURRENT];

					//int damageBonusPercent = m_STR[ATTR_CURRENT]/20 * level/20;
					//int toHitBonusPercent = m_DEX[ATTR_CURRENT]/10 * level/20;

					DamageBonus += damage * 1;//damageBonusPercent/100;
					ToHitBonus += toHit * 5;//toHitBonusPercent/100;
				}

			}
		}
		else if (IClass == Item::ITEM_CLASS_SWORD)	// by sigi. 2002.6.7
		{
			// SWORD_MASTERY
			SlayerSkillSlot* pMastery = getSkill(SKILL_SWORD_MASTERY);
			if (pMastery != NULL && pMastery->canUse())
			{
				int level = m_SkillDomainLevels[SKILL_DOMAIN_SWORD];

				/*
				switch (g_pSkillInfoManager->getGradeByDomainLevel(level))
				{
					case SKILL_GRADE_APPRENTICE:   DamageBonus += 2; break;
					case SKILL_GRADE_ADEPT:        DamageBonus += 3; break;
					case SKILL_GRADE_EXPERT:       DamageBonus += 4; break;
					case SKILL_GRADE_MASTER:       DamageBonus += 5; break;
					case SKILL_GRADE_GRAND_MASTER: DamageBonus += 6; break;
					default:                       break;
				}
				*/

				// by sigi. 2002.12.3
//				DamageBonus += level*10/125;
				DamageBonus += 3 + level/15;
				ToHitBonus += 5 + ( level/5 );
			}
		}
		else if (IClass == Item::ITEM_CLASS_BLADE)	// by sigi. 2002.6.7
		{
			// CONCENTRATION
			SlayerSkillSlot* pSkill = getSkill(SKILL_CONCENTRATION);
			if (pSkill != NULL && pSkill->canUse())
			{
				int level = m_SkillDomainLevels[SKILL_DOMAIN_BLADE];

				/*
				switch (g_pSkillInfoManager->getGradeByDomainLevel(level))
				{
					case SKILL_GRADE_APPRENTICE:   ToHitBonus += 3; break;
					case SKILL_GRADE_ADEPT:        ToHitBonus += 6; break;
					case SKILL_GRADE_EXPERT:       ToHitBonus += 9; break;
					case SKILL_GRADE_MASTER:       ToHitBonus += 12; break;
					case SKILL_GRADE_GRAND_MASTER: ToHitBonus += 15; break;
					default:                       break;
				}
				*/
				// by sigi. 2002.12.3
//				ToHitBonus += level/6;
				//ToHitBonus += 3+(level/10);
				ToHitBonus += 5+(level/4);
			}

			// EVASION
		 	pSkill = getSkill(SKILL_EVASION);
			if (pSkill != NULL && pSkill->canUse())
			{
				int level = m_SkillDomainLevels[SKILL_DOMAIN_BLADE];

				//  by sigi. 2002.12.3
//				Defense_t DefenseBonus = (level-20)/5;
				Defense_t DefenseBonus = 3 + (level-20)/5;
				/*
				switch (g_pSkillInfoManager->getGradeByDomainLevel(level))
				{
					case SKILL_GRADE_APPRENTICE:   DefenseBonus += 3; break;
					case SKILL_GRADE_ADEPT:        DefenseBonus += 6; break;
					case SKILL_GRADE_EXPERT:       DefenseBonus += 9; break;
					case SKILL_GRADE_MASTER:       DefenseBonus += 12; break;
					case SKILL_GRADE_GRAND_MASTER: DefenseBonus += 15; break;
					default:                       break;
				}
				*/

				// �ϴ� Evasion�� defense�� �ٲٹǷ� ���⼭�� ���.. by sigi
				m_Defense[ATTR_CURRENT]  = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + DefenseBonus);
				m_Defense[ATTR_MAX]      = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_MAX] + DefenseBonus);
			}
		}

		//cout << "ToHitBonus = " << ToHitBonus << endl;

		if ( pWeapon->isGun() )
		{
			// Concealment ���ʽ� �����ֱ�
			m_Defense[ATTR_CURRENT] += DefBonus;
			m_Protection[ATTR_CURRENT] += ProBonus;
			m_Defense[ATTR_MAX] += DefBonus;
			m_Protection[ATTR_MAX] += ProBonus;
		}

		m_ToHit[ATTR_CURRENT]  = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
		m_ToHit[ATTR_MAX]      = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_MAX] + ToHitBonus);
		m_Damage[ATTR_CURRENT] = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + DamageBonus);
		m_Damage[ATTR_MAX]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + DamageBonus);
		m_CriticalRatio[ATTR_CURRENT] = m_CriticalRatio[ATTR_CURRENT] + CriticalRatioBonus;
		m_CriticalRatio[ATTR_MAX]     = m_CriticalRatio[ATTR_MAX] + CriticalRatioBonus;

	}

	// ���� üũ. by sigi. 2002.6.7
	if (pShield!=NULL && pShield->getItemClass()==Item::ITEM_CLASS_SHIELD)
	{
		int ProtectionBonus = 0;
		SlayerSkillSlot* pMastery = getSkill(SKILL_SHIELD_MASTERY);
		if (pMastery != NULL && pMastery->canUse())
		{
			int level = m_SkillDomainLevels[SKILL_DOMAIN_SWORD];
			/*
			switch (g_pSkillInfoManager->getGradeByDomainLevel(level))
			{
				case SKILL_GRADE_APPRENTICE:   ProtectionBonus += 3; break;
				case SKILL_GRADE_ADEPT:        ProtectionBonus += 6; break;
				case SKILL_GRADE_EXPERT:       ProtectionBonus += 9; break;
				case SKILL_GRADE_MASTER:       ProtectionBonus += 12; break;
				case SKILL_GRADE_GRAND_MASTER: ProtectionBonus += 15; break;
				default:                       break;
			}
			*/
			// by sigi. 2002.12.3
//			ProtectionBonus += (level-20)/5;
			ProtectionBonus += 5 + (level-20)/5;

			m_Protection[ATTR_CURRENT]  = min(SLAYER_MAX_PROTECTION, m_Protection[ATTR_CURRENT] + ProtectionBonus);
			m_Protection[ATTR_MAX]      = min(SLAYER_MAX_PROTECTION, m_Protection[ATTR_MAX] + ProtectionBonus);
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK))
	{
		EffectBloodsSymposionAttack* pEffect = dynamic_cast<EffectBloodsSymposionAttack*>(findEffect(Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK));
		if( pEffect != NULL )
		{
			int DefensePenalty = pEffect->getDefensePenalty();
			int ProtectionPenalty = pEffect->getProtectionPenalty();
			int HPPenalty = pEffect->getHPPenalty();

			m_Defense[ATTR_CURRENT]		= max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
			m_Protection[ATTR_CURRENT]	= max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
			m_HP[ATTR_MAX]			= max((HP_t)1,(HP_t)( m_HP[ATTR_MAX] - HPPenalty));
			// 20080813 ���� �����ۿ� ���� ��Ƣ�� ���ʽ��� ���� ��� �� �κ� ������ �ǰ� Ȯ ���̴�  ������ �ȴ�. �� �κ��� �� �ڷ� ����.
//			if( m_HP[ATTR_CURRENT] > m_HP[ATTR_MAX] )
//			{
//				m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
//			}
		}
	}
/*
	// 2007 01 16 �̽��׸� ���� ����Ʈ ��� �߰�
	if (isFlag(Effect::EFFECT_CLASS_MYSTERY_POTION))
	{
		EffectMysteryPotion* pEffect = dynamic_cast<EffectMysteryPotion*>(findEffect(Effect::EFFECT_CLASS_MYSTERY_POTION));
		if( pEffect != NULL )
		{
			HP_t bonusHP = 50;
			Defense_t bonusDef = 10; 
			m_Defense[ATTR_CURRENT]  = max( 0, m_Defense[ATTR_CURRENT] + bonusDef );
			m_HP[ATTR_CURRENT]		 = max( 0, m_HP[ATTR_CURRENT] + bonusHP );
			computeOptionStat( 185 );		// ��� + 3
		}
	}
*/
	///////////////////////////////////////////////////////////////////////////////
	// ��� ���ʽ��� ����Ѵ�.
	///////////////////////////////////////////////////////////////////////////////
	/*if ( hasRankBonus( RankBonus::RANK_BONUS_DEADLY_SPEAR ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_DEADLY_SPEAR );
		Assert( pRankBonus != NULL );

		int CriticalRatioBonus = pRankBonus->getPoint();

		m_CriticalRatio[ATTR_CURRENT] = m_CriticalRatio[ATTR_CURRENT] + CriticalRatioBonus;
		m_CriticalRatio[ATTR_MAX]     = m_CriticalRatio[ATTR_MAX] + CriticalRatioBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_BEHEMOTH_ARMOR ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_BEHEMOTH_ARMOR );
		Assert( pRankBonus != NULL );

		Defense_t DefenseBonus = pRankBonus->getPoint();

		m_Defense[ATTR_CURRENT]  = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + DefenseBonus);
		m_Defense[ATTR_MAX]      = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_MAX] + DefenseBonus);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_DRAGON_EYE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_DRAGON_EYE );
		Assert( pRankBonus != NULL );

		int ToHitBonus = pRankBonus->getPoint();

		m_ToHit[ATTR_CURRENT]  = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
		m_ToHit[ATTR_MAX]      = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_MAX] + ToHitBonus);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_IMMORTAL_HEART ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_IMMORTAL_HEART );
		Assert( pRankBonus != NULL );

		int HPBonus = pRankBonus->getPoint();

		m_HP[ATTR_MAX] = m_HP[ATTR_MAX] + HPBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_RELIANCE_BRAIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_RELIANCE_BRAIN );
		Assert( pRankBonus != NULL );

		int MPBonus = pRankBonus->getPoint();

		m_MP[ATTR_MAX] = min(SLAYER_MAX_MP, m_MP[ATTR_MAX] + MPBonus);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_SLAYING_KNIFE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SLAYING_KNIFE );
		Assert( pRankBonus != NULL );

		int DamageBonus = pRankBonus->getPoint();

		m_Damage[ATTR_CURRENT] = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + DamageBonus);
		m_Damage[ATTR_MAX]     = min(SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + DamageBonus);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_HAWK_WING ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_HAWK_WING );
		Assert( pRankBonus != NULL );

		int AttackSpeedBonus = pRankBonus->getPoint();

		increaseAttackSpeed(AttackSpeedBonus, ATTR_CURRENT);
		increaseAttackSpeed(AttackSpeedBonus, ATTR_MAX);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_SAPPHIRE_BLESS ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SAPPHIRE_BLESS );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_ACID] += ResistBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_RUBY_BLESS ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_RUBY_BLESS );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_BLOOD] += ResistBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_DIAMOND_BLESS ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_DIAMOND_BLESS );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_CURSE] += ResistBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_EMERALD_BLESS ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_EMERALD_BLESS );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_POISON] += ResistBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_FORTUNE_HAND ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_FORTUNE_HAND );
		Assert( pRankBonus != NULL );

		int LuckBonus = pRankBonus->getPoint();

		m_Luck += LuckBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_EVOLUTION_IMMORTAL_HEART ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_EVOLUTION_IMMORTAL_HEART );
		Assert( pRankBonus != NULL );

		HPBonus += getPercentValue( m_HP[ATTR_MAX], pRankBonus->getPoint() );
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_BEHEMOTH_ARMOR_2 ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_BEHEMOTH_ARMOR_2 );
		Assert( pRankBonus != NULL );

		Defense_t DefenseBonus = getPercentValue( m_Defense[ATTR_CURRENT], pRankBonus->getPoint() );

		m_Defense[ATTR_CURRENT]  = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + DefenseBonus);
		m_Defense[ATTR_MAX]      = min(SLAYER_MAX_DEFENSE, m_Defense[ATTR_MAX] + DefenseBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_DRAGON_EYE_2 ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_DRAGON_EYE_2 );
		Assert( pRankBonus != NULL );

		int ToHitBonus = getPercentValue( m_ToHit[ATTR_CURRENT], pRankBonus->getPoint() );

		m_ToHit[ATTR_CURRENT]  = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
		m_ToHit[ATTR_MAX]      = min(SLAYER_MAX_TOHIT, m_ToHit[ATTR_MAX] + ToHitBonus);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_EVOLUTION_RELIANCE_BRAIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_EVOLUTION_RELIANCE_BRAIN );
		Assert( pRankBonus != NULL );

		int MPBonus = getPercentValue( m_MP[ATTR_MAX], pRankBonus->getPoint() );

		m_MP[ATTR_MAX] = min(SLAYER_MAX_MP, m_MP[ATTR_MAX] + MPBonus);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_HIT_CONTROL ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_HIT_CONTROL );
		Assert( pRankBonus != NULL );

		int CriticalRatioBonus = getPercentValue( m_CriticalRatio[ATTR_CURRENT], pRankBonus->getPoint() );

		m_CriticalRatio[ATTR_CURRENT] = m_CriticalRatio[ATTR_CURRENT] + CriticalRatioBonus;
		m_CriticalRatio[ATTR_MAX]     = m_CriticalRatio[ATTR_MAX] + CriticalRatioBonus;
	}*/


	// ���� ���ʽ� ���
	if ( HPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)SLAYER_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + HPBonus ));
	}

	if ( RaceWarHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)SLAYER_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + RaceWarHPBonus) );
	}

	if ( DragonEyeHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min((HP_t) SLAYER_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + DragonEyeHPBonus) );
		EffectDragonEye* pEffectDragonEye= dynamic_cast<EffectDragonEye*>(findEffect(Effect::EFFECT_CLASS_DRAGON_EYE));
		if (pEffectDragonEye != NULL)
		{
			if( pEffectDragonEye->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)SLAYER_MAX_HP,(HP_t)( m_HP[ATTR_CURRENT] + DragonEyeHPBonus ));
				pEffectDragonEye->setHPBonus(false);
			}
		}
	}

#ifdef __CONTRIBUTION_SYSTEM__
	if ( HeroItemHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)SLAYER_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + HeroItemHPBonus ));
		EffectSlayerHeroItem* pEffectHeroItem = dynamic_cast<EffectSlayerHeroItem*>(findEffect(Effect::EFFECT_CLASS_SLAYER_HERO_ITEM));
		if (pEffectHeroItem != NULL)
		{
			if( pEffectHeroItem->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)SLAYER_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + HeroItemHPBonus ));
				pEffectHeroItem->setHPBonus(false);
			}
		}
	}
	if ( LevelHeroItemHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)SLAYER_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + LevelHeroItemHPBonus ));
		EffectSlayerLevelWarHeroItem* pEffectHeroItem = dynamic_cast<EffectSlayerLevelWarHeroItem*>(findEffect(Effect::EFFECT_CLASS_SLAYER_LEVELWAR_HERO_ITEM));
		if (pEffectHeroItem != NULL)
		{
			if( pEffectHeroItem->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)SLAYER_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + LevelHeroItemHPBonus ));
				pEffectHeroItem->setHPBonus(false);
			}
		}
	}
#endif

		// 20080416 SKILL_INCREASE_OF_HEALTH — MaxHP bonus (Phoenix 2026-09-07: yorum kaldirildi)
		if( getSkill(SKILL_INCREASE_OF_HEALTH_15) )
		{
			m_HP[ATTR_MAX] += 5400;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_14) )
		{
			m_HP[ATTR_MAX] += 4800;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_13) )
		{
			m_HP[ATTR_MAX] += 4200;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_12) )
		{
			m_HP[ATTR_MAX] += 3600;
		} 
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_11) )
		{
			m_HP[ATTR_MAX] += 3000;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_10) )
		{
			m_HP[ATTR_MAX] += 2400;
		}
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_9) )
		{
			m_HP[ATTR_MAX] += 1800;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_8) )
		{
			m_HP[ATTR_MAX] += 1600;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_7) )
		{
			m_HP[ATTR_MAX] += 1400;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_6) )
		{
			m_HP[ATTR_MAX] += 1200;
		} 
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_5) )
		{
			m_HP[ATTR_MAX] += 1000;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_4) )
		{
			m_HP[ATTR_MAX] += 800;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_3) )
		{
			m_HP[ATTR_MAX] += 600;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_2) )
		{
			m_HP[ATTR_MAX] += 400;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_1) )
		{
			m_HP[ATTR_MAX] += 200;
		}
             if( getSkill(SKILL_hpup_1500) )
		{
			 m_HP[ATTR_MAX] += 7000;
		} 
             if( getSkill(SKILL_ADV2_UNKNOW2) )
		{
			 m_HP[ATTR_MAX] += 1000;
                         m_PhysicBonusDamage += 30;

		} 
             if( getSkill(SKILL_ADV2_UNKNOW3) )
		{
			 m_HP[ATTR_MAX] += 1000;
                         m_PhysicBonusDamage += 30;

		}
             if( getSkill(SKILL_ADV2_UNKNOW4) )
		{
		m_ToHit[ATTR_CURRENT] += 50;
                m_CriticalRatio[ATTR_MAX] += 50;
		}
             if( getSkill(SKILL_ADV2_UNKNOW5) )
		{
			m_MagicDamageReduce += 30;
                        m_MagicBonusDamage += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW6) )
		{
			    m_MagicBonusDamage += 30;
                            m_PhysicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW7) )
		{
			m_PhysicBonusDamage += 30;
                        m_ToHit[ATTR_CURRENT] += 100;
		}
             if( getSkill(SKILL_ADV2_UNKNOW8) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW9) )
		{
			m_PhysicBonusDamage += 30;
                        m_Defense[ATTR_CURRENT] += 200;
		}
             if( getSkill(SKILL_ADV2_UNKNOW10) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW11) )
		{
		            m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW12) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}

                if( getSkill(SKILL_RALLSER_44) )
		{
			m_Defense[ATTR_CURRENT] += 100;
		} 
                if( getSkill(SKILL_RALLSER_5) )
		{
			m_ToHit[ATTR_CURRENT] += 100;
		} 
                if( getSkill(SKILL_RALLSER_6) )
		{
			m_Damage[ATTR_MAX] += 40;
		} 
                if( getSkill(SKILL_RALLSER_7) )
		{
			m_MagicBonusDamage += 40;
		} 
                if( getSkill(SKILL_RALLSER_8) )
		{
			m_HP[ATTR_MAX] += 800;
		} 
	// Hoodlum Stigma
	if ( isFlag( Effect::EFFECT_CLASS_HOODLUM_STIGMA ) )
	{
		m_HP[ATTR_MAX] = getPercentValue( m_HP[ATTR_MAX], 70 );
	}

	// ������ų �ʱ�ȭ
	initCastleSkill();

	if ( isFlag(Effect::EFFECT_CLASS_BURST_GUN ) )
	{
		m_CriticalRatio[ATTR_CURRENT]  = 200;
		m_CriticalRatio[ATTR_MAX]      = 200;
	}

	for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
	{
		if ( m_Resist[i] < 0 ) m_Resist[i] = 0;
	}
	
	// 20080813 ���� �����ۿ� ���� ��Ƣ�� ���ʽ��� ���� ��� ����� �� ���������� �� �ڵ带 �ִ´�.
	if( m_HP[ATTR_CURRENT] > m_HP[ATTR_MAX] )
	{
		m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
	}

//	cout << getName() << "�� Luck : " << m_Luck << endl;

	// ���� HP�� MAX HP���� ������
	/*
	if ( m_HP[ATTR_CURRENT] > m_HP[ATTR_MAX] )
	{
		m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
	}
	*/

	// ��Ƽ�� ũ�⿡ ��� �ɷ�ġ�� ���� �� �ִ�.

	/*
	// ��Ƽ �ο���� �Ѿ���� ���� ���� �ٽ� ����Ѵ�.
	if (numPartyMember == -1)
	{
		if (m_PartyID != 0)
		{
			LocalPartyManager* pLPM = getLocalPartyManager();
			Assert(pLPM != NULL);
	
			numPartyMember = pLPM->getAdjacentMemberSize(m_PartyID, this);
		}
	}

	if (numPartyMember > 1)
	{

		uint ToHitBonus      = 0;
		uint DefenseBonus    = 0;
		uint ProtectionBonus = 0;
		uint DamageBonus     = 0;

		switch (numPartyMember)
		{
			case 2: ToHitBonus +=  2; DefenseBonus += 1; ProtectionBonus += 1; DamageBonus += 1; break;
			case 3: ToHitBonus +=  4; DefenseBonus += 2; ProtectionBonus += 2; DamageBonus += 1; break;
			case 4: ToHitBonus +=  6; DefenseBonus += 3; ProtectionBonus += 3; DamageBonus += 2; break;
			case 5: ToHitBonus +=  8; DefenseBonus += 4; ProtectionBonus += 4; DamageBonus += 2; break;
			case 6: ToHitBonus += 10; DefenseBonus += 5; ProtectionBonus += 5; DamageBonus += 3; break;
			default: break; 
		}

		m_ToHit[ATTR_CURRENT]      += ToHitBonus;
		m_Defense[ATTR_CURRENT]    += DefenseBonus;
		m_Protection[ATTR_CURRENT] += ProtectionBonus;
		m_Damage[ATTR_CURRENT]     += DamageBonus;
		m_Damage[ATTR_MAX]         += DamageBonus;
	}
	*/

	/*
	printf("BareMINDamage:%d\n", m_Damage[ATTR_CURRENT]);
	printf("BareMAXDamage:%d\n", m_Damage[ATTR_MAX]);
	if (pWeapon != NULL)
	{
		printf("+WeaponMINDamage:%d\n", m_Damage[ATTR_CURRENT] + pWeapon->getMinDamage());
		printf("+WeaponMAXDamage:%d\n", m_Damage[ATTR_MAX] + pWeapon->getMaxDamage());
	}
	*/

	ComputeAllSkillDelay();
	if (isFlag(Effect::EFFECT_CLASS_FAVOROFLUNA))//slayer
	{
		EffectFAVOROFLUNA* pFAVOROFLUNA = dynamic_cast<EffectFAVOROFLUNA*>(findEffect(Effect::EFFECT_CLASS_FAVOROFLUNA));
		if ( pFAVOROFLUNA!= NULL )
		{
			int ratio =  20;
			m_Defense[ATTR_CURRENT]	-= getPercentValue(m_Defense[ATTR_CURRENT] , ratio );
			//m_HP[ATTR_CURRENT]		-= getPercentValue(m_HP[ATTR_CURRENT] , ratio );
			m_HP[ATTR_MAX]		-= getPercentValue(m_HP[ATTR_MAX] , ratio );
			m_HP[ATTR_CURRENT] = min( m_HP[ATTR_CURRENT] , m_HP[ATTR_MAX]);
		}
	}
	if ( isFlag(Effect::Effect::EFFECT_CLASS_ADV2INFINITYBLAZE) )
	{
		for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
		{
			m_Resist[i] -= getPercentValue(m_Resist[i], 20);;
		}
	}
	__END_CATCH
}

int Slayer::getBloodBibleSignOpenNum() const
{
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);

	int openNumLimit = g_pVariableManager->getVariable(PREMIUM_BLOOD_BIBLE_NUMBER);
//	if (!pGamePlayer->isPayPlaying() 
//		&& !pGamePlayer->isPremiumPlay())
	if (!pGamePlayer->isPremiumPlay()) // 20080227 ������ Ƽ�� �߰��� ����
	{
		openNumLimit = g_pVariableManager->getVariable(FREE_BLOOD_BIBLE_NUMBER);
	}

#ifdef __TEST_SERVER__
	Fame_t fame = getFame()*10;
#else
	Fame_t fame = getFame();
#endif

	uint openNum = 1;

	if ( g_pWarSystem->canApplyBloodBibleSign() )
	{
		switch( getHighestSkillDomain() )
		{
			case SKILL_DOMAIN_HEAL:
			case SKILL_DOMAIN_ENCHANT:
				{
					if ( fame < 100000 )
					{
						openNum = min( openNumLimit, 1 );
					}
					else if ( fame < 500000 )
					{
						openNum = min( openNumLimit, 2 );
					}
					else if ( fame < 2000000 )
					{
						openNum = min( openNumLimit, 3 );
					}
					else if ( fame < 4000000 )
					{
						openNum = min( openNumLimit, 4 );
					}
					else if ( fame < 60000000 )
					{
						openNum = min( openNumLimit, 5 );
					}
					else
					{
						openNum = min( openNumLimit, 6 );
					}
				}
				break;
			default:
				{
					if ( fame < 200000 )
					{
						openNum = min( openNumLimit, 1 );
					}
					else if ( fame < 1000000 )
					{
						openNum = min( openNumLimit, 2 );
					}
					else if ( fame < 5000000 )
					{
						openNum = min( openNumLimit, 3 );
					}
					else if ( fame < 10000000 )
					{
						openNum = min( openNumLimit, 4 );
					}
					else if ( fame < 100000000 )
					{
						openNum = min( openNumLimit, 5 );
					}
					else
					{
						openNum = min( openNumLimit, 6 );
					}
				}
				break;
		}
	}
	else
	{
		openNum = 0;
	}

	return openNum;
}

//////////////////////////////////////////////////////////////////////////////
// STR, DEX, INT�� ���
// CURRENT = �⺻ ��ġ + ������ ��ġ + ���� ��ġ
// MAX     = �⺻ ��ġ + ������ ��ġ
// BASIC   = �⺻ ��ġ
//
// HP, MP�� ���
// CURRENT = ���� ��ġ
// MAX     = ���� �ƽ�
// BASIC   = �����ۿ� ���� ��ȭ ��ġ
//
// Defense, Protection, ToHit�� ���
// CURRENT = ���� ��ġ
// MAX     = �����ۿ� ���� ��ȭ ��ġ
//
// Damage�� ���
// CURRENT = Min ������
// MAX     = Max ������
// BASIC   = �����ۿ� ���� ��ȭ ��ġ
//////////////////////////////////////////////////////////////////////////////
void Slayer::computeStatOffset(void) throw()
{
	__BEGIN_TRY

	Creature::CreatureClass CClass = getCreatureClass();
	BASIC_ATTR              cur_attr;

	cur_attr.nSTR    = m_STR[ATTR_CURRENT];
	cur_attr.nDEX    = m_DEX[ATTR_CURRENT];
	cur_attr.nINT    = m_INT[ATTR_CURRENT];
	cur_attr.pWeapon = m_pWearItem[WEAR_RIGHTHAND];

	for (int i=0; i<SKILL_DOMAIN_MAX; i++)
		cur_attr.pDomainLevel[i] = m_SkillDomainLevels[i];

	// ���ο��� STR, DEX, INT�� ���� ����� �� ����
	// ������ �Ǵ� ���� ��ġ�� ���Ѵ�.
	//m_HP[ATTR_MAX] = computeHP(CClass, &cur_attr);
	m_HP[ATTR_MAX] = computeTestModeHP(this, &cur_attr);
	m_HP[ATTR_MAX] += m_HP[ATTR_BASIC]; 

	m_MP[ATTR_MAX] = computeMP(CClass, &cur_attr);
	m_MP[ATTR_MAX] += m_MP[ATTR_BASIC]; 

	m_ToHit[ATTR_CURRENT] = computeToHit(CClass, &cur_attr);
	m_ToHit[ATTR_CURRENT] += m_ToHit[ATTR_MAX];

	m_Defense[ATTR_CURRENT] = computeDefense(CClass, &cur_attr);
	m_Defense[ATTR_CURRENT] += m_Defense[ATTR_MAX];

	m_Protection[ATTR_CURRENT] = computeProtection(CClass, &cur_attr);
	m_Protection[ATTR_CURRENT] += m_Protection[ATTR_MAX];

	m_Damage[ATTR_CURRENT] = computeMinDamage(CClass, &cur_attr);
	m_Damage[ATTR_MAX]     = computeMaxDamage(CClass, &cur_attr);

	m_Damage[ATTR_CURRENT] += m_Damage[ATTR_BASIC];
	m_Damage[ATTR_MAX] += m_Damage[ATTR_BASIC];

	setAttackSpeed(computeAttackSpeed(CClass, &cur_attr), ATTR_CURRENT);
	increaseAttackSpeed(getAttackSpeed(ATTR_MAX), ATTR_CURRENT);

	m_CriticalRatio[ATTR_CURRENT] = computeCriticalRatio(CClass, &cur_attr);
	m_CriticalRatio[ATTR_CURRENT] += m_CriticalRatio[ATTR_MAX];

	// 20070918
	m_MagicDefense = computeMagicDefense(CClass, &cur_attr);
	m_MagicToHit   = computeMagicToHit(CClass, &cur_attr);

	__END_CATCH
}

void Slayer::computeItemStat(Item* pItem) throw()
{
	__BEGIN_TRY

	// Debug: track per-item stat application (Bug 7 investigation)
	HP_t  prevHPMax = m_HP[ATTR_MAX];
	MP_t  prevMPMax = m_MP[ATTR_MAX];

	if (isSlayerWeapon(pItem->getItemClass()))
	{
		// ������ ���Ⱑ ������ �ӵ� �Ķ���͸� ���Ѵ�.
		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());
		increaseAttackSpeed(pItemInfo->getSpeed(), ATTR_CURRENT);
		increaseAttackSpeed(pItemInfo->getSpeed(), ATTR_MAX);
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP )
	{
		CoreZapInfo* pItemInfo = dynamic_cast<CoreZapInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType()));
		if ( pItemInfo != NULL )
		{
			computeOptionClassStat( pItemInfo->getOptionClass(), pItem->getGrade() );
		}
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_CUE_OF_ADAM )
	{
		CueOfAdamInfo* pItemInfo = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType()));

		if ( pItemInfo != NULL )
		{
			computeOptionClassStat( pItemInfo->getOptionClass() , pItem->getGrade() );
		}
	}

	m_Protection[ATTR_CURRENT] += pItem->getProtectionBonus();
	m_Protection[ATTR_MAX]     += pItem->getProtectionBonus();

	m_Defense[ATTR_CURRENT] += pItem->getDefenseBonus();
	m_Defense[ATTR_MAX]     += pItem->getDefenseBonus();

	m_ToHit[ATTR_CURRENT]   += pItem->getToHitBonus();
	m_ToHit[ATTR_MAX]       += pItem->getToHitBonus();

	m_MP[ATTR_MAX]          += pItem->getMPBonus();
	m_MP[ATTR_BASIC]        += pItem->getMPBonus();

	m_Luck					+= pItem->getLuck();

	m_MagicDamageReduce		+= pItem->getMagicProtectionBonus(); // 20080703

	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& optionType = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;
	for (itr=optionType.begin(); itr!=optionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}


#ifdef VERSION_THIRD_ENCHANT_1
	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& thirdoptionType = pItem->getThirdOptionTypeList();
	for (itr=thirdoptionType.begin(); itr!=thirdoptionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}
#endif

	// Item ��ü�� defaultOption�� ����Ų��.
	const list<OptionType_t>& defaultOptions = pItem->getDefaultOptions();
	list<OptionType_t>::const_iterator iOptions;

	for (iOptions=defaultOptions.begin(); iOptions!=defaultOptions.end(); iOptions++)
	{
		computeOptionStat( *iOptions );
	}

	// �ھ����� ��� �� ���� ������ �ɼ� ���ʽ��� �ִ�.
	if ( pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP )
	{
		bool zaps[4] = { false, false, false, false };
		for ( int i=WEAR_ZAP1; i<=WEAR_ZAP4; ++i )
		{
			Item* pCoreZap = m_pWearItem[i];
			if ( pCoreZap != NULL && m_pRealWearingCheck[i] == true && pCoreZap->getItemClass() == Item::ITEM_CLASS_CORE_ZAP && pCoreZap->getItemType()<4 )
			{
				zaps[pCoreZap->getItemType()] = true;
			}
		}
		if ( zaps[0] && zaps[1] && zaps[2] && zaps[3] )
		{
			computeOptionStat(182); //���� 9
			computeOptionStat(185); //��� 3
		}
	}

	// ť���� �ƴ��� ��� �� ���� �ְ� ���� �ٸ� ������ �ɼ� ���ʽ��� �ִ�.
	if ( pItem->getItemClass() == Item::ITEM_CLASS_CUE_OF_ADAM )
	{
		ItemType_t cues[2] = { 9999, 9999 };
		for ( int i = WEAR_CUE1; i <= WEAR_CUE2; ++i )
		{
			Item* pCueOfAdam = m_pWearItem[i];
			if ( pCueOfAdam != NULL && m_pRealWearingCheck[i] == true )
			{
				cues[i-WEAR_CUE1] = pCueOfAdam->getItemType();
			}
		}

//		if ( cues[0] != 9999 && cues[1] != 9999 && cues[0] != cues[1] )
		if ( cues[0] != 9999 && cues[1] != 9999 && cues[0] != cues[1] && cues[0] != 4 && cues[1] != 4) // 20070906 �߼��̺�Ʈ�� ����� �ŵ��� ��Ʈ ȿ�� ��� �ȵȴ�. 
		{
			CueOfAdamInfo* pItemInfo1 = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), cues[0]));
			CueOfAdamInfo* pItemInfo2 = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), cues[1]));

			if ( pItemInfo1 != NULL )
				computeOptionStat(pItemInfo1->getSetOptionType());
			if ( pItemInfo2 != NULL )
				computeOptionStat(pItemInfo2->getSetOptionType());
		}
	}

	__END_CATCH
}

void Slayer::computeOptionStat(Item* pItem) throw()
{
	__BEGIN_TRY

	// Option Type�� �޾ƿ´�.
//	OptionType_t  OptionType    = pItem->getOptionType();
//	computeOptionStat( OptionType );

	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& optionType = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;
	for (itr=optionType.begin(); itr!=optionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}

	// Item ��ü�� defaultOption�� ����Ų��.
	const list<OptionType_t>& defaultOptions = pItem->getDefaultOptions();
	list<OptionType_t>::const_iterator iOptions;

	for (iOptions=defaultOptions.begin(); iOptions!=defaultOptions.end(); iOptions++)
	{
		computeOptionStat( *iOptions );
	}



	__END_CATCH
}

void Slayer::computeOptionClassStat( OptionClass OClass, int PlusPoint )
{
	switch (OClass) 
	{
		case OPTION_STR:
			m_STR[ATTR_CURRENT] += PlusPoint;
			m_STR[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_DEX:
			m_DEX[ATTR_CURRENT] += PlusPoint;
			m_DEX[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_INT:
			m_INT[ATTR_CURRENT] += PlusPoint;
			m_INT[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_HP:
			m_HP[ATTR_MAX]   += PlusPoint;
			m_HP[ATTR_BASIC] += PlusPoint;
			break;
		case OPTION_MP:
			m_MP[ATTR_MAX]   += PlusPoint;
			m_MP[ATTR_BASIC] += PlusPoint;
			break;
		case OPTION_HP_STEAL:
			m_HPStealAmount += PlusPoint;
			break;
		case OPTION_MP_STEAL:
			m_MPStealAmount += PlusPoint;
			break;
		case OPTION_HP_REGEN:
			m_HPRegen += PlusPoint;
			break;
		case OPTION_MP_REGEN:
			m_MPRegen += PlusPoint;
			break;
		case OPTION_TOHIT:
			m_ToHit[ATTR_CURRENT] += PlusPoint;
			m_ToHit[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_DEFENSE:
			m_Defense[ATTR_CURRENT] += PlusPoint;
			m_Defense[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_DAMAGE:
			m_Damage[ATTR_CURRENT] += PlusPoint;
			m_Damage[ATTR_MAX]     += PlusPoint;
			m_Damage[ATTR_BASIC]   += PlusPoint;
			break;
		case OPTION_PROTECTION:
			m_Protection[ATTR_CURRENT] += PlusPoint;
			m_Protection[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_ATTACK_SPEED:
			increaseAttackSpeed(PlusPoint, ATTR_CURRENT);
			increaseAttackSpeed(PlusPoint, ATTR_MAX);
			break;
		case OPTION_POISON:
			m_Resist[MAGIC_DOMAIN_POISON] += PlusPoint;
			break;
		case OPTION_ACID:
			m_Resist[MAGIC_DOMAIN_ACID] += PlusPoint;
			break;
		case OPTION_CURSE:
			m_Resist[MAGIC_DOMAIN_CURSE] += PlusPoint;
			break;
		case OPTION_BLOOD:
			m_Resist[MAGIC_DOMAIN_BLOOD] += PlusPoint;
			break;
		case OPTION_VISION:
			break;
		case OPTION_CRITICAL_HIT:
			m_CriticalRatio[ATTR_CURRENT] += PlusPoint;
			m_CriticalRatio[ATTR_MAX]     += PlusPoint;
			break;

		case OPTION_ALL_ATTR:
			m_STR[ATTR_CURRENT] += PlusPoint;
			m_STR[ATTR_MAX]     += PlusPoint;

			m_DEX[ATTR_CURRENT] += PlusPoint;
			m_DEX[ATTR_MAX]     += PlusPoint;

			m_INT[ATTR_CURRENT] += PlusPoint;
			m_INT[ATTR_MAX]     += PlusPoint;

			computeStatOffset();
			break;

		case OPTION_ALL_RES:
			m_Resist[MAGIC_DOMAIN_POISON] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_ACID] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_CURSE] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_BLOOD] += PlusPoint;
///////////////////////////////////////////////////////////////////////////
//	������ ��� ����Ʈ�� ����...			
			m_Resist[MAGIC_DOMAIN_ALLRES] += PlusPoint;
			
			break;

		case OPTION_LUCK:
			m_Luck += PlusPoint;
		break;

		case OPTION_STR_TO_DEX:
		{
			int trans = getPercentValue( m_STR[ATTR_BASIC] + m_AdvancedSTR[ATTR_BASIC], PlusPoint );
			m_STR[ATTR_CURRENT]	-= trans;
			m_STR[ATTR_MAX]		-= trans;
			m_DEX[ATTR_CURRENT]	+= trans;
			m_DEX[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_STR_TO_INT:
		{
			int trans = getPercentValue( m_STR[ATTR_BASIC] + m_AdvancedSTR[ATTR_BASIC], PlusPoint );
			m_STR[ATTR_CURRENT]	-= trans;
			m_STR[ATTR_MAX]		-= trans;
			m_INT[ATTR_CURRENT] += trans;
			m_INT[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_DEX_TO_STR:
		{
			int trans = getPercentValue( m_DEX[ATTR_BASIC] + m_AdvancedDEX[ATTR_BASIC], PlusPoint );
			m_DEX[ATTR_CURRENT]	-= trans;
			m_DEX[ATTR_MAX]		-= trans;
			m_STR[ATTR_CURRENT] += trans;
			m_STR[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_DEX_TO_INT:
		{
			int trans = getPercentValue( m_DEX[ATTR_BASIC] + m_AdvancedDEX[ATTR_BASIC], PlusPoint );
			m_DEX[ATTR_CURRENT]	-= trans;
			m_DEX[ATTR_MAX]		-= trans;
			m_INT[ATTR_CURRENT] += trans;
			m_INT[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_INT_TO_STR:
		{
			int trans = getPercentValue( m_INT[ATTR_BASIC] + m_AdvancedINT[ATTR_BASIC], PlusPoint );
			m_INT[ATTR_CURRENT]	-= trans;
			m_INT[ATTR_MAX]		-= trans;
			m_STR[ATTR_CURRENT] += trans;
			m_STR[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_INT_TO_DEX:
		{
			int trans = getPercentValue( m_INT[ATTR_BASIC] + m_AdvancedINT[ATTR_BASIC], PlusPoint );
			m_INT[ATTR_CURRENT]	-= trans;
			m_INT[ATTR_MAX]		-= trans;
			m_DEX[ATTR_CURRENT] += trans;
			m_DEX[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}
		case OPTION_CONSUME_MP:
		{
			m_ConsumeMPRatio += PlusPoint;
			break;
		}
		case OPTION_MAGIC_DAMAGE:
		{
			m_MagicBonusDamage += PlusPoint;
			break;
		}
		case OPTION_PHYSIC_DAMAGE:
		{
			m_PhysicBonusDamage += PlusPoint;
			break;
		}
		case OPTION_GAMBLE_PRICE:
		{
			m_GamblePriceRatio += PlusPoint;
			break;
		}
		case OPTION_POTION_PRICE:
		{
			m_PotionPriceRatio += PlusPoint;
			break;
		}
		case OPTION_PHYSIC_PRO:
		{
			m_PhysicDamageReduce += PlusPoint;
			break;
		}
		case OPTION_MAGIC_PRO:
		{
			m_MagicDamageReduce += PlusPoint;
			break;
		}

		default:
			break;
	}

}

void Slayer::computeOptionStat(OptionType_t optionType) throw()
{
	__BEGIN_TRY

	OptionInfo*   pOptionInfo   = g_pOptionInfoManager->getOptionInfo(optionType);
	if ( pOptionInfo == NULL )
	{
		filelog("InvalidOptionInfo.log", "Slayer::computeOptionStat() Name=%s InvalidOptionType=%d",
			getName().c_str(), optionType
		);
		return;
	}

	computeOptionClassStat( pOptionInfo->getClass(), pOptionInfo->getPlusPoint() );
//	OptionClass   OClass        = pOptionInfo->getClass();

/*	switch (OClass) 
	{
		case OPTION_STR:
			m_STR[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_STR[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_DEX:
			m_DEX[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_DEX[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_INT:
			m_INT[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_INT[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_HP:
			m_HP[ATTR_MAX]   += pOptionInfo->getPlusPoint();
			m_HP[ATTR_BASIC] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_MP:
			m_MP[ATTR_MAX]   += pOptionInfo->getPlusPoint();
			m_MP[ATTR_BASIC] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_HP_STEAL:
			m_HPStealAmount += pOptionInfo->getPlusPoint();
			break;
		case OPTION_MP_STEAL:
			m_MPStealAmount += pOptionInfo->getPlusPoint();
			break;
		case OPTION_HP_REGEN:
			m_HPRegen += pOptionInfo->getPlusPoint();
			break;
		case OPTION_MP_REGEN:
			m_MPRegen += pOptionInfo->getPlusPoint();
			break;
		case OPTION_TOHIT:
			m_ToHit[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_ToHit[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_DEFENSE:
			m_Defense[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Defense[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_DAMAGE:
			m_Damage[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Damage[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			m_Damage[ATTR_BASIC]   += pOptionInfo->getPlusPoint();
			break;
		case OPTION_PROTECTION:
			m_Protection[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Protection[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_ATTACK_SPEED:
			m_AttackSpeed[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_AttackSpeed[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_POISON:
			m_Resist[MAGIC_DOMAIN_POISON] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_ACID:
			m_Resist[MAGIC_DOMAIN_ACID] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_CURSE:
			m_Resist[MAGIC_DOMAIN_CURSE] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_BLOOD:
			m_Resist[MAGIC_DOMAIN_BLOOD] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_VISION:
			break;
		case OPTION_CRITICAL_HIT:
			m_CriticalRatio[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_CriticalRatio[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;

		case OPTION_ALL_ATTR:
			m_STR[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_STR[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			m_DEX[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_DEX[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			m_INT[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_INT[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			computeStatOffset();
			break;

		case OPTION_ALL_RES:
			m_Resist[MAGIC_DOMAIN_POISON] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_ACID] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_CURSE] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_BLOOD] += pOptionInfo->getPlusPoint();
			break;

		case OPTION_LUCK:
			m_Luck += pOptionInfo->getPlusPoint();
		break;

		case OPTION_STR_TO_DEX:
		{
			int trans = getPercentValue( m_STR[ATTR_BASIC], pOptionInfo->getPlusPoint() );
			m_STR[ATTR_CURRENT]	-= trans;
			m_STR[ATTR_MAX]		-= trans;
			m_DEX[ATTR_CURRENT]	+= trans;
			m_DEX[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_STR_TO_INT:
		{
			int trans = getPercentValue( m_STR[ATTR_BASIC], pOptionInfo->getPlusPoint() );
			m_STR[ATTR_CURRENT]	-= trans;
			m_STR[ATTR_MAX]		-= trans;
			m_INT[ATTR_CURRENT] += trans;
			m_INT[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_DEX_TO_STR:
		{
			int trans = getPercentValue( m_DEX[ATTR_BASIC], pOptionInfo->getPlusPoint() );
			m_DEX[ATTR_CURRENT]	-= trans;
			m_DEX[ATTR_MAX]		-= trans;
			m_STR[ATTR_CURRENT] += trans;
			m_STR[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_DEX_TO_INT:
		{
			int trans = getPercentValue( m_DEX[ATTR_BASIC], pOptionInfo->getPlusPoint() );
			m_DEX[ATTR_CURRENT]	-= trans;
			m_DEX[ATTR_MAX]		-= trans;
			m_INT[ATTR_CURRENT] += trans;
			m_INT[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_INT_TO_STR:
		{
			int trans = getPercentValue( m_INT[ATTR_BASIC], pOptionInfo->getPlusPoint() );
			m_INT[ATTR_CURRENT]	-= trans;
			m_INT[ATTR_MAX]		-= trans;
			m_STR[ATTR_CURRENT] += trans;
			m_STR[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}

		case OPTION_INT_TO_DEX:
		{
			int trans = getPercentValue( m_INT[ATTR_BASIC], pOptionInfo->getPlusPoint() );
			m_INT[ATTR_CURRENT]	-= trans;
			m_INT[ATTR_MAX]		-= trans;
			m_DEX[ATTR_CURRENT] += trans;
			m_DEX[ATTR_MAX]		+= trans;

			computeStatOffset();
			break;
		}
		case OPTION_CONSUME_MP:
		{
			m_ConsumeMPRatio = pOptionInfo->getPlusPoint();
			break;
		}
		case OPTION_MAGIC_DAMAGE:
		{
			m_MagicBonusDamage = pOptionInfo->getPlusPoint();
			break;
		}
		case OPTION_PHYSIC_DAMAGE:
		{
			m_PhysicBonusDamage = pOptionInfo->getPlusPoint();
			break;
		}
		case OPTION_GAMBLE_PRICE:
		{
			m_GamblePriceRatio = pOptionInfo->getPlusPoint();
			break;
		}
		case OPTION_POTION_PRICE:
		{
			m_PotionPriceRatio = pOptionInfo->getPlusPoint();
			break;
		}

		default:
			break;
	}*/

	__END_CATCH
}

void Slayer::addModifyInfo(const SLAYER_RECORD& prev, ModifyInfo& pkt) const
	throw()
{
	__BEGIN_TRY

	if (prev.pSTR[ATTR_CURRENT] != m_STR[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_STR, m_STR[ATTR_CURRENT]);
	if (prev.pSTR[ATTR_MAX    ] != m_STR[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_STR,     m_STR[ATTR_MAX]);
	if (prev.pSTR[ATTR_BASIC  ] != m_STR[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_STR,   m_STR[ATTR_BASIC]);

	if (prev.pDEX[ATTR_CURRENT] != m_DEX[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_DEX, m_DEX[ATTR_CURRENT]);
	if (prev.pDEX[ATTR_MAX    ] != m_DEX[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_DEX,     m_DEX[ATTR_MAX]);
	if (prev.pDEX[ATTR_BASIC  ] != m_DEX[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_DEX,   m_DEX[ATTR_BASIC]);

	if (prev.pINT[ATTR_CURRENT] != m_INT[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_INT, m_INT[ATTR_CURRENT]);
	if (prev.pINT[ATTR_MAX    ] != m_INT[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_INT,     m_INT[ATTR_MAX]);
	if (prev.pINT[ATTR_BASIC  ] != m_INT[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_INT,   m_INT[ATTR_BASIC]);

	if (prev.pHP[ATTR_MAX    ] != m_HP[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_HP,     m_HP[ATTR_MAX]);
	if (prev.pHP[ATTR_CURRENT] != m_HP[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_HP, m_HP[ATTR_CURRENT]);

	if (prev.pMP[ATTR_MAX    ] != m_MP[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_MP,     m_MP[ATTR_MAX]);
	if (prev.pMP[ATTR_CURRENT] != m_MP[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_MP, m_MP[ATTR_CURRENT]);

	if (prev.pDamage[ATTR_CURRENT] != m_Damage[ATTR_CURRENT]) pkt.addShortData(MODIFY_MIN_DAMAGE, m_Damage[ATTR_CURRENT]);
	if (prev.pDamage[ATTR_MAX    ] != m_Damage[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_DAMAGE, m_Damage[ATTR_MAX]);

	if (prev.Defense     != m_Defense[ATTR_CURRENT])     pkt.addShortData(MODIFY_DEFENSE,      m_Defense[ATTR_CURRENT]);
	if (prev.Protection  != m_Protection[ATTR_CURRENT])  pkt.addShortData(MODIFY_PROTECTION,   m_Protection[ATTR_CURRENT]);
	if (prev.ToHit       != m_ToHit[ATTR_CURRENT])       pkt.addShortData(MODIFY_TOHIT,        m_ToHit[ATTR_CURRENT]);
	if (prev.AttackSpeed != getAttackSpeed(ATTR_CURRENT)) 
	{
		pkt.addShortData(MODIFY_ATTACK_SPEED, getAttackSpeed(ATTR_CURRENT));
	}

	// by sigi. 2002.9.10
	if (prev.Rank != getRank())
	{
		pkt.addShortData(MODIFY_RANK, getRank());
		pkt.addLongData(MODIFY_RANK_EXP, getRankGoalExp());
	}

	if (prev.Bonus != m_AdvancedAttrBonus)
	{
		pkt.addShortData(MODIFY_BONUS_POINT, m_AdvancedAttrBonus);
	}

	if(prev.pAdvancedSTR[ATTR_CURRENT] != m_AdvancedSTR[ATTR_CURRENT]) pkt.addShortData(MODIFY_ADVANCED_STR, m_AdvancedSTR[ATTR_BASIC]);
	if(prev.pAdvancedDEX[ATTR_CURRENT] != m_AdvancedDEX[ATTR_CURRENT]) pkt.addShortData(MODIFY_ADVANCED_DEX, m_AdvancedDEX[ATTR_BASIC]);
	if(prev.pAdvancedINT[ATTR_CURRENT] != m_AdvancedINT[ATTR_CURRENT]) pkt.addShortData(MODIFY_ADVANCED_INT, m_AdvancedINT[ATTR_BASIC]);
#ifdef __CONTRIBUTION_SYSTEM__
	if(prev.ContributePoint != m_ContributePoint)
		pkt.addLongData(MODIFY_CONTRIBUTE_POINT, m_ContributePoint);
#endif
	if (prev.Luck != m_Luck)
	{
		pkt.addShortData(MODIFY_LUCK, m_Luck);
	}
	if (prev.HpSteal != m_HPStealAmount)
	{
		pkt.addShortData(MODIFY_HPSTEAL, m_HPStealAmount);
	}
	if (prev.MpSteal != m_MPStealAmount)
	{
		pkt.addShortData(MODIFY_MPSTEAL, m_MPStealAmount);
	}
	if (prev.AllRes != m_Resist[MAGIC_DOMAIN_ALLRES])
	{
		pkt.addShortData(MODIFY_ALLRES, m_Resist[MAGIC_DOMAIN_ALLRES]);
	}
	if (prev.Cri != m_CriticalRatio[ATTR_MAX])
	{
		pkt.addShortData(MODIFY_CRI, m_CriticalRatio[ATTR_MAX]);
	}

	__END_CATCH
}

void Slayer::sendModifyInfo(const SLAYER_RECORD& prev) const
	throw()
{
	__BEGIN_TRY

	GCModifyInformation gcModifyInformation;
	addModifyInfo(prev, gcModifyInformation);
	m_pPlayer->sendPacket(&gcModifyInformation);

	BloodBibleSignInfo* pInfo = getBloodBibleSign();
	GCBloodBibleSignInfo gcInfo;
	gcInfo.setSignInfo( pInfo );
//	cout << "open num : " << pInfo->getOpenNum() << endl;;
	m_pPlayer->sendPacket( &gcInfo );

	__END_CATCH
}

void Slayer::initAllStatAndSend()
{
	SLAYER_RECORD prev;
	getSlayerRecord(prev);
	initAllStat();
	sendRealWearingInfo();
	sendModifyInfo(prev);
	//sendSkillInfo();
}

//////////////////////////////////////////////////////////////////////////////
// 
// �����̾�
//
//////////////////////////////////////////////////////////////////////////////

void Vampire::initCastleSkill() throw()
{
	__BEGIN_TRY

	removeAllCastleSkill();

	if ( !getZone()->isHolyLand() ) return;

	list<CastleInfo*> pCastleInfoList = g_pCastleInfoManager->getGuildCastleInfos( getGuildID() );
	if ( pCastleInfoList.empty() ) return;

	list<CastleInfo*>::iterator itr = pCastleInfoList.begin();

	for ( ; itr != pCastleInfoList.end() ; itr++ )
	{
		SkillType_t CastleSkillType = g_pCastleInfoManager->getCastleSkillType( (*itr)->getZoneID(), getGuildID() );
		if ( CastleSkillType == SKILL_MAX ) 
			continue;

		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(CastleSkillType);
		Assert( pSkillInfo != NULL );

		Turn_t Delay = pSkillInfo->getMaxDelay();

		VampireCastleSkillSlot* pCastleSkillSlot = new VampireCastleSkillSlot();

		pCastleSkillSlot->setName( m_Name );
		pCastleSkillSlot->setSkillType( CastleSkillType );
		pCastleSkillSlot->setInterval(Delay);
		pCastleSkillSlot->setRunTime();

		addSkill( pCastleSkillSlot );
	}

	__END_CATCH
}

void Vampire::initAllStat(int numPartyMember)
	throw()
{
	__BEGIN_TRY

	BASIC_ATTR attr;
	Creature::CreatureClass CClass = getCreatureClass();

	m_Resist[MAGIC_DOMAIN_NO_DOMAIN] = 0;
	m_Resist[MAGIC_DOMAIN_POISON]    = 50;
	m_Resist[MAGIC_DOMAIN_ACID]      = 0;
	m_Resist[MAGIC_DOMAIN_CURSE]     = 0;
	m_Resist[MAGIC_DOMAIN_BLOOD]     = 0;
////////////////////////////////////////////////////////////////////////
//	������ ��� ���� �ʱ�ȭ	
	m_Resist[MAGIC_DOMAIN_ALLRES]    = 0;

	m_Mastery[MAGIC_DOMAIN_NO_DOMAIN] = 0;
	m_Mastery[MAGIC_DOMAIN_POISON]    = 0;
	m_Mastery[MAGIC_DOMAIN_ACID]      = 0;
	m_Mastery[MAGIC_DOMAIN_CURSE]     = 0;
	m_Mastery[MAGIC_DOMAIN_BLOOD]     = 0;

	// BloodBible ��� ���ʽ� ��ġ�� �ʱ�ȭ
	m_ConsumeMPRatio	= 0;
	m_GamblePriceRatio	= 0;
	m_PotionPriceRatio	= 0;
	m_MagicBonusDamage	= 0;
	m_PhysicBonusDamage	= 0;
	m_MagicDamageReduce = 0;
	m_PhysicDamageReduce= 0;

	//////////////////////////////////////////////////////////////////////////////
	// ���� ���� �⺻ �ɷ�ġ�� �ʱ�ȭ��Ű��, 
	// �⺻ �ɷ�ġ�� ������ �ִ� ����Ʈ�� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	m_STR[ATTR_CURRENT] = m_STR[ATTR_MAX] = m_STR[ATTR_BASIC];
	m_DEX[ATTR_CURRENT] = m_DEX[ATTR_MAX] = m_DEX[ATTR_BASIC];
	m_INT[ATTR_CURRENT] = m_INT[ATTR_MAX] = m_INT[ATTR_BASIC];

	//////////////////////////////////////////////////////////////////////////////
	// �ɷ�ġ ����� ���� �Ķ���͵��� �ʱ�ȭ�Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	attr.nSTR    = m_STR[ATTR_CURRENT];
	attr.nDEX    = m_DEX[ATTR_CURRENT];
	attr.nINT    = m_INT[ATTR_CURRENT];
	attr.pWeapon = NULL;
	attr.nLevel  = m_Level;
	
	m_HPStealRatio  = 0;
	m_HPStealAmount = 0;
	m_HPRegen       = 0;
	m_Luck			= m_BaseLuck;
//	cout << getName() << "�� �⺻ ��� : " << m_Luck << endl;
	m_HPRegenBonus	= 0;

	////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ���� �ٽ� ����Ѵ�.
	////////////////////////////////////////////////////////////
	//m_HP[ATTR_MAX]                = computeHP(CClass, &attr);
	m_HP[ATTR_MAX]                = computeTestModeHP(this, &attr);
	m_HP[ATTR_BASIC]              = 0;
	m_ToHit[ATTR_CURRENT]         = computeToHit(CClass, &attr);
	m_ToHit[ATTR_MAX]             = 0;
	m_Defense[ATTR_CURRENT]       = computeDefense(CClass, &attr);
	m_Defense[ATTR_MAX]           = 0;
	m_Protection[ATTR_CURRENT]    = computeProtection(CClass, &attr);
	m_Protection[ATTR_MAX]        = 0;
	m_Damage[ATTR_CURRENT]        = computeMinDamage(CClass, &attr);
	m_Damage[ATTR_MAX]            = computeMaxDamage(CClass, &attr);
	m_Damage[ATTR_BASIC]          = 0;
	setAttackSpeed(computeAttackSpeed(CClass, &attr), ATTR_CURRENT);
	setAttackSpeed(0, ATTR_MAX);
	m_CriticalRatio[ATTR_CURRENT] = computeCriticalRatio(CClass, &attr);
	m_CriticalRatio[ATTR_MAX]     = 0;
	// 20070918
	m_MagicDefense				  = computeMagicDefense(CClass, &attr);
	m_MagicToHit				  = computeMagicToHit(CClass, &attr);

	int RaceWarHPBonus = 0;

	if ( m_pZone->isHolyLand() || m_pZone->isLevelWarZone() )
	{
		RaceWarHPBonus = getPercentValue( m_HP[ATTR_MAX], g_pVariableManager->getRaceWarHPBonus() );
	}

	int DragonEyeHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_DRAGON_EYE ) )
	{
		// HP ���ʽ��� �ι�
		DragonEyeHPBonus = m_HP[ATTR_MAX];
	}

#ifdef __CONTRIBUTION_SYSTEM__
	int HeroItemHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_VAMPIRE_HERO_ITEM ) )
	{
		// HP ���ʽ��� �ټ��� 
	          HeroItemHPBonus = m_HP[ATTR_MAX] * 2; // by svi 4->2
	}

	int LevelHeroItemHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_VAMPIRE_LEVELWAR_HERO_ITEM ) )
	{
		// HP ���ʽ��� �ټ��� 
		LevelHeroItemHPBonus = m_HP[ATTR_MAX] * 2;
	}
#endif

	// ���� ���ʽ�
	// ������ ���� ���п� ������ ������̵� ���ʽ��� ���� �� �ִ�. by sigi
	//if ( g_pCombatInfoManager->isVampireBonus() )
	int HPBonus = 0;
	{
		int bonusRatio = g_pVariableManager->getCombatVampireHPBonusRatio();
		//g_pCombatInfoManager->getVampireHPModify();

		if (bonusRatio > 0)
		{
			HPBonus = getPercentValue( m_HP[ATTR_MAX], bonusRatio );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// �ϴ� ��� üũ ������ �ʱ�ȭ�ؼ� ��� ��� �� ���� ������ �����ϰ� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	bool pOldRealWearingCheck[VAMPIRE_WEAR_MAX];	// by sigi. 2002.10.31
	for (int i=0; i<VAMPIRE_WEAR_MAX; i++) 
	{
		pOldRealWearingCheck[i] = m_pRealWearingCheck[i];
		m_pRealWearingCheck[i] = false;
	}

	//////////////////////////////////////////////////////////////////////////////
	// ���� ������ ������ ���ʽ� �ɼ��� �ް� �ȴ�
	//////////////////////////////////////////////////////////////////////////////
	// Blood Bible ������ ���ʽ� �ɼ��� �޴� �ɷ� ���ƴ�.
/*	if (m_pZone->isHolyLand() )
	{
		const list<OptionType_t>& optionType = g_pHolyLandRaceBonus->getVampireOptionTypeList();
		list<OptionType_t>::const_iterator itr;
		for (itr=optionType.begin(); itr!=optionType.end(); itr++)
		{
			computeOptionStat( *itr );
		}
	}
*/
	//////////////////////////////////////////////////////////////////////////////
	// Blood Bilbe ������ ���ʽ� �ɼ��� �޴´�.
	//////////////////////////////////////////////////////////////////////////////
/*	if ( m_pZone->isHolyLand() && !g_pWarSystem->hasActiveRaceWar() )
	{
		const BloodBibleBonusHashMap& bloodBibleBonus = g_pBloodBibleBonusManager->getBloodBibleBonuses();
		BloodBibleBonusHashMapConstItor itr;
		for (itr=bloodBibleBonus.begin(); itr!=bloodBibleBonus.end(); itr++)
		{
			if ( itr->second->getRace() == RACE_VAMPIRE )
			{
				OptionTypeList optionTypes = itr->second->getOptionTypeList();
				OptionTypeListConstItor optionItr;
				
				for ( optionItr = optionTypes.begin(); optionItr != optionTypes.end(); optionItr++ )
				{
					computeOptionStat( *optionItr );
				}
			}
		}
	}*/
	if ( g_pSweeperBonusManager->isAble( getZoneID() ) && g_pLevelWarZoneInfoManager->isCreatureBonusZone( this, getZoneID() ))
//	2007 01 03
//	if ( g_pLevelWarZoneInfoManager->isCreatureBonusZone( this, getZoneID() ))
	{
		const SweeperBonusHashMap& sweeperBonuses = g_pSweeperBonusManager->getSweeperBonuses();

		SweeperBonusHashMapConstItor itr = sweeperBonuses.begin();
		SweeperBonusHashMapConstItor endItr = sweeperBonuses.end();

		for (; itr!=endItr; itr++)
		{
			if ( itr->second->getRace() == RACE_VAMPIRE && itr->second->getLevel() == g_pLevelWarZoneInfoManager->getCreatureLevelGrade( this ) )
			{
				OptionTypeList optionTypes = itr->second->getOptionTypeList();
				OptionTypeListConstItor optionItr;
				
				for ( optionItr = optionTypes.begin(); optionItr != optionTypes.end(); optionItr++ )
				{
					computeOptionStat( *optionItr );
				}
			}
		}
	} 

	//////////////////////////////////////////////////////////////////////////////
	// �⺻������ ������ �ִ� �ɼǵ��� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	list<DefaultOptionSetType_t>::iterator itr = m_DefaultOptionSet.begin();
	for ( ; itr != m_DefaultOptionSet.end(); itr++ )
	{
		DefaultOptionSetInfo* pDefaultOptionSetInfo = g_pDefaultOptionSetInfoManager->getDefaultOptionSetInfo( (*itr) );
		if ( pDefaultOptionSetInfo != NULL )
		{
			const list<OptionType_t>& optionList = pDefaultOptionSetInfo->getOptionTypeList();
			list<OptionType_t>::const_iterator citr;
			for ( citr = optionList.begin(); citr != optionList.end(); citr++ )
			{
				computeOptionStat( *citr );
			}
		}
	}

	list<OptionType_t>::iterator oitr = m_DefaultOptions.begin();
	list<OptionType_t>::iterator oend = m_DefaultOptions.end();
	for ( ; oitr != oend; ++oitr )
	{
		computeOptionStat( *oitr );
	}

	HashMapObjectOptionConstItor ooitr = m_EffectOptions.begin();
	HashMapObjectOptionConstItor ooendItr = m_EffectOptions.end();

	for ( ; ooitr != ooendItr; ++ooitr )
	{
		computeOptionStat( ooitr->second );
	}

	//////////////////////////////////////////////////////////////////////////////
	// ���� �ִ� ���ʽ��� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	if ( m_pPetInfo != NULL )
	{
		if ( m_pPetInfo->getPetAttr() != 0xff ) computeOptionClassStat( (OptionClass)m_pPetInfo->getPetAttr(), (int)m_pPetInfo->getPetAttrLevel() );
		if ( m_pPetInfo->getPetOption() != 0 ) computeOptionStat( m_pPetInfo->getPetOption() );
		//1203 ~ 1204 wlzzi Pet's OptionType �߰�
		const list<OptionType_t>& optionList = m_pPetInfo->getOptionType();
		list<OptionType_t>::const_iterator itr;
		for ( itr = optionList.begin(); itr != optionList.end(); itr++ )
		{
			computeOptionStat( *itr );
		}
		if ( m_pPetInfo->getPetOption2() != 0 ) computeOptionStat( m_pPetInfo->getPetOption2() );
		if ( m_pPetInfo->getPetOption3() != 0 ) computeOptionStat( m_pPetInfo->getPetOption3() );
		//-
		//20090109 wlzzi - Ǫ��������
		switch (m_pPetInfo->getFunctionPetfoodType ())
		{
		case FOOD_FUNCTION_LUCKY:
			computeOptionClassStat ((OptionClass)21, 10);	//���+10 .. ���� �� ������.. �Ѥ�;�۾����� �ϴ� �ּ�ȭ �Ѵ�.
			break;
		}
		//-
		//20090601 ksym555
		const list<OptionType_t>& MixoptionList = m_pPetInfo->getMixOptionType();
		list<OptionType_t>::const_iterator itr2;
		for ( itr2 = MixoptionList.begin(); itr2 != MixoptionList.end(); itr2++ )
		{
			computeOptionStat( *itr2 );
		}
	}

	for(int i=NewRankGem::SMALL_RED_STR;i<NewRankGem::SKILL_BLADE_STORM_DAM;i++)
   	{
		if(hasRankGem((NewRankGem::RankGemType)i))
		{
			NewRankGem* pRankGem = getRankGem((NewRankGem::RankGemType)i);
			Assert( pRankGem != NULL );
			computeOptionClassStat(pRankGem->getOptionClass(), pRankGem->getGrade());
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_TaoZhuang))//vampire
	{
		EffectTaoZhuang* pEffect = dynamic_cast<EffectTaoZhuang*>(findEffect(Effect::EFFECT_CLASS_TaoZhuang));
		if (pEffect!= NULL)
		{
			/*int tz211[6][2] =
				{
					{23,5},//+5
					{3,5000},//HP+1000
					{32,10},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz221[6][2] =
				{
					{23,10},//+5
					{3,6000},//HP+1000
					{32,20},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz231[6][2] =
				{
					{23,15},//+5
					{3,7000},//HP+1000
					{32,30},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz241[6][2] =
				{
					{23,20},//+5
					{3,8000},//HP+1000
					{32,50},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};*/
			int tz211[6][2] =
				{
					{23,20},//+5
					{3,5000},//HP+1000
					{32,30},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz221[6][2] =
				{
					{23,30},//+5
					{3,6000},//HP+1000
					{32,40},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz231[6][2] =
				{
					{23,40},//+5
					{3,7000},//HP+1000
					{32,50},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz241[6][2] =
				{
					{23,50},//+5
					{3,8000},//HP+1000
					{32,60},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz251[6][2] =
				{
					{23,60},//+5
					{3,9000},//HP+1000
					{32,70},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int TzGourp = pEffect->getTzGroup();
			int Num 	= pEffect->getNum();
				for(int i = 0;i<(Num-1);i++)
				{
					if(tz211[i][0]>0 && TzGourp == 1)
					{
					  computeOptionClassStat ((OptionClass)tz211[i][0], tz211[i][1]);
					}
					if(tz221[i][0]>0 && TzGourp == 2)
					{
					  computeOptionClassStat ((OptionClass)tz221[i][0], tz221[i][1]);
					}
					if(tz231[i][0]>0 && TzGourp == 3)
					{
					  computeOptionClassStat ((OptionClass)tz231[i][0], tz231[i][1]);
					}
					if(tz241[i][0]>0 && TzGourp == 4)
					{
					  computeOptionClassStat ((OptionClass)tz241[i][0], tz241[i][1]);
					}
					if(tz251[i][0]>0 && TzGourp == 5)
					{
					  computeOptionClassStat ((OptionClass)tz251[i][0], tz251[i][1]);
					}
				}
		}
	}	
	if ( isFlag(Effect::EFFECT_CLASS_STRENGTHEN_INCENSE) )
	{
		computeOptionClassStat( OPTION_ALL_ATTR, 3 );
		HPBonus += 20;
	}

	// 2007 09 03
/*	if ( isFlag( Effect:: EFFECT_CLASS_BLOODY_SHOUT) )
	{
		EffectBloodyShout* pEffect = dynamic_cast<EffectBloodyShout*>(findEffect(Effect::EFFECT_CLASS_BLOODY_SHOUT));
		HP_t hpBonus = pEffect->getHPBonus();

		m_HP[ATTR_MAX] 			 = min(VAMPIRE_MAX_HP, m_HP[ATTR_MAX] + hpBonus );
//		m_HP[ATTR_CURRENT] 		 = min(VAMPIRE_MAX_HP, m_HP[ATTR_MAX] + hpBonus );
	}*/

	// ���� ���� ��� ���
	applyBloodBibleSign();
	CheckValidWearingItem();

	// 2006.09.08 by Shinobi
	// list<OptionType_t> ContractOptionList = getContractOfBloodPositionByItemType( (PlayerCreature*)this );
	list<OptionType_t> ContractOptionList = getContractOfBloodPositionByItemType( getInventory() );
	list<OptionType_t>::iterator ContractOptionIter = ContractOptionList.begin();

	for(; ContractOptionIter != ContractOptionList.end(); ++ContractOptionIter) 
	{
		computeOptionStat(*ContractOptionIter);
	}

	// by sigi. 2002.11.6
	bool bSendPacket = false;

	if (m_pPlayer!=NULL)
	{	
		bSendPacket = (dynamic_cast<GamePlayer*>(m_pPlayer)->getPlayerStatus()==GPS_NORMAL);
	}

	SendChangeShapes(bSendPacket);

	//////////////////////////////////////////////////////////////////////////////
	// HP, MP ��ƿ Ȯ���� ����� �д�.
	//////////////////////////////////////////////////////////////////////////////
	m_HPStealRatio = computeStealRatio(CClass, m_HPStealAmount, &attr);

	 // 2006.06.19 ���� ��ũ�� ���� ��ų�� �ɷ�ġ����� �ʵǴ°� ������ �̵�
	 // STR+15
	if ( isFlag(Effect::EFFECT_CLASS_GREYFELL_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_STR, 15);
	}
	// DEX+15
	if ( isFlag(Effect::EFFECT_CLASS_SOVIUS_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_DEX, 15);
	}
	// INT+15
	if ( isFlag(Effect::EFFECT_CLASS_MIR_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_INT, 15);
	}
	// 20081028 wlzzi - Extreme Force Scroll
	if ( isFlag(Effect::EFFECT_CLASS_EXTREME_FORCE_SCROLL))
	{
		computeOptionClassStat (OPTION_ALL_ATTR, 8);
	}
	// 2007 01 16 �̽��׸� ���� ����Ʈ ��� �߰�
	if (isFlag(Effect::EFFECT_CLASS_MYSTERY_POTION))
	{
		EffectMysteryPotion* pEffect = dynamic_cast<EffectMysteryPotion*>(findEffect(Effect::EFFECT_CLASS_MYSTERY_POTION));
		if( pEffect != NULL )
		{
			computeOptionStat( 185 );		// ��� + 3
			HP_t bonusHP = 50;
			Defense_t bonusDef = 10; 
			m_Defense[ATTR_MAX]  = max( 0, m_Defense[ATTR_MAX] + bonusDef );
			m_Defense[ATTR_CURRENT]  = max( 0, m_Defense[ATTR_CURRENT] + bonusDef );
			m_HP[ATTR_MAX]		 = max( (HP_t)0,(HP_t)( m_HP[ATTR_MAX] + bonusHP) );
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ�� ���� �����ϴ� ����Ʈ�� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	
	if (isFlag(Effect::EFFECT_CLASS_DOOM))
	{
		EffectDoom* pDoom = dynamic_cast<EffectDoom*>(findEffect(Effect::EFFECT_CLASS_DOOM));
		if (pDoom!= NULL)
		{
			int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT],    pDoom->getDefensePenalty());
			int ProtectionPenalty = getPercentValue(m_Protection[ATTR_CURRENT], pDoom->getProtectionPenalty());

			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT]    - DefensePenalty);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_SEDUCTION))
	{
		EffectSeduction* pSeduction = dynamic_cast<EffectSeduction*>(findEffect(Effect::EFFECT_CLASS_SEDUCTION));
		if (pSeduction!= NULL)
		{
			int ToHitPenalty   = getPercentValue(m_ToHit[ATTR_CURRENT],  pSeduction->getToHitPenalty());
			int DamagePenalty1 = getPercentValue(m_Damage[ATTR_CURRENT], pSeduction->getDamagePenalty());
			int DamagePenalty2 = getPercentValue(m_Damage[ATTR_MAX],     pSeduction->getDamagePenalty());

			m_ToHit[ATTR_CURRENT]  = max(0, m_ToHit[ATTR_CURRENT]  - ToHitPenalty);
			m_Damage[ATTR_CURRENT] = max(0, m_Damage[ATTR_CURRENT] - DamagePenalty1);
			m_Damage[ATTR_MAX]     = max(0, m_Damage[ATTR_MAX]     - DamagePenalty2);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2))//vampire
	{
		EffectOUTRAGEBOOSTER2* pOUTRAGEBOOSTER = dynamic_cast<EffectOUTRAGEBOOSTER2*>(findEffect(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2));
		if (pOUTRAGEBOOSTER!= NULL)
		{
			int STRPenalty   = getPercentValue(m_STR[ATTR_CURRENT],  pOUTRAGEBOOSTER->getSTRPenalty());
                        int DEXPenalty   = getPercentValue(m_DEX[ATTR_CURRENT],  pOUTRAGEBOOSTER->getDEXPenalty());
                        int INTPenalty   = getPercentValue(m_INT[ATTR_CURRENT],  pOUTRAGEBOOSTER->getINTPenalty());
			m_STR[ATTR_CURRENT]     = max(0, m_STR[ATTR_CURRENT] - STRPenalty);
			m_DEX[ATTR_CURRENT]     = max(0, m_DEX[ATTR_CURRENT] - DEXPenalty);
			m_INT[ATTR_CURRENT]     = max(0, m_INT[ATTR_CURRENT] - INTPenalty);
		}
	}
/*	if (isFlag(Effect::EFFECT_CLASS_PARALYZE))
	{
		EffectParalyze* pParalyze = dynamic_cast<EffectParalyze*>(findEffect(Effect::EFFECT_CLASS_PARALYZE));
		if (pParalyze != NULL)
		{
			int DefensePenalty = getPercentValue(m_Defense[ATTR_CURRENT], pParalyze->getDefensePenalty());
			m_Defense[ATTR_CURRENT] = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
		}
	}*/
	if (isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF))
	{
		EffectTransformToWolf* pTransformToWolf= dynamic_cast<EffectTransformToWolf*>(findEffect(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF));
		if (pTransformToWolf != NULL)
		{
			int PenaltyRatio	  = (isFlag(Effect::EFFECT_CLASS_HOWL))?10:30;
			int ToHitBonus        = getPercentValue(m_ToHit[ATTR_CURRENT], 20);
			int MinDamageBonus    = m_DEX[ATTR_CURRENT]/8 + m_STR[ATTR_CURRENT]/30;//getPercentValue(m_Damage[ATTR_CURRENT], 20);
			int MaxDamageBonus    = m_DEX[ATTR_CURRENT]/8 + m_STR[ATTR_CURRENT]/30;//getPercentValue(m_Damage[ATTR_MAX], 20);
			int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT], PenaltyRatio);//50);
			int ProtectionPenalty = getPercentValue(m_Protection[ATTR_CURRENT], PenaltyRatio);//50);

			m_ToHit[ATTR_CURRENT]      = min(VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
			m_Damage[ATTR_CURRENT]     = min(VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + MinDamageBonus);
			m_Damage[ATTR_MAX]         = min(VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_MAX] + MaxDamageBonus);
			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
		}
	}
	else if ( isFlag( Effect::EFFECT_CLASS_HOWL ) )
	{
		Effect* pEffect = findEffect( Effect::EFFECT_CLASS_HOWL );
		if ( pEffect != NULL ) pEffect->setDeadline(0);
	}

	// 20071003
	if ( isFlag( Effect:: EFFECT_CLASS_BLOODY_SHOUT) )
	{
		EffectBloodyShout* pEffect = dynamic_cast<EffectBloodyShout*>(findEffect(Effect::EFFECT_CLASS_BLOODY_SHOUT));
		if ( pEffect != NULL )
		{
			HP_t hpBonus = pEffect->getHPBonus();

			m_HP[ATTR_MAX] += min( (HP_t)VAMPIRE_MAX_HP, (HP_t)getPercentValue(m_HP[ATTR_MAX], hpBonus ) );
		}
	}
	if ( isFlag( Effect:: EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD) )
	{
		EffectADV2TRUECLOLOROFVAMPLOAD* pEffect = dynamic_cast<EffectADV2TRUECLOLOROFVAMPLOAD*>(findEffect(Effect::EFFECT_CLASS_ADV2TRUECLOLOROFVAMPLOAD));
		if ( pEffect != NULL )
		{
			//HP_t hpBonus = pEffect->getHPBonus();
                       HP_t hpBonus = 2225;
                       int     bonus= pEffect->getSTRBonus();
                       // int     DEXBonus= pEffect->getDEXBonus();
                       // int     INTBonus= pEffect->getINTBonus();
			
                        m_STR[ATTR_CURRENT] += bonus;
                        m_DEX[ATTR_CURRENT] += bonus;
                        m_INT[ATTR_CURRENT] += bonus;
                        m_STR[ATTR_MAX] += bonus;
                        m_DEX[ATTR_MAX] += bonus;
                        m_INT[ATTR_MAX] += bonus; 
			m_ToHit[ATTR_CURRENT]      += 35;
			m_Damage[ATTR_CURRENT]     += 5;
			m_Damage[ATTR_MAX]         += 8;
			m_Defense[ATTR_CURRENT]	   += 15;
                        m_Protection[ATTR_CURRENT] += 30;
			m_HP[ATTR_MAX]             += 225;
                        
		     //   computeStatOffset();
                       m_HP[ATTR_MAX] 			   = min((HP_t)VAMPIRE_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + hpBonus));
		  //     m_HP[ATTR_CURRENT] += hpBonus ;
                      
		}
	}
	if ( isFlag( Effect:: EFFECT_CLASS_WOLFROAR) )
	{
		EffectWOLFROAR* pEffect = dynamic_cast<EffectWOLFROAR*>(findEffect(Effect::EFFECT_CLASS_WOLFROAR));
		if ( pEffect != NULL )
		{
			int StrBouns = pEffect->getSTRBonus();
			int DexBouns = pEffect->getDEXBonus();
			int IntBouns = pEffect->getINTBonus();
			m_STR[ATTR_CURRENT] += StrBouns;
			m_DEX[ATTR_CURRENT] += DexBouns;
			m_INT[ATTR_CURRENT] += IntBouns;
			m_STR[ATTR_MAX] += StrBouns;
			m_DEX[ATTR_MAX] += DexBouns;
			m_INT[ATTR_MAX] += IntBouns;
			m_ToHit[ATTR_CURRENT]      += (DexBouns+StrBouns/6);
			m_Damage[ATTR_CURRENT]     += (StrBouns/6);
			m_Damage[ATTR_MAX]         += (StrBouns/6);
			m_Defense[ATTR_CURRENT]	   += (DexBouns/2);
                        m_Protection[ATTR_CURRENT] += StrBouns;
			m_HP[ATTR_MAX]             += (StrBouns*2+DexBouns+IntBouns);
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF))
	{
		EffectTransformToWerwolf* pTransformToWerwolf= dynamic_cast<EffectTransformToWerwolf*>(findEffect(Effect::EFFECT_CLASS_TRANSFORM_TO_WERWOLF));
		if (pTransformToWerwolf != NULL)
		{
			// 2007 06 22
			int ToHitBonus        = getPercentValue(m_ToHit[ATTR_CURRENT], 20);
	//		int ToHitBonus        = 100;
			int HPBonus			  = getPercentValue(m_HP[ATTR_MAX], 50);
	//		m_HP[ATTR_CURRENT]		-= getPercentValue(m_HP[ATTR_CURRENT] , ratio );
//			int MinDamageBonus    = m_DEX[ATTR_CURRENT]/6 + m_STR[ATTR_CURRENT]/40;//getPercentValue(m_Damage[ATTR_CURRENT], 20);
//			int MaxDamageBonus    = m_DEX[ATTR_CURRENT]/6 + m_STR[ATTR_CURRENT]/40;//getPercentValue(m_Damage[ATTR_MAX], 20);
			int MinDamageBonus    = m_DEX[ATTR_CURRENT]/3 + m_STR[ATTR_CURRENT]/3;//getPercentValue(m_Damage[ATTR_CURRENT], 20);
			int MaxDamageBonus    = m_DEX[ATTR_CURRENT]/3 + m_STR[ATTR_CURRENT]/3;//getPercentValue(m_Damage[ATTR_MAX], 20);
			int DefenseBonus	  = getPercentValue(m_Defense[ATTR_CURRENT], 30);
//			int ResistBonus		  = 9;
			int ResistBonus		  = 9;

			m_ToHit[ATTR_CURRENT]      = min(VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
			m_Damage[ATTR_CURRENT]     = min(VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + MinDamageBonus);
			m_Damage[ATTR_MAX]         = min(VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_MAX] + MaxDamageBonus);
			m_Defense[ATTR_CURRENT]	   = min(VAMPIRE_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + DefenseBonus);
			m_HP[ATTR_MAX] 			   = min((HP_t)VAMPIRE_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + HPBonus));
			// 20070904
			if( pTransformToWerwolf->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] 		   = min((HP_t)VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + HPBonus));
				pTransformToWerwolf->setHPBonus(false);
			}

			m_Resist[MAGIC_DOMAIN_POISON] += ResistBonus;
			m_Resist[MAGIC_DOMAIN_ACID] += ResistBonus;
			m_Resist[MAGIC_DOMAIN_CURSE] += ResistBonus;
			m_Resist[MAGIC_DOMAIN_BLOOD] += ResistBonus;
			m_Resist[MAGIC_DOMAIN_NO_DOMAIN] += ResistBonus;
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT))
	{
		EffectTransformToBat* pTransformToBat = dynamic_cast<EffectTransformToBat*>(findEffect(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT));
		if (pTransformToBat != NULL)
		{
			int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT], 25);
			int ProtectionPenalty = getPercentValue(m_Protection[ATTR_CURRENT], 25);

			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT]    - DefensePenalty);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
		}
	}
	if(isFlag(Effect::EFFECT_CLASS_EXTREME))
	{
		EffectExtreme* pExtreme = dynamic_cast<EffectExtreme*>(findEffect(Effect::EFFECT_CLASS_EXTREME));
		if (pExtreme!= NULL)
		{
//			int DamageBonus = 4 + ((m_STR[ATTR_CURRENT]-20)/50);
			int DamageBonus = min(15,4 + ((m_STR[ATTR_CURRENT]-20)/30));
			int ToHitBonus = min(20,4 + ((m_STR[ATTR_CURRENT] + m_DEX[ATTR_CURRENT]) / 40));

			m_Damage[ATTR_CURRENT] = max(0, m_Damage[ATTR_CURRENT] + DamageBonus);
			m_Damage[ATTR_MAX]     = max(0, m_Damage[ATTR_MAX]     + DamageBonus);
			m_ToHit[ATTR_CURRENT]  = min(VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_DEATH))
	{
		EffectDeath* pDeath = dynamic_cast<EffectDeath*>(findEffect(Effect::EFFECT_CLASS_DEATH));
		if (pDeath != NULL)
		{
			for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
			{
				m_Resist[i] -= pDeath->getResistPenalty();
//				if ( m_Resist[i] < 0 ) m_Resist[i] = 0;
			}
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_MEPHISTO))
	{
		EffectMephisto* pMephisto = dynamic_cast<EffectMephisto*>(findEffect(Effect::EFFECT_CLASS_MEPHISTO));
		if (pMephisto != NULL)
		{
			//int bonusPercent = 100 + pMephisto->getBonus();
			//m_ToHit[ATTR_CURRENT]      = min(VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] * bonusPercent / 100);
			//m_Defense[ATTR_CURRENT]    = min(VAMPIRE_MAX_DEFENSE, m_Defense[ATTR_CURRENT] * bonusPercent / 100);
			//m_Protection[ATTR_CURRENT] = min(VAMPIRE_MAX_PROTECTION, m_Protection[ATTR_CURRENT] * bonusPercent / 100);
			m_ToHit[ATTR_CURRENT]      = min(VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + pMephisto->getToHitBonus()+200 );
			m_Defense[ATTR_CURRENT]    = min(VAMPIRE_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + pMephisto->getDefenseBonus() );
			m_Protection[ATTR_CURRENT] = min(VAMPIRE_MAX_PROTECTION, m_Protection[ATTR_CURRENT] + pMephisto->getProtectionBonus() );
		}
	}
	//if (isFlag(Effect::EFFECT_CLASS_RAGE_OF_BLOOD))
	//{
	//	EffectRAGEOFBLOOD* pRAGEOFBLOOD = dynamic_cast<EffectRAGEOFBLOOD*>(findEffect(Effect::EFFECT_CLASS_RAGE_OF_BLOOD));
	//	if (pRAGEOFBLOOD != NULL)
	//	{
			//int bonusPercent = 100 + pMephisto->getBonus();
			//m_ToHit[ATTR_CURRENT]      = min(VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] * bonusPercent / 100);
			//m_Defense[ATTR_CURRENT]    = min(VAMPIRE_MAX_DEFENSE, m_Defense[ATTR_CURRENT] * bonusPercent / 100);
			//m_Protection[ATTR_CURRENT] = min(VAMPIRE_MAX_PROTECTION, m_Protection[ATTR_CURRENT] * bonusPercent / 100);
		//	m_ToHit[ATTR_CURRENT]      = min(VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + pRAGEOFBLOOD->getToHitBonus() );
	//		m_Defense[ATTR_CURRENT]    = min(VAMPIRE_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + pRAGEOFBLOOD->getDefenseBonus() );
		//	m_Protection[ATTR_CURRENT] = min(VAMPIRE_MAX_PROTECTION, m_Protection[ATTR_CURRENT] + pRAGEOFBLOOD->getProtectionBonus() );
		   //     int HPRegenBonus = pRAGEOFBLOOD->getHPBonus();

		   //     m_HPRegenBonus += HPRegenBonus;
	//	}
	//}
	// by sigi. 2002.6.19
	// isEffect�� isFlag�� �ٲ�. 2003.3.27 by Sequoia
	if (isFlag(Effect::EFFECT_CLASS_CASKET))
	{
		EffectSummonCasket* pCasket = dynamic_cast<EffectSummonCasket*>(findEffect(Effect::EFFECT_CLASS_CASKET));
		if (pCasket!= NULL)
		{
			// pCasket->getType()�� ��� �ٸ� ���� ����..
			// by sigi. 2002.12.3. 20 --> 30
			int DefenseBonus    = getPercentValue(m_Defense[ATTR_CURRENT],    30);
			int ProtectionBonus = getPercentValue(m_Protection[ATTR_CURRENT], 30);

			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT]    + DefenseBonus);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] + ProtectionBonus);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_BLUNTING))
	{
		EffectBlunting* pBlunting = dynamic_cast<EffectBlunting*>(findEffect(Effect::EFFECT_CLASS_BLUNTING));
		if ( pBlunting != NULL )
		{
			int DefensePenalty = pBlunting->getDefensePenalty();
			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_PARTY_AURA ) )
	{
		EffectPartyAura* pPartyAura = dynamic_cast<EffectPartyAura*>(findEffect(Effect::EFFECT_CLASS_PARTY_AURA));

		if ( pPartyAura != NULL )
		{
			Level_t level = getLevel();
			size_t partySize = pPartyAura->getPartySize();

			if ( pPartyAura->isHP() )
			{
				HP_t amount = (HP_t)( ( 65 + ( level / 3 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_HP[ATTR_MAX] = min((HP_t) VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + amount ));
			}
			if ( pPartyAura->isDefense() )
			{
				Defense_t amount = (Defense_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Defense[ATTR_CURRENT] = min( VAMPIRE_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + amount );
			}
			if ( pPartyAura->isToHit() )
			{
				ToHit_t amount = (ToHit_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_ToHit[ATTR_CURRENT] = min( VAMPIRE_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + amount );
			}
			if ( pPartyAura->isLuck() )
			{
				Luck_t amount = (Luck_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Luck += amount;
			}
			if ( pPartyAura->isDamage() )
			{
				Damage_t amount = (Damage_t)( ( 15 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Damage[ATTR_CURRENT] = min( VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + amount );
				m_Damage[ATTR_MAX] = min( VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_MAX] + amount );
			}
			if ( pPartyAura->isResist() )
			{
				Resist_t amount = (Resist_t)( ( 15 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Resist[MAGIC_DOMAIN_POISON] += amount;
				m_Resist[MAGIC_DOMAIN_ACID] += amount;
				m_Resist[MAGIC_DOMAIN_CURSE] += amount;
				m_Resist[MAGIC_DOMAIN_BLOOD] += amount;
			}
		}
	}

	if ( isFlag(Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL) )
	{
		m_Defense[ATTR_CURRENT] += 30;
	}

	if ( isFlag(Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL) )
	{
		m_Protection[ATTR_CURRENT] += 30;
	}

	if ( isFlag(Effect::EFFECT_CLASS_FURIOUS_INCENSE) )
	{
		m_Damage[ATTR_CURRENT] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + 3 );
		m_Damage[ATTR_MAX] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + 3 );
	}

	if (isFlag(Effect::EFFECT_CLASS_BLOOD_CURSE))
	{
		EffectBloodCurse* pBloodCurse = dynamic_cast<EffectBloodCurse*>(findEffect(Effect::EFFECT_CLASS_BLOOD_CURSE));
		if( pBloodCurse != NULL )
		{
			int DefensePenalty = pBloodCurse->getDefensePenalty();
			int ProtectionPenalty = pBloodCurse->getProtectionPenalty();
			int HPPenalty = pBloodCurse->getHPPenalty();

			m_Defense[ATTR_CURRENT] = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
			m_HP[ATTR_MAX] = max((HP_t)1, (HP_t)(m_HP[ATTR_MAX] - HPPenalty));
			// 20080813 ���� �����ۿ� ���� ��Ƣ�� ���ʽ��� ���� ��� �ǰ� Ȯ ���̴� ���װ� ����. ������ �� �ڿ� ���ڵ尡 �����Ƿ� �̰� ����.
//			if( m_HP[ATTR_MAX] < m_HP[ATTR_CURRENT] )
//			{
//				m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
//			}
		}
	}

	/*
	 // 2006.05.29
	 // STR+15
	if ( isFlag(Effect::EFFECT_CLASS_GREYFELL_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_STR, 15);
	}
	// DEX+15
	if ( isFlag(Effect::EFFECT_CLASS_SOVIUS_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_DEX, 15);
	}
	// INT+15
	if ( isFlag(Effect::EFFECT_CLASS_MIR_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_INT, 15);
	}
	*/

/*	// 2007 01 16 �̽��׸� ���� ����Ʈ ��� �߰�
	if (isFlag(Effect::EFFECT_CLASS_MYSTERY_POTION))
	{
		EffectMysteryPotion* pEffect = dynamic_cast<EffectMysteryPotion*>(findEffect(Effect::EFFECT_CLASS_MYSTERY_POTION));
		if( pEffect != NULL )
		{
			HP_t bonusHP = 50;
			Defense_t bonusDef = 10; 
			m_Defense[ATTR_CURRENT]  = max( 0, m_Defense[ATTR_CURRENT] + bonusDef );
			m_HP[ATTR_CURRENT]		 = max( 0, m_HP[ATTR_CURRENT] + bonusHP );
			computeOptionStat( 185 );		// ��� + 3
		}
	}*/

	///////////////////////////////////////////////////////////////////////////////
	// ��� ���ʽ��� ����Ѵ�.
	///////////////////////////////////////////////////////////////////////////////
	/*if ( hasRankBonus( RankBonus::RANK_BONUS_IMMORTAL_BLOOD ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_IMMORTAL_BLOOD );
		Assert( pRankBonus != NULL );

		int HPBonus = pRankBonus->getPoint();

		m_HP[ATTR_MAX] = min(VAMPIRE_MAX_HP, m_HP[ATTR_MAX] + HPBonus );
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_BEHEMOTH_SKIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_BEHEMOTH_SKIN );
		Assert( pRankBonus != NULL );

		int DefenseBonus = pRankBonus->getPoint();

		m_Defense[ATTR_CURRENT] = min(VAMPIRE_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + DefenseBonus ); 
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_SAFE_ROBE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SAFE_ROBE );
		Assert( pRankBonus != NULL );

		int ProtectionBonus = pRankBonus->getPoint();

		m_Protection[ATTR_CURRENT] = min(VAMPIRE_MAX_PROTECTION, m_Protection[ATTR_CURRENT] + ProtectionBonus );
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_CROW_WING ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_CROW_WING );
		Assert( pRankBonus != NULL );

		int AttackSpeedBonus = pRankBonus->getPoint();

		increaseAttackSpeed(AttackSpeedBonus, ATTR_CURRENT);
		increaseAttackSpeed(AttackSpeedBonus, ATTR_MAX);
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_URANUS_BLESS ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_URANUS_BLESS );
		Assert( pRankBonus != NULL );

		int HPRegenBonus = pRankBonus->getPoint();

		m_HPRegenBonus += HPRegenBonus;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_ACID_INQUIRY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_ACID_INQUIRY );
		Assert( pRankBonus != NULL );

		m_Resist[MAGIC_DOMAIN_ACID] += getPercentValue(m_Resist[MAGIC_DOMAIN_ACID], pRankBonus->getPoint());;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_BLOODY_INQUIRY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_BLOODY_INQUIRY );
		Assert( pRankBonus != NULL );

		m_Resist[MAGIC_DOMAIN_BLOOD] += getPercentValue(m_Resist[MAGIC_DOMAIN_BLOOD], pRankBonus->getPoint());;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_CURSE_INQUIRY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_CURSE_INQUIRY );
		Assert( pRankBonus != NULL );

		m_Resist[MAGIC_DOMAIN_CURSE] += getPercentValue(m_Resist[MAGIC_DOMAIN_CURSE], pRankBonus->getPoint());;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_POISON_INQUIRY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_POISON_INQUIRY );
		Assert( pRankBonus != NULL );

		m_Resist[MAGIC_DOMAIN_POISON] += getPercentValue(m_Resist[MAGIC_DOMAIN_POISON], pRankBonus->getPoint());;
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_INQUIRY_MASTERY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_INQUIRY_MASTERY );
		Assert( pRankBonus != NULL );

		for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
		{
			m_Resist[i] += getPercentValue(m_Resist[i], pRankBonus->getPoint());;
		}
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_ACID_MASTERY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_ACID_MASTERY );
		Assert( pRankBonus != NULL );

		m_Mastery[MAGIC_DOMAIN_ACID] += pRankBonus->getPoint();
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_BLOODY_MASTERY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_BLOODY_MASTERY );
		Assert( pRankBonus != NULL );

		m_Mastery[MAGIC_DOMAIN_BLOOD] += pRankBonus->getPoint();
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_CURSE_MASTERY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_CURSE_MASTERY );
		Assert( pRankBonus != NULL );

		m_Mastery[MAGIC_DOMAIN_CURSE] += pRankBonus->getPoint();
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_POISON_MASTERY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_POISON_MASTERY );
		Assert( pRankBonus != NULL );

		m_Mastery[MAGIC_DOMAIN_POISON] += pRankBonus->getPoint();
	}
	if ( hasRankBonus( RankBonus::RANK_BONUS_SKILL_MASTERY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SKILL_MASTERY );
		Assert( pRankBonus != NULL );

		for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
		{
			m_Mastery[i] += getPercentValue(m_Mastery[i], pRankBonus->getPoint());;
		}
	}*/

	// DEX �� �� HPRegenBonus ����Ʈ
	if ( m_DEX[ATTR_BASIC] > 450 )
	{
		m_HPRegenBonus += 7;
	}
	else if ( m_DEX[ATTR_BASIC] > 390 )
	{
		m_HPRegenBonus += 6;
	}
	else if ( m_DEX[ATTR_BASIC] > 330 )
	{
		m_HPRegenBonus += 5;
	}
	else if ( m_DEX[ATTR_BASIC] > 260 )
	{
		m_HPRegenBonus += 4;
	}
	else if ( m_DEX[ATTR_BASIC] > 190 )
	{
		m_HPRegenBonus += 3;
	}
	else if ( m_DEX[ATTR_BASIC] > 120 )
	{
		m_HPRegenBonus += 2;
	}
	else if ( m_DEX[ATTR_BASIC] > 50 )
	{
		m_HPRegenBonus += 1;
	}

	// ��Ƽ�� ũ�⿡ ��� �ɷ�ġ�� ���� �� �ִ�.

	// ���� ���ʽ� ���
	if ( HPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min((HP_t) VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + HPBonus) );
	}

	if ( RaceWarHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)VAMPIRE_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + RaceWarHPBonus ));
	}

	if ( DragonEyeHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + DragonEyeHPBonus ));
		EffectDragonEye* pEffectDragonEye= dynamic_cast<EffectDragonEye*>(findEffect(Effect::EFFECT_CLASS_DRAGON_EYE));
		if (pEffectDragonEye != NULL)
		{
			if( pEffectDragonEye->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + DragonEyeHPBonus ));
				pEffectDragonEye->setHPBonus(false);
			}
		}

	}

#ifdef __CONTRIBUTION_SYSTEM__
	if ( HeroItemHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min((HP_t) VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + HeroItemHPBonus ));
		EffectVampireHeroItem* pEffectHeroItem = dynamic_cast<EffectVampireHeroItem*>(findEffect(Effect::EFFECT_CLASS_VAMPIRE_HERO_ITEM));
		if (pEffectHeroItem != NULL)
		{
			if( pEffectHeroItem->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + HeroItemHPBonus) );
				pEffectHeroItem->setHPBonus(false);
			}
		}

	}
	if ( LevelHeroItemHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + LevelHeroItemHPBonus ));
		EffectVampireLevelWarHeroItem* pEffectHeroItem = dynamic_cast<EffectVampireLevelWarHeroItem*>(findEffect(Effect::EFFECT_CLASS_VAMPIRE_LEVELWAR_HERO_ITEM));
		if (pEffectHeroItem != NULL)
		{
			if( pEffectHeroItem->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)VAMPIRE_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + LevelHeroItemHPBonus ));
				pEffectHeroItem->setHPBonus(false);
			}
		}
	}
#endif

		// 20080416 SKILL_INCREASE_OF_HEALTH — MaxHP bonus (Phoenix 2026-09-07: yorum kaldirildi)
		if( getSkill(SKILL_INCREASE_OF_HEALTH_15) )
		{
			m_HP[ATTR_MAX] += 5400;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_14) )
		{
			m_HP[ATTR_MAX] += 4800;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_13) )
		{
			m_HP[ATTR_MAX] += 4200;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_12) )
		{
			m_HP[ATTR_MAX] += 3600;
		} 
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_11) )
		{
			m_HP[ATTR_MAX] += 3000;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_10) )
		{
			m_HP[ATTR_MAX] += 2400;
		}
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_9) )
		{
			m_HP[ATTR_MAX] += 1800;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_8) )
		{
			m_HP[ATTR_MAX] += 1600;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_7) )
		{
			m_HP[ATTR_MAX] += 1400;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_6) )
		{
			m_HP[ATTR_MAX] += 1200;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_5) )
		{
			m_HP[ATTR_MAX] += 1000;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_4) )
		{
			m_HP[ATTR_MAX] += 800;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_3) )
		{
			m_HP[ATTR_MAX] += 600;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_2) )
		{
			m_HP[ATTR_MAX] += 400;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_1) )
		{
			m_HP[ATTR_MAX] += 200;
		}
             if( getSkill(SKILL_hpup_1500) )
		{
			 m_HP[ATTR_MAX] += 7000;
		} 
             if( getSkill(SKILL_ADV2_UNKNOW2) )
		{
			 m_HP[ATTR_MAX] += 1000;
                         m_PhysicBonusDamage += 30;

		} 
             if( getSkill(SKILL_ADV2_UNKNOW3) )
		{
			 m_HP[ATTR_MAX] += 1000;
                         m_PhysicBonusDamage += 30;

		}
             if( getSkill(SKILL_ADV2_UNKNOW4) )
		{
		m_ToHit[ATTR_CURRENT] += 50;
                m_CriticalRatio[ATTR_MAX] += 50;
		}
             if( getSkill(SKILL_ADV2_UNKNOW5) )
		{
			m_MagicDamageReduce += 30;
                        m_MagicBonusDamage += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW6) )
		{
			    m_MagicBonusDamage += 30;
                            m_PhysicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW7) )
		{
			m_PhysicBonusDamage += 30;
                        m_ToHit[ATTR_CURRENT] += 100;
		}
             if( getSkill(SKILL_ADV2_UNKNOW8) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW9) )
		{
			m_PhysicBonusDamage += 30;
                       m_Defense[ATTR_CURRENT] += 200;
		}
             if( getSkill(SKILL_ADV2_UNKNOW10) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW11) )
		{
		            m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW12) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}

                if( getSkill(SKILL_RALLSER_44) )
		{
			m_Defense[ATTR_CURRENT] += 100;
		} 
                if( getSkill(SKILL_RALLSER_5) )
		{
			m_ToHit[ATTR_CURRENT] += 100;
		} 
                if( getSkill(SKILL_RALLSER_6) )
		{
			m_PhysicBonusDamage += 40;
		} 
                if( getSkill(SKILL_RALLSER_7) )
		{
			m_MagicBonusDamage += 40;
		} 
                if( getSkill(SKILL_RALLSER_8) )
		{
			m_HP[ATTR_MAX] += 800;
		} 
    // HP�� ����ġ�� HP�� �ְ�ġ�� �Ѵ� ���
    // ����ġ�� �ְ�ġ������ set
    if (m_HP[ATTR_CURRENT] > m_HP[ATTR_MAX])
    {
        m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
		/*
        if (m_pZone)
        {
            GCStatusCurrentHP gcStatusCurrentHP;
            gcStatusCurrentHP.setObjectID(m_ObjectID);
            gcStatusCurrentHP.setCurrentHP (m_HP[ATTR_CURRENT]);
            m_pZone->broadcastPacket(m_X, m_Y, &gcStatusCurrentHP);
        }
		*/
    }

	//////////////////////////////////////////////////////////////////////////////
	// �нú� ����� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	VampireSkillSlot* pNailMastery = getSkill( SKILL_NAIL_MASTERY );
	if ( pNailMastery != NULL )
	{
		int DamageBonus = 3 + ( (getLevel()-56) / 8 );

		m_Damage[ATTR_CURRENT] = max(0, m_Damage[ATTR_CURRENT] + DamageBonus);
		m_Damage[ATTR_MAX]     = max(0, m_Damage[ATTR_MAX]     + DamageBonus);
	}

	// Hoodlum Stigma
	if ( isFlag( Effect::EFFECT_CLASS_HOODLUM_STIGMA ) )
	{
		m_HP[ATTR_MAX] = getPercentValue( m_HP[ATTR_MAX], 70 );
	}

	initCastleSkill();

	if ( isFlag( Effect::EFFECT_CLASS_TRANSFORM_TO_BAT ) )
	{
		m_Resist[MAGIC_DOMAIN_NO_DOMAIN] = 0;
		m_Resist[MAGIC_DOMAIN_POISON]    = 50;
		m_Resist[MAGIC_DOMAIN_ACID]      = 0;
		m_Resist[MAGIC_DOMAIN_CURSE]     = 0;
		m_Resist[MAGIC_DOMAIN_BLOOD]     = 0;
	}

	for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
	{
		if ( m_Resist[i] < 0 ) m_Resist[i] = 0;
		//cout << i << " : " << m_Resist[i] << endl;
	}

	//20080813 ������ �������� �ִ� HP�� ���� ���� �� ���.
	if(m_SilverDamage >= (m_HP[ATTR_MAX]/2))
	{
		m_SilverDamage = m_HP[ATTR_MAX]/2;
	}
//	cout << getName() << "�� Luck : " << m_Luck << endl;

	/*
	// ��Ƽ �ο���� �Ѿ���� ���� ���� �ٽ� ����Ѵ�.
	if (numPartyMember == -1)
	{
		if (m_PartyID != 0)
		{
			LocalPartyManager* pLPM = getLocalPartyManager();
			Assert(pLPM != NULL);

			numPartyMember = pLPM->getAdjacentMemberSize(m_PartyID, this);
		}
	}

	if (numPartyMember > 1)
	{
		uint ToHitBonus      = 0;
		uint DefenseBonus    = 0;
		uint ProtectionBonus = 0;
		uint DamageBonus     = 0;

		switch (numPartyMember)
		{
			case 2: ToHitBonus +=  2; DefenseBonus += 1; ProtectionBonus += 1; DamageBonus += 1; break;
			case 3: ToHitBonus +=  4; DefenseBonus += 2; ProtectionBonus += 2; DamageBonus += 1; break;
			case 4: ToHitBonus +=  6; DefenseBonus += 3; ProtectionBonus += 3; DamageBonus += 2; break;
			case 5: ToHitBonus +=  8; DefenseBonus += 4; ProtectionBonus += 4; DamageBonus += 2; break;
			case 6: ToHitBonus += 10; DefenseBonus += 5; ProtectionBonus += 5; DamageBonus += 3; break;
			default: break; 
		}

		m_ToHit[ATTR_CURRENT]      += ToHitBonus;
		m_Defense[ATTR_CURRENT]    += DefenseBonus;
		m_Protection[ATTR_CURRENT] += ProtectionBonus;
		m_Damage[ATTR_CURRENT]     += DamageBonus;
		m_Damage[ATTR_MAX]         += DamageBonus;
	}
	*/

	ComputeAllSkillDelay();
	if (isFlag(Effect::EFFECT_CLASS_INTIMATE_GRAIL))
	{
		EffectIntimateGrail* pIntimateGrail = dynamic_cast<EffectIntimateGrail*>(findEffect(Effect::EFFECT_CLASS_INTIMATE_GRAIL));
		if ( pIntimateGrail!= NULL )
		{
			int ratio =  10 + ( pIntimateGrail->getSkillLevel()/10);
			m_Defense[ATTR_CURRENT]	-= getPercentValue(m_Defense[ATTR_CURRENT] , ratio );
		//	m_HP[ATTR_CURRENT]		-= getPercentValue(m_HP[ATTR_CURRENT] , ratio );
			m_HP[ATTR_MAX]		-= getPercentValue(m_HP[ATTR_MAX] , ratio );
			m_HP[ATTR_CURRENT] = min( m_HP[ATTR_CURRENT] , m_HP[ATTR_MAX]);
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_INTIMATE_GRAIL_2))
	{
		EffectIntimateGrail2* pIntimateGrail2 = dynamic_cast<EffectIntimateGrail2*>(findEffect(Effect::EFFECT_CLASS_INTIMATE_GRAIL_2));
		if ( pIntimateGrail2!= NULL )
		{
			int ratio =  20;
			m_Defense[ATTR_CURRENT]	-= getPercentValue(m_Defense[ATTR_CURRENT] , ratio );
			//m_HP[ATTR_CURRENT]		-= getPercentValue(m_HP[ATTR_CURRENT] , ratio );
			m_HP[ATTR_MAX]		-= getPercentValue(m_HP[ATTR_MAX] , ratio );
			m_HP[ATTR_CURRENT] = min( m_HP[ATTR_CURRENT] , m_HP[ATTR_MAX]);
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_FAVOROFLUNA))//vamprie
	{
		EffectFAVOROFLUNA* pFAVOROFLUNA = dynamic_cast<EffectFAVOROFLUNA*>(findEffect(Effect::EFFECT_CLASS_FAVOROFLUNA));
		if ( pFAVOROFLUNA!= NULL )
		{
			int ratio =  20;
			m_Defense[ATTR_CURRENT]	-= getPercentValue(m_Defense[ATTR_CURRENT] , ratio );
			//m_HP[ATTR_CURRENT]		-= getPercentValue(m_HP[ATTR_CURRENT] , ratio );
			m_HP[ATTR_MAX]		-= getPercentValue(m_HP[ATTR_MAX] , ratio );
			m_HP[ATTR_CURRENT] = min( m_HP[ATTR_CURRENT] , m_HP[ATTR_MAX]);
		}
	}
	if ( isFlag(Effect::Effect::EFFECT_CLASS_ADV2INFINITYBLAZE) )
	{
		for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
		{
			m_Resist[i] -= getPercentValue(m_Resist[i], 20);;
		}
	}	
	__END_CATCH
}

int	Vampire::getBloodBibleSignOpenNum() const
{
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);

	int openNumLimit = g_pVariableManager->getVariable(PREMIUM_BLOOD_BIBLE_NUMBER);
//	if (!pGamePlayer->isPayPlaying() 
//		&& !pGamePlayer->isPremiumPlay())
	if (!pGamePlayer->isPremiumPlay()) // 20080227 ������ Ƽ�� �߰��� ����
	{
		openNumLimit = g_pVariableManager->getVariable(FREE_BLOOD_BIBLE_NUMBER);
	}

#ifdef __TEST_SERVER__
	Fame_t fame = getFame()*10;
#else
	Fame_t fame = getFame();
#endif

	uint openNum = 1;

	if ( g_pWarSystem->canApplyBloodBibleSign() )
	{
		if ( fame < 100000 )
		{
			openNum = min( openNumLimit, 1 );
		}
		else if ( fame < 1000000 )
		{
			openNum = min( openNumLimit, 2 );
		}
		else if ( fame < 5000000 )
		{
			openNum = min( openNumLimit, 3 );
		}
		else if ( fame < 10000000 )
		{
			openNum = min( openNumLimit, 4 );
		}
		else if ( fame < 200000000 )
		{
			openNum = min( openNumLimit, 5 );
		}
		else
		{
			openNum = min( openNumLimit, 6 );
		}
	}
	else
	{
		openNum = 0;
	}

	return openNum;
}

//////////////////////////////////////////////////////////////////////////////
// STR, DEX, INT�� ���
// CURRENT = �⺻ ��ġ + ������ ��ġ + ���� ��ġ
// MAX     = �⺻ ��ġ + ������ ��ġ
// BASIC   = �⺻ ��ġ
//
// HP, MP�� ���
// CURRENT = ���� ��ġ
// MAX     = ���� �ƽ�
// BASIC   = �����ۿ� ���� ��ȭ ��ġ
//
// Defense, Protection, ToHit�� ���
// CURRENT = ���� ��ġ
// MAX     = �����ۿ� ���� ��ȭ ��ġ
//
// Damage�� ���
// CURRENT = Min ������
// MAX     = Max ������
// BASIC   = �����ۿ� ���� ��ȭ ��ġ
//////////////////////////////////////////////////////////////////////////////
void Vampire::computeStatOffset()
	throw()
{
	__BEGIN_TRY

	Creature::CreatureClass CClass = getCreatureClass();
	BASIC_ATTR              cur_attr;

	cur_attr.nSTR   = m_STR[ATTR_CURRENT];
	cur_attr.nDEX   = m_DEX[ATTR_CURRENT];
	cur_attr.nINT   = m_INT[ATTR_CURRENT];
	cur_attr.nLevel = m_Level;

	// ���ο��� STR, DEX, INT�� ���� ����� �� ����
	// ������ �Ǵ� ���� ��ġ�� ���Ѵ�. 
	//m_HP[ATTR_MAX] = computeHP(CClass, &cur_attr);
	m_HP[ATTR_MAX] = computeTestModeHP(this, &cur_attr);
	m_HP[ATTR_MAX] += m_HP[ATTR_BASIC];

	m_ToHit[ATTR_CURRENT] = computeToHit(CClass, &cur_attr);
	m_ToHit[ATTR_CURRENT] += m_ToHit[ATTR_MAX];

	m_Defense[ATTR_CURRENT] = computeDefense(CClass, &cur_attr);
	m_Defense[ATTR_CURRENT] += m_Defense[ATTR_MAX];

	m_Protection[ATTR_CURRENT] = computeProtection(CClass, &cur_attr);
	m_Protection[ATTR_CURRENT] += m_Protection[ATTR_MAX];

	m_Damage[ATTR_CURRENT] = computeMinDamage(CClass, &cur_attr);
	m_Damage[ATTR_MAX]     = computeMaxDamage(CClass, &cur_attr);

	m_Damage[ATTR_CURRENT] += m_Damage[ATTR_BASIC];
	m_Damage[ATTR_MAX] += m_Damage[ATTR_BASIC];

	setAttackSpeed(computeAttackSpeed(CClass, &cur_attr), ATTR_CURRENT);
	increaseAttackSpeed(getAttackSpeed(ATTR_MAX), ATTR_CURRENT);

	m_CriticalRatio[ATTR_CURRENT] = computeCriticalRatio(CClass, &cur_attr);
	m_CriticalRatio[ATTR_CURRENT] += m_CriticalRatio[ATTR_MAX];

	// 20070918
	m_MagicDefense = computeMagicDefense(CClass, &cur_attr);
	m_MagicToHit   = computeMagicToHit(CClass, &cur_attr);
		
	__END_CATCH
}

void Vampire::computeItemStat(Item* pItem)
	throw()
{
	__BEGIN_TRY

	if (isVampireWeapon(pItem->getItemClass()))
	{
		// ������ ���Ⱑ ������ �ӵ� �Ķ���͸� ���Ѵ�.
		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());
		increaseAttackSpeed(pItemInfo->getSpeed(), ATTR_CURRENT);
		increaseAttackSpeed(pItemInfo->getSpeed(), ATTR_MAX);
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP )
	{
		CoreZapInfo* pItemInfo = dynamic_cast<CoreZapInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType()));
		if ( pItemInfo != NULL )
		{
			computeOptionClassStat( pItemInfo->getOptionClass(), pItem->getGrade() );
		}
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_CUE_OF_ADAM )
	{
		CueOfAdamInfo* pItemInfo = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType()));

		if ( pItemInfo != NULL )
		{
			computeOptionClassStat( pItemInfo->getOptionClass() , pItem->getGrade() );
		}
	}

	m_Protection[ATTR_CURRENT] += pItem->getProtectionBonus();
	m_Protection[ATTR_MAX]     += pItem->getProtectionBonus();

	m_Defense[ATTR_CURRENT] += pItem->getDefenseBonus();
	m_Defense[ATTR_MAX]     += pItem->getDefenseBonus();

	m_ToHit[ATTR_CURRENT] += pItem->getToHitBonus();
	m_ToHit[ATTR_MAX] += pItem->getToHitBonus();

	m_Luck	+= pItem->getLuck();

	m_MagicDamageReduce		+= pItem->getMagicProtectionBonus(); // 20080703 

//	if (pItem->getOptionType()) computeOptionStat(pItem);
	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& optionType = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;
	for (itr=optionType.begin(); itr!=optionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}
#ifdef VERSION_THIRD_ENCHANT_1
	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& thirdoptionType = pItem->getThirdOptionTypeList();
	for (itr=thirdoptionType.begin(); itr!=thirdoptionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}
#endif

	// Item ��ü�� defaultOption�� ����Ų��.
	const list<OptionType_t>& defaultOptions = pItem->getDefaultOptions();
	list<OptionType_t>::const_iterator iOptions;

	for (iOptions=defaultOptions.begin(); iOptions!=defaultOptions.end(); iOptions++)
	{
		computeOptionStat( *iOptions );
	}
	
	// �ھ����� ��� �� ���� ������ �ɼ� ���ʽ��� �ִ�.
	if ( pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP )
	{
		bool zaps[4] = { false, false, false, false };
		for ( int i=WEAR_ZAP1; i<=WEAR_ZAP4; ++i )
		{
			Item* pCoreZap = m_pWearItem[i];
			if ( pCoreZap != NULL && m_pRealWearingCheck[i] == true && pCoreZap->getItemClass() == Item::ITEM_CLASS_CORE_ZAP && pCoreZap->getItemType()<4 )
			{
				zaps[pCoreZap->getItemType()] = true;
			}
		}
		if ( zaps[0] && zaps[1] && zaps[2] && zaps[3] )
		{
			computeOptionStat(182); //���� 9
			computeOptionStat(185); //��� 3
		}
	}

	// ť���� �ƴ��� ��� �� ���� �ְ� ���� �ٸ� ������ �ɼ� ���ʽ��� �ִ�.
	if ( pItem->getItemClass() == Item::ITEM_CLASS_CUE_OF_ADAM )
	{
		ItemType_t cues[2] = { 9999, 9999 };
		for ( int i = WEAR_CUE1; i <= WEAR_CUE2; ++i )
		{
			Item* pCueOfAdam = m_pWearItem[i];
			if ( pCueOfAdam != NULL && m_pRealWearingCheck[i] == true )
			{
				cues[i-WEAR_CUE1] = pCueOfAdam->getItemType();
			}
		}

		if ( cues[0] != 9999 && cues[1] != 9999 && cues[0] != cues[1] && cues[0] != 4 && cues[1] != 4) // 20070906 �߼��̺�Ʈ�� ����� �ŵ��� ��Ʈ ȿ�� ��� �ȵȴ�.
		{
//			if ( cues[0] != 4 && cues[1] != 4) // 20070906 �߼��̺�Ʈ�� ����� �ŵ��� ��Ʈ ȿ�� ��� �ȵȴ�.
			CueOfAdamInfo* pItemInfo1 = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), cues[0]));
			CueOfAdamInfo* pItemInfo2 = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), cues[1]));

			if ( pItemInfo1 != NULL )
				computeOptionStat(pItemInfo1->getSetOptionType());
			if ( pItemInfo2 != NULL )
				computeOptionStat(pItemInfo2->getSetOptionType());
		}
	}

	__END_CATCH
}

void Vampire::computeOptionStat(Item* pItem)
	throw()
{
	__BEGIN_TRY

	// Option Type�� �޾ƿ´�.
	//OptionType_t  OptionType    = pItem->getOptionType();
	//computeOptionStat( OptionType );

	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& optionType = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;
	for (itr=optionType.begin(); itr!=optionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}

	// Item ��ü�� defaultOption�� ����Ų��.
	const list<OptionType_t>& defaultOptions = pItem->getDefaultOptions();
	list<OptionType_t>::const_iterator iOptions;

	for (iOptions=defaultOptions.begin(); iOptions!=defaultOptions.end(); iOptions++)
	{
		computeOptionStat( *iOptions );
	}

	__END_CATCH
}

void Vampire::computeOptionClassStat(OptionClass OClass, int PlusPoint)
{
	switch (OClass)
	{
		case OPTION_STR:
			m_STR[ATTR_CURRENT] += PlusPoint;
			m_STR[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_DEX:
			m_DEX[ATTR_CURRENT] += PlusPoint;
			m_DEX[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_INT:
			m_INT[ATTR_CURRENT] += PlusPoint;
			m_INT[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_HP:
			m_HP[ATTR_MAX]   += PlusPoint;
			m_HP[ATTR_BASIC] += PlusPoint;
			break;
		case OPTION_MP:
			m_HP[ATTR_MAX]   += PlusPoint;
			m_HP[ATTR_BASIC] += PlusPoint;
			break;
		// �����̾�� MP�������� ���� �����۵� HP����� ó�����ش�.
		// 2003. 1. 17. Sequoia
		case OPTION_HP_STEAL:
		case OPTION_MP_STEAL:
			m_HPStealAmount += PlusPoint;
			break;
		case OPTION_HP_REGEN:
		case OPTION_MP_REGEN:
			m_HPRegen += PlusPoint;
			break;
		case OPTION_TOHIT:
			m_ToHit[ATTR_CURRENT] += PlusPoint;
			m_ToHit[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_DEFENSE:
			m_Defense[ATTR_CURRENT] += PlusPoint;
			m_Defense[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_DAMAGE:
			m_Damage[ATTR_CURRENT] += PlusPoint;
			m_Damage[ATTR_MAX]     += PlusPoint;
			m_Damage[ATTR_BASIC]   += PlusPoint;
			break;
		case OPTION_PROTECTION:
			m_Protection[ATTR_CURRENT] += PlusPoint;
			m_Protection[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_POISON:
			m_Resist[MAGIC_DOMAIN_POISON] += PlusPoint;
			break;
		case OPTION_ACID:
			m_Resist[MAGIC_DOMAIN_ACID] += PlusPoint;
			break;
		case OPTION_CURSE:
			m_Resist[MAGIC_DOMAIN_CURSE] += PlusPoint;
			break;
		case OPTION_BLOOD:
			m_Resist[MAGIC_DOMAIN_BLOOD] += PlusPoint;
			break;
		case OPTION_VISION:
			break;
		case OPTION_ATTACK_SPEED:
			increaseAttackSpeed(PlusPoint, ATTR_CURRENT);
			increaseAttackSpeed(PlusPoint, ATTR_MAX);
			break;
		case OPTION_CRITICAL_HIT:
			m_CriticalRatio[ATTR_CURRENT] += PlusPoint;
			m_CriticalRatio[ATTR_MAX]     += PlusPoint;
			break;

		case OPTION_ALL_ATTR:
			m_STR[ATTR_CURRENT] += PlusPoint;
			m_STR[ATTR_MAX]     += PlusPoint;

			m_DEX[ATTR_CURRENT] += PlusPoint;
			m_DEX[ATTR_MAX]     += PlusPoint;

			m_INT[ATTR_CURRENT] += PlusPoint;
			m_INT[ATTR_MAX]     += PlusPoint;

			computeStatOffset();
			break;

		case OPTION_ALL_RES:
			m_Resist[MAGIC_DOMAIN_POISON] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_ACID] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_CURSE] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_BLOOD] += PlusPoint;
///////////////////////////////////////////////////////////////////////////
//	������ ��� ����Ʈ�� ����...			
			m_Resist[MAGIC_DOMAIN_ALLRES] += PlusPoint;
			
			break;

		case OPTION_LUCK:
			m_Luck += PlusPoint;
			break;

		case OPTION_CONSUME_MP:
			m_ConsumeMPRatio += PlusPoint;
			break;

		case OPTION_MAGIC_DAMAGE:
			m_MagicBonusDamage += PlusPoint;
			break;

		case OPTION_PHYSIC_DAMAGE:
			m_PhysicBonusDamage += PlusPoint;
			break;

		case OPTION_GAMBLE_PRICE:
			m_GamblePriceRatio += PlusPoint;
			break;
			
		case OPTION_POTION_PRICE:
			m_PotionPriceRatio += PlusPoint;
			break;
		case OPTION_PHYSIC_PRO:
			m_PhysicDamageReduce += PlusPoint;
			break;
		case OPTION_MAGIC_PRO:
			m_MagicDamageReduce += PlusPoint;
			break;

		default:
			break;
	}
}

void Vampire::computeOptionStat(OptionType_t OptionType)
	throw()
{
	__BEGIN_TRY

	OptionInfo*   pOptionInfo   = g_pOptionInfoManager->getOptionInfo(OptionType);
	if ( pOptionInfo == NULL )
	{
		filelog("InvalidOptionInfo.log", "Vampire::computeOptionStat() Name=%s InvalidOptionType=%d",
			getName().c_str(), OptionType
		);
		return;
	}

	computeOptionClassStat( pOptionInfo->getClass(), pOptionInfo->getPlusPoint() );

/*	OptionClass   OClass        = pOptionInfo->getClass();

	switch (OClass)
	{
		case OPTION_STR:
			m_STR[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_STR[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_DEX:
			m_DEX[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_DEX[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_INT:
			m_INT[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_INT[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_HP:
			m_HP[ATTR_MAX]   += pOptionInfo->getPlusPoint();
			m_HP[ATTR_BASIC] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_MP:
			m_HP[ATTR_MAX]   += pOptionInfo->getPlusPoint();
			m_HP[ATTR_BASIC] += pOptionInfo->getPlusPoint();
			break;
		// �����̾�� MP�������� ���� �����۵� HP����� ó�����ش�.
		// 2003. 1. 17. Sequoia
		case OPTION_HP_STEAL:
		case OPTION_MP_STEAL:
			m_HPStealAmount += pOptionInfo->getPlusPoint();
			break;
		case OPTION_HP_REGEN:
		case OPTION_MP_REGEN:
			m_HPRegen += pOptionInfo->getPlusPoint();
			break;
		case OPTION_TOHIT:
			m_ToHit[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_ToHit[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_DEFENSE:
			m_Defense[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Defense[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_DAMAGE:
			m_Damage[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Damage[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			m_Damage[ATTR_BASIC]   += pOptionInfo->getPlusPoint();
			break;
		case OPTION_PROTECTION:
			m_Protection[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Protection[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_POISON:
			m_Resist[MAGIC_DOMAIN_POISON] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_ACID:
			m_Resist[MAGIC_DOMAIN_ACID] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_CURSE:
			m_Resist[MAGIC_DOMAIN_CURSE] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_BLOOD:
			m_Resist[MAGIC_DOMAIN_BLOOD] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_VISION:
			break;
		case OPTION_ATTACK_SPEED:
			m_AttackSpeed[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_AttackSpeed[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_CRITICAL_HIT:
			m_CriticalRatio[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_CriticalRatio[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;

		case OPTION_ALL_ATTR:
			m_STR[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_STR[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			m_DEX[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_DEX[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			m_INT[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_INT[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			computeStatOffset();
			break;

		case OPTION_ALL_RES:
			m_Resist[MAGIC_DOMAIN_POISON] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_ACID] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_CURSE] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_BLOOD] += pOptionInfo->getPlusPoint();
			break;

		case OPTION_LUCK:
			m_Luck += pOptionInfo->getPlusPoint();
			break;

		case OPTION_CONSUME_MP:
			m_ConsumeMPRatio = pOptionInfo->getPlusPoint();
			break;

		case OPTION_MAGIC_DAMAGE:
			m_MagicBonusDamage = pOptionInfo->getPlusPoint();
			break;

		case OPTION_PHYSIC_DAMAGE:
			m_PhysicBonusDamage = pOptionInfo->getPlusPoint();
			break;

		case OPTION_GAMBLE_PRICE:
			m_GamblePriceRatio = pOptionInfo->getPlusPoint();
			break;
			
		case OPTION_POTION_PRICE:
			m_PotionPriceRatio = pOptionInfo->getPlusPoint();
			break;

		default:
			break;
	}
*/
	__END_CATCH
}

void Vampire::addModifyInfo(const VAMPIRE_RECORD& prev, ModifyInfo& pkt) const
	throw()
{
	__BEGIN_TRY

	if (prev.pSTR[ATTR_CURRENT] != m_STR[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_STR, m_STR[ATTR_CURRENT]);
	if (prev.pSTR[ATTR_MAX    ] != m_STR[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_STR,     m_STR[ATTR_MAX]);
	if (prev.pSTR[ATTR_BASIC  ] != m_STR[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_STR,   m_STR[ATTR_BASIC]);

	if (prev.pDEX[ATTR_CURRENT] != m_DEX[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_DEX, m_DEX[ATTR_CURRENT]);
	if (prev.pDEX[ATTR_MAX    ] != m_DEX[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_DEX,     m_DEX[ATTR_MAX]);
	if (prev.pDEX[ATTR_BASIC  ] != m_DEX[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_DEX,   m_DEX[ATTR_BASIC]);

	if (prev.pINT[ATTR_CURRENT] != m_INT[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_INT, m_INT[ATTR_CURRENT]);
	if (prev.pINT[ATTR_MAX    ] != m_INT[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_INT,     m_INT[ATTR_MAX]);
	if (prev.pINT[ATTR_BASIC  ] != m_INT[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_INT,   m_INT[ATTR_BASIC]);

	if (prev.pHP[ATTR_MAX    ] != m_HP[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_HP,     m_HP[ATTR_MAX]);
	if (prev.pHP[ATTR_CURRENT] != m_HP[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_HP, m_HP[ATTR_CURRENT]);

	if (prev.pDamage[ATTR_CURRENT] != m_Damage[ATTR_CURRENT]) pkt.addShortData(MODIFY_MIN_DAMAGE, m_Damage[ATTR_CURRENT]);
	if (prev.pDamage[ATTR_MAX    ] != m_Damage[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_DAMAGE, m_Damage[ATTR_MAX]);

	if (prev.Defense     != m_Defense[ATTR_CURRENT])     pkt.addShortData(MODIFY_DEFENSE,      m_Defense[ATTR_CURRENT]);
	if (prev.Protection  != m_Protection[ATTR_CURRENT])  pkt.addShortData(MODIFY_PROTECTION,   m_Protection[ATTR_CURRENT]);
	if (prev.ToHit       != m_ToHit[ATTR_CURRENT])       pkt.addShortData(MODIFY_TOHIT,        m_ToHit[ATTR_CURRENT]);
	if (prev.AttackSpeed != getAttackSpeed(ATTR_CURRENT)) pkt.addShortData(MODIFY_ATTACK_SPEED, getAttackSpeed(ATTR_CURRENT));

	// by sigi. 2002.9.10
	if (prev.Rank != getRank())
	{
		pkt.addShortData(MODIFY_RANK, getRank());
		pkt.addLongData(MODIFY_RANK_EXP, getRankGoalExp());
	}

	if (prev.Bonus != m_Bonus)
	{
		pkt.addShortData(MODIFY_BONUS_POINT, m_Bonus);
	}
#ifdef __CONTRIBUTION_SYSTEM__
	if(prev.ContributePoint != m_ContributePoint)
		pkt.addLongData(MODIFY_CONTRIBUTE_POINT, m_ContributePoint);
#endif

	if (prev.Luck != m_Luck)
	{
		pkt.addShortData(MODIFY_LUCK, m_Luck);
	}
	if (prev.HpSteal != m_HPStealAmount)
	{
		pkt.addShortData(MODIFY_HPSTEAL, m_HPStealAmount);
		pkt.addShortData(MODIFY_MPSTEAL, m_HPStealAmount);
	}
	if (prev.AllRes != m_Resist[MAGIC_DOMAIN_ALLRES])
	{
		pkt.addShortData(MODIFY_ALLRES, m_Resist[MAGIC_DOMAIN_ALLRES]);
	}
	if (prev.Cri != m_CriticalRatio[ATTR_MAX])
	{
		pkt.addShortData(MODIFY_CRI, m_CriticalRatio[ATTR_MAX]);
	}
	__END_CATCH
}

void Vampire::sendModifyInfo(const VAMPIRE_RECORD& prev) const
	throw()
{
	__BEGIN_TRY

	GCModifyInformation gcModifyInformation;
	addModifyInfo(prev, gcModifyInformation);
	m_pPlayer->sendPacket(&gcModifyInformation);

	BloodBibleSignInfo* pInfo = getBloodBibleSign();
	GCBloodBibleSignInfo gcInfo;
	gcInfo.setSignInfo( pInfo );
//	cout << "open num : " << pInfo->getOpenNum() << endl;;
	m_pPlayer->sendPacket( &gcInfo );

	__END_CATCH	
}

void Vampire::initAllStatAndSend()
{
	VAMPIRE_RECORD prev;
	getVampireRecord(prev);
	initAllStat();
	sendRealWearingInfo();
	sendModifyInfo(prev);
	//sendSkillInfo();
}

//////////////////////////////////////////////////////////////////////////////
//
// ����
//
//////////////////////////////////////////////////////////////////////////////
void Monster::initAllStat(void)
	throw()
{
	__BEGIN_TRY

	if ( m_MonsterType == 753 ||  m_MonsterType == 1358) return;

	MonsterInfo* pMonsterInfo;
	
	if ( m_pMonsterInfo == NULL )
	{
		pMonsterInfo = (MonsterInfo *)g_pMonsterInfoManager->getMonsterInfo(m_MonsterType);		
	}
	else
	{
		pMonsterInfo = m_pMonsterInfo;
	}

	Creature::CreatureClass CClass = getCreatureClass();
	BASIC_ATTR              attr;

	m_Resist[MAGIC_DOMAIN_NO_DOMAIN] = 0;
	m_Resist[MAGIC_DOMAIN_POISON]    = 50;
	m_Resist[MAGIC_DOMAIN_ACID]      = 0;
	m_Resist[MAGIC_DOMAIN_CURSE]     = 0;
	m_Resist[MAGIC_DOMAIN_BLOOD]     = 0;
////////////////////////////////////////////////////////////////////////
//	������ ��� ���� �ʱ�ȭ	
	m_Resist[MAGIC_DOMAIN_ALLRES]    = 0;
	 
	////////////////////////////////////////////////////////////
	// ���� ���� �⺻ �ɷ�ġ�� �ʱ�ȭ��Ű��...
	////////////////////////////////////////////////////////////
	m_STR = pMonsterInfo->getSTR();
	m_DEX = pMonsterInfo->getDEX();
	m_INT = pMonsterInfo->getINT();

	////////////////////////////////////////////////////////////
	// �⺻ �ɷ¿� ������ �ִ� ����Ʈ�� �˻��Ѵ�.
	////////////////////////////////////////////////////////////
	attr.nSTR   = m_STR;
	attr.nDEX   = m_DEX;
	attr.nINT   = m_INT;
	attr.nLevel = pMonsterInfo->getLevel();

	////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ���� �ٽ� ����Ѵ�.
	////////////////////////////////////////////////////////////
	//m_HP[ATTR_MAX]         = computeHP(CClass, &attr, pMonsterInfo->getEnhanceHP());
	m_HP[ATTR_MAX]         = computeTestModeHP(this, &attr, getEnhanceHP());
	m_ToHit                = computeToHit(CClass, &attr, pMonsterInfo->getEnhanceToHit());
	m_Defense              = computeDefense(CClass, &attr, pMonsterInfo->getEnhanceDefense());
	m_Protection           = computeProtection(CClass, &attr, pMonsterInfo->getEnhanceProtection());
	m_Damage[ATTR_CURRENT] = computeMinDamage(CClass, &attr, pMonsterInfo->getEnhanceMinDamage());
	m_Damage[ATTR_MAX]     = computeMaxDamage(CClass, &attr, pMonsterInfo->getEnhanceMaxDamage());

	// 20070918 ���ʹ� ���� ��� ��������;;;
//	m_MagicDefense		   = computeMagicDefense( CClass, &attr );
//	m_MagicToHit  		   = computeMagicToHit( CClass, &attr );


//#ifdef __XMAS_EVENT_CODE__
	// �� ũ�������� �̺�Ʈ�� ��ȹ�� ��������, ������ ��� ���� ������
	// �����ȴ�. 
	if (m_MonsterType == 358 || m_MonsterType == 359 || m_MonsterType == 360 || m_MonsterType == 361)
		m_HP[ATTR_MAX] = m_HP[ATTR_MAX] * 10;
//#endif
	
	////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ���� ���� �����ϴ� ����Ʈ�� �˻��Ѵ�.
	////////////////////////////////////////////////////////////
	if (isFlag(Effect::EFFECT_CLASS_DOOM))
	{
		EffectDoom* pDoom = dynamic_cast<EffectDoom*>(findEffect(Effect::EFFECT_CLASS_DOOM));
		if (pDoom!= NULL)
		{
			int DefensePenalty    = getPercentValue(m_Defense,    pDoom->getDefensePenalty());
			int ProtectionPenalty = getPercentValue(m_Protection, pDoom->getProtectionPenalty());

			m_Defense    = max(0, m_Defense - DefensePenalty);
			m_Protection = max(0, m_Protection - ProtectionPenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_SEDUCTION))//monster
	{
		EffectSeduction* pSeduction = dynamic_cast<EffectSeduction*>(findEffect(Effect::EFFECT_CLASS_SEDUCTION));
		if (pSeduction!= NULL)
		{
			int ToHitPenalty   = getPercentValue(m_ToHit,                pSeduction->getToHitPenalty());
			int DamagePenalty1 = getPercentValue(m_Damage[ATTR_CURRENT], pSeduction->getDamagePenalty());
			int DamagePenalty2 = getPercentValue(m_Damage[ATTR_MAX],     pSeduction->getDamagePenalty());

			m_ToHit                = max(0, m_ToHit - ToHitPenalty);
			m_Damage[ATTR_CURRENT] = max(0, m_Damage[ATTR_CURRENT] - DamagePenalty1);
			m_Damage[ATTR_MAX]     = max(0, m_Damage[ATTR_MAX] - DamagePenalty2);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2))//monster
	{
		EffectOUTRAGEBOOSTER2* pOUTRAGEBOOSTER = dynamic_cast<EffectOUTRAGEBOOSTER2*>(findEffect(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2));
		if (pOUTRAGEBOOSTER!= NULL)
		{
			int STRPenalty =    (pOUTRAGEBOOSTER->getSTRPenalty())*5;
			//int DEXPenalty =    pOUTRAGEBOOSTER->getDEXPenalty();
			//int INTPenalty =    pOUTRAGEBOOSTER->getINTPenalty();

			m_Damage[ATTR_CURRENT] -= STRPenalty;
			m_Damage[ATTR_MAX]     -= STRPenalty;
		}
	}
/*	if (isFlag(Effect::EFFECT_CLASS_PARALYZE))
	{
		EffectParalyze* pParalyze = dynamic_cast<EffectParalyze*>(findEffect(Effect::EFFECT_CLASS_PARALYZE));
		if (pParalyze != NULL)
		{
			int DefensePenalty = getPercentValue(m_Defense, pParalyze->getDefensePenalty());
			m_Defense = max(0, m_Defense - DefensePenalty);
		}
	}*/
	if (isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF))
	{
		EffectTransformToWolf* pTransformToWolf= dynamic_cast<EffectTransformToWolf*>(findEffect(Effect::EFFECT_CLASS_TRANSFORM_TO_WOLF));
		if (pTransformToWolf != NULL)
		{
			int ToHitBonus        = getPercentValue(m_ToHit, 20);
			int MinDamageBonus    = getPercentValue(m_Damage[ATTR_CURRENT], 20);
			int MaxDamageBonus    = getPercentValue(m_Damage[ATTR_MAX], 20);
			int DefensePenalty    = getPercentValue(m_Defense, 50);
			int ProtectionPenalty = getPercentValue(m_Protection, 50);

			m_ToHit                = min(VAMPIRE_MAX_TOHIT, m_ToHit + ToHitBonus);
			m_Damage[ATTR_CURRENT] = min(VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + MinDamageBonus);
			m_Damage[ATTR_MAX]     = min(VAMPIRE_MAX_DAMAGE, m_Damage[ATTR_MAX] + MaxDamageBonus);
			m_Defense              = max(0, m_Defense - DefensePenalty);
			m_Protection           = max(0, m_Protection - ProtectionPenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT))
	{
		EffectTransformToBat* pTransformToBat = dynamic_cast<EffectTransformToBat*>(findEffect(Effect::EFFECT_CLASS_TRANSFORM_TO_BAT));
		if (pTransformToBat != NULL)
		{
			int DefensePenalty    = getPercentValue(m_Defense, 25);
			int ProtectionPenalty = getPercentValue(m_Protection, 25);

			m_Defense    = max(0, m_Defense - DefensePenalty);
			m_Protection = max(0, m_Protection - ProtectionPenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_BLUNTING))
	{
		EffectBlunting* pBlunting = dynamic_cast<EffectBlunting*>(findEffect(Effect::EFFECT_CLASS_BLUNTING));
		if ( pBlunting != NULL )
		{
			int DefensePenalty = pBlunting->getDefensePenalty();
			m_Defense = max(0, m_Defense - DefensePenalty);
		}
	}

	for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
	{
		if ( m_Resist[i] < 0 ) m_Resist[i] = 0;
		//cout << i << " : " << m_Resist[i] << endl;
	}
	
	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// 
// �ƿ콺�ͽ�
//
//////////////////////////////////////////////////////////////////////////////

void Ousters::initCastleSkill() throw()
{
	__BEGIN_TRY

	removeAllCastleSkill();

	if ( !getZone()->isHolyLand() ) return;

	list<CastleInfo*> pCastleInfoList = g_pCastleInfoManager->getGuildCastleInfos( getGuildID() );
	if ( pCastleInfoList.empty() ) return;

	list<CastleInfo*>::iterator itr = pCastleInfoList.begin();

	for ( ; itr != pCastleInfoList.end() ; itr++ )
	{
		SkillType_t CastleSkillType = g_pCastleInfoManager->getCastleSkillType( (*itr)->getZoneID(), getGuildID() );
		if ( CastleSkillType == SKILL_MAX ) 
			continue;

		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo(CastleSkillType);
		Assert( pSkillInfo != NULL );

		Turn_t Delay = pSkillInfo->getMaxDelay();

		OustersCastleSkillSlot* pCastleSkillSlot = new OustersCastleSkillSlot();

		pCastleSkillSlot->setName( m_Name );
		pCastleSkillSlot->setSkillType( CastleSkillType );
		pCastleSkillSlot->setExpLevel( 1 );
		pCastleSkillSlot->setInterval(Delay);
		pCastleSkillSlot->setRunTime();

		addSkill( pCastleSkillSlot );
	}

	__END_CATCH
}

void Ousters::initAllStat(int numPartyMember)
	throw()
{
	__BEGIN_TRY

	BASIC_ATTR attr;
	Creature::CreatureClass CClass = getCreatureClass();

	m_Resist[MAGIC_DOMAIN_NO_DOMAIN] = 0;
	m_Resist[MAGIC_DOMAIN_POISON]    = 0;
	m_Resist[MAGIC_DOMAIN_ACID]      = 0;
	m_Resist[MAGIC_DOMAIN_CURSE]     = 0;
	m_Resist[MAGIC_DOMAIN_BLOOD]     = 0;
////////////////////////////////////////////////////////////////////////
//	������ ��� ���� �ʱ�ȭ	
	m_Resist[MAGIC_DOMAIN_ALLRES]    = 0;

	// BloodBible ��� ���ʽ� ��ġ�� �ʱ�ȭ
	m_ConsumeMPRatio	= 0;
	m_GamblePriceRatio	= 0;
	m_PotionPriceRatio	= 0;
	m_MagicBonusDamage	= 0;
	m_PhysicBonusDamage	= 0;
	m_MagicDamageReduce = 0;
	m_PhysicDamageReduce= 0;

	//////////////////////////////////////////////////////////////////////////////
	// ���� ���� �⺻ �ɷ�ġ�� �ʱ�ȭ��Ű��, 
	// �⺻ �ɷ�ġ�� ������ �ִ� ����Ʈ�� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	m_STR[ATTR_CURRENT] = m_STR[ATTR_MAX] = m_STR[ATTR_BASIC];
	m_DEX[ATTR_CURRENT] = m_DEX[ATTR_MAX] = m_DEX[ATTR_BASIC];
	m_INT[ATTR_CURRENT] = m_INT[ATTR_MAX] = m_INT[ATTR_BASIC];

	//////////////////////////////////////////////////////////////////////////////
	// �ɷ�ġ ����� ���� �Ķ���͵��� �ʱ�ȭ�Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	attr.nSTR    = m_STR[ATTR_CURRENT];
	attr.nDEX    = m_DEX[ATTR_CURRENT];
	attr.nINT    = m_INT[ATTR_CURRENT];
	attr.pWeapon = getWearItem(WEAR_RIGHTHAND);
	attr.nLevel  = m_Level;

	m_HPStealRatio  = 0;
	m_MPStealRatio  = 0;

	m_HPStealAmount = 0;
	m_MPStealAmount = 0;

	m_HPRegen		= 0;
	m_MPRegen		= 0;
	m_Luck			= m_BaseLuck;
//	cout << getName() << "�� �⺻ ��� : " << m_Luck << endl;

	m_FireDamage	= 0;
	m_WaterDamage	= 0;
	m_EarthDamage	= 0;

	m_SilverResist	= 0;

	m_ElementalFire = 0;
	m_ElementalWater = 0;
	m_ElementalEarth = 0;
	m_ElementalWind = 0;

	m_PassiveRatio = 0;

	////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ���� �ٽ� ����Ѵ�.
	////////////////////////////////////////////////////////////
	//m_HP[ATTR_MAX]                = computeHP(CClass, &attr);
	m_HP[ATTR_MAX]                = computeTestModeHP(this, &attr);
	m_HP[ATTR_BASIC]              = 0;
	m_MP[ATTR_MAX]                = computeMP(CClass, &attr);
	m_MP[ATTR_BASIC]              = 0;
	m_ToHit[ATTR_CURRENT]         = computeToHit(CClass, &attr);
	m_ToHit[ATTR_MAX]             = 0;
	m_Defense[ATTR_CURRENT]       = computeDefense(CClass, &attr);
	m_Defense[ATTR_MAX]           = 0;
	m_Protection[ATTR_CURRENT]    = computeProtection(CClass, &attr);
	m_Protection[ATTR_MAX]        = 0;
	m_Damage[ATTR_CURRENT]        = computeMinDamage(CClass, &attr);
	m_Damage[ATTR_MAX]            = computeMaxDamage(CClass, &attr);
	m_Damage[ATTR_BASIC]          = 0;
	setAttackSpeed(computeAttackSpeed(CClass, &attr), ATTR_CURRENT);
	setAttackSpeed(0, ATTR_MAX);
	m_CriticalRatio[ATTR_CURRENT] = computeCriticalRatio(CClass, &attr);
	m_CriticalRatio[ATTR_MAX]     = 0;

	// 20070918
	m_MagicDefense				  = computeMagicDefense(CClass, &attr);
	m_MagicToHit  				  = computeMagicToHit(CClass, &attr);

	int RaceWarHPBonus = 0;

	if ( m_pZone->isHolyLand() || m_pZone->isLevelWarZone() )
	{
		RaceWarHPBonus = getPercentValue( m_HP[ATTR_MAX], g_pVariableManager->getRaceWarHPBonus() );
	}

	int DragonEyeHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_DRAGON_EYE ) )
	{
		// HP ���ʽ��� �ι�
		DragonEyeHPBonus = m_HP[ATTR_MAX];
	}

#ifdef __CONTRIBUTION_SYSTEM__
	int HeroItemHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_OUSTERS_HERO_ITEM ) )
	{
		// HP ���ʽ��� �ټ��� 
	        HeroItemHPBonus = m_HP[ATTR_MAX] * 2; // by svi 4->2
	}

	int LevelHeroItemHPBonus = 0;
	if ( isFlag( Effect::EFFECT_CLASS_OUSTERS_LEVELWAR_HERO_ITEM ) )
	{
		// HP ���ʽ��� �ټ��� 
		LevelHeroItemHPBonus = m_HP[ATTR_MAX] * 2;
	}
#endif

	// 2006.05.29
	// STR + 15
	if( isFlag(Effect::EFFECT_CLASS_GREYFELL_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_STR, 15 );
	}
	// DEX + 15
	if( isFlag(Effect::EFFECT_CLASS_SOVIUS_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_DEX, 15 );
	}
	// INT + 15
	if( isFlag(Effect::EFFECT_CLASS_MIR_FORCE_SCROLL) )
	{
		computeOptionClassStat( OPTION_INT, 15 );
	}
	// 20081028 wlzzi - Extreme Force Scroll
	if ( isFlag(Effect::EFFECT_CLASS_EXTREME_FORCE_SCROLL))
	{
		computeOptionClassStat (OPTION_ALL_ATTR, 8);
	}
	//////////////////////////////////////////////////////////////////////////////
	// �ϴ� ��� üũ ������ �ʱ�ȭ�ؼ� ��� ��� �� ���� ������ �����ϰ� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	bool pOldRealWearingCheck[OUSTERS_WEAR_MAX];	// by sigi. 2002.10.31
	for (int i=0; i<OUSTERS_WEAR_MAX; i++) 
	{
		pOldRealWearingCheck[i] = m_pRealWearingCheck[i];
		m_pRealWearingCheck[i] = false;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Blood Bible ������ ���ʽ� �ɼ��� �޴´�.
	//////////////////////////////////////////////////////////////////////////////
/*	if ( m_pZone->isHolyLand() && !g_pWarSystem->hasActiveRaceWar() )
	{
		const BloodBibleBonusHashMap& bloodBibleBonus = g_pBloodBibleBonusManager->getBloodBibleBonuses();
		BloodBibleBonusHashMapConstItor itr;
		for (itr=bloodBibleBonus.begin(); itr!=bloodBibleBonus.end(); itr++)
		{
			if ( itr->second->getRace() == RACE_OUSTERS )
			{
				OptionTypeList optionTypes = itr->second->getOptionTypeList();
				OptionTypeListConstItor optionItr;
				
				for ( optionItr = optionTypes.begin(); optionItr != optionTypes.end(); optionItr++ )
				{
					computeOptionStat( *optionItr );
				}
			}
		}
	}*/

	if ( g_pSweeperBonusManager->isAble( getZoneID() ) && g_pLevelWarZoneInfoManager->isCreatureBonusZone( this, getZoneID() ))
//	2007 01 03
//	if ( g_pLevelWarZoneInfoManager->isCreatureBonusZone( this, getZoneID() ))
	{
		const SweeperBonusHashMap& sweeperBonuses = g_pSweeperBonusManager->getSweeperBonuses();

		SweeperBonusHashMapConstItor itr = sweeperBonuses.begin();
		SweeperBonusHashMapConstItor endItr = sweeperBonuses.end();

		for (; itr!=endItr; itr++)
		{
			if ( itr->second->getRace() == RACE_OUSTERS && itr->second->getLevel() == g_pLevelWarZoneInfoManager->getCreatureLevelGrade( this ) )
			{
				OptionTypeList optionTypes = itr->second->getOptionTypeList();
				OptionTypeListConstItor optionItr;
				
				for ( optionItr = optionTypes.begin(); optionItr != optionTypes.end(); optionItr++ )
				{
					computeOptionStat( *optionItr );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////
	// �⺻������ ������ �ִ� �ɼǵ��� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	list<DefaultOptionSetType_t>::iterator itr = m_DefaultOptionSet.begin();
	for ( ; itr != m_DefaultOptionSet.end(); itr++ )
	{
		DefaultOptionSetInfo* pDefaultOptionSetInfo = g_pDefaultOptionSetInfoManager->getDefaultOptionSetInfo( (*itr) );
		if ( pDefaultOptionSetInfo != NULL )
		{
			const list<OptionType_t>& optionList = pDefaultOptionSetInfo->getOptionTypeList();
			list<OptionType_t>::const_iterator citr;
			for ( citr = optionList.begin(); citr != optionList.end(); citr++ )
			{
				computeOptionStat( *citr );
			}
		}
	}

	list<OptionType_t>::iterator oitr = m_DefaultOptions.begin();
	list<OptionType_t>::iterator oend = m_DefaultOptions.end();
	for ( ; oitr != oend; ++oitr )
	{
		computeOptionStat( *oitr );
	}

	HashMapObjectOptionConstItor ooitr = m_EffectOptions.begin();
	HashMapObjectOptionConstItor ooendItr = m_EffectOptions.end();

	for ( ; ooitr != ooendItr; ++ooitr )
	{
		computeOptionStat( ooitr->second );
	}
	//////////////////////////////////////////////////////////////////////////////
	// ���� �ִ� ���ʽ��� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	if ( m_pPetInfo != NULL )
	{
		if ( m_pPetInfo->getPetAttr() != 0xff ) computeOptionClassStat( (OptionClass)m_pPetInfo->getPetAttr(), (int)m_pPetInfo->getPetAttrLevel() );
		if ( m_pPetInfo->getPetOption() != 0 ) computeOptionStat( m_pPetInfo->getPetOption() );
		//1203 ~ 1204 wlzzi Pet's OptionType �߰�
		const list<OptionType_t>& optionList = m_pPetInfo->getOptionType();
		list<OptionType_t>::const_iterator itr;
		for ( itr = optionList.begin(); itr != optionList.end(); itr++ )
		{
			computeOptionStat( *itr );
		}
		if ( m_pPetInfo->getPetOption2() != 0 ) computeOptionStat( m_pPetInfo->getPetOption2() );
		if ( m_pPetInfo->getPetOption3() != 0 ) computeOptionStat( m_pPetInfo->getPetOption3() );
		//-
		//20090109 wlzzi - Ǫ��������
		switch (m_pPetInfo->getFunctionPetfoodType ())
		{
		case FOOD_FUNCTION_LUCKY:
			computeOptionClassStat ((OptionClass)21, 10);	//���+10 .. ���� �� ������.. �Ѥ�;�۾����� �ϴ� �ּ�ȭ �Ѵ�.
			break;
		}
		//-
		const list<OptionType_t>& MixoptionList = m_pPetInfo->getMixOptionType();
		list<OptionType_t>::const_iterator itr2;
		for ( itr2 = MixoptionList.begin(); itr2 != MixoptionList.end(); itr2++ )
		{
			computeOptionStat( *itr2 );
		}
	}

	if ( isFlag( Effect::EFFECT_CLASS_GROUND_BLESS ) )
	{
		EffectGroundBless* pEffect = dynamic_cast<EffectGroundBless*>(findEffect(Effect::EFFECT_CLASS_GROUND_BLESS));

		if ( pEffect != NULL )
		{
			int bonus = pEffect->getBonus();
			m_STR[ATTR_CURRENT] += bonus;
			m_DEX[ATTR_CURRENT] += bonus;
			m_INT[ATTR_CURRENT] += bonus;
			m_STR[ATTR_MAX] += bonus;
			m_DEX[ATTR_MAX] += bonus;
			m_INT[ATTR_MAX] += bonus;

			computeStatOffset();
		}
	}
	for(int i=NewRankGem::SMALL_RED_STR;i<NewRankGem::SKILL_BLADE_STORM_DAM;i++)
   	{
		if(hasRankGem((NewRankGem::RankGemType)i))
		{
			NewRankGem* pRankGem = getRankGem((NewRankGem::RankGemType)i);
			Assert( pRankGem != NULL );
			computeOptionClassStat(pRankGem->getOptionClass(), pRankGem->getGrade());
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_TaoZhuang))//
	{
		EffectTaoZhuang* pEffect = dynamic_cast<EffectTaoZhuang*>(findEffect(Effect::EFFECT_CLASS_TaoZhuang));
		if (pEffect!= NULL)
		{
			/*int tz211[6][2] =
				{
					{23,5},//+5
					{3,5000},//HP+1000
					{32,10},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz221[6][2] =
				{
					{23,10},//+5
					{3,6000},//HP+1000
					{32,20},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz231[6][2] =
				{
					{23,15},//+5
					{3,7000},//HP+1000
					{32,30},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz241[6][2] =
				{
					{23,20},//+5
					{3,8000},//HP+1000
					{32,50},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};*/
			int tz211[6][2] =
				{
					{23,20},//+5
					{3,5000},//HP+1000
					{32,30},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz221[6][2] =
				{
					{23,30},//+5
					{3,6000},//HP+1000
					{32,40},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz231[6][2] =
				{
					{23,40},//+5
					{3,7000},//HP+1000
					{32,50},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz241[6][2] =
				{
					{23,50},//+5
					{3,8000},//HP+1000
					{32,60},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int tz251[6][2] =
				{
					{23,60},//+5
					{3,9000},//HP+1000
					{32,70},//�� �+10
					{0,0},
					{0,0},
					{0,0}
				};
			int TzGourp = pEffect->getTzGroup();
			int Num 	= pEffect->getNum();
				for(int i = 0;i<(Num-1);i++)
				{
					if(tz211[i][0]>0 && TzGourp == 1)
					{
					  computeOptionClassStat ((OptionClass)tz211[i][0], tz211[i][1]);
					}
					if(tz221[i][0]>0 && TzGourp == 2)
					{
					  computeOptionClassStat ((OptionClass)tz221[i][0], tz221[i][1]);
					}
					if(tz231[i][0]>0 && TzGourp == 3)
					{
					  computeOptionClassStat ((OptionClass)tz231[i][0], tz231[i][1]);
					}
					if(tz241[i][0]>0 && TzGourp == 4)
					{
					  computeOptionClassStat ((OptionClass)tz241[i][0], tz241[i][1]);
					}
					if(tz251[i][0]>0 && TzGourp == 5)
					{
					  computeOptionClassStat ((OptionClass)tz251[i][0], tz251[i][1]);
					}
				}
		}
	}
	int HPBonus = 0;

	if ( isFlag(Effect::EFFECT_CLASS_STRENGTHEN_INCENSE) )
	{
		computeOptionClassStat( OPTION_ALL_ATTR, 3 );
		HPBonus += 20;
	}
	// ���� ���� ��� ���
	applyBloodBibleSign();
	CheckValidWearingItem();

	// 2006.09.08 by Shinobi
	//list<OptionType_t> ContractOptionList = getContractOfBloodKindByItemType( (PlayerCreature*)this );
	list<OptionType_t> ContractOptionList = getContractOfBloodPositionByItemType( getInventory() );
	list<OptionType_t>::iterator ContractOptionIter = ContractOptionList.begin();

	for(; ContractOptionIter != ContractOptionList.end(); ++ContractOptionIter) 
	{
		computeOptionStat(*ContractOptionIter);
	}

	// by sigi. 2002.11.6
	bool bSendPacket = false;

	if (m_pPlayer!=NULL)
	{	
		bSendPacket = (dynamic_cast<GamePlayer*>(m_pPlayer)->getPlayerStatus()==GPS_NORMAL);
	}

	SendChangeShapes(bSendPacket);

	//////////////////////////////////////////////////////////////////////////////
	// HP, MP ��ƿ Ȯ���� ����� �д�.
	//////////////////////////////////////////////////////////////////////////////
	/*if ( hasRankBonus( RankBonus::RANK_BONUS_LIFE_ABSORB ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_LIFE_ABSORB );
		Assert( pRankBonus != NULL );

		int StealBonus = pRankBonus->getPoint();

		m_HPStealAmount += StealBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_SOUL_ABSORB ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SOUL_ABSORB );
		Assert( pRankBonus != NULL );

		int StealBonus = pRankBonus->getPoint();

		m_MPStealAmount += StealBonus;
	}*/

	m_HPStealRatio = computeStealRatio(CClass, m_HPStealAmount, &attr);
	m_MPStealRatio = computeStealRatio(CClass, m_MPStealAmount, &attr);
	//cout << getName() << " HPSteal : " << (int)m_HPStealAmount << endl;

	//////////////////////////////////////////////////////////////////////////////
	// �ΰ����� �ɷ�ġ�� ���� �����ϴ� ����Ʈ�� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	if (isFlag(Effect::EFFECT_CLASS_MYSTERY_POTION))
	{
		EffectMysteryPotion* pEffect = dynamic_cast<EffectMysteryPotion*>(findEffect(Effect::EFFECT_CLASS_MYSTERY_POTION));
		if( pEffect != NULL )
		{
			computeOptionStat( 185 );		// ��� + 3
			HP_t bonusHP = 50;
			Defense_t bonusDef = 10; 
			m_Defense[ATTR_MAX]  = min( OUSTERS_MAX_DEFENSE, m_Defense[ATTR_MAX] + bonusDef );
			m_Defense[ATTR_CURRENT]  = min( OUSTERS_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + bonusDef );
			m_HP[ATTR_MAX]		 = min((HP_t) OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + bonusHP ));
		}
	}

	if (isFlag(Effect::EFFECT_CLASS_DOOM))
	{
		EffectDoom* pDoom = dynamic_cast<EffectDoom*>(findEffect(Effect::EFFECT_CLASS_DOOM));
		if (pDoom!= NULL)
		{
			int DefensePenalty    = getPercentValue(m_Defense[ATTR_CURRENT], pDoom->getDefensePenalty());
			int ProtectionPenalty = getPercentValue(m_Protection[ATTR_CURRENT], pDoom->getProtectionPenalty());

			m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT]    - DefensePenalty);
			m_Protection[ATTR_CURRENT] = max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_SEDUCTION))//ouster
	{
		EffectSeduction* pSeduction = dynamic_cast<EffectSeduction*>(findEffect(Effect::EFFECT_CLASS_SEDUCTION));
		if (pSeduction!= NULL)
		{
			int ToHitPenalty   = getPercentValue(m_ToHit[ATTR_CURRENT],  pSeduction->getToHitPenalty());
			int DamagePenalty1 = getPercentValue(m_Damage[ATTR_CURRENT], pSeduction->getDamagePenalty());
			int DamagePenalty2 = getPercentValue(m_Damage[ATTR_MAX],     pSeduction->getDamagePenalty());

			m_ToHit[ATTR_CURRENT]  = max(0, m_ToHit[ATTR_CURRENT] - ToHitPenalty);
			m_Damage[ATTR_CURRENT] = max(0, m_Damage[ATTR_CURRENT] - DamagePenalty1);
			m_Damage[ATTR_MAX]     = max(0, m_Damage[ATTR_MAX] - DamagePenalty2);
		}
	}
/*	if (isFlag(Effect::EFFECT_CLASS_OUTRAGEBOOSTER2))//ouster
	{
		EffectOUTRAGEBOOSTER* pOUTRAGEBOOSTER = dynamic_cast<EffectOUTRAGEBOOSTER*>(findEffect(Effect::EFFECT_CLASS_OUTRAGEBOOSTER));
		if (pOUTRAGEBOOSTER!= NULL)
		{
			int STRPenalty =    pOUTRAGEBOOSTER->getSTRPenalty();
			int DEXPenalty =    pOUTRAGEBOOSTER->getDEXPenalty();
			int INTPenalty =    pOUTRAGEBOOSTER->getINTPenalty();

			m_STR[ATTR_CURRENT]  = max(0, m_STR[ATTR_CURRENT] - STRPenalty);
			m_DEX[ATTR_CURRENT] = max(0, m_DEX[ATTR_CURRENT] - DEXPenalty);
			m_INT[ATTR_CURRENT]     = max(0, m_INT[ATTR_CURRENT] - INTPenalty);
		}
	}*/
	if (isFlag(Effect::EFFECT_CLASS_DEATH))
	{
		EffectDeath* pDeath = dynamic_cast<EffectDeath*>(findEffect(Effect::EFFECT_CLASS_DEATH));
		if (pDeath != NULL)
		{
			for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
			{
				m_Resist[i] -= pDeath->getResistPenalty();
//				if ( m_Resist[i] < 0 ) m_Resist[i] = 0;
			}
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_ADV2GRACEOFELQUINES ) )
	{
		EffectADV2GRACEOFELQUINES* pEffect = dynamic_cast<EffectADV2GRACEOFELQUINES*>(findEffect(Effect::EFFECT_CLASS_ADV2GRACEOFELQUINES));

		if ( pEffect != NULL )
		{
			cout<<"i'm load"<<endl;
			int bonus = pEffect->getBonus();

			int havebuffs = pEffect->getAddbuff();
			if (havebuffs == 2)
			{
				 m_PhysicBonusDamage += 70;
				 m_MagicBonusDamage += 70;
			}
			m_STR[ATTR_CURRENT] += getPercentValue( m_STR[ATTR_CURRENT], bonus );
			m_DEX[ATTR_CURRENT] += getPercentValue( m_DEX[ATTR_CURRENT], bonus );
			m_INT[ATTR_CURRENT] += getPercentValue( m_INT[ATTR_CURRENT], bonus );

			m_STR[ATTR_MAX] += getPercentValue( m_STR[ATTR_MAX], bonus );
			m_DEX[ATTR_MAX] += getPercentValue( m_DEX[ATTR_MAX], bonus );
			m_INT[ATTR_MAX] += getPercentValue( m_INT[ATTR_MAX], bonus );

			computeStatOffset();

		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_ALLYSTER_WIND ) )
	{
		EffectALLYSTERWIND* pEffect = dynamic_cast<EffectALLYSTERWIND*>(findEffect(Effect::EFFECT_CLASS_ALLYSTER_WIND));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
                           {

				m_Defense[ATTR_CURRENT] = min( OUSTERS_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + (Defense_t)pEffect->getBonus() );
				m_ToHit[ATTR_CURRENT] = min( OUSTERS_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + (ToHit_t)pEffect->getBonus() );
		                m_Damage[ATTR_CURRENT]  = min(OUSTERS_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + (Damage_t)pEffect->getBonus());
		                m_Damage[ATTR_MAX]      = min(OUSTERS_MAX_DAMAGE, m_Damage[ATTR_MAX] + (Damage_t)pEffect->getBonus());
                          }
			else
				pEffect->setDeadline(0);
		}
	}
	if ( isFlag( Effect:: EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT) )
	{
		EffectADV2THERAGEOFGUARDIANSPRIT* pEffect = dynamic_cast<EffectADV2THERAGEOFGUARDIANSPRIT*>(findEffect(Effect::EFFECT_CLASS_ADV2THERAGEOFGUARDIANSPRIT));
		if ( pEffect != NULL )
		{
			//HP_t hpBonus = pEffect->getHPBonus();
                       HP_t hpBonus = 2225;
                       int     bonus= pEffect->getSTRBonus();
                       // int     DEXBonus= pEffect->getDEXBonus();
                       // int     INTBonus= pEffect->getINTBonus();
			
                        m_STR[ATTR_CURRENT] += bonus;
                        m_DEX[ATTR_CURRENT] += bonus;
                        m_INT[ATTR_CURRENT] += bonus;
                        m_STR[ATTR_MAX] += bonus;
                        m_DEX[ATTR_MAX] += bonus;
                        m_INT[ATTR_MAX] += bonus; 
			m_ToHit[ATTR_CURRENT]      += 35;
			m_Damage[ATTR_CURRENT]     += 5;
			m_Damage[ATTR_MAX]         += 8;
			m_Defense[ATTR_CURRENT]	   += 15;
                        m_Protection[ATTR_CURRENT] += 30;
			m_HP[ATTR_MAX]             += 225;
                        
		     //   computeStatOffset();
                       m_HP[ATTR_MAX] 			   = min((HP_t)OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + hpBonus));
		 //      m_HP[ATTR_CURRENT] += hpBonus ;
		}
	}
	//if ( isFlag( Effect::EFFECT_CLASS_RADCHIA ) )
	//{
	//	EffectRADCHIA* pEffect = dynamic_cast<EffectRADCHIA*>(findEffect(Effect::EFFECT_CLASS_RADCHIA));
//
	//	if ( pEffect != NULL )
	//	{
	//		//if ( attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
                         //  {

			//	m_MagicBonusDamage += 70;
                         // }
			//else
			//	pEffect->setDeadline(0);
	//	}
	//}
	if ( isFlag( Effect::EFFECT_CLASS_EVADE ) )
	{
		EffectEvade* pEffect = dynamic_cast<EffectEvade*>(findEffect(Effect::EFFECT_CLASS_EVADE));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
//				m_Defense[ATTR_CURRENT] += pEffect->getBonus();
				m_Defense[ATTR_CURRENT] = min( OUSTERS_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + (Defense_t)pEffect->getBonus() );
			else
				pEffect->setDeadline(0);
		}
	}

	if ( isFlag( Effect::EFFECT_CLASS_CROSS_GUARD ) )
	{
		EffectCrossGuard* pEffect = dynamic_cast<EffectCrossGuard*>(findEffect(Effect::EFFECT_CLASS_CROSS_GUARD));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
//				m_Protection[ATTR_CURRENT] += pEffect->getBonus();
				m_Protection[ATTR_CURRENT] = min( OUSTERS_MAX_PROTECTION, m_Protection[ATTR_CURRENT] + (Protection_t)pEffect->getBonus() );
			else
				pEffect->setDeadline(0);
		}
	}
	if ( isFlag( Effect:: EFFECT_CLASS_BREATHOFDRYAD) )
	{
		EffectBREATHOFDRYAD* pEffect = dynamic_cast<EffectBREATHOFDRYAD*>(findEffect(Effect::EFFECT_CLASS_BREATHOFDRYAD));
		if ( pEffect != NULL )
		{
			m_HP[ATTR_MAX] = min( (HP_t)OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + 200 ));
                  //      m_HP[ATTR_MAX] +=200
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_BLUNTING))
	{
		EffectBlunting* pBlunting = dynamic_cast<EffectBlunting*>(findEffect(Effect::EFFECT_CLASS_BLUNTING));
		if ( pBlunting != NULL )
		{
			if ( attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
			{
				int DefensePenalty = pBlunting->getDefensePenalty();
				m_Defense[ATTR_CURRENT]    = max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
			}
			else
				pBlunting->setDeadline(0);
		}
	}

/*	if ( isFlag( Effect::EFFECT_CLASS_HANDS_OF_FIRE ) )
	{
		//cout << getName() << " ����������̾� �پ��" << endl;
		EffectHandsOfFire* pEffect = dynamic_cast<EffectHandsOfFire*>(findEffect(Effect::EFFECT_CLASS_HANDS_OF_FIRE));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
				pEffect->setDeadline(0);
			else
			{
				m_FireDamage += pEffect->getBonus();
				//cout << getName() << " FireDamage : " << m_FireDamage << endl;
			}
		}
	}*/

	if ( isFlag( Effect::EFFECT_CLASS_RING_OF_FLARE ) )
	{
		EffectRingOfFlare* pEffect = dynamic_cast<EffectRingOfFlare*>(findEffect(Effect::EFFECT_CLASS_RING_OF_FLARE));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
				pEffect->setDeadline(0);
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_RING_OF_FLARE2 ) )
	{
		EffectRingOfFlare2* pEffect = dynamic_cast<EffectRingOfFlare2*>(findEffect(Effect::EFFECT_CLASS_RING_OF_FLARE2));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
				pEffect->setDeadline(0);
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_ADV2CYCLINGWARROCK ) )
	{
		EffectADV2CYCLINGWARROCK* pEffect = dynamic_cast<EffectADV2CYCLINGWARROCK*>(findEffect(Effect::EFFECT_CLASS_ADV2CYCLINGWARROCK));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
				pEffect->setDeadline(0);
		}
	}

	if ( isFlag( Effect::EFFECT_CLASS_WATER_BARRIER ) )
	{
		EffectWaterBarrier* pEffect = dynamic_cast<EffectWaterBarrier*>(findEffect(Effect::EFFECT_CLASS_WATER_BARRIER));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
				pEffect->setDeadline(0);
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_WATER_BARRIER2 ) )
	{
		EffectWaterBarrier2* pEffect = dynamic_cast<EffectWaterBarrier2*>(findEffect(Effect::EFFECT_CLASS_WATER_BARRIER2));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
				pEffect->setDeadline(0);
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER1 ) )
	{
		EffectSquallyBarrier1* pEffect = dynamic_cast<EffectSquallyBarrier1*>(findEffect(Effect::EFFECT_CLASS_SQUALLY_BARRIER1));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_CHAKRAM )
			{
				pEffect->unaffect();
			}
		}
	}
	
	if ( isFlag( Effect::EFFECT_CLASS_SQUALLY_BARRIER2 ) )
	{
		EffectSquallyBarrier2* pEffect = dynamic_cast<EffectSquallyBarrier2*>(findEffect(Effect::EFFECT_CLASS_SQUALLY_BARRIER2));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_CHAKRAM )
			{
				pEffect->unaffect();
			}
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_EVGENEIABELLUM ) )
	{
		EffectEVGENEIABELLUM* pEffect = dynamic_cast<EffectEVGENEIABELLUM*>(findEffect(Effect::EFFECT_CLASS_EVGENEIABELLUM));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_CHAKRAM )
			{
				pEffect->unaffect();
			}
		}
	}
/*	if ( isFlag( Effect::EFFECT_CLASS_GNOMES_WHISPER ) )
	{
		EffectGnomesWhisper* pEffect = dynamic_cast<EffectGnomesWhisper*>(findEffect(Effect::EFFECT_CLASS_GNOMES_WHISPER));

		if ( pEffect != NULL )
		{
			if ( attr.pWeapon == NULL || attr.pWeapon->getItemClass() != Item::ITEM_CLASS_OUSTERS_WRISTLET )
				pEffect->setDeadline(0);
		}
	}*/

	if (isFlag(Effect::EFFECT_CLASS_SUMMON_SYLPH))
	{
		int ProtectionBonus = max( 5,getLevel() / 10 );
		int ResistBonus = max( 5,getLevel() / 15 );

		m_Protection[ATTR_CURRENT] += ProtectionBonus;

		m_Resist[MAGIC_DOMAIN_NO_DOMAIN] += ResistBonus;
		m_Resist[MAGIC_DOMAIN_POISON]    += ResistBonus;
		m_Resist[MAGIC_DOMAIN_ACID]      += ResistBonus;
		m_Resist[MAGIC_DOMAIN_CURSE]     += ResistBonus;
		m_Resist[MAGIC_DOMAIN_BLOOD]     += ResistBonus;
	}

	if ( isFlag( Effect::EFFECT_CLASS_REACTIVE_ARMOR ) )
	{
		EffectReactiveArmor* pEffect = dynamic_cast<EffectReactiveArmor*>(findEffect(Effect::EFFECT_CLASS_REACTIVE_ARMOR));

		if ( pEffect != NULL )
		{
			int bonus = pEffect->getBonus();
			m_Protection[ATTR_CURRENT] += bonus;
			m_Defense[ATTR_CURRENT] += bonus;
		}
	}
	if ( isFlag( Effect::EFFECT_CLASS_REACTIVE_ARMOR2 ) )
	{
		EffectReactiveArmor2* pEffect = dynamic_cast<EffectReactiveArmor2*>(findEffect(Effect::EFFECT_CLASS_REACTIVE_ARMOR2));

		if ( pEffect != NULL )
		{
			int bonus = pEffect->getBonus();
			m_Protection[ATTR_CURRENT] += bonus;
			m_Defense[ATTR_CURRENT] += bonus;
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_FAVOROFLUNA))//ousters
	{
		EffectFAVOROFLUNA* pFAVOROFLUNA = dynamic_cast<EffectFAVOROFLUNA*>(findEffect(Effect::EFFECT_CLASS_FAVOROFLUNA));

		if ( pFAVOROFLUNA != NULL )
		{


			m_HP[ATTR_MAX] += pFAVOROFLUNA->getHPBonus();
			m_MP[ATTR_MAX] += pFAVOROFLUNA->getHPBonus();

			m_Defense[ATTR_CURRENT] += pFAVOROFLUNA->getDefenseBonus();
			m_Protection[ATTR_CURRENT] += pFAVOROFLUNA->getDefenseBonus();

			m_Resist[MAGIC_DOMAIN_POISON] += pFAVOROFLUNA->getRESBonus();
			m_Resist[MAGIC_DOMAIN_ACID] += pFAVOROFLUNA->getRESBonus();
			m_Resist[MAGIC_DOMAIN_CURSE] += pFAVOROFLUNA->getRESBonus();
			m_Resist[MAGIC_DOMAIN_BLOOD] += pFAVOROFLUNA->getRESBonus();
		}
	}

	if ( isFlag( Effect::EFFECT_CLASS_PARTY_AURA ) )
	{
		EffectPartyAura* pPartyAura = dynamic_cast<EffectPartyAura*>(findEffect(Effect::EFFECT_CLASS_PARTY_AURA));

		if ( pPartyAura != NULL )
		{
			Level_t level = getLevel();
			size_t partySize = pPartyAura->getPartySize();

			if ( pPartyAura->isHP() )
			{
				HP_t amount = (HP_t)( ( 65 + ( level / 3 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_HP[ATTR_MAX] = min( (HP_t)OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + amount ));
			}
			if ( pPartyAura->isDefense() )
			{
				Defense_t amount = (Defense_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Defense[ATTR_CURRENT] = min( OUSTERS_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + amount );
			}
			if ( pPartyAura->isToHit() )
			{
				ToHit_t amount = (ToHit_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_ToHit[ATTR_CURRENT] = min( OUSTERS_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + amount );
			}
			if ( pPartyAura->isLuck() )
			{
				Luck_t amount = (Luck_t)( ( 28 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Luck += amount;
			}
			if ( pPartyAura->isDamage() )
			{
				Damage_t amount = (Damage_t)( ( 15 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_MagicBonusDamage += amount;
			}
			if ( pPartyAura->isResist() )
			{
				Resist_t amount = (Resist_t)( ( 15 + ( level / 10 ) ) / ( 1.0 + ( 0.2 * partySize ) ) );
				m_Resist[MAGIC_DOMAIN_POISON] += amount;
				m_Resist[MAGIC_DOMAIN_ACID] += amount;
				m_Resist[MAGIC_DOMAIN_CURSE] += amount;
				m_Resist[MAGIC_DOMAIN_BLOOD] += amount;
			}
		}
	}
	if ( isFlag(Effect::EFFECT_CLASS_BEHEMOTH_FORCE_SCROLL) )
	{
		m_Defense[ATTR_CURRENT] += 30;
	}

	if ( isFlag(Effect::EFFECT_CLASS_SAFE_FORCE_SCROLL) )
	{
		m_Protection[ATTR_CURRENT] += 30;
	}

	if ( isFlag(Effect::EFFECT_CLASS_FURIOUS_INCENSE) )
	{
		m_Damage[ATTR_CURRENT] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + 3 );
		m_Damage[ATTR_MAX] = min( SLAYER_MAX_DAMAGE, m_Damage[ATTR_MAX] + 3 );
	}

	//////////////////////////////////////////////////////////////////////////////
	// �нú� ����� ����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	OustersSkillSlot* pHideSight = getSkill( SKILL_HIDE_SIGHT );
	if ( pHideSight != NULL && attr.pWeapon != NULL && attr.pWeapon->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
	{
		SkillLevel_t level = pHideSight->getExpLevel();

		if ( level <= 15 )
		{
//			m_ToHit[ATTR_CURRENT] += (int)((m_DEX[ATTR_CURRENT] / 20.0) * ( 1.0 + (level / 15.0) ));
			m_ToHit[ATTR_CURRENT] += ( 15 + (level * 8 / 9) );
		}
		else
		{
//			m_ToHit[ATTR_CURRENT] += (int)((m_DEX[ATTR_CURRENT] / 20.0) * ( 1.5 + (level / 30.0) ));
			int ToHitBonus = ( 35 + (level * 4 / 9) );
			if ( level == 30 ) ToHitBonus = (int)(ToHitBonus * 1.1);

			m_ToHit[ATTR_CURRENT] += ToHitBonus;
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK))
	{
		EffectBloodsSymposionAttack* pEffect = dynamic_cast<EffectBloodsSymposionAttack*>(findEffect(Effect::EFFECT_CLASS_BLOODS_SYMPOSION_ATTACK));
		if( pEffect != NULL )
		{
			int DefensePenalty = pEffect->getDefensePenalty();
			int ProtectionPenalty = pEffect->getProtectionPenalty();
			int HPPenalty = pEffect->getHPPenalty();

			m_Defense[ATTR_CURRENT]		= max(0, m_Defense[ATTR_CURRENT] - DefensePenalty);
			m_Protection[ATTR_CURRENT]	= max(0, m_Protection[ATTR_CURRENT] - ProtectionPenalty);
			m_HP[ATTR_MAX]			= max((HP_t)1, (HP_t)(m_HP[ATTR_MAX] - HPPenalty));
// 			20080813 ���� �����ۿ� ���� ��Ƣ�� ���ʽ��� ���� ��� �� �κ� ������ �ǰ� Ȯ ���̴�  ������ �ȴ�. �� �κ��� �� �ڷ� ����.
//			if( m_HP[ATTR_CURRENT] > m_HP[ATTR_MAX] )
//			{
//				m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
//			}

		}
	}
/*
	// 2007 01 16 �̽��׸� ���� ����Ʈ ��� �߰�
	if (isFlag(Effect::EFFECT_CLASS_MYSTERY_POTION))
	{
		EffectMysteryPotion* pEffect = dynamic_cast<EffectMysteryPotion*>(findEffect(Effect::EFFECT_CLASS_MYSTERY_POTION));
		if( pEffect != NULL )
		{
			HP_t bonusHP = 50;
			Defense_t bonusDef = 10; 
			m_Defense[ATTR_CURRENT]  = min( OUSTERS_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + bonusDef );
			m_HP[ATTR_CURRENT]		 = min( OUSTERS_MAX_HP, m_HP[ATTR_CURRENT] + bonusHP );
			computeOptionStat( 185 );		// ��� + 3
		}
	}
*/

	///////////////////////////////////////////////////////////////////////////////
	// ��� ���ʽ��� ����Ѵ�.
	///////////////////////////////////////////////////////////////////////////////
	/*if ( hasRankBonus( RankBonus::RANK_BONUS_WOOD_SKIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_WOOD_SKIN );
		Assert( pRankBonus != NULL );

		int ProtectionBonus = pRankBonus->getPoint();

		m_Protection[ATTR_CURRENT]  = min(OUSTERS_MAX_PROTECTION, m_Protection[ATTR_CURRENT] + ProtectionBonus);
		m_Protection[ATTR_MAX]      = min(OUSTERS_MAX_PROTECTION, m_Protection[ATTR_MAX] + ProtectionBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_WIND_SENSE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_WIND_SENSE );
		Assert( pRankBonus != NULL );

		int DefenseBonus = pRankBonus->getPoint();

		m_Defense[ATTR_CURRENT]  = min(OUSTERS_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + DefenseBonus);
		m_Defense[ATTR_MAX]      = min(OUSTERS_MAX_DEFENSE, m_Defense[ATTR_MAX] + DefenseBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_HOMING_EYE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_HOMING_EYE );
		Assert( pRankBonus != NULL );

		int ToHitBonus = pRankBonus->getPoint();

		m_ToHit[ATTR_CURRENT]  = min(OUSTERS_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
		m_ToHit[ATTR_MAX]      = min(OUSTERS_MAX_TOHIT, m_ToHit[ATTR_MAX] + ToHitBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_LIFE_ENERGY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_LIFE_ENERGY );
		Assert( pRankBonus != NULL );

		int HPBonus = pRankBonus->getPoint();

		m_HP[ATTR_MAX]  = min(OUSTERS_MAX_HP, m_HP[ATTR_MAX] + HPBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_SOUL_ENERGY ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SOUL_ENERGY );
		Assert( pRankBonus != NULL );

		int MPBonus = pRankBonus->getPoint();

		m_MP[ATTR_MAX]  = min(OUSTERS_MAX_MP, m_MP[ATTR_MAX] + MPBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_STONE_MAUL ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_STONE_MAUL );
		Assert( pRankBonus != NULL );

		int DamageBonus = pRankBonus->getPoint();

		m_Damage[ATTR_CURRENT]  = min(OUSTERS_MAX_DAMAGE, m_Damage[ATTR_CURRENT] + DamageBonus);
		m_Damage[ATTR_MAX]      = min(OUSTERS_MAX_DAMAGE, m_Damage[ATTR_MAX] + DamageBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_SWIFT_ARM ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SWIFT_ARM );
		Assert( pRankBonus != NULL );

		int AttackSpeedBonus = pRankBonus->getPoint();

		increaseAttackSpeed(AttackSpeedBonus, ATTR_CURRENT);
		increaseAttackSpeed(AttackSpeedBonus, ATTR_MAX);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_FIRE_ENDOW ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_FIRE_ENDOW );
		Assert( pRankBonus != NULL );

		int FireDamageBonus = pRankBonus->getPoint();

		m_FireDamage += FireDamageBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_WATER_ENDOW ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_WATER_ENDOW );
		Assert( pRankBonus != NULL );

		int WaterDamageBonus = pRankBonus->getPoint();

		m_WaterDamage += WaterDamageBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_EARTH_ENDOW ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_EARTH_ENDOW );
		Assert( pRankBonus != NULL );

		int EarthDamageBonus = pRankBonus->getPoint();

		m_EarthDamage += EarthDamageBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_ANTI_ACID_SKIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_ANTI_ACID_SKIN );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_ACID] += ResistBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_ANTI_BLOODY_SKIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_ANTI_BLOODY_SKIN );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_BLOOD] += ResistBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_ANTI_CURSE_SKIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_ANTI_CURSE_SKIN );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_CURSE] += ResistBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_ANTI_POISON_SKIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_ANTI_POISON_SKIN );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_Resist[MAGIC_DOMAIN_POISON] += ResistBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_ANTI_SILVER_DAMAGE_SKIN ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_ANTI_SILVER_DAMAGE_SKIN );
		Assert( pRankBonus != NULL );

		int ResistBonus = pRankBonus->getPoint();

		m_SilverResist += ResistBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_BLESS_OF_NATURE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_BLESS_OF_NATURE );
		Assert( pRankBonus != NULL );

		int MPAmount = pRankBonus->getPoint();

		m_ConsumeMPRatio -= MPAmount;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_MYSTIC_RULE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_MYSTIC_RULE );
		Assert( pRankBonus != NULL );

		int LuckBonus = pRankBonus->getPoint();

		m_Luck += LuckBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_POWER_OF_SPIRIT ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_POWER_OF_SPIRIT );
		Assert( pRankBonus != NULL );

		int ProtectionBonus= getPercentValue( m_Protection[ATTR_CURRENT], pRankBonus->getPoint() );

		m_Protection[ATTR_CURRENT]  = min(OUSTERS_MAX_PROTECTION, m_Protection[ATTR_CURRENT] + ProtectionBonus);
		m_Protection[ATTR_MAX]      = min(OUSTERS_MAX_PROTECTION, m_Protection[ATTR_MAX] + ProtectionBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_WIND_OF_SPIRIT ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_WIND_OF_SPIRIT );
		Assert( pRankBonus != NULL );

		int DefenseBonus= getPercentValue( m_Defense[ATTR_CURRENT], pRankBonus->getPoint() );

		m_Defense[ATTR_CURRENT]  = min(OUSTERS_MAX_DEFENSE, m_Defense[ATTR_CURRENT] + DefenseBonus);
		m_Defense[ATTR_MAX]      = min(OUSTERS_MAX_DEFENSE, m_Defense[ATTR_MAX] + DefenseBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_PIXIES_EYES ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_PIXIES_EYES );
		Assert( pRankBonus != NULL );

		int ToHitBonus= getPercentValue( m_ToHit[ATTR_CURRENT], pRankBonus->getPoint() );

		m_ToHit[ATTR_CURRENT]  = min(OUSTERS_MAX_TOHIT, m_ToHit[ATTR_CURRENT] + ToHitBonus);
		m_ToHit[ATTR_MAX]      = min(OUSTERS_MAX_TOHIT, m_ToHit[ATTR_MAX] + ToHitBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_GROUND_OF_SPIRIT ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_GROUND_OF_SPIRIT );
		Assert( pRankBonus != NULL );

		int MPBonus= getPercentValue( m_MP[ATTR_MAX], pRankBonus->getPoint() );

		m_MP[ATTR_MAX]  = min(OUSTERS_MAX_MP, m_MP[ATTR_MAX] + MPBonus);
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_FIRE_OF_SPIRIT ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_FIRE_OF_SPIRIT );
		Assert( pRankBonus != NULL );

		int CriticalRatioBonus= getPercentValue( m_CriticalRatio[ATTR_CURRENT], pRankBonus->getPoint() );

		m_CriticalRatio[ATTR_CURRENT]  = m_CriticalRatio[ATTR_CURRENT] + CriticalRatioBonus;
		m_CriticalRatio[ATTR_MAX]      = m_CriticalRatio[ATTR_MAX] + CriticalRatioBonus;
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_SALAMANDERS_KNOWLEDGE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_SALAMANDERS_KNOWLEDGE );
		Assert( pRankBonus != NULL );

		m_ElementalFire += pRankBonus->getPoint();
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_UNDINES_KNOWLEDGE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_UNDINES_KNOWLEDGE );
		Assert( pRankBonus != NULL );

		m_ElementalWater += pRankBonus->getPoint();
	}

	if ( hasRankBonus( RankBonus::RANK_BONUS_GNOMES_KNOWLEDGE ) )
	{
		RankBonus* pRankBonus = getRankBonus( RankBonus::RANK_BONUS_GNOMES_KNOWLEDGE );
		Assert( pRankBonus != NULL );

		m_ElementalEarth += pRankBonus->getPoint();
	}*/

	// -_- %�� ���Ǵ� ��ų�� �������� ����Ų��.
	if ( isFlag( Effect::EFFECT_CLASS_SHARP_CHAKRAM ) )
	{
		EffectSharpChakram* pEffect = dynamic_cast<EffectSharpChakram*>(findEffect(Effect::EFFECT_CLASS_SHARP_CHAKRAM));

		if ( pEffect != NULL )
		{
			int bonus = pEffect->getBonus();
			m_ToHit[ATTR_CURRENT] += getPercentValue( m_ToHit[ATTR_CURRENT], bonus );
		}
	}

    // HP,MP�� ����ġ�� HP,MP�� �ְ�ġ�� �Ѵ� ���
    // ����ġ�� �ְ�ġ������ set
/*    if (m_HP[ATTR_CURRENT] > m_HP[ATTR_MAX])
    {
        m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
    }
	if (m_MP[ATTR_CURRENT] > m_MP[ATTR_MAX])
	{
		m_MP[ATTR_CURRENT] = m_MP[ATTR_MAX];
	}
*/
	// �нú� ��ų �ʱ�ȭ
	bool bCanUsePassive = false;
	if ( hasSkill( SKILL_FIRE_OF_SOUL_STONE ) != NULL )
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_FIRE_OF_SOUL_STONE );
		Assert( pSkillInfo != NULL );

		if ( satisfySkillRequire( pSkillInfo ) )
		{
			bCanUsePassive = true;

			m_PassiveSkillMap[SKILL_FIRE_OF_SOUL_STONE].first = true;
			m_PassiveSkillMap[SKILL_FIRE_OF_SOUL_STONE].second = min( 60, (int)(( getSTR() / 7.0 ) + ( getDEX() / 5.0 )) );
		}
	}

	if ( !bCanUsePassive )
	{
		m_PassiveSkillMap[SKILL_FIRE_OF_SOUL_STONE].first = false;
		m_PassiveSkillMap[SKILL_FIRE_OF_SOUL_STONE].second = 0;
	}

	bCanUsePassive = false;
	if ( hasSkill( SKILL_ICE_OF_SOUL_STONE ) != NULL )
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_ICE_OF_SOUL_STONE );
		Assert( pSkillInfo != NULL );

		if ( satisfySkillRequire( pSkillInfo ) )
		{
			bCanUsePassive = true;

			m_PassiveSkillMap[SKILL_ICE_OF_SOUL_STONE].first = true;
			m_PassiveSkillMap[SKILL_ICE_OF_SOUL_STONE].second = min(5,1 + getDEX() / 20) * 10;
		}
	}

	if ( !bCanUsePassive )
	{
		m_PassiveSkillMap[SKILL_ICE_OF_SOUL_STONE].first = false;
		m_PassiveSkillMap[SKILL_ICE_OF_SOUL_STONE].second = 0;
	}

	bCanUsePassive = false;
	if ( hasSkill( SKILL_SAND_OF_SOUL_STONE ) != NULL )
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_SAND_OF_SOUL_STONE );
		Assert( pSkillInfo != NULL );

		if ( satisfySkillRequire( pSkillInfo ) )
		{
			bCanUsePassive = true;

			m_PassiveSkillMap[SKILL_SAND_OF_SOUL_STONE].first = true;
			m_PassiveSkillMap[SKILL_SAND_OF_SOUL_STONE].second = (uint)(( getSTR() / 15.0 ) + ( getDEX() / 5.0 ));
		}
	}

	if ( !bCanUsePassive )
	{
		m_PassiveSkillMap[SKILL_SAND_OF_SOUL_STONE].first = false;
		m_PassiveSkillMap[SKILL_SAND_OF_SOUL_STONE].second = 0;
	}

	bCanUsePassive = false;
	if ( hasSkill( SKILL_BLOCK_HEAD ) != NULL )
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_BLOCK_HEAD );
		Assert( pSkillInfo != NULL );

		if ( satisfySkillRequire( pSkillInfo ) )
		{
			bCanUsePassive = true;

			m_PassiveSkillMap[SKILL_BLOCK_HEAD].first = true;
			m_PassiveSkillMap[SKILL_BLOCK_HEAD].second = min(4,1 + getDEX() / 30) * 10;
		}
	}

	if ( !bCanUsePassive )
	{
		m_PassiveSkillMap[SKILL_BLOCK_HEAD].first = false;
		m_PassiveSkillMap[SKILL_BLOCK_HEAD].second = 0;
	}

	bCanUsePassive = false;
	if ( hasSkill( SKILL_BLESS_FIRE ) != NULL )
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_BLESS_FIRE );
		Assert( pSkillInfo != NULL );

		if ( satisfySkillRequire( pSkillInfo ) )
		{
			bCanUsePassive = true;

			m_PassiveSkillMap[SKILL_BLESS_FIRE].first = true;
			m_PassiveSkillMap[SKILL_BLESS_FIRE].second = min( (uint)150, (uint)(( getSTR() / 4.0 ) + ( getDEX() / 5.0 )) );
		}
	}

	if ( !bCanUsePassive )
	{
		m_PassiveSkillMap[SKILL_BLESS_FIRE].first = false;
		m_PassiveSkillMap[SKILL_BLESS_FIRE].second = 0;
	}

	bCanUsePassive = false;
	if ( hasSkill( SKILL_WATER_SHIELD ) != NULL )
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_WATER_SHIELD );
		Assert( pSkillInfo != NULL );

		if ( satisfySkillRequire( pSkillInfo ) )
		{
			bCanUsePassive = true;

			m_PassiveSkillMap[SKILL_WATER_SHIELD].first = true;
			m_PassiveSkillMap[SKILL_WATER_SHIELD].second = 0;
		}
	}

	if ( !bCanUsePassive )
	{
		m_PassiveSkillMap[SKILL_WATER_SHIELD].first = false;
		m_PassiveSkillMap[SKILL_WATER_SHIELD].second = 0;
	}

	bCanUsePassive = false;
	if ( hasSkill( SKILL_SAND_CROSS ) != NULL )
	{
		SkillInfo* pSkillInfo = g_pSkillInfoManager->getSkillInfo( SKILL_SAND_CROSS );
		Assert( pSkillInfo != NULL );

		if ( satisfySkillRequire( pSkillInfo ) )
		{
			bCanUsePassive = true;

			m_PassiveSkillMap[SKILL_SAND_CROSS].first = true;
			//m_PassiveSkillMap[SKILL_SAND_CROSS].second = (uint)(( getSTR() / 10.0 ) + ( getDEX() / 10.0 ));
			m_PassiveSkillMap[SKILL_SAND_CROSS].second = min( (uint)50, (uint)(( getSTR() / 8.0 ) + ( getDEX() / 12.0 )) );	// ��� ���� by bezz 2005.06.15
		}
	}

	if ( !bCanUsePassive )
	{
		m_PassiveSkillMap[SKILL_SAND_CROSS].first = false;
		m_PassiveSkillMap[SKILL_SAND_CROSS].second = 0;
	}
	
	m_PassiveRatio = getElementalSum();

	for ( int i = WEAR_STONE1; i <= WEAR_STONE3; ++i )
	{
		Item* pItem = getWearItem((WearPart)i);

		if ( pItem == NULL || pItem->getItemClass() != Item::ITEM_CLASS_OUSTERS_STONE ) continue;
		OustersStone* pOustersStone = dynamic_cast<OustersStone*>(pItem);
		Assert( pOustersStone != NULL );

		m_PassiveRatio += pOustersStone->getElemental();
	}

	if ( HPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + HPBonus) );
	}

	if ( RaceWarHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min((HP_t) OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + RaceWarHPBonus) );
	}

	if ( DragonEyeHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min((HP_t) OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + DragonEyeHPBonus ));
		EffectDragonEye* pEffectDragonEye= dynamic_cast<EffectDragonEye*>(findEffect(Effect::EFFECT_CLASS_DRAGON_EYE));
		if (pEffectDragonEye != NULL)
		{
			if( pEffectDragonEye->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_CURRENT] + DragonEyeHPBonus ));
				pEffectDragonEye->setHPBonus(false);
			}
		}
	}

#ifdef __CONTRIBUTION_SYSTEM__
	if ( HeroItemHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min( (HP_t)OUSTERS_MAX_HP, (HP_t)(m_HP[ATTR_MAX] + HeroItemHPBonus ));
		EffectOustersHeroItem* pEffectHeroItem = dynamic_cast<EffectOustersHeroItem*>(findEffect(Effect::EFFECT_CLASS_OUSTERS_HERO_ITEM));
		if (pEffectHeroItem != NULL)
		{
			if( pEffectHeroItem->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)OUSTERS_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + HeroItemHPBonus ));
				pEffectHeroItem->setHPBonus(false);
			}
		}
	}
	if ( LevelHeroItemHPBonus > 0 )
	{
		m_HP[ATTR_MAX] = min((HP_t) OUSTERS_MAX_HP,(HP_t)( m_HP[ATTR_MAX] + LevelHeroItemHPBonus ));
		EffectOustersLevelWarHeroItem* pEffectHeroItem = dynamic_cast<EffectOustersLevelWarHeroItem*>(findEffect(Effect::EFFECT_CLASS_OUSTERS_LEVELWAR_HERO_ITEM));
		if (pEffectHeroItem != NULL)
		{
			if( pEffectHeroItem->isHPBonus() )
			{
				m_HP[ATTR_CURRENT] = min((HP_t)OUSTERS_MAX_HP, (HP_t)(m_HP[ATTR_CURRENT] + LevelHeroItemHPBonus ));
				pEffectHeroItem->setHPBonus(false);
			}
		}
	}
#endif

		// 20080416 SKILL_INCREASE_OF_HEALTH — MaxHP bonus (Phoenix 2026-09-07: yorum kaldirildi)
		if( getSkill(SKILL_INCREASE_OF_HEALTH_15) )
		{
			m_HP[ATTR_MAX] += 5400;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_14) )
		{
			m_HP[ATTR_MAX] += 4800;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_13) )
		{
			m_HP[ATTR_MAX] += 4200;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_12) )
		{
			m_HP[ATTR_MAX] += 3600;
		} 
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_11) )
		{
			m_HP[ATTR_MAX] += 3000;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_10) )
		{
			m_HP[ATTR_MAX] += 2400;
		}
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_9) )
		{
			m_HP[ATTR_MAX] += 1800;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_8) )
		{
			m_HP[ATTR_MAX] += 1600;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_7) )
		{
			m_HP[ATTR_MAX] += 1400;
		} 
		else if( getSkill(SKILL_INCREASE_OF_HEALTH_6) )
		{
			m_HP[ATTR_MAX] += 1200;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_5) )
		{
			m_HP[ATTR_MAX] += 1000;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_4) )
		{
			m_HP[ATTR_MAX] += 800;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_3) )
		{
			m_HP[ATTR_MAX] += 600;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_2) )
		{
			m_HP[ATTR_MAX] += 400;
		}
		else if(getSkill(SKILL_INCREASE_OF_HEALTH_1) )
		{
			m_HP[ATTR_MAX] += 200;
		}
             if( getSkill(SKILL_hpup_1500) )
		{
			 m_HP[ATTR_MAX] += 7000;
		} 
             if( getSkill(SKILL_ADV2_UNKNOW2) )
		{
			 m_HP[ATTR_MAX] += 1000;
                         m_PhysicBonusDamage += 30;

		} 
             if( getSkill(SKILL_ADV2_UNKNOW3) )
		{
			 m_HP[ATTR_MAX] += 1000;
                         m_PhysicBonusDamage += 30;

		}
             if( getSkill(SKILL_ADV2_UNKNOW4) )
		{
		m_ToHit[ATTR_CURRENT] += 50;
                m_CriticalRatio[ATTR_MAX] += 50;
		}
             if( getSkill(SKILL_ADV2_UNKNOW5) )
		{
			m_MagicDamageReduce += 30;
                        m_MagicBonusDamage += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW6) )
		{
			    m_MagicBonusDamage += 30;
                            m_PhysicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW7) )
		{
			m_PhysicBonusDamage += 30;
                        m_ToHit[ATTR_CURRENT] += 100;
		}
             if( getSkill(SKILL_ADV2_UNKNOW8) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW9) )
		{
			m_PhysicBonusDamage += 30;
                        m_Defense[ATTR_CURRENT] += 200;
		}
             if( getSkill(SKILL_ADV2_UNKNOW10) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW11) )
		{
		            m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
             if( getSkill(SKILL_ADV2_UNKNOW12) )
		{
			    m_MagicBonusDamage += 30;
                            m_MagicDamageReduce += 30;
		}
        if( getSkill(SKILL_RALLSER_44) )
        {
	m_Defense[ATTR_CURRENT] += 100;
	} 
        if( getSkill(SKILL_RALLSER_5) )
	{
	   m_ToHit[ATTR_CURRENT] += 100;
	} 
                if( getSkill(SKILL_RALLSER_6) )
		{
			m_Damage[ATTR_MAX] += 40;
		} 
                if( getSkill(SKILL_RALLSER_7) )
		{
			m_MagicBonusDamage += 40;
		} 
                if( getSkill(SKILL_RALLSER_8) )
		{
			m_HP[ATTR_MAX] += 800;
		} 
	// Hoodlum Stigma
	if ( isFlag( Effect::EFFECT_CLASS_HOODLUM_STIGMA ) )
	{
		m_HP[ATTR_MAX] = getPercentValue( m_HP[ATTR_MAX], 70 );
	}

	initCastleSkill();

	for (int i=0; i<MAGIC_DOMAIN_MAX; i++)
	{
		if ( m_Resist[i] < 0 ) m_Resist[i] = 0;
	//	cout << i << " : " << m_Resist[i] << endl;
	}

	// �������� ����
	if ( m_HP[ATTR_MAX] < m_HP[ATTR_CURRENT] + m_SilverDamage )
	{
		if( m_HP[ATTR_MAX] > m_HP[ATTR_CURRENT] )
		{
			setSilverDamage( m_HP[ATTR_MAX] - m_HP[ATTR_CURRENT] );
//			HP_t currentHP = m_HP[ATTR_MAX]/10;
//			setSilverDamage( currentHP*9 );
//			setHP( currentHP );
		}
	}

	//20080813 ������ �������� �ִ� HP�� ���� ���� �� ���.
	if(m_SilverDamage >= (m_HP[ATTR_MAX]/2))
	{
		m_SilverDamage = m_HP[ATTR_MAX]/2;
	}
	
	// 20080813 ���� �����ۿ� ���� ��Ƣ�� ���ʽ��� ���� ��� ����� �� ���������� �� �ڵ带 �ִ´�.
	if( m_HP[ATTR_CURRENT] > m_HP[ATTR_MAX] )
	{
		m_HP[ATTR_CURRENT] = m_HP[ATTR_MAX];
	}

//	cout << "�� : " << m_ElementalFire << endl;
//	cout << "�� : " << m_ElementalWater << endl;
//	cout << "���� : " << m_ElementalEarth << endl;

//	cout << getName() << "�� Luck : " << m_Luck << endl;

/*	cout << getName() << ":" << endl;
	for ( int i=0; i<MAGIC_DOMAIN_MAX; ++i )
	{
		cout << "���� " << i << " : " << m_Resist[i] << endl;
	}

	cout << "������ݷ� " << m_PhysicBonusDamage << endl;
	cout << "�������� " << m_PhysicDamageReduce << endl;
	cout << "�����ݷ� " << m_MagicBonusDamage << endl;
	cout << "������� " << m_MagicDamageReduce << endl;*/

	ComputeAllSkillDelay();

	if (isFlag(Effect::EFFECT_CLASS_INTIMATE_GRAIL))
	{
		EffectIntimateGrail* pIntimateGrail = dynamic_cast<EffectIntimateGrail*>(findEffect(Effect::EFFECT_CLASS_INTIMATE_GRAIL));
		if ( pIntimateGrail!= NULL )
		{
			int ratio =  10 + ( pIntimateGrail->getSkillLevel()/10);
			m_Defense[ATTR_CURRENT]	-= getPercentValue(m_Defense[ATTR_CURRENT] , ratio );
			//m_HP[ATTR_CURRENT]		-= getPercentValue(m_HP[ATTR_CURRENT] , ratio );
			m_HP[ATTR_MAX]		-= getPercentValue(m_HP[ATTR_MAX] , ratio );
			m_HP[ATTR_CURRENT] = min(m_HP[ATTR_CURRENT],m_HP[ATTR_MAX]);
		}
	}
	if (isFlag(Effect::EFFECT_CLASS_INTIMATE_GRAIL_2))
	{
		EffectIntimateGrail2* pIntimateGrail2 = dynamic_cast<EffectIntimateGrail2*>(findEffect(Effect::EFFECT_CLASS_INTIMATE_GRAIL_2));
		if ( pIntimateGrail2!= NULL )
		{
			int ratio =  20;
			m_Defense[ATTR_CURRENT]	-= getPercentValue(m_Defense[ATTR_CURRENT] , ratio );
		//	m_HP[ATTR_CURRENT]		-= getPercentValue(m_HP[ATTR_CURRENT] , ratio );
			m_HP[ATTR_MAX]		-= getPercentValue(m_HP[ATTR_MAX] , ratio );
			m_HP[ATTR_CURRENT] = min(m_HP[ATTR_CURRENT],m_HP[ATTR_MAX]);
		}
	}
	
	__END_CATCH
}

int	Ousters::getBloodBibleSignOpenNum() const
{
	GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayer);

	int openNumLimit = g_pVariableManager->getVariable(PREMIUM_BLOOD_BIBLE_NUMBER);
//	if (!pGamePlayer->isPayPlaying() 
//		&& !pGamePlayer->isPremiumPlay())
	if (!pGamePlayer->isPremiumPlay()) // 20080227 ������ Ƽ�� �߰��� ����
	{
		openNumLimit = g_pVariableManager->getVariable(FREE_BLOOD_BIBLE_NUMBER);
	}

#ifdef __TEST_SERVER__
	Fame_t fame = getFame()*10;
#else
	Fame_t fame = getFame();
#endif

	uint openNum = 1;

	if ( g_pWarSystem->canApplyBloodBibleSign() )
	{
		if ( fame < 30000 )
		{
			openNum = min( openNumLimit, 1 );
		}
		else if ( fame < 500000 )
		{
			openNum = min( openNumLimit, 2 );
		}
		else if ( fame < 3000000 )
		{
			openNum = min( openNumLimit, 3 );
		}
		else if ( fame < 7000000 )
		{
			openNum = min( openNumLimit, 4 );
		}
		else if ( fame < 50000000 )
		{
			openNum = min( openNumLimit, 5 );
		}
		else
		{
			openNum = min( openNumLimit, 6 );
		}
	}
	else
	{
		openNum = 0;
	}

	return openNum;
}

//////////////////////////////////////////////////////////////////////////////
// STR, DEX, INT�� ���
// CURRENT = �⺻ ��ġ + ������ ��ġ + ���� ��ġ
// MAX     = �⺻ ��ġ + ������ ��ġ
// BASIC   = �⺻ ��ġ
//
// HP, MP�� ���
// CURRENT = ���� ��ġ
// MAX     = ���� �ƽ�
// BASIC   = �����ۿ� ���� ��ȭ ��ġ
//
// Defense, Protection, ToHit�� ���
// CURRENT = ���� ��ġ
// MAX     = �����ۿ� ���� ��ȭ ��ġ
//
// Damage�� ���
// CURRENT = Min ������
// MAX     = Max ������
// BASIC   = �����ۿ� ���� ��ȭ ��ġ
//////////////////////////////////////////////////////////////////////////////
void Ousters::computeStatOffset()
	throw()
{
	__BEGIN_TRY

	Creature::CreatureClass CClass = getCreatureClass();
	BASIC_ATTR              cur_attr;

	cur_attr.nSTR   = m_STR[ATTR_CURRENT];
	cur_attr.nDEX   = m_DEX[ATTR_CURRENT];
	cur_attr.nINT   = m_INT[ATTR_CURRENT];
	cur_attr.nLevel = m_Level;
	cur_attr.pWeapon = getWearItem(WEAR_RIGHTHAND);

	// ���ο��� STR, DEX, INT�� ���� ����� �� ����
	// ������ �Ǵ� ���� ��ġ�� ���Ѵ�. 
	//m_HP[ATTR_MAX] = computeHP(CClass, &cur_attr);
	m_HP[ATTR_MAX] = computeTestModeHP(this, &cur_attr);
	m_HP[ATTR_MAX] += m_HP[ATTR_BASIC];

	m_MP[ATTR_MAX] = computeMP(CClass, &cur_attr);
	m_MP[ATTR_MAX] += m_MP[ATTR_BASIC];

	m_ToHit[ATTR_CURRENT] = computeToHit(CClass, &cur_attr);
	m_ToHit[ATTR_CURRENT] += m_ToHit[ATTR_MAX];

	m_Defense[ATTR_CURRENT] = computeDefense(CClass, &cur_attr);
	m_Defense[ATTR_CURRENT] += m_Defense[ATTR_MAX];

	m_Protection[ATTR_CURRENT] = computeProtection(CClass, &cur_attr);
	m_Protection[ATTR_CURRENT] += m_Protection[ATTR_MAX];

	m_Damage[ATTR_CURRENT] = computeMinDamage(CClass, &cur_attr);
	m_Damage[ATTR_MAX]     = computeMaxDamage(CClass, &cur_attr);

	m_Damage[ATTR_CURRENT] += m_Damage[ATTR_BASIC];
	m_Damage[ATTR_MAX] += m_Damage[ATTR_BASIC];

	setAttackSpeed(computeAttackSpeed(CClass, &cur_attr), ATTR_CURRENT);
	increaseAttackSpeed(getAttackSpeed(ATTR_MAX), ATTR_CURRENT);

	m_CriticalRatio[ATTR_CURRENT] = computeCriticalRatio(CClass, &cur_attr);
	m_CriticalRatio[ATTR_CURRENT] += m_CriticalRatio[ATTR_MAX];

	// 20070918
	m_MagicDefense			= computeMagicDefense(CClass, &cur_attr);
	m_MagicToHit  			= computeMagicToHit(CClass, &cur_attr);
		
	__END_CATCH
}

void Ousters::computeItemStat(Item* pItem)
	throw()
{
	__BEGIN_TRY

//	if (isOustersWeapon(pItem->getItemClass()))
	if ( pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_CHAKRAM )
	{
		// ������ ���Ⱑ ������ �ӵ� �Ķ���͸� ���Ѵ�.
		ItemInfo* pItemInfo = g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType());
		increaseAttackSpeed(pItemInfo->getSpeed(), ATTR_CURRENT);
		increaseAttackSpeed(pItemInfo->getSpeed(), ATTR_MAX);
	}
	
	if ( pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP )
	{
		CoreZapInfo* pItemInfo = dynamic_cast<CoreZapInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType()));
		if ( pItemInfo != NULL )
		{
			computeOptionClassStat( pItemInfo->getOptionClass(), pItem->getGrade() );
		}
	}

	if ( pItem->getItemClass() == Item::ITEM_CLASS_CUE_OF_ADAM )
	{
		CueOfAdamInfo* pItemInfo = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), pItem->getItemType()));

		if ( pItemInfo != NULL )
		{
			computeOptionClassStat( pItemInfo->getOptionClass() , pItem->getGrade() );
		}
	}

	Elemental_t point = 0;
	ElementalType type = ELEMENTAL_MAX;

	if ( pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_WRISTLET )
	{
		OustersWristlet* pWristlet = dynamic_cast<OustersWristlet*>(pItem);
		Assert( pWristlet != NULL );

		point = pWristlet->getElemental();
		type = pWristlet->getElementalType();
	}
	else if ( pItem->getItemClass() == Item::ITEM_CLASS_OUSTERS_STONE )
	{
		OustersStone* pStone = dynamic_cast<OustersStone*>(pItem);
		Assert( pStone != NULL );

		point = pStone->getElemental();
		type = pStone->getElementalType();
	}

	if ( point != 0 && type != ELEMENTAL_MAX )
	{
		switch ( type )
		{
			case ELEMENTAL_FIRE:
				m_ElementalFire += point;
				break;
			case ELEMENTAL_WATER:
				m_ElementalWater += point;
				break;
			case ELEMENTAL_EARTH:
				m_ElementalEarth += point;
				break;
			case ELEMENTAL_WIND:
				m_ElementalWind += point;
				break;
			default:
				break;
		}
	}

	m_Protection[ATTR_CURRENT] += pItem->getProtectionBonus();
	m_Protection[ATTR_MAX]     += pItem->getProtectionBonus();

	m_Defense[ATTR_CURRENT] += pItem->getDefenseBonus();
	m_Defense[ATTR_MAX]     += pItem->getDefenseBonus();

	m_ToHit[ATTR_CURRENT] += pItem->getToHitBonus();
	m_ToHit[ATTR_MAX] += pItem->getToHitBonus();
	
	m_Luck	+= pItem->getLuck();

	m_MagicDamageReduce		+= pItem->getMagicProtectionBonus(); // 20080703 

//	if (pItem->getOptionType()) computeOptionStat(pItem);
	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& optionType = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;
	for (itr=optionType.begin(); itr!=optionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}
#ifdef VERSION_THIRD_ENCHANT_1
	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& thirdoptionType = pItem->getThirdOptionTypeList();
	for (itr=thirdoptionType.begin(); itr!=thirdoptionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}
#endif

	// Item ��ü�� defaultOption�� ����Ų��.
	const list<OptionType_t>& defaultOptions = pItem->getDefaultOptions();
	list<OptionType_t>::const_iterator iOptions;

	for (iOptions=defaultOptions.begin(); iOptions!=defaultOptions.end(); iOptions++)
	{
		computeOptionStat( *iOptions );
	}

	// �ھ����� ��� �� ���� ������ �ɼ� ���ʽ��� �ִ�.
	if ( pItem->getItemClass() == Item::ITEM_CLASS_CORE_ZAP )
	{
		bool zaps[4] = { false, false, false, false };
		for ( int i=WEAR_ZAP1; i<=WEAR_ZAP4; ++i )
		{
			Item* pCoreZap = m_pWearItem[i];
			if ( pCoreZap != NULL && m_pRealWearingCheck[i] == true && pCoreZap->getItemClass() == Item::ITEM_CLASS_CORE_ZAP && pCoreZap->getItemType()<4 )
			{
				zaps[pCoreZap->getItemType()] = true;
			}
		}
		if ( zaps[0] && zaps[1] && zaps[2] && zaps[3] )
		{
			computeOptionStat(182); //���� 9
			computeOptionStat(185); //��� 3
		}
	}

	// ť���� �ƴ��� ��� �� ���� �ְ� ���� �ٸ� ������ �ɼ� ���ʽ��� �ִ�.
	if ( pItem->getItemClass() == Item::ITEM_CLASS_CUE_OF_ADAM )
	{
		ItemType_t cues[2] = { 9999, 9999 };
		for ( int i = WEAR_CUE1; i <= WEAR_CUE2; ++i )
		{
			Item* pCueOfAdam = m_pWearItem[i];
			if ( pCueOfAdam != NULL && m_pRealWearingCheck[i] == true )
			{
				cues[i-WEAR_CUE1] = pCueOfAdam->getItemType();
			}
		}

		if ( cues[0] != 9999 && cues[1] != 9999 && cues[0] != cues[1] && cues[0] != 4 && cues[1] != 4) // 20070906 �߼��̺�Ʈ�� ����� �ŵ��� ��Ʈ ȿ�� ��� �ȵȴ�. 
//		if ( cues[0] != 9999 && cues[1] != 9999 && cues[0] != cues[1] )
		{
			CueOfAdamInfo* pItemInfo1 = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), cues[0]));
			CueOfAdamInfo* pItemInfo2 = dynamic_cast<CueOfAdamInfo*>(g_pItemInfoManager->getItemInfo(pItem->getItemClass(), cues[1]));

			if ( pItemInfo1 != NULL )
				computeOptionStat(pItemInfo1->getSetOptionType());
			if ( pItemInfo2 != NULL )
				computeOptionStat(pItemInfo2->getSetOptionType());
		}
	}

	__END_CATCH
}

void Ousters::computeOptionStat(Item* pItem)
	throw()
{
	__BEGIN_TRY

	// �ΰ����� �ɼǵ�
	const list<OptionType_t>& optionType = pItem->getOptionTypeList();
	list<OptionType_t>::const_iterator itr;
	for (itr=optionType.begin(); itr!=optionType.end(); itr++)
	{
		computeOptionStat( *itr );
	}

	// Item ��ü�� defaultOption�� ����Ų��.
	const list<OptionType_t>& defaultOptions = pItem->getDefaultOptions();
	list<OptionType_t>::const_iterator iOptions;

	for (iOptions=defaultOptions.begin(); iOptions!=defaultOptions.end(); iOptions++)
	{
		computeOptionStat( *iOptions );
	}

	__END_CATCH
}

void Ousters::computeOptionClassStat(OptionClass OClass, int PlusPoint)
{
	switch (OClass)
	{
		case OPTION_STR:
			m_STR[ATTR_CURRENT] += PlusPoint;
			m_STR[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_DEX:
			m_DEX[ATTR_CURRENT] += PlusPoint;
			m_DEX[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_INT:
			m_INT[ATTR_CURRENT] += PlusPoint;
			m_INT[ATTR_MAX]     += PlusPoint;
			computeStatOffset();
			break;
		case OPTION_HP:
			m_HP[ATTR_MAX]   += PlusPoint;
			m_HP[ATTR_BASIC] += PlusPoint;
			break;
		case OPTION_MP:
			m_MP[ATTR_MAX]   += PlusPoint;
			m_MP[ATTR_BASIC] += PlusPoint;
			break;
		case OPTION_HP_STEAL:
			m_HPStealAmount += PlusPoint;
			break;
		case OPTION_MP_STEAL:
			m_MPStealAmount += PlusPoint;
			break;
		case OPTION_HP_REGEN:
			m_HPRegen += PlusPoint;
			break;
		case OPTION_MP_REGEN:
			m_MPRegen += PlusPoint;
			break;
		case OPTION_TOHIT:
			m_ToHit[ATTR_CURRENT] += PlusPoint;
			m_ToHit[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_DEFENSE:
			m_Defense[ATTR_CURRENT] += PlusPoint;
			m_Defense[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_DAMAGE:
			m_Damage[ATTR_CURRENT] += PlusPoint;
			m_Damage[ATTR_MAX]     += PlusPoint;
			m_Damage[ATTR_BASIC]   += PlusPoint;
			break;
		case OPTION_PROTECTION:
			m_Protection[ATTR_CURRENT] += PlusPoint;
			m_Protection[ATTR_MAX]     += PlusPoint;
			break;
		case OPTION_POISON:
			m_Resist[MAGIC_DOMAIN_POISON] += PlusPoint;
			break;
		case OPTION_ACID:
			m_Resist[MAGIC_DOMAIN_ACID] += PlusPoint;
			break;
		case OPTION_CURSE:
			m_Resist[MAGIC_DOMAIN_CURSE] += PlusPoint;
			break;
		case OPTION_BLOOD:
			m_Resist[MAGIC_DOMAIN_BLOOD] += PlusPoint;
			break;
		case OPTION_VISION:
			break;
		case OPTION_ATTACK_SPEED:
			increaseAttackSpeed(PlusPoint, ATTR_CURRENT);
			increaseAttackSpeed(PlusPoint, ATTR_MAX);
			break;
		case OPTION_CRITICAL_HIT:
			m_CriticalRatio[ATTR_CURRENT] += PlusPoint;
			m_CriticalRatio[ATTR_MAX]     += PlusPoint;
			break;

		case OPTION_ALL_ATTR:
			m_STR[ATTR_CURRENT] += PlusPoint;
			m_STR[ATTR_MAX]     += PlusPoint;

			m_DEX[ATTR_CURRENT] += PlusPoint;
			m_DEX[ATTR_MAX]     += PlusPoint;

			m_INT[ATTR_CURRENT] += PlusPoint;
			m_INT[ATTR_MAX]     += PlusPoint;

			computeStatOffset();
			break;

		case OPTION_ALL_RES:
			m_Resist[MAGIC_DOMAIN_POISON] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_ACID] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_CURSE] += PlusPoint;
			m_Resist[MAGIC_DOMAIN_BLOOD] += PlusPoint;
///////////////////////////////////////////////////////////////////////////
//	������ ��� ����Ʈ�� ����...			
			m_Resist[MAGIC_DOMAIN_ALLRES] += PlusPoint;
			break;

		case OPTION_LUCK:
			m_Luck += PlusPoint;
			break;

		case OPTION_CONSUME_MP:
			m_ConsumeMPRatio += PlusPoint;
			break;

		case OPTION_MAGIC_DAMAGE:
			m_MagicBonusDamage += PlusPoint;
			break;

		case OPTION_PHYSIC_DAMAGE:
			m_PhysicBonusDamage += PlusPoint;
			break;

		case OPTION_GAMBLE_PRICE:
			m_GamblePriceRatio += PlusPoint;
			break;
			
		case OPTION_POTION_PRICE:
			m_PotionPriceRatio += PlusPoint;
			break;
		case OPTION_PHYSIC_PRO:
			m_PhysicDamageReduce += PlusPoint;
			break;
		case OPTION_MAGIC_PRO:
			m_MagicDamageReduce += PlusPoint;
			break;


		default:
			break;
	}

}

void Ousters::computeOptionStat(OptionType_t OptionType)
	throw()
{
	__BEGIN_TRY

	OptionInfo*   pOptionInfo   = g_pOptionInfoManager->getOptionInfo(OptionType);
	if ( pOptionInfo == NULL )
	{
		filelog("InvalidOptionInfo.log", "Ousters::computeOptionStat() Name=%s InvalidOptionType=%d",
			getName().c_str(), OptionType
		);
		return;
	}

	computeOptionClassStat( pOptionInfo->getClass(), pOptionInfo->getPlusPoint() );
/*	OptionClass   OClass        = pOptionInfo->getClass();

	switch (OClass)
	{
		case OPTION_STR:
			m_STR[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_STR[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_DEX:
			m_DEX[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_DEX[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_INT:
			m_INT[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_INT[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			computeStatOffset();
			break;
		case OPTION_HP:
			m_HP[ATTR_MAX]   += pOptionInfo->getPlusPoint();
			m_HP[ATTR_BASIC] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_MP:
			m_MP[ATTR_MAX]   += pOptionInfo->getPlusPoint();
			m_MP[ATTR_BASIC] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_HP_STEAL:
			m_HPStealAmount += pOptionInfo->getPlusPoint();
			break;
		case OPTION_MP_STEAL:
			m_MPStealAmount += pOptionInfo->getPlusPoint();
			break;
		case OPTION_HP_REGEN:
			m_HPRegen += pOptionInfo->getPlusPoint();
			break;
		case OPTION_MP_REGEN:
			m_MPRegen += pOptionInfo->getPlusPoint();
			break;
		case OPTION_TOHIT:
			m_ToHit[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_ToHit[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_DEFENSE:
			m_Defense[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Defense[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_DAMAGE:
			m_Damage[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Damage[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			m_Damage[ATTR_BASIC]   += pOptionInfo->getPlusPoint();
			break;
		case OPTION_PROTECTION:
			m_Protection[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_Protection[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_POISON:
			m_Resist[MAGIC_DOMAIN_POISON] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_ACID:
			m_Resist[MAGIC_DOMAIN_ACID] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_CURSE:
			m_Resist[MAGIC_DOMAIN_CURSE] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_BLOOD:
			m_Resist[MAGIC_DOMAIN_BLOOD] += pOptionInfo->getPlusPoint();
			break;
		case OPTION_VISION:
			break;
		case OPTION_ATTACK_SPEED:
			m_AttackSpeed[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_AttackSpeed[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;
		case OPTION_CRITICAL_HIT:
			m_CriticalRatio[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_CriticalRatio[ATTR_MAX]     += pOptionInfo->getPlusPoint();
			break;

		case OPTION_ALL_ATTR:
			m_STR[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_STR[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			m_DEX[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_DEX[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			m_INT[ATTR_CURRENT] += pOptionInfo->getPlusPoint();
			m_INT[ATTR_MAX]     += pOptionInfo->getPlusPoint();

			computeStatOffset();
			break;

		case OPTION_ALL_RES:
			m_Resist[MAGIC_DOMAIN_POISON] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_ACID] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_CURSE] += pOptionInfo->getPlusPoint();
			m_Resist[MAGIC_DOMAIN_BLOOD] += pOptionInfo->getPlusPoint();
			break;

		case OPTION_LUCK:
			m_Luck += pOptionInfo->getPlusPoint();
			break;

		case OPTION_CONSUME_MP:
			m_ConsumeMPRatio = pOptionInfo->getPlusPoint();
			break;

		case OPTION_MAGIC_DAMAGE:
			m_MagicBonusDamage = pOptionInfo->getPlusPoint();
			break;

		case OPTION_PHYSIC_DAMAGE:
			m_PhysicBonusDamage = pOptionInfo->getPlusPoint();
			break;

		case OPTION_GAMBLE_PRICE:
			m_GamblePriceRatio = pOptionInfo->getPlusPoint();
			break;
			
		case OPTION_POTION_PRICE:
			m_PotionPriceRatio = pOptionInfo->getPlusPoint();
			break;

		default:
			break;
	}*/

	__END_CATCH
}

void Ousters::addModifyInfo(const OUSTERS_RECORD& prev, ModifyInfo& pkt) const
	throw()
{
	__BEGIN_TRY

	if (prev.pSTR[ATTR_CURRENT] != m_STR[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_STR, m_STR[ATTR_CURRENT]);
	if (prev.pSTR[ATTR_MAX    ] != m_STR[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_STR,     m_STR[ATTR_MAX]);
	if (prev.pSTR[ATTR_BASIC  ] != m_STR[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_STR,   m_STR[ATTR_BASIC]);

	if (prev.pDEX[ATTR_CURRENT] != m_DEX[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_DEX, m_DEX[ATTR_CURRENT]);
	if (prev.pDEX[ATTR_MAX    ] != m_DEX[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_DEX,     m_DEX[ATTR_MAX]);
	if (prev.pDEX[ATTR_BASIC  ] != m_DEX[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_DEX,   m_DEX[ATTR_BASIC]);

	if (prev.pINT[ATTR_CURRENT] != m_INT[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_INT, m_INT[ATTR_CURRENT]);
	if (prev.pINT[ATTR_MAX    ] != m_INT[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_INT,     m_INT[ATTR_MAX]);
	if (prev.pINT[ATTR_BASIC  ] != m_INT[ATTR_BASIC  ]) pkt.addShortData(MODIFY_BASIC_INT,   m_INT[ATTR_BASIC]);

	if (prev.pHP[ATTR_MAX    ] != m_HP[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_HP,     m_HP[ATTR_MAX]);
	if (prev.pHP[ATTR_CURRENT] != m_HP[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_HP, m_HP[ATTR_CURRENT]);

	if (prev.pMP[ATTR_MAX    ] != m_MP[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_MP,     m_MP[ATTR_MAX]);
	if (prev.pMP[ATTR_CURRENT] != m_MP[ATTR_CURRENT]) pkt.addShortData(MODIFY_CURRENT_MP, m_MP[ATTR_CURRENT]);

	if (prev.pSilverDamage != m_SilverDamage) pkt.addShortData(MODIFY_SILVER_DAMAGE, m_SilverDamage);

	if (prev.pDamage[ATTR_CURRENT] != m_Damage[ATTR_CURRENT]) pkt.addShortData(MODIFY_MIN_DAMAGE, m_Damage[ATTR_CURRENT]);
	if (prev.pDamage[ATTR_MAX    ] != m_Damage[ATTR_MAX    ]) pkt.addShortData(MODIFY_MAX_DAMAGE, m_Damage[ATTR_MAX]);

	if (prev.Defense     != m_Defense[ATTR_CURRENT])     pkt.addShortData(MODIFY_DEFENSE,      m_Defense[ATTR_CURRENT]);
	if (prev.Protection  != m_Protection[ATTR_CURRENT])  pkt.addShortData(MODIFY_PROTECTION,   m_Protection[ATTR_CURRENT]);
	if (prev.ToHit       != m_ToHit[ATTR_CURRENT])       pkt.addShortData(MODIFY_TOHIT,        m_ToHit[ATTR_CURRENT]);
	if (prev.AttackSpeed != getAttackSpeed(ATTR_CURRENT)) pkt.addShortData(MODIFY_ATTACK_SPEED, getAttackSpeed(ATTR_CURRENT));

	// by sigi. 2002.9.10
	if (prev.Rank != getRank())
	{
		pkt.addShortData(MODIFY_RANK, getRank());
		pkt.addLongData(MODIFY_RANK_EXP, getRankGoalExp());
	}

	if (prev.Bonus != m_Bonus)
	{
		pkt.addShortData(MODIFY_BONUS_POINT, m_Bonus);
	}

#ifdef __CONTRIBUTION_SYSTEM__
	if(prev.ContributePoint != m_ContributePoint)
		pkt.addLongData(MODIFY_CONTRIBUTE_POINT, m_ContributePoint);
#endif

	if (prev.Luck != m_Luck)
	{
		pkt.addShortData(MODIFY_LUCK, m_Luck);
	}
	if (prev.HpSteal != m_HPStealAmount)
	{
		pkt.addShortData(MODIFY_HPSTEAL, m_HPStealAmount);
	}
	if (prev.MpSteal != m_MPStealAmount)
	{
		pkt.addShortData(MODIFY_MPSTEAL, m_MPStealAmount);
	}
	if (prev.AllRes != m_Resist[MAGIC_DOMAIN_ALLRES])
	{
		pkt.addShortData(MODIFY_ALLRES, m_Resist[MAGIC_DOMAIN_ALLRES]);
	}
	if (prev.Cri != m_CriticalRatio[ATTR_MAX])
	{
		pkt.addShortData(MODIFY_CRI, m_CriticalRatio[ATTR_MAX]);
	}
	__END_CATCH
}

void Ousters::sendModifyInfo(const OUSTERS_RECORD& prev) const
	throw()
{
	__BEGIN_TRY

	GCModifyInformation gcModifyInformation;
	addModifyInfo(prev, gcModifyInformation);
	m_pPlayer->sendPacket(&gcModifyInformation);

	BloodBibleSignInfo* pInfo = getBloodBibleSign();
	GCBloodBibleSignInfo gcInfo;
	gcInfo.setSignInfo( pInfo );
//	cout << "open num : " << pInfo->getOpenNum() << endl;;
	m_pPlayer->sendPacket( &gcInfo );

	__END_CATCH	
}

void Ousters::initAllStatAndSend()
{
	OUSTERS_RECORD prev;
	getOustersRecord(prev);
	initAllStat();
	sendRealWearingInfo();
	sendModifyInfo(prev);
	//sendSkillInfo();
}


