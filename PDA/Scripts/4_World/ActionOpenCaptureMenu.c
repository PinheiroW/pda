// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Action: Open Capture Menu on TerritoryFlag
// Vanilla reference: P:\scripts\4_world\classes\useractionscomponent\actions\actioninteractbase.c
// Pattern: ActionInteractBase with CCTObject target condition
// ═══════════════════════════════════════════════════════════════════════════════

class ActionOpenCaptureMenu : ActionInteractBase
{
	void ActionOpenCaptureMenu()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}
	
	override string GetText()
	{
		return "Начать захват";
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target) return false;
		
		TerritoryFlag flag = TerritoryFlag.Cast(target.GetObject());
		if (!flag) return false;
		
		// Flag pole must be built (has at least base part)
		if (!flag.HasBase()) return false;
		
		return true;
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		TerritoryFlag flagPole = TerritoryFlag.Cast(action_data.m_Target.GetObject());
		if (!flagPole) return;
		
		// Store target in 4_World static holder (readable by 5_Mission)
		GPDA_CapturePending.s_TargetFlagPole = flagPole;
		
		// Open capture menu via EnterScriptedMenu
		GetGame().GetUIManager().EnterScriptedMenu(MENU_GPDA_CAPTURE, null);
		
		Print("[GPDA][Capture] Opened capture menu for flag pole at " + flagPole.GetPosition().ToString());
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		// Server-side: nothing needed, capture start is via RPC
	}
}
