function IsAbleToUseItem(Creature, ItemClass, ItemType)
	Result = true
	
	if ItemClass == 39 then			-- 鬼族印记
		Result = false
	elseif ItemClass == 51 then		-- 鬼族结婚戒指
		Result = false
	elseif ItemClass == 52 then		-- 人类结婚戒指
		Result = false
	elseif ItemClass == 93 then		-- 魔灵结婚戒指
		Result = false
	elseif ItemClass == 55 then		-- 复活卷
		Result = false
	end

	if ( Result == false ) then
		DEScriptZone:SendSystemMessage(Creature, 0, "该物品禁止在本地图使用.")
	end
			
	return Result
end

function IsAbleToUseSkill(Creature, SkillType)
	Result = true
	
	if SkillType == 185 then		-- 灵魂锁链
		Result = false
	elseif SkillType == 115 then	-- 血之漩涡
		Result = false
	elseif SkillType == 114 then	-- 血之印记
		Result = false
	elseif SkillType == 289 then	-- 复活
		Result = false
	elseif SkillType == 119 then	-- 复活
		Result = false
	elseif SkillType == 278 then	-- 死亡之吻
		Result = false
	elseif SkillType == 327 then	-- 太阳风
		Result = false
	elseif SkillType == 423 then	-- 冰冻之环1
		Result = false
	elseif SkillType == 424 then	-- 冰冻之环2
		Result = false
	elseif SkillType == 427 then	-- 血牢1
		Result = false
	elseif SkillType == 428 then	-- 血牢2
		Result = false
	elseif SkillType == 433 then	-- 灵魂迷雾1
		Result = false
	elseif SkillType == 434 then	-- 灵魂迷雾2
		Result = false
	elseif SkillType == 156 then	-- 血液互补
		Result = false
	elseif SkillType == 432 then	-- 血翼
		Result = false
	elseif SkillType == 576 then	-- 剧烈掠夺
		Result = false
	elseif SkillType == 634 then	-- 灵魂庇佑
		Result = false
	elseif SkillType == 98 then	-- 隐蔽
		Result = false
	end
	
	if ( Result == false ) then
		DEScriptZone:SendSystemMessage(Creature, 0, "该技能禁止在本地图使用.")
	end
			
	return Result
	
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

