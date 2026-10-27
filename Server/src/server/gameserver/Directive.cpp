////////////////////////////////////////////////////////////////////////
// Filename    : Directive.cpp
// Written By  : �輺��
// Description : 
// MonsterAI���� �ൿ�� �����ϴ� Directive�� DirectiveSet��
// �����س��� �����̴�.
////////////////////////////////////////////////////////////////////////

#include "Directive.h"
#include "StringStream.h"
#include "DB.h"
#include "Assert.h"
#include <string.h>
#include <stdio.h>

#include "skill/Skill.h"

// �����...
#define MODE_CONDITION   0
#define MODE_ACTION      1

// ���� ����
DirectiveSetManager* g_pDirectiveSetManager = NULL;

////////////////////////////////////////////////////////////////////////
// class Directive member methods
////////////////////////////////////////////////////////////////////////

Directive::Directive()
	throw()
{
	__BEGIN_TRY

	m_Action    = 0;
	m_Parameter = 0;
	m_Ratio     = 0;
	m_Weight    = 0;

	__END_CATCH
}

Directive::~Directive()
	throw()
{
	__BEGIN_TRY

	m_Conditions.clear();

	__END_CATCH
}

void Directive::addCondition(int condition) 
	throw()
{
	__BEGIN_TRY

	// ���� ������� ���� ������ �˻��Ѵ�.
	list<int>::iterator itr = m_Conditions.begin();
	for (; itr != m_Conditions.end(); itr++)
	{
		if (condition == *itr) return;
	}

	m_Conditions.push_back(condition);

	__END_CATCH
}

string Directive::toString()
	throw()
{
	__BEGIN_TRY

	StringStream msg;

	// header
	msg << "Directive(";

	// condition output
	msg	<< "Condition : ";
	list<int>::const_iterator itr = m_Conditions.begin();
	for (; itr != m_Conditions.end(); itr++)
		msg << DirectiveCondition2String[*itr] << ",";
	msg << " ";

	// action output
	msg << "Action : " << DirectiveAction2String[m_Action] << " ";

	// parameter output
	if (m_Parameter != 0)
		msg << "Parameter : " << SkillTypes2String[m_Parameter] << " ";

	// ratio output
	if (m_Ratio != 0)
		msg << "Ratio : " << m_Ratio;

	// tail 
	msg << ")";

	return msg.toString();

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// class DirectiveSet member methods
////////////////////////////////////////////////////////////////////////

DirectiveSet::DirectiveSet()
	throw()
{
	__BEGIN_TRY

	__END_CATCH
}

DirectiveSet::~DirectiveSet()
	throw()
{
	__BEGIN_TRY

	// ����Ʈ�� �˻��ϸ鼭 Directive ��ü�� �����Ѵ�.
	list<Directive*>::iterator itr = m_Directives.begin();
	for (; itr != m_Directives.end(); itr++)
	{
		Directive* pDirective = *itr;
		SAFE_DELETE(pDirective);
	}

	m_Directives.clear();

	__END_CATCH
}

void DirectiveSet::clear()
	throw()
{
	__BEGIN_TRY

	// ����Ʈ�� �˻��ϸ鼭 Directive ��ü�� �����Ѵ�.
	list<Directive*>::iterator itr = m_Directives.begin();
	for (; itr != m_Directives.end(); itr++)
	{
		Directive* pDirective = *itr;
		SAFE_DELETE(pDirective);
	}

	m_Directives.clear();

	__END_CATCH
}

bool DirectiveSet::hasCondition( int condition ) const
{
	list<Directive*>::const_iterator itr = m_Directives.begin();
	list<Directive*>::const_iterator endItr = m_Directives.end();

	for (; itr != endItr; ++itr)
	{
		if ( (*itr)->hasCondition(condition) ) return true;
	}

	return false;
}

void DirectiveSet::addDirective(Directive* pDirective)
	throw()
{
	__BEGIN_TRY

	m_Directives.push_back(pDirective);	

	__END_CATCH
}

void DirectiveSet::addDeadDirective(Directive* pDirective)
	throw()
{
	__BEGIN_TRY

	m_DeadDirectives.push_back(pDirective);	

	__END_CATCH
}



////////////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////////////
DirectiveSetManager::DirectiveSetManager()
	throw()
{
	__BEGIN_TRY

	m_nSetCount = 0;
	m_ppSet     = NULL;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// �Ҹ���
////////////////////////////////////////////////////////////////////////
DirectiveSetManager::~DirectiveSetManager()
	throw()
{
	__BEGIN_TRY

	if (m_ppSet)
	{
		for (uint i=0; i<m_nSetCount; i++)
		{
			SAFE_DELETE(m_ppSet[i]);
		}

		SAFE_DELETE_ARRAY(m_ppSet);	// �̰� �ҷ��� ���� ��������.. -_-; by sigi
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// init()
// ��ü�� �ʱ�ȭ�Ѵ�. �ʱ�ȭ�� ObjectManager����...
////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::init()
	throw()
{
	__BEGIN_TRY

	load();

	__END_CATCH
}


////////////////////////////////////////////////////////////////////////
// load()
// DB���� ��� DirectiveSet�� �ε��Ѵ�. 
// MonsterAI ��ü���� �ε�� DirectiveSet�� ���� �����͸��� ������ �ȴ�.
////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::load()
	throw()
{
	__BEGIN_TRY

	Statement* pStmt   = NULL;
	Result*    pResult = NULL;

	BEGIN_DB
	{
		pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

		// ���� ���ڸ� �о�´�.
		pResult = pStmt->executeQuery("SELECT MAX(ID) FROM DirectiveSet");

		if (pResult->getRowCount() == 0) throw ("DirectiveSetManager::load() : ���̺��� �����ϴ�.");

		pResult->next();
		m_nSetCount = pResult->getInt(1)+1;

		// �о���� ���ڸ�ŭ �޸𸮸� �Ҵ��ϰ�...
		if (m_ppSet==NULL)
		{
			m_ppSet = new DirectiveSet*[m_nSetCount];
			Assert(m_ppSet != NULL);
			for (uint i=0; i<m_nSetCount; i++)
			{
				m_ppSet[i] = NULL;
			}
		}

		// ������ directive�� �о���δ�.
		pResult = pStmt->executeQuery("SELECT ID, Name, Content, DeadContent FROM DirectiveSet");

		while (pResult->next())
		{
			uint   index    = pResult->getInt(1);
			string name     = pResult->getString(2);
			string text     = pResult->getString(3);
			string deadtext = pResult->getString(4);

			printf("ID[%d] Directive Loading Begin >> ", index);

			////////////////
			///filelog("Directive.txt", "===============================================");
			//filelog("Directive.txt", "Name:%s", name.c_str());
			////////////////

			createDirectiveSet(index, name, text, deadtext);

			printf("Loading End\n");
		}

		SAFE_DELETE(pStmt);
	}
	END_DB(pStmt)

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getDirectiveSet()
// ������ Ÿ���� DirectiveSet�� �����͸� �����Ѵ�.
////////////////////////////////////////////////////////////////////////
DirectiveSet* DirectiveSetManager::getDirectiveSet(uint index)
	throw()
{
	__BEGIN_TRY

	if (index >= m_nSetCount)
	{
		// FAZ 4 - throw() spec'li fonksiyondan throw() etmek terminate cagiriyor.
		// NULL don, caller null-check yapsin (veri butunlugu garanti edilemez).
		cerr << "[Warn] DirectiveSetManager::getDirectiveSet(): Out of bounds index="
		     << index << " (max=" << m_nSetCount << ") — return NULL" << endl;
		return NULL;
	}

	return m_ppSet[index];

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// createDirectiveSet()
// ���ڿ��� �Ľ��ؼ� ������ �ε����� �ڸ����� DirectiveSet�� ����.
////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::createDirectiveSet(DirectiveSet *pDirectiveSet, const string& text, const string& deadtext)
{
	string beginToken = "DIRECTIVE BEGIN";
	string endToken   = "DIRECTIVE END";
	// FAZ 4 — 64-bit migration fix. string indices size_t olmalı, uint değil.
	size_t start      = 0;
	size_t end        = 0;

	while (end < text.size() - 1)
	{
		////////////////////////////////////////////////////////////
		// ex)
		// DIRECTIVE BEGIN
		//   (CONDITION:ImDying)
		//   (ACTION:Flee)
		// DIRECTIVE END
		// DIRECTIVE BEGIN
		//   (CONDITION:EnemyRangeMelee)
		//   (CONDITION:EnemyDying)
		//   (CONDITION:EnemyNotBloodDrained)
		//   (ACTION:UseSkill,BloodDrain,30)
		// DIRECTIVE END
		////////////////////////////////////////////////////////////

		start = text.find(beginToken, end);
		end   = text.find(endToken,   start+1);

		if (start > end) break;

		Directive* pDirective = new Directive;
		string     directive  = trim(text.substr(start+beginToken.size()+1, end-start-beginToken.size()-1));

		Assert(pDirective != NULL);

		size_t i = 0, j = 0, k = 0; // FAZ 4 — 64-bit: string indices size_t olmalı

		while (k < directive.size())
		{
			// Directive�� �����ϴ� �׼��̳� ����� ���� 
			// �ϳ��� �о���δ�.
			i = directive.find_first_of('(', k);
			j = directive.find_first_of(':', i+1);
			k = directive.find_first_of(')', j+1);

			// ���ڿ��� ���� �ʰ��ߴٸ�, �Ľ��� �ߴ��Ѵ�.
			if (i > j || j > k) break;

			// identifier�� �׼��̳�, ������̳��� ����.
			// parameter�� �� �ڿ� ������� �μ��̴�.
			string identifier = directive.substr(i+1, j-i-1);
			string parameter  = directive.substr(j+1, k-j-1);

			if (identifier == "CONDITION")
			{
				parseDirectiveParameter(pDirective, parameter, MODE_CONDITION);
			}
			else if (identifier == "ACTION")
			{
				parseDirectiveParameter(pDirective, parameter, MODE_ACTION);
			}
			else
			{
				cerr << "DirectiveSetManager::createDirectiveSet() : �� �� ���� IDENTIFIER" << endl;
				throw ("DirectiveSetManager::createDirectiveSet() : �� �� ���� IDENTIFIER");
			}
		}

		pDirectiveSet->addDirective(pDirective);
	}

	// �׾��� �� �ϴ� �ൿ�� �о���δ�. 5�� ���ڿ� ū �ǹ̴� ����.
	// �ƹ� �ൿ�� ���� ���� ���� ���� 0������, ���� �׼��� ���� 
	// ���̰� �밭 50�� �Ǹ���� ������ �ϴµ�... �Ͽ�ư �׼��� ���� 
	// ��쿡�� �о������ �ʱ� ���ؼ�, �ϴ� �˻��. -- �輺��
	if (deadtext.size() > 5)
	{
		start = 0;
		end   = 0;

		while (end < deadtext.size() - 1)
		{
			////////////////////////////////////////////////////////////
			// ex)
			// DIRECTIVE BEGIN
			//   (CONDITION:ImDying)
			//   (ACTION:Flee)
			// DIRECTIVE END
			// DIRECTIVE BEGIN
			//   (CONDITION:EnemyRangeMelee)
			//   (CONDITION:EnemyDying)
			//   (CONDITION:EnemyNotBloodDrained)
			//   (ACTION:UseSkill,BloodDrain,30)
			// DIRECTIVE END
			////////////////////////////////////////////////////////////

			start = deadtext.find(beginToken, end);
			end   = deadtext.find(endToken,   start+1);

			if (start > end) break;

			Directive* pDirective = new Directive;
			string     directive  = trim(deadtext.substr(start+beginToken.size()+1, end-start-beginToken.size()-1));

			Assert(pDirective != NULL);

			size_t i = 0, j = 0, k = 0; // FAZ 4 — 64-bit: string indices size_t olmalı

			while (k < directive.size())
			{
				// Directive�� �����ϴ� �׼��̳� ����� ���� 
				// �ϳ��� �о���δ�.
				i = directive.find_first_of('(', k);
				j = directive.find_first_of(':', i+1);
				k = directive.find_first_of(')', j+1);

				// ���ڿ��� ���� �ʰ��ߴٸ�, �Ľ��� �ߴ��Ѵ�.
				if (i > j || j > k) break;

				// identifier�� �׼��̳�, ������̳��� ����.
				// parameter�� �� �ڿ� ������� �μ��̴�.
				string identifier = directive.substr(i+1, j-i-1);
				string parameter  = directive.substr(j+1, k-j-1);

				if (identifier == "CONDITION")
				{
					parseDirectiveParameter(pDirective, parameter, MODE_CONDITION);
				}
				else if (identifier == "ACTION")
				{
					parseDirectiveParameter(pDirective, parameter, MODE_ACTION);
				}
				else
				{
					cerr << "DirectiveSetManager::createDirectiveSet() : �� �� ���� IDENTIFIER" << endl;
					throw ("DirectiveSetManager::createDirectiveSet() : �� �� ���� IDENTIFIER");
				}
			}

			pDirectiveSet->addDeadDirective(pDirective);
		}
	}
}

void DirectiveSetManager::createDirectiveSet(uint index, const string& name, const string& text, const string& deadtext)
	throw()
{
	__BEGIN_TRY

	if (index >= m_nSetCount)
	{
		cerr << "[Warn] DirectiveSetManager::createDirectiveSet(): Out of bounds index="
		     << index << " (max=" << m_nSetCount << ") — skip create" << endl;
		return;
	}

	// ������ DirectiveSet�� �����ϰ�, �� ���� �����.
	if (m_ppSet[index]==NULL)	// by sigi. 2002.9.26
	{
		//SAFE_DELETE(m_ppSet[index]);
		m_ppSet[index] = new DirectiveSet;
	}
	else
	{
		// ������ �ִ��� �ִٸ�.. �����.
		m_ppSet[index]->clear();
	}

	m_ppSet[index]->setName(name);
	
	createDirectiveSet(m_ppSet[index], text, deadtext);

	m_ppSet[index]->setAttackAir( m_ppSet[index]->hasCondition( DIRECTIVE_COND_ENEMY_ON_AIR ) );
	m_ppSet[index]->setSeeSafeZone( m_ppSet[index]->hasCondition( DIRECTIVE_COND_ENEMY_ON_SAFE_ZONE ) );

	if ( m_ppSet[index]->canSeeSafeZone() ) cout << index << " : can see safe zone" << endl;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// parseDirectiveParameter()
// ���ڿ��� �Ľ��ؼ�, ������̳� �׼��� ����� Directive�� �����Ѵ�.
////////////////////////////////////////////////////////////////////////
void DirectiveSetManager::parseDirectiveParameter(Directive* pDirective, const string& text, int mode)
	throw()
{
	__BEGIN_TRY

	// FAZ 4 — 64-bit migration fix.
	// Eski kod: uint oldpos, pos — uint 32-bit, string::npos ise size_t (64-bit).
	// find_first_of npos döndüğünde uint'e cast olunca 0xFFFFFFFF oluyor,
	// `if (pos == string::npos)` karşılaştırması false veriyor → sonsuz döngü
	// / yanlış token parse → "There's two or more parameters in condition" throw.
	// Düzeltme: string indices için size_t kullan.
	size_t oldpos     = 0;
	size_t pos        = 0;
	uint   paramCount = 0;

	while (oldpos < text.size() && pos < text.size())
	{
		// ���ӵ� �Ķ���ʹ� ","�� ���еȴ�.
		oldpos = pos;
		pos    = text.find_first_of(',', oldpos);

		// ����� ���� ��쿡�� �Ķ���Ͱ� �ϳ��̱� ������
		// ","�� ã���� �ƽ� ���� ������ ���´�. 
		// �̰ɷδ� ���꽺Ʈ�� �Լ��� ����� �θ� ���� 
		// ���� ������ ���ڿ��� ���̷� pos ���� �������ش�.
		if (pos == string::npos) pos = text.size();
		pos++;

		string token = text.substr(oldpos, (pos-1)-oldpos);

		switch (mode)
		{
			case MODE_CONDITION:
				if (paramCount != 0) 
				{
					cerr << "DirectiveSetManager::parseParameter() : There's two or more parameters in condition" << endl;
					cerr << "ParsingText: " << text.c_str() << endl;
					throw("DirectiveSetManager::parseParameter() : There's two or more parameters in condition");
				}
				pDirective->addCondition(getCondition(token));
				break;
			case MODE_ACTION:
				if (paramCount == 0)      pDirective->setAction(getAction(token));
				else if (paramCount == 1)
				{
					switch (pDirective->getAction())
					{
						case DIRECTIVE_ACTION_USE_SKILL :
							pDirective->setParameter(getParameter(token));
						break;

						case DIRECTIVE_ACTION_CHANGE_ENEMY :
						case DIRECTIVE_ACTION_MOVE_RANDOM :
						case DIRECTIVE_ACTION_SAY :
							pDirective->setParameter(atoi(token.c_str()));
						break;


						default :
							cerr << "DirectiveSetManager::parseParameter() : There's two or more parameters in condition" << endl;
							cerr << "ParsingText: " << text.c_str() << endl;
							throw Error("No Need Parameter!");
					}

					////////////////
					//if (pDirective->getAction() == DIRECTIVE_ACTION_USE_SKILL)
					//{
					//	filelog("Directive.txt", "%s", token.c_str());
					//}
					////////////////
				}
				else if (paramCount == 2) pDirective->setRatio(getRatio(token));
				break;	
			default:
				break;
		}

		paramCount++;
	}

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getCondition()
// ���ڿ��� �޾� �׿� �ش��ϴ� ����� ����� �����Ѵ�.
////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getCondition(const string& token)
	throw()
{
	__BEGIN_TRY

	for (int i=0; i<DIRECTIVE_COND_MAX; i++)
		if (DirectiveCondition2String[i] == token) return i;

	StringStream msg;
	msg << "DirectiveSetManager::getCondition() : Invalid condition string["
		<< token << "]";

	cerr << msg.toString() << endl;
	throw(msg.toString());
	return DIRECTIVE_COND_MAX;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getAction()
// ���ڿ��� �޾� �׿� �ش��ϴ� �׼� ����� �����Ѵ�.
////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getAction(const string& token)
	throw()
{
	__BEGIN_TRY
	
	for (int i=0; i<DIRECTIVE_ACTION_MAX; i++)
		if (DirectiveAction2String[i] == token) return i;

	cerr << "DirectiveSetManager::getAction() : Invalid action[" << token << "]" << endl;
	throw("DirectiveSetManager::getAction() : Invalid action");
	return DIRECTIVE_COND_MAX;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getParameter()
// ���ڿ��� �޾� �׿� �ش��ϴ� �Ķ���͸� �����Ѵ�.
// ����μ��� ���Ͱ� ����ϴ� ��ų Ÿ�� ���̴�.
////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getParameter(const string& token)
	throw()
{
	__BEGIN_TRY

	struct SkillParameterAlias
	{
		const char* Token;
		int SkillType;
	};

	static const SkillParameterAlias aliases[] =
	{
		{ "SKILL_CARMILRA_MELEE_ATTACK", RESULT_SKILL_CARMILRA_MELEE_ATTACK },
		{ "SKILL_SMILE_OF_DEATH", SKILL_TRANSFORM_SMILE_OF_DEATH },
		{ "SKILL_BLUE_ULRAYIRY_ANGER_1", SKILL_BLUE_ULRAYIRY_ANGER_2 },
		{ "SKILL_BOMBING_VULCAN", SLILL_BOMBING_VULCAN },
		{ "SKILL_DRACULE_BLOODY_SCARIFY_2", SKILL_DRACULE_BLOOKDY_SCARIFY_2 },
		{ "SKILL_REACTIVE_ARMOR2", SKILL_REACTIVE_ARMOR2 }
	};

	for (int i=0; i<SKILL_MAX; i++)
	{
		if (strcasecmp(token.c_str(), SkillTypes2String[i].c_str()) == 0)
			return i;
	}

	for (uint i=0; i<sizeof(aliases)/sizeof(aliases[0]); ++i)
	{
		if (strcasecmp(token.c_str(), aliases[i].Token) == 0)
			return aliases[i].SkillType;
	}

	cerr << "DirectiveSetManager::getParameter() : Invalid parameter[" << token << "]" << endl;
	throw("DirectiveSetManager::getParameter() : Invalid parameter");

	return 0;

	__END_CATCH
}

////////////////////////////////////////////////////////////////////////
// getRatio()
// ���ڿ��� �޾� �׿� �ش��ϴ� Ȯ������ �����Ѵ�. ������̴�.
////////////////////////////////////////////////////////////////////////
int DirectiveSetManager::getRatio(const string& token)
	throw()
{
	__BEGIN_TRY

	int rValue = atoi(token.c_str());

	if (rValue < 0 || rValue > 100) 
	{
		cerr << "DirectiveSetManager::getRatio() : Invalid ratio value" << endl;
		throw ("DirectiveSetManager::getRatio() : Invalid ratio value");
	}

	return rValue;

	__END_CATCH
}

