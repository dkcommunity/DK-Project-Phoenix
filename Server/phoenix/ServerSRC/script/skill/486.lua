-- SKILL_TIFFAUGES_PAIMON_RED
dofile("../script/skill/TiffaugesSkillINT_B.lua")

function OnInitialize()
end

function OnComputeOuput(InputInfo, OutputInfo)
	ComputeOuput(InputInfo, OutputInfo)
end

function OnExecuteSkill(SkillInputInfo)
	SkillInputInfo.bMagicHitRoll = false;
	SkillInputInfo.bMagicDamage  = false;
	SkillInputInfo.bAdd          = true;
	
	for y = -2, 2 do
		for x = -2, 2 do
			DEScriptSkill:AddMask(SkillInputInfo, x, y, 100)
		end
	end
end