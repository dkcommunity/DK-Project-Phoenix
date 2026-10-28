dofile("../script/zone/Lacoste/Lacoste_Monsterinfo.lua")
dofile("../script/zone/Lacoste/Lacoste_Util.lua")
dofile("../script/zone/Lacoste/Lacoste_Restrict.lua")
WarpToOtherWorldZoneID = 0
DoungeonStartTime = 0	-- 0 seconds
DoungeonLimitTime = 90 * 60 -- 60 minutes
DoungeonClear = false 
MissionCount = 6
CurrentMission = 0
MissionOneMonsterList = {}
MissionOneBossMonsterList = {}
PlayerNameList = {}
NumOfCurrentUser = 0

TwinNameList = {}
TwinNameToIDList = {}
TwinNameCountList = {}
TwinPos = {}
TwinCount = {}
TwinsKill = 0
TWIN_TIME_START = 10

local TempLogFileName = "../script/zone/Lacoste/Lacoste5.log"
local TempItemLogFileName = "../script/zone/Lacoste/Lacoste5_Item.log"


function OnInitializeVariables(ZoneCreateInfo)--生成副本地图

	ZoneCreateInfo.Name = "Lacoste5F"	
	ZoneCreateInfo.TemplateZoneID = 6425
	ZoneCreateInfo.StartPosX = 28
	ZoneCreateInfo.StartPosY = 113
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
	
	TwinNameList[0] = "双子0"
	TwinNameList[1] = "双子1"
	TwinNameList[2] = "双子2"
	TwinNameList[3] = "双子3"
	TwinNameList[4] = "双子4"
	TwinNameList[5] = "双子5"
	
	TwinPos[0] = {47, 90}
	TwinPos[1] = {57, 80}
	TwinPos[2] = {62, 74}
	TwinPos[3] = {69, 68}
	TwinPos[4] = {76, 61}
	TwinPos[5] = {96, 30}
	
	TwinNameToIDList["双子0"] = 0
	TwinNameToIDList["双子1"] = 1
	TwinNameToIDList["双子2"] = 2
	TwinNameToIDList["双子3"] = 3
	TwinNameToIDList["双子4"] = 4
	TwinNameToIDList["双子5"] = 5
	
	TwinNameCountList["双子0"] = 0
	TwinNameCountList["双子1"] = 0
	TwinNameCountList["双子2"] = 0
	TwinNameCountList["双子3"] = 0
	TwinNameCountList["双子4"] = 0
	TwinNameCountList["双子5"] = 0
end 

function OnInitialize()--初始化副本
	DoungeonStartTime = DEScript:GetCurrentTime()
	StartMission1()
	DEScriptZone:SetTimer(1, 1000 * 5)
	
	DEScriptZone:SetPKZone(false)
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	WriteLog(TempLogFileName, string.format("%s 死亡离开副本", CreatureInfo.Name))
	return false;
end

function OnBeforeEnterZone(Creature)--进入副本之前
	return true
end

function OnAfterEnterZone(Creature)--进入副本之后
	if NumOfCurrentUser == 0 then
		-- 贸澜 甸绢坷绰 某腐磐甫 扁霖栏肺 巢篮 矫埃 汲沥
		DoungeonLimitTime = DEScript:ReadIntegerOnWhiteBoard("Lacoste.5F.RemainTime."..DEScriptZone:GetZoneID())
		
	end
	NumOfCurrentUser = NumOfCurrentUser + 1
	

	remainTime = GetRemainTime() * 10 -- 1/10 seconds
	
	SendRemiainTimeFire(Creature, remainTime) 
	table.insert(PlayerNameList,Creature.Name)
end

function OnBeforeLeaveZone(Creature)--离开副本之前
	SendRemiainTimeFire(Creature, 0)
end

function OnAfterLeaveZone(Creature)--离开副本之后
    NumOfCurrentUser = NumOfCurrentUser - 1
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
	return true
end

function OnWarpToOtherWorld(Creature)--下一阶段传送
	if WarpToOtherWorldZoneID ~= 0 then
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
			WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste6F")
			
			DEScript:WriteIntegerOnWhiteBoard("Lacoste.6F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())
			
			OnTraverseCreature = OnWarpToOtherWorld
			DEScriptZone:TraverseCreature()	
		end
		
		if TimerID >= 10 and TimerID <= 15 then
			DEScriptZone:BroadcastSystemMessageInZone(1, "复活吧 我的兄弟" )
			SummonTwin( TwinNameToIDList[ TwinNameList[TimerID - TWIN_TIME_START] ] )
			DEScriptZone:KillTimer(TimerID)
		end
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )
	if TwinNameCountList[ DefenderCreatureInfo.Name ] ~= nil then
		if math.random(0, 100) < 40 then
			SummonOther( TwinNameToIDList[DefenderCreatureInfo.Name] )
		end
	end 
end

function SummonOther(index)
		AddMonsterInfo = MonsterFive
		AddMonsterInfo.X = TwinPos[index][1] + math.random(-5, 5)
		AddMonsterInfo.Y = TwinPos[index][2] + math.random(-5, 5)
        AddMonsterInfo.Name = "小怪"
        AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)	
end

function SummonTwin(index)

		AddMonsterInfo = NormalMonsterFive
		
		if TwinNameCountList[ TwinNameList[index] ] >= 1 then --大于1,位置随机
			AddMonsterInfo.X = TwinPos[index][1] + 5
			AddMonsterInfo.Y = TwinPos[index][2] + 5
			--DEScriptZone:BroadcastSystemMessage(0, "大于1,位置随机")
		else
			AddMonsterInfo.X = TwinPos[index][1]
			AddMonsterInfo.Y = TwinPos[index][2]
		end
        AddMonsterInfo.Name = TwinNameList[index]
        AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)	
        TwinNameCountList[ TwinNameList[index] ] = TwinNameCountList[ TwinNameList[index] ] + 1
end

function StartMission1()--刷双子
		CurrentMission = 1
		AddMonsterInfo = NormalMonsterFive
        for i= 0, 5, 1 do
          SummonTwin(i)
          SummonTwin(i)
		end		  
end

function OnKillCreature(AttackerCreature, DeadCreature)--怪物死亡判断
	if CurrentMission == 1 then --当前任务第一阶段 刷效果
		if TwinNameCountList[ DeadCreature.Name ] ~= nil then
			TwinNameCountList[ DeadCreature.Name ] = TwinNameCountList[ DeadCreature.Name ] - 1
			if TwinNameCountList[ DeadCreature.Name ] == 0 then
			    DEScriptZone:KillTimer( TwinNameToIDList[DeadCreature.Name] + TWIN_TIME_START ) --删除计时器
				TwinsKill = TwinsKill + 2
				--DEScriptZone:BroadcastSystemMessageInZone(4, "1对双子已经被杀死！！！TwinsKill ="..TwinsKill)
				
				if TwinsKill == 12 then
					MissionSuccess()
				end
				
			else
				--还有一个双子没死，那么开始计时
				DEScriptZone:SetTimer(TwinNameToIDList[DeadCreature.Name] + TWIN_TIME_START, 1000 * 5, 1)--复活
			end
		end
	end

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