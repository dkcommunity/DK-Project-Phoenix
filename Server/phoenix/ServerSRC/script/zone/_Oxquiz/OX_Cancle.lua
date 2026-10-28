function OnInitializeVariables(ZoneCreateInfo)

	ZoneCreateInfo.Name = "OX_Event"	
	ZoneCreateInfo.TemplateZoneID = 1005
	
	-- 종족별로 다른경우에는 ??
	ZoneCreateInfo.StartPosX = 29
	ZoneCreateInfo.StartPosY = 29

	-- indivisual(개인), party(파티), lobby(로비, 로비에 들어간 후 참여)
	ZoneCreateInfo.EntranceType = "indivisual"
	--ZoneCreateInfo.EntranceType = "party"
	--ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.StaticZone = true
	
	--  아무도 없는 상태에서 최대 지속 시간 
	ZoneCreateInfo.EmptyTimeOut = 10 -- 15 sec
end 

function OnInitialize()
	print("OnInitialize()")
end

function OnTerminate()
	
end

function OnBeforeEnterZone(Creature)
	
	return false
	
end

function OnAfterEnterZone(Creature)
end

function OnBeforeLeaveZone(Creature)
	
end

function OnAfterLeaveZone(Creature)
	
end

function OnAttackCreature(AttackerCreature, DefenderCreature)
	-- 다른 사람 공격하면 퇴장?
end

function OnKillCreature(AttackerCreature, DeadCreature)
end

function OnAddMonster(Monster)

end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	
	return false;	-- 설정된 부활 위치로 이동
	
end


function IsAbleToUseItem(Creature, ItemClass, ItemType)
			
	return true
end

function IsAbleToUseSkill(Creature, SkillType)

	return true;
	
end

function OnUseItemInGear(Creature, ItemClass, ItemType)
	return IsAbleToUseItem(Creature, ItemClass, ItemType)
end

function OnUseItemInQuick(Creature, ItemClass, ItemType)
	return IsAbleToUseItem(Creature, ItemClass, ItemType)
end

function OnUseItemInInventory(Creature, ItemClass, ItemType)
	return IsAbleToUseItem(Creature, ItemClass, ItemType)
end

function OnUseInventorySkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseObjectSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseNamedSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseSelfSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseTileSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnSay(CreatureInfo, Mesg)

	return true
end

