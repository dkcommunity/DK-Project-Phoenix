
function ComputeOuput(InputInfo, OutputInfo)
	OutputInfo.Damage = 10 + (InputInfo.STR/8 + InputInfo.INTE/4)
	OutputInfo.Duration = 10
	OutputInfo.Delay = 20	
end

function ExecuteSkill(SkillInputInfo)
	SkillInputInfo.bMagicHitRoll = false;
	SkillInputInfo.bMagicDamage  = false;
	SkillInputInfo.bAdd          = true;
	
	DEScriptSkill:AddMask(SkillInputInfo, 0, 0, 100)
end