
DoorLeftModel = {
	MType = 1704,
	Name = "×ó±ßÃÅ",
	X = 44,
	Y = 197,
	Dir = 4,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}

Door_1714 = {
	MType = 1714,
	Name = "ÉÏÃæµÄÃÅ",
	X = 44,
	Y = 197,
	Dir = 4,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}



Monster_779 = {
	MType = 779,
	Name = "Ë«×ÓÕÙ»½µÄ¹Ö",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

Monster_1613 = {
	MType = 1683,
	Name = "Ê¯Ïñ¹í",
	X = 12,
	Y = 27,
	Dir = 2,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}



ChiefShamanOaf = {
	MType = 597,
	Name = "Ä¡ÇÁ»þ¸Õ¿ÀÇÁ",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

ChiefMumRimmon = {
	MType = 598,
	Name = "Ä¡ÇÁ¸Ø¸°¸ó",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "DIRECTIVE BEGIN\n"..
			"(CONDITION:ImHiding)\n"..
			"(ACTION:UseSkill,SKILL_UN_BURROW,90)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(CONDITION:ImBat)\n"..
			"(ACTION:UseSkill,SKILL_UN_TRANSFORM,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:TimingBloodDrain)\n"..
			"(ACTION:UseSkill,SKILL_BLOOD_DRAIN,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_NAIL,70)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_ATTACK_MELEE,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(CONDITION:EnemyNotDoomed)\n"..
			"(ACTION:UseSkill,SKILL_DOOM,10)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(CONDITION:EnemyNotSeduction)\n"..
			"(ACTION:UseSkill,SKILL_SEDUCTION,10)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:ImWalkingWall)\n"..
			"(ACTION:UseSkill,SKILL_TRANSFORM_TO_BAT,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(ACTION:Approach)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeOutOfSight)\n"..
			"(ACTION:Forget)\n"..
		"DIRECTIVE END",

	DeadAI = "",
	Enhance = ""
}

