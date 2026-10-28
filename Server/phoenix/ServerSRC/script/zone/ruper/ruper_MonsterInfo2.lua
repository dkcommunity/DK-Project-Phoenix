HellGrouper = {
	MType = 1067,
	Name = "헬그루퍼",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

ChiefShamanOaf = {
	MType = 597,
	Name = "치프샤먼오프",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

ChiefMumRimmon = {
	MType = 598,
	Name = "치프멈린몬",
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

ChiefGiantOs = {
	MType = 601,
	Name = "치프자이언트오스",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "DIRECTIVE BEGIN\n"..
			"(CONDITION:ImHiding)\n"..
			"(ACTION:UseSkill,SKILL_UN_BURROW,90)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:ImInBadPosition)\n"..
			"(ACTION:MoveRandom,80)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:PossibleSummonMonsters)\n"..
			"(ACTION:UseSkill,SKILL_SUMMON_MONSTERS,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotDoomed)\n"..
			"(ACTION:UseSkill,SKILL_DOOM,10)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyTileNotAcidSwamp)\n"..
			"(ACTION:UseSkill,SKILL_ACID_SWAMP,11)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotDeath)\n"..
			"(ACTION:UseSkill,SKILL_DEATH,12)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotSeduction)\n"..
			"(ACTION:UseSkill,SKILL_SEDUCTION,14)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_POISON_STORM,20)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_ACID_STORM,25)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_KNIFE,33)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_POISON_STRIKE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_ACID_STRIKE,100)\n"..
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
	Enhance = "(HP,-50)(TOHIT,-60)(DEFENSE,-60)(PROTECTION,-50)(DAMAGE,-55)"
}

ChiefMountCrag = {
	MType = 602,
	Name = "치프마운트크래그",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,-50)(TOHIT,-60)(DEFENSE,-60)(PROTECTION,-50)(DAMAGE,-100)"
}

ChiefLichJel = {
	MType = 603,
	Name = "치프리치젤",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "DIRECTIVE BEGIN\n"..
			"(CONDITION:ImHiding)\n"..
			"(ACTION:UseSkill,SKILL_UN_BURROW,90)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:ImInBadPosition)\n"..
			"(ACTION:MoveRandom,80)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:PossibleSummonMonsters)\n"..
			"(ACTION:UseSkill,SKILL_SUMMON_MONSTERS,60)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotParalyzed)\n"..
			"(ACTION:UseSkill,SKILL_PARALYZE,10)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyTileNotAcidSwamp)\n"..
			"(ACTION:UseSkill,SKILL_ACID_SWAMP,25)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotDeath)\n"..
			"(ACTION:UseSkill,SKILL_DEATH,20)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotDoomed)\n"..
			"(ACTION:UseSkill,SKILL_DOOM,12)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotSeduction)\n"..
			"(ACTION:UseSkill,SKILL_SEDUCTION,14)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_WAVE,16)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_KNIFE,20)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_POISON_STORM,25)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_ACID_STORM,33)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_ACID_STRIKE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_POISON_STRIKE,100)\n"..
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
	Enhance = "(HP,-50)(TOHIT,-60)(DEFENSE,-60)(PROTECTION,-50)(DAMAGE,-55)"
}

Trasla = {
	MType = 771,
	Name = "트라슬라",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

Pusca = {
	MType = 772,
	Name = "푸스카",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

NodCopila = {
	MType = 774,
	Name = "노드코피라",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

Razor_1 = {
	MType = 775,
	Name = "레이저",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

Razor_2 = {
	MType = 775,
	Name = "레이저",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,43)(TOHIT,-9)(DEFENSE,-70)(PROTECTION,-6)(MINDAMAGE,111)(MAXDAMAGE,103)"
}

GenuineBathory = {
	MType = 1069,
	Name = "진바토리",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "DIRECTIVE BEGIN\n"..
			"(CONDITION:TimingMasterBloodDrain)\n"..
			"(ACTION:UseSkill,SKILL_BLOOD_DRAIN,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:ImInBadPosition)\n"..
			"(ACTION:MoveRandom,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotParalyzed)\n"..
			"(ACTION:UseSkill,SKILL_PARALYZE,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:FindWeakEnemy)\n"..
			"(ACTION:ChangeEnemy,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_WARP,10)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotHallucination)\n"..
			"(ACTION:UseSkill,SKILL_HALLUCINATION,7)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(CONDITION:EnemyNotDoomed)\n"..
			"(ACTION:UseSkill,SKILL_DOOM,7)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(CONDITION:EnemyNotDeath)\n"..
			"(ACTION:UseSkill,SKILL_DEATH,7)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(CONDITION:EnemyNotSeduction)\n"..
			"(ACTION:UseSkill,SKILL_SEDUCTION,7)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_ICE_AUGER,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_WIDE_ICE_HAIL,30)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_ICE_LANCE,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_WALL,35)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_WAVE,20)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_MASTER_WAVE,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_STORM,80)\n"..
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
	Enhance = "(HP,800)(TOHIT,120)(DEFENSE,-60)(PROTECTION,30)(MINDAMAGE,324)(MAXDAMAGE,471)(DECREASEDAMAGE,45)(NEXTRATIO,50)"
}

CloneGussion = {
	MType = 1044,
	Name = "클론구시온",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

RuinGuardian = {
	MType = 1063,
	Name = "루인가디언",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

GenuineEstroider = {
	MType = 1064,
	Name = "진에스트로이더",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

LordAbyss = {
	MType = 1065,
	Name = "로드어비스",
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
			"(ACTION:UseSkill,SKILL_BLOODY_NAIL,50)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_ATTACK_MELEE,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(CONDITION:EnemyNotParalyzed)\n"..
			"(ACTION:UseSkill,SKILL_PARALYZE,10)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_KNIFE,50)\n"..
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
						"(TOHIT,400)"..
						"(DAMAGE,150)"..
						"(DEFENCE,-10)"..
						"(DECREASEDAMAGE,75)"
}

AgonyChild = {
	MType = 1072,
	Name = "아고니차일드",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = 
		"",

	DeadAI = "",
	Enhance = "(HP,600)(TOHIT,360)(DEFENSE,-60)(MINDAMAGE,279)(MAXDAMAGE,439)(DECREASEDAMAGE,45)"
}

AgonysHound = {
	MType = 1073,
	Name = "아고니의하운드",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = 
		"",

	DeadAI = "",
	Enhance = ""
}

Bigtore = {
	MType = 1076,
	Name = "빅토어",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = 
		"",

	DeadAI = "",
	Enhance = "(HP,200)(TOHIT,240)(DEFENSE,-60)(MINDAMAGE,279)(MAXDAMAGE,439)(DECREASEDAMAGE,45)"
}

Onibla = {
	MType = 1071,
	Name = "오니블라",
	X = 12,
	Y = 27,
	Dir = 2,

	AI =
		"",

	DeadAI = "",
	Enhance = ""
}

VillaninLady = {
	MType = 1074,
	Name = "빌라인레이디",
	X = 12,
	Y = 27,
	Dir = 2,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}

VillaninLadyGhost = {
	MType = 1074,
	Name = "빌라인레이디분신",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = 
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_VILLAIN_LADY_SELF_DESTRUCTION,100)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(ACTION:ApproachFast)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeOutOfSight)\n"..
			"(ACTION:Forget)\n"..
		"DIRECTIVE END",
	
	DeadAI = "",
	Enhance = ""
}

OniSpaniel = {
	MType = 1075,
	Name = "오니스패니얼",
	X = 12,
	Y = 27,
	Dir = 2,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}

BlackAstralMagicCrystal = {
	MType = 1077,
	Name = "블랙아스트랄",
	X = 12,
	Y = 27,
	Dir = 2,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}

WhiteAstralSpellCrystal = {
	MType = 1078,
	Name = "화이트아스트랄",
	X = 12,
	Y = 27,
	Dir = 2,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}
