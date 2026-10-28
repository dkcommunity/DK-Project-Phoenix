BalBerithInfo =
{
	MType = 1032,
	Name = "발베리드",
	X = 0,
	Y = 0,
	Dir = 2,
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BALBERITH_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BALBERITH_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BALBERITH_RED,30)\n"..
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
	Enhance = "(EXP,600)"..
						"(HP,150)"..
						"(TOHIT,27)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,45)"
}

BerithInfo =
{
	MType = 1033,
	Name = "베리드",
	X = 0,
	Y = 0,
	Dir = 2,
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BERITH_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BERITH_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BERITH_RED,30)\n"..
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
	Enhance = "(EXP,600)"..
						"(HP,150)"..
						"(TOHIT,36)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,50)"
}

ValkyrjaInfo =
{
	MType = 1034,
	Name = "발퀴레",
	X = 0,
	Y = 0,
	Dir = 2,
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_RED,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_RED,40)\n"..
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
	Enhance = "(EXP,600)"..
						"(HP,150)"..
						"(TOHIT,8)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,55)"
}

PaimonInfo = 
{
	MType = 1039,
	Name = "파이몬",
	X = 0,
	Y = 0,	
	Dir = 2,
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_PAIMON_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_PAIMON_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_PAIMON_RED,50)\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeMissile)\n"..
		"(ACTION:Approach)\n"..
		"DIRECTIVE END\n"..		
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeOutOfSight)\n"..
		"	(ACTION:Forget)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,600)"..
						"(HP,150)"..
						"(TOHIT,16)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,50)"
}

GomoryInfo = 
{
	MType = 1035,
	Name = "고모리",
	X = 0,
	Y = 0,	
	Dir = 2,
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_RED,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_A_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_A_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeMissile)\n"..
		"(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,800)"..
						"(HP,500)"..
						"(TOHIT,-8)"..
						"(DECREASEDAMAGE,70)"
}

KrsnikInfo = 
{
	MType = 1036,
	Name = "크루소닉",
	X = 0,
	Y = 0,	
	Dir = 2,
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_RED,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_A_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_A_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeMissile)\n"..
		"(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,800)"..
						"(HP,500)"..
						"(TOHIT,-9)"..
						"(DECREASEDAMAGE,70)"
}

GusionInfo = 
{
	MType = 1037,
	Name = "구시온",
	X = 0,
	Y = 0,	
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_RED,60)\n"..
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
	Enhance = "(EXP,800)"..
						"(HP,700)"..
						"(TOHIT,-47)"..
						"(DAMAGE,250)"..
						"(DECREASEDAMAGE,75)"		
}

CloneGusionInfo = 
{
	MType = 1044,
	Name = "클론구시온",
	X = 0,
	Y = 0,	
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_RED,60)\n"..
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,800)"..
						"(HP,700)"..
						"(TOHIT,-23)"..
						"(DAMAGE,200)"..
						"(DECREASEDAMAGE,75)"
}

MorgothInfo = 
{
	MType = 1040,
	Name = "모르고스",
	X = 0,
	Y = 0,	
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_RED,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_A_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,800)"..
						"(HP,700)"..
						"(TOHIT,-40)"..
						"(DAMAGE,250)"..
						"(DECREASEDAMAGE,75)"
}

CloneMorgothInfo = 
{
	MType = 1045,
	Name = "클론모르고스",
	X = 0,
	Y = 0,	
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_RED,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_RED,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_A_RED,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_RED,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,800)"..	
						"(HP,700)"..
						"(TOHIT,-30)"..
						"(DAMAGE,200)"..
						"(DECREASEDAMAGE,75)"
}