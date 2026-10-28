function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "event"
	ZoneCreateInfo.TemplateZoneID = 1006
	ZoneCreateInfo.StartPosX = 15
	ZoneCreateInfo.StartPosY = 30
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true

end

function OnInitialize()
	DEScriptZone:SetPKZone(false);
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
		WarpX = 102
		WarpY = 220
	elseif DEScript:IsOusters(Creature) then
		WarpID = 62
		WarpX = 35
		WarpY = 32
	end

	DEScriptZone:TransportCreature(Creature, WarpID, WarpX, WarpY)
end

function OnSetStartPos(CreatureInfo, PositionInfo)

    if DEScript:IsSlayer(CreatureInfo) then
		PositionInfo.X = 15
        		PositionInfo.Y = 30
	elseif DEScript:IsVampire(CreatureInfo) then
		PositionInfo.X = 31
        		PositionInfo.Y = 14
	elseif DEScript:IsOusters(CreatureInfo) then
		PositionInfo.X = 47
        		PositionInfo.Y = 33
	end

end
function GiveItem(PlayerInfo, Class, Type, count)

	if PlayerInfo == nil then
		return;
	end

	local GiveItemInfo =
	{
		-- È
		ItemClass = Class,
		ItemType = Type,
		ItemCount = count,
		Options = "",
		LimitedTime = 0
	}
	ItemInfo = {}
	ItemInfo = GiveItemInfo

	if count <= 200 then

		if DEScriptZone:GiveItem(PlayerInfo, ItemInfo) then
			return true
		else
			return false
		end

	else

		ItemInfo.ItemCount = 200
		local GiveTime = count / 200
		GiveTime = math.floor(GiveTime)
		local gewei = count % 200


		local strMsg = string.format("GiveTime = %d", GiveTime)
		DEScriptZone:BroadcastSystemMessageInZone(4, strMsg)

		for i = 1, GiveTime, 1 do
			DEScriptZone:GiveItem(PlayerInfo, ItemInfo)
		end

		if gewei > 0 then
			ItemInfo.ItemCount = gewei
			DEScriptZone:GiveItem(PlayerInfo, ItemInfo)
		end

	return true

	end
end

function OnAfterEnterZone(Creature)

end

function OnBeforeLeaveZone(Creature)

end

function OnAfterLeaveZone(Creature)

end