dofile("../script/zone/TepezHidden/TepezHidden_Main.lua")

function OnInitializeVariables(ZoneCreateInfo)

	ZoneCreateInfo.Name = "TepezHidden_2"
	ZoneCreateInfo.TemplateZoneID = 1115

	ZoneCreateInfo.StartPosX = 23
	ZoneCreateInfo.StartPosY = 51

	-- indivisual(개인), party(파티), lobby(로비, 로비에 들어간 후 참여)
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = false
	
	--  아무도 없는 상태에서 최대 지속 시간 (seconds)
	ZoneCreateInfo.EmptyTimeOut = 60
	
end
