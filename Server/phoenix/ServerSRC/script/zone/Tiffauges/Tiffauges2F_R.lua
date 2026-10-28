dofile("../script/zone/Tiffauges/Tiffauges2F_Restrict.lua")
dofile("../script/zone/Tiffauges/Tiffauges2F_Util.lua")
dofile("../script/zone/Tiffauges/Tiffauges2F_TrapPos.lua")
dofile("../script/zone/Tiffauges/Tiffauges2F_R_MonsterInfo.lua")

MaxScriptZoneCount = 10
DoungeonOpened = false
WarpToOtherWorldZoneID = 0

strMissionRoomHiddenStartScript = "大老远的跑来，让我看看你的水平"
strMissionRoomHiddenKillScript = "啊！不可能，我会回来的！"

strMissionRoomBossStartScript = "哈哈，哥哥又回来了，看我教训你."
strMissionRoomBossKillScript = "(sigh).. 老子跟你没完，等到."

EffectGateID_NW = 580
EffectGateID_NE = 581

CurrentMissionRoom = 0	-- 矫累 傈
DoungeonStartTime = 0	-- 0 seconds
DoungeonLimitTime = 60 * 60 -- 60 minutes
DoungeonClear = false 
DoungeonTrapType = 1	-- 1 : 泅技飘乏, 2 : 捞拌飘乏

MissionRoomHiddenEnable = false
MissionRoomHiddenGusionScript1Enable = false
MissionRoomHiddenGusionObjectID = 0
MissionRoomHiddenMonsterInfo = CloneMorgothInfo
MissionRoomHiddenMonsterPrevHP = -1
MissionRoomHiddenMonsterWarpHP = 5000

MissionRoomBossGusionScript1Enable = false
MissionRoomBossGusionObjectID = 0
MissionRoomBossMonsterInfo = MorgothInfo
MissionRoomBossMonsterPrevHP = -1
MissionRoomBossMonsterWarpHP = 6000

MissionRoomSummonList = {}

MissionRoom1MonsterList = {}
MissionRoom2MonsterList = {}
MissionRoom3MonsterList = {}
MissionRoomHiddenMonsterList = {}
MissionRoom4MonsterList = {}
MissionRoom5MonsterList = {}
MissionRoom6MonsterList = {}
MissionRoomBossMonsterList = {}

MissionRoom1_MonsterPos = 
{
	{
		MonsterInfo = BalBerithInfo,
		MonsterPos = 
		{
			{118,91},
			{118,95},
			{121,93},
			{121,99},
			{124,96},
			{109,81},
			{113,79},
			{115,75}
		}
	},
	{
		MonsterInfo = BerithInfo,
		MonsterPos =
		{
			{105,77},
			{109,73},
			{112,70},
			{115,67},
			{104,70},
			{108,65},
			{111,61}
		}
	}
}

MissionRoom1_BlockedPos =
{
	GatePos = {102, 64},
	
	{106,59},
	{106,60},
	{105,59},
	{105,60},
	{105,61},
	{104,60},
	{104,61},
	{104,62},
	{103,61},
	{103,62},
	{103,63},
	{102,62},
	{102,63},
	{102,64}
	
}

MissionRoom2_MonsterPos =
{
	{
		MonsterInfo = PaimonInfo,
		MonsterPos = 
		{
			{89,48},
			{92,49},
			{96,48},
			{97,43},
			{97,38}
		}
	},
	{
		MonsterInfo = ValkyrjaInfo,
		MonsterPos =
		{
			{89,39},
			{92,35}
		},
		SummonFunc = function (AttackerCreature, DefenderCreature)
			if ( CalcRemainHPRate(DefenderCreature) < 50 ) then
				DEScriptZone:BroadcastGCSay(DefenderCreature, "哎哟喂，你太弱了，看看哥哥我的力量!",  16777215)  -- 16777215 = 0x00FFFFFF
				
				SummonMonsters(AttackerCreature, DefenderCreature, BalBerithInfo, 5, 3)
				
				MissionRoomSummonList[DefenderCreature.ObjectID] = nil
			end
		end
	},
}

MissionRoom2_BlockedPos =
{
	GatePos = {78, 40},
	
	{82,35},
	{82,36},
	{81,35},
	{81,36},
	{81,37},
	{80,36},
	{80,37},
	{80,38},
	{79,37},
	{79,38},
	{79,39},
	{78,38},
	{78,39},
	{78,40}
}

MissionRoom3_MonsterPos =
{
	{
		MonsterInfo = BalBerithInfo,
		MonsterPos =
		{
			{73,20},
			{75,24},
			{68,22},
			{70,27},
			{62,23},
			{64,27},
			{66,31}
		}
	},
	{
		MonsterInfo = PaimonInfo,
		MonsterPos = 
		{
			{55,29},
			{58,33},
			{61,36}
		}
	},
	{
		MonsterInfo = GomoryInfo,
		MonsterPos = 
		{
			{58,33},
			{58,37},
			{55,32}
		},
		SummonFunc = function (AttackerCreature, DefenderCreature)
			if ( CalcRemainHPRate(DefenderCreature) < 50 ) then
				DEScriptZone:BroadcastGCSay(DefenderCreature, "滚出我的领地!",  16777215)  -- 16777215 = 0x00FFFFFF
				
				SummonMonsters(AttackerCreature, DefenderCreature, ValkyrjaInfo, 3, 3)
				
				MissionRoomSummonList[DefenderCreature.ObjectID] = nil
			end
		end
	}
}

MissionRoom3_BlockedPos =
{
	GatePos = {38, 49},
	
	{41,50},
	{41,51},
	{41,52},
	{40,50},
	{40,51},
	{39,49},
	{39,50},
	{39,51},
	{38,48},
	{38,49},
	{38,50},
	{37,48},
	{37,49}
}

	
MissionRoomHidden_MonsterPos =
{
	{
		MonsterInfo = MissionRoomHiddenMonsterInfo,
		MonsterPos = 
		{
			{18, 24}
		},
		SummonFunc = function (AttackerCreature, DefenderCreature)
			if ( CalcRemainHPRate(DefenderCreature) < 50 ) then
				DEScriptZone:BroadcastGCSay(DefenderCreature, "这次玩真的了，识相的赶紧滚!",  16777215)  -- 16777215 = 0x00FFFFFF
				
				SummonMonsters(AttackerCreature, DefenderCreature, BerithInfo, 15, 8)
				
				DEScript:SetHP(DefenderCreature, DefenderCreature.MaxHP*0.8)
				
				MissionRoomSummonList[DefenderCreature.ObjectID] = nil
			end
		end
	},
	{
		MonsterInfo = PaimonInfo,
		MonsterPos =
		{
			{17,29},
			{20,28},
			{22,23}
		}
	}
}
		
 function MissionRoom4_GomorySummonFunc1(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 70 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "这次玩真的了，识相的赶紧滚!",  16777215)  -- 16777215 = 0x00FFFFFF
		
		SummonMonsters(AttackerCreature, DefenderCreature, BalBerithInfo, 6, 5)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = MissionRoom4_GomorySummonFunc2
	end
end
	
function MissionRoom4_GomorySummonFunc2(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 50 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "嘿哟，你比我想象的强那么一萝卜丝，再来再来!",  16777215)  -- 16777215 = 0x00FFFFFF
		
		SummonMonsters(AttackerCreature, DefenderCreature, BerithInfo, 6, 5)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = MissionRoom4_GomorySummonFunc3
	end
end
		
function MissionRoom4_GomorySummonFunc3(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 30 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "我XXX，小子们出来吃唐僧肉了!",  16777215)  -- 16777215 = 0x00FFFFFF
		
		SummonMonsters(AttackerCreature, DefenderCreature, ValkyrjaInfo, 12, 6)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = nil
	end
end
				
MissionRoom4_MonsterPos =
{
	{
		MonsterInfo = GomoryInfo,
		MonsterPos =
		{
			{29,59}
		},
		SummonFunc = MissionRoom4_GomorySummonFunc1,
	}
}

MissionRoom4_BlockedPos =
{
	GatePos = {34, 84},
	
	{38,80},
	{38,81},
	{37,80},
	{37,81},
	{37,82},
	{36,81},
	{36,82},
	{36,83},
	{35,82},
	{35,83},
	{35,84},
	{34,83},
	{34,84}
}

MissionRoom5_MonsterPos =
{
	{
		MonsterInfo = KrsnikInfo,
		MonsterPos =
		{
			{36,95},
			{40,92},
			{41,97},
			{42,104},
			{45,102}
		},
		SummonFunc = function (AttackerCreature, DefenderCreature)
			if ( CalcRemainHPRate(DefenderCreature) < 50 ) then
				DEScriptZone:BroadcastGCSay(DefenderCreature, "太好了，大老远的送肉给俺吃!",  16777215)  -- 16777215 = 0x00FFFFFF
				
				SummonMonsters(AttackerCreature, DefenderCreature, PaimonInfo, 3, 2)
				
				MissionRoomSummonList[DefenderCreature.ObjectID] = nil
			end
		end
	}
}

MissionRoom5_BlockedPos =
{
	GatePos = {58, 116},
	
	{62,111},
	{62,112},
	{61,111},
	{61,112},
	{61,113},
	{60,112},
	{60,113},
	{60,114},
	{59,113},
	{59,114},
	{59,115},
	{58,114},
	{58,115}
	
}

function MissionRoom6_KrsnikSummonFunc1(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 70 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "这次玩真的了，识相的赶紧滚!",  16777215)  -- 16777215 = 0x00FFFFFF
		
		SummonMonsters(AttackerCreature, DefenderCreature, BalBerithInfo, 6, 3)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = MissionRoom6_KrsnikSummonFunc2
	end
end
	
function MissionRoom6_KrsnikSummonFunc2(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 50 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "你都这点能耐？都耍一遍我瞧瞧!",  16777215)  -- 16777215 = 0x00FFFFFF
		
		SummonMonsters(AttackerCreature, DefenderCreature, BerithInfo, 12, 6)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = MissionRoom6_KrsnikSummonFunc3
	end
end
		
function MissionRoom6_KrsnikSummonFunc3(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 30 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "眼睛睁大了，这才是我真正的实力!",  16777215)  -- 16777215 = 0x00FFFFFF
		
		SummonMonsters(AttackerCreature, DefenderCreature, GomoryInfo, 15, 8)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = nil
	end
end

MissionRoom6_MonsterPos =
{
	{
		MonsterInfo = KrsnikInfo,
		MonsterPos =
		{
			{84,120}
		},
		SummonFunc = MissionRoom6_KrsnikSummonFunc1
	}
}

MissionRoom6_BlockedPos1 =
{
	GatePos = {86, 104},
	
	{90,100},
	{90,101},
	{89,100},
	{89,101},
	{89,102},
	{88,101},
	{88,102},
	{88,103},
	{87,102},
	{87,103},
	{87,104},
	{86,103},
	{86,104}
}

MissionRoom6_BlockedPos2 =
{
	GatePos = {102, 128},
	
	{106,124},
	{106,125},
	{105,124},
	{105,125},
	{105,106},
	{104,125},
	{104,126},
	{104,127},
	{103,126},
	{103,127},
	{103,128},
	{102,127},
	{102,128}
}

function MissionRoomBoss_MoveFast(DefenderCreature)
	dx = math.random(4, 6)
	if math.random(0, 1) == 0 then 
		dx = dx * -1 
	end
	
	dy = math.random(4, 6)
	if math.random(0, 1) == 0 then 
		dy = dy * -1 
	end
	
	MoveFastPos =
	{
		X1 = DefenderCreature.X,
		Y1 = DefenderCreature.Y,
		X2 = DefenderCreature.X + dx,
		Y2 = DefenderCreature.Y + dy,
		SkillType = 0 
	}
	
	DEScriptZone:MoveFastMonster(DefenderCreature, MoveFastPos)
end

function MissionRoomBoss_KrsnikSummonFunc1(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 70 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "It wont be easy this time. I will show you my power!",  16777215)  -- 16777215 = 0x00FFFFFF
		SummonMonsters(AttackerCreature, DefenderCreature, ValkyrjaInfo, 6, 3)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = MissionRoomBoss_KrsnikSummonFunc2
	end
end
	
function MissionRoomBoss_KrsnikSummonFunc2(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 50 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "Is that all you got.. Show me your power!",  16777215)  -- 16777215 = 0x00FFFFFF
		SummonMonsters(AttackerCreature, DefenderCreature, GomoryInfo, 12, 6)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = MissionRoomBoss_KrsnikSummonFunc3
	end
end
		
function MissionRoomBoss_KrsnikSummonFunc3(AttackerCreature, DefenderCreature)
	if ( CalcRemainHPRate(DefenderCreature) < 30 ) then
		DEScriptZone:BroadcastGCSay(DefenderCreature, "捞巴捞 唱狼 柳沥茄 塞捞促",  16777215)  -- 16777215 = 0x00FFFFFF

		SummonMonsters(AttackerCreature, DefenderCreature, KrsnikInfo, 25, 8)

		DEScript:SetHP(DefenderCreature, DefenderCreature.MaxHP*0.8)
		
		MissionRoomSummonList[DefenderCreature.ObjectID] = nil
	end
end

MissionRoomBoss_MonsterPos =
{
	{
		MonsterInfo = MissionRoomBossMonsterInfo,
		MonsterPos =
		{
			{73,79}
		},
		SummonFunc = MissionRoomBoss_KrsnikSummonFunc1
	},
	{
		MonsterInfo = GomoryInfo,
		MonsterPos =
		{
			{74,85},
			{78,79},
			{70,83}
		},
	},
	{
		MonsterInfo = KrsnikInfo,
		MonsterPos =
		{
			{77,84},
			{70,78},
			{73,73}
		},
	}
}
		

TrapActivationInfoList = 
{
	-- MissionRoom1
	{
	},
	-- MissionRoom2
	{
		Stage = 0,
		TrapPosList = MissionRoom2_TrapPosList,
		
		ActivationTime =
		{
			{10, 30},
			{5, 20}
		}
	},
	-- MissionRoom3
	{
	},
	-- MissionRoom4
	{
		Stage = 0,
		TrapPosList = MissionRoom4_TrapPosList,
		
		ActivationTime =
		{
			{10, 30},
			{5, 20}
		}
	},
	-- MissionRoom5
	{
		Stage = 0,
		TrapPosList = MissionRoom5_TrapPosList,
		
		ActivationTime =
		{
			{10, 30},
			{5, 20}
		}
	},
	-- MissionRoom6
	{
	},
	-- MissionRoomBoss
	{
		Stage = 0,
		TrapPosList = MissionRoomBoss_TrapPosList,
		
		ActivationTime =
		{
			{10,30},
			{5,20},
			{5,10}
		}
	},
}

function StartMissionRoom1()

	CurrentMissionRoom = 1
	
	for i, MonsterArrangeInfo in pairs(MissionRoom1_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
		
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom1MonsterList, AddedMonster.ObjectID)
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end	
	end

end

function StartMissionRoom2()

	DEScriptZone:BroadcastSystemMessageInZone(0, "Lock of the first epistle is being lifted.")
	OpenGate(MissionRoom1_BlockedPos)

	CurrentMissionRoom = 2
	
	for i, MonsterArrangeInfo in pairs(MissionRoom2_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
	
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom2MonsterList, AddedMonster.ObjectID)
			
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end
	end

	StartToIncreaseTrapStage(60 * 3)	-- 3 minutes
	
end

function StartMissionRoom3()
	DEScriptZone:BroadcastSystemMessageInZone(0, "Lock of the second epistle is being lifted.")
	OpenGate(MissionRoom2_BlockedPos)
	
	CurrentMissionRoom = 3

	for i, MonsterArrangeInfo in pairs(MissionRoom3_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
	
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom3MonsterList, AddedMonster.ObjectID)
			
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end
	end
end

function StartMissionRoomHidden()

	MissionRoomHiddenEnable = true
	
	for i, MonsterArrangeInfo in pairs(MissionRoomHidden_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
	
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			if AddMonsterInfo.MType == 1045 then
				MissionRoomHiddenGusionObjectID = AddedMonster.ObjectID
			end
			
			table.insert(MissionRoomHiddenMonsterList, AddedMonster.ObjectID)
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end
	end
end

function StartMissionRoom4()
	DEScriptZone:BroadcastSystemMessageInZone(0, "Lock of the third epistle is being lifted.")
	OpenGate(MissionRoom3_BlockedPos)
	
	CurrentMissionRoom = 4
	
	StartMissionRoomHidden()

	for i, MonsterArrangeInfo in pairs(MissionRoom4_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
	
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom4MonsterList, AddedMonster.ObjectID)
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end
	end
	
	StartToIncreaseTrapStage(60 * 3)	-- 3 minutes
end

function StartMissionRoom5()
	DEScriptZone:BroadcastSystemMessageInZone(0, "Lock of the fourth epistle is being lifted.")
	OpenGate(MissionRoom4_BlockedPos)
	
	CurrentMissionRoom = 5
	
	for i, MonsterArrangeInfo in pairs(MissionRoom5_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
	
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom5MonsterList, AddedMonster.ObjectID)
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end
	end
	
	StartToIncreaseTrapStage(60 * 3)	-- 3 minutes
end

function StartMissionRoom6()
	DEScriptZone:BroadcastSystemMessageInZone(0, "Lock of the fifth epistle is being lifted.")
	OpenGate(MissionRoom5_BlockedPos)
	
	CurrentMissionRoom = 6

	for i, MonsterArrangeInfo in pairs(MissionRoom6_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
	
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom6MonsterList, AddedMonster.ObjectID)
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end
	end
	
end

function StartMissionRoomBoss()
	DEScriptZone:BroadcastSystemMessageInZone(0, "Lock of the final epistle is being lifted.")
	OpenGate(MissionRoom6_BlockedPos1)
	OpenGate(MissionRoom6_BlockedPos2)
	
	CurrentMissionRoom = 7
	
	for i, MonsterArrangeInfo in pairs(MissionRoomBoss_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
	
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			if AddedMonster.MType == MissionRoomBossMonsterInfo.MType  then
				MissionRoomBossGusionObjectID = AddedMonster.ObjectID
			end
			
			table.insert(MissionRoomBossMonsterList, AddedMonster.ObjectID)
			if MonsterArrangeInfo.SummonFunc ~= nil then
				MissionRoomSummonList[AddedMonster.ObjectID] = MonsterArrangeInfo.SummonFunc
			end
		end
	end
	
	StartToIncreaseTrapStage(60 * 3)	-- 3 minutes
end

function StartMissionSuccess()
	DoungeonClear = true	
	
	OnTraverseCreature = OnGiveRewardItem
	DEScriptZone:TraverseCreature()
	
	DEScriptZone:BroadcastSystemMessageInZone(0, "You will be teleported to Tiffauges Castle after 5 seconds.")
	DEScriptZone:SetTimer(5, 1000 * 5, 1)

end

function StartMissionFailed()
	-- 10檬饶 萍葛福 龋荐肺 捞悼
	
	DEScriptZone:BroadcastSystemMessageInZone(0, "You have failed to clear Tiffauges Castle 2F. You will be teleported to Lake Timore after 10 seconds.")
	DEScriptZone:SetTimer(3, 1000 * 10, 1)
	
end

function OnInitializeVariables(ZoneCreateInfo)

	ZoneCreateInfo.Name = "Tiffauges2F_R"
	ZoneCreateInfo.TemplateZoneID = 1803
	
	-- 辆练喊肺 促弗版快俊绰 ??
	ZoneCreateInfo.StartPosX = 120
	ZoneCreateInfo.StartPosY = 138

	-- indivisual(俺牢), party(颇萍), lobby(肺厚, 肺厚俊 甸绢埃 饶 曼咯)
	--ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.EntranceType = "party"
	ZoneCreateInfo.StaticZone = false
	
	--  酒公档 绝绰 惑怕俊辑 弥措 瘤加 矫埃 
	ZoneCreateInfo.EmptyTimeOut = 10 -- 15 sec
end 

function OnInitialize()
	
	if ( DEScriptZone:GetScriptZoneCount() > MaxScriptZoneCount ) then
		DoungeonOpened = false
		
		return
	end
	
	DoungeonOpened = true
	
	CloseGate(MissionRoom1_BlockedPos, EffectGateID_NW)
	CloseGate(MissionRoom2_BlockedPos, EffectGateID_NW)
	CloseGate(MissionRoom3_BlockedPos, EffectGateID_NE)
	CloseGate(MissionRoom4_BlockedPos, EffectGateID_NW)
	CloseGate(MissionRoom5_BlockedPos, EffectGateID_NW)
	CloseGate(MissionRoom6_BlockedPos1, EffectGateID_NW)
	CloseGate(MissionRoom6_BlockedPos2, EffectGateID_NW)
	
	DoungeonStartTime = DEScript:GetCurrentTime()
		
	StartMissionRoom1()
	--StartMissionRoom2()
	--StartMissionRoom3()
	--StartMissionRoom4()
	--StartMissionRoom5()
	--StartMissionRoom6()
	--StartMissionRoomBoss()

	DEScriptZone:SetTimer(1, 1000 * 5)
	
end

function OnTerminate()
	
end

function OnBeforeEnterZone(Creature)
	if DoungeonOpened == false then
		-- 带傈 积己 力茄 檬苞
		DEScriptZone:SendSystemMessage(Creature, 0, "The number of players has already exceeded the player limit.")
		
		DEScriptZone:SetIgnoreEmptyTimeOut(true)
		
		return false;
	end
	
	PlayerLevel = DEScript:GetLevel(Creature)
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)

	if (PlayerLevel + AdvLevel ) < 130 then
		DEScriptZone:SendSystemMessage(Creature, 0, "Your level is too low to enter.")
		
		return false
	end

	HaveItemInfo = 
	{
		-- 康氰茄 辑
		ItemClass = 91,
		ItemType = 30,
		ItemCount = 1
	}

	if DEScriptZone:HaveItem(Creature, HaveItemInfo) then
		TakeItemInfo = {}
		TakeItemInfo = HaveItemInfo

		DEScriptZone:TakeItem(Creature, TakeItemInfo)

		return true
	end

	DEScriptZone:SendSystemMessage(Creature, 0, "You do not have a miracle epistle required.")

	return false
	
end

function OnAfterEnterZone(Creature)
	remainTime = GetRemainTime() * 10 -- 1/10 seconds
	
	SendRemiainTime(Creature, remainTime) 
end

function OnBeforeLeaveZone(Creature)
	
	SendRemiainTime(Creature, 0)
end

function OnAfterLeaveZone(Creature)
	
end

function OnAttackCreature(AttackerCreature, DefenderCreature)
	
	if DefenderCreature.ObjectID == 0 then
		return
	end
	
	if DEScript:IsMonster(DefenderCreature) then
		if MissionRoomSummonList[DefenderCreature.ObjectID] ~= nil then
			local f = MissionRoomSummonList[DefenderCreature.ObjectID]
			f(AttackerCreature, DefenderCreature)
		end		
	end
	
	if MissionRoomHiddenEnable == true then
		if MissionRoomHiddenGusionObjectID == DefenderCreature.ObjectID and MissionRoomHiddenGusionScript1Enable == false then
			DEScriptZone:BroadcastGCSay(DefenderCreature, strMissionRoomHiddenStartScript, 16777215)  -- 16777215 = 0x00FFFFFF
			
			MissionRoomHiddenGusionScript1Enable = true
		end
		
		if DefenderCreature.ObjectID == MissionRoomHiddenGusionObjectID then
		
			if MissionRoomHiddenMonsterPrevHP == -1 then
				MissionRoomHiddenMonsterPrevHP = DefenderCreature.CurHP 
			end
				
			DiffHP = MissionRoomHiddenMonsterPrevHP - DefenderCreature.CurHP
			if DiffHP > MissionRoomHiddenMonsterWarpHP then
				MissionRoomHiddenMonsterPrevHP = DefenderCreature.CurHP
				
				MissionRoomBoss_MoveFast(DefenderCreature)
				MissionRoomHiddenMonsterWarpHP = MissionRoomHiddenMonsterWarpHP * 0.9
				
				if ( MissionRoomHiddenMonsterWarpHP < 2000 ) then
					MissionRoomHiddenMonsterWarpHP = 2000
				end
			end
		end
	end
	
	if CurrentMissionRoom == 7 then

		if MissionRoomBossGusionScript1Enable == false and DefenderCreature.ObjectID == MissionRoomBossGusionObjectID then
			DEScriptZone:BroadcastGCSay(DefenderCreature, strMissionRoomBossStartScript, 16777215)  -- 16777215 = 0x00FFFFFF
		
			MissionRoomBossGusionScript1Enable = true
		end
		
		if DefenderCreature.ObjectID == MissionRoomBossGusionObjectID then
			if MissionRoomBossMonsterPrevHP == -1 then
				MissionRoomBossMonsterPrevHP = DefenderCreature.CurHP 
			end
			
			DiffHP = MissionRoomBossMonsterPrevHP - DefenderCreature.CurHP
			if DiffHP > MissionRoomBossMonsterWarpHP then
				MissionRoomBossMonsterPrevHP = DefenderCreature.CurHP
				
				MissionRoomBoss_MoveFast(DefenderCreature)
				MissionRoomBossMonsterWarpHP = MissionRoomBossMonsterWarpHP * 0.9
				
				if ( MissionRoomBossMonsterWarpHP < 2000 ) then
					MissionRoomBossMonsterWarpHP = 2000
				end
			end
		end
	end
end

function OnKillCreature(AttackerCreature, DeadCreature)
	if CurrentMissionRoom == 1 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom1MonsterList) then
			StartMissionRoom2()
		end
	end
	
	if CurrentMissionRoom == 2 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom2MonsterList) then
			StartMissionRoom3()
		end
	end
	
	if CurrentMissionRoom == 3 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom3MonsterList) then
			StartMissionRoom4()
		end
	end
	
	if MissionRoomHiddenEnable == true then
		if MissionRoomHiddenGusionObjectID == DeadCreature.ObjectID then
			DEScriptZone:BroadcastGCSay(DeadCreature, strMissionRoomHiddenKillScript,  16777215)  -- 16777215 = 0x00FFFFFF
			
			MissionRoomHiddenEnable = false
		end
	end
	
	if CurrentMissionRoom == 4 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom4MonsterList) then
			StartMissionRoom5()
		end
	end
	
	if CurrentMissionRoom == 5 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom5MonsterList) then
			StartMissionRoom6()
		end
	end
	
	if CurrentMissionRoom == 6 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom6MonsterList) then
			StartMissionRoomBoss()
		end
	end
	
	if CurrentMissionRoom == 7 then
		if MissionRoomBossGusionObjectID == DeadCreature.ObjectID then
			DEScriptZone:BroadcastGCSay(DeadCreature, strMissionRoomBossKillScript,  16777215)  -- 16777215 = 0x00FFFFFF
			
			StartMissionSuccess()
		end
	end
	
end

function OnAddMonster(Monster)
	
	EffectTranslucencyCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 586,
		
		NextTime = 0,
		Deadline = 99999999
	}

	Monster._pCreature = Monster._pMonster
	DEScriptZone:AddEffectToCreature(EffectTranslucencyCreateInfo, Monster)	

end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	
	ZoneCoordInfo.ZoneID = 51
	ZoneCoordInfo.X = 155 + math.random(-7, 7)
	ZoneCoordInfo.Y = 54 + math.random(-14, 14)
	
	return true;	-- 汲沥等 何劝 困摹肺 捞悼
	
end

function OnSay(CreatureInfo, Mesg)
	
	return true
end

function OnTimer(TimerID)

	if TimerID == 1 then
	
		remainTime = GetRemainTime()
		 
		--DEScriptZone:BroadcastSystemMessageInZone(0, " "..remainTime.." seconds left")
		if DoungeonClear == false and remainTime < 0 then
			-- 带傈 努府绢 角菩
			
			DEScriptZone:KillTimer(1)
			
			StartMissionFailed()
		end
		
		if remainTime < 60 * 5 then
			-- 5 盒捞窍啊 巢疽阑  锭,
		
		end
	end
	
	if TimerID == 2 then
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Tiffauges/Tiffauges2F_O")
		
		DEScript:WriteIntegerOnWhiteBoard("Tiffauges.2F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())
		
		OnTraverseCreature = OnWarpToOtherWorld
		DEScriptZone:TraverseCreature()	
	end
	
	if TimerID == 3 then
		OnTraverseCreature = OnWarpToGetOut
		DEScriptZone:TraverseCreature()	
	end
	
	if TimerID == 5 then
		-- 固记 己傍矫
		DEScriptZone:KillTimer(5)
		
		OnTraverseCreature = NotifyWarpToWorld
		DEScriptZone:TraverseCreature()

		DEScriptZone:SetTimer(2, 1000 * 15, 1)

	end
	
	ProcessTrap(TimerID)
end


function OnWarpToOtherWorld(Creature)
 	-- 泅技 -> 捞拌 捞悼

	if WarpToOtherWorldZoneID ~= 0 then
		DEScriptZone:TransportCreature(Creature, WarpToOtherWorldZoneID, 120, 138)
	end
end

function OnGiveRewardItem(Creature)

	GiveItemInfo = 
	{
		ItemClass = 40,
		ItemType = 33,
		ItemCount = 1,
		Options = "",
		LimitedTime = 0
	}

	if DEScriptZone:GiveItem(Creature, GiveItemInfo) then
		
	end
end

	
