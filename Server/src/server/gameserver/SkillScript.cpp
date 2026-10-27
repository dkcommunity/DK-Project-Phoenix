//////////////////////////////////////////////////////////////////////////////
// Filename    : SkillScript.cpp
// Written by  : 
// Description : 
//////////////////////////////////////////////////////////////////////////////

#include "SkillScript.h"
#include "Properties.h"

#include "DEScriptSkill.h"

#include "SimpleMeleeSkill.h"
#include "SimpleMissileSkill.h"
#include "SimpleTileMissileSkill.h"

SkillScript::SkillScript(string SkillName, SkillType_t SkillType) throw()
{
	m_SkillType = SkillType;
	m_SkillName = SkillName;
	
	StringStream skillScriptFile;
	skillScriptFile
		<< g_pConfig->getProperty("HomePath")
		<< "/script/skill/"
		<< (int)m_SkillType
		<< ".lua";
		
	m_pDEScriptSkill = new DEScriptSkill;
	
	m_pDEScriptSkill->Load((char *)skillScriptFile.toString().c_str());
	m_pDEScriptSkill->NotifyInitialize();

}

SkillScript::~SkillScript() throw()
{
	delete m_pDEScriptSkill;
}

string SkillScript::getSkillHandlerName() const throw() 
{
	return m_SkillName; 
}

SkillType_t SkillScript::getSkillType() const throw() 
{ 
	return m_SkillType; 
}
	
void SkillScript::computeOutput(const SkillInput& input, SkillOutput& output)
{
	m_pDEScriptSkill->NotifyComputOutput(input, output);
}

void SkillScript::execute(Monster* pMonster, Creature* pEnemy)
	throw(Error)
{
	__BEGIN_TRY

	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT simpleSkillInput;
	SIMPLE_SKILL_OUTPUT simpleSkillOutput;
	
	simpleSkillInput.SkillType     = getSkillType();
	simpleSkillInput.SkillDamage   = output.Damage;
	simpleSkillInput.Delay         = output.Delay;
	
	m_pDEScriptSkill->NotifyExecuteSkill(simpleSkillInput);
	
	//g_SimpleMeleeSkill.execute(pMonster, pEnemy, simpleSkillInput, simpleSkillOutput);
	g_SimpleMissileSkill.execute(pMonster, pEnemy, simpleSkillInput, simpleSkillOutput);
	
	/*
	//g_SimpleLinearMeleeSkill
	
	//g_SimpleCureSkill
	//g_SimpleTileCureSkill
	
	//g_SimpleMeleeSkill
	//g_SimpleTileMeleeSkill
	 */
	
	/*
	ZoneCoord_t X;
	ZoneCoord_t Y;
	
	X = pEnemy->getX();
	Y = pEnemy->getY();
	
	execute(pMonster, X, Y);
	*/

	__END_CATCH
}

void SkillScript::execute(Monster* pMonster, ZoneCoord_t x, ZoneCoord_t y) throw(Error)
{
	SkillInput input(pMonster);
	SkillOutput output;
	computeOutput(input, output);

	SIMPLE_SKILL_INPUT simpleSkillInput;
	SIMPLE_SKILL_OUTPUT simpleSkillOutput;
	
	simpleSkillInput.SkillType     = getSkillType();
	simpleSkillInput.SkillDamage   = output.Damage;
	simpleSkillInput.Delay         = output.Delay;
	
	m_pDEScriptSkill->NotifyExecuteSkill(simpleSkillInput);

	g_SimpleTileMissileSkill.execute(pMonster, x, y, simpleSkillInput, simpleSkillOutput);
}

