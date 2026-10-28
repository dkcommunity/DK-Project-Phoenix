function IsAbleToUseItem(Creature, ItemClass, ItemType)
	Result = true
	
	if ItemClass == 39 then			-- ¹ìÆÄÀÌ¾î ¾Á »ç¿ë  ±İÁö
		Result = false
	elseif ItemClass == 51 then		-- ½½·¹ÀÌ¾î Ä¿ÇÃ¸µ »ç¿ë  ±İÁö
		Result = false
	elseif ItemClass == 52 then		-- ¹ìÆÄÀÌ¾î Ä¿ÇÃ¸µ »ç¿ë  ±İÁö
		Result = false
	elseif ItemClass == 93 then		-- ¾Æ¿ì½ºÅÍÁî ÇÏ¸ğ´Ğ ÆÒ´øÆ® »ç¿ë  ±İÁö
		Result = false
	end

	if ( Result == false ) then
		DEScriptZone:SendSystemMessage(Creature, 0, "¸ÃÎïÆ·½ûÖ¹ÔÚ±¾µØÍ¼Ê¹ÓÃ.")
	end
			
	return Result
end

function IsAbleToUseSkill(Creature, SkillType)
	Result = true
	
	if SkillType == 185 then		-- ¼Ò¿ïÃ¼ÀÎ  ±İÁö
		Result = false
	elseif SkillType == 115 then	-- ºí·¯µğ ÅÍ³Î  ±İÁö(¹ìÆÄ ¾Á)
		Result = false
	elseif SkillType == 114 then	-- ºí·¯µğ ¸¶Å© ±İÁö(¹ìÆÄ ¾Á)
		Result = false
	end
	
	if ( Result == false ) then
		DEScriptZone:SendSystemMessage(Creature, 0, "¸Ã¼¼ÄÜ½ûÖ¹ÔÚ±¾µØÍ¼Ê¹ÓÃ.")
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

