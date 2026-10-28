local	FightStartTime = 0
local	FightRaceTime = 60 --100 * 60
local   L_PLAYER_ID_LIST = {}
local   FightTeam = {}

local eFightState_Over = 1
local eFightState_Start = 2

local 	g_FightState = eFightState_Over

function SendRemainTime(Info, remainTime, timerDesc)
	if timerDesc == nil then
		timerDesc = ""
	end
			
	DEScriptZone:SendSystemMessage(Info, 7, timerDesc.." ["..remainTime.."]")
end


function SendAllRemainTime(Time, timerDesc)
	for key,Player in pairs(L_PLAYER_ID_LIST) do
			SendRemainTime(Player.Info, Time, timerDesc);
    end
end

function OnTraverseCreature( CreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseCreature")
end

function OnTraverseMonster( MonsterInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseMonster")
end

function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "War"	
	ZoneCreateInfo.TemplateZoneID = 1122
	ZoneCreateInfo.StartPosX = 26
	ZoneCreateInfo.StartPosY = 28
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true	
	
	DEScriptZone:SetPKZone(false)
end 


function OnInitialize()
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnInitialize")
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnAttackCreature")
end


function OnKillCreature( AttackerCreatureInfo, DeadCreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnKillCreature")
end


function OnBeforeEnterZone( CreatureInfo )
	HaveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 52,
		ItemCount = 1
	}
	
	local NewPlayer = {Info = CreatureInfo, bIsPK = false}
	local TeamID = DEScript:TeamID( CreatureInfo )

	if DEScriptZone:HaveItem(Creature, HaveItemInfo) then --有物品的玩家 是参与PK的
		TakeItemInfo = {}
		TakeItemInfo = HaveItemInfo
        NewPlayer.bIsPK = true
		DEScriptZone:TakeItem(Creature, TakeItemInfo)
		
		if TeamID ~= 0 then
			FightTeam[ TeamID ] = CreatureInfo                  --记录队长信息
		end
	else  												
	    if FightTeam[ TeamID ] == nil then                   --其他玩家 设置buf
			AddEffect(CreatureInfo, 108)           
			AddEffect(CreatureInfo, 109)
		end
	end
	
	L_PLAYER_ID_LIST[CreatureInfo.ObjectID] = NewPlayer

	return false
end

function OnAfterEnterZone( CreatureInfo )
	if g_FightState == eFightState_Start then
		local nRemainTime = FightRaceTime - ( DEScript:GetCurrentTime() - FightStartTime )
		SendRemainTime(CreatureInfo, nRemainTime * 10, "战场剩余时间:")		
	end
end


function OnBeforeLeaveZone( CreatureInfo )
	--清除
	local LeavePlayer = L_PLAYER_ID_LIST[CreatureInfo.ObjectID]
	if LeavePlayer ~= nil then
        RemoveEffect(LeavePlayer)
		L_PLAYER_ID_LIST[LeavePlayer.ObjectID] = nil
	end
	
end

function OnAfterLeaveZone( CreatureInfo )
    DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterLeaveZone")
end

function OnAddMonster( MonsterInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnAddMonster")
end

function OnResurrectCreature( CreatureInfo, ZoneCoordInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnResurrectCreature")
	return false
end

function OnTimer( TimerID )
	
	if TimerID == 1 then --20准备时间结束，开始PK时间为30分钟
		DEScriptZone:KillTimer( TimerID )
        DEScriptZone:SetPKZone(true)
		DEScriptZone:SetTimer(2, 60 * 1000 , 1) --正式开始比赛计时
		SendAllRemainTime(600, "比赛倒计时")
		DEScriptZone:BroadcastSystemMessageInZone(4, "禽兽们！！给老子上！！！")		
	end
	
	if TimerID == 2 then
		FightOver()
	end
end


function AddEffect(PlayerInfo, EffectID)
	EffectInfo =
	{
		EffectCastCreature = nil,
		EffectClass = EffectID,
		NextTime = 0,
		Deadline = 99999999
	}

	DEScriptZone:AddEffectToCreature(EffectInfo, PlayerInfo)	     
end

function RemoveEffect(Player)
	if Player.bIsPK == false then
		DEScriptZone:RemoveEffectFromCreature(108, PlayerInfo)	
		DEScriptZone:RemoveEffectFromCreature(109, PlayerInfo)	  
	end
end

function FightStart()
	g_FightState = eFightState_Start
	FightStartTime = DEScript:GetCurrentTime()
	DEScriptZone:SetTimer(1, 20 * 1000 , 1) --GM开启比赛 20 秒
	SendAllRemainTime(200, "准备时间")--20时间倒计时
	DEScriptZone:BroadcastSystemMessageInZone(4, "20秒倒计时开始，请各队伍加好自己的BUF")
end

function FightOver()
	g_FightState = eFightState_Over
	DEScriptZone:KillTimer( 1 )
	DEScriptZone:KillTimer( 2 )
	DEScriptZone:SetPKZone(false)
	SendAllRemainTime(0, "比赛倒计时")
end

function ResetFuben()
	--把所有人都清除出副本  数据全部重新初始化
	for i,Player in pairs(L_PLAYER_ID_LIST) do
		RemoveEffect(Player)
		OnWarpZone(Player.Info);
    end
	
	L_PLAYER_ID_LIST = nil
	L_PLAYER_ID_LIST = {}
	
	FightTeam = nil
	FightTeam = {}

	g_FightState = eFightState_Over
end

function OnWarpZone(Creature)
	
	local WarpID = 0;
	local WarpX = 0;
	local WarpY = 0;
	
	if DEScript:IsSlayer(Creature) then
		WarpID = 12
		WarpX = 209
		WarpY = 104
	elseif DEScript:IsVampire(Creature) then
		WarpID = 1003
		WarpX = 62
		WarpY = 64
	elseif DEScript:IsOusters(Creature) then
		WarpID = 1311
		WarpX = 24
		WarpY = 73	
	end
    
	DEScriptZone:TransportCreature(Creature, WarpID, WarpX, WarpY)

end

function OnSay( CreatureInfo, Mesg )

    if DEScript:IsGod( CreatureInfo ) then 
		if Mesg == "开始比赛" then
			FightStart()
		end		
		
		if Mesg == "结束比赛" then
			FightOver()
		end
		
		if Mesg == "重置副本" then
			ResetFuBen()
		end
	end
	

	return true
end

function OnUseItemInGear( CreatureInfo, ItemClass, ItemType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInGear")
	return true
end

function OnUseItemInInventory( CreatureInfo, ItemClass, ItemType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInInventory")
	return true
end

function OnUseItemInQuick( CreatureInfo, ItemClass, ItemType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInQuick")
	return true
end

function OnUseInventorySkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseInventorySkill")
	return true
end

function OnUseObjectSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseObjectSkill")
	return true
end

function OnUseNamedSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseNamedSkill")
	return true
end

function OnUseSelfSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseSelfSkill")
	return true
end

function OnUseTileSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseTileSkill")
	return true
end

function OnBeforeUseMonsterSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeUseMonsterSkill")
	return true
end

function OnAfterUseMonsterSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterUseMonsterSkill")
end

function OnSetStartPos(CreatureInfo, PositionInfo)

    if DEScript:IsSlayer(CreatureInfo) then
		PositionInfo.X = 107
        PositionInfo.Y = 27
	elseif DEScript:IsVampire(CreatureInfo) then
		PositionInfo.X = 18
        PositionInfo.Y = 27
	elseif DEScript:IsOusters(CreatureInfo) then
		PositionInfo.X = 12
        PositionInfo.Y = 103
	end
	
end
