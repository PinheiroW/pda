// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - PDA Item Script Class & Card Slot Helper
// Registers the open action and provides flash card slot checking
// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
// Helper class to track which PDA is currently active and check its card slots
// ─────────────────────────────────────────────────────────────────────────────
class GPDA_PDAItemHelper
{
	protected static ItemBase s_ActivePDA;
	
	// Card slot mapping:
	// Slot A = GRESHNIK_Card_A = Map card
	// Slot B = GRESHNIK_Card_B = Capture card
	// Slot C = GRESHNIK_Card_C = Groups card
	// Slot D = GRESHNIK_Card_D = SOS/Support card
	// Slot E = GRESHNIK_Card_E = GPS card
	// Slot F = GRESHNIK_Card_F = Database card
	
	static void SetActivePDA(ItemBase pda)
	{
		s_ActivePDA = pda;
		if (pda)
			Print("[GPDA] Active PDA set");
		else
			Print("[GPDA] Active PDA cleared");
	}
	
	static ItemBase GetActivePDA()
	{
		return s_ActivePDA;
	}
	
	// Check if a specific card slot has a card inserted
	static bool HasCard(string slotName)
	{
		if (!s_ActivePDA) return false;
		
		int slotId = InventorySlots.GetSlotIdFromString(slotName);
		if (slotId == InventorySlots.INVALID)
		{
			Print("[GPDA] WARNING: Invalid slot name: " + slotName);
			return false;
		}
		
		EntityAI attachment = s_ActivePDA.GetInventory().FindAttachment(slotId);
		if (attachment)
		{
			Print("[GPDA] Card found in slot " + slotName + ": " + attachment.GetType());
			return true;
		}
		
		Print("[GPDA] No card in slot " + slotName);
		return false;
	}
	
	// Check specific cards by category
	static bool HasMapCard()
	{
		return HasCard("A");
	}
	
	static bool HasCaptureCard()
	{
		return HasCard("B");
	}
	
	static bool HasGroupsCard()
	{
		return HasCard("C");
	}
	
	static bool HasSupportCard()
	{
		return HasCard("D");
	}
	
	static bool HasGPSCard()
	{
		return HasCard("E");
	}
	
	static bool HasDatabaseCard()
	{
		return HasCard("F");
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// PDA Item Base class - registers the open action
// ─────────────────────────────────────────────────────────────────────────────
class GRESHNIK_PDA_Base: ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionOpenGPDA);
	}
	
	// Проверка работоспособности (батарейка есть и не сломан)
	bool IsWorking()
	{
		if (IsRuined())
			return false;
			
		if (!HasEnergyManager())
			return false;
			
		return GetCompEM().CanWork();
	}
}
