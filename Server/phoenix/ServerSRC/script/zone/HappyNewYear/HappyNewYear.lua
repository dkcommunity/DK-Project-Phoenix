local L_PLAYER_ID_LIST = {}
local MAX_ITEM_Normal = 14
local MAX_ITEM_Better = 24
local MAX_ITEM_Best = 14
local MAX_ITEM_Hard = 3
local MAX_ITEM_Special = 3
local HardNum = 0
local SpecialNum = 0
local YanHua = 
	{
		ItemClass = 91,
		ItemType = 60,
		ItemCount = 1
	}
local ItemList_Normal = 
                {
                 {"蓝鸟2",41, 82, 1},--1
				 {"铁壁之香",70, 23, 1},--2
				 {"伤害显示",70, 88, 1},--3
				 {"分流麦克风",70, 87, 1},--4
                 {"莉莉丝宝石",41, 56, 10},--5
				 {"蓝鸟2",41, 82, 1},--6
				 {"伤害显示",70, 88, 1},--7
				 {"蓝鸟2",41, 82, 1},--8
				 {"蓝鸟2",41, 82, 1},--9
				 {"白色蒸糕",48, 15, 1},--10
				 {"蒸糕",48, 16, 9},--11
				 {"烧毁的古籍",91, 63, 1},--12
				 {"复活卷轴",55, 0, 1},--13
				 {"孵蛋卷轴",91, 35, 1},--14
				}
local ItemList_Better = 
                 {
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
 				 {"彩蛋10",91, 45, 1},--24
				}
local ItemList_Best = 
                {
				 {"10x6包",90, 2, 1},--1
				 {"德·莱斯 黑宝石",79, 0, 1},--2
				 {"德·莱斯 红宝石",79, 1, 1},--3
				 {"德·莱斯 蓝宝石",79, 2, 1},--4
				 {"德·莱斯 绿宝石",79, 3, 1},--5
				 {"召唤卷轴 [西乌斯]",81, 10, 1},--6
				 {"召唤卷轴 [桑提斯]",81, 11, 1},--7
				 {"召唤卷轴 [德古?璢",81, 12, 1},--8
				 {"召唤卷轴 [卡米拉]",81, 13, 1},--9
				 {"黄色宝石3",41, 57, 1},--10
				 {"平凡的莉莉丝心灵",41, 71, 1},--11
				 {"平凡的莉莉丝希望",41, 70, 1},--12
				 {"宠物能药水",76, 49, 1},--13
				 {"7倍经验石",70, 86, 1},--14
				}
local ItemList_Hard = 
                {
				 {"深渊契约",96, 8, 1},--1
				 {"混沌契约",96, 9, 1},--2
				 {"遗忘契约",96, 10, 1},--3
				}
local ItemList_Special = 
                {
				 {"满月卡",46, 5, 1},--1
				 {"神奇的石头",40, 48, 1},--2
				 {"莉莉丝宝石3",41, 72, 1},--3
				}				
local TempLogFileName = "../script/zone/HappyNewYear/HappyNewYear.log"
function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "HayypeNewYear"
	ZoneCreateInfo.TemplateZoneID = 1006
	ZoneCreateInfo.StartPosX = 15
	ZoneCreateInfo.StartPosY = 30
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true

end

function OnInitialize()
	DEScriptZone:SetPKZone(false);
	DEScriptZone:SetTimer(1, 1000 * 60 , 1);--1 minuete
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
function OnUseItemInGear(CreatureInfo, ItemClass, ItemType)
	return IsAbleToUseItem(CreatureInfo, ItemClass, ItemType)
end

function OnUseItemInQuick(CreatureInfo, ItemClass, ItemType)
	return IsAbleToUseItem(CreatureInfo, ItemClass, ItemType)
end

function OnUseItemInInventory(CreatureInfo, ItemClass, ItemType)
	return IsAbleToUseItem(CreatureInfo, ItemClass, ItemType)
end
function IsAbleToUseItem(CreatureInfo, ItemClass, ItemType)
	Result = true
	
	if ItemClass == 91 and ItemType == 60 then			-- 轨颇捞绢 玖 荤侩  陛瘤
		Result = false
	end

	if ( Result == false ) then
		DEScriptZone:SendSystemMessage(CreatureInfo, 0, "聊天框输入:欢度国庆 就可以触发祝福抽奖了！")
	end
			
	return Result
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
		         --DEScriptZone:SendSystemMessage(Player.Info, 0, "现在没有经验奖励啦,用完烟花快出去吧.")
				AdvLevel = DEScript:GetAdvancementClassLevel(Player.Info)
				if AdvLevel < 200 then
				DEScriptZone:GiveExpAdv(Player.Info,3000000)
				end
		end	
		
	end
end


function OnBeforeEnterZone( CreatureInfo )
    --   AdvLevel = DEScript:GetAdvancementClassLevel(CreatureInfo)
     --	 if AdvLevel >0 then
	-- return true
	-- else
	-- DEScriptZone:SendSystemMessage(CreatureInfo, 0, "只有转职职业才可以进入.")
	 --DEScriptZone:SendSystemMessage(CreatureInfo, 0, "活动结束,下次开发时间请关注官网www.t1dk.com")
	 return false
	-- end
	--if DEScriptZone:HaveItem(CreatureInfo, YanHua) == false then
	--DEScriptZone:SendSystemMessage(CreatureInfo, 0, "错过时间,还剩余新年烟花的玩家可以进入.")
	--return false
	--else
	--DEScriptZone:SendSystemMessage(CreatureInfo, 0, "现在没?芯榻崩?用完烟花快出去吧.")
	return true
	--end
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


	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnResurrectCreature")
	ZoneCoordInfo.ZoneID = DEScriptZone:GetZoneID()

	if DEScript:IsSlayer(CreatureInfo) then
		ZoneCoordInfo.X = 46
        ZoneCoordInfo.Y = 49
	elseif DEScript:IsVampire(CreatureInfo) then
		ZoneCoordInfo.X = 46
        ZoneCoordInfo.Y = 49
	elseif DEScript:IsOusters(CreatureInfo) then
		ZoneCoordInfo.X = 46
        ZoneCoordInfo.Y = 49
	end

	return true;
end

function RanDomGift( CreatureInfo)
	    local rate = math.random(10,999)
		local rateMesg = math.modf(rate/10)

		if rate < 750 then  -- 进入普通物品列表 75% 几率
			local randI_Index = math.random(1, MAX_ITEM_Normal)
			local Item = ItemList_Normal[randI_Index]
			GiveItem(CreatureInfo, Item[2], Item[3], Item[4])
			
			local LogBuf = string.format("%s:扔出%s点,获得ItemClass:%s ItemType:%s 数量:%s", CreatureInfo.Name,rate,Item[2], Item[3], Item[4])
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local MSEEAGE = string.format("[国庆抽奖活动]:您扔出了：%s点,获得参与奖:%s!再接再厉哦，大奖等着你",rateMesg,Item[1])
			DEScriptZone:SendSystemMessage(CreatureInfo, 0,MSEEAGE )

		
		elseif rate >=750 and rate <950 then  --进入中级物品列表 25%几率
			local randI_Index = math.random(1, MAX_ITEM_Better)
			local Item = ItemList_Better[randI_Index]
			GiveItem(CreatureInfo, Item[2], Item[3], Item[4])
			
			local LogBuf = string.format("%s:扔出%s点,获得ItemClass:%s ItemType:%s 数量:%s", CreatureInfo.Name,rate,Item[2], Item[3], Item[4])
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local MSEEAGE = string.format("[国庆?榻被疃痌:您扔出了：%s点,获得二等奖:%s!再接再厉哦，大奖等着你",rateMesg,Item[1])
			DEScriptZone:SendSystemMessage(CreatureInfo, 0,MSEEAGE )
			
		
	    elseif rate >=950 and rate <=999 then --进入高级物品列表 5%几率
			local randI_Index = math.random(1, MAX_ITEM_Best)
			local Item = ItemList_Best[randI_Index]
			GiveItem(CreatureInfo, Item[2], Item[3], Item[4])
			
			local LogBuf = string.format("%s:扔出%s点,获得ItemClass:%s ItemType:%s 数量:%s", CreatureInfo.Name,rate,Item[2], Item[3], Item[4])
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local Msg = string.format("恭喜[%s]扔出:%s点,获得一等奖:%s",CreatureInfo.Name,rateMesg,Item[1])
			DEScriptZone:BroadcastSystemMessageInZone(8, Msg)
			
		end
		
		
		if HardNum == 1000 then
			local randI_Index = math.random(1, MAX_ITEM_Hard)
			local Item = ItemList_Hard[randI_Index]
			GiveHard(CreatureInfo, Item[2], Item[3], Item[4])
			
			local LogBuf = string.format("%s:扔出%s点,获得ItemClass:%s ItemType:%s 数量:%s", CreatureInfo.Name,rate,Item[2], Item[3], Item[4])
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local Msg = string.format("恭喜[%s]扔出了:100点满点获得特等奖:%s！",CreatureInfo.Name,Item[1])
			DEScriptZone:BroadcastSystemMessageInZone(8, Msg)
			ClearHardNum()		
		end
		
		if SpecialNum == 150 then
			local randI_Index = math.random(1, MAX_ITEM_Special)
			local Item = ItemList_Special[randI_Index]
			GiveItem(CreatureInfo, Item[2], Item[3], Item[4])
			
			local LogBuf = string.format("%s:扔出%s点,获得ItemClass:%s ItemType:%s 数量:%s", CreatureInfo.Name,rate,Item[2], Item[3], Item[4])
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local Msg = string.format("恭喜[%s]扔出了:99点获得一等奖:%s！",CreatureInfo.Name,Item[1])
			DEScriptZone:BroadcastSystemMessageInZone(8, Msg)
			ClearSpecialNum()		
		end	
		
		SpecialNum = SpecialNum+1
		HardNum = HardNum+1
		
	return true
end
function ClearHardNum()
         HardNum = 0
end
function ClearSpecialNum()
         SpecialNum = 0
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
		Options = "ATTR+10,ATTR+10,PDAM+10,MDAM+10,VIS+10,PDAM+10,MDAM+10",
		LimitedTime = 0
	}
	ItemInfo = {}
	ItemInfo = GiveItemInfo
    DEScriptZone:GiveItem(PlayerInfo, ItemInfo)
	return true
end
function OnSay( CreatureInfo, Mesg )
    if DEScript:IsGod(CreatureInfo) == true then
	       if  Mesg == "2" then
	         local Msg = string.format("当前积累：%s",HardNum)
	         DEScriptZone:SendSystemMessage(CreatureInfo,0,Msg)
	       end
		   if  Mesg == "start" then
	         HardNum = 999
	       end
	elseif DEScript:IsPLAYER(CreatureInfo) == true then
	    if  Mesg == "欢度国庆" then
		    if DEScriptZone:HaveItem(CreatureInfo, YanHua) == true then
			   if DEScriptZone:TakeItem(CreatureInfo, YanHua) == true then 
				   RanDomGift(CreatureInfo)
			   else
					local Msg = string.format("国庆礼包扣除失败，请检查背包是否有国庆礼包!!!")
					DEScriptZone:SendSystemMessage(CreatureInfo,0,Msg)
			   end
			else
			   	local Msg = string.format("你没有国庆礼包，无法抽奖!!!")
	            DEScriptZone:SendSystemMessage(CreatureInfo,0,Msg)
		    end
		end
	end
    return true
end
function OnSetStartPos(CreatureInfo, PositionInfo)

    if DEScript:IsSlayer(CreatureInfo) then
		PositionInfo.X = 15
        		PositionInfo.Y = 30
	elseif DEScript:IsVampire(CreatureInfo) then
		PositionInfo.X = 31
        		PositionInfo.Y = 14
	elseif DEScript:IsOusters(CreatureInfo) then
		PositionInfo.X = 47
        		PositionInfo.Y = 33
	end

end