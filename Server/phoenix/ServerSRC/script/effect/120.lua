
function OnInitialize()
--	print("Effect-580, OnInitialize()......")
	
	DEScriptEffect:SetNextTime(0)
	DEScriptEffect:SetDeadLine(99999999)
end

function OnAffect()
--	print("Effect-580, OnAffect()......X="..DEScriptEffect:GetBaseX().." Y="..DEScriptEffect:GetBaseY())
	
	DEScriptEffect:TraverseEffectRegion(
		DEScriptEffect:GetBaseX(), 
		DEScriptEffect:GetBaseY(), 
		1, 
		1
	)
	
	--DEScriptEffect:SetNextTime(99999999)
	DEScriptEffect:SetNextTime(15)
	
	DEScriptEffect:SetDeadLine(99999999)

end

function OnUnaffect()
--	print("Effect-580, OnUnaffect()......")

end

function OnTraverseTile(ZoneInfo, TileX, TileY)
	
	DEScriptEffect:BrocastGCAddEffectToTile(ZoneInfo, 
		TileX, 
		TileY, 
		15
	)
		
	return false
end

function OnAffectCreature(TargetCreature, TileX, TileY)
	
end

function OnUnaffectCreature(TargetCreature)

end