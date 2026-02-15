// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Spawn Handler
// Handles group-based spawn points for players ONLY after death (new character)
// ═══════════════════════════════════════════════════════════════════════════════

// Track players who need group spawn (died and respawning)
class GPDA_SpawnTracker
{
	protected static ref map<string, bool> m_PendingSpawns;
	
	static void Init()
	{
		if (!m_PendingSpawns)
			m_PendingSpawns = new map<string, bool>;
	}
	
	static void MarkForGroupSpawn(string steam64)
	{
		Init();
		m_PendingSpawns.Set(steam64, true);
		Print("[GPDA] Player marked for group spawn: " + steam64);
	}
	
	static bool NeedsGroupSpawn(string steam64)
	{
		Init();
		return m_PendingSpawns.Contains(steam64);
	}
	
	static void ClearGroupSpawn(string steam64)
	{
		Init();
		m_PendingSpawns.Remove(steam64);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// PLAYER BASE EXTENSION - Track death for respawn
// ═══════════════════════════════════════════════════════════════════════════════

modded class PlayerBase
{
	// Called when player dies - mark for group spawn
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		
		if (GetGame().IsServer() && GetIdentity())
		{
			string steam64 = GetIdentity().GetPlainId();
			
			// Check if player has group spawn
			if (g_GPDA_GroupManager && g_GPDA_GroupManager.HasGroupSpawn(steam64))
			{
				GPDA_SpawnTracker.MarkForGroupSpawn(steam64);
			}
		}
	}
	
	protected void GPDA_ClearAllInventory()
	{
		if (!GetGame().IsServer())
			return;
		
		EntityAI hands = GetItemInHands();
		if (hands)
			GetGame().ObjectDelete(hands);
		
		array<EntityAI> items = new array<EntityAI>;
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		for (int i = items.Count() - 1; i >= 0; i--)
		{
			EntityAI e = items[i];
			if (!e) continue;
			if (e == this) continue;
			GetGame().ObjectDelete(e);
		}
	}
	
	override void OnPlayerLoaded()
	{
		super.OnPlayerLoaded();
		
		// Check if this player was marked for group spawn (died previously)
		if (GetGame().IsServer() && GetIdentity())
		{
			string steam64 = GetIdentity().GetPlainId();
			
			if (GPDA_SpawnTracker.NeedsGroupSpawn(steam64))
			{
				// Delay teleport to ensure player is fully loaded
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GPDA_DoGroupSpawn, 2000, false);
			}
		}
	}
	
	protected void GPDA_DoGroupSpawn()
	{
		if (!GetIdentity()) return;
		if (!g_GPDA_GroupManager) return;
		if (!IsAlive()) return;
		
		string steam64 = GetIdentity().GetPlainId();
		
		// Clear the spawn flag first
		GPDA_SpawnTracker.ClearGroupSpawn(steam64);
		
		// Get group spawn point
		vector groupSpawn = g_GPDA_GroupManager.GetPlayerSpawnPoint(steam64);
		
		if (groupSpawn != "0 0 0")
		{
			// Get proper Y coordinate (ground level)
			float groundY = GetGame().SurfaceY(groupSpawn[0], groupSpawn[2]);
			groupSpawn[1] = groundY + 0.5;
			
			// Teleport player to group spawn
			SetPosition(groupSpawn);
			
			GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(steam64);
			string groupName = "";
			if (group) groupName = group.Name;
			
			// Apply group loadout (optional)
			if (group && group.Items && group.Items.Count() > 0)
			{
				GPDA_ClearAllInventory();
				
				ref array<string> pending = new array<string>;
				
				// Pass 1: try to equip everything possible (clothes/backpack/vest etc)
				foreach (string itemType : group.Items)
				{
					if (itemType == "")
						continue;
					
					EntityAI att = EntityAI.Cast(GetInventory().CreateAttachment(itemType));
					if (!att)
						pending.Insert(itemType);
				}
				
				// Pass 2: put the rest into inventory (now we should have pockets/containers)
				foreach (string itemType2 : pending)
				{
					EntityAI inv = EntityAI.Cast(GetInventory().CreateInInventory(itemType2));
					if (inv)
						continue;
					
					Print("[GPDA] Loadout item could not be placed in inventory, dropping on ground: " + itemType2);
					vector dropPos = GetPosition();
					dropPos[0] = dropPos[0] + 0.2;
					dropPos[2] = dropPos[2] + 0.2;
					dropPos[1] = GetGame().SurfaceY(dropPos[0], dropPos[2]) + 0.05;
					GetGame().CreateObject(itemType2, dropPos, false, true);
				}
			}
			
			Print("[GPDA] Player " + GetIdentity().GetName() + " respawned at group '" + groupName + "' base: " + groupSpawn.ToString());
		}
	}
}
