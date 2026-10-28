dofile("../script/zone/BathoryHidden/BathoryHidden_Main.lua")

function OnInitializeVariables(ZoneCreateInfo)

	ZoneCreateInfo.Name = "BathoryHidden_2"
	ZoneCreateInfo.TemplateZoneID = 1106

	ZoneCreateInfo.StartPosX = 22
	ZoneCreateInfo.StartPosY = 24

	-- indivisual(개인), party(파티), lobby(로비, 로비에 들어간 후 참여)
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = false
	
	--  아무도 없는 상태에서 최대 지속 시간 (seconds)
	ZoneCreateInfo.EmptyTimeOut = 60
	
end
