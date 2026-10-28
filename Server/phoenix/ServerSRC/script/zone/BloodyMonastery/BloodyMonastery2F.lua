dofile("../script/zone/BloodyMonastery/BloodyMonastery_Monsterinfo.lua")
dofile("../script/zone/BloodyMonastery/BloodyMonastery_Util.lua")
dofile("../script/zone/BloodyMonastery/BloodyMonastery_Restrict.lua")


local L_PLAYER_ID_LIST = {}

local Mons_ID = 1798
local Mons_Count = 150
local Mons_KilledCount = 0

local Ready_Timer = 1
local Start_Timer = 2
local Finish_Timer = 3

local Ready_Count = 30
local Finish_Count = 15

local Ready_Time = 1000
local Start_Time = 1000 * 60 * 15 --暂时设置15秒,需要设置成15分钟
local Finish_Time = 1000
local CanEnter = false
local WarpToOtherWorldZoneID = 0

local CurrentMission = 0      --当前任务
local MissionOneMonsterList = {} --统计小怪OBJ

function OnInitializeVariables(ZoneCreateInfo)--生成副本地图
	ZoneCreateInfo.Name = "BloodyMonastery2F"	
	ZoneCreateInfo.TemplateZoneID = 6701
	ZoneCreateInfo.StartPosX = 30
	ZoneCreateInfo.StartPosY = 101
	ZoneCreateInfo.EntranceType = "party"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 


function OnInitialize()--初始化副本
	CanEnter = false
	DEScriptZone:SetPKZone(false)
	DEScriptZone:SetTimer(Ready_Timer, Ready_Time, 1)
	SummonNpcTest(1909,28,103)
end

function OnSetStartPos(CreatureInfo, PositionInfo)--玩家进入场景坐标
		PositionInfo.X = 30
        PositionInfo.Y = 101
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	return false;
end

function OnBeforeEnterZone(Creature)--进入副本之前
	HaveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 66,
		ItemCount = 1
	}
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
	if AdvLevel < 130 then
	DEScriptZone:SendSystemMessage(Creature, 0, "等级低于280级不可以进入.")

	return false
	end
	if CanEnter == false then
		if DEScriptZone:HaveItem(Creature, HaveItemInfo) then
			TakeItemInfo = {}
			TakeItemInfo = HaveItemInfo
			DEScriptZone:TakeItem(Creature, TakeItemInfo)
			CanEnter = true
			return true
		end
		DEScriptZone:SendSystemMessage(Creature, 0, "你没有灵魂石,无法进入.")
	else
		return true-- CanEnter == true all player can enter
	end
	return false

	--end
end

function OnAfterEnterZone(Creature)--进入副本之后
		local player = {Info = Creature}
		L_PLAYER_ID_LIST[ Creature.ObjectID ] = player
end

function OnBeforeLeaveZone(Creature)--离开副本之前
	local ID = Creature.ObjectID;
	L_PLAYER_ID_LIST[ ID ] = nil; 
end

function OnAfterLeaveZone(Creature)--离开副本之后

end

function SummonMonster(ID, Name, x, y)
	local monsInfo = 
	{
		MType = ID,
		Name = Name,
		X = x,
		Y = y,
		Dir = 2,

		AI = "",

		DeadAI = "",
		Enhance = ""
	}
	
	AddedMonster = DEScriptZone:AddMonster(monsInfo)
	table.insert(MissionOneMonsterList, AddedMonster.ObjectID)
	
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
	if Mesg == '1' then
		if CanEnter == false then
			DEScriptZone:SendSystemMessage(Creature, 0, "你没有灵魂石,无法进入.")
		end
	end
	return true
end

function SummonAllMonster()
	local PositionInfo = {}
	
	for i=1,Mons_Count,1 do
		if DEScriptZone:FindPosition(Mons_ID, PositionInfo) == true then
	        SummonMonster(Mons_ID, "小怪", PositionInfo.X, PositionInfo.Y)
		end
	end
end

function SendRemiainToAll(time)
	for i,Player in pairs(L_PLAYER_ID_LIST) do
		SendRemiainTimeTwo(Player.Info, time/100)
	end			
end

function OnTimer(TimerID)
    if TimerID == Ready_Timer then
	    Ready_Count = Ready_Count - 1
	    local Msg = string.format("%d 秒后开始刷怪", Ready_Count)
	    BroadMsgToAll( Msg )
		if Ready_Count == 0 then
			CurrentMission = 1
			SummonAllMonster()
			DEScriptZone:SetTimer(Start_Timer, Start_Time, 1)
			SendRemiainToAll(Start_Time)
		    DEScriptZone:KillTimer(TimerID)
			DEScriptZone:BroadcastSystemMessageInZone(8,"任务完成条件：杀死地图上有怪物!")
			--DEScriptZone:recallparty(Player.Info,28,113)
		end
	end
	
	if TimerID == Start_Timer then
	    --执行到这里 就等于任务失败 传送出去
			OnTraverseCreature = OnWarpToGetOut
			DEScriptZone:TraverseCreature()	
			DEScriptZone:KillTimer(TimerID)
	end
	
	if TimerID == Finish_Timer then
		Finish_Count = Finish_Count - 1
	    local Msg = string.format("%d 秒后传送到下一层", Finish_Count)
	    BroadMsgToAll( Msg )		
		if Finish_Count == 0 then
			WarpToOtherWorldZoneID = DEScriptZone:CreateInstanceDoungeon("BloodyMonastery/BloodyMonastery3F")--创建副本3层
			OnTraverseCreature = OnWarpToOtherWorld--传送玩家到下一副本
			DEScriptZone:TraverseCreature()	
		end
	end
end


function BroadMsgToAll(Msg)
	for i,Player in pairs(L_PLAYER_ID_LIST) do
		DEScriptZone:SendSystemMessage(Player.Info,0,Msg)
	end		
end

function OnWarpToOtherWorld(Creature)--下一阶段
	
	if WarpToOtherWorldZoneID ~= 0 then
		DEScriptZone:TransportCreature(Creature, WarpToOtherWorldZoneID, 22, 44)
	end
	
end


function OnKillCreature(AttackerCreature, DeadCreature)--怪物死亡判断
	if CurrentMission == 1 then --当前任务第一阶段 刷效果
		if DeadCreature.MType == Mons_ID then
			if CheckMissionAllKill(DeadCreature.ObjectID, MissionOneMonsterList) then --全部杀死小怪
				DEScriptZone:BroadcastSystemMessageInZone(1, "恭喜，挑战成功!")
				MissionSuccess()--挑战成功
			end
		end
	end
	MobCount = table.maxn(MissionOneMonsterList)
	local MSEEAGE = string.format("当前怪物剩余数量：%s!",MobCount)
	DEScriptZone:BroadcastSystemMessageInZone(0, MSEEAGE)
end

function MissionSuccess()--怪物死亡判断
		DEScriptZone:KillTimer(Start_Timer)
		SendRemiainToAll(0)
		DEScriptZone:SetTimer(Finish_Timer, Finish_Time, 1)
		OnTraverseCreature = OnGiveRewardItem
		DEScriptZone:TraverseCreature()		
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
			WriteLog(TempItemLogFileName, string.format("%s 获得 经验值:1亿.", Creature.Name))
		else 
		    DEScriptZone:GiveItem(Creature, GiveItemInfo)
			DEScriptZone:BroadcastGCSay(Creature,"您获得物品:莉莉丝宝石1x10.",166666)
			WriteLog(TempItemLogFileName, string.format("%s 获得 莉莉丝宝石1x10", Creature.Name))
		end
end