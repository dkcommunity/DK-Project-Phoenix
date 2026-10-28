
local L_PLAYER_ID_LIST = {}

local Boss_ID = 1920

local Boss_TimerID = 1113 --BOSS重生计时器
local Minute_TimerID = 1114

local Minute_Index = 5

local BossTimeStart = 0
local Boss = nil
local totalSec = 0
local CanUseSkill = true



function OnInitializeVariables(ZoneCreateInfo)--生成副本地图
	ZoneCreateInfo.Name = "BloodyMonasteryF"	
	ZoneCreateInfo.TemplateZoneID = 7001
	ZoneCreateInfo.StartPosX = 28
	ZoneCreateInfo.StartPosY = 113
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = true
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 

--计算BOSS活动时间
function BeginBossGameTimer()
	Boss = nil
	Minute_Index = 5
	BossTimeStart = DEScript:WarOpenTime()
	
	local hour = 0
	local min = 0
	local sec = 0
	local totalSec = 0
	
	if BossTimeStart.hour < 22 then
                  totalSec = 22*60*60 - (BossTimeStart.hour*60*60+BossTimeStart.min*60+BossTimeStart.sec)
	else
                  totalSec = 24*60*60 - (BossTimeStart.hour*60*60+BossTimeStart.min*60+BossTimeStart.sec) + 22*60*60	
	end
	
         local totalMin = totalSec/60
               hour = totalMin/60
               min = totalMin%60
			   
	local hourW = math.floor(hour)
	local minW  = math.floor(min)
	DEScriptZone:SetTimer(Boss_TimerID, totalSec * 1000, 1);--1 second
	local Msg = string.format("BOSS将在%s时,%s分后刷.",hourW, minW)
	BroadMsgToAll(Msg)
end

function OnInitialize()--初始化副本
	DEScriptZone:SetPKZone(true)
	BeginBossGameTimer();
	
end

function OnSetStartPos(CreatureInfo, PositionInfo)--玩家进入场景坐标
		PositionInfo.X = 28
        PositionInfo.Y = 113
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	return false;
end

function OnBeforeEnterZone(Creature)--进入副本之前
	return true

	--end
end
function SendTimeMsg()

	local BossTimeStartTWO =0 
	BossTimeStartTWO = DEScript:WarOpenTime()
	local totalSecT = 0
	local totalSecTWO = 0
	
	if BossTimeStartTWO.hour < 22 then
				  totalSecT = 22*60*60 - (BossTimeStartTWO.hour*60*60+BossTimeStartTWO.min*60+BossTimeStartTWO.sec)
	else
				  totalSecT = 24*60*60 - (BossTimeStartTWO.hour*60*60+BossTimeStartTWO.min*60+BossTimeStartTWO.sec) + 22*60*60	
	end
	
	local totalMinT = totalSecT/60
	local hourT = totalMinT/60
	local minT = totalMinT%60
	local hourTT = math.floor(hourT)
	local minTT  = math.floor(minT)
	local Msg = string.format("BOSS将在%s小时%s分钟后刷新.",hourTT, minTT)
	BroadMsgToAll(Msg)
	
end

function OnAfterEnterZone(Creature)--进入副本之后
    local player = {Info = Creature}
	L_PLAYER_ID_LIST[ Creature.ObjectID ] = player
	
	if Boss == nil then
	
		SendTimeMsg()
	end
	
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
	
	Boss = DEScriptZone:AddMonster(monsInfo)
	
end

function OnSay(CreatureInfo, Mesg)--玩家聊天信息
	if Mesg == 'timenow' then
	   SendTimeMsg()
	end
	return true
end

function OnTimer(TimerID)
    if TimerID == Boss_TimerID then
        DEScriptZone:SetTimer(Minute_TimerID, 5 * 1000, 1);--1 second
		DEScriptZone:KillTimer(TimerID)
	end
	
	if TimerID == Minute_TimerID then 
		if Minute_Index == 0 then
			SummonMonster(Boss_ID, "BOSS", 30, 32)
			DEScriptZone:BroadcastSystemMessageInZone(8, "BOSS：哇哈哈哈，这么多人来送死啊！")
			DEScriptZone:KillTimer(TimerID)
		end
		local Msg = string.format("BOSS将在%d分钟后刷，请勇士们做好准备!", Minute_Index)
		BroadMsgToAll(Msg)
		Minute_Index = Minute_Index - 1;
	end
end

function BroadMsgToAll(Msg)
	for i,Player in pairs(L_PLAYER_ID_LIST) do
		DEScriptZone:SendSystemMessage(Player.Info,0,Msg)
	end		
end


function OnPK(AttackerCreature, DeadCreature)--怪物死亡判断

	if DeadCreature.MType == Boss_ID then
		OnGiveRewardItem(AttackerCreature)
		BeginBossGameTimer(); --重新开始计时
		local Msg = string.format("BOSS被[%s]杀死了,恭喜[%s]获得了丰厚的奖励！", AttackerCreature.Name, AttackerCreature.Name)
		DEScriptZone:BroadcastSystemMessageInZone(8, Msg)
		
	end
	
end

function OnGiveRewardItem(Creature)

	GiveItemInfo = 
	{
		ItemClass = 41,
		ItemType = 82,
		ItemCount = 1,
		Options = "",
		LimitedTime = 0
	}
	
	DEScriptZone:GiveItem(Creature, GiveItemInfo)
	
end