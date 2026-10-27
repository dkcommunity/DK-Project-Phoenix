#ifndef _DESCRIPTEFFECT_H_
#define _DESCRIPTEFFECT_H_

#include "DEScript.h"
#include "PlayerCreature.h"
#include "EffectScript.h"
#include "Zone.h"

class EffectScript;

class DEScriptEffect : public DEScript
{
public :
	DEScriptEffect();
	virtual ~DEScriptEffect();
	
	void SetEffectScript(EffectScript *pEffectScript);
	
	void SetDamage(lua_tinker::table TargetCreatureInfo, SkillType_t skillType, Damage_t Damage, lua_tinker::table CastCreatureInfo);
	void SetNextTime(Turn_t delay);
	void SetDeadLine(Turn_t delay);
	void TraverseEffectRegion(ZoneCoord_t Left, ZoneCoord_t Top, ZoneCoord_t Width, ZoneCoord_t Height);
	
	ZoneCoord_t GetBaseX();
	ZoneCoord_t GetBaseY();
	
	void BrocastGCAddEffectToTile(lua_tinker::table ZoneInfo, int iPosX, int iPosY, Duration_t duration);
	
	void NotifyInitialize();
	void NotifyAffect();
	void NotifyUnaffect();
	void NotifyAffectCreature(Creature *pTargetCreature);
	void NotifyUnaffectCreature(Creature *pTargetCreature);
	bool NotifyTraverseTile(Zone *pZone, ZoneCoord_t TileX, ZoneCoord_t TileY);
		
protected :
	EffectScript *m_pEffectScript;
	
};

#endif /* _DESCRIPTEFFECT_H_ */
