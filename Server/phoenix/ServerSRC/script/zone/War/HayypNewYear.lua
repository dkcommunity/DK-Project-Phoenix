local L_PLAYER_ID_LIST = {}
local MAX_ITEM_Normal = 13
local MAX_ITEM_Better = 28
local MAX_ITEM_Best = 17
local MAX_ITEM_Hard = 3
local HardNum = 0
local ItemList_Normal = 
                { --class/type/cout 75%
                 {"活性之香",70, 22, 1},--1
				 {"铁壁之香",70, 23, 1},--2
				 {"伤害显示",70, 88, 1},--3
				 {"分流麦克风",70, 87, 1},--4
                 {"莉莉丝宝石",41, 56, 1},--5
				 {"黄宝石2",41, 48, 1},--6
				 {"奥尼布拉的礼物",40, 46, 1},--7
				 {"酷雪人",47, 54, 1},--8
                 {"美丽的圣诞树",47, 12, 1},--9
				 {"白色蒸糕",48, 15, 1},--10
				 {"蒸糕",48, 16, 1},--11
				 {"烧毁的古籍",91, 63, 1},--12
				 {"复活卷轴",55, 0, 1},--13
				}
local ItemList_Better = 
                 { --class/type/cout 25%
				 {"英雄证照",91, 32, 1},--1
				 {"奥尼布拉的脊骨",91, 31, 1},--2
				 {"彩蛋12-201幻影箱",91, 47, 1},--3
				 {"莉莉丝宝石2",41, 62, 1},--4
				 {"里尔里斯心灵2",41, 61, 1},--5
				 {"里尔里斯希望2",41, 60, 1},--6
				 {"三倍经验石头",70, 47, 1},--7
				 {"全能水滴",76, 16, 1},--8
				 {"B6心脏",91, 17, 1},--9
				 {"震撼胶囊",76, 27, 1},--10
				 {"冰冻胶囊",76, 28, 1},--11
				 {"减肥胶囊",76, 29, 1},--12
				 {"蓝鸟2",41, 82, 1},--13
				 {"万能卷轴",55, 1, 1},	--14			 
				 {"增加生命7",97, 71, 1},--15
				 {"增加生命8",97, 72, 1},--16
				 {"增加生命9",97, 73, 1},--17
				 {"增加生命10",97, 104, 1},--18
				 {"增加生命11",97, 105, 1},--19
				 {"增加生命12",97, 106, 1},--20
				 {"增加生命13",97, 107, 1},--21
				 {"增加生命14",97, 108, 1},--22
				 {"增加生命15",97, 109, 1},--23
				 {"高空闪避",97, 74, 1},--24
				 {"高空猎鹰",97, 75, 1},--25
				 {"高空袭击",97, 76, 1},--26
				 {"高空神力",97, 77, 1},--27
				 {"高空火力",97, 78, 1},--28
				}
local ItemList_Best = 
                { --class/type/cout 5%
				 {"10x6包",90, 2, 1},--1
				 {"德·莱斯 黑宝石",79, 0, 1},--2
				 {"德·莱斯 红宝石",79, 1, 1},--3
				 {"德·莱斯 蓝宝石",79, 2, 1},--4
				 {"德·莱斯 绿宝石",79, 3, 1},--5
				 {"召唤卷轴 [西乌斯]",81, 10, 1},--6
				 {"召唤卷轴 [桑提斯]",81, 11, 1},--7
				 {"召唤卷轴 [德古拉]",81, 12, 1},--8
				 {"召唤卷轴 [卡米拉]",81, 13, 1},--9
				 {"黄色宝石3",41, 57, 1},--10
				 {"平凡的莉莉丝心灵",41, 71, 1},--11
				 {"平凡的莉莉丝希望",41, 70, 1},--12
				 {"宠物能药水",76, 49, 1},--13
				 {"7倍经验石",70, 86, 1},--14
				 {"生命进阶",97, 101, 1},--15
				 {"物防进阶",97, 102, 1},--16
				 {"魔防进阶",97, 103, 1},--17
				}
local ItemList_Hard = 
                { --class/type/cout  1 or 100
				 {"深渊契约",96, 8, 1},--1
				 {"混沌契约",96, 9, 1},--2
				 {"遗忘契约",96, 10, 1},--3
				}
function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "HayypeNewYear"
	ZoneCreateInfo.TemplateZoneID = 1622
	ZoneCreateInfo.StartPosX = 26
	ZoneCreateInfo.StartPosY = 28
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true

end

function OnInitialize()
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnInitialize")
	DEScriptZone:SetPKZone(true);
	DEScriptZone:SetTimer(1, 1000 * 5 , 1);--1 minuete
	ClearHardNum()
end

function OnWarpZone(Creature)
	local WarpID = 0;
	local WarpX = 0;
	local WarpY = 0;

	if DEScript:IsSlayer(Creature) then
		WarpID = 64
		WarpX = 27
		WarpY = 220
	elseif DEScript:IsVampire(Creature) then
		WarpID = 61
		WarpX = 102
		WarpY = 220
	elseif DEScript:IsOusters(Creature) then
		WarpID = 62
		WarpX = 35
		WarpY = 32
	end

	DEScriptZone:TransportCreature(Creature, WarpID, WarpX, WarpY)

end

function GiveItem(PlayerInfo, Class, Type, count)

	if PlayerInfo == nil then
		return;
	end

	local GiveItemInfo =
	{
		--书
		ItemClass = Class,
		ItemType = Type,
		ItemCount = count,
		Options = "",
		LimitedTime = 0
	}
	ItemInfo = {}
	ItemInfo = GiveItemInfo

	if count <= 200 then

		if DEScriptZone:GiveItem(PlayerInfo, ItemInfo) then
			return true
		else
			return false
		end

	else

		ItemInfo.ItemCount = 200
		local GiveTime = count / 200
		GiveTime = math.floor(GiveTime)
		local gewei = count % 200


		local strMsg = string.format("GiveTime = %d", GiveTime)
		DEScriptZone:BroadcastSystemMessageInZone(4, strMsg)

		for i = 1, GiveTime, 1 do
			DEScriptZone:GiveItem(PlayerInfo, ItemInfo)
		end

		if gewei > 0 then
			ItemInfo.ItemCount = gewei
			DEScriptZone:GiveItem(PlayerInfo, ItemInfo)
		end

	return true

	end
end

function OnTimer( TimerID )
	if TimerID == 1 then --1 fenzhong
	
		for i,Player in pairs(L_PLAYER_ID_LIST) do
				AdvLevel = DEScript:GetAdvancementClassLevel(Player.Info)
				if AdvLevel > 199 then
				DEScriptZone:SendSystemMessage(Player.Info, 0, "350级不能获取经验.")
				else
				DEScriptZone:GiveExpAdv(Player.Info,2000000)
				end
		end	
		
	end

end


function OnBeforeEnterZone( CreatureInfo )
     AdvLevel = DEScript:GetAdvancementClassLevel(CreatureInfo)
	 if AdvLevel >0 then
	 return true
	 else
	 DEScriptZone:SendSystemMessage(CreatureInfo, 0, "只有转职职业才可以进入.")
	 return false
	 end
end


function OnAfterEnterZone( CreatureInfo )
	local player = {Info = CreatureInfo}
	L_PLAYER_ID_LIST[ CreatureInfo.ObjectID ] = player
end

function OnAfterLeaveZone( CreatureInfo )
	local ID = CreatureInfo.ObjectID;
	L_PLAYER_ID_LIST[ ID ] = nil; 
end

function OnResurrectCreature( CreatureInfo, ZoneCoordInfo )


	DEScriptZone:BroadcastSystemMessageInZone(4, "OnResurrectCreature")
	ZoneCoordInfo.ZoneID = DEScriptZone:GetZoneID()

	if DEScript:IsSlayer(CreatureInfo) then
		ZoneCoordInfo.X = 42
        ZoneCoordInfo.Y = 74
	elseif DEScript:IsVampire(CreatureInfo) then
		ZoneCoordInfo.X = 63
        ZoneCoordInfo.Y = 109
	elseif DEScript:IsOusters(CreatureInfo) then
		ZoneCoordInfo.X = 85
        ZoneCoordInfo.Y = 75
	end

	return true;
end

function OnUseItemInInventory( CreatureInfo, ItemClass, ItemType )
    if ItemClass == 48 and ItemType == 13 then
	    local rate = math.random(10,999)
		local rateMesg = math.modf(rate/10)

		if rate < 750 then  -- 进入普通物品列表 75% 几率
		local randI_Index = math.random(1, MAX_ITEM_Normal)
		local Item = ItemList_Normal[randI_Index]
		GiveItem(CreatureInfo, Item[2], Item[3], Item[4])
		
		local MSEEAGE = string.format("[新年祝福活动]:您扔出了：%s点,获得参与奖:%s!再接再厉哦，大奖等着你",rateMesg,Item[1])
		DEScriptZone:SendSystemMessage(CreatureInfo, 0,MSEEAGE )

		HardNum = HardNum+1
		
		elseif rate >=750 and rate <950 then  --进入中级物品列表 25%几率
		local randI_Index = math.random(1, MAX_ITEM_Better)
		local Item = ItemList_Better[randI_Index]
		GiveItem(CreatureInfo, Item[2], Item[3], Item[4])
		
		local MSEEAGE = string.format("[新年祝福活动]:您扔出了：%s点,获得二等奖:%s!再接再厉哦，大奖等着你",rateMesg,Item[1])
		DEScriptZone:SendSystemMessage(CreatureInfo, 0,MSEEAGE )
		HardNum = HardNum+1
		
	    elseif rate >=950 and rate <=999 then --进入高级物品列表 5%几率
		local randI_Index = math.random(1, MAX_ITEM_Best)
		local Item = ItemList_Best[randI_Index]
		GiveItem(CreatureInfo, Item[2], Item[3], Item[4])
		
		local Msg = string.format("恭喜[%s]扔出:%s点,获得一等奖:%s",CreatureInfo.Name,rateMesg,Item[1])
		DEScriptZone:BroadcastSystemMessageInZone(8, Msg)
		HardNum = HardNum+1
		end
		
		if HardNum == 3000 then
		local randI_Index = math.random(1, MAX_ITEM_Hard)
		local Item = ItemList_Hard[randI_Index]
		GiveHard(CreatureInfo, Item[2], Item[3], Item[4])
		
		local Msg = string.format("恭喜[%s]扔出了:100点满点获得特等奖:%s！",CreatureInfo.Name,Item[1])
		DEScriptZone:BroadcastSystemMessageInZone(8, Msg)
        ClearHardNum()		
		end
	end
	return true
end
function ClearHardNum()
         HardNum = 0
end
function GiveHard(PlayerInfo,Class,Type,Count)
	if PlayerInfo == nil then
		return;
	end
	local GiveItemInfo =
	{
		--书
		ItemClass = Class,
		ItemType = Type,
		ItemCount = count,
		Options = "ATTR+10,ATTR+10,PDAM+10,MDAM+10,VIS+10",
		LimitedTime = 0
	}
	ItemInfo = {}
	ItemInfo = GiveItemInfo
    DEScriptZone:GiveItem(PlayerInfo, ItemInfo)
	return true
end
function OnSay( CreatureInfo, Mesg )
    if Mesg == "1" then
	  local randI_Index = math.random(1, MAX_ITEM_Hard)
	  local Item = ItemList_Hard[randI_Index]
	  GiveHard(CreatureInfo,Item[2],Item[3],Item[4] )
	elseif Mesg == "test" then
	        local num0 = 0
	        local num1 = 0
	        local num2 = 0
	        local num3 = 0
	    for i=1,150 do  
	            local rate=math.random(1,1000)
		    if rate < 750 then
		       num0=num0+1
		    elseif rate >=750 and rate < 950 then
		       num1=num1+1
		    elseif rate >=950 and rate <=999 then
		       num2=num2+1
		    elseif rate == 1000 then
		       num3=num3+1
	     	end
		end	 
	  local Msg = string.format("统计：三等奖：%s,二等奖:%s,一等奖：%s,特等奖：%s",num0,num1,num2,num3)
	  DEScriptZone:BroadcastGCSay(CreatureInfo, Msg,  16777215)
	elseif  Mesg == "2" then
	  local Msg = string.format("全局基础：%s",HardNum)
	  DEScriptZone:BroadcastGCSay(CreatureInfo, Msg,  16777215)
	elseif   Mesg == "3" then
	  local Msg = string.format("我其实只想知道这个活动的消息框到底有多长，所以我发这个")	
	  DEScriptZone:BroadcastSystemMessageInZone(8, Msg)
	end
	  return true
end
function OnSetStartPos(CreatureInfo, PositionInfo)

    if DEScript:IsSlayer(CreatureInfo) then
		PositionInfo.X = 42
        PositionInfo.Y = 74
	elseif DEScript:IsVampire(CreatureInfo) then
		PositionInfo.X = 63
        PositionInfo.Y = 109
	elseif DEScript:IsOusters(CreatureInfo) then
		PositionInfo.X = 85
        PositionInfo.Y = 75
	end

end