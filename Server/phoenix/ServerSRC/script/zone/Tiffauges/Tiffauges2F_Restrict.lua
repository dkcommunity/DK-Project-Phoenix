function IsAbleToUseItem(Creature, ItemClass, ItemType)
	Result = true

	if ItemClass == 39 then			-- 뱀파이어 씰 사용  금지
		Result = false
	elseif ItemClass == 51 then		-- 슬레이어 커플링 사용  금지
		Result = false
	elseif ItemClass == 52 then		-- 뱀파이어 커플링 사용  금지
		Result = false
	elseif ItemClass == 93 then		-- 아우스터즈 하모닉 팬던트 사용  금지
		Result = false
	end
	
	if ItemClass == 7 then			-- 바이크 금지
		Result = false
	elseif ItemClass == 98 then		-- 뱀파 윙아이템 금지
		Result = false
	elseif ItemClass == 99 then		-- 아우 윙아이템 금지
		Result = false
	end
	
	if ( Result == false ) then
		DEScriptZone:SendSystemMessage(Creature, 0, "This item cannot be used in current dungeon.")
	end
			
	return Result
end

function IsAbleToUseSkill(Creature, SkillType)
	Result = true

	if SkillType == 185 then		-- 소울체인  금지
		Result = false
	elseif SkillType == 115 then	-- 블러디 터널  금지(뱀파 씰)
		Result = false
	elseif SkillType == 114 then	-- 블러디 마크 금지(뱀파 씰)
		Result = false
	end
	
	if SkillType == 280 then	-- 텔레포트 금지
		Result = false
	elseif SkillType == 102 then	-- 박쥐 변신 금지
		Result = false
	elseif SkillType == 203 then	-- 래피드 글라이딩  금지
		Result = false		
	end	

	if ( Result == false ) then
		DEScriptZone:SendSystemMessage(Creature, 0, "This skill cannot be used in current dungeon.")
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

