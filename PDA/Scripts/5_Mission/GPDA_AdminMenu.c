// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Admin Menu
// Admin panel interface for server management
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_AdminMenu extends UIScriptedMenu
{
	// Updated to new unified layout
	protected const string ADMIN_LAYOUT_PATH = "Greshnik_PDALITE/GUI/Layouts/adminnews.layout";
	
	// Main frame
	protected Widget m_AdminMainFrame;
	
	// Tab buttons
	protected ButtonWidget m_TabRanks;
	protected ButtonWidget m_TabTickets;
	protected ButtonWidget m_TabZones;
	protected ButtonWidget m_TabXP;
	protected ButtonWidget m_BtnClose;
	
	// Current active tab
	protected int m_ActiveTab = 0; // 0=Ranks, 1=Tickets, 2=Zones, 3=XP
	
	// ========== RANK MANAGEMENT WIDGETS ==========
	protected EditBoxWidget m_Rank_Input_Name;
	protected EditBoxWidget m_Rank_Input_XP;
	protected EditBoxWidget m_Rank_Input_Sequence;
	protected MultilineEditBoxWidget m_Rank_Input_Permissions;
	protected ButtonWidget m_Rank_Btn_Create;
	protected TextListboxWidget m_Rank_RankList;
	protected ButtonWidget m_Rank_Btn_Edit;
	protected ButtonWidget m_Rank_Btn_Delete;
	
	// All Rank widgets for visibility toggle
	protected ref array<Widget> m_RankWidgets;
	
	// ========== TICKET TOOLS WIDGETS ==========
	protected Widget m_Ticket_TicketListContent;
	protected Widget m_Ticket_TicketListScroll;
	protected ScrollWidget m_Ticket_TicketDescScroll;
	protected ref array<Widget> m_TicketListItems;
	protected int m_SelectedTicketIndex = -1;
	protected ButtonWidget m_Ticket_Btn_Refresh;
	protected TextWidget m_Ticket_TicketTitle;
	protected TextWidget m_Ticket_TicketDescription;
	protected MultilineEditBoxWidget m_Ticket_Input_Response;
	protected ButtonWidget m_Ticket_Btn_Reply;
	protected ButtonWidget m_Ticket_Btn_CloseTicket;
	protected string m_SelectedTicketId;
	
	// All Ticket widgets for visibility toggle
	protected ref array<Widget> m_TicketWidgets;
	protected ref array<ref GPDA_TicketInfo> m_TicketsData;
	
	// Store ranks data for delete/edit
	protected ref array<ref GPDA_RankEntry> m_RanksData;
	
	// Zone owner and container
	protected string m_SelectedZoneOwner = "";
	protected string m_SelectedZoneContainer = "";
	protected ref array<string> m_AllGroups;
	protected bool m_SelectingZoneOwner = false;
	
	// ========== CAPTURE ZONES WIDGETS ==========
	protected EditBoxWidget m_Zone_Input_Name;
	protected EditBoxWidget m_Zone_Input_Coords;
	protected ButtonWidget m_Zone_Btn_GetPos;
	protected EditBoxWidget m_Zone_Input_Radius;
	protected EditBoxWidget m_Zone_Input_Points;
	protected ButtonWidget m_Zone_Btn_SelectOwner;
	protected ButtonWidget m_Zone_Btn_SelectContainer;
	protected ButtonWidget m_Zone_Btn_Create;
	protected ButtonWidget m_Zone_Btn_Save;
	protected TextListboxWidget m_Zone_ZoneList;
	protected ButtonWidget m_Zone_Btn_Edit;
	protected ButtonWidget m_Zone_Btn_Delete;
	
	// Editing state
	protected string m_Zone_EditingName = "";
	
	// All Zone widgets for visibility toggle
	protected ref array<Widget> m_ZoneWidgets;
	protected ref array<Widget> m_XPWidgets;
	
	// ========== XP SETTINGS WIDGETS ==========
	protected TextWidget m_XP_LblTitle;
	protected TextWidget m_XP_LblKills;
	protected TextListboxWidget m_XP_KillsList;
	protected EditBoxWidget m_XP_KillClass;
	protected EditBoxWidget m_XP_KillXP;
	protected ButtonWidget m_XP_BtnKillApply;
	protected ButtonWidget m_XP_BtnKillDelete;
	protected ButtonWidget m_XP_BtnModeHands;
	protected ButtonWidget m_XP_BtnModeInv;
	protected TextListboxWidget m_XP_ItemsList;
	protected EditBoxWidget m_XP_ItemClass;
	protected EditBoxWidget m_XP_ItemXP;
	protected ButtonWidget m_XP_BtnItemApply;
	protected ButtonWidget m_XP_BtnItemDelete;
	protected int m_XP_ItemMode = 0; // 0=inventory, 1=hands
	protected ref array<string> m_XP_KillsEntries;
	protected ref array<string> m_XP_ItemsHandsEntries;
	protected ref array<string> m_XP_ItemsInvEntries;
	
	// Stored capture points for edit/delete
	protected ref array<ref GPDA_CapturePointInfo> m_CapturePointsData;
	
	// Static instance
	private static ref GPDA_AdminMenu s_Instance;
	
	static GPDA_AdminMenu GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new GPDA_AdminMenu();
		}
		return s_Instance;
	}
	
	// Static helper for legacy calls GPDA_AdminMenu.Open()
	static void Open()
	{
		GetInstance().OpenAdminMenu();
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets(ADMIN_LAYOUT_PATH);
		
		if (!layoutRoot)
		{
			Print("[GPDA] ERROR: Failed to load admin menu layout!");
			return null;
		}
		
		// layoutRoot is already AdminNews_Root
		m_AdminMainFrame = layoutRoot;
		
		Print("[GPDA] Layout root found: " + layoutRoot.GetName());
		
		// Initialize widget arrays
		m_RankWidgets = new array<Widget>;
		m_TicketWidgets = new array<Widget>;
		m_ZoneWidgets = new array<Widget>;
		m_XPWidgets = new array<Widget>;
		m_CapturePointsData = new array<ref GPDA_CapturePointInfo>;
		m_XP_KillsEntries = new array<string>;
		m_XP_ItemsHandsEntries = new array<string>;
		m_XP_ItemsInvEntries = new array<string>;
		
		// Map tab buttons
		m_TabRanks = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Tab_Ranks"));
		m_TabTickets = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Tab_Tickets"));
		m_TabZones = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Tab_Zones"));
		m_TabXP = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Tab_XP"));
		m_BtnClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Btn_Close"));
		
		Print("[GPDA] Tab buttons - Ranks: " + (m_TabRanks != null) + ", Tickets: " + (m_TabTickets != null) + ", Zones: " + (m_TabZones != null) + ", XP: " + (m_TabXP != null) + ", Close: " + (m_BtnClose != null));
		
		// ========== MAP RANK WIDGETS ==========
		m_Rank_Input_Name = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Rank_Input_Name"));
		m_Rank_Input_XP = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Rank_Input_XP"));
		m_Rank_Input_Sequence = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Rank_Input_Sequence"));
		m_Rank_Input_Permissions = MultilineEditBoxWidget.Cast(layoutRoot.FindAnyWidget("Rank_Input_Permissions"));
		m_Rank_Btn_Create = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Rank_Btn_Create"));
		m_Rank_RankList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("Rank_RankList"));
		m_Rank_Btn_Edit = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Rank_Btn_Edit"));
		m_Rank_Btn_Delete = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Rank_Btn_Delete"));
		
		// Manually collect all Rank widgets
		m_RankWidgets.Insert(m_Rank_Input_Name);
		m_RankWidgets.Insert(m_Rank_Input_XP);
		m_RankWidgets.Insert(m_Rank_Input_Sequence);
		m_RankWidgets.Insert(m_Rank_Input_Permissions);
		m_RankWidgets.Insert(m_Rank_Btn_Create);
		m_RankWidgets.Insert(m_Rank_RankList);
		m_RankWidgets.Insert(m_Rank_Btn_Edit);
		m_RankWidgets.Insert(m_Rank_Btn_Delete);
		// Add Rank labels and backgrounds
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_LblName"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_InputNameBg"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_LblXP"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_InputXPBg"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_LblSequence"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_InputSequenceBg"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_LblPermissions"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_InputPermissionsBg"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_LblRanks"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_ColName"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_ColXP"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_ColID"));
		m_RankWidgets.Insert(layoutRoot.FindAnyWidget("Rank_ListBg"));
		
		// ========== MAP TICKET WIDGETS ==========
		m_Ticket_TicketListScroll = layoutRoot.FindAnyWidget("Ticket_TicketListScroll");
		m_Ticket_TicketListContent = layoutRoot.FindAnyWidget("Ticket_TicketListContent");
		m_TicketListItems = new array<Widget>;
		m_Ticket_Btn_Refresh = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Ticket_Btn_Refresh"));
		m_Ticket_TicketTitle = TextWidget.Cast(layoutRoot.FindAnyWidget("Ticket_TicketTitle"));
		m_Ticket_TicketDescScroll = ScrollWidget.Cast(layoutRoot.FindAnyWidget("Ticket_TicketDescScroll"));
		m_Ticket_TicketDescription = TextWidget.Cast(layoutRoot.FindAnyWidget("Ticket_TicketDescription"));
		m_Ticket_Input_Response = MultilineEditBoxWidget.Cast(layoutRoot.FindAnyWidget("Ticket_Input_Response"));
		m_Ticket_Btn_Reply = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Ticket_Btn_Reply"));
		m_Ticket_Btn_CloseTicket = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Ticket_Btn_CloseTicket"));
		
		// Manually collect all Ticket widgets
		m_TicketWidgets.Insert(m_Ticket_TicketListScroll);
		m_TicketWidgets.Insert(m_Ticket_Btn_Refresh);
		m_TicketWidgets.Insert(m_Ticket_TicketTitle);
		m_TicketWidgets.Insert(m_Ticket_TicketDescScroll);
		m_TicketWidgets.Insert(m_Ticket_TicketDescription);
		m_TicketWidgets.Insert(m_Ticket_Input_Response);
		m_TicketWidgets.Insert(m_Ticket_Btn_Reply);
		m_TicketWidgets.Insert(m_Ticket_Btn_CloseTicket);
		// Add Ticket labels and backgrounds
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_LblTickets"));
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_TicketListBg"));
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_LblTicketDetails"));
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_LblTicketTitle"));
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_LblTicketDesc"));
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_TicketDescBg"));
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_LblResponse"));
		m_TicketWidgets.Insert(layoutRoot.FindAnyWidget("Ticket_InputResponseBg"));
		
		// ========== MAP ZONE WIDGETS ==========
		m_Zone_Input_Name = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Zone_Input_Name"));
		m_Zone_Input_Coords = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Zone_Input_Coords"));
		m_Zone_Btn_GetPos = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Zone_Btn_GetPos"));
		m_Zone_Input_Radius = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Zone_Input_Radius"));
		m_Zone_Input_Points = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("Zone_Input_Points"));
		m_Zone_Btn_SelectOwner = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Zone_Btn_SelectOwner"));
		m_Zone_Btn_SelectContainer = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Zone_Btn_SelectContainer"));
		m_Zone_Btn_Create = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Zone_Btn_Create"));
		m_Zone_Btn_Save = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Zone_Btn_Save"));
		m_Zone_ZoneList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("Zone_ZoneList"));
		m_Zone_Btn_Edit = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Zone_Btn_Edit"));
		m_Zone_Btn_Delete = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Zone_Btn_Delete"));
		
		// Manually collect all Zone widgets
		m_ZoneWidgets.Insert(m_Zone_Input_Name);
		m_ZoneWidgets.Insert(m_Zone_Input_Coords);
		m_ZoneWidgets.Insert(m_Zone_Btn_GetPos);
		m_ZoneWidgets.Insert(m_Zone_Input_Radius);
		m_ZoneWidgets.Insert(m_Zone_Input_Points);
		m_ZoneWidgets.Insert(m_Zone_Btn_SelectOwner);
		m_ZoneWidgets.Insert(m_Zone_Btn_SelectContainer);
		m_ZoneWidgets.Insert(m_Zone_Btn_Create);
		m_ZoneWidgets.Insert(m_Zone_Btn_Save);
		m_ZoneWidgets.Insert(m_Zone_ZoneList);
		m_ZoneWidgets.Insert(m_Zone_Btn_Edit);
		m_ZoneWidgets.Insert(m_Zone_Btn_Delete);
		// Add Zone labels and backgrounds
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_LblName"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_InputNameBg"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_LblCoords"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_InputCoordsBg"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_LblRadius"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_InputRadiusBg"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_LblPoints"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_InputPointsBg"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_LblOwner"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_InputOwnerBg"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_LblContainer"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_InputContainerBg"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_LblZones"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_ColName"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_ColOwner"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_ColPoints"));
		m_ZoneWidgets.Insert(layoutRoot.FindAnyWidget("Zone_ListBg"));
		
		m_XP_LblTitle = TextWidget.Cast(layoutRoot.FindAnyWidget("XP_LblTitle"));
		m_XP_LblKills = TextWidget.Cast(layoutRoot.FindAnyWidget("XP_LblKills"));
		m_XP_KillsList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("XP_KillsList"));
		m_XP_KillClass = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("XP_KillClass"));
		m_XP_KillXP = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("XP_KillXP"));
		m_XP_BtnKillApply = ButtonWidget.Cast(layoutRoot.FindAnyWidget("XP_BtnKillApply"));
		m_XP_BtnKillDelete = ButtonWidget.Cast(layoutRoot.FindAnyWidget("XP_BtnKillDelete"));
		m_XP_BtnModeHands = ButtonWidget.Cast(layoutRoot.FindAnyWidget("XP_BtnModeHands"));
		m_XP_BtnModeInv = ButtonWidget.Cast(layoutRoot.FindAnyWidget("XP_BtnModeInv"));
		m_XP_ItemsList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("XP_ItemsList"));
		m_XP_ItemClass = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("XP_ItemClass"));
		m_XP_ItemXP = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("XP_ItemXP"));
		m_XP_BtnItemApply = ButtonWidget.Cast(layoutRoot.FindAnyWidget("XP_BtnItemApply"));
		m_XP_BtnItemDelete = ButtonWidget.Cast(layoutRoot.FindAnyWidget("XP_BtnItemDelete"));
		
				
		m_XPWidgets.Insert(m_XP_LblTitle);
		m_XPWidgets.Insert(m_XP_LblKills);
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_KillsListBg"));
		m_XPWidgets.Insert(m_XP_KillsList);
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_LblKillEdit"));
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_LblKillClass"));
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_KillClassBg"));
		m_XPWidgets.Insert(m_XP_KillClass);
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_LblKillXP"));
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_KillXPBg"));
		m_XPWidgets.Insert(m_XP_KillXP);
		m_XPWidgets.Insert(m_XP_BtnKillApply);
		m_XPWidgets.Insert(m_XP_BtnKillDelete);
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_LblItems"));
		m_XPWidgets.Insert(m_XP_BtnModeHands);
		m_XPWidgets.Insert(m_XP_BtnModeInv);
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_ItemsListBg"));
		m_XPWidgets.Insert(m_XP_ItemsList);
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_LblItemClass"));
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_ItemClassBg"));
		m_XPWidgets.Insert(m_XP_ItemClass);
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_LblItemXP"));
		m_XPWidgets.Insert(layoutRoot.FindAnyWidget("XP_ItemXPBg"));
		m_XPWidgets.Insert(m_XP_ItemXP);
		m_XPWidgets.Insert(m_XP_BtnItemApply);
		m_XPWidgets.Insert(m_XP_BtnItemDelete);
		
		// Set default tab (Ranks)
		SwitchTab(0);
		
		Print("[GPDA] Admin menu loaded successfully!");
		return layoutRoot;
	}
	
	// Switch between tabs
	void SwitchTab(int tabIndex)
	{
		m_ActiveTab = tabIndex;
		
		Print("[GPDA] Switching to tab: " + tabIndex.ToString());
		
		// Hide all sections
		SetWidgetsVisible(m_RankWidgets, false);
		SetWidgetsVisible(m_TicketWidgets, false);
		SetWidgetsVisible(m_ZoneWidgets, false);
		SetWidgetsVisible(m_XPWidgets, false);
		
		// Show active section
		switch (tabIndex)
		{
			case 0: // Ranks
				SetWidgetsVisible(m_RankWidgets, true);
				Print("[GPDA] Rank widgets visible: " + m_RankWidgets.Count().ToString());
				break;
			case 1: // Tickets
				SetWidgetsVisible(m_TicketWidgets, true);
				Print("[GPDA] Ticket widgets visible: " + m_TicketWidgets.Count().ToString());
				break;
			case 2: // Zones
				SetWidgetsVisible(m_ZoneWidgets, true);
				Print("[GPDA] Zone widgets visible: " + m_ZoneWidgets.Count().ToString());
				// Reset zone owner selection mode and refresh zones list
				if (m_SelectingZoneOwner)
				{
					Print("[GPDA] Resetting zone owner selection mode");
					m_SelectingZoneOwner = false;
				}
				RequestZonesFromServer();
				break;
			case 3: // XP
				SetWidgetsVisible(m_XPWidgets, true);
				Print("[GPDA] XP widgets visible: " + m_XPWidgets.Count().ToString());
				RequestXPConfigFromServer();
				UpdateXPModeButtons();
				UpdateXPItemsList();
				break;
		}
	}
	
	protected void UpdateXPModeButtons()
	{
		int activeCol = ARGB(255, 20, 120, 160);
		int inactiveCol = ARGB(255, 40, 60, 70);
		if (m_XP_BtnModeHands)
		{
			if (m_XP_ItemMode == 1)
				m_XP_BtnModeHands.SetColor(activeCol);
			else
				m_XP_BtnModeHands.SetColor(inactiveCol);
		}
		if (m_XP_BtnModeInv)
		{
			if (m_XP_ItemMode == 0)
				m_XP_BtnModeInv.SetColor(activeCol);
			else
				m_XP_BtnModeInv.SetColor(inactiveCol);
		}
	}
	
	protected void ParseXPString(string src, array<string> outArr)
	{
		outArr.Clear();
		if (src == "") return;
		TStringArray entries = new TStringArray;
		src.Split(";", entries);
		foreach (string e : entries)
		{
			e.Trim();
			if (e != "")
				outArr.Insert(e);
		}
	}
	
	protected void FillList(TextListboxWidget list, array<string> entries)
	{
		if (!list) return;
		list.ClearItems();
		for (int i = 0; i < entries.Count(); i++)
		{
			string row = entries[i];
			row.Replace("|", " | ");
			list.AddItem(row, null, 0);
		}
	}
	
	protected void UpdateXPItemsList()
	{
		if (m_XP_ItemMode == 1)
			FillList(m_XP_ItemsList, m_XP_ItemsHandsEntries);
		else
			FillList(m_XP_ItemsList, m_XP_ItemsInvEntries);
	}
	
	void UpdateXPConfig(string killsStr, string handsStr, string invStr)
	{
		ParseXPString(killsStr, m_XP_KillsEntries);
		ParseXPString(handsStr, m_XP_ItemsHandsEntries);
		ParseXPString(invStr, m_XP_ItemsInvEntries);
		FillList(m_XP_KillsList, m_XP_KillsEntries);
		UpdateXPItemsList();
	}
	
	// Helper to set visibility for array of widgets
	void SetWidgetsVisible(array<Widget> widgets, bool visible)
	{
		if (!widgets)
			return;
			
		for (int i = 0; i < widgets.Count(); i++)
		{
			if (widgets[i])
				widgets[i].Show(visible);
		}
	}
	
	override void OnShow()
	{
		super.OnShow();
		// Lock player controls and show cursor while admin menu is open
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetUIManager().ShowCursor(true);
		GetGame().GetInput().ChangeGameFocus(1);
		Print("[GPDA] Admin menu opened");
		
		// Request ranks, tickets, and zones from server
		RequestRanksFromServer();
		RequestTicketsFromServer();
		RequestZonesFromServer();
		RequestXPConfigFromServer();
	}
	
	void RequestXPConfigFromServer()
	{
		GetRPCManager().SendRPC("GPDA_Rank", "RequestXPConfig", null, true);
	}
	
	void RequestRanksFromServer()
	{
		Print("[GPDA] Requesting ranks from server...");
		GetRPCManager().SendRPC("GPDA_Rank", "RequestAllRanks", null, true);
	}
	
	void RequestTicketsFromServer()
	{
		Print("[GPDA] Requesting tickets from server...");
		GetRPCManager().SendRPC("GPDA_Tickets", "RequestAllTickets", null, true);
	}
	
	void RequestZonesFromServer()
	{
		Print("[GPDA] Requesting zones from server...");
		GetRPCManager().SendRPC("GPDA_Capture", "RequestCapturePoints", null, true);
	}
	
	override void OnHide()
	{
		super.OnHide();
		// Restore controls when menu closes
		MissionBase mission = GetGame().GetMission();
		if (mission)
			mission.PlayerControlEnable(true);
		
		UIManager ui = GetGame().GetUIManager();
		if (ui)
			ui.ShowCursor(false);
		
		Input input = GetGame().GetInput();
		if (input)
			input.ResetGameFocus();
		
		Print("[GPDA] Admin menu closed");
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!w)
			return false;
		
		string widgetName = w.GetName();
		Print("[GPDA] OnClick: " + widgetName);
		
		// Close button
		if (w == m_BtnClose || widgetName == "Btn_Close")
		{
			Print("[GPDA] Close button clicked");
			CloseAdminMenu();
			return true;
		}
		
		// Tab buttons
		if (w == m_TabRanks || widgetName == "Tab_Ranks")
		{
			Print("[GPDA] Tab Ranks clicked");
			SwitchTab(0);
			return true;
		}
		if (w == m_TabTickets || widgetName == "Tab_Tickets")
		{
			Print("[GPDA] Tab Tickets clicked");
			SwitchTab(1);
			return true;
		}
		if (w == m_TabZones || widgetName == "Tab_Zones")
		{
			Print("[GPDA] Tab Zones clicked");
			SwitchTab(2);
			return true;
		}
		if (w == m_TabXP || widgetName == "Tab_XP")
		{
			Print("[GPDA] Tab XP clicked");
			SwitchTab(3);
			return true;
		}
		
		if (w == m_XP_BtnModeHands || widgetName == "XP_BtnModeHands")
		{
			m_XP_ItemMode = 1;
			UpdateXPModeButtons();
			UpdateXPItemsList();
			return true;
		}
		if (w == m_XP_BtnModeInv || widgetName == "XP_BtnModeInv")
		{
			m_XP_ItemMode = 0;
			UpdateXPModeButtons();
			UpdateXPItemsList();
			return true;
		}
		if (w == m_XP_BtnKillApply || widgetName == "XP_BtnKillApply")
		{
			OnXPKillApply();
			return true;
		}
		if (w == m_XP_BtnKillDelete || widgetName == "XP_BtnKillDelete")
		{
			OnXPKillDelete();
			return true;
		}
		if (w == m_XP_BtnItemApply || widgetName == "XP_BtnItemApply")
		{
			OnXPItemApply();
			return true;
		}
		if (w == m_XP_BtnItemDelete || widgetName == "XP_BtnItemDelete")
		{
			OnXPItemDelete();
			return true;
		}
		if (w == m_XP_KillsList || widgetName == "XP_KillsList")
		{
			OnXPKillSelected();
			return true;
		}
		if (w == m_XP_ItemsList || widgetName == "XP_ItemsList")
		{
			OnXPItemSelected();
			return true;
		}
		
		// Rank Management buttons
		if (w == m_Rank_Btn_Create)
		{
			OnRankCreate();
			return true;
		}
		if (w == m_Rank_Btn_Edit)
		{
			OnRankEdit();
			return true;
		}
		if (w == m_Rank_Btn_Delete)
		{
			OnRankDelete();
			return true;
		}
		
		// Ticket Tools buttons
		if (w == m_Ticket_Btn_Refresh)
		{
			OnTicketRefresh();
			return true;
		}
		if (w == m_Ticket_Btn_Reply)
		{
			OnTicketReply();
			return true;
		}
		if (w == m_Ticket_Btn_CloseTicket)
		{
			OnTicketClose();
			return true;
		}
		
		// Capture Zones buttons
		if (w == m_Zone_Btn_GetPos)
		{
			OnZoneGetPos();
			return true;
		}
		if (w == m_Zone_Btn_SelectOwner)
		{
			OnZoneSelectOwner();
			return true;
		}
		if (w == m_Zone_Btn_SelectContainer)
		{
			OnZoneSelectContainer();
			return true;
		}
		if (w == m_Zone_Btn_Create)
		{
			OnZoneCreate();
			return true;
		}
		if (w == m_Zone_Btn_Save)
		{
			OnZoneSave();
			return true;
		}
		if (w == m_Zone_Btn_Edit)
		{
			OnZoneEdit();
			return true;
		}
		if (w == m_Zone_Btn_Delete)
		{
			OnZoneDelete();
			return true;
		}
		
		// Ticket list item click
		if (m_TicketListItems && m_TicketListItems.Find(w) != -1)
		{
			m_SelectedTicketIndex = m_TicketListItems.Find(w);
			OnTicketSelected();
			return true;
		}
		
		// Rank list selection
		if (w == m_Rank_RankList)
		{
			OnRankSelected();
			return true;
		}
		
		// Zone list selection (for zone editing or group selection)
		if (w == m_Zone_ZoneList)
		{
			if (m_SelectingZoneOwner)
			{
				OnZoneOwnerSelected();
				return true;
			}
			// Otherwise, normal zone selection for editing
			return true;
		}
		
		// Zone list selection
		if (w == m_Zone_ZoneList)
		{
			OnZoneSelected();
			return true;
		}
		
		return super.OnClick(w, x, y, button);
	}

	protected void OnXPKillSelected()
	{
		if (!m_XP_KillsList) return;
		int selected = m_XP_KillsList.GetSelectedRow();
		if (selected == -1 || selected >= m_XP_KillsEntries.Count()) return;
		string entry = m_XP_KillsEntries[selected];
		TStringArray parts = new TStringArray;
		entry.Split("|", parts);
		if (parts.Count() < 2) return;
		string cls = parts[0];
		cls.Trim();
		string xpStr = parts[1];
		xpStr.Trim();
		if (m_XP_KillClass) m_XP_KillClass.SetText(cls);
		if (m_XP_KillXP) m_XP_KillXP.SetText(xpStr);
	}

	protected void OnXPKillApply()
	{
		if (!m_XP_KillClass || !m_XP_KillXP) return;
		string cls = m_XP_KillClass.GetText();
		cls.Trim();
		if (cls == "") return;
		int xp = m_XP_KillXP.GetText().ToInt();
		ref Param2<string, int> data = new Param2<string, int>(cls, xp);
		GetRPCManager().SendRPC("GPDA_Rank", "AdminSetXPKill", data, true);
	}

	protected void OnXPKillDelete()
	{
		if (!m_XP_KillsList || m_XP_KillsEntries.Count() == 0) return;
		int selected = m_XP_KillsList.GetSelectedRow();
		if (selected == -1 || selected >= m_XP_KillsEntries.Count()) return;
		string entry = m_XP_KillsEntries[selected];
		TStringArray parts = new TStringArray;
		entry.Split("|", parts);
		if (parts.Count() < 1) return;
		string cls = parts[0];
		cls.Trim();
		ref Param1<string> data = new Param1<string>(cls);
		GetRPCManager().SendRPC("GPDA_Rank", "AdminRemoveXPKill", data, true);
	}

	protected void OnXPItemSelected()
	{
		if (!m_XP_ItemsList) return;
		array<string> src;
		if (m_XP_ItemMode == 1)
			src = m_XP_ItemsHandsEntries;
		else
			src = m_XP_ItemsInvEntries;
		int selected = m_XP_ItemsList.GetSelectedRow();
		if (selected == -1 || selected >= src.Count()) return;
		string entry = src[selected];
		TStringArray parts = new TStringArray;
		entry.Split("|", parts);
		if (parts.Count() < 2) return;
		string cls = parts[0];
		cls.Trim();
		string xpStr = parts[1];
		xpStr.Trim();
		if (m_XP_ItemClass) m_XP_ItemClass.SetText(cls);
		if (m_XP_ItemXP) m_XP_ItemXP.SetText(xpStr);
	}

	protected void OnXPItemApply()
	{
		if (!m_XP_ItemClass || !m_XP_ItemXP) return;
		string cls = m_XP_ItemClass.GetText();
		cls.Trim();
		if (cls == "") return;
		int xp = m_XP_ItemXP.GetText().ToInt();
		ref Param3<int, string, int> data = new Param3<int, string, int>(m_XP_ItemMode, cls, xp);
		GetRPCManager().SendRPC("GPDA_Rank", "AdminSetXPItem", data, true);
	}

	protected void OnXPItemDelete()
	{
		if (!m_XP_ItemsList) return;
		array<string> src;
		if (m_XP_ItemMode == 1)
			src = m_XP_ItemsHandsEntries;
		else
			src = m_XP_ItemsInvEntries;
		int selected = m_XP_ItemsList.GetSelectedRow();
		if (selected == -1 || selected >= src.Count()) return;
		string entry = src[selected];
		TStringArray parts = new TStringArray;
		entry.Split("|", parts);
		if (parts.Count() < 1) return;
		string cls = parts[0];
		cls.Trim();
		ref Param2<int, string> data = new Param2<int, string>(m_XP_ItemMode, cls);
		GetRPCManager().SendRPC("GPDA_Rank", "AdminRemoveXPItem", data, true);
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			CloseAdminMenu();
			return true;
		}
		return super.OnKeyDown(w, x, y, key);
	}
	
	void OpenAdminMenu()
	{
		if (IsOpen())
			return;
			
		GetGame().GetUIManager().ShowScriptedMenu(this, null);
	}
	
	void CloseAdminMenu()
	{
		if (!IsOpen())
			return;
			
		GetGame().GetUIManager().HideScriptedMenu(this);
	}
	
	static bool IsOpen()
	{
		return GetGame().GetUIManager().GetMenu() == GetInstance();
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// RANK MANAGEMENT HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void OnRankCreate()
	{
		if (!m_Rank_Input_Name || !m_Rank_Input_XP || !m_Rank_Input_Sequence)
			return;
			
		string name = m_Rank_Input_Name.GetText();
		string xpStr = m_Rank_Input_XP.GetText();
		string seqStr = m_Rank_Input_Sequence.GetText();
		string permissions = "";
		
		if (m_Rank_Input_Permissions)
			m_Rank_Input_Permissions.GetText(permissions);
		
		if (name == "" || xpStr == "" || seqStr == "")
		{
			Print("[GPDA] Rank creation: Please fill all required fields");
			return;
		}
		
		int xp = xpStr.ToInt();
		int seq = seqStr.ToInt();
		
		Print("[GPDA] Creating rank: " + name + " (XP: " + xpStr + ", Seq: " + seqStr + ")");
		
		// Send RPC to server
		ref Param4<int, string, int, string> data = new Param4<int, string, int, string>(seq, name, xp, "#FFFFFF");
		GetRPCManager().SendRPC("GPDA_Rank", "AdminCreateRank", data, true);
		
		// Clear form
		if (m_Rank_Input_Name) m_Rank_Input_Name.SetText("");
		if (m_Rank_Input_XP) m_Rank_Input_XP.SetText("0");
		if (m_Rank_Input_Sequence) m_Rank_Input_Sequence.SetText("1");
		if (m_Rank_Input_Permissions) m_Rank_Input_Permissions.SetText("");
		
		// Request updated list after a short delay
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestRanksFromServer, 500, false);
	}
	
	void OnRankEdit()
	{
		if (!m_Rank_RankList || !m_RanksData)
			return;
			
		int selected = m_Rank_RankList.GetSelectedRow();
		if (selected == -1 || selected >= m_RanksData.Count())
		{
			Print("[GPDA] Please select a rank to edit");
			return;
		}
		
		GPDA_RankEntry rank = m_RanksData[selected];
		if (!rank)
		{
			Print("[GPDA] Invalid rank data");
			return;
		}
		
		Print("[GPDA] Editing rank: " + rank.Name);
		
		// Load rank data into form
		if (m_Rank_Input_Name) m_Rank_Input_Name.SetText(rank.Name);
		if (m_Rank_Input_XP) m_Rank_Input_XP.SetText(rank.RequiredXP.ToString());
		if (m_Rank_Input_Sequence) m_Rank_Input_Sequence.SetText(rank.Order.ToString());
	}
	
	void OnRankDelete()
	{
		if (!m_Rank_RankList || !m_RanksData)
			return;
			
		int selected = m_Rank_RankList.GetSelectedRow();
		if (selected == -1 || selected >= m_RanksData.Count())
		{
			Print("[GPDA] Please select a rank to delete");
			return;
		}
		
		GPDA_RankEntry rank = m_RanksData[selected];
		if (!rank)
		{
			Print("[GPDA] Invalid rank data");
			return;
		}
		
		Print("[GPDA] Deleting rank: " + rank.Name);
		
		// Send delete RPC
		ref Param1<string> data = new Param1<string>(rank.Name);
		GetRPCManager().SendRPC("GPDA_Rank", "AdminDeleteRank", data, true);
		
		// Request updated list after a short delay
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestRanksFromServer, 500, false);
	}
	
	void OnRankSelected()
	{
		OnRankEdit(); // Same as edit - load into form
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// TICKET TOOLS HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void OnTicketRefresh()
	{
		Print("[GPDA] Refreshing ticket list...");
		RequestTicketsFromServer();
	}
	
	void OnTicketReply()
	{
		if (!m_Ticket_Input_Response || !m_SelectedTicketId || m_SelectedTicketId == "")
		{
			Print("[GPDA] Please select a ticket first");
			return;
		}
			
		string response;
		m_Ticket_Input_Response.GetText(response);
		response.Trim();
		if (response == "")
		{
			Print("[GPDA] Please enter a response");
			return;
		}
		
		Print("[GPDA] Sending ticket reply to " + m_SelectedTicketId);
		ref Param2<string, string> data = new Param2<string, string>(m_SelectedTicketId, response);
		GetRPCManager().SendRPC("GPDA_Tickets", "SendMessage", data, true);
		
		m_Ticket_Input_Response.SetText("");
		
		// Request updated messages
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestTicketMessages, 500, false);
	}
	
	void RequestTicketMessages()
	{
		if (!m_SelectedTicketId || m_SelectedTicketId == "")
			return;
		
		ref Param1<string> data = new Param1<string>(m_SelectedTicketId);
		GetRPCManager().SendRPC("GPDA_Tickets", "RequestTicketMessages", data, true);
	}
	
	void OnTicketClose()
	{
		if (!m_SelectedTicketId || m_SelectedTicketId == "")
		{
			Print("[GPDA] Please select a ticket to close");
			return;
		}
		
		Print("[GPDA] Closing ticket: " + m_SelectedTicketId);
		ref Param1<string> data = new Param1<string>(m_SelectedTicketId);
		GetRPCManager().SendRPC("GPDA_Tickets", "CloseTicket", data, true);
		
		// Clear selection and refresh list
		m_SelectedTicketId = "";
		if (m_Ticket_TicketTitle) m_Ticket_TicketTitle.SetText("[Select a ticket to view details]");
		if (m_Ticket_TicketDescription) m_Ticket_TicketDescription.SetText("");
		if (m_Ticket_Input_Response) m_Ticket_Input_Response.SetText("");
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestTicketsFromServer, 500, false);
	}
	
	void OnTicketSelected()
	{
		if (!m_TicketsData)
			return;
			
		int selected = m_SelectedTicketIndex;
		if (selected == -1 || selected >= m_TicketsData.Count())
			return;
		
		GPDA_TicketInfo ticket = m_TicketsData[selected];
		if (!ticket)
			return;
		
		m_SelectedTicketId = ticket.Id;
		
		if (m_Ticket_TicketTitle)
			m_Ticket_TicketTitle.SetText(ticket.Title);
		
		Print("[GPDA] Ticket selected: " + ticket.Id + " - " + ticket.Title);
		
		// Request messages for this ticket
		RequestTicketMessages();
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// CAPTURE ZONES HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void OnZoneGetPos()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
			
		vector pos = player.GetPosition();
		string coords = pos[0].ToString() + " " + pos[1].ToString() + " " + pos[2].ToString();
		
		if (m_Zone_Input_Coords)
		{
			m_Zone_Input_Coords.SetText(coords);
			Print("[GPDA] Position set: " + coords);
		}
	}
	
	void OnZoneCreate()
	{
		if (!m_Zone_Input_Name || !m_Zone_Input_Coords || !m_Zone_Input_Radius)
			return;
			
		string name = m_Zone_Input_Name.GetText();
		string coords = m_Zone_Input_Coords.GetText();
		string radius = m_Zone_Input_Radius.GetText();
		string points = "100";
		
		if (m_Zone_Input_Points)
			points = m_Zone_Input_Points.GetText();
		
		if (name == "" || coords == "" || radius == "")
		{
			Print("[GPDA] Zone creation: Please fill all required fields");
			return;
		}
		
		// Parse coordinates
		array<string> coordsArray = new array<string>;
		coords.Split(" ", coordsArray);
		if (coordsArray.Count() != 3)
		{
			Print("[GPDA] Invalid coordinates format. Use: X Y Z");
			return;
		}
		
		float x = coordsArray[0].ToFloat();
		float y = coordsArray[1].ToFloat();
		float z = coordsArray[2].ToFloat();
		vector pos = Vector(x, y, z);
		
		float radiusF = radius.ToFloat();
		int pointsI = points.ToInt();
		
		// Use selected owner or empty string
		string finalOwner = m_SelectedZoneOwner;
		if (!finalOwner) finalOwner = "";
		
		Print("[GPDA] Creating zone: " + name + " at " + coords + " (radius: " + radius + ", owner: '" + finalOwner + "')");
		
		// Send RPC to server
		ref Param5<string, vector, float, int, string> data = new Param5<string, vector, float, int, string>(name, pos, radiusF, pointsI, finalOwner);
		GetRPCManager().SendRPC("GPDA_Capture", "AdminCreateCapture", data, true);
		
		// Clear form after creation
		if (m_Zone_Input_Name) m_Zone_Input_Name.SetText("");
		if (m_Zone_Input_Coords) m_Zone_Input_Coords.SetText("");
		if (m_Zone_Input_Radius) m_Zone_Input_Radius.SetText("50");
		if (m_Zone_Input_Points) m_Zone_Input_Points.SetText("100");
		m_SelectedZoneOwner = "";
		if (m_Zone_Btn_SelectOwner) m_Zone_Btn_SelectOwner.SetText("Select Owner/Faction");
		
		// Request updated list after a short delay
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestZonesFromServer, 1000, false);
	}
	
	void OnZoneEdit()
	{
		if (!m_Zone_ZoneList)
			return;
			
		int selected = m_Zone_ZoneList.GetSelectedRow();
		if (selected == -1)
		{
			Print("[GPDA] Please select a zone to edit");
			return;
		}
		
		// Need data for this index
		if (!m_CapturePointsData || selected >= m_CapturePointsData.Count())
		{
			Print("[GPDA] No capture data for selected index");
			return;
		}
		
		GPDA_CapturePointInfo cp = m_CapturePointsData[selected];
		if (!cp)
		{
			Print("[GPDA] Invalid capture point data");
			return;
		}
		
		Print("[GPDA] Editing zone: " + cp.Name + " (owner: '" + cp.OwnerGroup + "')");
		
		// Store name for save operation
		m_Zone_EditingName = cp.Name;
		
		string coords = cp.Position[0].ToString() + " " + cp.Position[1].ToString() + " " + cp.Position[2].ToString();
		
		if (m_Zone_Input_Name) m_Zone_Input_Name.SetText(cp.Name);
		if (m_Zone_Input_Coords) m_Zone_Input_Coords.SetText(coords);
		if (m_Zone_Input_Radius) m_Zone_Input_Radius.SetText(cp.Radius.ToString());
		if (m_Zone_Input_Points) m_Zone_Input_Points.SetText(cp.RewardPoints.ToString());
		
		// Set owner state/button text
		m_SelectedZoneOwner = cp.OwnerGroup;
		if (m_Zone_Btn_SelectOwner)
		{
			if (m_SelectedZoneOwner == "" || m_SelectedZoneOwner == "None")
				m_Zone_Btn_SelectOwner.SetText("None (No Owner)");
			else
				m_Zone_Btn_SelectOwner.SetText("Owner: " + m_SelectedZoneOwner);
		}
	}
	
	void OnZoneSave()
	{
		if (m_Zone_EditingName == "")
		{
			Print("[GPDA] No zone selected for editing. Click EDIT first.");
			return;
		}
		
		if (!m_Zone_Input_Name || !m_Zone_Input_Coords || !m_Zone_Input_Radius)
			return;
		
		string newName = m_Zone_Input_Name.GetText();
		string coords = m_Zone_Input_Coords.GetText();
		string radius = m_Zone_Input_Radius.GetText();
		string points = "100";
		if (m_Zone_Input_Points) points = m_Zone_Input_Points.GetText();
		
		if (newName == "" || coords == "" || radius == "")
		{
			Print("[GPDA] Save zone: fill all required fields");
			return;
		}
		
		array<string> coordsArray = new array<string>;
		coords.Split(" ", coordsArray);
		if (coordsArray.Count() != 3)
		{
			Print("[GPDA] Invalid coordinates format. Use: X Y Z");
			return;
		}
		
		float x = coordsArray[0].ToFloat();
		float y = coordsArray[1].ToFloat();
		float z = coordsArray[2].ToFloat();
		vector pos = Vector(x, y, z);
		float radiusF = radius.ToFloat();
		int pointsI = points.ToInt();
		
		string finalOwner = m_SelectedZoneOwner;
		if (!finalOwner) finalOwner = "";
		
		// Pack originalName|newName
		string packed = m_Zone_EditingName + "|" + newName;
		
		Print("[GPDA] Saving zone: " + m_Zone_EditingName + " -> " + newName + " radius=" + radius);
		
		ref Param5<string, vector, float, int, string> data = new Param5<string, vector, float, int, string>(packed, pos, radiusF, pointsI, finalOwner);
		GetRPCManager().SendRPC("GPDA_Capture", "AdminUpdateCapture", data, true);
		
		m_Zone_EditingName = "";
		
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestZonesFromServer, 1000, false);
	}
	
	void OnZoneDelete()
	{
		if (!m_Zone_ZoneList)
			return;
			
		int selected = m_Zone_ZoneList.GetSelectedRow();
		if (selected == -1)
		{
			Print("[GPDA] Please select a zone to delete");
			return;
		}
		
		string zoneName = "";
		
		// Prefer stored data
		if (m_CapturePointsData && selected < m_CapturePointsData.Count() && m_CapturePointsData[selected])
		{
			zoneName = m_CapturePointsData[selected].Name;
		}
		else
		{
			string rowText;
			m_Zone_ZoneList.GetItemText(selected, 0, rowText);
			TStringArray parts = new TStringArray;
			rowText.Split("|", parts);
			if (parts.Count() > 0)
			{
				zoneName = parts[0];
				zoneName.Trim();
			}
		}
		
		if (zoneName == "")
		{
			Print("[GPDA] Cannot delete zone - name not found");
			return;
		}
		
		Print("[GPDA] Deleting zone '" + zoneName + "' at index " + selected);
		
		// Send delete RPC
		ref Param1<string> data = new Param1<string>(zoneName);
		GetRPCManager().SendRPC("GPDA_Capture", "AdminDeleteCapture", data, true);
		
		// Request updated list after a short delay
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestZonesFromServer, 500, false);
	}
	
	void OnZoneSelected()
	{
		OnZoneEdit(); // Same as edit - load into form
	}
	
	void OnZoneSelectOwner()
	{
		Print("[GPDA] Zone owner selection - requesting groups list");
		
		// Reset selection mode first
		m_SelectingZoneOwner = true;
		
		// Show loading message immediately
		if (m_Zone_ZoneList)
		{
			m_Zone_ZoneList.ClearItems();
			m_Zone_ZoneList.AddItem("Loading groups...", null, 0);
		}
		
		// Request groups list from server
		Print("[GPDA] Sending RequestAllGroups RPC");
		GetRPCManager().SendRPC("GPDA_Groups", "RequestAllGroups", null, true);
	}
	
	void OnZoneOwnerSelected()
	{
		if (!m_Zone_ZoneList)
			return;
			
		// Only process if we're in selection mode
		if (!m_SelectingZoneOwner)
		{
			Print("[GPDA] OnZoneOwnerSelected called but not in selection mode - ignoring");
			return;
		}
			
		int selected = m_Zone_ZoneList.GetSelectedRow();
		if (selected == -1)
			return;
			
		string selectedText;
		m_Zone_ZoneList.GetItemText(selected, 0, selectedText);
		
		Print("[GPDA] Zone owner selected from list: " + selectedText);
		
		// Reset selection mode FIRST
		m_SelectingZoneOwner = false;
		
		if (selectedText == "None (No Owner)" || selectedText == "Loading groups..." || selectedText == "No groups available")
		{
			m_SelectedZoneOwner = "";
			if (m_Zone_Btn_SelectOwner)
				m_Zone_Btn_SelectOwner.SetText("None (No Owner)");
		}
		else
		{
			m_SelectedZoneOwner = selectedText;
			if (m_Zone_Btn_SelectOwner)
				m_Zone_Btn_SelectOwner.SetText("Owner: " + selectedText);
			Print("[GPDA] Zone owner set to: '" + m_SelectedZoneOwner + "'");
		}
		
		// Refresh zones list to show normal zones again
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(RequestZonesFromServer, 100, false);
		
		Print("[GPDA] Zone owner selected: " + m_SelectedZoneOwner);
	}
	
	void OnZoneSelectContainer()
	{
		// TODO: Open container type selection dialog
		Print("[GPDA] Zone container selection - TODO: Implement container selector");
		m_SelectedZoneContainer = ""; // Will be set when selector is implemented
		if (m_Zone_Btn_SelectContainer)
			m_Zone_Btn_SelectContainer.SetText("Select Container Type");
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// DATA UPDATE METHODS (CALLED FROM CLIENT RPC / MAIN PDA)
	// ═══════════════════════════════════════════════════════════════════════════
	
	// Update list of groups in admin UI
	void UpdateGroupsList(array<string> groups)
	{
		Print("[GPDA][AdminMenu] UpdateGroupsList called. Groups count = " + groups.Count().ToString() + ", m_SelectingZoneOwner = " + m_SelectingZoneOwner.ToString());
		m_AllGroups = groups;
		
		// If we're selecting zone owner, show groups in zone list
		if (m_SelectingZoneOwner && m_Zone_ZoneList)
		{
			m_Zone_ZoneList.ClearItems();
			m_Zone_ZoneList.AddItem("None (No Owner)", null, 0);
			
			if (groups && groups.Count() > 0)
			{
				foreach (string groupName : groups)
				{
					if (groupName != "")
						m_Zone_ZoneList.AddItem(groupName, null, 0);
				}
				Print("[GPDA][AdminMenu] Added " + groups.Count().ToString() + " groups to zone list");
			}
			else
			{
				m_Zone_ZoneList.AddItem("No groups available", null, 0);
				Print("[GPDA][AdminMenu] No groups available");
			}
			
			// Auto-reset selection mode after 30 seconds if user doesn't select
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(ResetZoneOwnerSelection, 30000, false);
		}
	}
	
	void ResetZoneOwnerSelection()
	{
		if (m_SelectingZoneOwner)
		{
			Print("[GPDA] Auto-resetting zone owner selection mode (timeout)");
			m_SelectingZoneOwner = false;
			RequestZonesFromServer();
		}
	}
	
	// Update list of all tickets (admin view)
	void UpdateAllTickets(array<ref GPDA_TicketInfo> tickets)
	{
		Print("[GPDA][AdminMenu] UpdateAllTickets called. Tickets count = " + tickets.Count().ToString());
		
		// Store tickets data
		m_TicketsData = tickets;
		
		if (!m_Ticket_TicketListContent)
			return;
		
		ClearTicketListItems();
		
		for (int i = 0; i < tickets.Count(); i++)
		{
			GPDA_TicketInfo ticket = tickets[i];
			string statusTag = "";
			if (ticket.Status == "open") statusTag = "[OPEN] ";
			else if (ticket.Status == "assigned") statusTag = "[ASSIGNED] ";
			else if (ticket.Status == "closed") statusTag = "[CLOSED] ";
			
			string row = statusTag + ticket.Title + " | " + ticket.PlayerName;
			CreateTicketListItem(row, i);
		}
	}
	
	void ClearTicketListItems()
	{
		if (m_TicketListItems)
		{
			foreach (Widget w : m_TicketListItems)
			{
				if (w) w.Unlink();
			}
			m_TicketListItems.Clear();
		}
		m_SelectedTicketIndex = -1;
	}
	
	void CreateTicketListItem(string text, int index)
	{
		if (!m_Ticket_TicketListContent) return;
		
		Widget item = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/ticket_item.layout", m_Ticket_TicketListContent);
		if (!item) return;
		
		TextWidget txt = TextWidget.Cast(item.FindAnyWidget("TicketItemText"));
		if (txt) txt.SetText(text);
		
		m_TicketListItems.Insert(item);
	}
	
	// Update messages for specific ticket
	void UpdateTicketMessages(string ticketId, array<ref GPDA_TicketMessageInfo> messages)
	{
		Print("[GPDA][AdminMenu] UpdateTicketMessages called for ticket " + ticketId + ", messages = " + messages.Count().ToString());
		
		if (!m_Ticket_TicketDescription)
			return;
		
		// Build description from messages
		string description = "";
		if (messages && messages.Count() > 0)
		{
			foreach (GPDA_TicketMessageInfo msg : messages)
			{
				if (description != "") description += "\n\n";
				string prefix = "[USER] ";
				if (msg.IsAdmin)
					prefix = "[ADMIN] ";
				description += prefix + msg.SenderName + ": " + msg.Message;
			}
		}
		
		if (description == "")
		{
			// Try to get initial description from ticket data
			if (m_TicketsData)
			{
				foreach (GPDA_TicketInfo ticket : m_TicketsData)
				{
					if (ticket.Id == ticketId)
					{
						description = "Initial request - see messages below";
						break;
					}
				}
			}
			if (description == "")
				description = "No messages yet";
		}
		
		m_Ticket_TicketDescription.SetText(description);
		
		// Make content taller than viewport so ScrollWidget can scroll
		int lineCount = 1;
		TStringArray lines = new TStringArray;
		description.Split("\n", lines);
		if (lines && lines.Count() > 0)
			lineCount = lines.Count();
		
		float targetHeight = Math.Max(180.0, (float)lineCount * 16.0);
		float curW;
		float curH;
		m_Ticket_TicketDescription.GetSize(curW, curH);
		m_Ticket_TicketDescription.SetSize(curW, targetHeight);
	}
	
	// Request refresh of ticket messages from client-side notification
	void RequestTicketMessages(string ticketId)
	{
		Print("[GPDA][AdminMenu] RequestTicketMessages called for ticket " + ticketId);
	}
	
	// Update capture points list in admin UI
	void UpdateCapturePoints(array<ref GPDA_CapturePointInfo> points)
	{
		Print("[GPDA][AdminMenu] UpdateCapturePoints called. Points count = " + points.Count().ToString());
		
		// Store data for edit/delete
		m_CapturePointsData = points;
		
		if (!m_Zone_ZoneList)
			return;
		
		// Don't update if we're selecting zone owner (showing groups list)
		if (m_SelectingZoneOwner)
		{
			Print("[GPDA][AdminMenu] Skipping zone list update - selecting owner");
			return;
		}
		
		m_Zone_ZoneList.ClearItems();
		for (int i = 0; i < points.Count(); i++)
		{
			GPDA_CapturePointInfo point = points[i];
			string row = point.Name + " | " + point.OwnerGroup + " | " + point.RewardPoints.ToString();
			m_Zone_ZoneList.AddItem(row, null, 0);
		}
	}
	
	// Update ranks list in admin UI
	void UpdateRanksList(array<ref GPDA_RankEntry> ranks)
	{
		Print("[GPDA][AdminMenu] UpdateRanksList called. Ranks count = " + ranks.Count().ToString());
		
		// Store ranks data
		m_RanksData = ranks;
		
		if (!m_Rank_RankList)
			return;
		
		m_Rank_RankList.ClearItems();
		for (int i = 0; i < ranks.Count(); i++)
		{
			GPDA_RankEntry rank = ranks[i];
			string row = rank.Name + " | " + rank.RequiredXP.ToString() + " | " + rank.Order.ToString();
			m_Rank_RankList.AddItem(row, null, 0);
		}
	}
}
