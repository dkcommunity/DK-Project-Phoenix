//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillInfo.cpp
// Written By  : beowulf
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SkillInfo.h"
#include "Assert.h"
#include "DB.h"
#include "Skill.h"
#include "SkillPropertyManager.h"
#include "SkillUtil.h"
#include "SkillCastingTimeManager.h"

//#include <algo.h>

//////////////////////////////////////////////////////////////////////////////
// class SkillInfo member methods
//////////////////////////////////////////////////////////////////////////////

SkillInfo::SkillInfo()
{
	__BEGIN_TRY

	m_RequireSkills.clear();

	m_RequireFire = 0;
	m_RequireWater = 0;
	m_RequireEarth = 0;
	m_RequireWind = 0;
	m_RequireSum = 0;

	m_RequireWristletElemental = ELEMENTAL_ANY;
	m_RequireStone1Elemental = ELEMENTAL_ANY;
	m_RequireStone2Elemental = ELEMENTAL_ANY;
	m_RequireStone3Elemental = ELEMENTAL_ANY;
	m_RequireStone4Elemental = ELEMENTAL_ANY;

#ifdef VERSION_SKILL_1
	m_AvoidAttribute = 0;
#endif

	m_CanDelete = 0;

	__END_CATCH
}

SkillInfo::~SkillInfo()
{
	__BEGIN_TRY
	__END_CATCH
}

void SkillInfo::setRequireSkill( const string& requireSkill )
{

	if ( requireSkill == "" ) return;

	/////////////////////////////
	// 012345678901234567890
	// 111,112,114
	// c  d
	//     c  d
	//         c  d
	/////////////////////////////
	// FAZ 4 - 64-bit migration fix: uint string::npos truncate oluyor.
	size_t a = requireSkill.find_first_of('(', 0 );
	size_t b = requireSkill.find_first_of(')', a+1 );

	if ( a == string::npos || b == string::npos || a > b ) return;

	string requires = trim( requireSkill.substr( a+1, b-a-1 ) );

	// FAZ 4 - 64-bit migration fix.
	size_t c = 0;
	size_t d;
	do
	{
		d = requires.find_first_of(',', c+1 );
		if ( d == string::npos )
			d = requires.size();

		SkillType_t skillType = (SkillType_t)atoi( trim( requires.substr( c, d-c ) ).c_str() );
		addRequireSkill( skillType );

		c = d + 1;
	} while ( requires.size() > 0 && c < requires.size() - 1 );
}

void SkillInfo::setCondition( const string& condition )
{
	if ( condition == "" ) return;

	// FAZ 4 - 64-bit migration fix: uint string::npos truncate oluyor.
	size_t a = 0, b = 0, c = 0;

	do
	{
		///////////////////////////////////////////////////////
		// 01234567890123456789012345678901234567890123456789
		// (Fire,10)(Water,5)(Wristlet,Fire)(Stone1,Water)
		// a    b  ca     b ca        b    ca      b     c
		///////////////////////////////////////////////////////
		a = condition.find_first_of('(', c);
		b = condition.find_first_of(',', a+1);
		c = condition.find_first_of(')', b+1);

		if ( a == string::npos || b == string::npos || c == string::npos ) break;
		if ( a > b || b > c ) break;

		Assert( a+1<b && b+1<c );

		string identifier = trim( condition.substr( a+1, b-a-1 ) );
		string require    = trim( condition.substr( b+1, c-b-1 ) );

		if ( identifier == "Fire" ) setRequireFire( atoi( require.c_str() ) );
		else if ( identifier == "Water" ) setRequireWater( atoi( require.c_str() ) );
		else if ( identifier == "Earth" ) setRequireEarth( atoi( require.c_str() ) );
		else if ( identifier == "Wind" ) setRequireWind( atoi( require.c_str() ) );
		else if ( identifier == "Sum" ) setRequireSum( atoi( require.c_str() ) );
		else if ( identifier == "Wristlet" ) setRequireWristletElemental( getElementalTypeFromString( require ) );
		else if ( identifier == "Stone1" ) setRequireStone1Elemental( getElementalTypeFromString( require ) );
		else if ( identifier == "Stone2" ) setRequireStone2Elemental( getElementalTypeFromString( require ) );
		else if ( identifier == "Stone3" ) setRequireStone3Elemental( getElementalTypeFromString( require ) );
		else if ( identifier == "Stone4" ) setRequireStone4Elemental( getElementalTypeFromString( require ) );

	// FAZ 4 - guard: bos string'de condition.size()-1 size_t underflow uretiyor.
	} while ( condition.size() > 0 && c < condition.size() - 1 );
}

string SkillInfo::toString() const
{
	__BEGIN_TRY
	
	StringStream msg;
	msg << "SkillInfo ("
		<< "Type:"         << (int)m_Type
		<< ",Name:"        << m_Name
		<< ",Level:"       << (int)m_Level
		<< ",MinDam:"      << (int)m_MinDamage
		<< ",MaxDam:"      << (int)m_MaxDamage
		<< ",MinDelay:"    << (int)m_MinDelay
		<< ",MaxDelay:"    << (int)m_MaxDelay
		<< ",MinCastTime:" << (int)m_MinCastTime
		<< ",MaxCastTime:" << (int)m_MaxCastTime
		<< ",MinDuration:" << (int)m_MinDuration
		<< ",MaxDuration:" << (int)m_MaxDuration
		<< ",ConsumeMP:"   << (int)m_ConsumeMP
		<< ",MaxRange:"    << (int)m_MaxRange
		<< ",MinRange:"    << (int)m_MinRange
		<< ",Point:"       << (int)m_Point
		<< ",Target:"      << (int)m_Target
		<< ",RequireFire:"	<< (int)m_RequireFire
		<< ",RequireWater:"	<< (int)m_RequireWater
		<< ",RequireEarth:"	<< (int)m_RequireEarth
		<< ",RequireWind:"	<< (int)m_RequireWind
		<< ",RequireSum:"	<< (int)m_RequireSum
		<< ",RequireWristletElemental:"	<< (int)m_RequireWristletElemental
		<< ",RequireStone1Elemental:"	<< (int)m_RequireStone1Elemental
		<< ",RequireStone2Elemental:"	<< (int)m_RequireStone2Elemental
		<< ",RequireStone3Elemental:"	<< (int)m_RequireStone3Elemental
		<< ",RequireStone4Elemental:"	<< (int)m_RequireStone4Elemental
		<< ",SkillPoint:"				<< (int)m_SkillPoint
		<< ",LevelUpPoint:"				<< (int)m_LevelUpPoint;
	for ( list<SkillType_t>::const_iterator itr = m_RequireSkills.begin() ; itr != m_RequireSkills.end() ; itr++ )
	{
		msg << ",SkillType:" << (int)(*itr);
	}

	msg << ")";
	return msg.toString();

	__END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// class SkillInfoManager member methods
//////////////////////////////////////////////////////////////////////////////

SkillInfoManager::SkillInfoManager()
	throw()
{
	__BEGIN_TRY

	m_SkillCount = 0;
	m_SkillInfoList = NULL;

	__END_CATCH
}

SkillInfoManager::~SkillInfoManager()
	throw()
{
	__BEGIN_TRY

	// �̰͸� �ؼ��� �ȵǴµ�.. ������ �� �ҷ����ű� ������ �ϴ� ����.. by sigi
	SAFE_DELETE_ARRAY(m_SkillInfoList);

	__END_CATCH
}

void SkillInfoManager::init()
	throw(Error)
{
	__BEGIN_TRY

	load();

	for ( uint i=0; i<m_SkillCount; ++i )
	{
		if ( m_SkillInfoList[i] != NULL )
		{
			list<SkillType_t>& rSkills = m_SkillInfoList[i]->getRequireSkills();
			list<SkillType_t>::iterator itr = rSkills.begin();

			for ( ; itr != rSkills.end(); ++itr )
			{
				SkillInfo* pRequireSkillInfo = m_SkillInfoList[(*itr)];

				if ( pRequireSkillInfo == NULL )
				{
					cout << "��ų �ε� ������ Ʋ���Ծ����ϴ�. : " << (int)m_SkillInfoList[i]->getType() << " / " << (int)(*itr) << endl;
					Assert(false);
				}

				pRequireSkillInfo->addRequiredSkill(m_SkillInfoList[i]->getType());
			}
		}
	}

	// ��ų ���� �� �ʱ�ȭ
	for(int i = 0 ; i < SKILL_DOMAIN_MAX; i++) 
	{
		for(int j = 0; j < SLAYER_MAX_DOMAIN_LEVEL+1 ; j++) 
		{
			m_SkillLevelMap[i][j] = 0;
		}
	}

	// ������ ���� ������ �׷��̵� �ʱ�ȭ
	for(int i = 0; i < GRADE_APPRENTICE_LIMIT_LEVEL + 1; i++) 
	{
		m_DomainGradeMap[i] = SKILL_GRADE_APPRENTICE;
	}
	for(int i = GRADE_APPRENTICE_LIMIT_LEVEL + 1 ; i < GRADE_ADEPT_LIMIT_LEVEL + 1; i++) 
	{
		m_DomainGradeMap[i] = SKILL_GRADE_ADEPT;
	}
	for(int i = GRADE_ADEPT_LIMIT_LEVEL + 1 ; i < GRADE_EXPERT_LIMIT_LEVEL + 1; i++) 
	{
		m_DomainGradeMap[i] = SKILL_GRADE_EXPERT;
	}
	for(int i = GRADE_EXPERT_LIMIT_LEVEL + 1 ; i < GRADE_MASTER_LIMIT_LEVEL + 1; i++) 
	{
		m_DomainGradeMap[i] = SKILL_GRADE_MASTER;
	}
	for(int i = GRADE_MASTER_LIMIT_LEVEL + 1 ; i < GRADE_GRAND_MASTER_LIMIT_LEVEL + 1; i++) 
	{
		m_DomainGradeMap[i] = SKILL_GRADE_GRAND_MASTER;
	}
	for(int i = GRADE_GRAND_MASTER_LIMIT_LEVEL + 1 ; i < SLAYER_MAX_DOMAIN_LEVEL + 1; i++) 
	{
		m_DomainGradeMap[i] = SKILL_GRADE_GRAND_MASTER;
	}


	// ������ �׷��̵忡 ���� ��� ���� ���� �ʱ�ȭ
	m_LimitLevelMap[SKILL_GRADE_APPRENTICE] = GRADE_APPRENTICE_LIMIT_LEVEL;
	m_LimitLevelMap[SKILL_GRADE_ADEPT] = GRADE_ADEPT_LIMIT_LEVEL;
	m_LimitLevelMap[SKILL_GRADE_EXPERT] = GRADE_EXPERT_LIMIT_LEVEL;
	m_LimitLevelMap[SKILL_GRADE_MASTER] = GRADE_MASTER_LIMIT_LEVEL;
	m_LimitLevelMap[SKILL_GRADE_GRAND_MASTER] = GRADE_GRAND_MASTER_LIMIT_LEVEL;
	m_LimitLevelMap[SKILL_GRADE_GRAND_MASTER + 1] = GRADE_GRAND_MASTER_LIMIT_LEVEL;

	// ��Ʈ�� ���� MP ������ ���̺� �ʱ�ȭ
	for(int i = 0; i < 43; i++) 
	{
		m_decreaseConsumeMP[i] = 0;
	}

	int provide = 8;
	int Multiple = 1;
	for(int i = 43; i < 85; i++) 
	{
		m_decreaseConsumeMP[i] = 5* Multiple;
		provide--;
		if (provide  == 0) 
		{
			provide = 8;
			Multiple++;
		}
	}

	provide = 10;

	for(int i = 85; i < 140; i++) 
	{
		m_decreaseConsumeMP[i] = 5* Multiple;
		provide--;
		if (provide == 0) 
		{
			provide = 10;
			Multiple++;
		}
	}

	provide = 15;
	for(int i = 140; i < 201; i++) 
	{
		m_decreaseConsumeMP[i] = 5* Multiple;
		provide--;
		if (provide == 0) 
		{
			provide = 15;
			Multiple++;
		}
	}

	provide = 15;
	for(int i = 201; i < 301; i++) 
	{
		m_decreaseConsumeMP[i] = 5* Multiple;
		provide--;
		if (provide == 0) 
		{
			provide = 15;
			Multiple++;
		}
	}

	__END_CATCH
}

void SkillInfoManager::load()
	throw(Error)
{
	__BEGIN_TRY
	__BEGIN_DEBUG

	Statement* pStmt    = NULL;
	Result*    pResult  = NULL;

	// Skill Property Manager �� �ʱ�ȭ�Ѵ�.
	g_pSkillPropertyManager->init();
	
	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
		pResult = pStmt->executeQuery("SELECT MAX(Type) FROM SkillBalance");

		if (pResult->getRowCount() == 0)
		{
			SAFE_DELETE(pStmt);
			throw Error ("There is no data in SkillInfo Table");
		}

		pResult->next();

		int maxSkillType = pResult->getInt(1);
		const int requiredLiveSkillBalanceMaxType = SKILL_METEORIC_FALL_2;
		const int maxCompiledSkillBalanceType = SKILL_MAX - 1;

		if (maxSkillType < requiredLiveSkillBalanceMaxType)
		{
			StringStream msg;
			msg << "SkillBalance contract mismatch: DB max Type=" << maxSkillType
				<< ", required at least " << requiredLiveSkillBalanceMaxType
				<< " for live skill contract";
			cerr << msg.toString() << endl;
			SAFE_DELETE(pStmt);
			throw Error(msg.toString());
		}

		if (maxSkillType > maxCompiledSkillBalanceType)
		{
			StringStream msg;
			msg << "SkillBalance contract mismatch: DB max Type=" << maxSkillType
				<< ", supported max Type is " << maxCompiledSkillBalanceType
				<< " for this GameServer binary";
			cerr << msg.toString() << endl;
			SAFE_DELETE(pStmt);
			throw Error(msg.toString());
		}

		if (maxSkillType > requiredLiveSkillBalanceMaxType)
		{
			cerr << "SkillBalance contains extension/test rows: DB max Type="
				<< maxSkillType << ", live contract ends at "
				<< requiredLiveSkillBalanceMaxType << endl;
		}

		m_SkillCount = maxSkillType + 1;
		if (m_SkillCount < (uint)SKILL_MAX)
		{
			m_SkillCount = SKILL_MAX;
		}

		Assert (m_SkillCount > 0);

		m_SkillInfoList = new SkillInfo* [m_SkillCount];
		
		string strDefCastingTimeList;
		string strCastingTimeList;

		for (uint i=0 ; i < m_SkillCount; i++)
		{
			m_SkillInfoList[i] = NULL;
		}

		pResult = pStmt->executeQuery(
			"SELECT " 
				"Type, Name, Level, MinDam, MaxDam, MinDelay, MaxDelay, MinDur, MaxDur, "
				"Mana, MinRange, MaxRange, Target, SubExp, Point, Domain, MagicDomain,"
				"Melee, Magic, Physic, SingleTarget, WideTarget, "
				"RequireSkill, DefaultCastingTime, CastingTime, "
				"SkillPoint, LevelUpPoint, Conditions, ElementalDomain, CanDelete, AvoidAttribute "
			"FROM SkillBalance");

		while (pResult->next()) 
		{
			SkillInfo* pSkillInfo = new SkillInfo ();
			int        i          = 0;

			pSkillInfo->setType(pResult->getInt(++i));
			pSkillInfo->setName (pResult->getString(++i));
			pSkillInfo->setLevel(pResult->getInt(++i));

			pSkillInfo->setMinDamage(pResult->getInt(++i));
			pSkillInfo->setMaxDamage(pResult->getInt(++i));
			pSkillInfo->setMinDelay(pResult->getInt(++i));
			pSkillInfo->setMaxDelay(pResult->getInt(++i));
			pSkillInfo->setMinDuration(pResult->getInt(++i));
			pSkillInfo->setMaxDuration(pResult->getInt(++i));

			pSkillInfo->setConsumeMP(pResult->getInt(++i));
			pSkillInfo->setMinRange(pResult->getInt(++i));
			pSkillInfo->setMaxRange(pResult->getInt(++i));
			pSkillInfo->setTarget(pResult->getInt(++i));
			pSkillInfo->setSubSkill(pResult->getInt(++i));
			pSkillInfo->setPoint(pResult->getInt(++i));
			pSkillInfo->setDomainType(pResult->getBYTE(++i));
			pSkillInfo->setMagicDomain(pResult->getBYTE(++i));

			// Skill Property �߰�
			SkillProperty* pSkillProperty = new SkillProperty();

			pSkillProperty->setType( pSkillInfo->getType() );
			pSkillProperty->setMelee( pResult->getInt(++i) );
			pSkillProperty->setMagic( pResult->getInt(++i) );
			pSkillProperty->setPhysic( pResult->getInt(++i) );
			pSkillProperty->setSingleTarget( pResult->getInt(++i) );
			pSkillProperty->setWideTarget( pResult->getInt(++i) );

			g_pSkillPropertyManager->addSkillProperty( pSkillProperty );

			pSkillInfo->setRequireSkill( pResult->getString(++i) ); // 20080402 ��ġ �ٲ۴�. 

			strDefCastingTimeList = pResult->getString(++i);
			strCastingTimeList = pResult->getString(++i);
			
			if ( pSkillInfo->getDomainType() == SKILL_DOMAIN_OUSTERS )
			{
				pSkillInfo->setSkillPoint( pResult->getInt(++i) );
				pSkillInfo->setLevelUpPoint( pResult->getInt(++i) );
				pSkillInfo->setCondition( pResult->getString(++i) );
				pSkillInfo->setElementalDomain( pResult->getInt(++i) );
				pSkillInfo->setCanDelete( pResult->getInt(++i) );
			}

			pSkillInfo->setAvoidAttribute( pResult->getInt(++i) );
			
			// Skill Info �߰�
			addSkillInfo(pSkillInfo);
			
			////////////////////////////////////////////////////////////////////////////////////////
			// ��ų �����ð� ����
			SkillCastingTimeManager::TParamList DefParamList;
			SkillCastingTimeManager::TParamList ParamList;
			
			g_SkillCastingTimeManager.ParseSkillCastingTime(strDefCastingTimeList, DefParamList);
			g_SkillCastingTimeManager.SetDefault(pSkillInfo->getType(), DefParamList);
			
			g_SkillCastingTimeManager.ParseSkillCastingTime(strCastingTimeList, ParamList);
			g_SkillCastingTimeManager.Set(pSkillInfo->getType(), ParamList);
		
		}

		const SkillType_t requiredSkillBalanceTypes[] =
		{
			SKILL_BLADE_ERUPTION_1,
			SKILL_ATTRACT_SWORD_CRASH_1,
			SKILL_STRATEGIC_BOMBER_1,
			SKILL_AURA_EXPLOSION_1,
			SKILL_HOLY_ACIDALIUM_1,
			SKILL_BAT_SPEAR_1,
			SKILL_EXTENSIVE_DELUSION_1,
			SKILL_ACCELERATE_CHARGE_1,
			SKILL_RAGE_BURST_1,
			SKILL_FROZEN_GROUND_1,
			SKILL_METEORIC_FALL_1,
			SKILL_BLADE_ERUPTION_2,
			SKILL_ATTRACT_SWORD_CRASH_2,
			SKILL_STRATEGIC_BOMBER_2,
			SKILL_AURA_EXPLOSION_2,
			SKILL_HOLY_ACIDALIUM_2,
			SKILL_BAT_SPEAR_2,
			SKILL_EXTENSIVE_DELUSION_2,
			SKILL_ACCELERATE_CHARGE_2,
			SKILL_RAGE_BURST_2,
			SKILL_FROZEN_GROUND_2,
			SKILL_METEORIC_FALL_2,
			SKILL_IRON_MAIDEN,
			SKILL_CHAOS_SPLITTER_2_M,
			SKILL_FIRE_WAVE_M,
			SKILL_EXTENSIVE_DELUSION_2_M,
			SKILL_BLOODY_SKULL_2_M,
			SKILL_TEMENOS_M,
			SKILL_WILD_WOLF_STRIKE_M,
			SKILL_VENOM_CIRCLE_M,
			SKILL_BAT_SPEAR_2_M
		};

		for (uint i = 0 ; i < sizeof(requiredSkillBalanceTypes) / sizeof(requiredSkillBalanceTypes[0]) ; ++i)
		{
			SkillType_t requiredType = requiredSkillBalanceTypes[i];
			if (requiredType >= m_SkillCount || m_SkillInfoList[requiredType] == NULL)
			{
				StringStream msg;
				msg << "SkillBalance contract mismatch: missing required Type="
					<< (int)requiredType;
				cerr << msg.toString() << endl;
				SAFE_DELETE(pStmt);
				throw Error(msg.toString());
			}
		}
		
		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_DEBUG
	__END_CATCH
}

void SkillInfoManager::save()
	throw(Error)
{
	__BEGIN_TRY

	throw UnsupportedError (__PRETTY_FUNCTION__);
	
	__END_CATCH
}

void SkillInfoManager::addSkillInfo(SkillInfo* pSkillInfo)
	throw(DuplicatedException, Error)
{
	__BEGIN_TRY

  	Assert (pSkillInfo != NULL);

	if (m_SkillInfoList[pSkillInfo->getType()] != NULL)
		throw DuplicatedException ();

	m_SkillInfoList[pSkillInfo->getType()] = pSkillInfo;

	m_SkillLevelMap[pSkillInfo->getDomainType()][pSkillInfo->getLevel()] = pSkillInfo->getType();

/*	list<SkillType_t>& rSkills = pSkillInfo->getRequireSkills();
	list<SkillType_t>::iterator itr = rSkills.begin();

	for ( ; itr != rSkills.end(); ++itr )
	{
		SkillInfo* pRequireSkillInfo = m_SkillInfoList[(*itr)];

		if ( pRequireSkillInfo == NULL )
		{
			cout << "��ų �ε� ������ Ʋ���Ծ����ϴ�. : " << (int)pSkillInfo->getType() << " / " << (int)(*itr) << endl;
			Assert(false);
		}

		pRequireSkillInfo->addRequiredSkill(pSkillInfo->getType());
	}*/
	
	__END_CATCH
}

SkillInfo* SkillInfoManager::getSkillInfo(SkillType_t SkillType)
	const throw(NoSuchElementException , OutOfBoundException, Error)
{
	__BEGIN_TRY

	if (SkillType >= m_SkillCount)
	{
		cerr << "SkillInfoManager::getSkillInfo() : out of bounds : " << (int)SkillType << endl;
		throw OutOfBoundException ();
	}

	if (m_SkillInfoList[SkillType] == NULL)
	{
		cerr << "SkillInfoManager::getSkillInfo() : no such element : " << (int)SkillType << endl;
		throw NoSuchElementException ();
	}

	return m_SkillInfoList[SkillType];
	
	__END_CATCH
}

SkillType_t SkillInfoManager::getSkillTypeByLevel(SkillDomainType_t SkillDomain , SkillLevel_t SkillLevel)
	throw(OutOfBoundException)
{
	__BEGIN_TRY

	if (SkillDomain >= SKILL_DOMAIN_MAX) throw OutOfBoundException();
	if (SkillLevel > SLAYER_MAX_DOMAIN_LEVEL) throw OutOfBoundException();

	return m_SkillLevelMap[SkillDomain][SkillLevel];

	__END_CATCH
}

// �������� ������ �� ������ � ��޿� �ش��ϴ��� �ƴ� �Լ�.
SkillGrade SkillInfoManager::getGradeByDomainLevel(SkillLevel_t Level)
	throw(OutOfBoundException)
{
	__BEGIN_TRY

	if (Level > SLAYER_MAX_DOMAIN_LEVEL) throw OutOfBoundException();
	return m_DomainGradeMap[Level];

	__END_CATCH
}

// ���� ��޿��� ����� ������� �������� �ö� �� �ִ��� �˾� ����.
Level_t SkillInfoManager::getLimitLevelByDomainGrade(SkillGrade Grade)
	throw(OutOfBoundException)
{
	__BEGIN_TRY

	if (Grade > SKILL_GRADE_MAX) throw OutOfBoundException();
	return m_LimitLevelMap[Grade];

	__END_CATCH
}

// SkillInfoManager:: toString()
string SkillInfoManager::toString() const
	throw()
{
	__BEGIN_TRY

	StringStream msg;

	msg << "SkillInfoManager(\n";

	for (int i = 0 ; i < (int)m_SkillCount ; i ++)
	{
		msg << "SkillInfos[" << i << "] == ";
		msg	<< (m_SkillInfoList[i] == NULL ? "NULL" : m_SkillInfoList[i]->getName());
		msg << "\n";
		if (m_SkillInfoList[i] != NULL) {
			msg << m_SkillInfoList[i]->toString() << "\n";
		}
	}

	msg << "\n";

	for(int i = 0 ; i < SKILL_DOMAIN_MAX; i++) {
		for(int j = 0 ; i < SLAYER_MAX_DOMAIN_LEVEL+1 ; i++) {
			msg << "SkillLevelMap [" << i << "][" << j << "] == ";
			msg << (int)m_SkillLevelMap[i][j] << "\n";
		}
	}

	for(int i = 0; i < SLAYER_MAX_DOMAIN_LEVEL+1 ; i++) {
		msg << "DomainGradeMap [" << i << "] == ";
		msg << (int)m_DomainGradeMap[i] << "\n";
	}

	for(int i = 0 ; i < SKILL_GRADE_MAX; i++) {
		msg << "LimitLevelMap [" << i << "] == ";
		msg << (int)m_LimitLevelMap[i] << "\n";
	}

	msg << ")";

	/*
	StringStream msg;
	msg << "SkillInfoManager(";

	for (uint i = 0 ; i < m_SkillCount ; i ++) 
	{
		if (m_SkillInfoList[i] != NULL)
		{
			msg << m_SkillInfoList[i]->toString();
		}
		else 
		{
			msg << "NULL" ;
		}
	}
	
	msg << ")";
	*/

	return msg.toString();

	__END_CATCH
}

// Global Variable definition
SkillInfoManager* g_pSkillInfoManager = NULL;
