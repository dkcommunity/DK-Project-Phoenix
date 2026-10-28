BalBerithInfo =
{
	MType = 1032,
	Name = "발베리드",
	X = 0,
	Y = 0,
	Dir = 2,
	AI =	"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,80)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_TIFFAUGES_BALBERITH_RED,20)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMissile)\n"..
				"	(ACTION:UseSkill,SKILL_TIFFAUGES_BALBERITH_RED,10)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"(CONDITION:EnemyRangeMissile)\n"..
				"(ACTION:Approach)\n"..
				"DIRECTIVE END\n"..		
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeOutOfSight)\n"..
				"	(ACTION:Forget)\n"..
				"DIRECTIVE END",
	DeadAI = "",
	Enhance = "(EXP,400)"..
						"(HP,100)"..
						"(TOHIT,27)"..
						"(DEFENCE,-60)"..
						"(DAMAGE,-50)"..
						"(DECREASEDAMAGE,15)"
}

BerithInfo =
{
	MType = 1033,
	Name = "베리드",
	X = 0,
	Y = 0,
	Dir = 2,
	AI =	"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,80)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_TIFFAUGES_BERITH_RED,20)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMissile)\n"..
				"	(ACTION:UseSkill,SKILL_TIFFAUGES_BERITH_RED,10)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"(CONDITION:EnemyRangeMissile)\n"..
				"(ACTION:Approach)\n"..
				"DIRECTIVE END\n"..		
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeOutOfSight)\n"..
				"	(ACTION:Forget)\n"..
				"DIRECTIVE END",		
	DeadAI = "",
	Enhance = "(EXP,400)"..
						"(HP,100)"..
						"(TOHIT,36)"..
						"(DEFENCE,-60)"..
						"(DAMAGE,-50)"..
						"(DECREASEDAMAGE,15)"
}

ValkyrjaInfo =
{
	MType = 1034,
	Name = "발퀴레",
	X = 0,
	Y = 0,
	Dir = 2,
	AI =	"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,80)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_RED,20)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMelee)\n"..
				"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeMissile)\n"..
				"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_RED,10)\n"..
				"DIRECTIVE END\n"..
				"DIRECTIVE BEGIN\n"..
				"(CONDITION:EnemyRangeMissile)\n"..
				"(ACTION:Approach)\n"..
				"DIRECTIVE END\n"..		
				"DIRECTIVE BEGIN\n"..
				"	(CONDITION:EnemyRangeOutOfSight)\n"..
				"	(ACTION:Forget)\n"..
				"DIRECTIVE END",	
	DeadAI = "",
	Enhance = "(EXP,400)"..
						"(HP,100)"..
						"(TOHIT,8)"..
						"(DEFENCE,-60)"..
						"(DAMAGE,-50)"..
						"(DECREASEDAMAGE,15)"
}

CloneGusionBoss = {
	MType = 1044,
	Name = "클론구시온",
	X = 75,
	Y = 88,
	Dir = 2,

	AI =
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImInBadPosition)\n"..
		"	(ACTION:MoveRandom,10)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:FindWeakEnemy)\n"..
		"	(ACTION:ChangeEnemy,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_RED,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_RED,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_A_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",

	DeadAI = "",
	Enhance = "(EXP,500)"..
						"(HP,700)"..
						"(TOHIT,-23)"..
						"(DAMAGE,200)"..
						"(DECREASEDAMAGE,80)"
}