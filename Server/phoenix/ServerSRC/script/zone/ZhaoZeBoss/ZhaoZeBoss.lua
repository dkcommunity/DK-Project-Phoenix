dofile("../script/zone/ZhaoZeBoss/ZhaoZeBoss_Monsterinfo.lua")
dofile("../script/zone/ZhaoZeBoss/ZhaoZeBoss_Util.lua")
dofile("../script/zone/ZhaoZeBoss/ZhaoZeBoss_Restrict.lua")

local TempLogFileName = "../script/zone/ZhaoZeBoss/ZhaoZeBoss.log"
local TempItemLogFileName = "../script/zone/ZhaoZeBoss/ZhaoZeBoss_Item.log"

local L_PLAYER_ID_LIST = {}

local Mons_ID = 2003
local Mons_Count = 1
local Mons_KilledCount = 0

local Ready_Timer = 1
local Start_Timer = 2
local Finish_Timer = 3

local Ready_Count = 30
local Finish_Count = 30

local Ready_Time = 1000
local Start_Time = 1000 * 60 * 20 --暂时设置15秒,需要设置成15分钟
local Finish_Time = 1000
local AI = 0
local TotemNum = 0
local TotemNum2 = 0
local TotemNum3 = 0
local TotemNum4 = 0

local Totem_Time = 4
local Totem_Count = 30

local Totem_Time2 = 5
local Totem_Count2 = 30

local Totem_Time3 = 6
local Totem_Count3 = 30

local Totem_Time4 = 7
local Totem_Count4 = 30

local TotemF_MonsterList = {}
local SummonBoss = 0

local BOSS = 
{
	MType = 2003,
	Name = "精灵女王",
	X = 8,--75,
	Y = 25,--50,
	Dir = 2,

	AI = "",

	DeadAI = "",
	Enhance = ""
}
local FireTotem = 
{
	MType = 2004,
	Name = "火焰图腾",
	X = 6,
	Y = 23,
	Dir = 1,

	AI = "",

	DeadAI = "",
	Enhance = ""
}
local WaterTotem = 
{
	MType = 2005,
	Name = "冰霜图腾",
	X = 13,
	Y = 109,
	Dir = 1,

	AI = "",

	DeadAI = "",
	Enhance = ""
}
local EarthTotem = 
{
	MType = 2006,
	Name = "大地图腾",
	X = 122,
	Y = 13,
	Dir = 1,

	AI = "",

	DeadAI = "",
	Enhance = ""
}
local PhysicalTotem = 
{
	MType = 2007,
	Name = "物理图腾",
	X = 117,
	Y = 113,
	Dir = 1,

	AI = "",

	DeadAI = "",
	Enhance = ""
}
function OnInitializeVariables(ZoneCreateInfo)--生成副本地图
	ZoneCreateInfo.Name = "ZhaoZeBoss"	
	ZoneCreateInfo.TemplateZoneID = 1820
	ZoneCreateInfo.StartPosX = 6
	ZoneCreateInfo.StartPosY = 33
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 

function OnInitialize()--初始化副本	
	DEScriptZone:SetTimer(Ready_Timer, Ready_Time, 1)
	DEScriptZone:SetPKZone(false)
end
function ChangePowerToFire()
	if TotemNum == 0 then
		TotemNum = TotemNum + 1
		local Msg = string.format("火焰图腾出现，BOSS火焰攻击提高3倍，尽快破坏！！")
		BroadMsgToAll( Msg )
		AddedMonster = DEScriptZone:AddMonster(FireTotem)
		table.insert(TotemF_MonsterList, AddedMonster.ObjectID)
	end
end
function ChangePowerToWater()
	if TotemNum2 == 0 then
		TotemNum2 = TotemNum2 + 1
		local Msg = string.format("冰霜图腾出现，BOSS冰霜攻击提高3倍，尽快破坏！！")
		BroadMsgToAll( Msg )
		AddedMonster = DEScriptZone:AddMonster(WaterTotem)
		table.insert(TotemF_MonsterList, AddedMonster.ObjectID)
	end
end
function ChangePowerToEarth()
	if TotemNum3 == 0 then
		TotemNum3 = TotemNum3 + 1
		local Msg = string.format("大地图腾出现，BOSS大地攻击提高3倍，尽快破坏！！")
		BroadMsgToAll( Msg )
		AddedMonster = DEScriptZone:AddMonster(EarthTotem)
		table.insert(TotemF_MonsterList, AddedMonster.ObjectID)
	end
end
function ChangePowerToPhysical()
	if TotemNum4 == 0 then
		TotemNum4 = TotemNum4 + 1
		local Msg = string.format("物理图腾出现，BOSS物理攻击提高3倍，尽快破坏！！")
		BroadMsgToAll( Msg )
		AddedMonster = DEScriptZone:AddMonster(PhysicalTotem)
		table.insert(TotemF_MonsterList, AddedMonster.ObjectID)
	end
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
	if Mesg == '挑战' then
	   if SummonBoss == 1 then
	   DEScriptZone:TransportCreature(CreatureInfo, DEScriptZone:GetZoneID(), 61, 63)
	   end
	end
	return true
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )

		if DEScript:IsMonster(DefenderCreatureInfo) and DefenderCreatureInfo.MType == 2003 then
			local MaxHp = CalcRemainHPRate(DefenderCreatureInfo)
			if MaxHp >= 75 then 
				ChangePowerToFire()
				for index, ObjectID in pairs(TotemF_MonsterList) do
					if DEScript:IsAlive(DEScriptZone:GetMonster(ObjectID)) then
						DEScriptZone:setFlag(DefenderCreatureInfo)
					else
						DEScriptZone:removeFlag(DefenderCreatureInfo)
					end
				end
				DEScriptZone:MonsterUseSkill(DefenderCreatureInfo,BossAIFire)
			end
			if MaxHp >=50 and MaxHp < 75 then
				ChangePowerToWater()
				for index, ObjectID in pairs(TotemF_MonsterList) do
					if DEScript:IsAlive(DEScriptZone:GetMonster(ObjectID)) then
						DEScriptZone:setFlag(DefenderCreatureInfo)
					else
						DEScriptZone:removeFlag(DefenderCreatureInfo)
					end
				end				
				DEScriptZone:MonsterUseSkill(DefenderCreatureInfo,BossAIWater)
			end
			if MaxHp >=25 and MaxHp < 50 then
				ChangePowerToEarth()
				for index, ObjectID in pairs(TotemF_MonsterList) do
					if DEScript:IsAlive(DEScriptZone:GetMonster(ObjectID)) then
						DEScriptZone:setFlag(DefenderCreatureInfo)
					else
						DEScriptZone:removeFlag(DefenderCreatureInfo)
					end
				end				
				DEScriptZone:MonsterUseSkill(DefenderCreatureInfo,BossAIEarth)
			end
			if MaxHp < 25 then
				ChangePowerToPhysical()
				for index, ObjectID in pairs(TotemF_MonsterList) do
					if DEScript:IsAlive(DEScriptZone:GetMonster(ObjectID)) then
						DEScriptZone:setFlag(DefenderCreatureInfo)
					else
						DEScriptZone:removeFlag(DefenderCreatureInfo)
					end
				end				
				DEScriptZone:MonsterUseSkill(DefenderCreatureInfo,BossAIPhysical)
			end			
		end
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	return false;
end

function OnBeforeEnterZone(Creature)--进入副本之前
	return true
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

function SummonAllMonster()
	for i=1,Mons_Count,1 do
	        SummonMonster(Mons_ID, "精灵女王", 75, 50)
	end
end

function SendRemiainToAll(time)
	for i,Player in pairs(L_PLAYER_ID_LIST) do
		SendRemiainTimeSix(Player.Info, time/100)
	end			
end

function OnTimer(TimerID)
    if TimerID == Ready_Timer then
	    Ready_Count = Ready_Count - 1
	    local Msg = string.format("%d 秒后开始刷BOSS", Ready_Count)
	    BroadMsgToAll( Msg )
		if Ready_Count == 0 then
			SummonAllMonster()
			SummonBoss = 1
			local Msg = string.format("精灵女王已经出现，输入 挑战 可以进入BOSS平台")
			BroadMsgToAll( Msg )
			DEScriptZone:SetTimer(Start_Timer, Start_Time, 1)
			SendRemiainToAll(Start_Time)
		    DEScriptZone:KillTimer(TimerID)
		end
	end
	
	if TimerID == Totem_Time then
	    Totem_Count = Totem_Count - 1
		local Msg = string.format("火图腾计时 %d 秒", Totem_Count)
		BroadMsgToAll( Msg )
		if Totem_Count == 0 then
			TotemNum = 0
			DEScriptZone:KillTimer(TimerID)
			local Msg = string.format("火图腾计时结束")
			BroadMsgToAll( Msg )
		end
	end
	
	if TimerID == Totem_Time2 then
	    Totem_Count2 = Totem_Count2 - 1
		local Msg = string.format("冰霜图腾计时 %d 秒", Totem_Count)
		BroadMsgToAll( Msg )
		if Totem_Count2 == 0 then
			TotemNum2 = 0
			DEScriptZone:KillTimer(TimerID)
			local Msg = string.format("冰霜图腾计时结束")
			BroadMsgToAll( Msg )
		end
	end
		
	if TimerID == Totem_Time3 then
	    Totem_Count3 = Totem_Count3 - 1
		local Msg = string.format("大地图腾计时 %d 秒", Totem_Count)
		BroadMsgToAll( Msg )
		if Totem_Count3 == 0 then
			TotemNum3 = 0
			DEScriptZone:KillTimer(TimerID)
			local Msg = string.format("大地图腾计时结束")
			BroadMsgToAll( Msg )
		end
	end

	if TimerID == Totem_Time4 then
	    Totem_Count4 = Totem_Count4 - 1
		local Msg = string.format("物理图腾计时 %d 秒", Totem_Count)
		BroadMsgToAll( Msg )
		if Totem_Count4 == 0 then
			TotemNum4 = 0
			DEScriptZone:KillTimer(TimerID)
			local Msg = string.format("物理图腾计时结束")
			BroadMsgToAll( Msg )
		end
	end
	
	if TimerID == Start_Timer then
			OnTraverseCreature = OnWarpToGetOut
			DEScriptZone:TraverseCreature()	
			DEScriptZone:KillTimer(TimerID)
	end
	
	if TimerID == Finish_Timer then
		Finish_Count = Finish_Count - 1
	    local Msg = string.format("%d 秒后传送回城市", Finish_Count)
	    BroadMsgToAll( Msg )		
		if Finish_Count == 0 then
			OnTraverseCreature = OnWarpToGetOut
			DEScriptZone:TraverseCreature()	
			DEScriptZone:KillTimer(TimerID)
		end
	end
end


function BroadMsgToAll(Msg)
	for i,Player in pairs(L_PLAYER_ID_LIST) do
		DEScriptZone:SendSystemMessage(Player.Info,0,Msg)
	end		
end

function OnPK(AttackerCreature, DeadCreature)--怪物死亡判断
	local PositionInfo = {}
	if DeadCreature.MType == 2004 then
			Totem_Count = 30
			DEScriptZone:SetTimer(Totem_Time, 1000, 1)
			local Msg = string.format("火图腾计时开始")
			BroadMsgToAll( Msg )
	end
	if DeadCreature.MType == 2005 then
			Totem_Count2 = 30
			DEScriptZone:SetTimer(Totem_Time2, 1000, 1)
			local Msg = string.format("冰霜图腾计时开始")
			BroadMsgToAll( Msg )
	end
	if DeadCreature.MType == 2006 then
			Totem_Count3 = 30
			DEScriptZone:SetTimer(Totem_Time3, 1000, 1)
			local Msg = string.format("大地图腾计时开始")
			BroadMsgToAll( Msg )
	end
	if DeadCreature.MType == 2007 then
			Totem_Count4 = 30
			DEScriptZone:SetTimer(Totem_Time4, 1000, 1)
			local Msg = string.format("物理图腾计时开始")
			BroadMsgToAll( Msg )
	end
    if DeadCreature.MType == Mons_ID then
			BroadMsgToAll("恭喜，挑战成功")
			DEScriptZone:KillTimer(Start_Timer)
			SendRemiainToAll(0)
			DEScriptZone:SetTimer(Finish_Timer, Finish_Time, 1)
			OnTraverseCreature = OnGiveRewardItem
			DEScriptZone:TraverseCreature()	
	end
	
end

function OnGiveRewardItem(Creature)

	GiveItemInfo = 
	{
		ItemClass = 40,
		ItemType = 49,
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
			DEScriptZone:SendSystemMessage(Creature, 0, "您获得物品:希尔的礼物 x1个.")
			WriteLog(TempItemLogFileName, string.format("%s 获得 希尔的礼物 x1个", Creature.Name))
		else 
		    DEScriptZone:GiveItem(Creature, GiveItemInfo)
			DEScriptZone:SendSystemMessage(Creature, 0, "您获得物品:希尔的礼物 x1个")
			WriteLog(TempItemLogFileName, string.format("%s 获得 希尔的礼物 x1个", Creature.Name))
		end
end