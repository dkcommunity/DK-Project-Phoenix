#include "DEScriptEffect.h"

#include "Player.h"
#include "PacketUtil.h"

#include <string>

DEScriptEffect::DEScriptEffect()
{
	m_pEffectScript = NULL;
	
	lua_tinker::class_add<DEScriptEffect>(m_pLuaState, "DEScriptEffect");
	lua_tinker::class_def<DEScriptEffect>(m_pLuaState, "SetDamage", &DEScriptEffect::SetDamage);
	lua_tinker::class_def<DEScriptEffect>(m_pLuaState, "SetNextTime", &DEScriptEffect::SetNextTime);
	lua_tinker::class_def<DEScriptEffect>(m_pLuaState, "SetDeadLine", &DEScriptEffect::SetDeadLine);
	lua_tinker::class_def<DEScriptEffect>(m_pLuaState, "TraverseEffectRegion", &DEScriptEffect::TraverseEffectRegion);
	
	lua_tinker::class_def<DEScriptEffect>(m_pLuaState, "GetBaseX", &DEScriptEffect::GetBaseX);
	lua_tinker::class_def<DEScriptEffect>(m_pLuaState, "GetBaseY", &DEScriptEffect::GetBaseY);
	
	lua_tinker::class_def<DEScriptEffect>(m_pLuaState, "BrocastGCAddEffectToTile", &DEScriptEffect::BrocastGCAddEffectToTile);
	
	lua_tinker::set(m_pLuaState, "DEScriptEffect", this);
}

DEScriptEffect::~DEScriptEffect()
{
}

void DEScriptEffect::SetEffectScript(EffectScript *pEffectScript)
{
	m_pEffectScript = pEffectScript;
}

void DEScriptEffect::SetDamage(lua_tinker::table TargetCreatureInfo, SkillType_t skillType, Damage_t Damage, lua_tinker::table CastCreatureInfo)
{
	Creature *pCastCreature = NULL;
	Creature *pTargetCreature = NULL;
	
	pCastCreature = CastCreatureInfo.get<Creature *>("_pCreature");	
	pTargetCreature = TargetCreatureInfo.get<Creature *>("_pCreature");
	
	m_pEffectScript->SetDamage(pTargetCreature, skillType, Damage, pCastCreature);
}

void DEScriptEffect::TraverseEffectRegion(ZoneCoord_t Left, ZoneCoord_t Top, ZoneCoord_t Width, ZoneCoord_t Height)
{
	m_pEffectScript->TraverseEffectRegion(Left, Top, Width, Height);
}

void DEScriptEffect::SetNextTime(Turn_t delay)
{
	m_pEffectScript->setNextTime(delay);
}

void DEScriptEffect::SetDeadLine(Turn_t delay)
{
	m_pEffectScript->setDeadline(delay);
}

ZoneCoord_t DEScriptEffect::GetBaseX()
{
	return m_pEffectScript->getX();
}

ZoneCoord_t DEScriptEffect::GetBaseY()
{
	return m_pEffectScript->getY();
}

void DEScriptEffect::BrocastGCAddEffectToTile(lua_tinker::table ZoneInfo, int iPosX, int iPosY, Duration_t duration)
{
	Zone *pZone = ZoneInfo.get<Zone *>("_pZone");
	
	if ( pZone == NULL )
	{
		return;
	}
	
	brocastGCAddEffectToTile(pZone, iPosX, iPosY, duration, m_pEffectScript);
}
	
void DEScriptEffect::NotifyInitialize()
{
	try
	{
		lua_tinker::call<void>(m_pLuaState, "OnInitialize");
	}
	catch(DEScriptNS::CallException& e )
	{
		// 스크립트에 해당 함수가 없는 경우
		
		// do nothing
	}		
}

void DEScriptEffect::NotifyAffect()
{
	try
	{
		lua_tinker::call<void>(m_pLuaState, "OnAffect");
	}
	catch(DEScriptNS::CallException& e )
	{
		// 스크립트에 해당 함수가 없는 경우
		
		// do nothing
	}		
}

void DEScriptEffect::NotifyUnaffect()
{
	try
	{
		lua_tinker::call<void>(m_pLuaState, "OnUnaffect");
	}
	catch(DEScriptNS::CallException& e )
	{
		// 스크립트에 해당 함수가 없는 경우
		
		// do nothing
	}
}

void DEScriptEffect::NotifyAffectCreature(Creature *pTargetCreature)
{
	try
	{
		lua_tinker::table TargetCreatureInfo(m_pLuaState);
	
		MakeCreatureInfo(TargetCreatureInfo, pTargetCreature);
	
		lua_tinker::call<void>(m_pLuaState, "OnAffectCreature", TargetCreatureInfo);
	}
	catch(DEScriptNS::CallException& e )
	{
		// 스크립트에 해당 함수가 없는 경우
		
		// do nothing
	}		
}

void DEScriptEffect::NotifyUnaffectCreature(Creature *pTargetCreature)
{
	try
	{
		lua_tinker::table TargetCreatureInfo(m_pLuaState);
	
		MakeCreatureInfo(TargetCreatureInfo, pTargetCreature);
	
		lua_tinker::call<void>(m_pLuaState, "OnUnaffectCreature", TargetCreatureInfo);
	}
	catch(DEScriptNS::CallException& e )
	{
		// 스크립트에 해당 함수가 없는 경우
		
		// do nothing
	}		
}

bool DEScriptEffect::NotifyTraverseTile(Zone *pZone, ZoneCoord_t TileX, ZoneCoord_t TileY)
{
	try
	{
		lua_tinker::table ZoneInfo(m_pLuaState);
		
		MakeZoneInfo(ZoneInfo, pZone);
		
		return lua_tinker::call<bool>(m_pLuaState, "OnTraverseTile", ZoneInfo, TileX, TileY);
	}
	catch(DEScriptNS::CallException& e )
	{
		// 스크립트에 해당 함수가 없는 경우
		
		return false;
	}
}
