// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Capture Zones Tool
// Interface for managing capture points
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_CaptureToolMenu extends UIScriptedMenu
{
	// New layout filename
	protected const string CAPTURE_LAYOUT_PATH = "Greshnik_PDALITE/GUI/Layouts/capture_zones.layout";
	
	// Main frame (optional)
	protected Widget m_CaptureMainFrame;
	protected ButtonWidget m_CaptureBtnBack;
	
	// Static instance
	private static ref GPDA_CaptureToolMenu s_Instance;
	
	static GPDA_CaptureToolMenu GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new GPDA_CaptureToolMenu();
		}
		return s_Instance;
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets(CAPTURE_LAYOUT_PATH);
		
		if (!layoutRoot)
		{
			Print("[GPDA] ERROR: Failed to load capture tool layout!");
			return null;
		}
		
		// Main frame (not critical)
		m_CaptureMainFrame = layoutRoot.FindAnyWidget("CaptureZones_Root");
		// No back button in new layout; allow closing with ESC only
		
		Print("[GPDA] Capture tool menu loaded successfully!");
		return layoutRoot;
	}
	
	override void OnShow()
	{
		super.OnShow();
		// Lock player and show cursor while menu is open
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetUIManager().ShowCursor(true);
		GetGame().GetInput().ChangeGameFocus(1);
		Print("[GPDA] Capture tool menu opened");
	}
	
	override void OnHide()
	{
		super.OnHide();
		// Restore controls when closing
		GetGame().GetMission().PlayerControlEnable(true);
		GetGame().GetUIManager().ShowCursor(false);
		GetGame().GetInput().ResetGameFocus();
		Print("[GPDA] Capture tool menu closed");
	}
	
	// No buttons to handle; rely on ESC/back
	
	void OpenCaptureTool()
	{
		if (IsOpen())
			return;
			
		GetGame().GetUIManager().ShowScriptedMenu(this, null);
	}
	
	void CloseCaptureTool()
	{
		if (!IsOpen())
			return;
			
		GetGame().GetUIManager().HideScriptedMenu(this);
	}
	
	static bool IsOpen()
	{
		return GetGame().GetUIManager().GetMenu() == GetInstance();
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			CloseCaptureTool();
			return true;
		}
		return super.OnKeyDown(w, x, y, key);
	}
}
