Mission_Monster = 
{
	{
		MonsterInfo = Monster_1613,
		Name = "门卫1号",
		X = 34,
		y = 215,
		KillCallBack = OnGuard_0_Kill
	},
	{
		MonsterInfo = Monster_1613,
		Name = "门卫2号",
		X = 38,
		y = 220,
		KillCallBack = OnGuard_0_Kill
	},
	{
		MonsterInfo = Monster_1613,
		Name = "一阶段上面BOSS",
		X = 66,
		y = 176,
		KillCallBack = OnBoss_Kill
	},
	{
		MonsterInfo = Monster_1613,
		Name = "一阶段下面BOSS",
		X = 73,
		y = 181,
		KillCallBack = OnBoss_Kill
	},
	{
		MonsterInfo = Monster_1613,
		Name = "一阶段左门守卫",
		X = 73,
		y = 169,
		KillCallBack = OnGuard_1_Kill
	},
	{
		MonsterInfo = Monster_1613,
		Name = "一阶段右门守卫",
		X = 81,
		y = 178,
		KillCallBack = OnGuard_1_Kill
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段绿色",
		X = 78,
		y = 140,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段蓝色",
		X = 85,
		y = 151,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段红色上",
		X = 92,
		y = 148,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段绿色",
		X = 78,
		y = 140,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段红色下",
		X = 101,
		y = 160,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段蓝色下",
		X = 99,
		y = 168,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子绿上",
		X = 135,
		y = 101,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子绿下",
		X = 147,
		y = 115,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子蓝上",
		X = 147,
		y = 92,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子蓝下",
		X = 156,
		y = 101,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子红上",
		X = 155,
		y = 84,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子红下",
		X = 163,
		y = 94,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子大红上",
		X = 149,
		y = 66,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子大红下",
		X = 179,
		y = 102,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子杂色上",
		X = 172,
		y = 66,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "二阶段双子杂色下",
		X = 180,
		y = 77,
		KillCallBack = OnTwinsDead
	},
	{
		MonsterInfo = Monster_1613,
		Name = "三阶段上大怪",
		X = 176,
		y = 44,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "三阶段上小怪",
		X = 185,
		y = 44,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "三阶段下大怪",
		X = 199,
		y = 73,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "三阶段下小怪",
		X = 200,
		y = 60,
		KillCallBack = nil
	},
	{
		MonsterInfo = Monster_1613,
		Name = "三阶段最终BOSS",
		X = 206,
		y = 39,
		KillCallBack = nil
	}
}
Statue_Monster = --雕像怪
{
	{
		MonsterInfo = Monster_1613,
		MonsterPos = 
		{
			{34,215},--门卫1号
		    {38,220}, --门卫2号
			
			{73,169},-- 一阶段左门守卫
		    {81,178}, -- 一阶段右门守卫
			
			{78,140},-- 二阶段绿色
		    {85,151}, -- 二阶段蓝色
			{92,148},--  二阶段红色上
		    {101,160}, -- 二阶段红色下 
			{99,168}, -- 二阶段蓝色下
			
			{135,101},-- 二阶段双子绿上
		    {147,115}, --二阶段双子绿下
			
			{147,92},--二阶段双子蓝上
		    {156,101}, --二阶段双子蓝下
			
		    {155,84},--二阶段双子红上
			{163,94},--二阶段双子红下
			
		    {149,66}, --二阶段双子大红上
			{179,102},--二阶段双子大绿下
			
		    {172,66}, --二阶段双子杂色上
		    {180,77}, --二阶段双子杂色下
			
		    {176,44}, --三阶段上大怪
			{185,44},--三阶段上小怪
		    {199,73}, --三阶段下大怪
			{200,60},--三阶段下小怪
		    {206,39}, --三阶段最终BOSS
		}
	},
	{
		MonsterInfo = FirstBoss,
		MonsterPos = 
		{
		    {66,176},-- 一阶段上面BOSS
		    {73,181} -- 一阶段下面BOSS
		}
	}
}