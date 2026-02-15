// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Action to open PDA via item interaction (LMB)
// Uses EnterScriptedMenu to avoid cross-module dependency (4_World -> 5_Mission)
// ═══════════════════════════════════════════════════════════════════════════════

class ActionOpenGPDA: ActionSingleUseBase
{
	void ActionOpenGPDA()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_PRONE;
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override string GetText()
	{
		return "Открыть ПДА";
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!item) return false;
		
		// Check via GRESHNIK_PDA_Base.IsWorking() if applicable
		GRESHNIK_PDA_Base pdaModel = GRESHNIK_PDA_Base.Cast(item);
		if (pdaModel)
			return pdaModel.IsWorking();
		
		return true;
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		// Store reference to the PDA item being used so menu can check card slots
		GPDA_PDAItemHelper.SetActivePDA(action_data.m_MainItem);
		
		// Open PDA menu via EnterScriptedMenu (avoids 4_World -> 5_Mission dependency)
		GetGame().GetUIManager().EnterScriptedMenu(MENU_GPDA_MAIN, null);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		// Nothing needed server-side for opening UI
	}
}
