-- SKILL_TIFFAUGES_GUSION_C_BLUE
dofile("../script/skill/TiffaugesSkillSTR_C.lua")

function OnInitialize()
end

function OnComputeOuput(InputInfo, OutputInfo)
	ComputeOuput(InputInfo, OutputInfo)
end

function OnExecuteSkill(SkillInputInfo)
	SkillInputInfo.bMagicHitRoll = false;
	SkillInputInfo.bMagicDamage  = false;
	SkillInputInfo.bAdd          = true;
	
	for y = -4, 4 do
		for x = -4, 4 do
			DEScriptSkill:AddMask(SkillInputInfo, x, y, 100)
		end
	end
end