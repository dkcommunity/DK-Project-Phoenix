function OnInitializeVariables(ZoneCreateInfo)

	ZoneCreateInfo.Name = "OX_Event"	
	ZoneCreateInfo.TemplateZoneID = 1005
	
	-- 종족별로 다른경우에는 ??
	ZoneCreateInfo.StartPosX = 29
	ZoneCreateInfo.StartPosY = 29

	-- indivisual(개인), party(파티), lobby(로비, 로비에 들어간 후 참여)
	ZoneCreateInfo.EntranceType = "indivisual"
	--ZoneCreateInfo.EntranceType = "party"
--	ZoneCreateInfo.StaticZone = false
	ZoneCreateInfo.StaticZone = true
	
	--  아무도 없는 상태에서 최대 지속 시간 
	ZoneCreateInfo.EmptyTimeOut = 10 -- 15 sec
end 

function OnInitialize()
	print("OnInitialize()")
	
	dofile("../script/zone/Oxquiz/NoPassPlayer.lua")
	dofile("../script/zone/Oxquiz/AllowPlayer.lua")
end

function OnTerminate()
	
end

EnterZone = false
OnPlayer = 0

function OnBeforeEnterZone(Creature)
	if DEScript:IsGod(Creature) == true then
		print("운영자 : "..Creature.Name.." 님이 OX 존에 입장 하셨습니다.")
		return true
	-- 인원을 300명으로 제한한다.
	elseif OnPlayer >= 300 then
		print("OnPlayer : "..OnPlayer)
		DEScriptZone:SendSystemMessage(Creature, 0, "You have completed the quest. Please try again next time.")
		return false
	elseif EnterZone == true then

		for i, WinPlayer in ipairs(WinPlayerList) do
			if Creature.Name == WinPlayer then
				DEScriptZone:SendSystemMessage(Creature, 0, "The champion is not allowed to admit again in this event.")
				return false
			end
		end
		
		if table.getn(AllowPlayerList) > 0 then
		
			for i, AllowPlayer in ipairs(AllowPlayerList) do
				if Creature.Name == AllowPlayer then
					print("OnBeforeEnterZone() --- Creature.Name="..Creature.Name.." 님이 OX 존에 입장 하셨습니다.")
					return true
				end
			end
			
			DEScriptZone:SendSystemMessage(Creature, 0, "You cannot participate in the event at the moment.")
			
			return false
		end
	
		print("OnBeforeEnterZone() --- Creature.Name="..Creature.Name.." You have entered the OX area.")
		return true
		
	elseif  EnterZone == false then
		DEScriptZone:SendSystemMessage(Creature, 0, "You are unable to enter.")
		return false
	end
	
	return false
end

function OnAfterEnterZone(Creature)
	if DEScript:IsGod(Creature) == false then
		OnPlayer = OnPlayer + 1
		print("OnPlayer1 : "..OnPlayer)
	end
end

function OnBeforeLeaveZone(Creature)
end

function OnAfterLeaveZone(Creature)
	if DEScript:IsGod(Creature) == false then
		OnPlayer = OnPlayer - 1
		print("OX 이벤트 존을 벗어난 플레이어 : "..Creature.Name.. "  현재 존의 인원 : "..OnPlayer )
	end
end

function OnAttackCreature(AttackerCreature, DefenderCreature)
end

function OnKillCreature(AttackerCreature, DeadCreature)
end

function OnAddMonster(Monster)
end

function OnResurrectCreature(CreatureInfo, ZoneCoordInfo)
	return false;	-- 설정된 부활 위치로 이동
end


function IsAbleToUseItem(Creature, ItemClass, ItemType)
	print("IsAbleToUseItem() Creature.Name="..Creature.Name.." ItemClass="..ItemClass.." ItemType="..ItemType)

	Result = true
	
	if ItemClass == 7 then			-- 바이크 금지
		Result = false
	elseif ItemClass == 98 then		-- 뱀파 윙아이템 금지
		Result = false
	elseif ItemClass == 98 then		-- 아우 윙아이템 금지
		Result = false
	end
	
--	DEScriptZone:SendSystemMessage(Creature, 4, "Items are restricted.")
			
	return false
end

function IsAbleToUseSkill(Creature, SkillType)
	print("IsAbleToUseSkill() Creature.Name="..Creature.Name.." SkillType="..SkillType)

	Result = true

	if SkillType == 280 then		-- 텔레포트 금지
		print("Creature.Name="..Creature.Name)

		OnTraverseCreature = OnCheckCorrect
		DEScriptZone:TraverseCreature()
	elseif SkillType == 102 then	-- 박쥐 변신 금지
		Result = false
	end
		
--	DEScriptZone:SendSystemMessage(Creature, 4, "Skills are restricted.")
			
	return false;
	
end

function OnUseItemInGear(Creature, ItemClass, ItemType)
	return IsAbleToUseItem(Creature, ItemClass, ItemType)
end

function OnUseItemInQuick(Creature, ItemClass, ItemType)
	return IsAbleToUseItem(Creature, ItemClass, ItemType)
end

function OnUseItemInInventory(Creature, ItemClass, ItemType)
	return IsAbleToUseItem(Creature, ItemClass, ItemType)
end

function OnUseInventorySkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseObjectSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseNamedSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseSelfSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnUseTileSkill(Creature, SkillType)
	return IsAbleToUseSkill(Creature, SkillType)
end

function OnCheckToCorrect(Creature)
--	print("OnCheckToCorrect(), Creature.Name="..Creature.Name)
--	DEScriptZone:BroadcastSystemMessageInZone(8, "Be ready to answer~~~")
--	DEScriptZone:SendSystemMessage(Creature, 0, "Correct~~~")
end

--20080703 kim sung yong
--1005번 OX존의 블럭 위치를 지정한다. 
BlockPos =
{
	{25,37},
	{26,36},
	{27,35},
	{28,34},
	{29,33},
	{30,32},
	{31,31},
	{32,30},
	{33,29},
	{34,28},
	{35,27},
	{36,26},
	{37,25},

	{25,36},
	{26,35},
	{27,34},
	{28,33},
	{29,32},
	{30,31},
	{31,30},
	{32,29},
	{33,28},
	{34,27},
	{35,26},
	{36,25}
}

--20080703 kim sung yong
--1005번 OX존을 통제 할 사람을 등록한다.
CanSayPeople =
{
	{"총퀘스트06"},
	{"퀘스트4"},
	{"비승직2"},
	{"전아레어"},
	{"물약테스"},
	{"총슬퀘"}
}

KeepingCreature = {}
UserCountTempTable = {}

MessageTimer = 0

function OpenLine()
	DEScriptZone:BroadcastSystemMessageInZone(4, "Restriction of movement has been lifted.")

	for j, Pos in ipairs(BlockPos) do
   		DEScriptZone:ClearBlocked(Pos[1],Pos[2])
	end
end

function TransportCreatureToVileage(Creature)
	if DEScript:IsSlayer(Creature) == true then
		DEScriptZone:TransportCreature(Creature, 12, 104, 125)
	end
	if DEScript:IsVampire(Creature) == true then
		DEScriptZone:TransportCreature(Creature, 1003, 50, 53)
	end
	if DEScript:IsOusters(Creature) == true then
		DEScriptZone:TransportCreature(Creature, 1311, 36, 63)
	end

end

function CheckOnLine(Creature)
	
		for j, Pos in ipairs(BlockPos) do
			if (Creature.X == Pos[1] and Creature.Y == Pos[2]) then
				if DEScript:IsGod(Creature) == false then
					TransportCreatureToVileage(Creature)
				end
			end
		end
		
end

function CloseLine()
	
	DEScriptZone:BroadcastSystemMessageInZone(4, "Movement is restricted.")

	OnTraverseCreature = CheckOnLine 
	DEScriptZone:TraverseCreature()

	for j, Pos in ipairs(BlockPos) do
   		DEScriptZone:SetBlocked(Pos[1],Pos[2])
	end

end

UserCount = 0

--20080703 kim sung yong
--명령어나 대화 관련을 제어한다. 현재는 존의 통제를 담당한다.
function OnSay(CreatureInfo, Mesg)
	print("Create.P sition.x="..CreatureInfo.X.." Create.Position.y="..CreatureInfo.Y)
	print("Create.Name="..CreatureInfo.Name.." Mesg="..Mesg)

	if DEScript:IsGod(CreatureInfo) == true then
		-- *문자가 첫번째에 위치하고 있다면, 운영자 명령어다. 보여주지말자.
		GMSay = string.find(Mesg, "*")
		QuizSay = string.find(Mesg, "*quiz")
		if GMSay == 1 then
			if Mesg == "*close" then
				CloseLine()
			elseif Mesg == "*open" then
				OpenLine()
			elseif Mesg == "*ook" then
				WriteUserList()
				
				MessageTimer = 0
				CreaturePos = 0
				DEScriptZone:SetTimer(1, 1000 * 1)
			elseif Mesg == "*xok" then
				WriteUserList()
			
            	MessageTimer = 0
				CreaturePos = 0
				DEScriptZone:SetTimer(2, 1000 * 1)
			elseif Mesg == "*usercount" then
				OnTraverseCreature = OnUserCount
				DEScriptZone:TraverseCreature()
				OnPlayer = UserCount
				DEScriptZone:SendSystemMessage(CreatureInfo, 4, "There are currently "..OnPlayer.." participating in this event.")
				UserCount = 0
			elseif Mesg == "*zoneopen" then
				dofile("../script/zone/Oxquiz/NoPassPlayer.lua")
				dofile("../script/zone/Oxquiz/AllowPlayer.lua")
				
				EnterZone = true
				
				if table.getn(AllowPlayerList) > 0 then
					DEScriptZone:SendSystemMessage(CreatureInfo, 4, "Players who were restricted to enter are now allowed.")
				else
					DEScriptZone:SendSystemMessage(CreatureInfo, 4, "Everyone is allowed to enter now.")
				end
			elseif Mesg == "*zoneclose" then
				EnterZone = false
				DEScriptZone:SendSystemMessage(CreatureInfo, 4, "Entrance limitation has been set.")
			elseif Mesg == "*writeuserlist" then
				WriteUserList()
				
				DEScriptZone:SendSystemMessage(CreatureInfo, 4, "A list of all the current participants is saved.")
				
			elseif QuizSay == 1 then
				print("문제를 출제 합니다.")
				QuizString = string.sub(Mesg, 7)
        		DEScriptZone:BroadcastSystemMessageInZone(8, QuizString)
			end
		else
			DEScriptZone:BroadcastSystemMessageInZone(1, Mesg)
		end
	end
		
	return true
end

function OnTimer(TimerID)

    if TimerID == 1 then
	    MessageTimer = MessageTimer + 1
		print(MessageTimer)
	    DEScriptZone:BroadcastSystemMessageInZone(4, (6 - MessageTimer).."You will be teleported soon.")
		if MessageTimer == 5 then
			DEScriptZone:KillTimer(1)
			OnTraverseCreature = OnWarpToGetOutX
        	DEScriptZone:TraverseCreature()
			DEScriptZone:SetTimer(3, 50 * 1)
		end
	end

	if TimerID == 2 then
        MessageTimer = MessageTimer + 1
		print(MessageTimer)
        DEScriptZone:BroadcastSystemMessageInZone(4, (6 - MessageTimer).."You will be teleported soon.")
        if MessageTimer == 5 then
            DEScriptZone:KillTimer(2)
       	    OnTraverseCreature = OnWarpToGetOutO
	        DEScriptZone:TraverseCreature()
			DEScriptZone:SetTimer(3, 50 * 1)
		end 	
	end

	if TimerID == 3 then
    	TransportPlayer()
	end


end
CreaturePos = 0;

function OnUserCount(Creature)
	if DEScript:IsGod(Creature) == false then
		if Creature == nil then
			print("인원이 없습니다")
		else
			UserCount = UserCount + 1
		end
	end
end

function OnWarpToGetOutX(Creature)
		CreaturePos = Creature.X + Creature.Y 
		
		if DEScript:IsGod(Creature) == false then 
			if CreaturePos >= 62 then
				tempCreature = {}

	            tempCreature.Name = Creature.Name
                tempCreature._pCreature = Creature._pCreature
	            table.insert(KeepingCreature, tempCreature)			
				
			end
		end
end

function OnWarpToGetOutO(Creature)
	print("Creature.Name="..Creature.Name)
	print(Creature._pCreature)


        CreaturePos = Creature.X + Creature.Y

        if DEScript:IsGod(Creature) == false then
            if CreaturePos <= 62 then
				tempCreature = {}

				tempCreature.Name = Creature.Name
				tempCreature._pCreature = Creature._pCreature
				table.insert(KeepingCreature, tempCreature)
			end
        end
end

function TransportPlayer()
	print("퇴장 명령 실행중...")
	if table.getn(KeepingCreature) > 0 then
		for k, KeepPlayer in pairs(KeepingCreature) do
        	print("X Zone Create.Name="..KeepPlayer.Name)
        	print(k, KeepPlayer)
        	print(table.getn(KeepingCreature))
        	
			TransportCreatureToVileage(KeepPlayer)

			table.remove(KeepingCreature, k)

			k = k + 1
		end
	end

	if table.getn(KeepingCreature) <= 0 then
		DEScriptZone:KillTimer(3)
	end		
end

WriteUserCount = 0

function WriteUserList()
	DEScript:WriteFileLog("OX_Event.log", "------ 현재 참가자 저장 시작")

	WriteUserCount = 0
					
	OnTraverseCreature = OnWriteUserList
	DEScriptZone:TraverseCreature()
	
	DEScript:WriteFileLog("OX_Event.log", "------ 현재 참가자 저장 끝 ( 총 : "..WriteUserCount.." 명 )")
end


function OnWriteUserList(Creature)
	if DEScript:IsGod(Creature) == false then
		if Creature ~= nil then
			DEScript:WriteFileLog("OX_Event.log", Creature.Name)
			
			WriteUserCount = WriteUserCount + 1
		end
	end
end
