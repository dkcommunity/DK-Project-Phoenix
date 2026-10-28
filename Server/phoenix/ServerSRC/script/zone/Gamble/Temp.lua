dofile("../script/zone/Gamble/Temp2.lua")


L_XiaZhu_1List = { }
L_XiaZhu_2List = { }

local L_Zhuang = {
	   ID = 0,
       Info = nil,
       XiaZhu = 0,
       Total_1XiaZhu = 0,
       Total_2XiaZhu = 0,
       Name = nil,
       Account = nil,
       IsInZone = true
}


local TempLogFileName = "../script/zone/Gamble/Temp.log"
local L_CountDown = 30
local L_IsFighting = false;
local L_IsStop = false;


function OnWarpZone(Creature)
	-- 捞拌 -> 泅技 捞悼矫糯
	
	local WarpID = 0;
	local WarpX = 0;
	local WarpY = 0;
	
	if DEScript:IsSlayer(Creature) then
		WarpID = 12
		WarpX = 209
		WarpY = 104
	elseif DEScript:IsVampire(Creature) then
		WarpID = 1003
		WarpX = 62
		WarpY = 64
	elseif DEScript:IsOusters(Creature) then
		WarpID = 1311
		WarpX = 24
		WarpY = 73	
	end
    
	DEScriptZone:TransportCreature(Creature, WarpID, WarpX, WarpY)

end


function OnTraverseCreature( CreatureInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseCreature")
end

function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "Temp"	
	ZoneCreateInfo.TemplateZoneID = 1006
	ZoneCreateInfo.StartPosX = 29
	ZoneCreateInfo.StartPosY = 29
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true	
	
	DEScriptZone:SetTimer(1, 4000 , 1)
end 

function OnTraverseMonster( MonsterInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseMonster")
end

function OnInitialize()
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnInitialize")
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnAttackCreature")
end


function ClearGameData()

	 L_XiaZhu_2List = nil
	 L_XiaZhu_1List = nil
	 L_XiaZhu_2List = { }
	 L_XiaZhu_1List = { }	 
     L_Zhuang.Info = nil
	 L_Zhuang.XiaZhu = 0	
	 L_Zhuang.Total_1XiaZhu = 0
	 L_Zhuang.Total_2XiaZhu = 0
	 L_Zhuang.Name = nil
	 L_Zhuang.Account = nil;
     L_Zhuang.IsInZone = true

	 L_IsFighting = false
end

--1号赢，
function OneWin(  )

	local TotalCount = L_Zhuang.XiaZhu
	
	for i,Player in pairs(L_XiaZhu_1List) do
        local WinNumber = Player.XiaZhu * 2
        TotalCount = TotalCount - Player.XiaZhu
        
        if Player.IsInZone == true then
			GiveItem( Player.Info, WinNumber )
		else
			local LogBuf = string.format("%s      %d", Player.Name, WinNumber)
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local nShopCount = WinNumber / 10;
			DEScriptZone:BroadcastSystemMessageInZone(4, string.format("Account:%s,Name:%s车中%d本书",Player.Account,Player.Name, nShopCount));
			--DEScriptZone:GiveGoodsItem(Player.Info, 2279, nShopCount)
			DEScriptZone:GiveGoodsItemForLogOut("War", Player.Account, Player.Name, 2279, nShopCount);
			
		end
	end

    
	for i,Player in pairs(L_XiaZhu_2List) do
        TotalCount = TotalCount + Player.XiaZhu        
	end
	
	--把输的那边的书给庄家
	
	if TotalCount > 0 then
		if L_Zhuang.IsInZone == true then
			GiveItem( L_Zhuang.Info, TotalCount )
		else
			local LogBuf = string.format("%s      %d", L_Zhuang.Name, TotalCount)
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local nShopCount = TotalCount / 10;
			DEScriptZone:BroadcastSystemMessageInZone(4, string.format("Account:%s,Name:%s车中%d本书",L_Zhuang.Account, L_Zhuang.Name, nShopCount));
			--DEScriptZone:GiveGoodsItem(L_Zhuang.Info, 2279, nShopCount)
			DEScriptZone:GiveGoodsItemForLogOut("War", L_Zhuang.Account, L_Zhuang.Name, 2279, nShopCount);
			
		end
	end
	
	ClearGameData()

end


--2号赢
function TwoWin(  )

	local TotalCount = L_Zhuang.XiaZhu
	
	for i,Player in pairs(L_XiaZhu_2List) do
        local WinNumber = Player.XiaZhu * 2
        TotalCount = TotalCount - Player.XiaZhu
        
        if Player.IsInZone == true then
			GiveItem( Player.Info, WinNumber )
		else
			local LogBuf = string.format("%s      %d", Player.Name, WinNumber)
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local nShopCount = WinNumber / 10;
			DEScriptZone:BroadcastSystemMessageInZone(4, string.format("Account:%s,Name:%s车中%d本书", Player.Account,Player.Name, nShopCount));
			--DEScriptZone:GiveGoodsItem(Player.Info, 2279, nShopCount)
			DEScriptZone:GiveGoodsItemForLogOut("War", Player.Account, Player.Name, 2279, nShopCount);
						
		end
	end

    
	for i,Player in pairs(L_XiaZhu_1List) do
        TotalCount = TotalCount + Player.XiaZhu        
	end
	
	
	--把输的那边的书给庄家
	if TotalCount > 0 then
		if L_Zhuang.IsInZone == true then
			GiveItem( L_Zhuang.Info, TotalCount )
		else
			local LogBuf = string.format("%s      %d", L_Zhuang.Name, TotalCount)
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local nShopCount = TotalCount / 10;
			DEScriptZone:BroadcastSystemMessageInZone(4, string.format("Account:%s,Name:%s车中%d本书",L_Zhuang.Account, L_Zhuang.Name, nShopCount));
			--DEScriptZone:GiveGoodsItem(L_Zhuang.Info, 2279, nShopCount)
			DEScriptZone:GiveGoodsItemForLogOut("War", L_Zhuang.Account, L_Zhuang.Name, 2279, nShopCount);
			
		end
	end
	
	ClearGameData()
	
end

function GetListBookCount(XiaZhuList)

	local TotalCount = 0
	for i,Player in pairs(XiaZhuList) do
        TotalCount = TotalCount + Player.XiaZhu        
	end
	
	return TotalCount
	
end

function OnKillCreature( AttackerCreatureInfo, DeadCreatureInfo )


     if oneinfo ~= nil and oneinfo.ObjectID == DeadCreatureInfo.ObjectID then
        DEScriptZone:BroadcastSystemMessageInZone(8, "2号获胜利")
        DEScriptZone:SetTimer(5, 5000 , 1)
        TwoWin()
		oneinfo = nil
	 end
	 
     if twoinfo ~= nil and twoinfo.ObjectID == DeadCreatureInfo.ObjectID then
        DEScriptZone:BroadcastSystemMessageInZone(8, "1号获胜利")
        DEScriptZone:SetTimer(5, 5000 , 1)
        OneWin()
        twoinfo = nil
	 end
	
	 
end


function ClearCreature()
     if oneinfo ~= nil then
		DEScript:SetHP(oneinfo, 0)
		oneinfo = nil
     end

     if twoinfo ~= nil then
		DEScript:SetHP(twoinfo, 0)
		twoinfo = nil
     end
end

--归还所有东西
function GiveAllThingsBack()

	for i,Player in pairs(L_XiaZhu_1List) do
        if Player.IsInZone == true then
			GiveItem( Player.Info, Player.XiaZhu )
		else
			local LogBuf = string.format("服务器重启 %s      %d", Player.Name, Player.XiaZhu)
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			
			local nShopCount = Player.XiaZhu / 10;
			DEScriptZone:GiveGoodsItemForLogOut("War", Player.Account, Player.Name, 2279, nShopCount);
		end
    end
    
    
	for i,Player in pairs(L_XiaZhu_2List) do
        if Player.IsInZone == true then
			GiveItem( Player.Info, Player.XiaZhu )
		else
			local LogBuf = string.format("服务器重启 %s      %d", Player.Name, Player.XiaZhu)
			DEScript:WriteFileLog(TempLogFileName, LogBuf)
			local nShopCount = Player.XiaZhu / 10;
			DEScriptZone:GiveGoodsItemForLogOut("War", Player.Account, Player.Name, 2279, nShopCount);	
		end
    end    
    
    
	if L_Zhuang.IsInZone == true then
		GiveItem( L_Zhuang.Info, L_Zhuang.XiaZhu )
	else
		local LogBuf = string.format("服务器重启 %s      %d", L_Zhuang.Name, L_Zhuang.XiaZhu)
		DEScript:WriteFileLog(TempLogFileName, LogBuf)
		
		local nShopCount = L_Zhuang.XiaZhu / 10;
		DEScriptZone:GiveGoodsItemForLogOut("War", L_Zhuang.Account, L_Zhuang.Name, 2279, nShopCount);
	end    
    
end

function TransPortBack()

	for i,Player in pairs(L_XiaZhu_1List) do
        if Player.IsInZone == true then
			OnWarpZone(Player.Info);
		end
    end
    
    
	for i,Player in pairs(L_XiaZhu_2List) do
        if Player.IsInZone == true then
		    OnWarpZone(Player.Info);	
		end
    end    
    
    
	if L_Zhuang.IsInZone == true then
		OnWarpZone(L_Zhuang.Info);
	end   

end

function OnTimer( TimerID )


	if TimerID == 1 then
		if L_IsStop == true then
			DEScriptZone:BroadcastSystemMessageInZone(8, "服务器进行维护！！玩家将被传送出副本")
		else
			DEScriptZone:BroadcastSystemMessageInZone(8, "当前没有庄家!!!!!!!!!!")
		end
	end
	
	
	if TimerID == 5 then
		DEScriptZone:SetTimer(1, 4000 , 1)
		DEScriptZone:KillTimer(5)
	end
	
	if TimerID == 2 then
		ClearGameData()
		DEScriptZone:SetTimer(1, 2000 , 1)
		DEScriptZone:KillTimer(2)
	end
	
	if TimerID == 6 then
		TransPortBack();
		ClearGameData()
		DEScriptZone:SetTimer(1, 2000 , 1)
		DEScriptZone:KillTimer(6)
	end

    --停顿3秒后进入TIMEID-4
	if TimerID == 3 then
		DEScriptZone:SetTimer(4, 1000 , 1)--10秒后开始刷怪
		L_CountDown = 30
		DEScriptZone:KillTimer(3)
	end
	
	--每一秒执行一次
	if TimerID == 4 then	
		L_CountDown = L_CountDown - 1

		if L_IsStop == true then
		    DEScriptZone:KillTimer(4);
		    L_IsFighting = false;
		    GiveAllThingsBack();
		    DEScriptZone:BroadcastSystemMessageInZone(8, "服务器即将进行维护，本局结束，书本自动归还给玩家")
		    DEScriptZone:SetTimer(6, 2000 , 1)
		else
		    local msg = string.format("下注倒计时:%d   抢庄请输入 #坐庄 10的倍数",L_CountDown)
		    DEScriptZone:BroadcastSystemMessageInZone(8, msg)
			if L_CountDown == 0 then	
			
				if L_Zhuang.Total_1XiaZhu == 0 and L_Zhuang.Total_2XiaZhu == 0 then --如果没人下注 那么
					DEScriptZone:BroadcastSystemMessageInZone(8, "当前没人下注，取消庄家")
					DEScriptZone:SetTimer(2, 2000 , 1)
					GiveItem( L_Zhuang.Info, L_Zhuang.XiaZhu )--把书归还给庄家
				else
					AddMonsterInfo = one
					AddMonsterInfo.X = 31
					AddMonsterInfo.Y = 31
					AddMonsterInfo.Name = "1号"
					oneinfo = DEScriptZone:AddMonster(AddMonsterInfo)
					
					AddMonsterInfo = two
					AddMonsterInfo.X = 28
					AddMonsterInfo.Y = 28
					AddMonsterInfo.Name = "2号"
					twoinfo = DEScriptZone:AddMonster(AddMonsterInfo)		
					  
				    
					local oneBookCount = GetListBookCount(L_XiaZhu_1List)
					local TwoBookCount = GetListBookCount(L_XiaZhu_2List)
					local msg = string.format("统计：庄家：%d  1号：%d  2号：%d",L_Zhuang.XiaZhu, oneBookCount, TwoBookCount)
					DEScriptZone:BroadcastSystemMessageInZone(8, msg)
					
					L_IsFighting = true
				end		
				DEScriptZone:KillTimer(4)  
			end
		end
	end --Timer == 4

	
end


function OnBeforeEnterZone( CreatureInfo )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeEnterZone")
	return true
end

function OnAfterEnterZone( CreatureInfo )
    --DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterEnterZone")
end


function CheckOffLine( PlayerList, CreatureInfo)

	for i,Player in pairs(PlayerList) do
	
        if Player.ID == CreatureInfo.ObjectID then
			local bufMsg = string.format("%s 已经掉线了！！！！！！！！！！！！！！！！！！！！！！！！", Player.Name)
			DEScriptZone:BroadcastGCSay(L_Zhuang.Info, bufMsg,  16777215)
            Player.IsInZone = false
        end
        
	end
	
end

--玩家离开副本【异常掉线，游戏崩溃，主动离开 等情况都在这个函数里处理】
function OnBeforeLeaveZone( CreatureInfo )
	
	DEScriptZone:BroadcastGCSay(L_Zhuang.Info, "OnBeforeLeaveZone",  16777215)
	CheckOffLine(L_XiaZhu_1List, CreatureInfo)
	CheckOffLine(L_XiaZhu_2List, CreatureInfo)
	
	if L_Zhuang.ID == CreatureInfo.ObjectID then
	    L_Zhuang.IsInZone = false
	end
	
end

function OnAfterLeaveZone( CreatureInfo )
    --DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterLeaveZone")
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
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnResurrectCreature")
	return false
end


function OnSay( CreatureInfo, Mesg )

	--GM命令
	if DEScript:IsGod(CreatureInfo) then
		if Mesg == "*Stop" then
		    L_IsStop = true;
		end
		
		if Mesg == "*Start" then
			L_IsStop = false;
		end
		
		if Mesg == "fk" then
		   GiveItem(CreatureInfo, 400)
        end
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
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInGear")
	return true
end

function OnUseItemInInventory( CreatureInfo, ItemClass, ItemType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInInventory")
	return true
end

function OnUseItemInQuick( CreatureInfo, ItemClass, ItemType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseItemInQuick")
	return true
end

function OnUseInventorySkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseInventorySkill")
	return true
end

function OnUseObjectSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseObjectSkill")
	return true
end

function OnUseNamedSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseNamedSkill")
	return true
end

function OnUseSelfSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseSelfSkill")
	return true
end

function OnUseTileSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnUseTileSkill")
	return true
end

function OnBeforeUseMonsterSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeUseMonsterSkill")
	return true
end

function OnAfterUseMonsterSkill( CreatureInfo, SkillType )
	--DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterUseMonsterSkill")
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
    
    if Number == nil then
		Number = 0
	end

    return Number

end

--检测倍数
function CheckNumberMod( inNumber , Mod)

	local ret = inNumber % Mod;

	if inNumber == 0 or ret ~= 0  then
	    return false
	end

	return true

end


function IsContain(Mesg, subStr) --是否包含字符串

    local find = string.find(Mesg, subStr)

	if ( find ~= nil ) then
		return find
	end

	return 0

end


--拿走玩家身上的指定物品
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


--拿走玩家身上的指定物品
function GiveItem(PlayerInfo, count)

	local GiveItemInfo = 
	{
		--书
		ItemClass = 91,
		ItemType = 63,
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


--玩家进场景之后的欢迎信息，以及当前下注的情况
function WelcomeMessage(PlayerInfo, Mesg)

end

function ChangeZhuang(PlayerInfo, Mesg)

	if L_IsStop == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "服务器即将维护！！！",  16777215)
	    return	
	end

	if L_IsFighting == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "无法抢庄。游戏已经开始，请等下一轮！",  16777215)
	    return	
	end
	
	if L_Zhuang.Info.ObjectID == PlayerInfo.ObjectID then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "您已经是庄家了！",  16777215)
	    return		
	end
	
	local changeNumber = GetMesgNumber(Mesg, "#坐庄")
	
	if CheckNumberMod( changeNumber, 10) == false then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "抢庄失败，书本数量要10的倍数，如 20,30",  16777215)
	    return
	end
	
	local msg = string.format("L_Zhuang.XiaZhu = %d, changeNumber = %d", L_Zhuang.XiaZhu, changeNumber)
	DEScriptZone:BroadcastSystemMessageInZone(4, msg)
	
	
	if L_Zhuang.XiaZhu >= changeNumber then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "输入书本数量错误，不能抢庄！！",  16777215)
	    return
	end
	
	--扣除抢庄的人的书
	if TabkeItem(PlayerInfo, changeNumber) == false then --取得人物身上的书，没有书就返回
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "您身上的书不够！",  16777215)
		return
	end
	
	
	--归还庄家书籍
	if GiveItem(L_Zhuang.Info, L_Zhuang.XiaZhu) == true then
	
	    local msg = string.format("您的庄被抢，书籍%d已归还到您的背包，请检查。下注请输入 #下注1/2号 [10的倍数]", L_Zhuang.XiaZhu)
		DEScriptZone:BroadcastGCSay(L_Zhuang.Info, msg,  16777215)
		
	else
	    DEScriptZone:BroadcastGCSay(L_Zhuang.Info, "您的庄被抢，书籍暂时被服务器保管!",  16777215)
	end
	
	--DEScriptZone:SetTimer(4, 1000 , 1)--10秒后开始刷怪
	--L_CountDown = 30
	
	DEScriptZone:SetTimer(3, 1000 * 3, 1)--消息停顿3秒开始倒计时
	DEScriptZone:KillTimer(4)
	
	local strZhuang = string.format("%s用%d本书抢夺了%s的庄,庄家已更改！！！！！", PlayerInfo.Name, changeNumber, L_Zhuang.Info.Name)
	DEScriptZone:BroadcastSystemMessageInZone(8, strZhuang)
	
	--更改庄家
	L_Zhuang.Info = PlayerInfo
	L_Zhuang.XiaZhu = changeNumber
	L_Zhuang.ID = PlayerInfo.ObjectID
	L_Zhuang.Name = PlayerInfo.Name
	L_Zhuang.Account = PlayerInfo.Account
    L_Zhuang.IsInZone = true
	
end


function ZuoZhuang(PlayerInfo, Mesg)

	if L_IsStop == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "服务器即将维护！！！",  16777215)
	    return	
	end

	if L_IsFighting == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "无法坐庄。游戏已经开始，请等下一轮！",  16777215)
	    return	
	end
	
	
	local Number = GetMesgNumber(Mesg, "#坐庄")
	
	if CheckNumberMod( Number, 10) == false then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "坐庄失败，输入书本数量要10的倍数，如 20,30",  16777215)
	    return
	end
	
	if TabkeItem(PlayerInfo, Number) == false then --取得人物身上的书，没有书就返回
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "您身上的书不够！",  16777215)
		return
	end
	
	L_Zhuang.Info = PlayerInfo
	L_Zhuang.XiaZhu = Number
	L_Zhuang.Name = PlayerInfo.Name
	L_Zhuang.Account = PlayerInfo.Account
	L_Zhuang.ID = PlayerInfo.ObjectID
	L_Zhuang.IsInZone = true
	
	local msg = string.format("[%s] 已获得庄家权利, 其他玩家开始下注", PlayerInfo.Name)
	DEScriptZone:BroadcastSystemMessageInZone(8, msg)
	DEScriptZone:SetTimer(3, 1000 * 3, 1)--消息停顿3秒开始倒计时
	
	ClearCreature()
	DEScriptZone:KillTimer(1)
end


--庄家下注，支持抬注
function DoZuoZhuang(PInfo, Mesg)
	
	local ID = PInfo.ObjectID
	
	if L_Zhuang.Info ~= nil and L_Zhuang.Info~= PInfo then
	    DEScriptZone:BroadcastGCSay(PInfo, Mesg,  16777215)
	    ChangeZhuang(PInfo, Mesg)
	else
	   ZuoZhuang(PInfo, Mesg)
	end
	
end


--2号下注
function DoXiaZhu_2(PlayerInfo, Mesg)

	if L_IsStop == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "服务器即将维护！！！",  16777215)
	    return	
	end

	if L_IsFighting == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "无法下注。游戏已经开始，请等下一轮！",  16777215)
	    return	
	end
	
	local ID = PlayerInfo.ObjectID

	if L_XiaZhu_2List[ ID ] ~= nil then  --已经下注过的玩家不允许再次下注
		DEScriptZone:BroadcastGCSay(PlayerInfo, "您已经下过注了，不允许再下注！",  16777215)
	    return
	end
	
	if L_Zhuang.Info == nil then
		DEScriptZone:BroadcastGCSay(PlayerInfo, "目前还没有庄家，无法下注！想做庄的请输入: #坐庄 [100的倍数]",  16777215)
		return
	end
	
	--分析下注数量
	local Number = GetMesgNumber(Mesg, "#下注2号")
	
	if CheckNumberMod(Number, 10) == false then
		DEScriptZone:BroadcastGCSay(PlayerInfo, "您输入的数字不对，要10的倍数哟！！",  16777215)
		return
	end
	
	if L_Zhuang.Total_2XiaZhu + Number > L_Zhuang.XiaZhu then
		local strMsg = string.format("下注数量不能超过总庄，目前1号还可以下注:%d", L_Zhuang.XiaZhu - L_Zhuang.Total_2XiaZhu)
		DEScriptZone:BroadcastGCSay(PlayerInfo, strMsg,  16777215)
		return
	end	
	
	if TabkeItem(PlayerInfo, Number) == false then --取得人物身上的书，没有书就返回
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "您身上的书不够！",  16777215)
		return
	end
	
	local msg = string.format("%s下注2号%d本书", PlayerInfo.Name, Number)	
	DEScriptZone:BroadcastSystemMessageInZone(4, msg)

	local NewPlayer = {
				ID = PlayerInfo.ObjectID,
                Info = PlayerInfo,
                Account = PlayerInfo.Account,
                XiaZhu = Number,
                Name = PlayerInfo.Name,
                IsInZone = true
                }
                
    L_Zhuang.Total_2XiaZhu = L_Zhuang.Total_2XiaZhu + Number                
                   
	L_XiaZhu_2List[ID] = NewPlayer
	
end


--1号下注
function DoXiaZhu_1(PlayerInfo, Mesg)

	if L_IsStop == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "服务器即将维护！！！",  16777215)
	    return	
	end

	if L_IsFighting == true then
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "无法下注。游戏已经开始，请等下一轮！",  16777215)
	    return	
	end
	
	local ID = PlayerInfo.ObjectID
	if L_XiaZhu_1List[ ID ] ~= nil then  --已经下注过的玩家不允许再次下注
		DEScriptZone:BroadcastGCSay(PlayerInfo, "您已下过注，不允许再次下注",  16777215)
	    return
	end
	
	if L_Zhuang.Info == nil then
		DEScriptZone:BroadcastGCSay(PlayerInfo, "目前还没有庄家，无法下注！想做庄的请输入: #坐庄 [100的倍数]",  16777215)
		return
	end
	
	--分析下注数量
	local Number = GetMesgNumber(Mesg, "#下注1号")
	
	if CheckNumberMod(Number, 10) == false then
		DEScriptZone:BroadcastGCSay(PlayerInfo, "亲爱的玩家！您输入的数字不对，要10的倍数哟！！！",  16777215)
		return
	end
	
	if L_Zhuang.Total_1XiaZhu + Number > L_Zhuang.XiaZhu then
		local strMsg = string.format("下注数量不能超过总庄，目前1号还可以下注:%d", L_Zhuang.XiaZhu - L_Zhuang.Total_1XiaZhu)
		DEScriptZone:BroadcastGCSay(PlayerInfo, strMsg,  16777215)
		return
	end
	
	if TabkeItem(PlayerInfo, Number) == false then --取得人物身上的书，没有书就返回
	    DEScriptZone:BroadcastGCSay(PlayerInfo, "亲爱的玩家！您身上的书不够！",  16777215)
		return
	end

    local msg = string.format("%s下注1号%d本书", PlayerInfo.Name, Number)	
	DEScriptZone:BroadcastSystemMessageInZone(4, msg)

	local NewPlayer = {
				ID = PlayerInfo.ObjectID,
                Info = PlayerInfo,
                Account = PlayerInfo.Account,
                XiaZhu = Number,
                Name = PlayerInfo.Name,
                IsInZone = true
                }
    L_Zhuang.Total_1XiaZhu = L_Zhuang.Total_1XiaZhu + Number            
                   
	L_XiaZhu_1List[ID] = NewPlayer;
	
end
