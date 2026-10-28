dofile("../script/zone/Item/item.lua")
function OnBeforeEnterZone(Creature)
	
	HaveItemInfo = 
	{
		ItemClass = 91,
		ItemType = 63,
		ItemCount = 300
	}
	
	if DEScriptZone:HaveItem(Creature, HaveItemInfo) then
		TakeItemInfo = {}
		TakeItemInfo = HaveItemInfo

		DEScriptZone:TakeItem(Creature, TakeItemInfo)
		GiveItem(Creature,97,154,1)
		DEScriptZone:SendSystemMessage(Creature, 0, "兑换成功！.")
		return false
	end
	DEScriptZone:SendSystemMessage(Creature, 0, "您没有足够的烧书！")

	return false	
end