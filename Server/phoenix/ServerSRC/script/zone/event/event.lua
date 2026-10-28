local YanHua = 
	{
		ItemClass = 91,
		ItemType = 60,
		ItemCount = 1
	}
local TempLogFileName = "../script/zone/event/event.log"
function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "event"
	ZoneCreateInfo.TemplateZoneID = 1006
	ZoneCreateInfo.StartPosX = 15
	ZoneCreateInfo.StartPosY = 30
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true

end

function OnInitialize()
	DEScriptZone:SetPKZone(false);
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
		DEScriptZone:SendSystemMessage(CreatureInfo, 0, "聊天框输入:51活动 就可以获得道具奖励！")
	end
			
	return Result
end



function OnBeforeEnterZone( CreatureInfo )
	 DEScriptZone:SendSystemMessage(CreatureInfo, 0, "51活动专用场景,请关注官网www.t1dk.com")
	 return true
end


function OnAfterEnterZone( CreatureInfo )
			local MSEEAGE = string.format("聊天框输入:51活动 即可获取51礼包的奖励道具")
			DEScriptZone:SendSystemMessage(CreatureInfo, 0,MSEEAGE )
end

function OnAfterLeaveZone( CreatureInfo )
end

function OnResurrectCreature( CreatureInfo, ZoneCoordInfo )

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
function OnSay( CreatureInfo, Mesg )
	    if  Mesg == "51活动" then
		    if DEScriptZone:HaveItem(CreatureInfo, YanHua) == true then
			   if DEScriptZone:TakeItem(CreatureInfo, YanHua) == true then 
				   DEScriptZone:GiveGoodsItem(CreatureInfo,2278,1)--50个蓝鸟2
				   DEScriptZone:GiveGoodsItem(CreatureInfo,2274,5)--5个7倍
				   DEScriptZone:GiveGoodsItem(CreatureInfo,2286,1)--LLS3一个
				   DEScriptZone:GiveGoodsItem(CreatureInfo,2287,2)--神奇的石头1个
				   DEScriptZone:GiveGoodsItem(CreatureInfo,2288,1)--BOSS 1
				   DEScriptZone:GiveGoodsItem(CreatureInfo,2289,1)--BOSS 2
				   DEScriptZone:GiveGoodsItem(CreatureInfo,2290,1)--BOSS 3
				   --DEScriptZone:GiveGoodsItem(CreatureInfo,2292,2)--手镯套测试
				   local MSEEAGE = string.format("恭喜!51活动礼包奖励已经发送到您的购物车请查收,50个蓝鸟2,5个7倍,1个平凡的莉莉丝,神奇的石头2个,BOSS召唤卷3个")
				   DEScriptZone:SendSystemMessage(CreatureInfo, 0,MSEEAGE )
			   else
					local Msg = string.format("礼包扣除失败，请检查背包是否有 51活动礼包!!!")
					DEScriptZone:SendSystemMessage(CreatureInfo,0,Msg)
			   end
			else
			   	local Msg = string.format("你没有 51活动礼包，无法获得奖励!!!")
	            DEScriptZone:SendSystemMessage(CreatureInfo,0,Msg)
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