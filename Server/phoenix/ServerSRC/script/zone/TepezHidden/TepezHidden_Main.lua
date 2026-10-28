dofile("../script/zone/TepezHidden/TepezHidden_MonsterInfo.lua")

function CheckMissionAllKill(DeadCreatureObjectID, MissionMonsterList)
	for index, ObjectID in pairs(MissionMonsterList) do
		if ObjectID == DeadCreatureObjectID then
			table.remove(MissionMonsterList, index)
		end
	end
	
	if table.maxn(MissionMonsterList) == 0 then
		return true
	end
	
	return false
end

function InstallTrap(TrapBaseX, TrapBaseY, TrapNextTime, TrapDeadLine)
	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 585,
		
		PosX = TrapBaseX,
		PosY = TrapBaseY,
		
		NextTime = TrapNextTime,
		Deadline = TrapDeadLine
	}
	
	DEScriptZone:AddEffectToTile(EffectCreateInfo)

end

function SendRemainTime(Creature, remainTime, timerDesc)
	if timerDesc == nil then
		timerDesc = "히든 테페즈 레어"
	end
	
	DEScriptZone:SendSystemMessage(Creature, 7, timerDesc.." ["..remainTime.."]")
end

EffectShakeCreateInfo =
{
	EffectClass = 0,
	Duration = 15 * 1000
}

EffectLightCreateInfo =
{
	EffectClass = 2,
	Duration = 0
}
		
function OnBroadcastEffectShakeToScreen(CreatureInfo)
	DEScriptZone:AddEffectToScreen(EffectShakeCreateInfo, CreatureInfo)
end

function OnBroadcastEffectLightToScreen(CreatureInfo)
	DEScriptZone:AddEffectToScreen(EffectLightCreateInfo, CreatureInfo)
end

function OnSendRemainTime100(CreatureInfo)
	SendRemainTime(CreatureInfo, 10 * 100, "히든 테페즈 레어 종료") -- 100초
end

function OnWarpToGetOut(Creature)

	if DEScript:IsSlayer(Creature) then
		WarpID = 12
		WarpX = 209
		WarpY = 104
	elseif DEScript:IsVampire(Creature) then
		WarpID = 1003
		WarpX = 62
		WarpY = 64
	elseif DEScript:IsOusters(Creature) then
		WarpID = 1311
		WarpX = 24
		WarpY = 73	
	end
	
	DEScriptZone:TransportCreature(Creature, 
		WarpID, 
		WarpX + math.random(-7, 7), 
		WarpY + math.random(-7, 7)
	)
	
end

MissionRoom1_MonsterPos = 
{
	{
		MonsterInfo = ChiefShamanOaf,
		MonsterPos = 
		{
			{21,30},
			{24,33},
			{26,30},
			{36,29},
			{37,30},
			{38,31}
		}
	},
	{
		MonsterInfo = ChiefMumRimmon,
		MonsterPos =
		{
			{21,31},
			{24,32},
			{26,31},
			{42,36},
			{43,37},
			{44,38}
		}
	},
	{
		MonsterInfo = Pusca,
		MonsterPos =
		{
			{22,31},
			{25,32},
			{26,32},
			{35,29},
			{36,30},
			{37,31}
		}
	},
	{
		MonsterInfo = Trasla,
		MonsterPos =
		{
			{20,30},
			{23,33},
			{27,32},
			{42,37},
			{43,38},
			{44,39}
		}
	},
	{
		MonsterInfo = NodCopila,
		MonsterPos =
		{
			{22,32},
			{23,34},
			{27,31},
			{34,31},
			{35,32},
			{36,33}
		}
	},
	{
		MonsterInfo = Razor,
		MonsterPos =
		{
			{22,33},
			{25,35},
			{28,32},
			{40,38},
			{41,39},
			{42,40}
		}
	},
	{
		MonsterInfo = ChiefLichJel,
		MonsterPos =
		{
			{24,25},
			{38,27}
		}
	},
	{
		MonsterInfo = ChiefGiantOs,
		MonsterPos =
		{
			{24,27},
			{45,34}
		}
	},
	{
		MonsterInfo = ChiefMountCrag,
		MonsterPos =
		{
			{23,29},
			{25,29},
			{37,28},
			{44,36}
		}
	}	
}

MissionRoom2_MonsterPos = 
{
	{
		MonsterInfo = GrudgeWidows,
		MonsterPos = 
		{
			{25,17},
			{29,19}
		}
	},
	{
		MonsterInfo = HellGrouper,
		MonsterPos = 
		{
			{25,19},
			{27,20},
			{29,20}
		}
	},
	{
		MonsterInfo = PitchBlackWizard,
		MonsterPos = 
		{
			{26,20},
			{28,21},
			{30,18}
		}
	}
}
	
MissionRoom1MonsterList = {}
MissionRoom2MonsterList = {}

GenuineTepezInfo = nil
WaitingTimeAfterMissionClear = 100	-- 클리어 이후 대기 최대 시간

CurrentMissionRoom = 1
DoungeonPlayTime = 60 * 60	-- 60 minutes * 60 seconds

function StartMissionRoom1()

	CurrentMissionRoom = 1
	
	for i, MonsterArrangeInfo in pairs(MissionRoom1_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
		
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom1MonsterList, AddedMonster.ObjectID)
		end	
	end
	
end

function SummonMissionRoom2()

	if ( table.getn(MissionRoom2MonsterList) > 200 ) then
		return
	end
	
	for i, MonsterArrangeInfo in pairs(MissionRoom2_MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
		
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
		
			if GenuineTepezInfo == nil then
				AddMonsterInfo.X = MonsterPos[1]
				AddMonsterInfo.Y = MonsterPos[2]			
			else
				AddMonsterInfo.X = GenuineTepezInfo.X + math.random(0, 3) - 3
				AddMonsterInfo.Y = GenuineTepezInfo.Y + math.random(0, 3) - 3
			end
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MissionRoom2MonsterList, AddedMonster.ObjectID)

		end	
	end
	
end

function StartMissionRoom2()
	
	CurrentMissionRoom = 2
	
	SummonMissionRoom2()
	
	AddMonsterInfo = CloneMorgoth
	AddMonsterInfo.X = 29
	AddMonsterInfo.Y = 17
	
	AddedMonster = DEScriptZone:AddMonster(CloneMorgoth)	
	table.insert(MissionRoom2MonsterList, AddedMonster.ObjectID)

	AddMonsterInfo = GenuineTepez
	AddMonsterInfo.X = 24
	AddMonsterInfo.Y = 16
	
	GenuineTepezInfo = DEScriptZone:AddMonster(AddMonsterInfo)
	table.insert(MissionRoom2MonsterList, GenuineTepezInfo.ObjectID)
		
	DEScriptZone:SetTimer(2, 1000 * 30, 1)
	
end

function OnInitialize()
		
	StartMissionRoom1()
	
	DoungeonStartTime = DEScript:GetCurrentTime()
	
	DEScriptZone:SetTimer(3, 1000 * DoungeonPlayTime, 1)
	
end

function OnTerminate()
	
end

function OnBeforeEnterZone(Creature)

	PlayerLevel = DEScript:GetLevel(Creature)
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
	
	if (PlayerLevel + AdvLevel ) <= 100 then
		DEScriptZone:SendSystemMessage(Creature, 0, "Your level is too low to enter.")
		
		return false
	end	
	
	HaveItemInfo1 = 
	{
		-- 손상된 가죽
		ItemClass = 91,
		ItemType = 5,
		ItemCount = 20
	}
	
	HaveItemInfo2 = 
	{
		-- 테페즈 펜던트
		ItemClass = 46,
		ItemType = 3,
		ItemCount = 1
	}

	if DEScriptZone:HaveItem(Creature, HaveItemInfo1) and DEScriptZone:HaveItem(Creature, HaveItemInfo2) then
		TakeItemInfo1 = {}
		TakeItemInfo1 = HaveItemInfo1
		
		TakeItemInfo2 = {}
		TakeItemInfo2 = HaveItemInfo2

		DEScriptZone:TakeItem(Creature, TakeItemInfo1)
		DEScriptZone:TakeItem(Creature, TakeItemInfo2)

		return true
	end
	
	DEScriptZone:SendSystemMessage(Creature, 0, "You need more item for enter.")
	
	return false;	
	
end

function OnAfterEnterZone(Creature)

	DEScriptZone:AddEffectToScreen(EffectLightCreateInfo, Creature)
	
	DoungeonRemainTime = DoungeonPlayTime - ( DEScript:GetCurrentTime() - DoungeonStartTime )
	SendRemainTime(Creature, DoungeonRemainTime * 10)
	
end

function OnBeforeLeaveZone(Creature)
	SendRemainTime(Creature, 0)
end

function OnAfterLeaveZone(Creature)

end

function OnAttackCreature(AttackerCreature, DefenderCreature)
	if GenuineTepezInfo ~= nil then
		if DefenderCreature ~= nil and DefenderCreature.ObjectID == GenuineTepezInfo.ObjectID then
			GenuineTepezInfo.X = DefenderCreature.X
			GenuineTepezInfo.Y = DefenderCreature.Y
		end
	end
end

function OnKillCreature(AttackerCreature, DeadCreature)

	if CurrentMissionRoom == 1 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom1MonsterList) then

			OnTraverseCreature = OnBroadcastEffectShakeToScreen
			DEScriptZone:TraverseCreature()
					
			StartMissionRoom2()
		end
	end
	
	if CurrentMissionRoom == 2 then
		if CheckMissionAllKill(DeadCreature.ObjectID, MissionRoom2MonsterList) then
			OnTraverseCreature = OnSendRemainTime100
			DEScriptZone:TraverseCreature()
			
			DEScriptZone:SetTimer(10, 1000 * 1, 1)

		end
	end

	if CurrentMissionRoom == 2 and GenuineTepezInfo ~= nil and GenuineTepezInfo.ObjectID == DeadCreature.ObjectID then
		DEScriptZone:KillTimer(2)
		
		GenuineTepezInfo = nil
	end
	
end

function OnAddMonster(Monster)

end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	
	return false;	-- 설정된 부활 위치로 이동
	
end

function OnSay(CreatureInfo, Mesg)

	return true
end

function OnTimer(TimerID)
	
	if ( TimerID == 2 ) then
		SummonMissionRoom2()
	end

	if TimerID == 3 then
		-- 시간 초과되어 던전 밖으로 보냄
		OnTraverseCreature = OnWarpToGetOut
		DEScriptZone:TraverseCreature()
	end
	
	if TimerID == 10 then
		WaitingTimeAfterMissionClear = WaitingTimeAfterMissionClear - 1
		
		if WaitingTimeAfterMissionClear == 35 then
			DEScriptZone:SetTimer(11, 1000 * 5, 1)
		end
		
		if WaitingTimeAfterMissionClear == 10 then
			DEScriptZone:SetTimer(12, 1000 * 1, 1)
		end
		
		if WaitingTimeAfterMissionClear <= 0 then
		
			OnTraverseCreature = OnWarpToGetOut
			DEScriptZone:TraverseCreature()
		
			DEScriptZone:KillTimer(10)
			DEScriptZone:KillTimer(11)
			DEScriptZone:KillTimer(12)
		end
	end
	
	if TimerID == 11 then
		OnTraverseCreature = OnBroadcastEffectShakeToScreen
		DEScriptZone:TraverseCreature()
	end
	
	if TimerID == 12 then
		
		for i = 0, 30 do
			InstallTrap(math.random(5, 50), math.random(10,60), math.random(0, 30), 30)
		end 
	end
end

function OnUseItemInGear(Creature, ItemClass, ItemType)
	return true
end

function OnUseItemInQuick(Creature, ItemClass, ItemType)
	return true
end

function OnUseItemInInventory(Creature, ItemClass, ItemType)
	return true
end

function OnUseInventorySkill(Creature, SkillType)
	return true
end

function OnUseObjectSkill(Creature, SkillType)
	return true
end

function OnUseNamedSkill(Creature, SkillType)
	return true
end

function OnUseSelfSkill(Creature, SkillType)
	return true
end

function OnUseTileSkill(Creature, SkillType)
	return true
end

