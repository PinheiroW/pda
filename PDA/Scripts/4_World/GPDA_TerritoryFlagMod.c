// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Modded TerritoryFlag
// Vanilla reference: P:\scripts\4_world\entities\itembase\basebuildingbase\totem.c
// Adds ActionOpenCaptureMenu to vanilla TerritoryFlag
// Flag attachment: TAB drag allowed for Flag_Base, server validates group on capture
// Flag removal: blocked on client (only capture system can replace)
// ═══════════════════════════════════════════════════════════════════════════════

modded class TerritoryFlag
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpenCaptureMenu);
	}
	
	// Allow Flag_Base attachment on all contexts (client TAB + server sync)
	// Vanilla ref: P:\scripts\4_world\entities\itembase\basebuildingbase\totem.c:294
	// Server validates group FlagClass on capture start
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (attachment && attachment.IsKindOf("Flag_Base"))
			return super.CanReceiveAttachment(attachment, slotId);
		
		return false;
	}
	
	// Block flag removal on client — only capture system can replace flags
	// Vanilla ref: totem.c — EEItemDetached resets flag if removed while raised
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		// Server can always release (for capture system logic)
		if (GetGame().IsDedicatedServer())
			return super.CanReleaseAttachment(attachment);
		
		// Block client-side removal (TAB drag out)
		if (attachment && attachment.IsKindOf("Flag_Base"))
			return false;
		
		return super.CanReleaseAttachment(attachment);
	}
	
	// Server-side: when flag is attached via TAB, validate player's group
	// If wrong flag → remove and return to player
	// Vanilla ref: totem.c EEItemAttached
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		if (!GetGame().IsDedicatedServer()) return;
		if (slot_name != "Material_FPole_Flag") return;
		if (!item || !item.IsKindOf("Flag_Base")) return;
		
		// Find the nearest player who likely attached it (within 3m)
		string flagType = item.GetType();
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		PlayerBase nearestPlayer = null;
		float nearestDist = 5.0;
		
		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb || !pb.GetIdentity() || !pb.IsAlive()) continue;
			
			float dist = vector.Distance(pb.GetPosition(), GetPosition());
			if (dist < nearestDist)
			{
				nearestDist = dist;
				nearestPlayer = pb;
			}
		}
		
		if (!nearestPlayer || !nearestPlayer.GetIdentity())
		{
			Print("[GPDA][Capture] Flag attached but no nearby player found - allowing");
			return;
		}
		
		string uid = nearestPlayer.GetIdentity().GetPlainId();
		
		// Check player group
		if (!g_GPDA_GroupManager)
		{
			Print("[GPDA][Capture] GroupManager not available - allowing");
			return;
		}
		
		GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(uid);
		if (!group)
		{
			// Not in a group — remove flag
			Print("[GPDA][Capture] Player " + uid + " not in any group - rejecting flag");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RejectFlag, 100, false, item, nearestPlayer, "You must be in a group to attach a flag!");
			return;
		}
		
		// Check FlagClass match
		if (group.FlagClass != "" && group.FlagClass != flagType)
		{
			Print("[GPDA][Capture] Wrong flag! Player group=" + group.Name + " needs=" + group.FlagClass + " got=" + flagType);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RejectFlag, 100, false, item, nearestPlayer, "Wrong flag! Your group uses: " + group.FlagClass);
			return;
		}
		
		Print("[GPDA][Capture] Flag " + flagType + " attached by " + group.Name + " member - OK");
	}
	
	// Deferred rejection: delete flag, recreate in player inventory
	void RejectFlag(EntityAI flag, PlayerBase player, string reason)
	{
		if (!flag || !player) return;
		
		// Store flag type before deleting
		string flagType = flag.GetType();
		
		// Unlock slot
		int flagSlotId = InventorySlots.GetSlotIdFromString("Material_FPole_Flag");
		GetInventory().SetSlotLock(flagSlotId, false);
		
		// Delete the stuck flag (server authority bypasses client CanReleaseAttachment)
		GetGame().ObjectDelete(flag);
		Print("[GPDA][Capture] Deleted stuck flag: " + flagType);
		
		// Create new flag in player inventory or drop
		EntityAI newFlag = player.GetInventory().CreateInInventory(flagType);
		if (newFlag)
		{
			Print("[GPDA][Capture] Created " + flagType + " in player inventory");
		}
		else
		{
			// No space — drop on ground
			vector groundPos = player.GetPosition();
			newFlag = GetGame().CreateObject(flagType, groundPos, false, false, false);
			Print("[GPDA][Capture] No space — " + flagType + " dropped at " + groundPos.ToString());
		}
		
		// Notify player
		NotificationSystem.SendNotificationToPlayerIdentityExtended(
			player.GetIdentity(), 5.0, "Flag Rejected", reason, "set:dayz_gui image:icon_x");
	}
}
