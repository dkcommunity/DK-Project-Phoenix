

local L_RACE_LIST = {}
local L_PLAYER_ID_LIST = {}
local L_PLAYER_NAME_LIST = {}


local L_RACE_VAMPIRE = 0
local L_RACE_SLAYER = 1
local L_RACE_OUSTERS = 2
local l_RACE_NULL = 3

local L_SCORE = {}
--分数

local L_NO_1 = {}  
local L_NO_2 = {}
local L_NO_3 = {}

local TEMP_STRING = "                                            "
local TEMP_KILL_SECOND = 15
local L_DEBUG = true

local L_TIMER_TYPE_KILLING = 1
local L_TIMER_TYPE_RELIVE = 2

local L_WarStartTime = 0
local L_WarFightingTime = 20        --单位（秒） 1分钟  战斗时间
		
local L_NextFightWaitTime = 60		--单位（秒） 1分钟  下一战场等待时间

local L_CanEnter = false
local L_bClearAllData = false

function SysMsg( type, msg )
	DEScriptZone:BroadcastSystemMessageInZone(type, msg)
end

function DebugMsg(Player,msg)
    if L_DEBUG == true then
    
        if Player ~= nil then
			DEScriptZone:BroadcastGCSay(Player.Info, "1111111111111111111111111111",  16777215)
        else 
			DEScriptZone:BroadcastSystemMessageInZone(4, msg)
		end
			
	end
end

function ClearAllData()

	
	L_PLAYER_ID_LIST = nil
	L_PLAYER_NAME_LIST = nil
	L_PLAYER_ID_LIST = {}
	L_PLAYER_NAME_LIST = {}	
	
	L_RACE_LIST[L_RACE_VAMPIRE] = nil;
	L_RACE_LIST[L_RACE_SLAYER] = nil;
	L_RACE_LIST[L_RACE_OUSTERS] = nil;
	L_RACE_LIST[L_RACE_VAMPIRE] = {};
	L_RACE_LIST[L_RACE_SLAYER] = {};
	L_RACE_LIST[L_RACE_OUSTERS] = {};

	
	L_SCORE[ L_RACE_VAMPIRE ] = 0;
	L_SCORE[ L_RACE_SLAYER ] = 0;
	L_SCORE[ L_RACE_OUSTERS ] = 0;
	L_SCORE[ l_RACE_NULL ] = 0;
	
	
	L_NO_1[L_RACE_VAMPIRE] = {Info = nil, Name = "", Kill = 0};
	L_NO_1[L_RACE_SLAYER] = {Info = nil, Name = "", Kill = 0};
	L_NO_1[L_RACE_OUSTERS] = {Info = nil, Name = "", Kill = 0};
	
	L_NO_2[L_RACE_VAMPIRE] = {Info = nil, Name = "", Kill = 0};
	L_NO_2[L_RACE_SLAYER] = {Info = nil, Name = "", Kill = 0};
	L_NO_2[L_RACE_OUSTERS] = {Info = nil, Name = "", Kill = 0};
	
	L_NO_3[L_RACE_VAMPIRE] = {Info = nil, Name = "", Kill = 0};
	L_NO_3[L_RACE_SLAYER] = {Info = nil,Name = "", Kill = 0};
	L_NO_3[L_RACE_OUSTERS] = {Info = nil, Name = "", Kill = 0};
	
	L_bClearAllData = true
		
end

--给玩家物品
function GiveItem(PlayerInfo, count)

	if PlayerInfo == nil then
		return;
	end

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



function SendRemainTime(Creature, remainTime, timerDesc)
	if timerDesc == nil then
		timerDesc = ""
	end
			
	DEScriptZone:SendSystemMessage(Creature, 7, timerDesc.." ["..remainTime.."]")
end


function SendAllRemainTime(Time, timerDesc)
	for i,Player in pairs(L_PLAYER_ID_LIST) do
			SendRemainTime(Player.Info, Time, timerDesc);
    end
end

function N_Msg(msg1, msg2, msg3)
    DEScriptZone:BroadcastSystemMessageInZone(4, msg1)
    DEScriptZone:BroadcastSystemMessageInZone(4, msg2)
    DEScriptZone:BroadcastSystemMessageInZone(4, msg3)
end

local CPlayer = {
	               New = function(self, Info)
                       local o = {};
				       setmetatable(o, {__index = self});
				       
				       o.Info = Info;
				       o.ID = Info.ObjectID;
				       o.Race = l_RACE_NULL;
				       o.Name = Info.Name;
				       o.Kill = 0;
				       o.ContinueKill = 0;
				       o.TempKill = 0;
				       o.Dead = 0;
				       o.IsDead = false;
				       o.KillingTimer = 0;
				       o.TimerType = 0;
				       
                       return o
                   end,
                   
                   AddEffect = function(self, EffectID)
						EffectInfo =
						{
							EffectCastCreature = nil,
							EffectClass = EffectID,
							NextTime = 0,
							Deadline = 99999999
						}
						
						DEScriptZone:AddEffectToCreature(EffectInfo, self.Info)	                   
                   end,
                   
                   StartTimer = function(self, type, time)
                       self.TimerType = type;
                       DEScriptZone:SetTimer(self.ID, time , 1);
                       DEScriptZone:BroadcastSystemMessageInZone(4, string.format("%s SetTimer!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", self.Name));
                   end,
                   
                   setFlag = function(self)
				       DEScriptZone:setFlag( self.Info );
				       self:StartTimer(L_TIMER_TYPE_RELIVE, 1000*10);
				       
				       DEScriptZone:BroadcastSystemMessageInZone(4, string.format("%s复活后获得了10秒无敌", self.Name));
                   end,
                   
                   removeFlag = function(self)
                       DEScriptZone:removeFlag( self.Info );
                       DEScriptZone:BroadcastSystemMessageInZone(4, string.format("%s的10秒无敌结束", self.Name));
                   end,
                   
                   RemoveEffect = function(self, EffectID)
                        
                        EffectInfo =
                        {
	                        EffectCastCreature = nil,
	                        EffectClass = EffectID,
                        }
                        
						DEScriptZone:RemoveEffectFromCreature(EffectInfo, self.Info);
                   end,
                   
                   ResetInfo = function(self, Info)
                       self.Info = Info;
				       self.ID = Info.ObjectID;
				       self.IsDead = false;
                   end,
                   
                   SetRace = function(self, Race)
				       self.Race = Race;
                   end,
                   
                   
                   OnBeKill = function(self, Killer) --被杀
                       self.Dead = self.Dead + 1;
                       self.IsDead = true;
                       
                       if self.ContinueKill >= 10 then
                           SysMsg(8, string.format("%s 终结了 %s 超越神似的杀戮!!!", Killer.Name, self.Name) )
                       end
                       
                       self.ContinueKill = 0;
                       
                       --if self.Dead == 4 then
					      --SysMsg(8, string.format("%s 已经死成猪了!!!", self.Name) )
                       --end

                       --if self.Dead == 7 then
                           --SysMsg(8, string.format("%s 已经死成鬼了!!谁去帮帮他吧", self.Name) )
                       --end
                       
                       --if self.Dead == 11 then
                           --SysMsg(8, string.format("%s 已经死得超越鬼了!!!这哥们已经无药可救了!!!", self.Name) )
                       --end                                              
                   end,
                   
                   
                   
                   
                   OnKillPlayer = function(self, Dead)--击杀某人
                   
						L_SCORE[ self.Race ] = L_SCORE[ self.Race ] + 10
						SysMsg(4, string.format("@<Score> 鬼:%d   人:%d   魔:%d", L_SCORE[ L_RACE_VAMPIRE ], L_SCORE[ L_RACE_SLAYER ], L_SCORE[ L_RACE_OUSTERS ]) )		
                        self.Kill = self.Kill + 1;                                   --总累计击杀
                        self.ContinueKill = self.ContinueKill + 1;                   --连续击杀 死亡即清0
                        
                        --开始计算排行榜
                        if L_NO_1[ self.Race ].Kill < self.Kill then
                            L_NO_1[ self.Race ].Kill = self.Kill;
                            L_NO_1[ self.Race ].Info = self.Info;
                            L_NO_1[ self.Race ].Name = self.Name;
                            N_Msg(string.format("@<NO_1_V> %s:%d", L_NO_1[ L_RACE_VAMPIRE ].Name, L_NO_1[ L_RACE_VAMPIRE ].Kill),string.format("@<NO_1_S> %s:%d", L_NO_1[ L_RACE_SLAYER ].Name, L_NO_1[ L_RACE_SLAYER ].Kill),string.format("@<NO_1_O> %s:%d", L_NO_1[ L_RACE_OUSTERS ].Name, L_NO_1[ L_RACE_OUSTERS ].Kill));
                        elseif L_NO_2[ self.Race ].Kill < self.Kill then
                            L_NO_2[ self.Race ].Kill = self.Kill;
                            L_NO_2[ self.Race ].Info = self.Info;
                            L_NO_2[ self.Race ].Name = self.Name;
                            N_Msg(string.format("@<NO_2_V> %s:%d", L_NO_2[ L_RACE_VAMPIRE ].Name, L_NO_2[ L_RACE_VAMPIRE ].Kill),string.format("@<NO_2_S> %s:%d", L_NO_2[ L_RACE_SLAYER ].Name, L_NO_2[ L_RACE_SLAYER ].Kill),string.format("@<NO_2_O> %s:%d", L_NO_2[ L_RACE_OUSTERS ].Name, L_NO_2[ L_RACE_OUSTERS ].Kill));
                        elseif L_NO_3[ self.Race ].Kill < self.Kill then
                            L_NO_3[ self.Race ].Kill = self.Kill;
                            L_NO_3[ self.Race ].Info = self.Info;
                            L_NO_3[ self.Race ].Name = self.Name;    
                            N_Msg(string.format("@<NO_3_V> %s:%d", L_NO_3[ L_RACE_VAMPIRE ].Name, L_NO_3[ L_RACE_VAMPIRE ].Kill),string.format("@<NO_3_S> %s:%d", L_NO_3[ L_RACE_SLAYER ].Name, L_NO_3[ L_RACE_SLAYER ].Kill),string.format("@<NO_3_O> %s:%d", L_NO_3[ L_RACE_OUSTERS ].Name, L_NO_3[ L_RACE_OUSTERS ].Kill));                                        
                        end
                        
                       	--第一次击杀
						if self.KillingTimer == 0 then
							                                                         --送入计时器
							self.KillingTimer = TEMP_KILL_SECOND                     --5秒内连续杀 将启动 功能
							self:StartTimer(L_TIMER_TYPE_KILLING, 1000);
							self.TempKill = 1;
							
						else                                          --已经处在击杀状态中
						
							self.KillingTimer = TEMP_KILL_SECOND     --继续加时间
							self.TempKill = self.TempKill + 1;
							
							if self.ContinueKill < 5 then
								if self.TempKill == 2 then
									local strMsg = string.format("%s 完成了一次双杀!!!", self.Name)
									DEScriptZone:BroadcastSystemMessageInZone(8, strMsg)
								end
								
								if self.TempKill == 3 then
									local strMsg = string.format("%s 完成了一次三杀!!!", self.Name)
									DEScriptZone:BroadcastSystemMessageInZone(8, strMsg)
								end								
							end
						end
						
						
						if self.ContinueKill == 6 then
							local strMsg = string.format("%s 已经妖怪似的杀戮!!!", self.Name)
							DEScriptZone:BroadcastSystemMessageInZone(8, strMsg)
						end

						if self.ContinueKill == 10 then
							local strMsg = string.format("%s 已经超越神似的杀戮!!!拜托谁来杀了他吧!!!!!", self.Name)
							DEScriptZone:BroadcastSystemMessageInZone(8, strMsg)
						end
						
						
						if Dead == nil then
							local strMsg = string.format("%s%s   杀死了  怪物!!!!!!",TEMP_STRING, self.Name)
							DEScriptZone:BroadcastSystemMessageInZone(0, strMsg)
						else
							local strMsg = string.format("%s%s   杀死了  %s!!!!!!", TEMP_STRING,self.Name, Dead.Name)
							DEScriptZone:BroadcastSystemMessageInZone(0, strMsg)
						end

                   end,
                   
					
                   
                   OnSay = function(self, Message)
                   
					if Message == "show" then
						DEScriptZone:BroadcastGCSay( self.Info, string.format("self.Kill:%d, self.ContinueKill:%d, self.TempKill:%d, self.Dead:%d", self.Kill, self.ContinueKill, self.TempKill, self.Dead), 16777215);
					end
					
					if Message == "show race" then
						DEScriptZone:BroadcastGCSay( self.Info, string.format("self.Race:%d", self.Race), 16777215);
					end
					
					if Message == "bifen" then
					    DEScriptZone:BroadcastGCSay( self.Info, string.format("当前比分  鬼:%d分, 人:%d分, 魔:%d分", L_SCORE[ L_RACE_VAMPIRE ], L_SCORE[ L_RACE_SLAYER ], L_SCORE[ L_RACE_OUSTERS ]), 16777215);
					end
					
					if Message == "0" then
                       DEScriptZone:SetPKZone(false);
                       SysMsg(4, string.format("DEScriptZone:SetPKZone(false);") )	
					end


					if Message == "1" then
                       DEScriptZone:SetPKZone(true);
                       SysMsg(4, string.format("DEScriptZone:SetPKZone(true);") )	
					end
					
					if Message == "2" then
                        					
					end					
	                   
	         
					if Message == "3" then
                        SysMsg(8, string.format("3333333333333333333333333333333") )					
					end	
					
							                   
					if Message == "4" then
                        SysMsg(8, string.format("44444444444444444444444444444444") )					
					end
					
					if Message == "5" then
					    local opentime =  DEScript:WarOpenTime();
					    SysMsg(4, string.format("当前时间:%d年%d月%d日%d时%d分%d秒", opentime.year, opentime.month, opentime.day, opentime.hour, opentime.min, opentime.sec) ) 
					end		
						                   
                   end,
                   
                   
                   IsSlayer = function(self)
					   if DEScript:IsSlayer( self.Info ) == true then
					       return true
					   end
					   return false
                   end,
                   
                   
                   IsVampire = function(self)
                   	   if DEScript:IsVampire( self.Info ) == true then
					       return true
					   end
					   return false
                   end,
                   
                   
                   IsOusters = function(self)
                   	   if DEScript:IsOusters( self.Info ) == true then
					       return true
					   end
					   return false                   
                   end,
                   
                   
                   OnTimer = function(self, TimerID)
                   
                   DEScriptZone:BroadcastSystemMessageInZone(4, string.format("%s Time Type=%d", self.Name, self.TimerType ))
                   
						if self.IsDead == true then
						    DEScriptZone:KillTimer( TimerID );   --死亡之后 就取消计时
						    return;
						end
						
						if self.TimerType == L_TIMER_TYPE_KILLING then --击杀计时
						
							if  self.KillingTimer == 0 then
									DEScriptZone:KillTimer( TimerID )
									self.TempKill = 0
									local strMsg = string.format("%s的killing已经结束", self.Name)
									DEScriptZone:BroadcastSystemMessageInZone(4, strMsg)
							end
							  
							self.KillingTimer = self.KillingTimer - 1
							if self.KillingTimer < 0 then
								self.KillingTimer = 0
								return
							end
							
						elseif self.TimerType == L_TIMER_TYPE_RELIVE then --复活无敌计时器
						
						DEScriptZone:BroadcastSystemMessageInZone(4, "self.TimerType == L_TIMER_TYPE_RELIVE")
							DEScriptZone:KillTimer( TimerID );
							self:removeFlag();
						
						end
						
                   
                   end,
                   
                   
                   Info = nil,
                   ID = 0,
                   Race = l_RACE_NULL,
				   Name = "",
				   Kill = 0,
				   ContinueKill = 0,
				   TempKill = 0,
				   Dead = 0,
				   IsDead = false,
				   TimerType = 0,
				   KillingTimer = 0
				}
				
				


function GameOver()

   --计算每个种族的前三名
	
end

function OnTraverseCreature( CreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseCreature")
end

function OnInitializeVariables( ZoneCreateInfo )

	ZoneCreateInfo.Name = "War"	
	ZoneCreateInfo.TemplateZoneID = 1622
	ZoneCreateInfo.StartPosX = 26
	ZoneCreateInfo.StartPosY = 28
	ZoneCreateInfo.EntranceType = "indivisual"
	ZoneCreateInfo.StaticZone = true	
	
	L_SCORE[ L_RACE_VAMPIRE ] = 0;
	L_SCORE[ L_RACE_SLAYER ] = 0;
	L_SCORE[ L_RACE_OUSTERS ] = 0;
	L_SCORE[ l_RACE_NULL ] = 0;
	
	L_RACE_LIST[L_RACE_VAMPIRE] = {};
	L_RACE_LIST[L_RACE_SLAYER] = {};
	L_RACE_LIST[L_RACE_OUSTERS] = {};
	
	L_NO_1[L_RACE_VAMPIRE] = {Info = nil, Name = "", Kill = 0};
	L_NO_1[L_RACE_SLAYER] = {Info = nil, Name = "", Kill = 0};
	L_NO_1[L_RACE_OUSTERS] = {Info = nil, Name = "", Kill = 0};
	
	L_NO_2[L_RACE_VAMPIRE] = {Info = nil, Name = "", Kill = 0};
	L_NO_2[L_RACE_SLAYER] = {Info = nil, Name = "", Kill = 0};
	L_NO_2[L_RACE_OUSTERS] = {Info = nil, Name = "", Kill = 0};
	
	L_NO_3[L_RACE_VAMPIRE] = {Info = nil, Name = "", Kill = 0};
	L_NO_3[L_RACE_SLAYER] = {Info = nil,Name = "", Kill = 0};
	L_NO_3[L_RACE_OUSTERS] = {Info = nil, Name = "", Kill = 0};
	
end 


function OnTraverseMonster( MonsterInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnTraverseMonster")
end


function OnInitialize()
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnInitialize")
	L_CanEnter = false;
	DEScriptZone:SetTimer(3, 1000 , 1);
end

function OnAttackCreature( AttackerCreatureInfo, DefenderCreatureInfo )
end


function OnKillCreature( AttackerCreatureInfo, DeadCreatureInfo )
	
	local AttID = AttackerCreatureInfo.ObjectID
	local DeaID = DeadCreatureInfo.ObjectID

	
	local Attacker = L_PLAYER_ID_LIST[ AttID ];
	local Deader = L_PLAYER_ID_LIST[ DeaID ];
	
	if Attacker.Race == Deader.Race then
		DebugMsg(nil, "Attacker.Race == Deader.Race");
	    return;
	end
	
	if Attacker ~= nil then
	    Attacker:OnKillPlayer(Deader);
	end
	
	if Deader ~= nil then
		Deader:OnBeKill(Attacker);
	end
end

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


function TransPortBack()

	for i,Player in pairs(L_PLAYER_ID_LIST) do
			OnWarpZone(Player.Info);
    end
    
end

local FinalMinTime = 45;
local WarFightTime = 59

function StartWar()
	L_WarStartTime = DEScript:GetCurrentTime();
	L_bClearAllData = false;
	DEScriptZone:SetTimer(1,L_WarFightingTime * 1000 , 1);
	DEScriptZone:SetPKZone( true );
end

function WarOver()
	TransPortBack();											--所有玩家传送出去
	ClearAllData();
end

function GetMaxScoreRace()
	if L_SCORE[ L_RACE_VAMPIRE ] + L_SCORE[ L_RACE_SLAYER ] > L_SCORE[ L_RACE_VAMPIRE ] + L_SCORE[ L_RACE_OUSTERS ] then 
		if L_SCORE[ L_RACE_VAMPIRE ] - L_SCORE[ L_RACE_SLAYER ] > 0 then
			return L_RACE_VAMPIRE;
		else
			return L_RACE_SLAYER;
		end
	else
		if L_SCORE[ L_RACE_VAMPIRE ] - L_SCORE[ L_RACE_OUSTERS ] > 0 then
			return L_RACE_VAMPIRE;
		else
			return L_RACE_OUSTERS;
		end	
	end
end

function GetMVPRace()
	if L_NO_1[ L_RACE_VAMPIRE ].Kill + L_NO_1[ L_RACE_SLAYER ].Kill > L_NO_1[ L_RACE_VAMPIRE ].Kill + L_NO_1[ L_RACE_OUSTERS ].Kill then 
		if L_NO_1[ L_RACE_VAMPIRE ].Kill - L_NO_1[ L_RACE_SLAYER ].Kill > 0 then
			return L_RACE_VAMPIRE;
		else
			return L_RACE_SLAYER;
		end
	else
		if L_NO_1[ L_RACE_VAMPIRE ].Kill - L_NO_1[ L_RACE_OUSTERS ].Kill > 0 then
			return L_RACE_VAMPIRE;
		else
			return L_RACE_OUSTERS;
		end	
	end
end

function GiveThePrice()  --颁发奖励
	--分数最高的一族 每人给10本书的奖励
	
	if L_SCORE[ L_RACE_VAMPIRE ] ~= 0 or L_SCORE[ L_RACE_SLAYER ] ~= 0 or L_SCORE[ L_RACE_OUSTERS ] ~= 0 then
		local MaxScoreRace = GetMaxScoreRace();
		
		if MaxScoreRace == L_RACE_VAMPIRE then
			SysMsg(8,"鬼族 获得了最终的胜利，勇士们将每人获得 10 本烧书" )
		end
		
		if MaxScoreRace == L_RACE_SLAYER then
			SysMsg(8,"人类 获得了最终的胜利，勇士们将每人获得 10 本烧书" )
		end
		
		if MaxScoreRace == L_RACE_OUSTERS then
			SysMsg(8,"魔族 获得了最终的胜利，勇士们将每人获得 10 本烧书" )
		end		
		
		for i,Player in pairs(L_RACE_LIST[ MaxScoreRace ]) do
				GiveItem( Player.Info, 10 );
		end	
	end

    
    --每族排行前三 奖励10本烧书
    SysMsg(8,"每族排名前三者 奖励10本烧书" )
    GiveItem( L_NO_1[L_RACE_VAMPIRE].Info, 10 );
    GiveItem( L_NO_1[L_RACE_SLAYER].Info, 10 );
    GiveItem( L_NO_1[L_RACE_OUSTERS].Info, 10 );
	 
    GiveItem( L_NO_2[L_RACE_VAMPIRE].Info, 10 );
    GiveItem( L_NO_2[L_RACE_SLAYER].Info , 10 );
    GiveItem( L_NO_2[L_RACE_OUSTERS].Info, 10 );
    
    GiveItem( L_NO_3[L_RACE_VAMPIRE].Info, 10 );
    GiveItem( L_NO_3[L_RACE_SLAYER].Info , 10 );
    GiveItem( L_NO_3[L_RACE_OUSTERS].Info, 10 );    	

    --三族中最高的一人 为MVP 奖励20本书
    local MVPRace = GetMVPRace();
    if L_NO_1[ MVPRace ].Info == nil then
		return;
    end
    
    SysMsg(8,string.format("%s获得了本次战场的MVP！！！将获得20本烧书", L_NO_1[ MVPRace ].Info.Name) )
    GiveItem( L_NO_1[L_RACE_OUSTERS].Info, 20 ); 
	
end

function WarMainLoop()

	local time =  DEScript:WarOpenTime();
	--SysMsg(4, string.format("当前时间:%d年%d月%d日%d时%d分%d秒", opentime.year, opentime.month, opentime.day, opentime.hour, opentime.min, opentime.sec) ) 
	
	local NowTime = time.sec;
	--DEScriptZone:BroadcastSystemMessage(0, string.format("战场%d分%d秒", time.min, time.sec));
	
	--if time.hour == 24 then
		if NowTime == FinalMinTime - 10 then
			DEScriptZone:BroadcastSystemMessage(0, "战场还有10分钟，即将开启！");
		end
		
		if NowTime == FinalMinTime - 5 then
			DEScriptZone:BroadcastSystemMessage(0, "战场还有5分钟，即将开启！");
		end
		
		if NowTime == FinalMinTime then
			L_CanEnter = true;
			DEScriptZone:BroadcastSystemMessage(0, "战场开启，5秒钟后禁止进入！");
			StartWar();			
		end
		
		if NowTime == FinalMinTime + 5 then
			DEScriptZone:BroadcastSystemMessage(0, "战场已经关闭！");
			L_CanEnter = false;
		end
		
	--end
	

end


function OnTimer( TimerID )

	if TimerID == 1 then --战场结束 10秒后传送出战场
		DEScriptZone:KillTimer( 1 );
		DEScriptZone:SetPKZone(false); --不可以PK
		GiveThePrice(); --给奖励
		DEScriptZone:SetTimer(2, 5000 , 1); --5秒时间
		--SysMsg(8, string.format("战场结束，奖励已派发，5秒后所有玩家自动传送出副本") )
		SendAllRemainTime(5 * 10, "离开战场剩余时间:")
		
	end
	
	if TimerID == 2 then
		DEScriptZone:KillTimer( 2 );
		WarOver();
	end
	
	
	if TimerID == 3 then  --开启新的一轮比赛
		WarMainLoop()
	end
	
	if TimerID > 10 then
	    local Player = L_PLAYER_ID_LIST[ TimerID ];
	    
	    if Player ~= nil then
			Player:OnTimer(TimerID);
	    end
	    
	end
	
end


function OnBeforeEnterZone( CreatureInfo )

    if L_CanEnter == false then
		DEScriptZone:SendSystemMessage(CreatureInfo, 0, "战场已经关闭，请在每天 xx 点前进入战场!" )
		return false;
	end
	
	return true
end

function OnAfterEnterZone( CreatureInfo )
	
	local EnterID = CreatureInfo.ObjectID;
	local Name = CreatureInfo.Name;
	
	local SavePlayer = nil;
	
	if L_PLAYER_NAME_LIST[ Name ] ~= nil then
		SavePlayer = L_PLAYER_NAME_LIST[ Name ];
	end
	
	if SavePlayer ~= nil then --玩家已经存在列表里了说明是死亡复活后的玩家
	
		SavePlayer:ResetInfo( CreatureInfo );
		L_PLAYER_ID_LIST[ EnterID ] = SavePlayer;
		L_PLAYER_ID_LIST[ EnterID ]:setFlag(); --复活 10秒无敌
			
	else                                      --新加入的玩家
	
		local NewPlayer = CPlayer:New(CreatureInfo);
					       

		if DEScript:IsSlayer( CreatureInfo ) == true then
		
			NewPlayer:SetRace( L_RACE_SLAYER );
			L_RACE_LIST[ L_RACE_SLAYER ][ CreatureInfo.ObjectID ] = NewPlayer ;
			
		elseif DEScript:IsVampire( CreatureInfo ) == true then
		
		    NewPlayer:SetRace( L_RACE_VAMPIRE );
			L_RACE_LIST[ L_RACE_VAMPIRE ][ CreatureInfo.ObjectID ] = NewPlayer ;
			
		elseif DEScript:IsOusters( CreatureInfo ) == true then
		
		    NewPlayer:SetRace( L_RACE_OUSTERS );
			L_RACE_LIST[ L_RACE_OUSTERS ][ CreatureInfo.ObjectID ] = NewPlayer ;
		end
		
		SysMsg(4, string.format("@ShowBord") );
		SysMsg(4, string.format("@<Score> 鬼:%d   人:%d   魔:%d", L_SCORE[ L_RACE_VAMPIRE ], L_SCORE[ L_RACE_SLAYER ], L_SCORE[ L_RACE_OUSTERS ]) )		
		
		--SysMsg(4, string.format("@<Ranking3> %s:%d   %s:%d   %s:%d", L_NO_3[ L_RACE_VAMPIRE ].Name, L_NO_3[ L_RACE_VAMPIRE ].Kill, L_NO_3[ L_RACE_SLAYER ].Name, L_NO_3[ L_RACE_SLAYER ].Kill, L_NO_3[ L_RACE_OUSTERS ].Name, L_NO_3[ L_RACE_OUSTERS ].Kill) )
		--SysMsg(4, string.format("@<Ranking2> %s:%d   %s:%d   %s:%d", L_NO_2[ L_RACE_VAMPIRE ].Name, L_NO_2[ L_RACE_VAMPIRE ].Kill, L_NO_2[ L_RACE_SLAYER ].Name, L_NO_2[ L_RACE_SLAYER ].Kill, L_NO_2[ L_RACE_OUSTERS ].Name, L_NO_2[ L_RACE_OUSTERS ].Kill) )
		--SysMsg(4, string.format("@<Ranking1> %s:%d   %s:%d   %s:%d", L_NO_1[ L_RACE_VAMPIRE ].Name, L_NO_1[ L_RACE_VAMPIRE ].Kill, L_NO_1[ L_RACE_SLAYER ].Name, L_NO_1[ L_RACE_SLAYER ].Kill, L_NO_1[ L_RACE_OUSTERS ].Name, L_NO_1[ L_RACE_OUSTERS ].Kill) )		

		N_Msg(string.format("@<NO_3_V> %s:%d", L_NO_3[ L_RACE_VAMPIRE ].Name, L_NO_3[ L_RACE_VAMPIRE ].Kill),string.format("@<NO_3_S> %s:%d", L_NO_3[ L_RACE_SLAYER ].Name, L_NO_3[ L_RACE_SLAYER ].Kill),string.format("@<NO_3_O> %s:%d", L_NO_3[ L_RACE_OUSTERS ].Name, L_NO_3[ L_RACE_OUSTERS ].Kill));   
		N_Msg(string.format("@<NO_2_V> %s:%d", L_NO_2[ L_RACE_VAMPIRE ].Name, L_NO_2[ L_RACE_VAMPIRE ].Kill),string.format("@<NO_2_S> %s:%d", L_NO_2[ L_RACE_SLAYER ].Name, L_NO_2[ L_RACE_SLAYER ].Kill),string.format("@<NO_2_O> %s:%d", L_NO_2[ L_RACE_OUSTERS ].Name, L_NO_2[ L_RACE_OUSTERS ].Kill));
		N_Msg(string.format("@<NO_1_V> %s:%d", L_NO_1[ L_RACE_VAMPIRE ].Name, L_NO_1[ L_RACE_VAMPIRE ].Kill),string.format("@<NO_1_S> %s:%d", L_NO_1[ L_RACE_SLAYER ].Name, L_NO_1[ L_RACE_SLAYER ].Kill),string.format("@<NO_1_O> %s:%d", L_NO_1[ L_RACE_OUSTERS ].Name, L_NO_1[ L_RACE_OUSTERS ].Kill));
     
		L_PLAYER_ID_LIST[ CreatureInfo.ObjectID ] = NewPlayer 
		L_PLAYER_NAME_LIST[ Name ] = NewPlayer
		NewPlayer:AddEffect(231);
		
		local Msg = string.format("%s 刚加入了战争！！！", NewPlayer.Name)
		DEScriptZone:BroadcastSystemMessageInZone(4, Msg)
		
		local WarRemainTime = L_WarFightingTime - ( DEScript:GetCurrentTime() - L_WarStartTime )
		SendRemainTime(NewPlayer.Info, WarRemainTime * 10, "战场剩余时间:")
		
	end
	
	

		
	--DEScriptZone:SendSystemMessage(CreatureInfo, 7, "战场剩余时间[100]");
	
end


function OnBeforeLeaveZone( CreatureInfo )
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnBeforeLeaveZone")
end

function OnAfterLeaveZone( CreatureInfo )
    DEScriptZone:BroadcastSystemMessageInZone(4, "OnAfterLeaveZone")
    
    if L_bClearAllData == true then
		return;
	end
    
    local ID = CreatureInfo.ObjectID;
    local Name = CreatureInfo.Name;
    
    if L_PLAYER_ID_LIST == nil then
		return;
    end
    
    if L_PLAYER_ID_LIST[ ID ].IsDead == false then --当前不是死亡状态 且执行到这里 说明玩家是主动离开，那么删除保存的数据
        local Player = L_PLAYER_NAME_LIST[ Name ];
        Player:RemoveEffect(231);
        DebugMsg(nil, string.format("%s 离开了战场", CreatureInfo.Name) );
        
	    L_RACE_LIST[L_RACE_VAMPIRE][ Player.Race ] = nil;
		L_RACE_LIST[L_RACE_SLAYER][ Player.Race ] = nil;
		L_RACE_LIST[L_RACE_OUSTERS][ Player.Race ] = nil;        
	    L_PLAYER_NAME_LIST[ Name ] = nil;
    end
    
    DebugMsg(nil, string.format("%s 死亡传送", CreatureInfo.Name) );
    
    L_PLAYER_ID_LIST[ ID ] = nil; 
    
end

function OnAddMonster(Monster)
	DEScriptZone:BroadcastSystemMessageInZone(4, "OnAddMonster")	
	return true
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


function OnSay( CreatureInfo, Mesg )
	
	local ID = CreatureInfo.ObjectID;
	
	local Player = L_PLAYER_ID_LIST[ ID ];
	
	if Player ~= nil then
	    Player:OnSay(Mesg);
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
