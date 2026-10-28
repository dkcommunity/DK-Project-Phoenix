dofile("../script/zone/HellCastle/HellCastle_MonsterInfo.lua")
dofile("../script/zone/HellCastle/HellCastle_BlockInfo.lua")

local PlayerCount = 0

Mission_Monster = 
{
	{
		MonsterInfo = Monster_1613,
		Name = "门卫1号",
		X = 34,
		Y = 45,
		KillCallBack = OnGuard_0_Kill
	}

}
 

function SummonAll()
	 for i ,Mobinfo in ipairs(Mission_Monster) do
		AddMonsterInfo = Mobinfo.MonsterInfo
		AddMonsterInfo.X = Mobinfo.X
		AddMonsterInfo.Y = Mobinfo.Y
		--AddMonsterInfo.Name = Mobinfo.Name
		AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
	 end
end    


function OnTraverseCreature( CreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseCreature")
end

function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "HellCastle"	
	ZoneCreateInfo.TemplateZoneID = 6431
	ZoneCreateInfo.StartPosX = 36
	ZoneCreateInfo.StartPosY = 37
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.EmptyTimeOut = 1 -- 15 sec
	
end 


function OnInitialize()
	SummonAll()
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnInitialize")
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnAttackCreature")
end


function OnKillCreature( AttackerCreatureInfo, DeadCreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnKillCreature")
end


function OnTimer( TimerID )

end


function OnBeforeEnterZone( CreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeEnterZone")
	return true
end

function OnAfterEnterZone( CreatureInfo ) --进入场景
	PlayerCount = PlayerCount + 1
end

function OnBeforeLeaveZone( CreatureInfo )
    DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeLeaveZone")
end

function OnAfterLeaveZone( CreatureInfo )--离开场景
	PlayerCount = PlayerCount - 1

    DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterLeaveZone")
end

function OnAddMonster( MonsterInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnAddMonster")
end

function OnResurrectCreature( CreatureInfo, ZoneCoordInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnResurrectCreature")
	return false
end


function OnSay( CreatureInfo, Mesg )
     NpcInfo ={
	 type = 1157,
	 x =   40,
	 y =   51
	 }
	if Mesg == '1' then
	DEScriptZone:addNPC(NpcInfo)
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

function OnSetStartPos( CreatureInfo, PositionInfo )
	PositionInfo.X = 36   --门口
    PositionInfo.Y = 37
    
   -- PositionInfo.X = 150  --双子
  --  PositionInfo.Y = 96
end


