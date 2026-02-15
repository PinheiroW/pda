// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Main PDA Menu
// ═══════════════════════════════════════════════════════════════════════════════

const int GPDA_SORT_PLAYERS = 1;
const int GPDA_SORT_ZOMBIES = 2;
const int GPDA_SORT_TOTAL = 3;

static string ICON_CONNECT = "set:dayz_gui image:icon_connect";

class GPDA_Menu extends UIScriptedMenu
{
	protected static ref GPDA_Menu s_Instance;
	protected static bool s_IsOpen = false;
	
	protected const string LAYOUT_PATH = "Greshnik_PDALITE/GUI/Layouts/pda_main.layout";
	
	// Header widgets
	protected TextWidget m_HeaderUserName;
	protected TextWidget m_HeaderUserFaction;
	protected string m_HeaderUserFactionValue;
	
	// Tab buttons
	protected ButtonWidget m_TabMap;
	protected ButtonWidget m_TabChat;
	protected ButtonWidget m_TabPM;
	protected ButtonWidget m_TabProfile;
	protected ButtonWidget m_TabTop;
	protected ButtonWidget m_TabGroups;
	protected ButtonWidget m_TabDatabase;
	protected ButtonWidget m_TabAdmins;
	
	// Panels
	protected Widget m_PanelMap;
	protected Widget m_PanelChat;
	protected Widget m_PanelPM;
	protected Widget m_PanelProfile;
	protected Widget m_PanelTop;
	protected Widget m_PanelGroups;
	protected Widget m_PanelDatabase;
	protected Widget m_PanelAdmins;
	protected Widget m_PanelSupport;
	
	// Map widgets
	protected MapWidget m_MapWidget;
	protected TextWidget m_MapCoords;
	protected TextWidget m_PlayerPosLabel;
	protected ButtonWidget m_BtnZoomIn;
	protected ButtonWidget m_BtnZoomOut;
	protected ButtonWidget m_BtnCenter;
	protected ButtonWidget m_BtnAutoFollow;
	protected ButtonWidget m_BtnAddMarker;
	
	protected Widget m_HeaderRoot;
	
	// Chat widgets
	protected ScrollWidget m_ChatScroll;
	protected WrapSpacerWidget m_ChatContent;
	protected ref array<Widget> m_ChatItems;
	protected EditBoxWidget m_ChatInput;
	protected ButtonWidget m_BtnSendChat;
	protected ButtonWidget m_BtnAnonChat;
	protected TextWidget m_BtnAnonChatText;
	protected ButtonWidget m_BtnNotifyToggle;
	protected TextWidget m_BtnNotifyToggleText;
	protected bool m_ChatNotifyEnabled;
	protected bool m_AnonChatEnabled;
	protected int m_LastCapturePointsRequest;
	
	protected Widget m_Sidebar;
	protected Widget m_ContentArea;
	protected bool m_AuthGate;
	
	protected Widget m_AuthPanel;
	protected EditBoxWidget m_AuthLoginInput;
	protected EditBoxWidget m_AuthPasswordInput;
	protected ButtonWidget m_AuthBtnLogin;
	protected ButtonWidget m_AuthBtnRegister;
	protected ButtonWidget m_AuthBtnLogout;
	protected TextWidget m_AuthStatusText;
	protected bool m_IsLoggedIn;
	protected string m_LoginName;
	protected ref array<ref GPDA_CapturePointInfo> m_MapCapturePoints;
	protected int m_LastMapMarksUpdate;
	
	// PM widgets
	protected EditBoxWidget m_PMRecipient;
	protected TextListboxWidget m_PMMessages;
	protected EditBoxWidget m_PMInput;
	protected ButtonWidget m_BtnSendPM;
	
	// Profile widgets
	protected TextWidget m_ValNickname;
	protected TextWidget m_ValKillsPlayers;
	protected TextWidget m_ValKillsZombies;
	protected TextWidget m_ValKillsAnimals;
	protected TextWidget m_ValDeaths;
	protected TextWidget m_ValPlaytime;
	protected TextWidget m_ValKD;
	protected TextWidget m_ValTotalKills;
	protected TextWidget m_ProfileGroup;
	protected TextWidget m_ProfileBio;
	
	// License widgets (Profile)
	protected TextWidget m_License1Status;
	protected TextWidget m_License2Status;
	protected TextWidget m_License3Status;
	protected TextWidget m_License4Status;
	protected TextWidget m_License5Status;
	protected ImageWidget m_License1Indicator;
	protected ImageWidget m_License2Indicator;
	protected ImageWidget m_License3Indicator;
	protected ImageWidget m_License4Indicator;
	protected ImageWidget m_License5Indicator;
	
	// Profile Rank widgets (new)
	protected TextWidget m_ProfileRankName;
	protected TextWidget m_ProfileXPText;
	protected ImageWidget m_ProfileXPBarFill;
	protected TextWidget m_ProfileNextRank;
	
	// Top100 widgets
	protected TextListboxWidget m_TopList;
	protected ButtonWidget m_BtnSortPlayers;
	protected ButtonWidget m_BtnSortZombies;
	protected ButtonWidget m_BtnSortTotal;
	protected int m_CurrentSort;
	
	// Groups widgets
	protected TextWidget m_GroupName;
	protected EditBoxWidget m_Steam64Input;
	protected ButtonWidget m_BtnAddMember;
	protected ButtonWidget m_BtnKickMember;
	protected TextListboxWidget m_MembersList;
	protected TextWidget m_GroupOnlineText;
	protected TextWidget m_GroupOnlineFooterText;
	protected ref array<string> m_MemberSteamIDs;
	
	// Database widgets (Military only)
	protected EditBoxWidget m_SearchInput;
	protected ButtonWidget m_BtnSearch;
	protected TextListboxWidget m_SearchResultsList;
	protected ref array<string> m_SearchResultSteamIDs;
	protected TextWidget m_DossierCallsign;
	protected TextWidget m_DossierSteam64;
	protected TextWidget m_DossierAge;
	protected TextWidget m_DossierGroup;
	protected TextWidget m_DossierBio;
	protected TextWidget m_LicenseHuntingStatus;
	protected TextWidget m_LicenseFishingStatus;
	protected TextWidget m_LicenseZoneStatus;
	protected TextWidget m_LicenseWeaponsStatus;
	protected TextWidget m_LicenseTradingStatus;
	protected ButtonWidget m_BtnLicenseHunting;
	protected ButtonWidget m_BtnLicenseFishing;
	protected ButtonWidget m_BtnLicenseZone;
	protected ButtonWidget m_BtnLicenseWeapons;
	protected ButtonWidget m_BtnLicenseTrading;
	protected string m_SelectedPlayerSteam64;
	protected bool m_HasDatabaseAccess;
	
	// Quests removed - moved to separate system
	
	// Rank widgets
	protected ButtonWidget m_TabRank;
	protected Widget m_PanelRank;
	protected TextWidget m_CurrentRankName;
	protected TextWidget m_CurrentXPValue;
	protected TextWidget m_NextRankName;
	protected ImageWidget m_RankProgressFill;
	protected TextWidget m_RankProgressText;
	protected ref GPDA_RankData m_RankData;
	
	// Support widgets
	protected ButtonWidget m_TabSupport;
	protected EditBoxWidget m_TicketTitleInput;
	protected MultilineEditBoxWidget m_TicketDescInput;
	protected ButtonWidget m_BtnCreateTicket;
	protected ScrollWidget m_MyTicketsScroll;
	protected WrapSpacerWidget m_MyTicketsContent;
	protected ref array<Widget> m_MyTicketItemWidgets;
	protected ScrollWidget m_TicketChatScroll;
	protected WrapSpacerWidget m_TicketChatContent;
	protected ref array<Widget> m_TicketChatItemWidgets;
	protected EditBoxWidget m_TicketChatInput;
	protected ButtonWidget m_BtnSendTicketMsg;
	protected ref array<ref GPDA_TicketInfo> m_MyTicketsData;
	protected string m_SelectedTicketID;
	
	// Admins tab (Tickets admin + Rank editor + Capture)
	protected Widget m_AdminsPanelTickets;
	protected Widget m_AdminsPanelRanks;
	protected Widget m_AdminsPanelCapture;
	protected ButtonWidget m_AdminsTabTickets;
	protected ButtonWidget m_AdminsTabRanks;
	protected ButtonWidget m_AdminsTabCapture;
	protected TextListboxWidget m_AdminsTicketsList;
	protected TextListboxWidget m_AdminsTicketChat;
	protected MultilineEditBoxWidget m_AdminsTicketReplyInput;
	protected ButtonWidget m_AdminsTicketReplySend;
	protected ButtonWidget m_AdminsTicketClose;
	protected EditBoxWidget m_AdminsRankName;
	protected EditBoxWidget m_AdminsRankXP;
	// simplified rank creation (name + xp only)
	protected ButtonWidget m_AdminsRankSave;
	protected TextListboxWidget m_AdminsRanksList;
	protected ref array<ref GPDA_TicketInfo> m_AdminTicketsData;
	// Admins Capture (Zone creation)
	protected EditBoxWidget m_AdminsZoneName;
	protected EditBoxWidget m_AdminsZoneRadius;
	protected EditBoxWidget m_AdminsZonePoints;
	protected EditBoxWidget m_AdminsZoneOwner;
	protected TextWidget m_AdminsZoneCoords;
	protected ButtonWidget m_AdminsBtnGetPos;
	protected ButtonWidget m_AdminsBtnCreateZone;
	protected TextListboxWidget m_AdminsZonesList;
	protected ButtonWidget m_AdminsBtnDeleteZone;
	
	// Groups - Points and Zones
	protected TextWidget m_GroupPointsValue;
	protected TextListboxWidget m_GroupZonesList;
	
	// Capture Zones removed - moved to GPDA_AdminMenu
	
	protected ButtonWidget m_BtnClose;
	
	protected int m_ActiveTabColor;
	protected int m_InactiveTabColor;
	
	protected bool m_HasGPS;
	protected bool m_AutoFollow;
	
	// Flash card slot states
	protected bool m_HasMapCard;      // Card A - Map
	protected bool m_HasCaptureCard;  // Card B - Capture
	protected bool m_HasGroupsCard;   // Card C - Groups
	protected bool m_HasSupportCard;  // Card D - SOS/Support
	protected bool m_HasGPSCard;      // Card E - GPS
	protected bool m_HasDatabaseCard; // Card F - Database
	
	// Marker creation
	protected Widget m_MarkerPanel;
	protected EditBoxWidget m_MarkerNameInput;
	protected ButtonWidget m_MarkerColorRed;
	protected ButtonWidget m_MarkerColorGreen;
	protected ButtonWidget m_MarkerColorBlue;
	protected ButtonWidget m_MarkerColorYellow;
	protected ButtonWidget m_MarkerColorWhite;
	protected ButtonWidget m_MarkerCreate;
	protected ButtonWidget m_MarkerCancel;
	protected vector m_PendingMarkerPos;
	protected int m_SelectedMarkerColor;
	protected int m_LastClickTime;
	protected ref array<ref Param3<vector, int, string>> m_UserMarkers;
	
	// Header info
	protected TextWidget m_OnlineStatus;
	protected TextWidget m_ServerTime;
	protected float m_HeaderUpdateTimer;
	
	void GPDA_Menu()
	{
		// ECHO PDA style colors
		m_ActiveTabColor = ARGB(64, 30, 128, 80);   // Green-tinted (selected)
		m_InactiveTabColor = ARGB(255, 15, 18, 20); // Dark (unselected)
		m_AutoFollow = false;  // Disabled by default so user can move map freely
		m_SelectedMarkerColor = ARGB(255, 255, 50, 50);
		m_IsLoggedIn = false;
		m_LoginName = "";
		m_MapCapturePoints = new array<ref GPDA_CapturePointInfo>;
		m_LastMapMarksUpdate = 0;
		m_AuthGate = true;
		m_HeaderUpdateTimer = 0;
		m_UserMarkers = new array<ref Param3<vector, int, string>>;
		
		// Initialize arrays
		m_MyTicketsData = new array<ref GPDA_TicketInfo>;
		m_MyTicketItemWidgets = new array<Widget>;
		m_TicketChatItemWidgets = new array<Widget>;
		m_ChatItems = new array<Widget>;
		m_MemberSteamIDs = new array<string>;
		m_SearchResultSteamIDs = new array<string>;
	}

	protected void ClearWidgetList(array<Widget> items)
	{
		if (!items) return;
		foreach (Widget w : items)
		{
			if (w) w.Unlink();
		}
		items.Clear();
	}

	protected Widget ResolveTicketItemRoot(Widget w)
	{
		Widget cur = w;
		while (cur)
		{
			if (cur.GetName() == "TicketItemRoot")
				return cur;
			cur = cur.GetParent();
		}
		return null;
	}

	protected void LoadNotifyPref()
	{
		m_ChatNotifyEnabled = true;
		if (g_GPDA_ClientPrefsManager)
		{
			GPDA_ClientPrefs prefs = g_GPDA_ClientPrefsManager.GetPrefs();
			if (prefs)
				m_ChatNotifyEnabled = prefs.ChatNotifyEnabled;
		}
	}
	
	protected void ToggleNotify()
	{
		m_ChatNotifyEnabled = !m_ChatNotifyEnabled;
		if (g_GPDA_ClientPrefsManager)
			g_GPDA_ClientPrefsManager.SetChatNotifyEnabled(m_ChatNotifyEnabled);
		UpdateNotifyButton();
	}
	
	protected void UpdateNotifyButton()
	{
		if (!m_BtnNotifyToggle) return;
		if (m_ChatNotifyEnabled)
		{
			m_BtnNotifyToggle.SetColor(ARGB(255, 40, 160, 90));
			if (m_BtnNotifyToggleText) m_BtnNotifyToggleText.SetColor(ARGB(255, 255, 255, 255));
		}
		else
		{
			m_BtnNotifyToggle.SetColor(ARGB(255, 64, 64, 72));
			if (m_BtnNotifyToggleText) m_BtnNotifyToggleText.SetColor(ARGB(255, 190, 190, 190));
		}
	}
	
	static bool IsChatNotifyEnabled()
	{
		if (!g_GPDA_ClientPrefsManager) return true;
		GPDA_ClientPrefs prefs = g_GPDA_ClientPrefsManager.GetPrefs();
		if (!prefs) return true;
		return prefs.ChatNotifyEnabled;
	}
	
	static void Toggle()
	{
		if (s_IsOpen)
			ClosePDA();
		else
			Open();
	}
	
	static void Open()
	{
		if (s_IsOpen) return;
		
		// Use EnterScriptedMenu (works with CreateScriptedMenu in MissionGameplay)
		GetGame().GetUIManager().EnterScriptedMenu(MENU_GPDA_MAIN, null);
		s_IsOpen = true;
	}
	
	static void ClosePDA()
	{
		if (!s_IsOpen) return;
		
		if (s_Instance)
			s_Instance.CloseMenu();
	}
	
	static bool IsOpen()
	{
		return s_IsOpen;
	}
	
	override Widget Init()
	{
		s_Instance = this;
		s_IsOpen = true;
		
		layoutRoot = GetGame().GetWorkspace().CreateWidgets(LAYOUT_PATH);
		if (!layoutRoot)
		{
			Print("[GPDA] ERROR: Failed to load layout!");
			return null;
		}
		
		CacheWidgets();
		
		// Check flash card slots first, then GPS
		CheckFlashCards();
		CheckForGPS();
		
		// Check if player is in a group, hide Groups tab if not
		UpdateGroupsTabVisibility();
		
		// Apply flash card visibility restrictions to tabs
		ApplyCardRestrictions();
		
		m_CurrentSort = GPDA_SORT_TOTAL;
		ShowAuthGate(true);
		
		UpdateHeader();
		RequestAuthStatus();
		RequestProfile();
		RequestPlayerRank();
		RequestDatabaseAccess();
		RequestCapturePointsForMap();
		
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetUIManager().ShowCursor(true);
		GetGame().GetInput().ChangeGameFocus(1);
		
		Print("[GPDA] Menu opened");
		return layoutRoot;
	}
	
	protected void ShowAuthGate(bool enabled)
	{
		m_AuthGate = enabled;
		if (m_HeaderRoot)
			m_HeaderRoot.Show(!enabled);
		if (m_Sidebar)
			m_Sidebar.Show(!enabled);
		if (m_AuthPanel)
			m_AuthPanel.Show(enabled);
		
		if (enabled)
		{
			// Hide all content panels
			if (m_PanelMap) m_PanelMap.Show(false);
			if (m_PanelChat) m_PanelChat.Show(false);
			if (m_PanelPM) m_PanelPM.Show(false);
			if (m_PanelProfile) m_PanelProfile.Show(false);
			if (m_PanelTop) m_PanelTop.Show(false);
			if (m_PanelGroups) m_PanelGroups.Show(false);
			if (m_PanelDatabase) m_PanelDatabase.Show(false);
			if (m_PanelRank) m_PanelRank.Show(false);
			if (m_PanelSupport) m_PanelSupport.Show(false);
		}
		else
		{
			ShowTab("chat");
		}
	}
	
	// Check if player is in a group and show/hide Groups tab
	void UpdateGroupsTabVisibility()
	{
		if (!m_TabGroups) return;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !player.GetIdentity())
		{
			m_TabGroups.Show(false);
			return;
		}
		
		string steam64 = player.GetIdentity().GetPlainId();
		
		// Request group check from server via RPC
		ref Param1<string> data = new Param1<string>(steam64);
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Groups", "CheckPlayerGroup", data, true, null, playerObj);
	}
	
	// Called from RPC when group check response arrives
	void OnGroupCheckReceived(bool hasGroup, string groupName)
	{
		if (m_TabGroups)
		{
			// Groups tab requires both: being in a group AND having Card C
			m_TabGroups.Show(hasGroup && m_HasGroupsCard);
		}
		
		if (hasGroup && m_HeaderUserFaction)
		{
			m_HeaderUserFaction.SetText(groupName);
			m_HeaderUserFactionValue = groupName;
		}
		else if (m_HeaderUserFaction)
		{
			m_HeaderUserFaction.SetText("Loner");
			m_HeaderUserFactionValue = "Loner";
		}
	}
	
	void RequestAuthStatus()
	{
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Accounts", "RequestStatus", null, true, null, playerObj);
	}
	
	void SetAuthStatus(bool loggedIn, string login)
	{
		m_IsLoggedIn = loggedIn;
		m_LoginName = login;
		if (m_AuthStatusText)
		{
			if (m_IsLoggedIn)
				m_AuthStatusText.SetText("Logged in as: " + m_LoginName);
			else
				m_AuthStatusText.SetText("Not logged in");
		}
		UpdateHeader();
		ShowAuthGate(!m_IsLoggedIn);
	}
	
	void OnAuthResponse(bool success, string message, string login)
	{
		if (success)
		{
			m_IsLoggedIn = true;
			m_LoginName = login;
		}
		if (m_AuthStatusText)
		{
			m_AuthStatusText.SetText(message);
		}
		UpdateHeader();
		ShowAuthGate(!m_IsLoggedIn);
		if (m_IsLoggedIn)
			RequestPlayerRank();
		RequestCapturePointsForMap();
	}
	
	protected void RequestPlayerRank()
	{
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Rank", "RequestPlayerRank", null, true, null, playerObj);
	}
	
	protected void RequestCapturedZones()
	{
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Groups", "RequestCapturedZones", null, true, null, playerObj);
	}
	
	protected void OnAuthLogin()
	{
		if (!m_AuthLoginInput || !m_AuthPasswordInput) return;
		string login = m_AuthLoginInput.GetText();
		string pass = m_AuthPasswordInput.GetText();
		login.Trim();
		pass.Trim();
		Print("[GPDA][AUTH] Login click: login='" + login + "' passLen=" + pass.Length().ToString());
		ref Param2<string, string> data = new Param2<string, string>(login, pass);
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Accounts", "Login", data, true, null, playerObj);
	}
	
	protected void OnAuthRegister()
	{
		if (!m_AuthLoginInput || !m_AuthPasswordInput) return;
		string login = m_AuthLoginInput.GetText();
		string pass = m_AuthPasswordInput.GetText();
		login.Trim();
		pass.Trim();
		Print("[GPDA][AUTH] Register click: login='" + login + "' passLen=" + pass.Length().ToString());
		ref Param2<string, string> data = new Param2<string, string>(login, pass);
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Accounts", "Register", data, true, null, playerObj);
	}
	
	protected void OnAuthLogout()
	{
		Print("[GPDA][AUTH] Logout click");
		Object playerObj = GetGame().GetPlayer();
		if (playerObj)
			GetRPCManager().SendRPC("GPDA_Accounts", "Logout", null, true, null, playerObj);
		m_IsLoggedIn = false;
		m_LoginName = "";
		UpdateHeader();
		ShowAuthGate(true);
	}
	
	void RequestCapturePointsForMap()
	{
		int now = GetGame().GetTime();
		if (m_LastCapturePointsRequest != 0 && (now - m_LastCapturePointsRequest) < 15000)
			return;
		m_LastCapturePointsRequest = now;
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Capture", "RequestCapturePoints", null, true, null, playerObj);
	}
	
	void UpdateCapturePointsForMap(array<ref GPDA_CapturePointInfo> points)
	{
		m_MapCapturePoints = points;
		m_LastMapMarksUpdate = 0;
	}
	
	protected void UpdateMapMarks()
	{
		if (!m_MapWidget) return;
		int now = GetGame().GetTime();
		if (m_LastMapMarksUpdate != 0 && (now - m_LastMapMarksUpdate) < 15000)
			return;
		m_LastMapMarksUpdate = now;
		m_MapWidget.ClearUserMarks();
		if (m_HasGPS)
		{
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player)
			{
				vector pos = player.GetPosition();
				m_MapWidget.AddUserMark(pos, "YOU", ARGB(255, 50, 255, 50), "\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
			}
		}
		// Capture zones on map only show if Capture card (B) is inserted
		if (m_HasCaptureCard)
			RenderCaptureZonesOnMap();
		
		// User markers
		if (m_UserMarkers)
		{
			string mIcon = "set:dayz_gui image:icon_pin";
			foreach (Param3<vector, int, string> um : m_UserMarkers)
			{
				if (!um) continue;
				m_MapWidget.AddUserMark(um.param1, um.param3, um.param2, mIcon);
			}
		}
	}
	
	protected void RenderCaptureZonesOnMap()
	{
		if (!m_MapWidget || !m_MapCapturePoints) return;
		
		// Colors: WHITE = not captured, RED = captured
		int colorOwned = ARGB(220, 220, 40, 40);      // Bright red for owned
		int colorNone = ARGB(200, 255, 255, 255);     // Pure white for neutral
		
		// Smooth circle like MutantSpawnerUI DrawRadiusCircle
		int segments = 200;
		float step = Math.PI * 2.0 / segments;
		
		foreach (GPDA_CapturePointInfo cp : m_MapCapturePoints)
		{
			if (!cp) continue;
			string owner = cp.OwnerGroup;
			bool isOwned = (owner != "" && owner != "None" && owner != "---");
			
			int zoneColor = colorNone;
			if (isOwned)
				zoneColor = colorOwned;
			
			// Center marker with zone name
			string label = cp.Name;
			if (isOwned)
				label = cp.Name + " [" + owner + "]";
			m_MapWidget.AddUserMark(cp.Position, label, zoneColor, "set:dayz_gui image:icon_pin");
			
			// Draw smooth circle (200 dots like spawner)
			float radius = cp.Radius;
			for (int i = 0; i < segments; i++)
			{
				float angle = step * i;
				float x = cp.Position[0] + (Math.Cos(angle) * radius);
				float z = cp.Position[2] + (Math.Sin(angle) * radius);
				vector p = Vector(x, cp.Position[1], z);
				m_MapWidget.AddUserMark(p, ".", zoneColor, "");
			}
		}
	}
	
	protected void CacheWidgets()
	{
		m_BtnClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnClose"));
		m_HeaderRoot = layoutRoot.FindAnyWidget("GPDA_Header");
		m_Sidebar = layoutRoot.FindAnyWidget("GPDA_Sidebar");
		m_ContentArea = layoutRoot.FindAnyWidget("GPDA_ContentArea");
		
		// Header widgets
		m_HeaderUserName = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_UserName"));
		m_HeaderUserFaction = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_UserFaction"));
		
		// Sidebar tabs
		m_TabMap = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Map"));
		m_TabChat = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Chat"));
		m_TabPM = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_PM"));
		m_TabProfile = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Profile"));
		m_TabTop = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Top100"));
		m_TabGroups = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Groups"));
		m_TabDatabase = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Database"));
		m_TabAdmins = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Admins"));
		
		// Content panels
		m_PanelMap = layoutRoot.FindAnyWidget("GPDA_Panel_Map");
		m_PanelChat = layoutRoot.FindAnyWidget("GPDA_Panel_Chat");
		m_PanelPM = layoutRoot.FindAnyWidget("GPDA_Panel_PM");
		m_PanelProfile = layoutRoot.FindAnyWidget("GPDA_Panel_Profile");
		m_PanelTop = layoutRoot.FindAnyWidget("GPDA_Panel_Top100");
		m_PanelGroups = layoutRoot.FindAnyWidget("GPDA_Panel_Groups");
		m_PanelDatabase = layoutRoot.FindAnyWidget("GPDA_Panel_Database");
		m_PanelSupport = layoutRoot.FindAnyWidget("GPDA_Panel_Support");
		// Admin widgets moved to GPDA_AdminMenu - no longer cached here
		
		// Groups zones/points widgets
		m_GroupPointsValue = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_GroupPointsValue"));
		m_GroupZonesList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_GroupZonesList"));
		
		// Map widgets
		m_MapWidget = MapWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MapWidget"));
		m_PlayerPosLabel = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_GPSCoords"));
		m_BtnAutoFollow = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Btn_AutoFollow"));
		m_BtnAddMarker = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Btn_AddMarker"));
		
		// Marker panel
		m_MarkerPanel = layoutRoot.FindAnyWidget("GPDA_MarkerPanel");
		m_MarkerNameInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerNameInput"));
		m_MarkerColorRed = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerColorRed"));
		m_MarkerColorGreen = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerColorGreen"));
		m_MarkerColorBlue = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerColorBlue"));
		m_MarkerColorYellow = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerColorYellow"));
		m_MarkerColorWhite = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerColorWhite"));
		m_MarkerCreate = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerCreate"));
		m_MarkerCancel = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MarkerCancel"));
		
		// Header info
		m_OnlineStatus = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_OnlineStatus"));
		m_ServerTime = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ServerTime"));
		
		// Chat widgets
		m_ChatScroll = ScrollWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ChatScroll"));
		m_ChatContent = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ChatContent"));
		m_ChatInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ChatInput"));
		m_BtnSendChat = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnSendChat"));
		m_BtnAnonChat = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnAnonChat"));
		m_BtnAnonChatText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnAnonChatText"));
		m_BtnNotifyToggle = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnNotifyToggle"));
		m_BtnNotifyToggleText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnNotifyToggleText"));
		m_AnonChatEnabled = false;
		UpdateAnonChatButton();
		LoadNotifyPref();
		UpdateNotifyButton();
		
		m_AuthPanel = layoutRoot.FindAnyWidget("GPDA_AuthPanel");
		m_AuthLoginInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_AuthLogin"));
		m_AuthPasswordInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_AuthPassword"));
		m_AuthBtnLogin = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_AuthBtnLogin"));
		m_AuthBtnRegister = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_AuthBtnRegister"));
		m_AuthBtnLogout = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_AuthBtnLogout"));
		m_AuthStatusText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_AuthStatus"));
		
		// PM widgets
		m_PMRecipient = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_PMRecipient"));
		m_PMMessages = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_PMList"));
		m_PMInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_PMMessage"));
		m_BtnSendPM = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnSendPM"));
		
		// Profile widgets
		m_ValNickname = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ProfileNickname"));
		m_ValKillsPlayers = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_StatBox1Value"));
		m_ValKillsZombies = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_StatBox2Value"));
		m_ValKD = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_StatBox3Value"));
		m_ValPlaytime = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_StatBox4Value"));
		m_ProfileGroup = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ProfileGroup"));
		m_ProfileBio = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ProfileBio"));
		
		// License widgets (Profile)
		m_License1Status = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License1Status"));
		m_License2Status = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License2Status"));
		m_License3Status = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License3Status"));
		m_License4Status = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License4Status"));
		m_License5Status = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License5Status"));
		m_License1Indicator = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License1Indicator"));
		m_License2Indicator = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License2Indicator"));
		m_License3Indicator = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License3Indicator"));
		m_License4Indicator = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License4Indicator"));
		m_License5Indicator = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_License5Indicator"));
		
		// Profile Rank widgets (new)
		m_ProfileRankName = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ProfileRankName"));
		m_ProfileXPText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ProfileXPText"));
		m_ProfileXPBarFill = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ProfileXPBarFill"));
		m_ProfileNextRank = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_ProfileNextRank"));
		
		// Top100 widgets
		m_TopList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Top100List"));
		m_BtnSortPlayers = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnSortPlayers"));
		m_BtnSortZombies = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnSortZombies"));
		m_BtnSortTotal = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnSortTotal"));
		
		// Groups widgets
		m_GroupName = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_GroupValueName"));
		m_Steam64Input = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Steam64Input"));
		m_BtnAddMember = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnAddMember"));
		m_BtnKickMember = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnKickMember"));
		m_MembersList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MembersList"));
		m_GroupOnlineText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_GroupOnlineText"));
		m_GroupOnlineFooterText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_GroupOnlineFooterText"));
		
		// Database widgets
		m_SearchInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_SearchInput"));
		m_BtnSearch = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnSearch"));
		m_SearchResultsList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_SearchResultsList"));
		m_DossierCallsign = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_DossierCallsign"));
		m_DossierSteam64 = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_DossierSteam64"));
		m_DossierAge = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_DossierAge"));
		m_DossierGroup = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_DossierGroup"));
		m_DossierBio = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_DossierBio"));
		m_LicenseHuntingStatus = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_LicenseHuntingStatus"));
		m_LicenseFishingStatus = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_LicenseFishingStatus"));
		m_LicenseZoneStatus = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_LicenseZoneStatus"));
		m_LicenseWeaponsStatus = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_LicenseWeaponsStatus"));
		m_LicenseTradingStatus = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_LicenseTradingStatus"));
		m_BtnLicenseHunting = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnLicenseHunting"));
		m_BtnLicenseFishing = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnLicenseFishing"));
		m_BtnLicenseZone = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnLicenseZone"));
		m_BtnLicenseWeapons = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnLicenseWeapons"));
		m_BtnLicenseTrading = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnLicenseTrading"));
		
		// Quests widgets removed
		
		// Rank widgets
		m_TabRank = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Rank"));
		m_PanelRank = layoutRoot.FindAnyWidget("GPDA_Panel_Rank");
		m_CurrentRankName = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_CurrentRankName"));
		m_CurrentXPValue = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_CurrentXPValue"));
		m_NextRankName = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_NextRankName"));
		m_RankProgressFill = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_RankProgressFill"));
		m_RankProgressText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_RankProgressText"));
		
		// Support widgets
		m_TabSupport = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_Support"));
		m_PanelSupport = layoutRoot.FindAnyWidget("GPDA_Panel_Support");
		m_TicketTitleInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_TicketTitleInput"));
		m_TicketDescInput = MultilineEditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_TicketDescInput"));
		m_BtnCreateTicket = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnCreateTicket"));
		m_MyTicketsScroll = ScrollWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MyTicketsScroll"));
		m_MyTicketsContent = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("GPDA_MyTicketsContent"));
		m_TicketChatScroll = ScrollWidget.Cast(layoutRoot.FindAnyWidget("GPDA_TicketChatScroll"));
		m_TicketChatContent = WrapSpacerWidget.Cast(layoutRoot.FindAnyWidget("GPDA_TicketChatContent"));
		m_TicketChatInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("GPDA_TicketChatInput"));
		m_BtnSendTicketMsg = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_BtnSendTicketMsg"));
		
				
		// Admin widgets moved to GPDA_AdminMenu - no longer cached here
		
		// Init data
		m_RankData = new GPDA_RankData();
		
		// Set initial state of AUTO button (off by default)
		if (m_BtnAutoFollow)
			m_BtnAutoFollow.SetColor(m_InactiveTabColor);
	}
	
	protected void CheckForGPS()
	{
		m_HasGPS = false;
		
		// GPS only works if GPS card (Card E) is inserted in the PDA
		if (!m_HasGPSCard)
		{
			UpdateGPSStatus();
			return;
		}
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			// With GPS card inserted, GPS always works (no need for separate GPSReceiver item)
			m_HasGPS = true;
		}
		
		UpdateGPSStatus();
	}
	
	protected void UpdateGPSStatus()
	{
		if (m_PlayerPosLabel)
		{
			if (m_HasGPS)
			{
				PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
				if (player)
				{
					vector pos = player.GetPosition();
					int px = Math.Round(pos[0]);
					int pz = Math.Round(pos[2]);
					m_PlayerPosLabel.SetText("GPS: " + px.ToString() + ", " + pz.ToString());
					m_PlayerPosLabel.SetColor(ARGB(255, 50, 230, 50));
				}
			}
			else
			{
				if (!m_HasGPSCard)
					m_PlayerPosLabel.SetText("GPS: NO CARD");
				else
					m_PlayerPosLabel.SetText("GPS: NO SIGNAL");
				m_PlayerPosLabel.SetColor(ARGB(255, 200, 50, 50));
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// FLASH CARD SLOT SYSTEM
	// Cards control which PDA features are available
	// Card A = Map, Card B = Capture, Card C = Groups, Card D = Support, Card E = GPS
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void CheckFlashCards()
	{
		m_HasMapCard = GPDA_PDAItemHelper.HasMapCard();
		m_HasCaptureCard = GPDA_PDAItemHelper.HasCaptureCard();
		m_HasGroupsCard = GPDA_PDAItemHelper.HasGroupsCard();
		m_HasSupportCard = GPDA_PDAItemHelper.HasSupportCard();
		m_HasGPSCard = GPDA_PDAItemHelper.HasGPSCard();
		m_HasDatabaseCard = GPDA_PDAItemHelper.HasDatabaseCard();
		
		string cardInfo = "[GPDA] Flash cards -";
		if (m_HasMapCard) cardInfo += " Map:YES"; else cardInfo += " Map:NO";
		if (m_HasCaptureCard) cardInfo += " Capture:YES"; else cardInfo += " Capture:NO";
		if (m_HasGroupsCard) cardInfo += " Groups:YES"; else cardInfo += " Groups:NO";
		if (m_HasSupportCard) cardInfo += " Support:YES"; else cardInfo += " Support:NO";
		if (m_HasGPSCard) cardInfo += " GPS:YES"; else cardInfo += " GPS:NO";
		if (m_HasDatabaseCard) cardInfo += " Database:YES"; else cardInfo += " Database:NO";
		Print(cardInfo);
	}
	
	protected void ApplyCardRestrictions()
	{
		// Map tab - requires Card A
		if (m_TabMap)
			m_TabMap.Show(m_HasMapCard);
		
		// Groups tab - requires Card C (also depends on group membership, handled separately)
		// Groups tab visibility is managed by UpdateGroupsTabVisibility + card check
		// We only force-hide if no card; group check can further hide it
		if (!m_HasGroupsCard && m_TabGroups)
			m_TabGroups.Show(false);
		
		// Support tab - requires Card D
		if (m_TabSupport)
			m_TabSupport.Show(m_HasSupportCard);
		
		// Database tab - requires Card F
		if (m_TabDatabase)
			m_TabDatabase.Show(m_HasDatabaseCard);
		
		// Capture zones on map - depend on Card B 
		// (capture tab is admin-only, but capture zones on map depend on card B)
		
		Print("[GPDA] Card restrictions applied");
	}
	
	protected void ShowTab(string tab)
	{
		// Hide all panels
		if (m_PanelMap) m_PanelMap.Show(false);
		if (m_PanelChat) m_PanelChat.Show(false);
		if (m_PanelPM) m_PanelPM.Show(false);
		if (m_PanelProfile) m_PanelProfile.Show(false);
		if (m_PanelTop) m_PanelTop.Show(false);
		if (m_PanelGroups) m_PanelGroups.Show(false);
		if (m_PanelDatabase) m_PanelDatabase.Show(false);
		if (m_PanelRank) m_PanelRank.Show(false);
		if (m_PanelSupport) m_PanelSupport.Show(false);
		
		// Reset all tabs to inactive
		SetTabInactive("Map");
		SetTabInactive("Chat");
		SetTabInactive("PM");
		SetTabInactive("Profile");
		SetTabInactive("Top100");
		SetTabInactive("Groups");
		SetTabInactive("Database");
		SetTabInactive("Rank");
		SetTabInactive("Support");
		SetTabInactive("Admins");
		
		if (tab == "map")
		{
			// Requires Map card (Card A)
			if (!m_HasMapCard)
			{
				NotificationSystem.AddNotificationExtended(3.0, "PDA", "Map card not inserted! Insert GRESHNIK_Card_A into PDA.", "set:dayz_gui image:icon_x");
				ShowTab("chat");
				return;
			}
			if (m_PanelMap) m_PanelMap.Show(true);
			SetTabActive("Map");
			CenterMapOnPlayer();
			RequestCapturePointsForMap();
		}
		else if (tab == "chat")
		{
			if (m_PanelChat) m_PanelChat.Show(true);
			SetTabActive("Chat");
			RequestChatHistory();
		}
		else if (tab == "pm")
		{
			if (m_PanelPM) m_PanelPM.Show(true);
			SetTabActive("PM");
		}
		else if (tab == "profile")
		{
			if (m_PanelProfile) m_PanelProfile.Show(true);
			SetTabActive("Profile");
			RequestProfile();
		}
		else if (tab == "top")
		{
			if (m_PanelTop) m_PanelTop.Show(true);
			SetTabActive("Top100");
			RequestTop100(m_CurrentSort);
		}
		else if (tab == "groups")
		{
			// Requires Groups card (Card C)
			if (!m_HasGroupsCard)
			{
				NotificationSystem.AddNotificationExtended(3.0, "PDA", "Groups card not inserted! Insert GRESHNIK_Card_C into PDA.", "set:dayz_gui image:icon_x");
				ShowTab("chat");
				return;
			}
			if (m_PanelGroups) m_PanelGroups.Show(true);
			SetTabActive("Groups");
			RequestCapturedZones();
			RequestGroupMembers();
		}
		else if (tab == "database")
		{
			// Requires Database card (Card F)
			if (!m_HasDatabaseCard)
			{
				NotificationSystem.AddNotificationExtended(3.0, "PDA", "Database card not inserted! Insert GRESHNIK_Card_F into PDA.", "set:dayz_gui image:icon_x");
				ShowTab("chat");
				return;
			}
			if (m_PanelDatabase) m_PanelDatabase.Show(true);
			SetTabActive("Database");
		}
		else if (tab == "rank")
		{
			if (m_PanelRank) m_PanelRank.Show(true);
			SetTabActive("Rank");
			RequestRankData();
		}
		else if (tab == "support")
		{
			// Requires Support card (Card D)
			if (!m_HasSupportCard)
			{
				NotificationSystem.AddNotificationExtended(3.0, "PDA", "Support card not inserted! Insert GRESHNIK_Card_D into PDA.", "set:dayz_gui image:icon_x");
				ShowTab("chat");
				return;
			}
			if (m_PanelSupport) m_PanelSupport.Show(true);
			SetTabActive("Support");
			// Admins with database access see all tickets, others see only their own
			if (m_HasDatabaseAccess)
				RequestAllTickets();
			else
				RequestMyTickets();
		}
		else if (tab == "admins")
		{
			// Open admin menu as separate draggable window
			GPDA_AdminMenu.GetInstance().OpenAdminMenu();
			// Don't show admin panel in main PDA anymore
		}
	}
	
	// Update header with player name and faction
	protected void UpdateHeader()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		
		string playerName = "Player";
		string factionName = "Loner";
		
		if (m_IsLoggedIn && m_LoginName != "")
		{
			playerName = m_LoginName;
		}
		else
		{
			if (player.GetIdentity())
			{
				playerName = player.GetIdentity().GetName();
			}
		}
		
		if (m_HeaderUserName)
			m_HeaderUserName.SetText(playerName);
		
		if (m_HeaderUserFaction)
		{
			m_HeaderUserFaction.SetText(factionName);
			m_HeaderUserFactionValue = factionName;
		}
	}
	
	protected void UpdateHeaderInfo()
	{
		// Online count
		if (m_OnlineStatus)
		{
			array<Man> onlinePlayers = new array<Man>;
			GetGame().GetPlayers(onlinePlayers);
			int count = onlinePlayers.Count();
			m_OnlineStatus.SetText(count.ToString() + " online");
		}
		
		// Server time (in-game world time)
		if (m_ServerTime)
		{
			int year, month, day, hour, minute;
			GetGame().GetWorld().GetDate(year, month, day, hour, minute);
			string hStr = hour.ToString();
			string mStr = minute.ToString();
			if (hour < 10) hStr = "0" + hStr;
			if (minute < 10) mStr = "0" + mStr;
			m_ServerTime.SetText(hStr + ":" + mStr);
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// MAP MARKER PANEL
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void ShowMarkerPanel(vector mapPos)
	{
		m_PendingMarkerPos = mapPos;
		if (!m_MarkerPanel) return;
		m_MarkerPanel.Show(true);
		m_MarkerPanel.SetSort(1024);
		if (m_MarkerNameInput)
		{
			string defName = "Marker " + (m_UserMarkers.Count() + 1).ToString();
			m_MarkerNameInput.SetText(defName);
			SetFocus(m_MarkerNameInput);
		}
		m_SelectedMarkerColor = ARGB(255, 255, 50, 50);
		UpdateMarkerColorSelection();
	}
	
	protected void HideMarkerPanel()
	{
		if (m_MarkerPanel)
			m_MarkerPanel.Show(false);
	}
	
	protected void CreateMarkerFromPanel()
	{
		string markerName = "";
		if (m_MarkerNameInput)
			markerName = m_MarkerNameInput.GetText();
		if (markerName == "")
			markerName = "Marker";
		
		Param3<vector, int, string> mark = new Param3<vector, int, string>(m_PendingMarkerPos, m_SelectedMarkerColor, markerName);
		m_UserMarkers.Insert(mark);
		
		HideMarkerPanel();
		// Force immediate refresh (bypass throttle)
		m_LastMapMarksUpdate = 0;
		UpdateMapMarks();
	}
	
	protected void UpdateMarkerColorSelection()
	{
		int selColor;
		
		if (m_MarkerColorRed)
		{
			if (m_SelectedMarkerColor == ARGB(255, 255, 50, 50))
				selColor = ARGB(255, 255, 80, 80);
			else
				selColor = ARGB(255, 130, 30, 30);
			m_MarkerColorRed.SetColor(selColor);
		}
		if (m_MarkerColorGreen)
		{
			if (m_SelectedMarkerColor == ARGB(255, 50, 220, 80))
				selColor = ARGB(255, 80, 255, 110);
			else
				selColor = ARGB(255, 30, 130, 50);
			m_MarkerColorGreen.SetColor(selColor);
		}
		if (m_MarkerColorBlue)
		{
			if (m_SelectedMarkerColor == ARGB(255, 50, 100, 230))
				selColor = ARGB(255, 80, 130, 255);
			else
				selColor = ARGB(255, 30, 60, 150);
			m_MarkerColorBlue.SetColor(selColor);
		}
		if (m_MarkerColorYellow)
		{
			if (m_SelectedMarkerColor == ARGB(255, 240, 215, 50))
				selColor = ARGB(255, 255, 240, 80);
			else
				selColor = ARGB(255, 150, 130, 30);
			m_MarkerColorYellow.SetColor(selColor);
		}
		if (m_MarkerColorWhite)
		{
			if (m_SelectedMarkerColor == ARGB(255, 230, 230, 230))
				selColor = ARGB(255, 255, 255, 255);
			else
				selColor = ARGB(255, 130, 130, 130);
			m_MarkerColorWhite.SetColor(selColor);
		}
	}
	
	// Request database access check from server
	protected void RequestDatabaseAccess()
	{
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Database", "CheckAccess", null, true, null, playerObj);
	}
	
	// Called from server to grant/revoke database access
	void SetDatabaseAccess(bool hasAccess)
	{
		m_HasDatabaseAccess = hasAccess;
		// Tab visibility is now controlled by Card F in ApplyCardRestrictions()
		// Database access flag is still used for admin functions within the tab
		
		// Check if player is admin for Admins tab
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetIdentity() && m_TabAdmins)
		{
			string steam64 = player.GetIdentity().GetPlainId();
			// Send RPC to check admin status
			GetRPCManager().SendRPC("GPDA_Database", "CheckAdminStatus", new Param1<string>(steam64), true);
		}
	}
	
	// Called from RPC to set admin status
	void SetAdminStatus(bool isAdmin)
	{
		if (m_TabAdmins)
			m_TabAdmins.Show(isAdmin);
	}
	
	// Update header with group name from server
	void SetHeaderFaction(string factionName)
	{
		if (m_HeaderUserFaction)
		{
			m_HeaderUserFaction.SetText(factionName);
			m_HeaderUserFactionValue = factionName;
		}
	}
	
	protected void SetTabActive(string tabName)
	{
		ButtonWidget tab = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_" + tabName));
		TextWidget tabText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_" + tabName + "Text"));
		ImageWidget tabIcon = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_" + tabName + "Icon"));
		
		if (tab) tab.SetColor(m_ActiveTabColor);
		if (tabText) tabText.SetColor(ARGB(255, 51, 217, 128)); // Green
		if (tabIcon) tabIcon.SetColor(ARGB(255, 51, 217, 128)); // Green
	}
	
	protected void SetTabInactive(string tabName)
	{
		ButtonWidget tab = ButtonWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_" + tabName));
		TextWidget tabText = TextWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_" + tabName + "Text"));
		ImageWidget tabIcon = ImageWidget.Cast(layoutRoot.FindAnyWidget("GPDA_Tab_" + tabName + "Icon"));
		
		if (tab) tab.SetColor(m_InactiveTabColor);
		if (tabText) tabText.SetColor(ARGB(255, 128, 140, 133)); // Gray
		if (tabIcon) tabIcon.SetColor(ARGB(255, 128, 140, 133)); // Gray
	}
	
	protected void CenterMapOnPlayer()
	{
		if (!m_MapWidget) return;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			vector pos = player.GetPosition();
			m_MapWidget.SetMapPos(Vector(pos[0], 0, pos[2]));
			m_MapWidget.SetScale(0.5);
			UpdatePlayerMarker();
			UpdateMapCoords();
		}
	}
	
	protected void UpdatePlayerMarker()
	{
		if (!m_MapWidget) return;
		
		m_MapWidget.ClearUserMarks();
		
		if (m_HasGPS)
		{
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (player)
			{
				vector pos = player.GetPosition();
				m_MapWidget.AddUserMark(pos, "YOU", ARGB(255, 50, 255, 50), "\\dz\\gear\\navigation\\data\\map_tree_ca.paa");
			}
		}
	}
	
	protected void UpdateMapCoords()
	{
		if (!m_MapWidget || !m_MapCoords) return;
		
		vector mapPos = m_MapWidget.GetMapPos();
		int mx = Math.Round(mapPos[0]);
		int mz = Math.Round(mapPos[2]);
		m_MapCoords.SetText("X: " + mx.ToString() + " | Z: " + mz.ToString());
		
		UpdateGPSStatus();
	}
	
	protected void ZoomMap(float delta)
	{
		if (!m_MapWidget) return;
		
		float scale = m_MapWidget.GetScale();
		scale = Math.Clamp(scale + delta, 0.1, 2.0);
		m_MapWidget.SetScale(scale);
	}
	
	protected void ToggleAutoFollow()
	{
		m_AutoFollow = !m_AutoFollow;
		
		if (m_BtnAutoFollow)
		{
			if (m_AutoFollow)
				m_BtnAutoFollow.SetColor(m_ActiveTabColor);
			else
				m_BtnAutoFollow.SetColor(m_InactiveTabColor);
		}
	}
	
	protected void SendChatMessage()
	{
		if (!m_ChatInput) return;
		
		string message = m_ChatInput.GetText();
		message = message.Trim();
		
		if (message == "") return;
		
		if (message.Length() >= 3 && message.Substring(0, 3) == "/nt")
		{
			string localText = message.Substring(3, message.Length() - 3);
			localText.Trim();
			if (localText.Length() > 0 && localText.Substring(0, 1) == " ")
			{
				localText = localText.Substring(1, localText.Length() - 1);
				localText.Trim();
			}
			GPDA_ChatNotify notifyTest = GPDA_ChatNotify.GetInstance();
			if (notifyTest)
				notifyTest.PushForce("PDA", localText);
			m_ChatInput.SetText("");
			return;
		}
		
		if (!m_IsLoggedIn)
		{
			if (GetGame().GetMission())
				NotificationSystem.AddNotificationExtended(3.0, "PDA", "You must login to use chat", "set:dayz_gui image:icon_x");
			return;
		}
		
		if (m_AnonChatEnabled)
		{
			message = "/anon " + message;
		}
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(message);
		rpc.Send(GetGame().GetPlayer(), GPDA_RPC.SEND_CHAT_MESSAGE, true, null);
		
		m_ChatInput.SetText("");
	}
	
	protected void ToggleAnonChat()
	{
		m_AnonChatEnabled = !m_AnonChatEnabled;
		if (m_ChatInput)
		{
			if (m_AnonChatEnabled)
				m_ChatInput.SetColor(ARGB(255, 220, 255, 220));
			else
				m_ChatInput.SetColor(ARGB(255, 230, 230, 230));
		}
		UpdateAnonChatButton();
	}
	
	static bool HasRequiredPDAItem()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return false;
		
		// Check if player has GRESHNIK_PDA in inventory
		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		for (int i = 0; i < items.Count(); i++)
		{
			if (!items[i]) continue;
			if (items[i].IsKindOf("GRESHNIK_PDA_Base"))
				return true;
		}
		return false;
	}
	
	protected void UpdateAnonChatButton()
	{
		if (!m_BtnAnonChat) return;
		if (m_AnonChatEnabled)
		{
			m_BtnAnonChat.SetColor(ARGB(255, 40, 160, 90));
			if (m_BtnAnonChatText) m_BtnAnonChatText.SetColor(ARGB(255, 255, 255, 255));
		}
		else
		{
			m_BtnAnonChat.SetColor(ARGB(255, 64, 64, 72));
			if (m_BtnAnonChatText) m_BtnAnonChatText.SetColor(ARGB(255, 190, 190, 190));
		}
	}
	
	protected void RequestChatHistory()
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Send(GetGame().GetPlayer(), GPDA_RPC.REQUEST_CHAT_HISTORY, true, null);
	}
	
	void ClearChatMessages()
	{
		ClearWidgetList(m_ChatItems);
	}
	
	void AddChatMessage(string sender, string message, int timestamp)
	{
		if (!m_ChatContent) return;
		
		string line = sender + ": " + message;
		Widget item = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/chat_item.layout", m_ChatContent);
		if (!item) return;
		TextWidget txt = TextWidget.Cast(item.FindAnyWidget("ChatItemText"));
		if (txt) txt.SetText(line);
		m_ChatItems.Insert(item);
	}
	
	protected void SendPrivateMessage()
	{
		if (!m_PMRecipient || !m_PMInput) return;
		if (!m_IsLoggedIn)
		{
			if (GetGame().GetMission())
				NotificationSystem.AddNotificationExtended(3.0, "PDA", "You must login to send messages", "set:dayz_gui image:icon_x");
			return;
		}
		
		string recipient = m_PMRecipient.GetText();
		recipient = recipient.Trim();
		string message = m_PMInput.GetText();
		message = message.Trim();
		
		if (recipient == "" || message == "") return;
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(recipient);
		rpc.Write(message);
		rpc.Send(GetGame().GetPlayer(), GPDA_RPC.SEND_PM, true, null);
		
		m_PMInput.SetText("");
		
		AddPMMessage("You -> " + recipient, message);
	}
	
	void AddPMMessage(string header, string message)
	{
		if (!m_PMMessages) return;
		
		string line = "[" + header + "]: " + message;
		m_PMMessages.AddItem(line, null, 0);
	}
	
	protected void RequestProfile()
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Send(GetGame().GetPlayer(), GPDA_RPC.REQUEST_PROFILE, true, null);
	}
	
	void UpdateProfile(string nickname, int killsPlayers, int killsZombies, int killsAnimals, int deaths, int playtime)
	{
		if (m_ValNickname) m_ValNickname.SetText(nickname);
		if (m_ValKillsPlayers) m_ValKillsPlayers.SetText(killsPlayers.ToString());
		if (m_ValKillsZombies) m_ValKillsZombies.SetText(killsZombies.ToString());
		if (m_ValKillsAnimals) m_ValKillsAnimals.SetText(killsAnimals.ToString());
		if (m_ValDeaths) m_ValDeaths.SetText(deaths.ToString());
		
		int hours = playtime / 60;
		int mins = playtime % 60;
		string timeText = hours.ToString() + "h " + mins.ToString() + "m";
		if (m_ValPlaytime) m_ValPlaytime.SetText(timeText);
		
		int totalKills = killsPlayers + killsZombies + killsAnimals;
		if (m_ValTotalKills) m_ValTotalKills.SetText(totalKills.ToString());
		
		float kd = 0;
		if (deaths > 0)
			kd = killsPlayers / deaths;
		else
			kd = killsPlayers;
		
		string kdText = kd.ToString();
		if (kdText.Length() > 4)
			kdText = kdText.Substring(0, 4);
		if (m_ValKD) m_ValKD.SetText(kdText);
	}
	
	void UpdateProfileExtended(string groupName, int licenseFlags)
	{
		// Update group
		if (m_ProfileGroup) 
			m_ProfileGroup.SetText("Group: " + groupName);
		
		// Update header faction too
		if (m_HeaderUserFaction)
		{
			m_HeaderUserFaction.SetText(groupName);
			m_HeaderUserFactionValue = groupName;
		}
		
		// Update licenses (flags: 1=Hunting, 2=Fishing, 4=Zone, 8=Weapons, 16=Trading)
		UpdateLicenseCard(m_License1Status, m_License1Indicator, (licenseFlags & 1) != 0);
		UpdateLicenseCard(m_License2Status, m_License2Indicator, (licenseFlags & 2) != 0);
		UpdateLicenseCard(m_License3Status, m_License3Indicator, (licenseFlags & 4) != 0);
		UpdateLicenseCard(m_License4Status, m_License4Indicator, (licenseFlags & 8) != 0);
		UpdateLicenseCard(m_License5Status, m_License5Indicator, (licenseFlags & 16) != 0);
	}
	
	// Update rank display in Profile
	void UpdateProfileRank(string rankName, int currentXP, int xpForNextRank, string nextRankName)
	{
		if (m_ProfileRankName)
			m_ProfileRankName.SetText(rankName);
		
		if (m_ProfileXPText)
		{
			if (xpForNextRank > 0)
				m_ProfileXPText.SetText(currentXP.ToString() + " / " + xpForNextRank.ToString());
			else
				m_ProfileXPText.SetText("MAX");
		}
		
		if (m_ProfileNextRank)
		{
			if (xpForNextRank > 0)
			{
				int left = xpForNextRank - currentXP;
				if (left < 0) left = 0;
				m_ProfileNextRank.SetText("Next: " + nextRankName + " (" + left.ToString() + " XP)");
			}
			else
			{
				m_ProfileNextRank.SetText("Next: MAX");
			}
		}
		
		// Update progress bar
		if (m_ProfileXPBarFill)
		{
			float progress = 0;
			if (xpForNextRank > 0)
				progress = (float)currentXP / (float)xpForNextRank;
			if (progress < 0) progress = 0;
			if (progress > 1) progress = 1;
			m_ProfileXPBarFill.SetSize(460 * progress, 8);
		}
	}
	
	protected void UpdateLicenseCard(TextWidget statusWidget, ImageWidget indicator, bool hasLicense)
	{
		if (statusWidget)
		{
			if (hasLicense)
			{
				statusWidget.SetText("YES");
				statusWidget.SetColor(ARGB(255, 50, 220, 100)); // Green
			}
			else
			{
				statusWidget.SetText("NO");
				statusWidget.SetColor(ARGB(255, 220, 60, 50)); // Red
			}
		}
		
		if (indicator)
		{
			if (hasLicense)
				indicator.SetColor(ARGB(255, 50, 220, 100)); // Green
			else
				indicator.SetColor(ARGB(255, 220, 60, 50)); // Red
		}
	}
	
	protected void UpdateLicenseStatus(TextWidget widget, bool hasLicense)
	{
		if (!widget)
		{
			Print("[GPDA] UpdateLicenseStatus: widget is NULL!");
			return;
		}
		
		if (hasLicense)
		{
			widget.SetText("YES");
			widget.SetColor(ARGB(255, 50, 220, 100)); // Bright Green
		}
		else
		{
			widget.SetText("NO");
			widget.SetColor(ARGB(255, 220, 60, 50)); // Red
		}
	}
	
	protected void RequestTop100(int sortType)
	{
		m_CurrentSort = sortType;
		
		if (m_BtnSortPlayers)
		{
			if (sortType == GPDA_SORT_PLAYERS)
				m_BtnSortPlayers.SetColor(m_ActiveTabColor);
			else
				m_BtnSortPlayers.SetColor(m_InactiveTabColor);
		}
		if (m_BtnSortZombies)
		{
			if (sortType == GPDA_SORT_ZOMBIES)
				m_BtnSortZombies.SetColor(m_ActiveTabColor);
			else
				m_BtnSortZombies.SetColor(m_InactiveTabColor);
		}
		if (m_BtnSortTotal)
		{
			if (sortType == GPDA_SORT_TOTAL)
				m_BtnSortTotal.SetColor(m_ActiveTabColor);
			else
				m_BtnSortTotal.SetColor(m_InactiveTabColor);
		}
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(sortType);
		rpc.Send(GetGame().GetPlayer(), GPDA_RPC.REQUEST_TOP100, true, null);
	}
	
	void UpdateTop100(array<ref GPDA_TopEntry> entries)
	{
		if (!m_TopList) return;
		
		m_TopList.ClearItems();
		
		int i;
		int cnt = entries.Count();
		for (i = 0; i < cnt; i++)
		{
			GPDA_TopEntry e = entries[i];
			string line = e.Rank.ToString() + ".  " + e.Nickname;
			while (line.Length() < 35)
				line = line + " ";
			line = line + e.KillsPlayers.ToString();
			while (line.Length() < 45)
				line = line + " ";
			line = line + e.KillsZombies.ToString();
			while (line.Length() < 55)
				line = line + " ";
			line = line + e.TotalKills.ToString();
			
			m_TopList.AddItem(line, null, 0);
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// GROUPS FUNCTIONALITY (Standalone PDA System)
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void RequestGroupMembers()
	{
		// Send RPC to server to get group data
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Groups", "RequestGroupData", null, true, null, playerObj);
	}
	
	protected void AddGroupMember()
	{
		if (!m_Steam64Input) return;
		
		string steam64 = m_Steam64Input.GetText();
		steam64 = steam64.Trim();
		
		if (steam64 == "") return;
		
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Groups", "RequestAddMember", new Param1<string>(steam64), true, null, playerObj);
		
		m_Steam64Input.SetText("");
	}
	
	protected void KickGroupMember()
	{
		if (!m_Steam64Input) return;
		
		string steam64 = m_Steam64Input.GetText();
		steam64 = steam64.Trim();
		
		if (steam64 == "") return;
		
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Groups", "RequestKickMember", new Param1<string>(steam64), true, null, playerObj);
		
		m_Steam64Input.SetText("");
	}
	
	void UpdateGroupInfo(string groupName, int points, int zones)
	{
		if (m_GroupName) m_GroupName.SetText(groupName);
		// Points and Zones removed in this version
	}
	
	void UpdateGroupMembers(array<ref GPDA_GroupMember> members)
	{
		if (!m_MembersList) return;
		
		m_MembersList.ClearItems();
		
		// Clear and recreate member IDs array
		if (!m_MemberSteamIDs)
			m_MemberSteamIDs = new array<string>;
		m_MemberSteamIDs.Clear();
		
		int onlineCount = 0;
		int totalCount = 0;
		
		if (members)
		{
			totalCount = members.Count();
			int i;
			for (i = 0; i < totalCount; i++)
			{
				GPDA_GroupMember gm = members[i];
				if (gm)
				{
					string statusMark = "";
					if (gm.IsOnline)
					{
						onlineCount++;
						statusMark = "[+] ";
					}
					else
					{
						statusMark = "[-] ";
					}
					
					string line = statusMark + gm.Name + "  |  " + gm.Steam64;
					m_MembersList.AddItem(line, null, 0);
					m_MemberSteamIDs.Insert(gm.Steam64);
				}
			}
		}
		
		if (m_GroupOnlineText)
			m_GroupOnlineText.SetText(onlineCount.ToString() + " / " + totalCount.ToString());
		
		if (m_GroupOnlineFooterText)
			m_GroupOnlineFooterText.SetText("Online: " + onlineCount.ToString() + " / " + totalCount.ToString());
	}
	
	// Handle member selection from list
	protected void OnMemberSelected(int index)
	{
		if (!m_MemberSteamIDs) return;
		if (index < 0 || index >= m_MemberSteamIDs.Count()) return;
		
		string steam64 = m_MemberSteamIDs[index];
		if (m_Steam64Input)
			m_Steam64Input.SetText(steam64);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// DATABASE FUNCTIONALITY (Military Only)
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void SearchPlayer()
	{
		if (!m_SearchInput) return;
		
		string query = m_SearchInput.GetText();
		query = query.Trim();
		
		if (query == "") return;
		
		// Send search request to server
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Database", "SearchPlayer", new Param1<string>(query), true, null, playerObj);
	}
	
	protected void IssueLicense(string licenseType)
	{
		if (m_SelectedPlayerSteam64 == "") return;
		
		// Send license issue request to server
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Database", "IssueLicense", new Param2<string, string>(m_SelectedPlayerSteam64, licenseType), true, null, playerObj);
	}
	
	void UpdateSearchResults(array<ref GPDA_PlayerSearchResult> results)
	{
		if (!m_SearchResultsList) return;
		
		m_SearchResultsList.ClearItems();
		
		if (!m_SearchResultSteamIDs)
			m_SearchResultSteamIDs = new array<string>;
		m_SearchResultSteamIDs.Clear();
		
		if (results)
		{
			foreach (GPDA_PlayerSearchResult r : results)
			{
				if (r)
				{
					string line = r.Callsign + " | " + r.Steam64;
					m_SearchResultsList.AddItem(line, null, 0);
					m_SearchResultSteamIDs.Insert(r.Steam64);
				}
			}
		}
	}
	
	void UpdateDossier(string callsign, string steam64, int age, string groupName, string bio, int licenseFlags)
	{
		m_SelectedPlayerSteam64 = steam64;
		
		Print("[GPDA] UpdateDossier: " + callsign + " steam=" + steam64 + " flags=" + licenseFlags.ToString());
		
		if (m_DossierCallsign) m_DossierCallsign.SetText("Callsign: " + callsign);
		if (m_DossierSteam64) m_DossierSteam64.SetText("Steam64: " + steam64);
		if (m_DossierAge) m_DossierAge.SetText("Age: " + age.ToString());
		if (m_DossierGroup) m_DossierGroup.SetText("Group: " + groupName);
		if (m_DossierBio) m_DossierBio.SetText(bio);
		
		// Update license statuses in Database dossier
		bool hasHunting = (licenseFlags & 1) != 0;
		bool hasFishing = (licenseFlags & 2) != 0;
		bool hasZone = (licenseFlags & 4) != 0;
		bool hasWeapons = (licenseFlags & 8) != 0;
		bool hasTrading = (licenseFlags & 16) != 0;
		
		Print("[GPDA] License flags decoded: H=" + hasHunting.ToString() + " F=" + hasFishing.ToString() + " Z=" + hasZone.ToString() + " W=" + hasWeapons.ToString() + " T=" + hasTrading.ToString());
		
		UpdateLicenseStatus(m_LicenseHuntingStatus, hasHunting);
		UpdateLicenseStatus(m_LicenseFishingStatus, hasFishing);
		UpdateLicenseStatus(m_LicenseZoneStatus, hasZone);
		UpdateLicenseStatus(m_LicenseWeaponsStatus, hasWeapons);
		UpdateLicenseStatus(m_LicenseTradingStatus, hasTrading);
	}
	
	protected void OnSearchResultSelected(int index)
	{
		if (!m_SearchResultSteamIDs) return;
		if (index < 0 || index >= m_SearchResultSteamIDs.Count()) return;
		
		string steam64 = m_SearchResultSteamIDs[index];
		
		// Request dossier from server
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Database", "RequestDossier", new Param1<string>(steam64), true, null, playerObj);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// EVENT HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		string name = "";
		if (w) name = w.GetName();
		if (m_AuthGate)
		{
			// Only allow interacting with Auth UI (including edit boxes) while gated
			bool isAuthWidget = false;
			if (name != "")
			{
				if (name.IndexOf("GPDA_Auth") == 0)
					isAuthWidget = true;
				if (name == "GPDA_BtnClose")
					isAuthWidget = true;
			}
			if (!isAuthWidget)
				return true;
		}
		
		if (w == m_BtnClose || name == "GPDA_BtnClose")
		{
			CloseMenu();
			return true;
		}
		
		// Tab buttons
		if (w == m_TabMap || name == "GPDA_Tab_Map")
		{
			ShowTab("map");
			return true;
		}
		if (w == m_TabChat || name == "GPDA_Tab_Chat")
		{
			ShowTab("chat");
			return true;
		}
		if (w == m_TabPM || name == "GPDA_Tab_PM")
		{
			ShowTab("pm");
			return true;
		}
		if (w == m_TabProfile || name == "GPDA_Tab_Profile")
		{
			ShowTab("profile");
			return true;
		}
		if (w == m_TabTop || name == "GPDA_Tab_Top100")
		{
			ShowTab("top");
			return true;
		}
		if (w == m_TabGroups || name == "GPDA_Tab_Groups")
		{
			ShowTab("groups");
			return true;
		}
		if (w == m_TabDatabase || name == "GPDA_Tab_Database")
		{
			ShowTab("database");
			return true;
		}
		if (w == m_TabRank || name == "GPDA_Tab_Rank")
		{
			// Close main PDA and open rank tool
			CloseMenu();
			GPDA_RankToolMenu.GetInstance().OpenRankTool();
			return true;
		}
		if (w == m_TabSupport || name == "GPDA_Tab_Support")
		{
			// Show support panel in PDA (not separate window)
			ShowTab("support");
			return true;
		}
		if (w == m_TabAdmins || name == "GPDA_Tab_Admins")
		{
			// Close main PDA and open admin menu
			CloseMenu();
			GPDA_AdminMenu.GetInstance().OpenAdminMenu();
			return true;
		}
		
		// Add capture tool button (if exists in layout)
		if (name == "GPDA_Tab_Capture")
		{
			// Close main PDA and open capture tool
			CloseMenu();
			GPDA_CaptureToolMenu.GetInstance().OpenCaptureTool();
			return true;
		}
		
		// Support buttons
		if (w == m_BtnCreateTicket || name == "GPDA_BtnCreateTicket")
		{
			OnCreateTicket();
			return true;
		}
		if (w == m_BtnSendTicketMsg || name == "GPDA_BtnSendTicketMsg")
		{
			OnSendTicketMessage();
			return true;
		}

		// Admin functionality moved to separate GPDA_AdminMenu window
		// All admin buttons now handled in GPDA_AdminMenu
		
		
		// Map buttons
		if (w == m_BtnZoomIn || name == "GPDA_Btn_ZoomIn")
		{
			ZoomMap(0.1);
			return true;
		}
		if (w == m_BtnZoomOut || name == "GPDA_Btn_ZoomOut")
		{
			ZoomMap(-0.1);
			return true;
		}
		if (w == m_BtnCenter || name == "GPDA_Btn_Center")
		{
			CenterMapOnPlayer();
			return true;
		}
		if (w == m_BtnAutoFollow || name == "GPDA_Btn_AutoFollow")
		{
			ToggleAutoFollow();
			return true;
		}
		
		// Chat
		if (w == m_BtnSendChat || name == "GPDA_BtnSendChat")
		{
			SendChatMessage();
			return true;
		}
		if (w == m_BtnAnonChat || name == "GPDA_BtnAnonChat" || name == "GPDA_BtnAnonChatText")
		{
			Print("[GPDA][CHAT] ANON click");
			ToggleAnonChat();
			return true;
		}
		if (w == m_BtnNotifyToggle || name == "GPDA_BtnNotifyToggle" || name == "GPDA_BtnNotifyToggleText")
		{
			ToggleNotify();
			return true;
		}
		
		if (w == m_AuthBtnLogin || name == "GPDA_AuthBtnLogin" || name == "GPDA_AuthBtnLoginText")
		{
			OnAuthLogin();
			return true;
		}
		if (w == m_AuthBtnRegister || name == "GPDA_AuthBtnRegister" || name == "GPDA_AuthBtnRegisterText")
		{
			OnAuthRegister();
			return true;
		}
		if (w == m_AuthBtnLogout || name == "GPDA_AuthBtnLogout" || name == "GPDA_AuthBtnLogoutText")
		{
			OnAuthLogout();
			return true;
		}
		
		// PM
		if (w == m_BtnSendPM || name == "GPDA_BtnSendPM")
		{
			SendPrivateMessage();
			return true;
		}
		
		// Top100 sort buttons
		if (w == m_BtnSortPlayers || name == "GPDA_BtnSortPlayers")
		{
			RequestTop100(GPDA_SORT_PLAYERS);
			return true;
		}
		if (w == m_BtnSortZombies || name == "GPDA_BtnSortZombies")
		{
			RequestTop100(GPDA_SORT_ZOMBIES);
			return true;
		}
		if (w == m_BtnSortTotal || name == "GPDA_BtnSortTotal")
		{
			RequestTop100(GPDA_SORT_TOTAL);
			return true;
		}
		
		// Groups buttons
		if (w == m_BtnAddMember || name == "GPDA_BtnAddMember")
		{
			AddGroupMember();
			return true;
		}
		if (w == m_BtnKickMember || name == "GPDA_BtnKickMember")
		{
			KickGroupMember();
			return true;
		}
		
		// Database buttons
		if (w == m_BtnSearch || name == "GPDA_BtnSearch")
		{
			SearchPlayer();
			return true;
		}
		if (w == m_BtnLicenseHunting || name == "GPDA_BtnLicenseHunting")
		{
			IssueLicense("Hunting");
			return true;
		}
		if (w == m_BtnLicenseFishing || name == "GPDA_BtnLicenseFishing")
		{
			IssueLicense("Fishing");
			return true;
		}
		if (w == m_BtnLicenseZone || name == "GPDA_BtnLicenseZone")
		{
			IssueLicense("ZoneAccess");
			return true;
		}
		if (w == m_BtnLicenseWeapons || name == "GPDA_BtnLicenseWeapons")
		{
			IssueLicense("Weapons");
			return true;
		}
		if (w == m_BtnLicenseTrading || name == "GPDA_BtnLicenseTrading")
		{
			IssueLicense("Trading");
			return true;
		}
		
		// Support: ticket list click (custom widgets)
		Widget ticketRoot = ResolveTicketItemRoot(w);
		if (ticketRoot)
		{
			int idx = m_MyTicketItemWidgets.Find(ticketRoot);
			if (idx >= 0)
			{
				OnTicketSelected(idx);
				return true;
			}
		}
		
		// Marker panel buttons
		if (w == m_MarkerCreate || name == "GPDA_MarkerCreate")
		{
			CreateMarkerFromPanel();
			return true;
		}
		if (w == m_MarkerCancel || name == "GPDA_MarkerCancel")
		{
			HideMarkerPanel();
			return true;
		}
		if (w == m_MarkerColorRed || name == "GPDA_MarkerColorRed")
		{
			m_SelectedMarkerColor = ARGB(255, 255, 50, 50);
			UpdateMarkerColorSelection();
			return true;
		}
		if (w == m_MarkerColorGreen || name == "GPDA_MarkerColorGreen")
		{
			m_SelectedMarkerColor = ARGB(255, 50, 220, 80);
			UpdateMarkerColorSelection();
			return true;
		}
		if (w == m_MarkerColorBlue || name == "GPDA_MarkerColorBlue")
		{
			m_SelectedMarkerColor = ARGB(255, 50, 100, 230);
			UpdateMarkerColorSelection();
			return true;
		}
		if (w == m_MarkerColorYellow || name == "GPDA_MarkerColorYellow")
		{
			m_SelectedMarkerColor = ARGB(255, 240, 215, 50);
			UpdateMarkerColorSelection();
			return true;
		}
		if (w == m_MarkerColorWhite || name == "GPDA_MarkerColorWhite")
		{
			m_SelectedMarkerColor = ARGB(255, 230, 230, 230);
			UpdateMarkerColorSelection();
			return true;
		}
		
		return super.OnClick(w, x, y, button);
	}
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		// Manual double-click detection on map (fallback)
		if (m_MapWidget && (w == m_MapWidget || w.GetName() == "GPDA_MapWidget"))
		{
			if (button == 0)
			{
				int now = GetGame().GetTime();
				if (m_LastClickTime != 0 && (now - m_LastClickTime) < 400)
				{
					m_LastClickTime = 0;
					vector worldPos = m_MapWidget.ScreenToMap(Vector(x, y, 0));
					if (worldPos != vector.Zero)
						ShowMarkerPanel(worldPos);
					return true;
				}
				m_LastClickTime = now;
				return false;
			}
			else if (button == 1)
			{
				// Right-click: remove nearest marker within 50m
				if (m_UserMarkers && m_UserMarkers.Count() > 0)
				{
					vector clickPos = m_MapWidget.ScreenToMap(Vector(x, y, 0));
					int bestIdx = -1;
					float bestDist = 50.0;
					for (int mi = 0; mi < m_UserMarkers.Count(); mi++)
					{
						if (!m_UserMarkers[mi]) continue;
						float d = vector.Distance(clickPos, m_UserMarkers[mi].param1);
						if (d < bestDist)
						{
							bestDist = d;
							bestIdx = mi;
						}
					}
					if (bestIdx >= 0)
					{
						m_UserMarkers.Remove(bestIdx);
						m_LastMapMarksUpdate = 0;
						UpdateMapMarks();
					}
				}
				return true;
			}
		}
		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		// Double click on map - show marker panel (native handler)
		if (w == m_MapWidget && button == 0)
		{
			if (m_MapWidget)
			{
				vector mapPos = m_MapWidget.ScreenToMap(Vector(x, y, 0));
				if (mapPos != vector.Zero)
					ShowMarkerPanel(mapPos);
				return true;
			}
		}
		
		// Double click on members list - select Steam64 ID
		if (w == m_MembersList && button == 0)
		{
			int row = m_MembersList.GetSelectedRow();
			if (row >= 0)
			{
				OnMemberSelected(row);
			}
			return true;
		}
		
		return super.OnDoubleClick(w, x, y, button);
	}
	
	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		// When selecting item in members list, copy Steam64 to input
		if (w == m_MembersList)
		{
			OnMemberSelected(row);
			return true;
		}
		
		// When selecting item in search results, load dossier
		if (w == m_SearchResultsList)
		{
			OnSearchResultSelected(row);
			return true;
		}

		// Support tickets list moved to custom widgets (click)
		
		// Admin tickets list moved to GPDA_AdminMenu
		
		return super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (finished)
		{
			if (w == m_ChatInput)
			{
				SendChatMessage();
				return true;
			}
			if (w == m_PMInput)
			{
				SendPrivateMessage();
				return true;
			}
		}
		return super.OnChange(w, x, y, finished);
	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		Widget focused = GetFocus();
		if (focused)
		{
			string focusName = focused.GetName();
			if (focusName.Contains("Input") || focusName.Contains("Edit") || focusName.Contains("Chat") || focusName.Contains("Ticket") || focusName.Contains("Search") || focusName.Contains("PM"))
			{
				if (key == KeyCode.KC_ESCAPE)
				{
					CloseMenu();
					return true;
				}
				return false;
			}
		}
		
		if (w)
		{
			string wName = w.GetName();
			if (wName.Contains("Input") || wName.Contains("Edit") || wName.Contains("Chat") || wName.Contains("Ticket") || wName.Contains("Search") || wName.Contains("PM"))
			{
				if (key == KeyCode.KC_ESCAPE)
				{
					CloseMenu();
					return true;
				}
				return false;
			}
		}
		
		if (key == KeyCode.KC_ESCAPE)
		{
			CloseMenu();
			return true;
		}
		
		return super.OnKeyDown(w, x, y, key);
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		// Update header info (online count + time) every 2 seconds
		m_HeaderUpdateTimer += timeslice;
		if (m_HeaderUpdateTimer >= 2.0)
		{
			m_HeaderUpdateTimer = 0;
			UpdateHeaderInfo();
		}
		
		if (m_PanelMap && m_PanelMap.IsVisible())
		{
			// Avoid inventory scan every frame
			int now = GetGame().GetTime();
			if (m_LastMapMarksUpdate == 0 || (now - m_LastMapMarksUpdate) >= 15000)
				CheckForGPS();
			UpdateMapMarks();
			UpdateMapCoords();
			
			if (m_AutoFollow && m_MapWidget)
			{
				PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
				if (player)
				{
					vector pos = player.GetPosition();
					m_MapWidget.SetMapPos(Vector(pos[0], 0, pos[2]));
				}
			}
		}
	}
	
	void CloseMenu()
	{
		GetGame().GetMission().PlayerControlEnable(true);
		GetGame().GetUIManager().ShowCursor(false);
		GetGame().GetInput().ResetGameFocus();
		
		GetGame().GetUIManager().Back();
		
		s_IsOpen = false;
		Print("[GPDA] Menu closed");
	}
	
	static bool IsAnyTextInputFocused()
	{
		if (!s_IsOpen || !s_Instance) return false;
		Widget f = GetFocus();
		if (!f) return false;
		
		string n = f.GetName();
		if (n == "GPDA_ChatInput") return true;
		if (n == "GPDA_PMRecipient") return true;
		if (n == "GPDA_PMInput") return true;
		if (n == "GPDA_TicketTitleInput") return true;
		if (n == "GPDA_TicketDescInput") return true;
		if (n == "GPDA_TicketChatInput") return true;
		if (n == "GPDA_SearchInput") return true;
		if (n == "GPDA_MarkerNameInput") return true;
		
		if (EditBoxWidget.Cast(f)) return true;
		if (MultilineEditBoxWidget.Cast(f)) return true;
		
		return false;
	}
	
	static GPDA_Menu GetInstance()
	{
		return s_Instance;
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// QUESTS TAB FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════════
	
	// Quest methods removed - moved to separate system
	
	// ═══════════════════════════════════════════════════════════════════════════
	// RANK TAB FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void UpdateRankDisplay(string rankName, int xp, int nextXP)
	{
		if (!m_RankData)
			m_RankData = new GPDA_RankData();
		
		m_RankData.RankName = rankName;
		m_RankData.CurrentXP = xp;
		m_RankData.NextRankXP = nextXP;
		
		// Update widgets
		if (m_CurrentRankName)
			m_CurrentRankName.SetText(rankName);
		
		if (m_CurrentXPValue)
			m_CurrentXPValue.SetText(FormatNumber(xp) + " XP");
		
		// Progress bar
		float progress = 0;
		if (nextXP > 0)
			progress = (float)xp / (float)nextXP;
		else
			progress = 1.0; // Max rank
		
		if (m_RankProgressFill)
		{
			float fillWidth = 0.9 * progress;
			if (fillWidth > 0.9) fillWidth = 0.9;
			m_RankProgressFill.SetSize(fillWidth, 0);
		}
		
		if (m_RankProgressText)
		{
			if (nextXP > 0)
				m_RankProgressText.SetText(FormatNumber(xp) + " / " + FormatNumber(nextXP) + " XP");
			else
				m_RankProgressText.SetText("MAX RANK");
		}
	}
	
	string FormatNumber(int num)
	{
		string result = num.ToString();
		// Simple formatting with commas for thousands
		if (num >= 1000)
		{
			int thousands = num / 1000;
			int remainder = num % 1000;
			result = thousands.ToString() + "," + remainder.ToString();
		}
		return result;
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// SUPPORT/TICKETS TAB FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════════
	
	
	void OnCreateTicket()
	{
		if (!m_TicketTitleInput) 
		{
			Print("[GPDA] Ticket title input not found!");
			return;
		}
		
		string title = m_TicketTitleInput.GetText();
		string description = "";
		
		// Try to get description if widget exists
		if (m_TicketDescInput)
		{
			m_TicketDescInput.GetText(description);
		}
		
		title.Trim();
		description.Trim();
		
		Print("[GPDA] Creating ticket - Title: '" + title + "', Description length: " + description.Length().ToString());
		
		if (title == "")
		{
			Print("[GPDA] Ticket title is empty");
			if (GetGame().GetMission())
			{
				NotificationSystem.AddNotificationExtended(5.0, "Support", "Please fill the ticket title!", ICON_CONNECT);
			}
			return;
		}
		
		// If description is empty, use title as description
		if (description == "")
		{
			description = title;
		}
		
		// Send RPC to create ticket
		ref Param2<string, string> data = new Param2<string, string>(title, description);
		GetRPCManager().SendRPC("GPDA_Tickets", "CreateTicket", data, true);
		Print("[GPDA] Creating ticket RPC sent: " + title);
		
		// Clear inputs
		m_TicketTitleInput.SetText("");
		if (m_TicketDescInput)
			m_TicketDescInput.SetText("");
	}
	
	void OnSendTicketMessage()
	{
		if (!m_TicketChatInput) return;
		
		string message = m_TicketChatInput.GetText();
		if (message == "")
		{
			Print("[GPDA] Ticket message empty");
			return;
		}
		if (m_SelectedTicketID == "")
		{
			Print("[GPDA] No ticket selected");
			return;
		}
		
		// Send RPC to send message
		SendTicketMessage(m_SelectedTicketID, message);
		Print("[GPDA] Sending message to ticket: " + m_SelectedTicketID);
		
		m_TicketChatInput.SetText("");
		
		// Refresh messages after sending
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RequestTicketMessages, 500, false, m_SelectedTicketID);
	}

	// Admin ticket methods moved to GPDA_AdminMenu
	
	void OnTicketSelected(int row)
	{
		if (!m_MyTicketsData || row < 0 || row >= m_MyTicketsData.Count()) return;
		
		GPDA_TicketInfo ticket = m_MyTicketsData[row];
		if (!ticket) return;
		
		m_SelectedTicketID = ticket.Id;
		
		// Request messages for this ticket
		RequestTicketMessages(m_SelectedTicketID);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════════
	// TICKET SYSTEM - NEW METHODS
	// ═══════════════════════════════════════════════════════════════════════════════
	
	void UpdateMyTickets(array<ref GPDA_TicketInfo> tickets)
	{
		if (!m_MyTicketsContent) return;
		
		m_MyTicketsData = tickets;
		ClearWidgetList(m_MyTicketItemWidgets);
		
		foreach (GPDA_TicketInfo ticket : tickets)
		{
			string statusColor = "";
			if (ticket.Status == "open") statusColor = "[OPEN] ";
			else if (ticket.Status == "assigned") statusColor = "[ASSIGNED] ";
			else if (ticket.Status == "closed") statusColor = "[CLOSED] ";
			
			Widget item = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/ticket_item.layout", m_MyTicketsContent);
			if (!item) continue;
			TextWidget txt = TextWidget.Cast(item.FindAnyWidget("TicketItemText"));
			if (txt) txt.SetText(statusColor + ticket.Title);
			m_MyTicketItemWidgets.Insert(item);
		}

		// Auto-select first ticket to enable messaging
		if (tickets.Count() > 0)
		{
			OnTicketSelected(0);
		}
	}
	
	void UpdateAllTickets(array<ref GPDA_TicketInfo> tickets)
	{
		// Update admin menu if open
		GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
		if (adminMenu)
		{
			adminMenu.UpdateAllTickets(tickets);
		}
		
		// Also update support tab if admin is viewing all tickets
		if (m_MyTicketsContent && m_HasDatabaseAccess)
		{
			UpdateMyTickets(tickets);
		}
	}
	
	void UpdateTicketMessages(string ticketId, array<ref GPDA_TicketMessageInfo> messages)
	{
		// Update admin menu if open
		GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
		if (adminMenu)
		{
			adminMenu.UpdateTicketMessages(ticketId, messages);
		}
		
		// Also update support tab chat
		if (!m_TicketChatContent) return;
		
		ClearWidgetList(m_TicketChatItemWidgets);
		foreach (GPDA_TicketMessageInfo msg : messages)
		{
			string prefix;
			if (msg.IsAdmin) prefix = "[ADMIN] "; else prefix = "[USER] ";
			Widget item = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/chat_item.layout", m_TicketChatContent);
			if (!item) continue;
			TextWidget txt = TextWidget.Cast(item.FindAnyWidget("ChatItemText"));
			if (txt) txt.SetText(prefix + msg.SenderName + ": " + msg.Message);
			m_TicketChatItemWidgets.Insert(item);
		}
	}
	
	void RequestMyTickets()
	{
		ref Param1<int> data = new Param1<int>(0);
		GetRPCManager().SendRPC("GPDA_Tickets", "RequestMyTickets", data, true);
	}
	
	void RequestAllTickets()
	{
		ref Param1<int> data = new Param1<int>(0);
		GetRPCManager().SendRPC("GPDA_Tickets", "RequestAllTickets", data, true);
	}
	
	void RequestTicketMessages(string ticketId)
	{
		ref Param1<string> data = new Param1<string>(ticketId);
		GetRPCManager().SendRPC("GPDA_Tickets", "RequestTicketMessages", data, true);
	}
	
	void SendTicketMessage(string ticketId, string message)
	{
		ref Param2<string, string> data = new Param2<string, string>(ticketId, message);
		GetRPCManager().SendRPC("GPDA_Tickets", "SendMessage", data, true);
	}

	// Admin ticket selection moved to GPDA_AdminMenu
	
	// ═══════════════════════════════════════════════════════════════════════════════
	// CAPTURE SYSTEM METHODS
	// ═══════════════════════════════════════════════════════════════════════════════
	
	// Capture Zones methods removed - moved to GPDA_AdminMenu
	
	// Update group points display
	void UpdateGroupPoints(int points)
	{
		if (m_GroupPointsValue)
			m_GroupPointsValue.SetText(points.ToString());
	}
	
	// Update group zones list
	void UpdateGroupZones(array<string> zones)
	{
		if (!m_GroupZonesList) return;
		
		m_GroupZonesList.ClearItems();
		
		foreach (string zone : zones)
		{
			m_GroupZonesList.AddItem(zone, null, 0);
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════════
	// RANK SYSTEM UPDATE
	// ═══════════════════════════════════════════════════════════════════════════════
	
	void RequestRankData()
	{
		Object playerObj = GetGame().GetPlayer();
		if (!playerObj) return;
		GetRPCManager().SendRPC("GPDA_Rank", "RequestPlayerRank", null, true, null, playerObj);
	}
	
	// Admin rank creation method moved to GPDA_AdminMenu
	
	void UpdateRankPanel(string rankName, int xp, int nextXP)
	{
		if (m_CurrentRankName)
			m_CurrentRankName.SetText(rankName);
		
		if (m_CurrentXPValue)
			m_CurrentXPValue.SetText(xp.ToString() + " XP");
		
		// Progress bar
		if (m_RankProgressFill && nextXP > 0)
		{
			float progress = (float)xp / (float)nextXP;
			if (progress > 1.0) progress = 1.0;
			m_RankProgressFill.SetSize(progress, 1.0);
		}
		
		if (m_RankProgressText && nextXP > 0)
		{
			int remaining = nextXP - xp;
			if (remaining < 0) remaining = 0;
			m_RankProgressText.SetText(remaining.ToString() + " XP to next rank");
		}
	}
	
	// Quest system methods removed - moved to separate system
	
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER CLASS FOR DATABASE SEARCH RESULTS
// ═══════════════════════════════════════════════════════════════════════════════
class GPDA_PlayerSearchResult
{
	string Callsign;
	string Steam64;
	
	void GPDA_PlayerSearchResult(string callsign, string steam64)
	{
		Callsign = callsign;
		Steam64 = steam64;
	}
}
