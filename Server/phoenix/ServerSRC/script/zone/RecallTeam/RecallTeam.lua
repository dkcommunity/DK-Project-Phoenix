function OnInitializeVariables(ZoneCreateInfo)--生成副本地图
	ZoneCreateInfo.Name = "BloodyMonastery1F"	
	ZoneCreateInfo.TemplateZoneID = 6700
	ZoneCreateInfo.StartPosX = 28
	ZoneCreateInfo.StartPosY = 113
	ZoneCreateInfo.EntranceType = "indivisual"-- indivisual(单人), party(组队)
	ZoneCreateInfo.StaticZone = true
	ZoneCreateInfo.EmptyTimeOut = 10 -- unit : seconds
end 

function OnInitialize()--初始化副本
	DEScriptZone:SetPKZone(false)
end

function OnSetStartPos(CreatureInfo, PositionInfo)--玩家进入场景坐标
		PositionInfo.X = 28
        PositionInfo.Y = 113
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	return false;
end

function OnBeforeEnterZone(Creature)--进入副本之前
	DEScriptZone:recallparty(Creature,28,113)
	return false

	--end
end

function OnAfterEnterZone(Creature)--进入副本之后

end

function OnBeforeLeaveZone(Creature)--离开副本之前

end

function OnAfterLeaveZone(Creature)--离开副本之后

end
