
function OnInitialize()
	DEScriptEffect:SetNextTime(0)
	DEScriptEffect:SetDeadLine(99999999)
end

function OnAffect()
	DEScriptEffect:TraverseEffectRegion(
		DEScriptEffect:GetBaseX(), 
		DEScriptEffect:GetBaseY(), 
		3, 
		3
	)
	
	DEScriptEffect:SetNextTime(10)

end

function OnUnaffect()
end

function OnTraverseTile(ZoneInfo, TileX, TileY)
	OffsetX = DEScriptEffect:GetBaseX() - TileX
	OffsetY = DEScriptEffect:GetBaseY() - TileY
	
	if ( OffsetX == 0 and OffsetY == 0 ) then

		DEScriptEffect:BrocastGCAddEffectToTile(ZoneInfo, 
			TileX, 
			TileY, 
			15
		)
	end
		
	return true
end

function OnAffectCreature(TargetCreature)
	if not DEScript:IsPC(TargetCreature) then
		return
	end
	
	SkillType = 0 -- SKILL_ATTACK_MELEE
	Damage = math.random(300, 350)
	
	DEScriptEffect:SetDamage(TargetCreature, SkillType, Damage, nil)
end

function OnUnaffectCreature(TargetCreature)

end