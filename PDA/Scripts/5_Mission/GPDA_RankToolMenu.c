// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Rank & XP Tool
// Interface for managing ranks and experience
// ═══════════════════════════════════════════════════════════════════════════════
 
 class GPDA_RankToolMenu extends UIScriptedMenu
 {
 	// New layout filename
 	protected const string RANK_LAYOUT_PATH = "Greshnik_PDALITE/GUI/Layouts/rank_management.layout";
 	
 	// Widgets
 	protected Widget m_RankMainFrame;
 	protected EditBoxWidget m_InputName;
 	protected EditBoxWidget m_InputXP;
 	protected EditBoxWidget m_InputOrder;
 	protected MultilineEditBoxWidget m_InputPermissions;
 	protected ButtonWidget m_BtnCreate;
 	protected TextListboxWidget m_RankList;
 	protected ButtonWidget m_BtnEdit;
 	protected ButtonWidget m_BtnDelete;
 	
 	protected ref array<ref GPDA_RankEntry> m_Ranks;
 	
 	// Static instance
 	private static ref GPDA_RankToolMenu s_Instance;
 	
 	static GPDA_RankToolMenu GetInstance()
 	{
 		if (!s_Instance)
 		{
 			s_Instance = new GPDA_RankToolMenu();
 		}
 		return s_Instance;
 	}
 	
 	override Widget Init()
 	{
 		layoutRoot = GetGame().GetWorkspace().CreateWidgets(RANK_LAYOUT_PATH);
 		
 		if (!layoutRoot)
 		{
 			Print("[GPDA] ERROR: Failed to load rank tool layout!");
 			return null;
 		}
 		
 		// Main frame (not critical)
 		m_RankMainFrame = layoutRoot.FindAnyWidget("RankManagement_Root");
 		
 		// Map widgets
 		m_InputName = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Input_Name"));
 		m_InputXP = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Input_XP"));
 		m_InputOrder = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Input_Sequence"));
 		m_InputPermissions = MultilineEditBoxWidget.Cast(layoutRoot.FindAnyWidget("Input_Permissions"));
 		m_BtnCreate = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Btn_Create"));
 		m_RankList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("RankList"));
 		m_BtnEdit = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Btn_Edit"));
 		m_BtnDelete = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Btn_Delete"));
 		
 		Print("[GPDA] Rank tool menu loaded successfully!");
 		return layoutRoot;
 	}
 	
 	override void OnShow()
 	{
 		super.OnShow();
 		if (GetGame() && GetGame().GetMission())
			GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
 		GetGame().GetUIManager().ShowCursor(true);
 		GetGame().GetInput().ChangeGameFocus(1);
 		Print("[GPDA] Rank tool menu opened");
		RequestAllRanks();
 	}
 	
 	override void OnHide()
 	{
 		super.OnHide();
 		if (GetGame() && GetGame().GetMission())
			GetGame().GetMission().PlayerControlEnable(true);
 		GetGame().GetUIManager().ShowCursor(false);
 		GetGame().GetInput().ResetGameFocus();
 		Print("[GPDA] Rank tool menu closed");
 	}
 	
 	void OpenRankTool()
 	{
 		if (IsOpen())
 			return;
 			
 		GetGame().GetUIManager().ShowScriptedMenu(this, null);
 		RequestAllRanks();
 	}
 	
 	void CloseRankTool()
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
 			CloseRankTool();
 			return true;
 		}
 		return super.OnKeyDown(w, x, y, key);
 	}
 	
 	override bool OnClick(Widget w, int x, int y, int button)
 	{
 		if (w == m_BtnCreate)
 		{
 			CreateRank();
 			return true;
 		}
 		if (w == m_BtnDelete)
 		{
 			DeleteSelectedRank();
 			return true;
 		}
 		return super.OnClick(w, x, y, button);
 	}
 
 	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
 	{
 		if (w == m_RankList)
 		{
 			return true;
 		}
 		return super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
 	}
 
 	protected void RequestAllRanks()
 	{
 		GetRPCManager().SendRPC("GPDA_Rank", "RequestAllRanks", null, true);
 	}
 
 	protected void CreateRank()
 	{
 		if (!m_InputName || !m_InputXP || !m_InputOrder) return;
 		string name = m_InputName.GetText();
 		name.Trim();
 		if (name == "") return;
 		int order = m_InputOrder.GetText().ToInt();
 		int xp = m_InputXP.GetText().ToInt();
 		string color = "#FFFFFF";
 		ref Param4<int, string, int, string> data = new Param4<int, string, int, string>(order, name, xp, color);
 		GetRPCManager().SendRPC("GPDA_Rank", "AdminCreateRank", data, true);
 		RequestAllRanks();
 	}
 
 	protected void DeleteSelectedRank()
 	{
 		if (!m_Ranks || !m_RankList) return;
 		int row = m_RankList.GetSelectedRow();
 		if (row < 0 || row >= m_Ranks.Count()) return;
 		string name = m_Ranks[row].Name;
 		ref Param1<string> data = new Param1<string>(name);
 		GetRPCManager().SendRPC("GPDA_Rank", "AdminDeleteRank", data, true);
 		RequestAllRanks();
 	}
 
 	void UpdateRanksList(array<ref GPDA_RankEntry> ranks)
 	{
 		m_Ranks = ranks;
 		if (!m_RankList) return;
 		m_RankList.ClearItems();
 		if (!ranks) return;
 		foreach (GPDA_RankEntry r : ranks)
 		{
 			string line = r.Order.ToString() + " | " + r.Name + " | XP " + r.RequiredXP.ToString();
 			m_RankList.AddItem(line, null, 0);
 		}
 		if (ranks.Count() > 0)
 			m_RankList.SelectRow(0);
 	}
 }
