
function OnInitialize()
	
end


function OnComputeOuput(InputInfo, OutputInfo)
	
	OutputInfo.Damage = math.random(60, 68);
	OutputInfo.Duration = 10;
	OutputInfo.Delay = 10;
	
end

function OnExecuteSkill(SkillInputInfo)
	
	SkillInputInfo.bMagicHitRoll = false;
	SkillInputInfo.bMagicDamage  = false;
	SkillInputInfo.bAdd          = true;
	
	for x = -2, 2
	do
		for y = -2, 2
		do
			DEScriptSkill:AddMask(SkillInputInfo, x, y, 100)
		end
	end
end