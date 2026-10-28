-- SKILL_TIFFAUGES_MORGOTH_C_RED
dofile("../script/skill/TiffaugesSkillINT_C.lua")

function OnInitialize()
end

function OnComputeOuput(InputInfo, OutputInfo)
	ComputeOuput(InputInfo, OutputInfo)
end

function OnExecuteSkill(SkillInputInfo)
	SkillInputInfo.bMagicHitRoll = false;
	SkillInputInfo.bMagicDamage  = false;
	SkillInputInfo.bAdd          = true;
	
	for y = -3, 3 do
		for x = -3, 3 do
			DEScriptSkill:AddMask(SkillInputInfo, x, y, 100)
		end
	end
end