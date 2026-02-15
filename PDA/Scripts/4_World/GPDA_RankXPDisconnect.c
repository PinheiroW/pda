class GPDA_RankXPDisconnectHelper
{
	static void Flush()
	{
		if (!GetGame() || !GetGame().IsServer()) return;
		if (g_GPDA_RankManager)
			g_GPDA_RankManager.FlushPlayerXPSave();
	}
}
