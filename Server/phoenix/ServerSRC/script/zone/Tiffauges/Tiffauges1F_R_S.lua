-- 浇饭捞绢侩 萍器陵 1F 泅技

dofile("../script/zone/Tiffauges/Tiffauges1F_R.lua")

function GetRace()
	return "Slayer"
end

function OnBeforeEnterZone(Creature)
	
	if not DEScript:IsSlayer(Creature) then
		DEScriptZone:SendSystemMessage(Creature, 0, "只有人类可以进入.")
		
		return false;
	end
	
	PlayerLevel = DEScript:GetLevel(Creature)
	AdvLevel = DEScript:GetAdvancementClassLevel(Creature)
	
	if (PlayerLevel + AdvLevel ) < 130 then
		DEScriptZone:SendSystemMessage(Creature, 0, "等级大于130可以进入.")
		
		return false
	end	

	return true;	
end

function OnAfterEnterZone(Creature)

end

function OnBeforeLeaveZone(Creature)

end

function OnAfterLeaveZone(Creature)

end