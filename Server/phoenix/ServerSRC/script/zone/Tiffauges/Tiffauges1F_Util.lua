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

function RegenMonsters(MType)

	if MonsterCount[MType] == nil then
		MonsterCount[MType] = 0
	end

	for index, MonsterInfo in pairs(MonsterList) do
		AddMonsterInfo = MonsterInfo.SummonMonsterInfo
		
		if AddMonsterInfo.MType == MType then
			DiffMonsterCount = MonsterInfo.MaxMonsterCount - MonsterCount[MType] 
				
			if DiffMonsterCount > 0  then
				PositionInfo = {}
			
				for i = 1, DiffMonsterCount do
					if DEScriptZone:FindPosition(MType, PositionInfo) == true then
			
						AddMonsterInfo.X = PositionInfo.X
						AddMonsterInfo.Y = PositionInfo.Y 
				
						AddedMonster = DEScriptZone:AddMonster(AddMonsterInfo)
					end		
				end
			end
			
			return
		end
	end
end