modded class ItemBase
{
	protected bool m_GPDA_XPProcessed;
	
	bool GPDA_IsXPProcessed()
	{
		return m_GPDA_XPProcessed;
	}
	
	void GPDA_MarkXPProcessed()
	{
		m_GPDA_XPProcessed = true;
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		if (!GetGame().IsServer()) return;
		if (!g_GPDA_RankManager) return;
		if (!new_owner) return;

		PlayerBase player = PlayerBase.Cast(new_owner.GetHierarchyRootPlayer());
		if (!player || !player.GetIdentity()) return;

		HumanInventory hi = player.GetHumanInventory();
		if (hi && hi.GetEntityInHands() == this) return;

		PlayerBase oldRoot;
		if (old_owner)
			oldRoot = PlayerBase.Cast(old_owner.GetHierarchyRootPlayer());
		if (oldRoot == player) return;
		if (GPDA_IsXPProcessed()) return;

		string uid = player.GetIdentity().GetPlainId();
		int low, high;
		GetNetworkID(low, high);
		string key = "I|" + uid + "|" + low.ToString() + "|" + high.ToString();
		if (g_GPDA_RankManager.IsItemProcessed(key)) return;

		int xp = g_GPDA_RankManager.GetItemXPInventory(GetType());
		if (xp != 0)
		{
			g_GPDA_RankManager.AddXP(uid, xp);
			g_GPDA_RankManager.SendXPNotification(player, xp, "Item Inventory");
			g_GPDA_RankManager.MarkItemProcessed(key);
			GPDA_MarkXPProcessed();
		}
	}
}
