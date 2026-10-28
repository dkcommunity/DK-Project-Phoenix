dofile("../script/zone/HellCastle/HellCastle_BlockInfo.lua")
dofile("../script/zone/HellCastle/HellCastle_MonsterInfo.lua")
--门
local LeftDoor = nil
local RightDoor = nil
local MidDoor = nil
local LastDoor = nil

local TIMER_LEVEL_ONE = 5
local TIMER_LEVEL_TWO = 6
local TIMER_LEVEL_THREE = 7
local TIMER_LEVEL_FOUR = 8
local TIMER_LEVEL_FIVE = 9

local TeamID = 0
local PlayerCount = 0
local MONSTER_LIST = {}
local PLAYER_LIST = {}

-------------------------------怪物出生数据表格------------------------------------

BornDataTable["门卫1号"] = {Name = "门卫1号", TableID = Monster_1613, nX = 34, nY = 215, KillCallBack = OnGuard_0_Kill}
BornDataTable["门卫2号"] = {Name = "门卫2号", TableID = Monster_1613, nX = 38, nY = 220, KillCallBack = OnGuard_0_Kill}

--第一阶段开始
BornDataTable["一阶段上面BOSS"] = {Name = "一阶段上面BOSS", TableID = Monster_1613, nX = 66, nY = 176, KillCallBack = OnBoss_Kill}
BornDataTable["一阶段下面BOSS"] = {Name = "一阶段下面BOSS", TableID = Monster_1613, nX = 73, nY = 181, KillCallBack = OnBoss_Kill}

BornDataTable["一阶段左门守卫"] = {Name = "一阶段左门守卫", TableID = Monster_1613, nX = 73, nY = 169, KillCallBack = OnGuard_1_Kill}
BornDataTable["一阶段右门守卫"] = {Name = "一阶段右门守卫", TableID = Monster_1613, nX = 81, nY = 178, KillCallBack = OnGuard_1_Kill}
	
--第二阶段开始 顺序是 由上往下	
BornDataTable["二阶段绿色"] =   {Name = "二阶段绿色",   TableID = Monster_1613, nX = 78,	 nY = 140, KillCallBack = nil}
BornDataTable["二阶段蓝色"] =   {Name = "二阶段蓝色",   TableID = Monster_1613, nX = 85,	 nY = 151, KillCallBack = nil}
BornDataTable["二阶段红色上"] = {Name = "二阶段红色上", TableID = Monster_1613, nX = 92,	 nY = 148, KillCallBack = nil}
BornDataTable["二阶段红色下"] = {Name = "二阶段红色下", TableID = Monster_1613, nX = 101,	 nY = 160, KillCallBack = nil}
BornDataTable["二阶段蓝色下"] = {Name = "二阶段蓝色下", TableID = Monster_1613, nX = 99,	 nY = 168, KillCallBack = nil}
	
--下面10个 5组双子怪
BornDataTable["二阶段双子绿上"] =   {Name = "二阶段双子绿上",   TableID = Monster_1613, nX = 135,	 nY = 101, KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子绿下"] =   {Name = "二阶段双子绿下",   TableID = Monster_1613, nX = 147,	 nY = 115, KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子蓝上"] =   {Name = "二阶段双子蓝上",   TableID = Monster_1613, nX = 147,	 nY = 92,  KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子蓝下"] =   {Name = "二阶段双子蓝下",   TableID = Monster_1613, nX = 156,	 nY = 101, KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子红上"] =   {Name = "二阶段双子红上",   TableID = Monster_1613, nX = 155,	 nY = 84,  KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子红下"] =   {Name = "二阶段双子红下",   TableID = Monster_1613, nX = 163,	 nY = 94,  KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子大红上"] = {Name = "二阶段双子大红上", TableID = Monster_1613, nX = 149,	 nY = 66,  KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子大绿下"] = {Name = "二阶段双子大绿下", TableID = Monster_1613, nX = 179,	 nY = 102, KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子杂色上"] = {Name = "二阶段双子杂色上", TableID = Monster_1613, nX = 172,	 nY = 66,  KillCallBack = OnTwinsDead}
BornDataTable["二阶段双子杂色下"] = {Name = "二阶段双子杂色下", TableID = Monster_1613, nX = 180,	 nY = 77,  KillCallBack = OnTwinsDead}
BornDataTable["双子召唤小怪"] =		{Name = "双子召唤小怪",		TableID = Monster_779,  nX = 206,	 nY = 39,   KillCallBack = nil}
	
--第三阶段开始 由上往下
BornDataTable["三阶段上大怪"] =   {Name = "三阶段上大怪",   TableID = Monster_1613, nX = 176,	 nY = 44,   KillCallBack = nil}
BornDataTable["三阶段上小怪"] =   {Name = "三阶段上小怪",   TableID = Monster_1613, nX = 185,	 nY = 44,   KillCallBack = nil}
BornDataTable["三阶段下大怪"] =   {Name = "三阶段下大怪",   TableID = Monster_1613, nX = 199,	 nY = 73,   KillCallBack = nil}
BornDataTable["三阶段下小怪"] =   {Name = "三阶段下小怪",   TableID = Monster_1613, nX = 200,	 nY = 60,   KillCallBack = nil}
BornDataTable["三阶段最终BOSS"] = {Name = "三阶段最终BOSS", TableID = Monster_1613, nX = 206,	 nY = 39,   KillCallBack = OnFinalBossDead}

function SummonMonster()

		MonsterInfo = bornData.TableID
		MonsterInfo.X = bornData.nX
		MonsterInfo.Y = bornData.nY
		MonsterInfo.Name = bornData.Name
		AddedMonster = DEScriptZone:AddMonster(MonsterInfo)	
		table.insert(MONSTER_LIST, {AddedMonster.ObjectID,MonsterInfo.Name})
end

function AddEffectToPlayer(PlayerInfo, ClassID, EffectTime)
	EffectInfo =
	{
		EffectClass = ClassID,
		Duration  = EffectTime
	}
	DEScriptZone:AddEffectToPlayer(PlayerInfo,EffectInfo)	
	return true
end

function RemoveEffectFromPlayer(PlayerInfo, ClassID)
	EffectInfo =
	{
		EffectClass = ClassID
	}
	DEScriptZone:RemoveEffectFromCreature(EffectInfo, PlayerInfo)
	DEScriptZone:BroadcastSystemMessageInZone(4, "RemoveEffectFromPlayer ")
end
