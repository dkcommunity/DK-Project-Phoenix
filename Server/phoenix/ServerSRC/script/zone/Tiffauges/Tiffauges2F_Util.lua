function OpenGate(BlockedPos)

	if BlockedPos.EffectGate ~= nil then
		DEScriptZone:RemoveEffect(BlockedPos.EffectGate)
	end
	
	for index, Pos in ipairs(BlockedPos) do
		DEScriptZone:ClearBlocked(Pos[1],Pos[2])
	end
end

function CloseGate(BlockedPos, EffectGateID)
	for index, Pos in ipairs(BlockedPos) do
		DEScriptZone:SetBlocked(Pos[1],Pos[2])
	end
	
	EffectCreateInfo =
	{
		-- CastCreature = CastCreature,
		EffectCastCreature = nil,
		
		EffectClass = EffectGateID,
		
		PosX = BlockedPos.GatePos[1],
		PosY = BlockedPos.GatePos[2],
		
		NextTime = 0,
		Deadline = 99999999
	}
	
	BlockedPos.EffectGate = DEScriptZone:AddEffectToTile(EffectCreateInfo) 	
end

function CheckMissionAllKill(DeadCreatureObjectID, MissionMonsterList)
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

function SummonMonsters(AttackerCreature, DefenderCreature, SummonMonsterInfo, SummonMonsterCount, SummonRange)

	SummonMonsterInfo = SummonMonsterInfo
	
	if math.random(0, 100) < 60 then
		-- 60% 확률로 공격자 주변
		SummonBaseX = AttackerCreature.X 
		SummonBaseY = AttackerCreature.Y
	else
		-- 40% 확률로 방어자  주변
		SummonBaseX = DefenderCreature.X 
		SummonBaseY = DefenderCreature.Y
	
	end
	
	NumOfRetry = 3
	
	for i = 1, SummonMonsterCount do
		NumOfRetry = 3
		 
		repeat
			SummonMonsterInfo.X = SummonBaseX + ( math.random(0, SummonRange*2) - SummonRange)
			SummonMonsterInfo.Y = SummonBaseY + ( math.random(0, SummonRange*2) - SummonRange)
		
			SummonedMonsterInfo = DEScriptZone:AddMonster(SummonMonsterInfo)
			
			NumOfRetry = NumOfRetry - 1
			
		until SummonedMonsterInfo.ObjectID ~= 0 or NumOfRetry < 0 
	end
end

function CalcRemainHPRate(Creature)
	return (Creature.CurHP/Creature.MaxHP) * 100
end

function GetRemainTime()
	-- unit - seconds
	return (DoungeonLimitTime - (DEScript:GetCurrentTime() - DoungeonStartTime))
end

function InstallTrap(TrapType, TrapBaseX, TrapBaseY, TrapNextTime, TrapDeadLine)
	EffectCreateInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 584,
		
		PosX = TrapBaseX,
		PosY = TrapBaseY,
		
		NextTime = TrapNextTime,
		Deadline = TrapDeadLine
	}
	
	if TrapType == 1 then
		EffectCreateInfo.EffectClass = 584
	elseif TrapType == 2 then
		EffectCreateInfo.EffectClass = 585
	else
		EffectCreateInfo.EffectClass = 584
	end
			
	DEScriptZone:AddEffectToTile(EffectCreateInfo)

end

function SendRemiainTime(Creature, remainTime)
	DEScriptZone:SendSystemMessage(Creature, 7, "Tiffauges Castle 2F["..remainTime.."]~~~~~~")
end

function StartToIncreaseTrapStage(Interval)
	--DEScriptZone:SetTimer((100 + CurrentMissionRoom * 10), 1000 * 10, 1)	-- 트랩 단계 증가
	DEScriptZone:SetTimer(100, 1000 * Interval, 1)	-- 트랩 단계 증가
end

function ProcessTrap(TimerID)

	TrapActivationInfo = TrapActivationInfoList[CurrentMissionRoom]
	if TrapActivationInfo == nil then
		return
	end
	
	if TrapActivationInfo.ActivationTime == nil then
		return
	end
	
	--TrapBaseTimerID = (100 + CurrentMissionRoom * 10)
	TrapBaseTimerID = 100

	if TimerID == TrapBaseTimerID then
		ActivationTimeCount = table.maxn(TrapActivationInfo.ActivationTime)
		
		TrapActivationInfo.Stage = TrapActivationInfo.Stage + 1
		if TrapActivationInfo.Stage > ActivationTimeCount then
			TrapActivationInfo.Stage = ActivationTimeCount
		end
			
		if TrapActivationInfo.Stage >= 1 then
					
			TrapTime = math.random(
				TrapActivationInfo.ActivationTime[TrapActivationInfo.Stage][1], 
				TrapActivationInfo.ActivationTime[TrapActivationInfo.Stage][2]
			)
			DEScriptZone:SetTimer((TrapBaseTimerID + 1), 1000 * TrapTime, 1)
		end
		
		TrapActivationInfoList[CurrentMissionRoom] = TrapActivationInfo
	end
	
	if TimerID == (TrapBaseTimerID + 1) then
		if TrapActivationInfo.Stage > 0 then

			TrapGroup = math.random(1, table.maxn(TrapActivationInfo.TrapPosList))
			for index, TrapPos in ipairs(TrapActivationInfo.TrapPosList[TrapGroup]) do
				InstallTrap(DoungeonTrapType, TrapPos[1], TrapPos[2], math.random(0, 30), 30) 
			end
			
			TrapTime = math.random(
				TrapActivationInfo.ActivationTime[TrapActivationInfo.Stage][1], 
				TrapActivationInfo.ActivationTime[TrapActivationInfo.Stage][2]
			)
			DEScriptZone:SetTimer((TrapBaseTimerID + 1), 1000 * TrapTime, 1)
		else
			DEScriptZone:KillTimer((TrapBaseTimerID + 1))
		end
	end
end

function OnWarpToGetOut(Creature)

	DEScriptZone:TransportCreature(Creature, 
		51, 
		155 + math.random(-7, 7), 
		54 + math.random(-14, 14)
	)
end

-- 화면 흔들림 전송 Effect
EffectShakeCreateInfo =
{
	EffectClass = 0,
	Duration = 15 * 1000
}	

-- 화면 어둡게 하기
EffectDarkCreateInfo =
{
	EffectClass = 1,
	Duration = 8 * 1000
}
	
function NotifyWarpToWorld(CreatureInfo)
	DEScriptZone:AddEffectToScreen(EffectShakeCreateInfo, CreatureInfo)
	DEScriptZone:AddEffectToScreen(EffectDarkCreateInfo, CreatureInfo)
end