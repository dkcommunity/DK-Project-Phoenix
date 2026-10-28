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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BALBERITH_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BALBERITH_BLUE,40)\n"..
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
	Enhance = "(EXP,300)"..
						"(HP,150)"..
						"(TOHIT,27)"..
						"(DEFENCE,-20)"..
						"(DECREASEDAMAGE,40)"
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BERITH_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_BERITH_BLUE,40)\n"..
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
	Enhance = "(EXP,300)"..
						"(HP,150)"..
						"(TOHIT,36)"..
						"(DEFENCE,-20)"..
						"(DECREASEDAMAGE,45)"
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_ATTACK_MELEE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_BLUE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_VALKYRJA_BLUE,20)\n"..
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
	Enhance = "(EXP,300)"..
						"(HP,150)"..
						"(TOHIT,8)"..
						"(DEFENCE,-20)"..
						"(DECREASEDAMAGE,50)"
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_PAIMON_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_PAIMON_BLUE,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_PAIMON_BLUE,40)\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeMissile)\n"..
		"(ACTION:Approach)\n"..
		"DIRECTIVE END\n"..		
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeOutOfSight)\n"..
		"	(ACTION:Forget)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,300)"..
						"(HP,150)"..
						"(TOHIT,16)"..
						"(DEFENCE,-20)"..
						"(DECREASEDAMAGE,45)"
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_BLUE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_BLUE,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_A_BLUE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_A_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GOMORY_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeMissile)\n"..
		"(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,500)"..
						"(HP,400)"..
						"(TOHIT,-8)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,65)"
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_BLUE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_BLUE,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_A_BLUE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_A_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_KRSNIK_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeMissile)\n"..
		"(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,500)"..
						"(HP,400)"..
						"(TOHIT,-9)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,65)"
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_BLUE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_A_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,500)"..
						"(HP,600)"..
						"(TOHIT,-47)"..
						"(DAMAGE,150)"..
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_BLUE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_A_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_GUSION_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,500)"..
						"(HP,600)"..
						"(TOHIT,-23)"..
						"(DAMAGE,150)"..
						"(DEFENCE,-10)"..
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_BLUE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_BLUE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_A_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,500)"..
						"(HP,600)"..
						"(TOHIT,-40)"..
						"(DAMAGE,150)"..
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
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_BLUE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:ImDying)\n"..
		"	(CONDITION:EnemyRangeInMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_BLUE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_B_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_A_BLUE,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMelee)\n"..
		"	(ACTION:UseSkill,SKILL_TIFFAUGES_MORGOTH_C_BLUE,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = "",
	Enhance = "(EXP,500)"..	
						"(HP,600)"..
						"(TOHIT,-30)"..
						"(DAMAGE,150)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,75)"
}