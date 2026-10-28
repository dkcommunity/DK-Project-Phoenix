
function OnInitialize()
--	print("Effect-586, OnInitialize()......")
	
	DEScriptEffect:SetNextTime(0)
	DEScriptEffect:SetDeadLine(99999999)
end

function OnAffect()
end

function OnUnaffect()
--	print("Effect-586, OnUnaffect()......")

end

function OnTraverseTile(ZoneInfo, TileX, TileY)
	
	return false
end

function OnAffectCreature(TargetCreature)
--	print("Effect-586, OnAffectCreature()......"..TargetCreature.Name)
			
	DEScriptEffect:SetNextTime(99999999)
	DEScriptEffect:SetDeadLine(99999999)
end

function OnUnaffectCreature(TargetCreature)

end