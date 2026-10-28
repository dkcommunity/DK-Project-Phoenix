BossAISay = 
{
	AI = 
	"DIRECTIVE BEGIN\n"..
		"(ACTION:Say,493)\n"..
	"DIRECTIVE END\n",
	DeadAI = ""
}
BossAIFire = 
{
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:FindWeakEnemy)\n"..
		"	(ACTION:ChangeEnemy,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeInMissile)\n"..
		"(ACTION:UseSkill,SKILL_HETER_CHAKRAM,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeOutOfSight)\n"..
			"(ACTION:Forget)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = ""
}
BossAIWater = 
{
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:FindWeakEnemy)\n"..
		"	(ACTION:ChangeEnemy,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeInMissile)\n"..
		"(ACTION:UseSkill,SKILL_WIDE_ICE_HAIL,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeOutOfSight)\n"..
			"(ACTION:Forget)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = ""
}
BossAIEarth = 
{
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:FindWeakEnemy)\n"..
		"	(ACTION:ChangeEnemy,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeInMissile)\n"..
		"(ACTION:UseSkill,SKILL_METEOR_STRIKE,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeOutOfSight)\n"..
			"(ACTION:Forget)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = ""
}
BossAIPhysical = 
{
	AI = 
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:FindWeakEnemy)\n"..
		"	(ACTION:ChangeEnemy,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"(CONDITION:EnemyRangeMelee)\n"..
		"(ACTION:UseSkill,SKILL_ATTACK_MELEE,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
		"	(CONDITION:EnemyRangeMissile)\n"..
		"	(ACTION:Approach)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeOutOfSight)\n"..
			"(ACTION:Forget)\n"..
		"DIRECTIVE END\n",
		
	DeadAI = ""
}