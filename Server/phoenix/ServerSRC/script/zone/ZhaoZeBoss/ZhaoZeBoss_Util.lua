function GetRemainTime()--获取副本剩余时间
	return (DoungeonLimitTime - (DEScript:GetCurrentTime() - DoungeonStartTime))
end
function SendRemiainTimeSix(Creature, remainTime)--发送6层副本时间到所有玩家
	DEScriptZone:SendSystemMessage(Creature, 7, "   堕落精灵女王["..remainTime.."]~~~~~~")
end

EffectShakeCreateInfo =--屏幕效果抖动
{
	EffectClass = 0,
	Duration = 15 * 1000
}	
EffectDarkCreateInfo =--屏幕效果黑暗
{
	EffectClass = 1,
	Duration = 8 * 1000
}
	
function NotifyWarpToWorld(CreatureInfo)--添加屏幕效果
	DEScriptZone:AddEffectToScreen(EffectShakeCreateInfo, CreatureInfo)
	DEScriptZone:AddEffectToScreen(EffectDarkCreateInfo, CreatureInfo)
end

function ScreenShake(CreatureInfo)--添加屏幕效果
	DEScriptZone:AddEffectToScreen(EffectShakeCreateInfo, CreatureInfo)
end

function CalcRemainHPRate(Creature)--怪物血量判断
	return (Creature.CurHP/Creature.MaxHP) * 100
end

function CheckMissionAllKill(DeadCreatureObjectID, MissionMonsterList)--任务是否完成
	for index, ObjectID in pairs(MissionMonsterList) do
		if ObjectID == DeadCreatureObjectID then
			table.remove(MissionMonsterList, index)
		end
	end
	
	if table.maxn(MissionMonsterList) == 0 then
		return true
	end
	
	return false
end

function OnWarpToGetOut(Creature) --副本失败传送坐标

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

function WriteLog(logFileName, Content)
	DEScript:WriteFileLog(logFileName, Content)
end