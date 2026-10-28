dofile("../script/zone/Lacoste/Lacoste_Monsterinfo.lua")
dofile("../script/zone/Lacoste/Lacoste_Util.lua")
dofile("../script/zone/Lacoste/Lacoste_Restrict.lua")
WarpToOtherWorldZoneID = 0
DoungeonStartTime = 0	-- 0 seconds
DoungeonLimitTime = 90 * 60 -- 60 minutes
DoungeonClear = false 
MissionCount = 20
CurrentMission = 0
MissionOneMonsterList = {}
MissionOneBossMonsterList = {}
PlayerNameList = {}
PlayerInfoList = {}
NumOfCurrentUser = 0
SelectPlayer = nil
NameList = {}
AgreeCount = 0
local CanSay = true

local TempLogFileName = "../script/zone/Lacoste/Lacoste4.log"
local TempItemLogFileName = "../script/zone/Lacoste/Lacoste4_Item.log"

function OnInitializeVariables(ZoneCreateInfo)--生成副本地图

	ZoneCreateInfo.Name = "Lacoste4F"	
	ZoneCreateInfo.TemplateZoneID = 6424
	ZoneCreateInfo.StartPosX = 28
	ZoneCreateInfo.StartPosY = 113
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 

function OnInitialize()--初始化副本
	DoungeonStartTime = DEScript:GetCurrentTime()
	
	DEScriptZone:SetTimer(1, 1000 * 5)
	DEScriptZone:SetTimer(10, 1000 * 6, 1)
	
	DEScriptZone:SetPKZone(false)
end

function OnBeforeEnterZone(Creature)--进入副本之前
	return true
end

function OnAfterEnterZone(Creature)--进入副本之后
	if NumOfCurrentUser == 0 then
		DoungeonLimitTime = DEScript:ReadIntegerOnWhiteBoard("Lacoste.4F.RemainTime."..DEScriptZone:GetZoneID())
		
	end
	NumOfCurrentUser = NumOfCurrentUser + 1
	PlayerInfoList[Creature.ObjectID] =  Creature

	remainTime = GetRemainTime() * 10 -- 1/10 seconds
	
	SendRemiainTimeFour(Creature, remainTime) 
	table.insert(PlayerNameList,Creature.Name)
end

function OnBeforeLeaveZone(Creature)--离开副本之前
	SendRemiainTimeFour(Creature, 0)
end

function OnAfterLeaveZone(Creature)--离开副本之后
    NumOfCurrentUser = NumOfCurrentUser - 1
    PlayerInfoList[Creature.ObjectID] =  nil
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	WriteLog(TempLogFileName, string.format("%s 死亡离开副本", CreatureInfo.Name))
	return false;
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
	if SelectPlayer == nil then
	    return
	end
	if CanSay == true then
		if Mesg == "同意" and CreatureInfo ~= SelectPlayer then
		--   DEScriptZone:BroadcastSystemMessageInZone(4, " 11111111111111")
		   
		   
		   if NameList[CreatureInfo.ObjectID] ~= CreatureInfo then --把不同意的人加进列表
			  NameList[CreatureInfo.ObjectID] = CreatureInfo
			  AgreeCount = AgreeCount + 1
			  DEScriptZone:BroadcastSystemMessageInZone(1, CreatureInfo.Name.."  表示同意")
			  if AgreeCount >= NumOfCurrentUser - 1 then
				  DEScriptZone:KillTimer(13)
				  DEScript:SetHP(SelectPlayer, 0)
				  MissionSuccessPass()
			  end
		   end
		   
		end
	end
	
	return true
end

function OnWarpToOtherWorld(Creature)--下一阶段传送
	if WarpToOtherWorldZoneID ~= 0 and DEScript:IsAlive(Creature) == true then
		DEScriptZone:TransportCreature(Creature, WarpToOtherWorldZoneID, 28, 113)
	end
end

function OnTimer(TimerID)
        if TimerID == 1 then
	
			remainTime = GetRemainTime()
		 
			if DoungeonClear == false and remainTime < 0 then
				
				DEScriptZone:KillTimer(1)
				
				MissionFailed()
			end
			
			if remainTime < 60 * 5 then
			
			end
		end
		
		if TimerID == 2 then--失败传送出去
			OnTraverseCreature = OnWarpToGetOut
			DEScriptZone:TraverseCreature()	
		end
		
		if TimerID == 3 then--挑战成功 进入下一副本
			DEScriptZone:KillTimer(3)
			
			OnTraverseCreature = NotifyWarpToWorld--启动屏幕效果 抖动 黑暗
			DEScriptZone:TraverseCreature()

			DEScriptZone:SetTimer(4, 1000 * 15, 1)--启动下一副本
			
		end
		
		if TimerID == 4 then
			WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste5F")
			
			DEScript:WriteIntegerOnWhiteBoard("Lacoste.5F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())
			
			OnTraverseCreature = OnWarpToOtherWorld
			DEScriptZone:TraverseCreature()	
		end
		
		if TimerID == 10 then
			DEScriptZone:BroadcastSystemMessageInZone(1, "德·萨德侯爵:止步!你们已经让我非常愤怒!")
			DEScriptZone:KillTimer(10)
			DEScriptZone:SetTimer(11, 1000 * 5, 1)
		end
		
		if TimerID == 11 then
			SelectPlayer = GetRandomPlayer()
			DEScriptZone:BroadcastSystemMessageInZone(1, "德·萨德侯爵:如果你们能把["..SelectPlayer.Name.."]献祭给我,我会好好奖励你们!")
			DEScriptZone:KillTimer(11)
			DEScriptZone:SetTimer(12, 1000 * 5, 1)			
		end
		
		if TimerID == 12 then
			DEScriptZone:BroadcastSystemMessageInZone(1, "快点决定吧,如果同意,输入:同意,不要浪费时间了,只有鲜血才可以弥补你们的过错!")
			DEScriptZone:BroadcastSystemMessageInZone(8, "玩家["..SelectPlayer.Name.."]已被点名献祭,你可以输入:同意 或者 不同意!.")
			DEScriptZone:KillTimer(12)
			DEScriptZone:SetTimer(13, 1000 * 20, 1)			
		end
		
		if TimerID == 13 then
			DEScriptZone:KillTimer(13)
			DEScriptZone:BroadcastSystemMessageInZone(1, "技术:不错,没有放弃任何队友,但是只有鲜血才可以打破幻境,献祭我吧!当然先得打败我！")
			OnTraverseCreature = ScreenShake--启动屏幕效果 抖动 黑暗
			DEScriptZone:TraverseCreature()
			StartMission1() --刷BOSS
		end
end

function GetRandomPlayer()
    local index = math.random(0, NumOfCurrentUser - 1)
    local j = 0;
    
	for i,PlayerInfo in pairs(PlayerInfoList) do
			if j == index then
			    return PlayerInfo
			end
			
			j = j+1;
    end
    return 0
end

function StartMission1()--刷技术1或者技术2

	SelectPlayer = nil --取消点名的玩家

	randPlayer = GetRandomPlayer()
	
	if randPlayer == nil then
	    return
	end

	if math.random(0, 1) == 1 then
		AddMonsterInfo = NormalMonsterFour
	else
		AddMonsterInfo = MonsterFour
	end
	
	AddMonsterInfo.X = DEScript:playerX(randPlayer) + math.random(-5, 5)
	AddMonsterInfo.Y = DEScript:playerY(randPlayer) + math.random(-5, 5)	
	AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)		

end

function OnKillCreature(AttackerCreature, DeadCreature)--怪物死亡判断
	if DeadCreature.MType == 1675 or DeadCreature.MType == 1676 then
		MissionSuccess()
	end
end

function MissionSuccessPass()--挑战成功
	DoungeonClear = true	
	CanSay = false
	for i,Name in pairs(PlayerNameList) do
	PlayerName = Name
	end
	DEScriptZone:BroadcastSystemMessageInZone(1, "恭喜您!挑战成功!,5秒后传送下一阶段.")
	DEScriptZone:SetTimer(3, 1000 * 5, 1)

end

function MissionSuccess()--挑战成功
	DoungeonClear = true	
	
	OnTraverseCreature = OnGiveRewardItem
	DEScriptZone:TraverseCreature()
	for i,Name in pairs(PlayerNameList) do
	PlayerName = Name
	end
	DEScriptZone:BroadcastSystemMessageInZone(1, "恭喜您!挑战成功!,5秒后传送下一阶段.")
	DEScriptZone:SetTimer(3, 1000 * 5, 1)

end

function MissionFailed()--挑战失败
	DEScriptZone:BroadcastSystemMessageInZone(1, "挑战失败,10秒后传送出副本")
	DEScriptZone:SetTimer(2, 1000 * 10, 1)
	
end

function OnGiveRewardItem(Creature)

	GiveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 63,
		ItemCount = 10,
		Options = "",
		LimitedTime = 0
	}
	
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
		if AdvLevel < 200 then
			DEScriptZone:GiveExpAdv(Creature,100000000)
			DEScriptZone:BroadcastGCSay(Creature,"获得经验值:1亿.",166666)
			WriteLog(TempItemLogFileName, string.format("%s 获得 经验值:1亿.", Creature.Name))
		else 
		    DEScriptZone:GiveItem(Creature, GiveItemInfo)
			DEScriptZone:BroadcastGCSay(Creature,"获得物品:烧毁的古籍 x10.",166666)
			WriteLog(TempItemLogFileName, string.format("%s 获得 烧毁的古籍 x10.", Creature.Name))
		end
end