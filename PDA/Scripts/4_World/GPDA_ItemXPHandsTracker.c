modded class PlayerBase
{
	override void EEItemIntoHands(EntityAI item)
	{
		super.EEItemIntoHands(item);
		if (!GetGame().IsServer()) return;
		if (!g_GPDA_RankManager) return;
		if (!item) return;
		if (!GetIdentity()) return;
		
		ItemBase ib = ItemBase.Cast(item);
		if (ib && ib.GPDA_IsXPProcessed())
			return;
		
		string uid = GetIdentity().GetPlainId();
		int low, high;
		item.GetNetworkID(low, high);
		string key = "I|" + uid + "|" + low.ToString() + "|" + high.ToString();
		if (g_GPDA_RankManager.IsItemProcessed(key)) return;

		int xp = g_GPDA_RankManager.GetItemXPHands(item.GetType());
		if (xp != 0)
		{
			g_GPDA_RankManager.AddXP(uid, xp);
			g_GPDA_RankManager.SendXPNotification(this, xp, "Item Hands");
			g_GPDA_RankManager.MarkItemProcessed(key);
			if (ib)
				ib.GPDA_MarkXPProcessed();
		}
	}
}
