local LevelUpTime = 60 * 60  --60秒 = 1分钟 * 60 = 1小时
local CurPlayer = nil  --当前练级的玩家

local L_MonsterID = 1449          --怪物ID
local L_MonsterCount = 20		 --怪物数量
local L_ItemClass = 46			 --物品类别
local L_ItemType = 5			 --物品类型
local TempLogFileName = "../script/zone/VipLevelUpV/VipLevelUp1.log"

local Monster = {
	MType = L_MonsterID,
	Name = "2号",
	X = 30,
	Y = 30,
	Dir = 2,

	AI =
		"",
	
	DeadAI = "",
	Enhance = ""
}


function RandomPosX()

end

function RandomPosY()

end


function OnWarpZone(Creature)
	-- 捞拌 -> 泅技 捞悼矫糯
	
	local WarpID = 0;
	local WarpX = 0;
	local WarpY = 0;
	
	if DEScript:IsSlayer(Creature) then
		WarpID = 64
		WarpX = 27
		WarpY = 220
	elseif DEScript:IsVampire(Creature) then
		WarpID = 61
		WarpX = 101
		WarpY = 220
	elseif DEScript:IsOusters(Creature) then
		WarpID = 62
		WarpX = 35
		WarpY = 32	
	end
    
	DEScriptZone:TransportCreature(Creature, WarpID, WarpX, WarpY)

end


--拿走玩家身上的指定物品
function TabkeItem(PlayerInfo, count)

	local HaveItemInfo = 
	{
		--书
		ItemClass = L_ItemClass,
		ItemType = L_ItemType,
		ItemCount = 1
	}
	
	if DEScriptZone:HaveItem(PlayerInfo, HaveItemInfo) then
		TakeItemInfo = {}
		TakeItemInfo = HaveItemInfo
		
		DEScriptZone:TakeItem(PlayerInfo, TakeItemInfo)
		
		return true
	else
	    return false
	end
	
end

function SendRemainTime(Creature, remainTime, timerDesc)
	if timerDesc == nil then
		timerDesc = ""
	end
			
	DEScriptZone:SendSystemMessage(Creature, 7, timerDesc.." ["..remainTime.."]")
end


function CreateMonster(Type, Count)
	AddMonsterInfo = Monster
	AddMonsterInfo.X = 28
	AddMonsterInfo.Y = 28
	AddMonsterInfo.Name = "2号"
	
    for i = 1, Count, 1
	do
	    DEScriptZone:AddMonster(AddMonsterInfo)	
	end
	
		
end


--function OnTraverseCreature( CreatureInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseCreature")
--end

function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "VipLevelUpO_5"	
	ZoneCreateInfo.TemplateZoneID = 6201
	ZoneCreateInfo.StartPosX = 23
	ZoneCreateInfo.StartPosY = 34
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true	
	
	DEScriptZone:SetTimer(1, 4000 , 1)
end 

function OnTraverseMonster( MonsterInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseMonster")
end



function OnInitialize()
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnInitialize")
	CreateMonster(0, L_MonsterCount); --初始化20个怪
	
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnAttackCreature")
end


--杀死一个怪 又重生一个怪
function OnKillCreature( AttackerCreatureInfo, DeadCreatureInfo )

	if DEScript:IsMonster( DeadCreatureInfo ) == true and DeadCreatureInfo.MType == 1449 then
		CreateMonster(0, 1);
	end

end


function TransPortBack( Player )
	
	if CurPlayer ~= nil then
		OnWarpZone(Player.Info);
	end
    
end


function FinishLevelUp()

	--传送玩家出副本
	--重新启动计时器
	
	TransPortBack( CurPlayer );
	CurPlayer = nil
end


function OnTimer( TimerID )

	if TimerID == 1 then
		OnTraverseCreature = OnWarpZone
		DEScriptZone:TraverseCreature()
		DEScriptZone:KillTimer( 1 );
	end
	
end


function OnBeforeEnterZone( CreatureInfo )

	if CurPlayer ~= nil then
		DEScriptZone:SendSystemMessage(CreatureInfo, 0, "已经有人在里面了,请选择其它房间!" )
		return false;
	end
	
	if TabkeItem(CreatureInfo, 1) == false then
		DEScriptZone:SendSystemMessage(CreatureInfo, 0, "您没有相关的道具,进不了副本!" )
		return false;		
	end
	
	return true
end

function OnAfterEnterZone( CreatureInfo )
    DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterEnterZone")
    
    local NewPlayer = {
						Info = CreatureInfo,
						ID = CreatureInfo.ObjectID,
					    Name = CreatureInfo.Name
					  }
					  
	CurPlayer = NewPlayer;
	DEScriptZone:SetTimer(1,LevelUpTime * 1000 , 1);
	SendRemainTime(CurPlayer.Info, LevelUpTime * 10, "练级本剩余时间:")
    local LogBuf = string.format("%s      进入副本", CurPlayer.Name)
	DEScript:WriteFileLog(TempLogFileName, LogBuf)
end


function OnBeforeLeaveZone( CreatureInfo )

end

function OnAfterLeaveZone( CreatureInfo )
	local LogBufA = string.format("%s      异常离开副本", CreatureInfo.Name)
	DEScript:WriteFileLog(TempLogFileName, LogBufA)
	CurPlayer = nil
end

function OnAddMonster(Monster)
	
	return true
end

function OnResurrectCreature( CreatureInfo, ZoneCoordInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnResurrectCreature")
		--ZoneCoordInfo.ZoneID = DEScriptZone:GetZoneID()
		--ZoneCoordInfo.X = 23
        --ZoneCoordInfo.Y = 34
	return false
end


function OnSay( CreatureInfo, Mesg )
     
    if Mesg == "create" then
		CreateMonster(0, 3);
    end
     
	return true
end

function OnUseItemInGear( CreatureInfo, ItemClass, ItemType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInGear")
	return true
end

function OnUseItemInInventory( CreatureInfo, ItemClass, ItemType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInInventory")
	return true
end

function OnUseItemInQuick( CreatureInfo, ItemClass, ItemType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInQuick")
	return true
end

function OnUseInventorySkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseInventorySkill")
	return true
end

function OnUseObjectSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseObjectSkill")
	return true
end

function OnUseNamedSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseNamedSkill")
	return true
end

function OnUseSelfSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseSelfSkill")
	return true
end

function OnUseTileSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseTileSkill")
	return true
end

function OnBeforeUseMonsterSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeUseMonsterSkill")
	return true
end

function OnAfterUseMonsterSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterUseMonsterSkill")
end

function OnSetStartPos(CreatureInfo, PositionInfo)
 		PositionInfo.X = 23
        PositionInfo.Y = 34
end