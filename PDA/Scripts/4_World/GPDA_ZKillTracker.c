// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Kill Tracker
// Tracks player kills, zombie kills, animal kills, and deaths
// ═══════════════════════════════════════════════════════════════════════════════

modded class PlayerBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		if (!GetGame().IsServer()) return;
		
		if (GetIdentity())
		{
			GPDA_ServerManager.GetInstance().AddDeath(GetIdentity().GetPlainId(), GetIdentity().GetName());
		}
		
		PlayerBase killerPlayer = PlayerBase.Cast(killer);
		if (!killerPlayer)
		{
			EntityAI killerEntity = EntityAI.Cast(killer);
			if (killerEntity)
			{
				PlayerBase weaponOwner = PlayerBase.Cast(killerEntity.GetHierarchyRootPlayer());
				if (weaponOwner)
				{
					killerPlayer = weaponOwner;
				}
			}
		}
		
		if (killerPlayer && killerPlayer.GetIdentity() && killerPlayer != this)
		{
			string odID = killerPlayer.GetIdentity().GetPlainId();
			string killName = killerPlayer.GetIdentity().GetName();
			int killType = 1;
			GPDA_ServerManager.GetInstance().AddKill(odID, killName, killType);
			
			// Add rank XP for player kill (only if killer is different from victim)
			if (g_GPDA_RankManager)
			{
				int xp = g_GPDA_RankManager.GetKillXP("SurvivorBase");
				if (xp > 0)
				{
					g_GPDA_RankManager.AddXP(odID, xp);
					g_GPDA_RankManager.SendXPNotification(killerPlayer, xp, "Player Kill");
				}
			}
		}
	}
}

modded class ZombieBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		if (!GetGame().IsServer()) return;
		
		PlayerBase killerPlayer = PlayerBase.Cast(killer);
		if (!killerPlayer)
		{
			EntityAI killerEntity = EntityAI.Cast(killer);
			if (killerEntity)
			{
				PlayerBase weaponOwner = PlayerBase.Cast(killerEntity.GetHierarchyRootPlayer());
				if (weaponOwner)
				{
					killerPlayer = weaponOwner;
				}
			}
		}
		
		if (killerPlayer && killerPlayer.GetIdentity())
		{
			string odID = killerPlayer.GetIdentity().GetPlainId();
			string killName = killerPlayer.GetIdentity().GetName();
			int killType = 2;
			GPDA_ServerManager.GetInstance().AddKill(odID, killName, killType);
			
			// Update quest progress
			// REMOVED: Quest Manager not implemented
			
			// Add rank XP
			if (g_GPDA_RankManager)
			{
				int xp = g_GPDA_RankManager.GetKillXP(GetType());
				if (xp > 0)
				{
					g_GPDA_RankManager.AddXP(odID, xp);
				}
			}
		}
	}
}

modded class AnimalBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		if (!GetGame().IsServer()) return;
		
		PlayerBase killerPlayer = PlayerBase.Cast(killer);
		if (!killerPlayer)
		{
			EntityAI killerEntity = EntityAI.Cast(killer);
			if (killerEntity)
			{
				PlayerBase weaponOwner = PlayerBase.Cast(killerEntity.GetHierarchyRootPlayer());
				if (weaponOwner)
				{
					killerPlayer = weaponOwner;
				}
			}
		}
		
		if (killerPlayer && killerPlayer.GetIdentity())
		{
			string odID = killerPlayer.GetIdentity().GetPlainId();
			string killName = killerPlayer.GetIdentity().GetName();
			int killType = 3;
			GPDA_ServerManager.GetInstance().AddKill(odID, killName, killType);
			
			// Update quest progress
			// REMOVED: Quest Manager not implemented
			
			// Add rank XP
			if (g_GPDA_RankManager)
			{
				int xp = g_GPDA_RankManager.GetKillXP(GetType());
				if (xp > 0)
				{
					g_GPDA_RankManager.AddXP(odID, xp);
				}
			}
		}
	}
}
