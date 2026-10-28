function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "ExpEvent"	
	ZoneCreateInfo.TemplateZoneID = 1005
	ZoneCreateInfo.StartPosX = 29
	ZoneCreateInfo.StartPosY = 29
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true	
	
end

function OnAfterEnterZone( CreatureInfo )
    
    local NewPlayer = {
		       Info = CreatureInfo,
		       ID = CreatureInfo.ObjectID,
		       Name = CreatureInfo.Name
                      }
					  
	CurPlayer = NewPlayer;
end

function OnWarpZone(Creature)
	local WarpID = 0;
	local WarpX = 0;
	local WarpY = 0;
	
	if DEScript:IsSlayer(Creature) then
		WarpID = 64
		WarpX = 27
		WarpY = 220
	elseif DEScript:IsVampire(Creature) then
		WarpID = 61
		WarpX = 101
		WarpY = 220
	elseif DEScript:IsOusters(Creature) then
		WarpID = 62
		WarpX = 35
		WarpY = 32	
	end
    
	DEScriptZone:TransportCreature(Creature, WarpID, WarpX, WarpY)

end

function OnSay( CreatureInfo, Mesg )

	--GM命令
	if DEScript:IsGod(CreatureInfo) then
		if Mesg == "*jiangli" then
		   DEScriptZone:GiveExpAdv(CurPlayer, 500000);
		end
		
		if Mesg == "*stop" then
		  OnWarpZone(CurPlayer);;
		end
	end 

end

function OnKillCreature( AttackerCreatureInfo, DeadCreatureInfo )

	if DEScript:IsMonster( DeadCreatureInfo ) == true and DeadCreatureInfo.MType == 1449 then
	   DEScriptZone:GiveExpAdv(CurPlayer, 500000);
	end

end
