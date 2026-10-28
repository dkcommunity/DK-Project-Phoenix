dofile("../script/zone/Lacoste/Lacoste_Monsterinfo.lua")
dofile("../script/zone/Lacoste/Lacoste_Util.lua")
dofile("../script/zone/Lacoste/Lacoste_Restrict.lua")
MaxScriptZoneCount = 20--副本最大启动数量
DoungeonOpened = false --副本是否启动
WarpToOtherWorldZoneID = 0 --传送下个副本的地图ID
DoungeonStartTime = 0	-- 0 seconds 副本开始时间
DoungeonLimitTime = 90 * 60 -- 60 minutes 副本限时
DoungeonClear = false   --副本是否所有任务完成
MissionCount = 20       --阶段一小怪数量
CurrentMission = 0      --当前任务
MissionOneMonsterList = {} --统计小怪OBJ
PlayerNameList = {}           --统计玩家名字
MobCount = 0

local TempLogFileName = "../script/zone/Lacoste/Lacoste1.log"
local TempItemLogFileName = "../script/zone/Lacoste/Lacoste1_Item.log"

NormalMonsterOne = 
{
	{
		MonsterInfo = NormalMonsterOne,
		MonsterPos = 
		{
			{32,42},
			{58,83},
			{88,96},
			{86,50},
			{97,33}
		}
	}
}
function OnInitializeVariables(ZoneCreateInfo)--生成副本地图

	ZoneCreateInfo.Name = "Lacoste1F"	
	ZoneCreateInfo.TemplateZoneID = 6421
	ZoneCreateInfo.StartPosX = 28
	ZoneCreateInfo.StartPosY = 113
	ZoneCreateInfo.EntranceType = "party"-- indivisual(单人), party(组队)
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
	--if  table.maxn(PlayerNameList) > 0 then
	--	DEScriptZone:SendSystemMessage(Creature, 0, "副本已经开启,禁止进入")
	--	return false;
	--else
	HaveItemInfo = 
	{
		-- 康氰茄 辑
		ItemClass = 91,
		ItemType = 62,
		ItemCount = 1
	}
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
	if AdvLevel < 110 then
	DEScriptZone:SendSystemMessage(Creature, 0, "等级低于260级不可以进入.")

	return false
	end
	if DEScriptZone:HaveItem(Creature, HaveItemInfo) then
		TakeItemInfo = {}
		TakeItemInfo = HaveItemInfo

		DEScriptZone:TakeItem(Creature, TakeItemInfo)
		return true
	end
	DEScriptZone:SendSystemMessage(Creature, 0, "你没有门票,无法进入.")

	return false

	--end
end

function OnAfterEnterZone(Creature)--进入副本之后
	remainTime = GetRemainTime() * 10 -- 1/10 seconds
	SendRemiainTime(Creature, remainTime) 
	table.insert(PlayerNameList,Creature.Name)
	
	fuhuo =
		{
			EffectCastCreature = nil,
			EffectClass = 799,
		}
                        
	DEScriptZone:RemoveEffectFromCreature(fuhuo, Creature);
end

function OnBeforeLeaveZone(Creature)--离开副本之前
	SendRemiainTime(Creature, 0)
end

function OnAfterLeaveZone(Creature)--离开副本之后

end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
    if Mesg == 'mobcount' then
		MobCount = table.maxn(MissionOneMonsterList)
		local MSEEAGE = string.format("当前怪物剩余数量：%s!",MobCount)
		DEScriptZone:BroadcastSystemMessage(0, MSEEAGE)
	end
	if Mesg == 'go2' then
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste2F")--创建副本2层
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.2F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()
	end	
	if Mesg == 'go3' then
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste3F")--创建副本2层
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.3F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()
	end
	if Mesg == 'go4' then
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste4F")--创建副本2层
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.4F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()
	end
	if Mesg == 'go5' then
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste5F")--创建副本2层
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.5F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()
	end
	if Mesg == 'go6' then
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste6F")--创建副本2层
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.6F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()
	end
	if Mesg == 'goboss' then
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/LacosteBoss")--创建副本2层
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.Boss.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()
	end
	if Mesg == '1' then
		DEScriptZone:GiveRankExp(CreatureInfo,100000000)
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
		WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("Lacoste/Lacoste2F")--创建副本2层
		
		DEScript:WriteIntegerOnWhiteBoard("Lacoste.2F.RemainTime."..WarpToOtherWorldZoneID, GetRemainTime())--传递副本剩余时间
		
		OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
		DEScriptZone:TraverseCreature()	
		end
		if TimerID == 10 then
		DEScriptZone:KillTimer(10)
		DEScriptZone:BroadcastSystemMessageInZone(1, "德·萨德侯爵:啊哈,和我的女仆好好玩玩吧!")
		DEScriptZone:SetTimer(11, 1000 * 5, 1)
		end
		if TimerID == 11 then
		DEScriptZone:KillTimer(11)
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
		DEScriptZone:BroadcastSystemMessageInZone(1, "任务:杀死<德·萨德>侯爵的<罪恶女仆>100个!")
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

--function OnPK( AttackerCreatureInfo, DeadCreatureInfo )
--	DEScriptZone:BroadcastSystemMessageInZone(4, string.format("攻击者的名字是:%s,  死亡者的名字是:%s", AttackerCreatureInfo.Name, DeadCreatureInfo.Name))
--	if DEScript:IsPC( DeadCreatureInfo ) then
		
--	end
--end

function OnKillCreature(AttackerCreature, DeadCreature)--怪物死亡判断
	if CurrentMission == 1 then --当前任务第一阶段 刷效果
		if DeadCreature.MType == 1696 then
			if CheckMissionAllKill(DeadCreature.ObjectID, MissionOneMonsterList) then --全部杀死小怪
				DEScriptZone:BroadcastSystemMessageInZone(1, "科斯特行宫1层任务完成!")
				MissionSuccess()--挑战成功
			end
		end
	end
	MobCount = table.maxn(MissionOneMonsterList)
	local MSEEAGE = string.format("当前怪物剩余数量：%s!",MobCount)
	DEScriptZone:BroadcastSystemMessageInZone(0, MSEEAGE)
end

function MissionSuccess()--挑战成功
	DoungeonClear = true	
	
	OnTraverseCreature = OnGiveRewardItem
	DEScriptZone:TraverseCreature()
	for i,Name in pairs(PlayerNameList) do
	PlayerName = Name
	end
	--local MSEEAGE = string.format("[恭喜]:玩家：%s,挑战科斯特行宫成功!",PlayerName)
	--DEScriptZone:BroadcastSystemMessage(0, MSEEAGE)
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
		ItemType = 56,
		ItemCount = 10,
		Options = "",
		LimitedTime = 0
	}
	
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
		if AdvLevel < 200 then
			DEScriptZone:GiveExpAdv(Creature,100000000)
			DEScriptZone:BroadcastGCSay(Creature,"获得经验值:1亿.",166666)
			WriteLog(TempItemLogFileName, string.format("%s 获得 经验值:1亿", Creature.Name))
		else 
		    DEScriptZone:GiveItem(Creature, GiveItemInfo)
			DEScriptZone:BroadcastGCSay(Creature,"您获得物品:莉莉丝宝石1x10.",166666)
			WriteLog(TempItemLogFileName, string.format("%s 获得 莉莉丝宝石1x10", Creature.Name))
		end
end