dofile("../script/zone/BloodyMonastery1F/1F_Restrict.lua")

local L_PLAYER_ID_LIST = {}

local Mons_ID = 1800
local Boss_ID = 1803
local Mons_Count = 499
local Boss_Count = 1
local Mons_Dead_Count = 500
local TeamID = 0
local BOSS_NUM = 0

local NpcInfo= {
	NpcID = 1818,
	Name = "死亡修士",
	PosX = 237,
	PosY = 9,
	Dir = 2
}
function SummonNpcTest (ID,X,Y)
		local NpcInfo=
		 {
			NpcID = ID,
			Name = "苛鸥何胶档快固1",
			PosX = X,
			PosY = Y,
			Dir = 2
		}
	DEScriptZone:addNPC(NpcInfo)

end
function OnInitializeVariables(ZoneCreateInfo)--生成副本地图
	ZoneCreateInfo.Name = "BloodyMonastery1F"	
	ZoneCreateInfo.TemplateZoneID = 6700
	ZoneCreateInfo.StartPosX = 20
	ZoneCreateInfo.StartPosY = 232
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = true
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 

function OnInitialize()--初始化副本
	DEScriptZone:SetPKZone(true)
	ClearMobNum()
	local PositionInfo = {}
	
	for i=0,Mons_Count,1 do
		if DEScriptZone:FindPosition(Mons_ID, PositionInfo) == true then
	        SummonMonster(Mons_ID, "小怪", PositionInfo.X, PositionInfo.Y)
		end
	end
	--DEScriptZone:addNPC(NpcInfo)
end

function OnSetStartPos(CreatureInfo, PositionInfo)--玩家进入场景坐标
		PositionInfo.X = 20
        PositionInfo.Y = 232
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	
		ZoneCoordInfo.ZoneID = DEScriptZone:GetZoneID()
		ZoneCoordInfo.X = 20
		ZoneCoordInfo.Y = 232
		
		return true;
end

function OnBeforeEnterZone(Creature)--进入副本之前
	HaveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 65,
		ItemCount = 1
	}
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
	if AdvLevel < 130 then
	DEScriptZone:SendSystemMessage(Creature, 0, "等级低于280级不可以进入.")

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
	
	DEScriptZone:AddMonster(monsInfo)
	
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
	local X = DEScript:playerX(CreatureInfo)
	local Y = DEScript:playerY(CreatureInfo)
	if DEScript:IsGod(CreatureInfo) then
		if Mesg == 'resetmob' then
			ClearMobNum()
			local MSEEAGE = string.format("当前击杀怪物数量：%s!",Mons_Dead_Count)
			DEScriptZone:BroadcastSystemMessageInZone(0,MSEEAGE )
		end
--[[		SummonNpcTest(Mesg,X,Y)--]]
	end
	return true
end

function OnWarpToOtherWorld(Creature)--下一阶段
	if WarpToOtherWorldZoneID ~= 0 then
		DEScriptZone:TransportCreature(Creature, WarpToOtherWorldZoneID, 28, 113)
	end
end
function OnKillCreature(AttackerCreature, DeadCreature)

	local PositionInfo = {}


	 if DeadCreature.MType == Mons_ID then
		if Mons_Dead_Count > 0 then
		Mons_Dead_Count = Mons_Dead_Count-1
		end
	    if DEScriptZone:FindPosition(Mons_ID, PositionInfo) == true then
	        SummonMonster(Mons_ID, "小怪", PositionInfo.X, PositionInfo.Y)
			local MSEEAGE = string.format("怪物击杀剩余数量：%s!",Mons_Dead_Count)
			DEScriptZone:BroadcastSystemMessageInZone(0,MSEEAGE )
		end
	end
	
	if Mons_Dead_Count == 0 and BOSS_NUM == 0 then
		if DEScriptZone:FindPosition(Boss_ID, PositionInfo) == true then
			BOSS_NUM = 1
			SummonMonster(Boss_ID, "BOSS", PositionInfo.X, PositionInfo.Y)
			local BossMsg = string.format("警告:未觉醒的希尔出现在X:%s,Y:%s!", PositionInfo.X,PositionInfo.Y)
			DEScriptZone:BroadcastSystemMessageInZone(1,BossMsg)
		end
	end
	
end

function OnPK(AttackerCreature, DeadCreature)--怪物死亡判断

	if DeadCreature.MType == Boss_ID then
		TeamID = DEScript:TeamID(AttackerCreature)
	    ClearMobNum()
		local Msg = string.format("%s的队伍 击败了 未觉醒的希尔", AttackerCreature.Name)
		DEScriptZone:BroadcastSystemMessageInZone(1,Msg)
		local MsgTwo = string.format("提示:%s的队伍 快去寻找死亡修士,对话后队伍进入下一层!", AttackerCreature.Name)
		DEScriptZone:BroadcastSystemMessageInZone(8,MsgTwo)
		CheckTeamID(AttackerCreature)
	end
end

function ClearMobNum()
	Mons_Dead_Count = 500
	BOSS_NUM = 0
end
function OnGiveRewardItemTeam(Creature)

	GiveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 66,
		ItemCount = 1,
		Options = "",
		LimitedTime = 3600
	}
	if TeamID == DEScript:TeamID(Creature) then
		DEScriptZone:GiveItem(Creature, GiveItemInfo)
	end
	
end

function OnGiveRewardItemPlayer(Creature)

	GiveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 66,
		ItemCount = 1,
		Options = "",
		LimitedTime = 3600
	}
	DEScriptZone:GiveItem(Creature, GiveItemInfo)
end

function CheckTeamID(Creature)
	TeamID = DEScript:TeamID(Creature)
	if TeamID == 0 then
		OnGiveRewardItemPlayer(Creature)
	else
		OnTraverseCreature = OnGiveRewardItemTeam
		DEScriptZone:TraverseCreature()	
	end
end