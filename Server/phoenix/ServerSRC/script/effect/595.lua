
function OnInitialize()
--	print("Effect-595, OnInitialize()......")
	
	DEScriptEffect:SetNextTime(15)
	DEScriptEffect:SetDeadLine(0)
end

function OnAffect()
--	print("Effect-595, OnAffect()......X="..DEScriptEffect:GetBaseX().." Y="..DEScriptEffect:GetBaseY())
	
	DEScriptEffect:TraverseEffectRegion(
		DEScriptEffect:GetBaseX(), 
		DEScriptEffect:GetBaseY(), 
		1, 
		1
	)
	
	--DEScriptEffect:SetNextTime(99999999)
	DEScriptEffect:SetNextTime(15)
	
	DEScriptEffect:SetDeadLine(0)

end

function OnUnaffect()
--	print("Effect-595, OnUnaffect()......")

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
