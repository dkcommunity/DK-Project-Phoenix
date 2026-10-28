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
NumOfCurrentUser = 0
PlayerName = ""

local TempLogFileName = "../script/zone/Lacoste/LacosteBoss.log"
local TempItemLogFileName = "../script/zone/Lacoste/LacosteBoss_Item.log"

function OnInitializeVariables(ZoneCreateInfo)--生成副本地图

	ZoneCreateInfo.Name = "LacosteBoss"	
	ZoneCreateInfo.TemplateZoneID = 6431
	ZoneCreateInfo.StartPosX = 28
	ZoneCreateInfo.StartPosY = 113
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 

function OnInitialize()--初始化副本
	DoungeonStartTime = DEScript:GetCurrentTime()
	--StartMission1()
	DEScriptZone:SetTimer(1, 1000 * 5)
	DEScriptZone:SetTimer(10, 1000 * 6, 1)	
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
		DoungeonLimitTime = DEScript:ReadIntegerOnWhiteBoard("Lacoste.Boss.RemainTime."..DEScriptZone:GetZoneID())
		
	end
	NumOfCurrentUser = NumOfCurrentUser + 1
	

	remainTime = GetRemainTime() * 10 -- 1/10 seconds
	
	SendRemiainTimeBoss(Creature, remainTime) 
	table.insert(PlayerNameList,Creature.Name)
end

function OnBeforeLeaveZone(Creature)--离开副本之前
	SendRemiainTimeBoss(Creature, 0)
end

function OnAfterLeaveZone(Creature)--离开副本之后
    NumOfCurrentUser = NumOfCurrentUser - 1
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
	return true
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
			OnTraverseCreature = OnWarpToGetOut
			DEScriptZone:TraverseCreature()	
		end
		if TimerID == 10 then
		DEScriptZone:KillTimer(10)
		DEScriptZone:BroadcastSystemMessageInZone(1, "德·萨德侯爵:看来我小看你们了!居然拥有如此强大的力量,我感觉非常满意!")
		DEScriptZone:SetTimer(11, 1000 * 5, 1)
		end
		if TimerID == 11 then
		DEScriptZone:KillTimer(11)
		DEScriptZone:BroadcastSystemMessageInZone(1, "德·萨德侯爵:喔!鲜血拿来!")
		StartMission1()
		end
end
function StartMission1()--刷小怪
		CurrentMission = 1
		AddMonsterInfo = NormalMonsterBOOS
		AddMonsterInfo.X = 59
		AddMonsterInfo.Y = 82
		AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
		table.insert(MissionOneMonsterList, AddedMonster.ObjectID)
end

function StartMission2()--刷BOSS
		CurrentMission = 2
		AddMonsterInfo = MonsterBOOS
		AddMonsterInfo.X = 101
		AddMonsterInfo.Y = 30
		AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
        table.insert(MissionOneBossMonsterList, AddedMonster.ObjectID)	  
		DEScriptZone:BroadcastSystemMessageInZone(1, "暗黑议会大长老:哈哈,谢谢你们干掉了 德·萨德!他的一切,还有你们都是我的,我的!~")		
end

function OnKillCreature(AttackerCreature, DeadCreature)--怪物死亡判断
	if CurrentMission == 1 then --当前任务第一阶段 刷效果
		if	DeadCreature.MType == 1701	then
			if CheckMissionAllKill(DeadCreature.ObjectID, MissionOneMonsterList) then --全部杀死小怪
				DEScriptZone:BroadcastSystemMessageInZone(8, "潜伏的大长老觉察到德·萨德的死亡,已经觉醒!")
				StartMission2()--开启第二阶段任务 BOSS战
			end
		end
	end
	if CurrentMission == 2 then--当前任务为第二阶段
		if	DeadCreature.MType == 1702	then
			if CheckMissionAllKill(DeadCreature.ObjectID, MissionOneBossMonsterList) then--杀死BOSS
				MissionSuccess()--挑战成功
			end
		end
    end
end

function MissionSuccess()--挑战成功
	DoungeonClear = true	
	
	OnTraverseCreature = OnGiveRewardItem
	DEScriptZone:TraverseCreature()
	for i,Name in pairs(PlayerNameList) do
			PlayerName = PlayerName.." "..Name
	end
	local MSEEAGE = string.format("[恭喜]:玩家：%s ,挑战科斯特行宫成功!",PlayerName)
	DEScriptZone:BroadcastSystemMessage(1, MSEEAGE)
	DEScriptZone:BroadcastSystemMessageInZone(1, "恭喜您!挑战成功!,5秒后传送下一阶段.")
	DEScriptZone:SetTimer(3, 1000 * 5, 1)

end

function MissionFailed()--挑战失败
	DEScriptZone:BroadcastSystemMessageInZone(0, "挑战失败,10秒后传送出副本")
	DEScriptZone:SetTimer(2, 1000 * 10, 1)
	
end

function OnGiveRewardItem(Creature)

	GiveItemInfo = 
	{
		ItemClass = 40,
		ItemType = 48,
		ItemCount = 1,
		Options = "",
		LimitedTime = 0
	}
	
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
		if AdvLevel < 200 then
			DEScriptZone:GiveExpAdv(Creature,100000000)
			DEScriptZone:SendSystemMessage(Creature, 0, "您获得经验值:1亿.")
			WriteLog(TempItemLogFileName, string.format("%s 获得 经验值:1亿.", Creature.Name))
			
		    DEScriptZone:GiveItem(Creature, GiveItemInfo)
			DEScriptZone:SendSystemMessage(Creature, 0, "您获得物品:神奇的石头 x1个.")
			WriteLog(TempItemLogFileName, string.format("%s 获得 神奇的石头 x1个", Creature.Name))
		else 
		    DEScriptZone:GiveItem(Creature, GiveItemInfo)
			DEScriptZone:SendSystemMessage(Creature, 0, "您获得物品:神奇的石头 x1个")
			WriteLog(TempItemLogFileName, string.format("%s 获得 神奇的石头 x1个", Creature.Name))
		end
end