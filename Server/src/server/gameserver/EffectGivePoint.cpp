#include "EffectGivePoint.h"
#include "PlayerCreature.h"
#include "Player.h"
#include "Slayer.h"
#include "Vampire.h"
#include "Ousters.h"
#include "DB.h"
#include "Gpackets/GCModifyInformation.h"
#include "Gpackets/GCRemoveEffect.h"
#include <vector>

namespace
{
	enum PointRewardRace
	{
		POINT_REWARD_ALL = 0,
		POINT_REWARD_SLAYER,
		POINT_REWARD_VAMPIRE,
		POINT_REWARD_OUSTERS
	};

	enum PointRewardLevelSource
	{
		POINT_REWARD_LEVEL = 0,
		POINT_REWARD_DOMAIN
	};

	struct PointRewardRule
	{
		uint RuleID;
		PointRewardRace Race;
		PointRewardLevelSource LevelSource;
		int MinLevel;
		int MaxLevel;
		Gold_t JiFenDelta;
		int IntervalSeconds;
		int Priority;
	};

	typedef vector<PointRewardRule> PointRewardRuleList;

	PointRewardRuleList s_PointRewardRules;
	bool s_PointRewardRulesLoaded = false;

	PointRewardRace parsePointRewardRace(const string& race)
	{
		if (race == "SLAYER") return POINT_REWARD_SLAYER;
		if (race == "VAMPIRE") return POINT_REWARD_VAMPIRE;
		if (race == "OUSTERS") return POINT_REWARD_OUSTERS;
		return POINT_REWARD_ALL;
	}

	PointRewardLevelSource parsePointRewardLevelSource(const string& source)
	{
		if (source == "DOMAIN") return POINT_REWARD_DOMAIN;
		return POINT_REWARD_LEVEL;
	}

	PointRewardRace getPointRewardRace(PlayerCreature* pPC)
	{
		if (pPC->isSlayer()) return POINT_REWARD_SLAYER;
		if (pPC->isVampire()) return POINT_REWARD_VAMPIRE;
		if (pPC->isOusters()) return POINT_REWARD_OUSTERS;
		return POINT_REWARD_ALL;
	}

	int getPointRewardLevel(PlayerCreature* pPC, PointRewardLevelSource source)
	{
		if (source == POINT_REWARD_DOMAIN)
		{
			Slayer* pSlayer = dynamic_cast<Slayer*>(pPC);
			if (pSlayer == NULL)
				return 0;

			return pSlayer->getHighestSkillDomainLevel();
		}

		return pPC->getLevel();
	}

	bool loadPointRewardRules()
	{
		if (s_PointRewardRulesLoaded)
			return !s_PointRewardRules.empty();

		s_PointRewardRulesLoaded = true;
		s_PointRewardRules.clear();

		Statement* pStmt = NULL;
		Result* pResult = NULL;

		BEGIN_DB
		{
			pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
			pResult = pStmt->executeQuery(
				"SELECT RuleID, Race, LevelSource, MinLevel, MaxLevel, JiFenDelta, IntervalSeconds, Priority "
				"FROM PointRewardRule "
				"WHERE Enabled=1 "
				"AND (StartsAt IS NULL OR StartsAt <= now()) "
				"AND (EndsAt IS NULL OR EndsAt >= now()) "
				"ORDER BY Priority DESC, RuleID ASC"
			);

			while (pResult->next())
			{
				int i = 0;
				PointRewardRule rule;
				rule.RuleID = pResult->getInt(++i);
				rule.Race = parsePointRewardRace(pResult->getString(++i));
				rule.LevelSource = parsePointRewardLevelSource(pResult->getString(++i));
				rule.MinLevel = pResult->getInt(++i);
				rule.MaxLevel = pResult->getInt(++i);
				rule.JiFenDelta = pResult->getInt(++i);
				rule.IntervalSeconds = pResult->getInt(++i);
				rule.Priority = pResult->getInt(++i);

				if (rule.MinLevel < 1 || rule.MaxLevel < rule.MinLevel || rule.JiFenDelta == 0 || rule.IntervalSeconds < 1)
				{
					filelog("MarketCurrencyError.log", "[EffectGivePoint] invalid PointRewardRule RuleID=%u race=%d source=%d min=%d max=%d delta=%u interval=%d priority=%d",
						rule.RuleID,
						(int)rule.Race,
						(int)rule.LevelSource,
						rule.MinLevel,
						rule.MaxLevel,
						(uint)rule.JiFenDelta,
						rule.IntervalSeconds,
						rule.Priority
					);
					continue;
				}

				s_PointRewardRules.push_back(rule);
			}

			SAFE_DELETE(pStmt);
		}
		END_DB(pStmt)

		if (s_PointRewardRules.empty())
		{
			filelog("MarketCurrencyError.log", "[EffectGivePoint] no enabled PointRewardRule rows loaded");
			return false;
		}

		return true;
	}

	const PointRewardRule* findPointRewardRule(PlayerCreature* pPC, int& rewardLevel)
	{
		if (!loadPointRewardRules())
			return NULL;

		PointRewardRace race = getPointRewardRace(pPC);

		for (PointRewardRuleList::const_iterator itr = s_PointRewardRules.begin(); itr != s_PointRewardRules.end(); ++itr)
		{
			const PointRewardRule& rule = *itr;
			if (rule.Race != POINT_REWARD_ALL && rule.Race != race)
				continue;

			rewardLevel = getPointRewardLevel(pPC, rule.LevelSource);
			if (rewardLevel >= rule.MinLevel && rewardLevel <= rule.MaxLevel)
				return &rule;
		}

		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
EffectGivePoint::EffectGivePoint(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY 

		setTarget(pCreature);

	__END_CATCH
}
void EffectGivePoint::affect()
	throw(Error)
{
	__BEGIN_TRY

	Creature* pCreature = dynamic_cast<Creature*>(m_pTarget);
	affect( pCreature );

	__END_CATCH
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGivePoint::affect(Creature* pCreature)
throw(Error)
{
	__BEGIN_TRY 
	setNextTime(6000);
	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
	Assert( pPC != NULL );

	int rewardLevel = 0;
	const PointRewardRule* pRule = findPointRewardRule(pPC, rewardLevel);
	if (pRule == NULL)
	{
		filelog("MarketCurrencyError.log", "[EffectGivePoint] no matching reward rule player=%s race=%d level=%d",
			pPC->getName().c_str(),
			(int)getPointRewardRace(pPC),
			rewardLevel
		);
		return;
	}

	setNextTime(pRule->IntervalSeconds * 10);

	if (pRule->JiFenDelta > 0)
	{
		if (!pPC->increaseMarketPointEx(pRule->JiFenDelta))
		{
			filelog("MarketCurrencyError.log", "[EffectGivePoint] MarketPoint grant failed player=%s rule=%u level=%d delta=%u",
				pPC->getName().c_str(),
				pRule->RuleID,
				rewardLevel,
				(uint)pRule->JiFenDelta
			);
			return;
		}

		Player* pPlayer = pPC->getPlayer();
		if (pPlayer != NULL)
		{
			GCModifyInformation gcMI;
			gcMI.addLongData(MODIFY_JIFEN, pPC->getJiFen());
			pPlayer->sendPacket(&gcMI);
		}
	}

	__END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGivePoint::unaffect(Creature* pFromCreature)
throw(Error)
{
	__BEGIN_TRY 

		Assert(pFromCreature != NULL);

	PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pFromCreature);
	Assert( pPC != NULL );

	Player* pPlayer = pPC->getPlayer();
	Assert( pPlayer != NULL );

	pPC->removeFlag( getEffectClass() );

	GCRemoveEffect gcRemoveEffect;
	gcRemoveEffect.setObjectID( pPC->getObjectID() );
	gcRemoveEffect.addEffectList( getEffectClass() );

	pPlayer->sendPacket( &gcRemoveEffect );

	destroy( pPC->getPlayer()->getID() );

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGivePoint::unaffect()
throw(Error)
{
	__BEGIN_TRY

		Creature* pCreature = dynamic_cast<Creature *>(m_pTarget);
	unaffect(pCreature);

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
string EffectGivePoint::toString()
const throw()
{
	__BEGIN_TRY

		StringStream msg;

	msg << "EffectGivePoint("
		<< "ObjectID:" << getObjectID()
		<< ")";

	return msg.toString();

	__END_CATCH

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void EffectGivePointLoader::load(Creature* pCreature) 
throw(Error)
{
	__BEGIN_TRY
	__END_CATCH
}

EffectGivePointLoader* g_pEffectGivePointLoader = NULL;
