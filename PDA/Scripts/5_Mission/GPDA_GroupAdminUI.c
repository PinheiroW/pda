// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA - GROUP ADMIN UI v2.0
// Admin panel for creating/editing groups (CTRL+K)
// Exact copy of MutantSpawnerUI patterns
// Vanilla reference: P:\scripts\5_Mission\GUI\ScriptConsole\ScriptConsoleItemsTab.c
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_GroupAdminUI extends UIScriptedMenu
{
	private static ref GPDA_GroupAdminUI s_Instance;
	
	// Root
	private Widget m_Root;
	
	// Header
	private ButtonWidget m_BtnCloseX;
	
	// Left: Group list
	private ScrollWidget m_GroupScroll;
	private GridSpacerWidget m_GroupList;
	private ButtonWidget m_BtnNewGroup;
	private ButtonWidget m_BtnDeleteGroup;
	private ButtonWidget m_BtnReloadGroups;
	private ButtonWidget m_BtnCloseBottom;
	private TextWidget m_StatsText;
	private TextWidget m_StatusText;
	
	// Center: Group editor fields
	private TextWidget m_EditorTitle;
	private EditBoxWidget m_EditGroupName;
	private EditBoxWidget m_EditLeaderID;
	private EditBoxWidget m_EditDeputyID;
	private EditBoxWidget m_EditFlagClass;
	private EditBoxWidget m_EditScore;
	private CheckBoxWidget m_ChkDbAccess;
	
	// Center: Members
	private TextWidget m_MemberTitle;
	private ScrollWidget m_MemberScroll;
	private GridSpacerWidget m_MemberList;
	private EditBoxWidget m_EditNewMember;
	private ButtonWidget m_BtnAddMember;
	private ButtonWidget m_BtnRemoveMember;
	
	// Center: Spawn Points
	private TextWidget m_SpawnTitle;
	private ScrollWidget m_SpawnScroll;
	private GridSpacerWidget m_SpawnList;
	private EditBoxWidget m_EditSpawnName;
	private ButtonWidget m_BtnMyPos;
	private ButtonWidget m_BtnAddSpawn;
	private ButtonWidget m_BtnRemoveSpawn;
	
	// Center: Items
	private TextWidget m_ItemTitle;
	private TextListboxWidget m_ItemListBox;
	private ButtonWidget m_BtnRemoveItem;
	private ButtonWidget m_BtnClearItems;
	private ButtonWidget m_BtnSaveGroup;
	private TextWidget m_CenterStatus;
	
	// Right: Entity Browser
	private EditBoxWidget m_SearchInput;
	private TextWidget m_EntityCountText;
	private TextListboxWidget m_EntityListBox;
	private ButtonWidget m_BtnAddToGroup;
	
	// Right: 3D Preview
	private ItemPreviewWidget m_ItemPreviewWidget;
	private TextWidget m_PreviewName;
	private EntityAI m_PreviewEntity;
	
	// Leader / Flag / Member selection
	private ButtonWidget m_BtnSelectLeader;
	private ButtonWidget m_BtnSelectFlag;
	private ButtonWidget m_BtnOnlineMember;
	private bool m_SelectingLeader = false;
	private bool m_SelectingFlag = false;
	private bool m_SelectingMember = false;
	
	// Data
	private ref array<ref GPDA_GroupData> m_Groups;
	private int m_SelectedGroupIndex = -1;
	private int m_SelectedMemberIndex = -1;
	private int m_SelectedSpawnIndex = -1;

	private string m_OriginalGroupName = "";
	private int m_ExpectedGroupCount = 0;
	
	// Cached player pos for MY POS
	private vector m_CachedPlayerPos;
	private bool m_HasCachedPos = false;
	
	// Dynamic rows
	private ref array<Widget> m_GroupRows;
	private ref array<Widget> m_MemberRows;
	private ref array<Widget> m_SpawnRows;
	
	// Search state
	private int m_SearchBoxCount = 0;
	
	// ═══════════════════════════════════════════════════════════════════════
	// STATIC CONTROL (same as MutantSpawnerUI)
	// ═══════════════════════════════════════════════════════════════════════
	
	static void Open()
	{
		if (!s_Instance)
			GetGame().GetUIManager().EnterScriptedMenu(MENU_GPDA_GROUP_ADMIN, null);
	}
	
	static void CloseMenu()
	{
		if (s_Instance)
			s_Instance.Close();
	}
	
	static bool IsOpen()
	{
		return s_Instance != null;
	}
	
	static GPDA_GroupAdminUI GetInstance()
	{
		return s_Instance;
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// INIT (FindAnyWidget exactly like spawner)
	// ═══════════════════════════════════════════════════════════════════════
	
	override Widget Init()
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/gpda_group_admin.layout");
		
		// Header
		m_BtnCloseX = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnCloseX"));
		
		// Left: Group list
		m_GroupScroll     = ScrollWidget.Cast(m_Root.FindAnyWidget("GroupScroll"));
		m_GroupList       = GridSpacerWidget.Cast(m_Root.FindAnyWidget("GroupList"));
		m_BtnNewGroup     = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnNewGroup"));
		m_BtnDeleteGroup  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnDeleteGroup"));
		m_BtnReloadGroups = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnReloadGroups"));
		m_BtnCloseBottom  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnCloseBottom"));
		m_StatsText       = TextWidget.Cast(m_Root.FindAnyWidget("StatsText"));
		m_StatusText      = TextWidget.Cast(m_Root.FindAnyWidget("StatusText"));
		
		// Center: Editor fields
		m_EditorTitle    = TextWidget.Cast(m_Root.FindAnyWidget("EditorTitle"));
		m_EditGroupName  = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditGroupName"));
		m_EditLeaderID   = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditLeaderID"));
		m_EditDeputyID   = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditDeputyID"));
		m_EditFlagClass  = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditFlagClass"));
		m_EditScore      = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditScore"));
		m_ChkDbAccess    = CheckBoxWidget.Cast(m_Root.FindAnyWidget("ChkDbAccess"));
		
		// Center: Members
		m_MemberTitle    = TextWidget.Cast(m_Root.FindAnyWidget("MemberTitle"));
		m_MemberScroll   = ScrollWidget.Cast(m_Root.FindAnyWidget("MemberScroll"));
		m_MemberList     = GridSpacerWidget.Cast(m_Root.FindAnyWidget("MemberList"));
		m_EditNewMember  = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditNewMember"));
		m_BtnAddMember   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAddMember"));
		m_BtnRemoveMember = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnRemoveMember"));
		
		// Center: Spawn Points
		m_SpawnTitle     = TextWidget.Cast(m_Root.FindAnyWidget("SpawnTitle"));
		m_SpawnScroll    = ScrollWidget.Cast(m_Root.FindAnyWidget("SpawnScroll"));
		m_SpawnList      = GridSpacerWidget.Cast(m_Root.FindAnyWidget("SpawnList"));
		m_EditSpawnName  = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditSpawnName"));
		m_BtnMyPos       = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnMyPos"));
		m_BtnAddSpawn    = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAddSpawn"));
		m_BtnRemoveSpawn = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnRemoveSpawn"));
		
		// Center: Items
		m_ItemTitle     = TextWidget.Cast(m_Root.FindAnyWidget("ItemTitle"));
		m_ItemListBox   = TextListboxWidget.Cast(m_Root.FindAnyWidget("ItemListBox"));
		m_BtnRemoveItem = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnRemoveItem"));
		m_BtnClearItems = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnClearItems"));
		m_BtnSaveGroup  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSaveGroup"));
		m_CenterStatus  = TextWidget.Cast(m_Root.FindAnyWidget("CenterStatus"));
		
		// Right: Entity Browser
		m_SearchInput     = EditBoxWidget.Cast(m_Root.FindAnyWidget("SearchInput"));
		m_EntityCountText = TextWidget.Cast(m_Root.FindAnyWidget("EntityCountText"));
		m_EntityListBox   = TextListboxWidget.Cast(m_Root.FindAnyWidget("EntityListBox"));
		m_BtnAddToGroup   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAddToGroup"));
		
		// Right: 3D Preview
		m_ItemPreviewWidget = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("ItemPreviewWidget"));
		m_PreviewName       = TextWidget.Cast(m_Root.FindAnyWidget("PreviewName"));
		
		// Leader / Flag selection buttons
		m_BtnSelectLeader = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSelectLeader"));
		m_BtnOnlineMember = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnOnlineMember"));
		m_BtnSelectFlag   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSelectFlag"));
		
		// Init data arrays
		m_Groups = new array<ref GPDA_GroupData>;
		m_GroupRows = new array<Widget>;
		m_MemberRows = new array<Widget>;
		m_SpawnRows = new array<Widget>;
		
		s_Instance = this;
		
		return m_Root;
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// SHOW / HIDE (same as MutantSpawnerUI)
	// ═══════════════════════════════════════════════════════════════════════
	
	override void OnShow()
	{
		super.OnShow();
		s_Instance = this;
		SetFocus(m_Root);
		m_Root.Show(true);
		
		GetGame().GetInput().ChangeGameFocus(1);
		GetGame().GetUIManager().ShowCursor(true);
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		
		RequestGroupsFromServer();
		UpdateEntityBrowser();
		
		SetStatus("Loading groups from server...");
	}
	
	override void OnHide()
	{
		super.OnHide();
		
		if (GetGame() && GetGame().GetMission())
			GetGame().GetMission().PlayerControlEnable(false);
		
		GetGame().GetInput().ChangeGameFocus(-1);
		GetGame().GetUIManager().ShowCursor(false);
		GetGame().GetInput().ResetGameFocus();
		
		ClearGroupRows();
		ClearMemberRows();
		ClearSpawnRows();
		CleanupPreview();
		
		m_Root.Show(false);
		s_Instance = null;
	}
	
	override bool UseKeyboard() { return true; }
	override bool UseMouse() { return true; }
	
	// ═══════════════════════════════════════════════════════════════════════
	// UPDATE (real-time search, same as SpawnerUI)
	// ═══════════════════════════════════════════════════════════════════════
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		if (!m_SearchInput) return;
		int newLen = m_SearchInput.GetText().Length();
		if (newLen != m_SearchBoxCount)
		{
			m_SearchBoxCount = newLen;
			UpdateEntityBrowser();
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// CLICKS (same pattern as SpawnerUI OnClick)
	// ═══════════════════════════════════════════════════════════════════════
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!w) return false;
		
		// Header close
		if (w == m_BtnCloseX)        { CloseMenu(); return true; }
		if (w == m_BtnCloseBottom)   { CloseMenu(); return true; }
		
		// Left: Group actions
		if (w == m_BtnNewGroup)      { CreateNewGroup(); return true; }
		if (w == m_BtnDeleteGroup)   { DeleteSelectedGroup(); return true; }
		if (w == m_BtnReloadGroups)  { RequestGroupsFromServer(); return true; }
		
		// Center: Members
		if (w == m_BtnAddMember)     { AddMember(); return true; }
		if (w == m_BtnRemoveMember)  { RemoveSelectedMember(); return true; }
		
		// Center: Spawn Points
		if (w == m_BtnMyPos)         { SetPositionFromPlayer(); return true; }
		if (w == m_BtnAddSpawn)      { AddSpawnPoint(); return true; }
		if (w == m_BtnRemoveSpawn)   { RemoveSelectedSpawn(); return true; }
		
		// Center: Items
		if (w == m_BtnRemoveItem)    { RemoveSelectedItem(); return true; }
		if (w == m_BtnClearItems)    { ClearAllItems(); return true; }
		if (w == m_BtnSaveGroup)     { SaveCurrentGroup(); return true; }
		
		// Right: Entity Browser
		if (w == m_BtnAddToGroup)    { AddSelectedEntityToLoadout(); return true; }
		
		// Leader / Flag / Member selection
		if (w == m_BtnSelectLeader)  { OnSelectLeader(); return true; }
		if (w == m_BtnSelectFlag)    { OnSelectFlag(); return true; }
		if (w == m_BtnOnlineMember)  { OnSelectMember(); return true; }
		
		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (w == m_EntityListBox && button == MouseState.LEFT)
		{
			if (m_SelectingLeader) { OnLeaderPicked(); return true; }
			if (m_SelectingFlag) { OnFlagPicked(); return true; }
			if (m_SelectingMember) { OnMemberPicked(); return true; }
			AddSelectedEntityToLoadout();
			return true;
		}
		return super.OnDoubleClick(w, x, y, button);
	}
	
	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			CloseMenu();
			return true;
		}
		return false;
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// MOUSE CLICK ON ROWS (groups, members, spawns)
	// Same pattern as SpawnerUI OnMouseButtonDown
	// ═══════════════════════════════════════════════════════════════════════
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT) return false;
		if (!w) return false;
		
		Widget check = w;
		int maxDepth = 5;
		
		while (check && maxDepth > 0)
		{
			Widget parent = check.GetParent();
			
			if (parent == m_GroupList)
			{
				SelectGroup(check.GetUserID());
				return true;
			}
			
			if (parent == m_MemberList)
			{
				m_SelectedMemberIndex = check.GetUserID();
				HighlightMemberRow(check.GetUserID());
				return true;
			}
			
			if (parent == m_SpawnList)
			{
				m_SelectedSpawnIndex = check.GetUserID();
				HighlightSpawnRow(check.GetUserID());
				return true;
			}
			
			check = parent;
			maxDepth--;
		}
		
		return false;
	}
	
	// Entity listbox selection changed -> update 3D preview
	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		if (w == m_EntityListBox)
		{
			// Preview works for normal items and flags, not for player list
			if (!m_SelectingLeader && !m_SelectingMember)
				UpdatePreview();
			return true;
		}
		return false;
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// ENTITY BROWSER (CfgVehicles scan - scope 2+ items)
	// Same pattern as MutantSpawnerUI
	// ═══════════════════════════════════════════════════════════════════════
	
	void UpdateEntityBrowser()
	{
		if (!m_EntityListBox) return;
		
		// Reset all selection modes when user searches manually
		m_SelectingLeader = false;
		m_SelectingFlag = false;
		m_SelectingMember = false;
		
		m_EntityListBox.ClearItems();
		
		string strSearch = "";
		if (m_SearchInput)
			strSearch = m_SearchInput.GetText();
		strSearch.ToLower();
		
		// Skip default text
		if (strSearch == "search...")
			strSearch = "";
		
		string cfgPath = "CfgVehicles";
		int nClasses = g_Game.ConfigGetChildrenCount(cfgPath);
		int foundCount = 0;
		
		for (int i = 0; i < nClasses; i++)
		{
			string className;
			GetGame().ConfigGetChildName(cfgPath, i, className);
			
			int scope = GetGame().ConfigGetInt(cfgPath + " " + className + " scope");
			if (scope < 2)
				continue;
			
			if (strSearch != "")
			{
				string lowerName = className;
				lowerName.ToLower();
				if (!lowerName.Contains(strSearch))
					continue;
			}
			
			m_EntityListBox.AddItem(className, NULL, 0);
			foundCount++;
		}
		
		if (m_EntityCountText)
			m_EntityCountText.SetText(foundCount.ToString() + " items");
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// 3D PREVIEW
	// Vanilla reference: P:\scripts\5_Mission\GUI\ScriptConsole\ScriptConsoleItemsTab.c
	// Uses CreateObjectEx + dBodyDestroy + DisableSimulation + SetItem
	// ═══════════════════════════════════════════════════════════════════════
	
	void UpdatePreview()
	{
		if (!m_ItemPreviewWidget) return;
		
		int row = m_EntityListBox.GetSelectedRow();
		if (row == -1) return;
		
		string className;
		m_EntityListBox.GetItemText(row, 0, className);
		
		CleanupPreview();
		
		// Create entity for preview (vanilla pattern from ScriptConsoleItemsTab.c)
		m_PreviewEntity = EntityAI.Cast(GetGame().CreateObjectEx(className, "0 0 0", ECE_LOCAL | ECE_CREATEPHYSICS | ECE_TRACE));
		if (m_PreviewEntity)
		{
			dBodyDestroy(m_PreviewEntity);
			m_PreviewEntity.DisableSimulation(true);
			m_ItemPreviewWidget.SetItem(m_PreviewEntity);
			m_ItemPreviewWidget.SetModelOrientation(Vector(0, 0, 0));
			m_ItemPreviewWidget.SetView(m_PreviewEntity.GetViewIndex());
		}
		
		if (m_PreviewName)
			m_PreviewName.SetText(className);
	}
	
	void CleanupPreview()
	{
		if (m_PreviewEntity)
		{
			if (m_ItemPreviewWidget)
				m_ItemPreviewWidget.SetItem(null);
			GetGame().ObjectDelete(m_PreviewEntity);
			m_PreviewEntity = null;
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// GROUP MANAGEMENT
	// ═══════════════════════════════════════════════════════════════════════
	
	void CreateNewGroup()
	{
		string groupName = "";
		if (m_EditGroupName)
		{
			groupName = m_EditGroupName.GetText();
			groupName = groupName.Trim();
		}
		if (groupName == "")
		{
			int count = m_Groups.Count() + 1;
			groupName = "NewGroup_" + count.ToString();
		}
		
		// Check duplicates
		for (int d = 0; d < m_Groups.Count(); d++)
		{
			if (m_Groups[d].Name == groupName)
			{
				groupName = groupName + "_" + (m_Groups.Count() + 1).ToString();
				break;
			}
		}
		
		ref GPDA_GroupData group = new GPDA_GroupData();
		group.Name = groupName;
		group.JsonVersion = 2;
		group.ItemsInitialized = true;
		
		m_Groups.Insert(group);
		
		RefreshGroupList();
		SelectGroup(m_Groups.Count() - 1);
		SetCenterStatus("Created: " + groupName + " (save to persist)");
	}
	
	void DeleteSelectedGroup()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("No group selected!");
			return;
		}
		
		string name = m_Groups[m_SelectedGroupIndex].Name;
		
		// Tell server to delete
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			ref Param1<string> params = new Param1<string>(name);
			GetGame().RPCSingleParam(player, GPDA_RPC.GRP_DELETE_GROUP, params, true);
		}
		
		m_Groups.Remove(m_SelectedGroupIndex);
		m_SelectedGroupIndex = -1;
		m_SelectedMemberIndex = -1;
		m_SelectedSpawnIndex = -1;
		
		RefreshGroupList();
		ClearEditor();
		
		SetStatus("Deleted: " + name);
	}
	
	void SelectGroup(int index)
	{
		if (index < 0 || index >= m_Groups.Count()) return;
		
		m_SelectedGroupIndex = index;
		m_SelectedMemberIndex = -1;
		m_SelectedSpawnIndex = -1;
		
		GPDA_GroupData group = m_Groups[index];
		m_OriginalGroupName = group.Name;
		
		// Populate editor fields
		if (m_EditorTitle)
		{
			m_EditorTitle.SetText("EDITING: " + group.Name);
			m_EditorTitle.SetColor(ARGB(255, 220, 190, 130));
		}
		if (m_EditGroupName) m_EditGroupName.SetText(group.Name);
		if (m_EditLeaderID) m_EditLeaderID.SetText(group.LeaderID);
		if (m_EditDeputyID) m_EditDeputyID.SetText(group.DeputyID);
		if (m_EditFlagClass) m_EditFlagClass.SetText(group.FlagClass);
		if (m_EditScore) m_EditScore.SetText(group.Score.ToString());
		
		if (m_ChkDbAccess) m_ChkDbAccess.SetChecked(group.DatabaseAccess);
		
		// Highlight selected group row
		HighlightGroupRow(index);
		
		RefreshMemberList();
		RefreshSpawnList();
		RefreshItemList();
	}
	
	void HighlightGroupRow(int index)
	{
		for (int i = 0; i < m_GroupRows.Count(); i++)
		{
			Widget row = m_GroupRows[i];
			if (!row) continue;
			Widget bg = Widget.Cast(row.FindAnyWidget("GrpRowBG"));
			if (bg)
			{
				if (i == index)
					bg.SetColor(ARGB(80, 200, 50, 30));
				else
					bg.SetColor(ARGB(0, 0, 0, 0));
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// ═══════════════════════════════════════════════════════════════════════
	// MEMBERS
	// ═══════════════════════════════════════════════════════════════════════
	
	void AddMember()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("Select a group first!");
			return;
		}
		
		string steam64 = "";
		if (m_EditNewMember) steam64 = m_EditNewMember.GetText();
		steam64 = steam64.Trim();
		
		if (steam64 == "")
		{
			SetCenterStatus("Enter Steam64 ID!");
			return;
		}
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		
		// Check duplicate
		if (group.Members)
		{
			for (int i = 0; i < group.Members.Count(); i++)
			{
				if (group.Members[i] == steam64)
				{
					SetCenterStatus("Already a member!");
					return;
				}
			}
		}
		
		group.AddMember(steam64);
		if (m_EditNewMember) m_EditNewMember.SetText("");
		
		RefreshMemberList();
		SetCenterStatus("Added member: " + steam64);
	}
	
	void RemoveSelectedMember()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("No group selected!");
			return;
		}
		if (m_SelectedMemberIndex < 0)
		{
			SetCenterStatus("Select a member to remove!");
			return;
		}
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.Members || m_SelectedMemberIndex >= group.Members.Count()) return;
		
		string removed = group.Members[m_SelectedMemberIndex];
		group.Members.Remove(m_SelectedMemberIndex);
		m_SelectedMemberIndex = -1;
		
		RefreshMemberList();
		SetCenterStatus("Removed: " + removed);
	}
	
	void HighlightMemberRow(int index)
	{
		for (int i = 0; i < m_MemberRows.Count(); i++)
		{
			Widget row = m_MemberRows[i];
			if (!row) continue;
			Widget bg = Widget.Cast(row.FindAnyWidget("GrpRowBG"));
			if (bg)
			{
				if (i == index)
					bg.SetColor(ARGB(60, 100, 150, 200));
				else
					bg.SetColor(ARGB(0, 0, 0, 0));
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// SPAWN POINTS
	// Fixed: SetPositionFromPlayer now caches position like MutantSpawnerUI
	// Fixed: AddSpawnPoint uses cached position
	// ═══════════════════════════════════════════════════════════════════════
	
	void SetPositionFromPlayer()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			SetCenterStatus("No player found!");
			return;
		}
		
		m_CachedPlayerPos = player.GetPosition();
		m_HasCachedPos = true;
		
		SetCenterStatus("Position set: " + m_CachedPlayerPos[0].ToString() + ", " + m_CachedPlayerPos[1].ToString() + ", " + m_CachedPlayerPos[2].ToString());
	}
	
	void AddSpawnPoint()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("Select a group first!");
			return;
		}
		
		string spawnName = "";
		if (m_EditSpawnName) spawnName = m_EditSpawnName.GetText();
		spawnName = spawnName.Trim();
		if (spawnName == "") spawnName = "Spawn";
		
		// Use cached pos from MY POS, or get current player position
		vector pos;
		if (m_HasCachedPos)
		{
			pos = m_CachedPlayerPos;
		}
		else
		{
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			if (!player)
			{
				SetCenterStatus("Press MY POS first or stand in position!");
				return;
			}
			pos = player.GetPosition();
		}
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.SpawnPoints) group.SpawnPoints = new array<ref GPDA_SpawnPoint>;
		
		ref GPDA_SpawnPoint sp = new GPDA_SpawnPoint();
		sp.Name = spawnName;
		sp.Position = pos;
		group.SpawnPoints.Insert(sp);
		
		m_HasCachedPos = false;
		
		RefreshSpawnList();
		SetCenterStatus("Added: " + spawnName + " [" + pos[0].ToString() + " " + pos[1].ToString() + " " + pos[2].ToString() + "]");
	}
	
	void RemoveSelectedSpawn()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("No group selected!");
			return;
		}
		if (m_SelectedSpawnIndex < 0)
		{
			SetCenterStatus("Select a spawn point to remove!");
			return;
		}
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.SpawnPoints || m_SelectedSpawnIndex >= group.SpawnPoints.Count()) return;
		
		string removed = group.SpawnPoints[m_SelectedSpawnIndex].Name;
		group.SpawnPoints.Remove(m_SelectedSpawnIndex);
		m_SelectedSpawnIndex = -1;
		
		RefreshSpawnList();
		SetCenterStatus("Removed spawn: " + removed);
	}
	
	void HighlightSpawnRow(int index)
	{
		for (int i = 0; i < m_SpawnRows.Count(); i++)
		{
			Widget row = m_SpawnRows[i];
			if (!row) continue;
			Widget bg = Widget.Cast(row.FindAnyWidget("GrpRowBG"));
			if (bg)
			{
				if (i == index)
					bg.SetColor(ARGB(60, 100, 200, 100));
				else
					bg.SetColor(ARGB(0, 0, 0, 0));
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// ITEMS (LOADOUT)
	// ═══════════════════════════════════════════════════════════════════════
	
	void AddSelectedEntityToLoadout()
	{
		if (!m_EntityListBox) return;
		
		// If in selection mode, delegate to picker instead
		if (m_SelectingLeader) { OnLeaderPicked(); return; }
		if (m_SelectingFlag) { OnFlagPicked(); return; }
		if (m_SelectingMember) { OnMemberPicked(); return; }
		
		int row = m_EntityListBox.GetSelectedRow();
		if (row == -1)
		{
			SetCenterStatus("Select an entity first!");
			return;
		}
		
		string entityClass;
		m_EntityListBox.GetItemText(row, 0, entityClass);
		
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("Select a group first!");
			return;
		}
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.Items) group.Items = new array<string>;
		
		group.Items.Insert(entityClass);
		group.ItemsInitialized = true;
		
		RefreshItemList();
		SetCenterStatus("+ " + entityClass);
	}
	
	void RemoveSelectedItem()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count()) return;
		if (!m_ItemListBox) return;
		
		int row = m_ItemListBox.GetSelectedRow();
		if (row == -1)
		{
			SetCenterStatus("Select an item to remove!");
			return;
		}
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.Items || row >= group.Items.Count()) return;
		
		string removed = group.Items[row];
		group.Items.Remove(row);
		
		RefreshItemList();
		SetCenterStatus("Removed: " + removed);
	}
	
	void ClearAllItems()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count()) return;
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (group.Items) group.Items.Clear();
		
		RefreshItemList();
		SetCenterStatus("All items cleared");
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// SAVE (RPC to server, same pattern as MutantSpawnerUI SaveZone)
	// ═══════════════════════════════════════════════════════════════════════
	
	void SaveCurrentGroup()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("No group selected!");
			return;
		}
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		
		// Read fields from editor
		if (m_EditGroupName)
		{
			string newName = m_EditGroupName.GetText();
			newName = newName.Trim();
			if (newName.Length() > 0) group.Name = newName;
		}
		if (m_EditLeaderID) group.LeaderID = m_EditLeaderID.GetText();
		if (m_EditDeputyID) group.DeputyID = m_EditDeputyID.GetText();
		if (m_EditFlagClass) group.FlagClass = m_EditFlagClass.GetText();
		if (m_EditScore) group.Score = m_EditScore.GetText().ToInt();
		if (m_ChkDbAccess) group.DatabaseAccess = m_ChkDbAccess.IsChecked();
		group.JsonVersion = 2;
		
		// Send via RPC (same pattern as MutantSpawnerUI)
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		
		string jsonData;
		JsonSerializer().WriteToString(group, false, jsonData);
		
		string sendName = m_OriginalGroupName;
		if (sendName == "") sendName = group.Name;
		
		ref Param2<string, string> rpcParams = new Param2<string, string>(sendName, jsonData);
		GetGame().RPCSingleParam(player, GPDA_RPC.GRP_SAVE_GROUP, rpcParams, true);
		
		m_OriginalGroupName = group.Name;
		
		RefreshGroupList();
		SetCenterStatus("Saving: " + group.Name + "...");
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// LIST RENDERING (same row approach as MutantSpawnerUI RefreshZoneList)
	// Using gpda_grp_row.layout - matches gpda_msp_zone_row.layout style
	// ═══════════════════════════════════════════════════════════════════════
	
	void ClearGroupRows()
	{
		for (int i = 0; i < m_GroupRows.Count(); i++)
		{
			Widget w = m_GroupRows[i];
			if (w) w.Unlink();
		}
		m_GroupRows.Clear();
	}
	
	void ClearMemberRows()
	{
		for (int i = 0; i < m_MemberRows.Count(); i++)
		{
			Widget w = m_MemberRows[i];
			if (w) w.Unlink();
		}
		m_MemberRows.Clear();
	}
	
	void ClearSpawnRows()
	{
		for (int i = 0; i < m_SpawnRows.Count(); i++)
		{
			Widget w = m_SpawnRows[i];
			if (w) w.Unlink();
		}
		m_SpawnRows.Clear();
	}
	
	void RefreshGroupList()
	{
		ClearGroupRows();
		
		int totalMembers = 0;
		
		for (int i = 0; i < m_Groups.Count(); i++)
		{
			GPDA_GroupData group = m_Groups[i];
			
			Widget row = GetGame().GetWorkspace().CreateWidgets(
				"Greshnik_PDALITE/GUI/Layouts/gpda_grp_row.layout",
				m_GroupList);
			if (!row) continue;
			
			row.SetUserID(i);
			
			TextWidget nameW = TextWidget.Cast(row.FindAnyWidget("GrpRowName"));
			TextWidget infoW = TextWidget.Cast(row.FindAnyWidget("GrpRowInfo"));
			Widget dotW = Widget.Cast(row.FindAnyWidget("GrpStatusDot"));
			
			if (nameW) nameW.SetText(group.Name);
			
			int memCount = 0;
			if (group.Members) memCount = group.Members.Count();
			totalMembers += memCount;
			
			if (infoW) infoW.SetText(memCount.ToString() + " mem");
			
			// Green dot if has members, red if empty
			if (dotW)
			{
				if (memCount > 0)
					dotW.SetColor(ARGB(255, 40, 170, 65));
				else
					dotW.SetColor(ARGB(255, 170, 40, 40));
			}
			
			if (i == m_SelectedGroupIndex)
			{
				Widget bg = Widget.Cast(row.FindAnyWidget("GrpRowBG"));
				if (bg) bg.SetColor(ARGB(80, 200, 50, 30));
			}
			
			m_GroupRows.Insert(row);
		}
		
		if (m_StatsText)
			m_StatsText.SetText("Groups: " + m_Groups.Count().ToString() + " | Members: " + totalMembers.ToString());
	}
	
	void RefreshMemberList()
	{
		ClearMemberRows();
		
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count()) return;
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.Members) return;
		
		for (int i = 0; i < group.Members.Count(); i++)
		{
			Widget row = GetGame().GetWorkspace().CreateWidgets(
				"Greshnik_PDALITE/GUI/Layouts/gpda_grp_row.layout",
				m_MemberList);
			if (!row) continue;
			
			row.SetUserID(i);
			
			TextWidget nameW = TextWidget.Cast(row.FindAnyWidget("GrpRowName"));
			TextWidget infoW = TextWidget.Cast(row.FindAnyWidget("GrpRowInfo"));
			Widget dotW = Widget.Cast(row.FindAnyWidget("GrpStatusDot"));
			
			if (nameW) nameW.SetText(group.Members[i]);
			
			// Show role
			if (infoW)
			{
				if (group.Members[i] == group.LeaderID)
					infoW.SetText("[LEADER]");
				else if (group.Members[i] == group.DeputyID)
					infoW.SetText("[DEPUTY]");
				else
					infoW.SetText("");
			}
			
			// Dot color: yellow for leader, blue for deputy, grey for member
			if (dotW)
			{
				if (group.Members[i] == group.LeaderID)
					dotW.SetColor(ARGB(255, 200, 180, 40));
				else if (group.Members[i] == group.DeputyID)
					dotW.SetColor(ARGB(255, 40, 120, 200));
				else
					dotW.SetColor(ARGB(255, 100, 100, 100));
			}
			
			m_MemberRows.Insert(row);
		}
		
		// Update header
		if (m_MemberTitle)
			m_MemberTitle.SetText("MEMBERS (" + group.Members.Count().ToString() + ")");
	}
	
	void RefreshSpawnList()
	{
		ClearSpawnRows();
		
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count()) return;
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.SpawnPoints) return;
		
		for (int i = 0; i < group.SpawnPoints.Count(); i++)
		{
			GPDA_SpawnPoint sp = group.SpawnPoints[i];
			
			Widget row = GetGame().GetWorkspace().CreateWidgets(
				"Greshnik_PDALITE/GUI/Layouts/gpda_grp_row.layout",
				m_SpawnList);
			if (!row) continue;
			
			row.SetUserID(i);
			
			TextWidget nameW = TextWidget.Cast(row.FindAnyWidget("GrpRowName"));
			TextWidget infoW = TextWidget.Cast(row.FindAnyWidget("GrpRowInfo"));
			Widget dotW = Widget.Cast(row.FindAnyWidget("GrpStatusDot"));
			
			if (nameW)
			{
				nameW.SetText(sp.Name);
				nameW.SetColor(ARGB(255, 220, 190, 130));
			}
			
			if (infoW)
			{
				string posStr = sp.Position[0].ToString() + " " + sp.Position[1].ToString() + " " + sp.Position[2].ToString();
				infoW.SetText(posStr);
			}
			
			if (dotW)
				dotW.SetColor(ARGB(255, 40, 170, 65));
			
			m_SpawnRows.Insert(row);
		}
		
		// Update header
		if (m_SpawnTitle)
			m_SpawnTitle.SetText("SPAWN POINTS (" + group.SpawnPoints.Count().ToString() + ")");
	}
	
	void RefreshItemList()
	{
		if (!m_ItemListBox) return;
		m_ItemListBox.ClearItems();
		
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count()) return;
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		if (!group.Items) return;
		
		for (int i = 0; i < group.Items.Count(); i++)
		{
			m_ItemListBox.AddItem(group.Items[i], NULL, 0);
		}
		
		// Update header
		if (m_ItemTitle)
			m_ItemTitle.SetText("LOADOUT ITEMS (" + group.Items.Count().ToString() + ")");
	}
	
	void ClearEditor()
	{
		if (m_EditorTitle)
		{
			m_EditorTitle.SetText("GROUP EDITOR");
			m_EditorTitle.SetColor(ARGB(255, 255, 255, 255));
		}
		if (m_EditGroupName) m_EditGroupName.SetText("");
		if (m_EditLeaderID) m_EditLeaderID.SetText("");
		if (m_EditDeputyID) m_EditDeputyID.SetText("");
		if (m_EditFlagClass) m_EditFlagClass.SetText("");
		if (m_EditScore) m_EditScore.SetText("0");
		if (m_ChkDbAccess) m_ChkDbAccess.SetChecked(false);
		ClearMemberRows();
		ClearSpawnRows();
		if (m_ItemListBox) m_ItemListBox.ClearItems();
		if (m_MemberTitle) m_MemberTitle.SetText("MEMBERS (0)");
		if (m_SpawnTitle) m_SpawnTitle.SetText("SPAWN POINTS (0)");
		if (m_ItemTitle) m_ItemTitle.SetText("LOADOUT ITEMS (0)");
		m_OriginalGroupName = "";
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// STATUS (same as MutantSpawnerUI)
	// ═══════════════════════════════════════════════════════════════════════
	
	void SetStatus(string msg)
	{
		if (m_StatusText) m_StatusText.SetText(msg);
	}
	
	void SetCenterStatus(string msg)
	{
		if (m_CenterStatus) m_CenterStatus.SetText(msg);
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// RPC: CLIENT -> SERVER
	// ═══════════════════════════════════════════════════════════════════════
	
	void RequestGroupsFromServer()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		GetGame().RPCSingleParam(player, GPDA_RPC.GRP_REQUEST_ALL, null, true);
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// RPC: SERVER -> CLIENT (called from GPDA_ClientRPC)
	// Same pattern as MutantSpawnerUI HandleReceiveZones
	// ═══════════════════════════════════════════════════════════════════════
	
	void HandleReceiveCount(ParamsReadContext ctx)
	{
		ref Param1<int> data = new Param1<int>(0);
		if (!ctx.Read(data)) return;
		
		m_ExpectedGroupCount = data.param1;
		m_Groups.Clear();
		m_SelectedGroupIndex = -1;
		
		Print("[GPDA][UI] GroupAdmin: Expecting " + m_ExpectedGroupCount.ToString() + " groups");
		
		if (m_ExpectedGroupCount == 0)
		{
			RefreshGroupList();
			SetStatus("No groups on server");
		}
		else
		{
			SetStatus("Loading " + m_ExpectedGroupCount.ToString() + " groups...");
		}
	}
	
	void HandleReceiveOne(ParamsReadContext ctx)
	{
		ref Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data)) return;
		
		string jsonStr = data.param1;
		if (jsonStr == "") return;
		
		string jsonErr;
		ref GPDA_GroupData group;
		if (JsonSerializer().ReadFromString(group, jsonStr, jsonErr))
		{
			if (group)
				m_Groups.Insert(group);
		}
		else
		{
			Print("[GPDA][UI] GroupAdmin: Failed to parse group JSON: " + jsonErr);
		}
		
		// When all received, refresh UI
		if (m_Groups.Count() >= m_ExpectedGroupCount)
		{
			RefreshGroupList();
			if (m_Groups.Count() > 0 && m_SelectedGroupIndex == -1)
				SelectGroup(0);
			SetStatus("Loaded " + m_Groups.Count().ToString() + " groups");
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// LEADER SELECTION FROM ONLINE PLAYERS
	// Pattern: AdminMenu.OnZoneSelectOwner - state flag + repurpose EntityListBox
	// ═══════════════════════════════════════════════════════════════════════
	
	void OnSelectLeader()
	{
		m_SelectingLeader = true;
		m_SelectingFlag = false;
		m_SelectingMember = false;
		
		if (m_EntityListBox)
		{
			m_EntityListBox.ClearItems();
			m_EntityListBox.AddItem("Loading online players...", NULL, 0);
		}
		if (m_EntityCountText)
			m_EntityCountText.SetText("ONLINE PLAYERS");
		
		// Request online players from server
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			GetGame().RPCSingleParam(player, GPDA_RPC.GRP_REQUEST_PLAYERS, null, true);
		
		SetCenterStatus("Select a player from the list...");
	}
	
	void HandleReceivePlayers(ParamsReadContext ctx)
	{
		ref Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data)) return;
		
		if (!m_EntityListBox) return;
		m_EntityListBox.ClearItems();
		
		string raw = data.param1;
		if (raw == "")
		{
			m_EntityListBox.AddItem("No players online", NULL, 0);
			if (m_EntityCountText)
				m_EntityCountText.SetText("0 players online");
			return;
		}
		
		// Format: "name:steam64|name2:steam64_2|..."
		array<string> entries = new array<string>;
		raw.Split("|", entries);
		
		for (int i = 0; i < entries.Count(); i++)
		{
			array<string> parts = new array<string>;
			entries[i].Split(":", parts);
			if (parts.Count() >= 2)
			{
				string displayText = parts[0] + "  [" + parts[1] + "]";
				m_EntityListBox.AddItem(displayText, NULL, 0);
			}
		}
		
		if (m_EntityCountText)
			m_EntityCountText.SetText(entries.Count().ToString() + " players online");
		
		SetCenterStatus("Double-click a player to set as leader");
	}
	
	void OnLeaderPicked()
	{
		if (!m_EntityListBox) return;
		
		int row = m_EntityListBox.GetSelectedRow();
		if (row == -1) return;
		
		string selectedText;
		m_EntityListBox.GetItemText(row, 0, selectedText);
		
		// Extract steam64 from "name  [steam64]"
		int bracketStart = selectedText.IndexOf("[");
		int bracketEnd = selectedText.IndexOf("]");
		if (bracketStart >= 0 && bracketEnd > bracketStart)
		{
			string steam64 = selectedText.Substring(bracketStart + 1, bracketEnd - bracketStart - 1);
			if (m_EditLeaderID)
				m_EditLeaderID.SetText(steam64);
			SetCenterStatus("Leader set: " + steam64);
		}
		
		// Reset to normal entity browser
		m_SelectingLeader = false;
		UpdateEntityBrowser();
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// MEMBER SELECTION FROM ONLINE PLAYERS
	// Same pattern as leader — but adds member to group instead of setting leader
	// ═══════════════════════════════════════════════════════════════════════
	
	void OnSelectMember()
	{
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count())
		{
			SetCenterStatus("Select a group first!");
			return;
		}
		
		m_SelectingMember = true;
		m_SelectingLeader = false;
		m_SelectingFlag = false;
		
		if (m_EntityListBox)
		{
			m_EntityListBox.ClearItems();
			m_EntityListBox.AddItem("Loading online players...", NULL, 0);
		}
		if (m_EntityCountText)
			m_EntityCountText.SetText("ONLINE PLAYERS");
		
		// Request online players from server (same RPC as leader)
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			GetGame().RPCSingleParam(player, GPDA_RPC.GRP_REQUEST_PLAYERS, null, true);
		
		SetCenterStatus("Double-click a player to add as member...");
	}
	
	void OnMemberPicked()
	{
		if (!m_EntityListBox) return;
		
		int row = m_EntityListBox.GetSelectedRow();
		if (row == -1) return;
		
		string selectedText;
		m_EntityListBox.GetItemText(row, 0, selectedText);
		
		// Extract steam64 from "name  [steam64]"
		int bracketStart = selectedText.IndexOf("[");
		int bracketEnd = selectedText.IndexOf("]");
		if (bracketStart < 0 || bracketEnd <= bracketStart) return;
		
		string steam64 = selectedText.Substring(bracketStart + 1, bracketEnd - bracketStart - 1);
		if (steam64 == "") return;
		
		if (m_SelectedGroupIndex < 0 || m_SelectedGroupIndex >= m_Groups.Count()) return;
		
		GPDA_GroupData group = m_Groups[m_SelectedGroupIndex];
		
		// Check duplicate
		if (group.Members)
		{
			for (int i = 0; i < group.Members.Count(); i++)
			{
				if (group.Members[i] == steam64)
				{
					SetCenterStatus("Already a member!");
					return;
				}
			}
		}
		
		group.AddMember(steam64);
		RefreshMemberList();
		
		// Extract name for status
		string playerName = selectedText.Substring(0, bracketStart).Trim();
		SetCenterStatus("Added member: " + playerName + " (" + steam64 + ")");
		
		// Stay in member selection mode so you can add more
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// FLAG SELECTION FROM CFGVEHICLES
	// Filter entity browser to show only flag classes (client-side only)
	// ═══════════════════════════════════════════════════════════════════════
	
	void OnSelectFlag()
	{
		m_SelectingFlag = true;
		m_SelectingLeader = false;
		m_SelectingMember = false;
		
		if (!m_EntityListBox) return;
		m_EntityListBox.ClearItems();
		
		string cfgPath = "CfgVehicles";
		int nClasses = g_Game.ConfigGetChildrenCount(cfgPath);
		int foundCount = 0;
		
		for (int i = 0; i < nClasses; i++)
		{
			string className;
			GetGame().ConfigGetChildName(cfgPath, i, className);
			
			int scope = GetGame().ConfigGetInt(cfgPath + " " + className + " scope");
			if (scope < 2) continue;
			
			string lowerName = className;
			lowerName.ToLower();
			if (!lowerName.Contains("flag")) continue;
			
			m_EntityListBox.AddItem(className, NULL, 0);
			foundCount++;
		}
		
		if (m_EntityCountText)
			m_EntityCountText.SetText(foundCount.ToString() + " flags found");
		
		SetCenterStatus("Double-click a flag to set for group");
	}
	
	void OnFlagPicked()
	{
		if (!m_EntityListBox) return;
		
		int row = m_EntityListBox.GetSelectedRow();
		if (row == -1) return;
		
		string className;
		m_EntityListBox.GetItemText(row, 0, className);
		
		if (m_EditFlagClass)
			m_EditFlagClass.SetText(className);
		
		SetCenterStatus("Flag set: " + className);
		
		// Reset to normal entity browser
		m_SelectingFlag = false;
		UpdateEntityBrowser();
	}
	
	// ═══════════════════════════════════════════════════════════════════════
	// RPC: SERVER -> CLIENT STATUS
	// ═══════════════════════════════════════════════════════════════════════
	
	void HandleStatus(ParamsReadContext ctx)
	{
		ref Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data)) return;
		
		string msg = data.param1;
		
		// Admin access denied - close menu automatically
		if (msg == "ACCESS_DENIED")
		{
			SetStatus("Access denied! Admin role required.");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.CloseMenuDelayed, 1500, false);
			return;
		}
		
		SetStatus(msg);
		SetCenterStatus(msg);
	}
	
	void CloseMenuDelayed()
	{
		CloseMenu();
	}
}
