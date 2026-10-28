dofile("../script/zone/Lacoste/Lacoste_Monsterinfo.lua")
dofile("../script/zone/Lacoste/Lacoste_Util.lua")
dofile("../script/zone/Lacoste/Lacoste_Restrict.lua")
MaxScriptZoneCount = 20--副本最大启动数量
DoungeonOpened = false --副本是否启动
WarpToOtherWorldZoneID = 0 --传送下个副本的地图ID
DoungeonStartTime = 0	-- 0 seconds 副本开始时间
DoungeonLimitTime = 90 * 60 -- 60 minutes 副本限时
DoungeonClear = false   --副本是否所有任务完成
MissionCount = 1       --阶段一小怪数量
CurrentMission = 0      --当前任务
MissionOneMonsterList = {} --统计小怪OBJ
PlayerNameList = {}           --统计玩家名字
NumOfCurrentUser = 0
MobCount = 0

local TempLogFileName = "../script/zone/Lacoste/Lacoste6.log"
local TempItemLogFileName = "../script/zone/Lacoste/Lacoste6_Item.log"

NormalMonsterOne = 
{
	{
		MonsterInfo = NormalMonsterSix,
		MonsterPos = 
		{
			{58,83},
		}
	}
}
function OnInitializeVariables(ZoneCreateInfo)--生成副本地图

	ZoneCreateInfo.Name = "Lacoste6F"	
	ZoneCreateInfo.TemplateZoneID = 6426
	ZoneCreateInfo.StartPosX = 28
	ZoneCreateInfo.StartPosY = 113
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 

function OnInitialize()--初始化副本
	if ( DEScriptZone:GetScriptZoneCount() > MaxScriptZoneCount ) then
		DoungeonOpened = false
		return
	end
	
	DoungeonOpened = true
	DoungeonStartTime = DEScript:GetCurrentTime()
	--StartMission1()
	DEScriptZone:SetTimer(1, 1000 * 5)
	DEScriptZone:SetTimer(10, 1000 * 6, 1)
	DEScriptZone:SetPKZone(false)
end

function OnSetStartPos(CreatureInfo, PositionInfo)--玩家进入场景坐标
		PositionInfo.X = 28
        PositionInfo.Y = 113
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
		DoungeonLimitTime = DEScript:ReadIntegerOnWhiteBoard("Lacoste.6F.RemainTime."..DEScriptZone:GetZoneID())	
	end
	NumOfCurrentUser = NumOfCurrentUser + 1
	remainTime = GetRemainTime() * 10 -- 1/10 seconds
	SendRemiainTimeSix(Creature, remainTime) 
	table.insert(PlayerNameList,Creature.Name)
end

function OnBeforeLeaveZone(Creature)--离开副本之前
	SendRemiainTime(Creature, 0)	
end

function OnAfterLeaveZone(Creature)--离开副本之后
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
    if Mesg == 'mobcount' then
		MobCount = table.maxn(MissionOneMonsterList)
		local MSEEAGE = string.format("当前怪物数量：%s!",MobCount)
		DEScriptZone:BroadcastSystemMessage(0, MSEEAGE)
	end
	return true
end

function OnTimer(TimerID)
        if TimerID == 1 then
	
			remainTime = GetRemainTime()
		 
			if DoungeonClear == false and remainTime < 0 then
				
				DEScriptZone:KillTimer(1)
				
				MissionFailed()--时间到或者任务没有完成 调用副本失败
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
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/LacosteBoss")--创建副本2层
		
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.Boss.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()	
		end
		if TimerID == 10 then
		DEScriptZone:KillTimer(10)
		DEScriptZone:BroadcastSystemMessageInZone(1, "德·萨德侯爵:什么!居然闯入到这里,巨拳怪何在!")
		DEScriptZone:SetTimer(11, 1000 * 5, 1)
		end
		if TimerID == 11 then
		DEScriptZone:KillTimer(11)
		DEScriptZone:BroadcastSystemMessageInZone(1, "巨拳怪:放心吧!主人!我会用他们的鲜血注满您的酒杯!")
		DEScriptZone:BroadcastSystemMessageInZone(1, "巨拳怪:来吧!闯入者!受死!")
		StartMission1()
		end
end

function OnWarpToOtherWorld(Creature)--下一阶段
	if WarpToOtherWorldZoneID ~= 0 then
		DEScriptZone:TransportCreature(Creature, WarpToOtherWorldZoneID, 28, 113)
	end
end

function StartMission1()--任务一 刷小怪
		CurrentMission = 1
	for index ,info in pairs(NormalMonsterOne) do
	    AddMonsterInfo = info.MonsterInfo
        for index, MonsterPos in ipairs(info.MonsterPos) do
			AddMonsterInfo.X = MonsterPos[1]
			AddMonsterInfo.Y = MonsterPos[2]
			for i= 1, MissionCount, 1 do
				AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
				table.insert(MissionOneMonsterList, AddedMonster.ObjectID)	
			end	
		end		
	end
end

function OnKillCreature(AttackerCreature, DeadCreature)--怪物死亡判断
	if CurrentMission == 1 then --当前任务第一阶段 刷效果
		if	DeadCreature.MType == 1683	then
			if CheckMissionAllKill(DeadCreature.ObjectID, MissionOneMonsterList) then --全部杀死小怪
				DEScriptZone:BroadcastSystemMessageInZone(1, "科斯特行宫6层任务完成!")
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
		ItemClass = 41,
		ItemType = 82,
		ItemCount = 3,
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
			DEScriptZone:BroadcastGCSay(Creature,"获得物品:蓝鸟2 x3个.",166666)
			WriteLog(TempItemLogFileName, string.format("%s 获得 蓝鸟2 x3个.", Creature.Name))
		end
end