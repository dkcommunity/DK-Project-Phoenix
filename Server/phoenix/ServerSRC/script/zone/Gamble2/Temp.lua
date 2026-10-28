dofile("../script/zone/Gamble/Temp2.lua")


L_XiaZhu_1List = { }
L_XiaZhu_2List = { }
L_ZuangID = nil


function OnTraverseCreature( CreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseCreature")
end
function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "Temp"	
	ZoneCreateInfo.TemplateZoneID = 1006
	ZoneCreateInfo.StartPosX = 29
	ZoneCreateInfo.StartPosY = 29
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true	
end 
function OnTraverseMonster( MonsterInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseMonster")
end

function OnInitialize()
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnInitialize")
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnAttackCreature")
end


function OnKillCreature( AttackerCreatureInfo, DeadCreatureInfo )
         if oneinfo ~= nil and oneinfo.ObjectID == DeadCreatureInfo.ObjectID then
		oneinfo = nil
         DEScriptZone:BroadcastSystemMessageInZone(8, "黄僵胜利")
	 end
         if twoinfo ~= nil and twoinfo.ObjectID == DeadCreatureInfo.ObjectID then
		twoinfo = nil
         DEScriptZone:BroadcastSystemMessageInZone(8, "绿僵胜利")
	 end
end


function OnTimer( TimerID )

end


function OnBeforeEnterZone( CreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeEnterZone")
	return true
end

function OnAfterEnterZone( CreatureInfo )
    DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterEnterZone")
end

function OnBeforeLeaveZone( CreatureInfo )
    DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeLeaveZone")
end

function OnAfterLeaveZone( CreatureInfo )
    DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterLeaveZone")
end

function OnAddMonster(Monster)
	
	EffectInfo =
	{
		EffectCastCreature = nil,
		
		EffectClass = 66,
		
		NextTime = 0,
		Deadline = 99999999
	}

	Monster._pCreature = Monster._pMonster
	DEScriptZone:AddEffectToCreature(EffectInfo , Monster)	
	return true
end

function OnResurrectCreature( CreatureInfo, ZoneCoordInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnResurrectCreature")
	return false
end

oneinfo = nill
twoinfo = nill
function OnSay( CreatureInfo, Mesg )
	
	if Mesg == "1" then
        AddMonsterInfo = one
	AddMonsterInfo.X = 31
	AddMonsterInfo.Y = 31
	oneinfo = DEScriptZone:AddMonster(AddMonsterInfo)
	--DEScriptZone:BroadcastSystemMessageInZone(4, "刷绿僵")
        AddMonsterInfo = two
	AddMonsterInfo.X = 28
	AddMonsterInfo.Y = 28
	twoinfo = DEScriptZone:AddMonster(AddMonsterInfo)
	
	--DEScriptZone:BroadcastSystemMessageInZone(4, "刷黄僵")
        end
        
        
	if IsContain(Mesg, "#下注1号") > 0 then
	    DoXiaZhu_1( CreatureInfo, Mesg )
	elseif IsContain(Mesg, "#下注2号") > 0 then
	    DoXiaZhu_2( CreatureInfo, Mesg )
	elseif IsContain(Mesg, "#坐庄") > 0 then
		DoZuoZhuang( CreatureInfo,Mesg )
	end
     
     
	return true
end

function OnUseItemInGear( CreatureInfo, ItemClass, ItemType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInGear")
	return true
end

function OnUseItemInInventory( CreatureInfo, ItemClass, ItemType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInInventory")
	return true
end

function OnUseItemInQuick( CreatureInfo, ItemClass, ItemType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInQuick")
	return true
end

function OnUseInventorySkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseInventorySkill")
	return true
end

function OnUseObjectSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseObjectSkill")
	return true
end

function OnUseNamedSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseNamedSkill")
	return true
end

function OnUseSelfSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseSelfSkill")
	return true
end

function OnUseTileSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseTileSkill")
	return true
end

function OnBeforeUseMonsterSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeUseMonsterSkill")
	return true
end

function OnAfterUseMonsterSkill( CreatureInfo, SkillType )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterUseMonsterSkill")
end

function OnSetStartPos(CreatureInfo, PositionInfo)
 		PositionInfo.X = 21
                PositionInfo.Y = 42
end


function GetMesgNumber(Mesg, Sub)

    local Pos = string.find(Mesg, Sub,0)
    Pos = Pos + string.len(Sub) + 1

    local strRet = string.sub(Mesg, Pos)

    local Number = tonumber(strRet)

    return Number

end


--下注成功的消息广播
function XiaZhuOkMessage(Name, Count, Mesg)

	local strRet = string.format("赌场:玩家:%s 成功下注[%s] %d!!", Name,Mesg, Count)
	DEScriptZone:BroadcastSystemMessageInZone(4, strRet)
	
end


function IsContain(Mesg, subStr)

    local find = string.find(Mesg, subStr)

	if ( find ~= nil ) then
		return find
	end

	return 0

end


function TabkeItem(PlayerInfo, count)

	local HaveItemInfo = 
	{
		--书
		ItemClass = 91,
		ItemType = 63,
		ItemCount = count
	}
	
	if DEScriptZone:HaveItem(PlayerInfo, HaveItemInfo) then
		TakeItemInfo = {}
		TakeItemInfo = HaveItemInfo
		
		DEScriptZone:TakeItem(PlayerInfo, TakeItemInfo)
		
		return true
	else
	    return false
	end
	
end


function WelcomeMessage(PlayerInfo, Mesg)

end

function DoZuoZhuang(PlayerInfo, Mesg)
	
	L_ZuangID = PlayerInfo.ObjectID
	    local NewPlayer = {
                   ID = PlayerInfo.ObjectID,
                   XiaZhu = 0,
                   }
                   
	L_GamblePlayerList[PlayerID] = NewPlayer;
	
end

function DoXiaZhu_2(PlayerInfo, Mesg)

	if L_XiaZhu_2List[ PlayerInfo.ObjectID ] ~= nil then  --已经下注过的玩家不允许再次下注
	    return
	end
	
	--分析下注数量
	local Number = GetMesgNumber(Mesg, "#下注2号")
	
	if TabkeItem(PlayerInfo, Number) == false then --取得人物身上的书，没有书就返回
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "亲爱的玩家！您身上的书不够！",  16777215)
		return
	end
	
	XiaZhuOkMessage(PlayerInfo.Name, Number, "2号")

	local NewPlayer = {
                ID = PlayerInfo.ObjectID,
                XiaZhu = Number,
                }
                   
	L_XiaZhu_2List[PlayerID] = NewPlayer;
	
end



function DoXiaZhu_1(PlayerInfo, Mesg)

	local ID = PlayerInfo.ObjectID
	if L_XiaZhu_1List[ ID ] ~= nil then  --已经下注过的玩家不允许再次下注
	    return
	end
	
	--分析下注数量
	local Number = GetMesgNumber(Mesg, "#下注1号")
	
	if TabkeItem(PlayerInfo, Number) == false then --取得人物身上的书，没有书就返回
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "亲爱的玩家！您身上的书不够！",  16777215)
		return
	end
	
	XiaZhuOkMessage(PlayerInfo.Name, Number, "1号")

	local NewPlayer = {
                ID = PlayerInfo.ObjectID,
                XiaZhu = Number,
                }
                   
	L_XiaZhu_1List[PlayerID] = NewPlayer;
	
end
