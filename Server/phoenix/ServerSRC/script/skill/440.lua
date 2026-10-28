
function OnInitialize()
	
end


function OnComputeOuput(InputInfo, OutputInfo)
	
	OutputInfo.Damage = math.random(30, 37);
	OutputInfo.Duration = 10;
	OutputInfo.Delay = 10;
	
end

function OnExecuteSkill(SkillInputInfo)
	
	SkillInputInfo.bMagicHitRoll = false;
	SkillInputInfo.bMagicDamage  = false;
	SkillInputInfo.bAdd          = true;
	
	DEScriptSkill:AddMask(SkillInputInfo, 0, 0, 100)
end