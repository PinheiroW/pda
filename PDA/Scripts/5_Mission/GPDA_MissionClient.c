// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Mission Client
// Handles initialization, CreateScriptedMenu, and CTRL+L keybind for spawner
// ═══════════════════════════════════════════════════════════════════════════════

modded class MissionGameplay
{
	protected ref GPDA_ClientRPC m_GPDA_RPC;
	
	override void OnInit()
	{
		super.OnInit();
		
		if (!GetGame().IsDedicatedServer())
		{
			if (!m_GPDA_RPC)
			{
				m_GPDA_RPC = new GPDA_ClientRPC();
			}
			
			// Initialize settings manager
			if (!g_GPDA_SettingsManager)
			{
				g_GPDA_SettingsManager = GPDA_SettingsManager.GetInstance();
				g_GPDA_SettingsManager.Init();
			}
			
			if (!g_GPDA_ClientPrefsManager)
			{
				g_GPDA_ClientPrefsManager = GPDA_ClientPrefsManager.GetInstance();
				g_GPDA_ClientPrefsManager.Init();
			}
			
			Print("[GPDA] Client initialized - Use PDA item to open menu, CTRL+L for Mutant Spawner (admin)");
		}
	}
	
	override void OnMissionFinish()
	{
		if (GPDA_Menu.IsOpen())
		{
			GPDA_Menu.ClosePDA();
		}
		
		super.OnMissionFinish();
	}
	
	// ─── Create scripted menus by ID ─────────────────────────────────────────
	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = NULL;
		
		switch (id)
		{
			case MENU_GPDA_MAIN:
				menu = new GPDA_Menu();
				break;
			case MENU_GPDA_MUTANT_SPAWNER:
				menu = new GPDA_MutantSpawnerUI();
				break;
			case MENU_GPDA_CAPTURE:
				menu = new GPDA_CaptureMenu();
				break;
			case MENU_GPDA_GROUP_ADMIN:
				menu = new GPDA_GroupAdminUI();
				break;
		}
		
		if (menu)
		{
			menu.SetID(id);
			return menu;
		}
		
		return super.CreateScriptedMenu(id);
	}
	
	// ─── CTRL+L keybind for Mutant Spawner admin menu ────────────────────────
	override void OnKeyPress(int key)
	{
		super.OnKeyPress(key);
		
		if (GetGame().IsDedicatedServer()) return;
		
		// CTRL+L = Open Mutant Spawner (admin only)
		if (key == KeyCode.KC_L)
		{
			// Check if CTRL is held (UAWalkRunTemp = Left CTRL in DayZ)
			Input inp = GetGame().GetInput();
			if (inp && inp.LocalValue("UAWalkRunTemp") > 0)
			{
				// Check if any menu is already open
				if (GetGame().GetUIManager().GetMenu() != null)
					return;
				
				GetGame().GetUIManager().EnterScriptedMenu(MENU_GPDA_MUTANT_SPAWNER, null);
			}
		}
		
		// CTRL+K = Open Group Admin (admin only)
		if (key == KeyCode.KC_K)
		{
			Input inp2 = GetGame().GetInput();
			if (inp2 && inp2.LocalValue("UAWalkRunTemp") > 0)
			{
				if (GetGame().GetUIManager().GetMenu() != null)
					return;
				
				GetGame().GetUIManager().EnterScriptedMenu(MENU_GPDA_GROUP_ADMIN, null);
			}
		}
	}
}
