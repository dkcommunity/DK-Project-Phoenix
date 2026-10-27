//////////////////////////////////////////////////////////////////////////////
// Filename    : HitRoll.h
// Written by  : excel96
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __HITROLL_H__
#define __HITROLL_H__

#include "ModifyInfo.h"

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////
class Creature;
class Slayer;
class Vampire;
class Ousters;
class Monster;
class SkillInfo;
class SlayerSkillSlot;
class VampireSkillSlot;
class OustersSkillSlot;

//////////////////////////////////////////////////////////////////////////////
// class HitRoll
// 각종 명중굴림 함수를 하나의 네임스페이스로 묶기 위해 만들어진 클래스이다.
//////////////////////////////////////////////////////////////////////////////

class HitRoll
{
public:
	// 일반 공격 명중굴림 함수
	static bool isSuccess(Creature* pAttacker, Creature* pDefender, int ToHitBonus = 0);
//	static bool isSuccess(ToHit_t ToHit, Creature* pDefender, int ToHitBonus = 0);
//#ifndef VERSION_SKILL_1
	// 슬레이어용 마법 명중굴림 함수
	static bool isSuccessMagic(Slayer* pSlayer, SkillInfo* pSkillInfo, SlayerSkillSlot* pSkillSlot);

	// 뱀파이어용 마법 명중굴림 함수
	static bool isSuccessMagic(Vampire* pVampire, SkillInfo* pSkillInfo, VampireSkillSlot* pVampireSkillSlot, int BonusPoint = 0);

	// 아우스터스용 마법 명중굴림 함수
	static bool isSuccessMagic(Ousters* pOusters, SkillInfo* pSkillInfo, OustersSkillSlot* pOustersSkillSlot, int BonusPoint = 0);

	// 몬스터용 마법 명중굴림 함수
	static bool isSuccessMagic(Monster* pMonster, SkillInfo* pSkillInfo);
//#else
	// 20070918 마법 명중률 함수
	static bool isSuccessMagic(Creature* pAttacker, Creature* pDefender, int BonusPoint = 0 );
//#endif

	// 뱀파이어 및 몬스터용 흡혈 명중굴림 함수
	static bool isSuccessBloodDrain(Creature* pAttacker, Creature* pDefender, int multiplier=3);

	// 저주 계열 마법 명중굴림 함수
	static bool isSuccessCurse(int MagicLevel, Resist_t resist);
	static bool isSuccessVampireCurse(int MagicLevel, Resist_t resist);

	// CurePoison 명중굴림 함수
	static bool isSuccessCurePoison(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio = 0);

	// CurePoison 명중굴림 함수. 새버전. 이제부터 이거 쓴다. 2005.11.30 by bezz
	static bool isSuccessNewCurePoison(int INTE, int skillType, int enemyINT);
	static bool isSuccessNewCurePoisonForOusters(int INTE, int skillType, int enemyINT);

	// Flare 명중굴림 함수
	static bool isSuccessFlare(Creature* pTargetCreature, int SkillLevel);

	// RemoveCurse 명중굴림 함수
	static bool isSuccessRemoveCurse(int Base, int SkillLevel, int Difficulty, int MagicLevel, int MinRatio = 0);

	// Rebuke 명중굴림 함수
	static bool isSuccessRebuke(Slayer* pSlayer, SlayerSkillSlot* pSkillSlot, Creature* pDefender);

	// Magic Elusion 명중굴림 함수
	static bool isSuccessMagicElusion(Slayer* pSlayer);

	// Poison Mesh 명중굴림 함수
	static bool isSuccessPoisonMesh(Vampire* pVampire);
	static bool isSuccessCHAINOFDEMON(Vampire* pVampire);
	static bool isSuccessRAGEOFBLOOD(Vampire* pVampire);

	// Illusion Of Avenge 명중굴림 함수
	static bool isSuccessIllusionOfAvenge(Slayer *pSlayer);

	// Will Of Life 명중굴림 함수
	static bool isSuccessWillOfLife(Vampire* pVampire);

	// 크리티컬 히트 롤을 수행한다.
	static bool isCriticalHit(Creature* pCreature, int CriticalBonus=0);

	static bool isMagicCriticalHit(Creature* pCreature, int Bonus=0);

	// 슬레이어 셀프 스킬인가를 체크하는 함수
	static bool isSlayerSelfSkill( SkillType_t skillType );
	static bool isOustersSelfSkill( SkillType_t skillType );

	// 할루 성공률
	static bool isSuccessHallucination( Vampire* pAttacker, Creature* pTarget );

	// 백스탭 성공률
	static bool isSuccessBackStab( Ousters* pAttacker );
};

#endif
