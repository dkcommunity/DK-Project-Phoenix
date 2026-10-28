dofile("../script/zone/ruper/ruper_MonsterInfo.lua")
dofile("../script/zone/ruper/ruper_BlockInfo.lua")

BossStage_Boss = 
{
	{
		MonsterInfo = Onibla,
		MonsterPos = 
		{
			{104,105},
			{106,105},
			{108,105}
		}
	}
}

BossStage_Monster = 
{
	{
		MonsterInfo = AgonyChild,
		MonsterPos = 
		{
			{94,101},
			{96,104},
			
		}
	},
	
}

NoneSlayer_MonsterPos = 
{
	{
		MonsterInfo = CloneGussion,
		MonsterPos = 
		{
			{100,95},
		}
	},
	
}

NoneVampire_MonsterPos = 
{
	{
		MonsterInfo = CloneGussion,
		MonsterPos = 
		{
			{122,115},

		}
	},
	
}

NoneOusters_MonsterPos = 
{
	{
		MonsterInfo = CloneGussion,
		MonsterPos = 
		{
			{102,129},

		}
	},
	
}

	STAGE_STATE_INIT				= 0
	STAGE_STATE_WAIT_TIME			= 1
	STAGE_STATE_RACE_WAR			= 2
	STAGE_STATE_OPEN_ONIBLA_ROOM	= 30
	STAGE_STATE_ATTACK_ONIBLA		= 31
	STAGE_STATE_WARP_TO_GET_OUT		= 4
	STAGE_STATE_FINISH				= 5
	
	-- ÀÔÀå Á¦ÇÑ
	CanEntrance = false
	-- Á¾Á·º° ÀÎ¿ø
	SlayerCount = 0
	VampireCount = 0
	OustersCount = 0
	-- ½Ã°£ °ü·Ã
	DoungeonStartTime = 0
	DoungeonFirstStartTime = 1 * 60		-- ´ÜÀ§ : seconds
	DoungeonReStartTime = 1 * 60			-- ´ÜÀ§ : seconds
	DoungeonWaitTime = 1 * 60				-- ´ÜÀ§ : seconds
	IcenDungeonRaceWarTime = 5 * 60	    -- ´ÜÀ§ : seconds 
	EntranceStageBoss = 1 * 10				-- ´ÜÀ§ : seconds
	
	LogFileName = "ruper.log"

	-- ¿ÞÂÊ ¹®ÀÇ À§Ä¡
	LeftDoorPosX = 102
	LeftDoorPosY = 103
	-- ¿À¸¥ÂÊ ¹®ÀÇ À§Ä¡
	RightDoorPosX = 105
	RightDoorPosY = 107

	JoinRaceMax = 24
	VillaninLadyGhostList = {}
	VillaninLadyMax = 40 + 1

	StageStat = STAGE_STATE_INIT
	BossDeadStat = false

	IsOpenLeftSecretRoom = true
	IsOpenRightSecretRoom = true
	IsOpenBossRoom = true
	IsOpenBossRoomExit = true
	
	NoneSlayer_MonsterList = {}
	NoneVampire_MonsterList = {}
	NoneOusters_MonsterList = {}
	BossStage_BossList = {}
	BossStage_MonsterList = {}
	
function OnInitializeVariables(ZoneCreateInfo)

	ZoneCreateInfo.Name = "ruper"	
	ZoneCreateInfo.TemplateZoneID = 6000
	
	-- Á¾Á·º°·Î ´Ù¸¥°æ¿ì¿¡´Â ??
	ZoneCreateInfo.StartPosX = 106
	ZoneCreateInfo.StartPosY = 51

	-- indivisual(°³ÀÎ), party(ÆÄÆ¼), lobby(·Îºñ, ·Îºñ¿¡ µé¾î°£ ÈÄ Âü¿©)
	ZoneCreateInfo.EntranceType = "indivisual"
	--ZoneCreateInfo.EntranceType = "party"
	ZoneCreateInfo.StaticZone = true
	
	--  ¾Æ¹«µµ ¾ø´Â »óÅÂ¿¡¼­ ÃÖ´ë Áö¼Ó ½Ã°£ 
--	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
	
	DEScriptZone:SetTimer(1, 1000 * DoungeonFirstStartTime, 1)
end 

function OnInitialize()
	DoungeonStartTime = DEScript:GetCurrentTime()
	DEScriptZone:SetPKZone(false)
end

function OnTerminate()

end

function OnSetStartPos(CreatureInfo, PositionInfo)
    if DEScript:IsSlayer(CreatureInfo) then
		PositionInfo.X = 106
        PositionInfo.Y = 51
	elseif DEScript:IsVampire(CreatureInfo) then
		PositionInfo.X = 63
        PositionInfo.Y = 109
	elseif DEScript:IsOusters(CreatureInfo) then
		PositionInfo.X = 85
        PositionInfo.Y = 75
	end
end

function SendRemainTime(Creature, remainTime, timerDesc)
	if timerDesc == nil then
		timerDesc = "Time left:";
	end
			
	DEScriptZone:SendSystemMessage(Creature, 7, timerDesc.." ["..remainTime.."]")
end

function OnBeforeEnterZone(Creature)
--	DEScriptZone:BroadcastGCSay(Creature, "³Ê¹« ±ô±ôÇØ~~~~~~~~~~~~~~~~~", 5592405)  -- 5592405 = 0x00555555 BGR
--	return false
	
	if CanEntrance == false then
		DEScriptZone:SendSystemMessage(Creature, 0, "ÏÖÔÚ²»ÄÜ½øÈë.") 
	end
	
	if SlayerCount > JoinRaceMax -1 and CanEntrance == true then
		DEScriptZone:SendSystemMessage(Creature, 0, "ÈËÀàÈËÊýÒÑµ½´ïÉÏÏÞ. ") 
		return false		
	end
	if VampireCount > JoinRaceMax -1 and CanEntrance == true then
		DEScriptZone:SendSystemMessage(Creature, 0, "ÎüÑª¹íÈËÊýÒÑ´ïµ½ÉÏÏÞ. ") 
		return false
	end
	if OustersCount > JoinRaceMax -1 and CanEntrance == true then
		DEScriptZone:SendSystemMessage(Creature, 0, "Ä§ÁéÈËÊýÒÑ´ïµ½ÉÏÏÞ.") 
		return false
	end
	
	return CanEntrance
end

function OnAfterEnterZone(Creature)
	if DEScript:IsSlayer(Creature) then
		SlayerCount = SlayerCount + 1
	elseif DEScript:IsVampire(Creature) then
		VampireCount = VampireCount + 1
	elseif DEScript:IsOusters(Creature) then
		OustersCount = OustersCount + 1
	end
	
	DoungeonRemainTime = DoungeonWaitTime - ( DEScript:GetCurrentTime() - DoungeonStartTime )
	SendRemainTime(Creature, DoungeonRemainTime * 10)

end

function OnBeforeLeaveZone(Creature)
end

function OnAfterLeaveZone(Creature)
	SendRemainTime(Creature, 0)
	
	if DEScript:IsSlayer(Creature) then
		SlayerCount = SlayerCount - 1
	elseif DEScript:IsVampire(Creature) then
		VampireCount = VampireCount - 1
	elseif DEScript:IsOusters(Creature) then
		OustersCount = OustersCount - 1
	end
			
	if StageStat == STAGE_STATE_RACE_WAR then
		
		MonsterCount = table.maxn(NoneSlayer_MonsterList) +
			table.maxn(NoneVampire_MonsterList) +
			table.maxn(NoneOusters_MonsterList)

		if NoneRaceCheck() == 1 and MonsterCount == 0 then
			
			StartMisssionOniblaRoom()
		end
	end
	
end

function OnKillCreature(AttackerCreature, DeadCreature)

	if StageStat == STAGE_STATE_RACE_WAR or StageStat == STAGE_STATE_FINISH then
		CheckMissionAllKill(DeadCreature.ObjectID, NoneSlayer_MonsterList)
		CheckMissionAllKill(DeadCreature.ObjectID, NoneVampire_MonsterList)
		CheckMissionAllKill(DeadCreature.ObjectID, NoneOusters_MonsterList)
	end
	
	if StageStat == STAGE_STATE_RACE_WAR then
	
		MonsterCount = table.maxn(NoneSlayer_MonsterList) +
			table.maxn(NoneVampire_MonsterList) +
			table.maxn(NoneOusters_MonsterList)

		if NoneRaceCheck() == 1 and MonsterCount == 0  then
			
			StartMisssionOniblaRoom()
		end
	end

	if StageStat == STAGE_STATE_ATTACK_ONIBLA or StageStat == STAGE_STATE_FINISH then	
		CheckMissionAllKill(DeadCreature.ObjectID, BossStage_MonsterList)
		CheckMissionAllKill(DeadCreature.ObjectID, BossStage_BossList)
	end
		
	if StageStat == STAGE_STATE_ATTACK_ONIBLA then
		MonsterAllKill = false
		BossAllKill = false
		
		if table.maxn(BossStage_MonsterList) == 0 then
			MonsterAllKill = true
		end
		
		if table.maxn(BossStage_BossList) == 0 then
			BossAllKill = true
		end
		
		if MonsterAllKill and BossAllKill then
			EndBossStage()
		end
		
		if BossAllKill and BossDeadStat == false then
			OnTraverseCreature = OnGiveRewardItem
			DEScriptZone:TraverseCreature()
			
			BossDeadStat = true
		end
		
		table.foreach(
			VillaninLadyGhostList, 
			function(i, G) 
				if G == DeadCreature.ObjectID then 
					table.remove(VillaninLadyGhostList, i) 
				end 
			end 
		)
	end
end

function OnAddMonster(Monster)
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	
	-- ÁöÁ¤µÈ À§Ä¡·Î ºÎÈ° 	
	ZoneCoordInfo.ZoneID = 32
	ZoneCoordInfo.X = 148
	ZoneCoordInfo.Y = 120
		
	return true
end

function SetRaceWarBlock()
	for index, Element in ipairs(RaceWarBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
	end
end

function SetLeftSecretBlock()
	for index, Element in ipairs(LeftSecretBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
	end
end

function SetRightSecretBlock()
	for index, Element in ipairs(RightSecretBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
	end
end

function SetBossRoomExitBlock()
	for index, Element in ipairs(BossRoomExitBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
	end
end

-- º¸½º¹æ ºí·Ï
function CloseBossRoom()
	if IsOpenBossRoom == false then
		return
	end
	
	IsOpenBossRoom = false
	
	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 592,
		
		PosX = 0,
		PosY = 0,
		
		NextTime = 0,
		Deadline = 99999999
	}
	
	EffectCreateInfo2 =
	{
		EffectCastCreature = nil,
		
		EffectClass = 591,
		
		PosX = 0,
		PosY = 0,
		
		NextTime = 0,
		Deadline = 99999999
	}

	for index, Element in ipairs(BossRoomLeftBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
		
		EffectCreateInfo.PosX = Element[1]
		EffectCreateInfo.PosY = Element[2]
		
		Element[3] = DEScriptZone:AddEffectToTile(EffectCreateInfo)
	end
	
	for index2, Element2 in ipairs(BossRoomRightBlockPos) do
		DEScriptZone:SetBlocked(Element2[1],Element2[2])
		
		EffectCreateInfo2.PosX = Element2[1]
		EffectCreateInfo2.PosY = Element2[2]
		
		Element2[3] = DEScriptZone:AddEffectToTile(EffectCreateInfo2)
	end
	
	DEScriptZone:SetBlocked(71, 60)
	
	SetRaceWarBlock()
	
end

function CloseBossRoomExit()
	if IsOpenBossRoomExit == false then
		return
	end
	
	IsOpenBossRoomExit = false

	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 592,
		
		PosX = 0,
		PosY = 0,
		
		NextTime = 0,
		Deadline = 99999999
	}
	
	for index, Element in ipairs(BossRoomLeftExitBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
		
		EffectCreateInfo.PosX = Element[1]
		EffectCreateInfo.PosY = Element[2]
		
		Element[3] = DEScriptZone:AddEffectToTile(EffectCreateInfo)
	end
		
	SetBossRoomExitBlock()
end

function CloseRightSecretRoom()
	if IsOpenRightSecretRoom == false then
		return
	end
	
	IsOpenRightSecretRoom = false
	
	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 594,
		
		PosX = 0,
		PosY = 0,
		
		NextTime = 0,
		Deadline = 99999999
	}

	for index, Element in ipairs(RightSecretRoomBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
	end
	
	for index, EffectElement in ipairs(RightSecretRoomEffectPos) do
		EffectCreateInfo.PosX = EffectElement[1]
		EffectCreateInfo.PosY = EffectElement[2]
		EffectElement[3] = DEScriptZone:AddEffectToTile(EffectCreateInfo)
	end
	
	SetRightSecretBlock()
end

function CloseLeftSecretRoom()
	if IsOpenLeftSecretRoom == false then
		return
	end
	
	IsOpenLeftSecretRoom = false

	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 596,
		
		PosX = 0,
		PosY = 0,
		
		NextTime = 0,
		Deadline = 99999999
	}

	for index, Element in ipairs(LeftSecretRoomBlockPos) do
		DEScriptZone:SetBlocked(Element[1],Element[2])
	end
	
	for index, EffectElement in ipairs(LeftSecretRoomEffectPos) do
		EffectCreateInfo.PosX = EffectElement[1]
		EffectCreateInfo.PosY = EffectElement[2]
		EffectElement[3] = DEScriptZone:AddEffectToTile(EffectCreateInfo)
	end
	
	SetLeftSecretBlock()
end

function EndBossStage()
	DEScript:WriteFileLog(LogFileName, "Finish Onibla Dungeon")
	WriteCurrentUser()
		
	DEScriptZone:BroadcastSystemMessageInZone(4, "Â³Åå¶ûµº Èë¿ÚÒÑ´ò¿ª.")
	OpenBossRoomExit()
end

function IcenSendRemainTime(CreatureInfo)
	SendRemainTime(CreatureInfo, 10 * IcenDungeonRaceWarTime, "Â³Åå¶ûµº")
end

function OnTimer(TimerID)
	if TimerID == 1 then
		DEScriptZone:KillTimer(1)
		
		StageStat = STAGE_STATE_WAIT_TIME
		
		SlayerCount = 0
		VampireCount = 0
		OustersCount = 0
		
		BossDeadStat = false
		
		CloseBossRoom()
		CloseBossRoomExit()
		CloseLeftSecretRoom()
		CloseRightSecretRoom()
		
		CanEntrance = true
		DEScriptZone:SetPKZone(false)
		
		-- DEScriptZone:BroadcastSystemMessage(0, "¾ÆÀÌ¼¾ ÁöÇÏ  4ÃþÀÌ ¿ÀÇÂ µÇ¾ú½À´Ï´Ù.") by viva
		DEScriptZone:BroadcastSystemMessage(0, "Â³Åå¶ûµº Èë¿ÚÒÑ´ò¿ª.")
		
		DoungeonStartTime = DEScript:GetCurrentTime()
		DEScriptZone:SetTimer(2, 1000 * DoungeonWaitTime, 1)
				
		DEScriptZone:SetTimer(100, 1000 * 10, 1)	-- ÀÔ±¸ ¿ÀÇÂ ¾Ë¸² 
	end
	
	if TimerID == 2 then
		DEScriptZone:KillTimer(2)
		DEScriptZone:KillTimer(100)
		
		StageStat = STAGE_STATE_RACE_WAR
		
		CanEntrance = false
		DEScriptZone:SetPKZone(true)
		
		if NoneRaceCheck() ~= 0 then
			AloneRacePlay()

		end

		DEScript:WriteFileLog(LogFileName, "Start RaceWar")
		WriteCurrentUser()
				
		DEScriptZone:BroadcastSystemMessageInZone(4, "Õ½¶·°ÉÓÂÊ¿ÃÇ£¬ÈÃÈëÇÖÕß¸ÐÊÜµ½ËÀÍöµÄ¿Ö¾å!")
		
		OnTraverseCreature = IcenSendRemainTime
		DEScriptZone:TraverseCreature()
		DEScriptZone:SetTimer(4, 1000 * IcenDungeonRaceWarTime, 1)
	end
	
	if TimerID == 3 then
		DEScriptZone:KillTimer(3)

		if StageStat == STAGE_STATE_OPEN_ONIBLA_ROOM then
		
			StageStat = STAGE_STATE_ATTACK_ONIBLA
			
			DEScript:WriteFileLog(LogFileName, "Appear Onibla")
			WriteCurrentUser()
			
			DEScriptZone:BroadcastSystemMessageInZone(4, "°ÂÄÝ²¼À­ ÒÑ³öÏÖ.")
		
			BossRoomMonsterStanding()
		end
	end
	
	if TimerID == 4 then
		DEScriptZone:KillTimer(4)
		
		StageStat = STAGE_STATE_WARP_TO_GET_OUT
		
		DEScript:WriteFileLog(LogFileName, "Warp To Get Out")
		WriteCurrentUser()
		
		OnTraverseCreature = OnWarpToGetOut
		DEScriptZone:TraverseCreature()
		
		DEScriptZone:SetTimer(5, 1000 * 10, 1)
	end
	
	if TimerID == 5 then
		DEScriptZone:KillTimer(5)
		
		StageStat = STAGE_STATE_FINISH

		OnTraverseMonster = OnKillMonster
		DEScriptZone:TraverseMonster()
			
		DEScriptZone:SetTimer(1, 1000 * DoungeonReStartTime, 1)
	end
	
	if TimerID == 100 then
	
		if StageStat == STAGE_STATE_WAIT_TIME then
			-- DEScriptZone:BroadcastSystemMessage(0, "¾ÆÀÌ¼¾ ÁöÇÏ  4ÃþÀÌ ¿ÀÇÂ µÇ¾ú½À´Ï´Ù.") by viva
			DEScriptZone:BroadcastSystemMessage(0, "Â³Åå¶ûµº Èë¿ÚÒÑ´ò¿ª.")
		end
	end
end

function OnSay(CreatureInfo, Mesg)

	HaveItemInfo1 = 
	{
		ItemClass = 91,
		ItemType = 34,
		ItemCount = 1
	}
	
	HaveItemInfo2 = 
	{
		ItemClass = 91,
		ItemType = 34,
		ItemCount = 1
	}
	
	if Mesg == "Ö¥Âé¿ªÃÅ" then	
		if IsOpenLeftSecretRoom == false and DEScriptZone:HaveItem(CreatureInfo, HaveItemInfo1) and CanDoorOpen(LeftDoorPosX, LeftDoorPosY, CreatureInfo.X, CreatureInfo.Y) then
			TakeItemInfo1 = {}
			TakeItemInfo1 = HaveItemInfo1
		
			DEScriptZone:TakeItem(CreatureInfo, TakeItemInfo1)
			OpenLeftSecretRoom()
			LeftSecretRoomAction()	
		end
	end
	
	if Mesg == "Ö¥Âé¿ªÃÅ" then		
		if IsOpenRightSecretRoom == false and DEScriptZone:HaveItem(CreatureInfo, HaveItemInfo2) and CanDoorOpen(RightDoorPosX, RightDoorPosY, CreatureInfo.X, CreatureInfo.Y) then
			TakeItemInfo2 = {}
			TakeItemInfo2 = HaveItemInfo2
		
			DEScriptZone:TakeItem(CreatureInfo, TakeItemInfo2)
			OpenRightSecretRoom()
			RightSecretRoomAction()
		end			
	end
	
	return true
end

function CanDoorOpen(DoorX, DoorY, CreatureX, CreatureY)
	Distance = (DoorX - CreatureX) * (DoorX - CreatureX) + (DoorY - CreatureY) * (DoorY - CreatureY)
	
	if Distance <= (5*5) then
		return true
	end
	
	return false
end

function NoneRaceCheck()
	OnlyOneRace = 3 

	if SlayerCount == 0 then
		OnlyOneRace = OnlyOneRace - 1
	end
	if VampireCount == 0 then
		OnlyOneRace = OnlyOneRace - 1
	end
	if OustersCount == 0 then
		OnlyOneRace = OnlyOneRace - 1
	end
	
	return OnlyOneRace
end

function AloneRacePlay()
	 
	if SlayerCount == 0 then
		MonsterDispose(NoneSlayer_MonsterPos, NoneSlayer_MonsterList)
	end	
	if VampireCount == 0 then
		MonsterDispose(NoneVampire_MonsterPos, NoneVampire_MonsterList)
	end 
	if OustersCount == 0 then
		MonsterDispose(NoneOusters_MonsterPos, NoneOusters_MonsterList)
	end	
	
end

function BossRoomMonsterStanding()
	for i, MonsterArrangeInfo in pairs(BossStage_Monster) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
		
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(BossStage_MonsterList, AddedMonster.ObjectID)
		end	
	end
	
	for i, BossMonsterArrangeInfo in pairs(BossStage_Boss) do
		AddBossMonsterInfo = BossMonsterArrangeInfo.MonsterInfo
		
		for index, MonsterPos in ipairs(BossMonsterArrangeInfo.MonsterPos) do
			AddBossMonsterInfo.X = MonsterPos[1]
			AddBossMonsterInfo.Y = MonsterPos[2]
			
			AddedBossMonster = DEScriptZone:AddMonster(AddBossMonsterInfo)
			
			table.insert(BossStage_BossList, AddedBossMonster.ObjectID)
		end	
	end
end

function MonsterDispose(MonsterPos, MonsterList)
	for i, MonsterArrangeInfo in pairs(MonsterPos) do
		AddMonsterInfo = MonsterArrangeInfo.MonsterInfo
		
		for index, MonsterPos in ipairs(MonsterArrangeInfo.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			
			AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
			
			table.insert(MonsterList, AddedMonster.ObjectID)
		end	
	end
end

-- ¸ó½ºÅÍ ´Ù Àâ¾Ò´Ï?
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

function BossRoomAction()
	DEScriptZone:BroadcastSystemMessageInZone(4, "°ÂÄÝ²¼À­ ÒÑËÕÐÑ.")
	DEScriptZone:SetTimer(3, 1000 * EntranceStageBoss, 1)
end

function ClearRaceWarBlock()
	for index, Element in ipairs(RaceWarBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
	end
end

function ClearLeftSecretBlock()
	for index, Element in ipairs(LeftSecretBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
	end
end

function ClearRightSecretBlock()
	for index, Element in ipairs(RightSecretBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
	end
end

function ClearBossRoomExitBlock()
	for index, Element in ipairs(BossRoomExitBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
	end
end

function OpenBossRoom()
	if IsOpenBossRoom == true then
		return 
	end
		
	IsOpenBossRoom = true

	for index, Element in ipairs(BossRoomLeftBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
		
		if Element[3] ~= nil then
			DEScriptZone:RemoveEffect(Element[3])
		end
	end
	
	for index2, Element2 in ipairs(BossRoomRightBlockPos) do
		DEScriptZone:ClearBlocked(Element2[1],Element2[2])
		
		if Element2[3] ~= nil then
			DEScriptZone:RemoveEffect(Element2[3])
		end
	end		
	
	DEScriptZone:ClearBlocked(71, 60)
	
	ClearRaceWarBlock()
end

function OpenBossRoomExit()
	if IsOpenBossRoomExit == true then
		return
	end
	
	IsOpenBossRoomExit = true

	for index, Element in ipairs(BossRoomLeftExitBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
		
		if Element[3] ~= nil then
			DEScriptZone:RemoveEffect(Element[3])
		end
	end	
	
	ClearBossRoomExitBlock()
end

function LeftSecretRoomAction()

	DEScript:WriteFileLog(LogFileName, "Open BlackAstral Room")
	WriteCurrentUser()
	
	DEScriptZone:BroadcastSystemMessageInZone(4, "ºÚÐÇÖ®ÃÅÒÑ´ò¿ª.")
		
	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 597,
		
		PosX = 36,
		PosY = 86,
		
		NextTime = 0,
		Deadline = 16
	}
	
	DEScriptZone:AddEffectToTile(EffectCreateInfo)
		
	AddMonsterInfo = BlackAstralMagicCrystal
	AddMonsterInfo.X = 12
	AddMonsterInfo.Y = 62
	
	AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
end

function OpenLeftSecretRoom()
	if IsOpenLeftSecretRoom == true then
		return
	end
	
	IsOpenLeftSecretRoom = true
	
	ClearLeftSecretBlock()

	for index, Element in ipairs(LeftSecretRoomBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
	end
	
	for index, EffectElement in ipairs(LeftSecretRoomEffectPos) do
		if EffectElement[3] ~= nil then
			DEScriptZone:RemoveEffect(EffectElement[3])
		end
	end
end

function RightSecretRoomAction()
	DEScript:WriteFileLog(LogFileName, "Open WhiteAstral Room")
	WriteCurrentUser()

	DEScriptZone:BroadcastSystemMessageInZone(4, "°×ÐÇÖ®ÃÅÒÑ´ò¿ª.")
	
	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 595,
		
		PosX = 78,
		PosY = 99,
		
		NextTime = 0,
		Deadline = 16
	}
	
	DEScriptZone:AddEffectToTile(EffectCreateInfo)

	AddMonsterInfo = WhiteAstralSpellCrystal
	AddMonsterInfo.X = 113
	AddMonsterInfo.Y = 61
	
	AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
end

function OpenRightSecretRoom()
	if IsOpenRightSecretRoom == true then
		return
	end
	
	IsOpenRightSecretRoom = true
	
	ClearRightSecretBlock()

	for index, Element in ipairs(RightSecretRoomBlockPos) do
		DEScriptZone:ClearBlocked(Element[1],Element[2])
	end	
	
	for index, EffectElement in ipairs(RightSecretRoomEffectPos) do
		if EffectElement[3] ~= nil then
			DEScriptZone:RemoveEffect(EffectElement[3])
		end
	end
end


function OnKillMonster(Monster)
	if Monster ~= nil then
		DEScript:SetHP(Monster, 0)
	end
end

function OnWarpToGetOut(Creature)

	if DEScript:IsSlayer(Creature) then
		WarpID = 12
		WarpX = 195
		WarpY = 129
	elseif DEScript:IsVampire(Creature) then
		WarpID = 1003
		WarpX = 80
		WarpY = 52
	elseif DEScript:IsOusters(Creature) then
		WarpID = 1311
		WarpX = 45
		WarpY = 87	
	end
	
	DEScriptZone:TransportCreature(Creature, 
		WarpID, 
		WarpX  + math.random(-7, 7), 
		WarpY  + math.random(-7, 7)
	)

end

function IsAbleToUseItem(Creature, ItemClass, ItemType)
--	print("IsAbleToUseItem() Creature.Name="..Creature.Name.." ItemClass="..ItemClass.." ItemType="..ItemType)

	Result = true
	
	if ItemClass == 7 then			-- ¹ÙÀÌÅ© ±ÝÁö
		Result = false
	elseif ItemClass == 98 then		-- ¹ìÆÄ À®¾ÆÀÌÅÛ ±ÝÁö
		Result = false
	elseif ItemClass == 98 then		-- ¾Æ¿ì À®¾ÆÀÌÅÛ ±ÝÁö
		Result = false
	elseif ItemClass == 39 then		-- ¹ìÆÄÀÌ¾î ¾Á »ç¿ë  ±ÝÁö
		Result = false
	elseif ItemClass == 51 then		-- ½½·¹ÀÌ¾î Ä¿ÇÃ¸µ »ç¿ë  ±ÝÁö
		Result = false
	elseif ItemClass == 52 then		-- ¹ìÆÄÀÌ¾î Ä¿ÇÃ¸µ »ç¿ë  ±ÝÁö
		Result = false
	elseif ItemClass == 93 then		-- ¾Æ¿ì½ºÅÍÁî ÇÏ¸ð´Ð ÆÒ´øÆ® »ç¿ë  ±ÝÁö
		Result = false
	end
	
	if Result == false then
		DEScriptZone:SendSystemMessage(Creature, 4, "¸ÃÎïÆ·²»ÄÜÔÚÕâÀïÊ¹ÓÃ.")
	end
			
	return Result
end

function IsAbleToUseSkill(Creature, SkillType)
--	print("IsAbleToUseSkill() Creature.Name="..Creature.Name.." SkillType="..SkillType)

	Result = true

	if SkillType == 102 then		-- ¹ÚÁã º¯½Å ±ÝÁö
		Result = false
	elseif SkillType == 185 then	-- ¼Ò¿ïÃ¼ÀÎ  ±ÝÁö
		Result = false
	elseif SkillType == 115 then	-- ºí·¯µð ÅÍ³Î  ±ÝÁö(¹ìÆÄ ¾Á)
		Result = false
	elseif SkillType == 114 then	-- ºí·¯µð ¸¶Å© ±ÝÁö(¹ìÆÄ ¾Á)
		Result = false
	end
		
	if Result == false then
		DEScriptZone:SendSystemMessage(Creature, 4, "¸Ã¼¼ÄÜ²»ÄÜÔÚÕâÀïÊ¹ÓÃ.")
	end
	
	return Result
	
end

function OnBeforeUseMonsterSkill(CreatureInfo, SkillType)
	return true
end


function OnAfterUseMonsterSkill(CreatureInfo, SkillType)
	if VillaninLadyMax > table.maxn(VillaninLadyGhostList) then
	
		if ( SkillType == 497 ) then
		
			MaxSummonGhost = math.min(4, VillaninLadyMax - table.maxn(VillaninLadyGhostList))
	
			M = DEScriptZone:GetMonster(CreatureInfo.ObjectID)
		
			OffsetArray = 
			{
				{ -2, -2 },
				{  2, 2 },
				{  2, -2 },
				{ -2, 2 }
			}
		
			for index = 1, math.random(1, MaxSummonGhost), 1 do
				Offset = OffsetArray[index]
			
				AddMonsterInfo = VillaninLadyGhost
				AddMonsterInfo.X = M.X + Offset[1]
				AddMonsterInfo.Y = M.Y + Offset[2]
			
				AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
				
				table.insert(VillaninLadyGhostList, AddedMonster.ObjectID)
			end
		end
	end
end

function OnGiveRewardItem(Creature)

	GiveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 31,
		ItemCount = 30,
		Options = "",
		LimitedTime = 0
	}

	if DEScriptZone:GiveItem(Creature, GiveItemInfo) then
	end
end

function StartMisssionOniblaRoom()

	if StageStat ~= STAGE_STATE_RACE_WAR then
		return
	end
	
	DEScript:WriteFileLog(LogFileName, "Start To Attack Onibla")
	WriteCurrentUser()
	
	StageStat = STAGE_STATE_OPEN_ONIBLA_ROOM
	
	OpenBossRoom()
	BossRoomAction()
end

function WriteCurrentUser()

	DEScript:WriteFileLog(
		LogFileName, 
		"StageState="..StageStat..",SlayerCount="..SlayerCount..",VampireCount="..VampireCount..",OustersCount="..OustersCount
	)
end