ChiefShamanOaf = {
	MType = 597,
	Name = "치프샤먼오프",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,-50)(TOHIT,-60)(DEFENSE,-60)(PROTECTION,-55)(DAMAGE,-60)"
}

ChiefMumRimmon = {
	MType = 598,
	Name = "피츠멈린몬",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,-50)(TOHIT,-69)(DEFENSE,-82)(PROTECTION,-55)(DAMAGE,15)"
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
	Enhance = "(HP,-50)(TOHIT,-60)(DEFENSE,-60)(PROTECTION,-50)(DAMAGE,-50)"
}

ChiefMountCrag = {
	MType = 602,
	Name = "치프마운트크래그",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,-50)(TOHIT,-60)(DEFENSE,-60)(PROTECTION,-50)(DAMAGE,-50)"
}

ChiefLichJel = {
	MType = 603,
	Name = "치프리치젤",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,-50)(TOHIT,-60)(DEFENSE,-60)(PROTECTION,-55)(DAMAGE,-60)"
}

Trasla = {
	MType = 771,
	Name = "트라슬라",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,140)(TOHIT,-46)(DEFENSE,-78)(PROTECTION,83)(MINDAMAGE,211)(MAXDAMAGE,136)"
}

Pusca = {
	MType = 772,
	Name = "푸스카",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,54)(TOHIT,-32)(DEFENSE,-74)(PROTECTION,21)(MINDAMAGE,100)(MAXDAMAGE,54)"
}

NodCopila = {
	MType = 774,
	Name = "노드코피라",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,213)(TOHIT,-43)(DEFENSE,-77)(PROTECTION,91)(MINDAMAGE,145)(MAXDAMAGE,139)"
}

Razor = {
	MType = 775,
	Name = "레이저",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = "(HP,43)(TOHIT,-9)(DEFENSE,-70)(PROTECTION,-1)(MINDAMAGE,96)(MAXDAMAGE,88)"
}

GenuineTepez = {
	MType = 1070,
	Name = "진테페즈",
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
			"(CONDITION:FindWeakEnemy)\n"..
			"(ACTION:ChangeEnemy,15)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(CONDITION:EnemyNotParalyzed)\n"..
			"(ACTION:UseSkill,SKILL_PARALYZE,15)\n"..
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
			"(ACTION:UseSkill,SKILL_FEEBLE_VIRUS,20)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_CHOPPING_FIREWOOD,40)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMissile)\n"..
			"(ACTION:UseSkill,SKILL_CHAIN_THROWING_AXE,20)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeInMissile)\n"..
			"(ACTION:UseSkill,SKILL_GUN_SHOT_GUIDANCE,20)\n"..
		"DIRECTIVE END\n"..
		"DIRECTIVE BEGIN\n"..
			"(CONDITION:EnemyRangeMelee)\n"..
			"(ACTION:UseSkill,SKILL_BLOODY_WAVE,30)\n"..
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
	Enhance = "(HP,800)(TOHIT,34)(DEFENSE,-60)(PROTECTION,30)(MINDAMAGE,309)(MAXDAMAGE,469)(DECREASEDAMAGE,45)(NEXTRATIO,50)"
}

CloneMorgoth = {
	MType = 1045,
	Name = "클론모르고스",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

GrudgeWidows = {
	MType = 1066,
	Name = "그루지위도우즈",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

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

PitchBlackWizard = {
	MType = 1068,
	Name = "피치블랙위자드",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

RuinGuardian = {
	MType = 470,
	Name = "루이가디언",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

GenuineEstroider = {
	MType = 473,
	Name = "진에스트로이더",
	X = 12,
	Y = 27,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}

LordAbyss = {
	MType = 478,
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