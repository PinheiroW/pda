// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
// GRESHNIK PDA - MUTANT SPAWNER UI v2.0
// VPP-style admin tool with Entity Browser
// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’

class GPDA_MutantSpawnerUI extends UIScriptedMenu
{
	private static ref GPDA_MutantSpawnerUI s_Instance;
	
	// РІвЂќР‚РІвЂќР‚ Root РІвЂќР‚РІвЂќР‚
	private Widget m_Root;
	
	// РІвЂќР‚РІвЂќР‚ Header РІвЂќР‚РІвЂќР‚
	private ButtonWidget m_BtnCloseX;
	
	// РІвЂќР‚РІвЂќР‚ Tab Buttons РІвЂќР‚РІвЂќР‚
	private ButtonWidget m_BtnTabZones;
	private ButtonWidget m_BtnTabPresets;
	private int m_CurrentTab = 0; // 0=zones, 1=presets
	
	// РІвЂќР‚РІвЂќР‚ Left: Zones РІвЂќР‚РІвЂќР‚
	private Widget m_ZonesContainer;
	private ScrollWidget m_ZoneScroll;
	private GridSpacerWidget m_ZoneList;
	private ButtonWidget m_BtnNewZone;
	private ButtonWidget m_BtnDeleteZone;
	private ButtonWidget m_BtnToggleZone;
	
	// РІвЂќР‚РІвЂќР‚ Left: Presets РІвЂќР‚РІвЂќР‚
	private Widget m_PresetsContainer;
	private ScrollWidget m_PresetScroll;
	private GridSpacerWidget m_PresetList;
	private ButtonWidget m_BtnNewPreset;
	private ButtonWidget m_BtnDeletePreset;
	private ButtonWidget m_BtnSaveAllPresets;
	
	// РІвЂќР‚РІвЂќР‚ Left: Stats & Actions РІвЂќР‚РІвЂќР‚
	private TextWidget m_StatsText1;
	private TextWidget m_StatsText2;
	private ButtonWidget m_BtnForceSpawnAll;
	private ButtonWidget m_BtnDespawnAll;
	private ButtonWidget m_BtnCloseBottom;
	private TextWidget m_StatusText;
	
	// РІвЂќР‚РІвЂќР‚ Center: Zone Editor РІвЂќР‚РІвЂќР‚
	private Widget m_ZoneEditorContainer;
	private TextWidget m_ZoneEditorTitle;
	private EditBoxWidget m_ZoneNameEdit;
	private string m_OriginalZoneName;
	private ScrollWidget m_PointScroll;
	private GridSpacerWidget m_PointList;
	private ButtonWidget m_BtnAddPoint;
	private ButtonWidget m_BtnAddAtPlayer;
	private ButtonWidget m_BtnRemovePoint;
	
	// Point Editor
	private ButtonWidget m_BtnPresetSelect;
	private EditBoxWidget m_EditPointName;
	private EditBoxWidget m_EditCountMin;
	private EditBoxWidget m_EditCountMax;
	private EditBoxWidget m_EditMaxAlive;
	private EditBoxWidget m_EditRadius;
	private EditBoxWidget m_EditPatrol;
	private EditBoxWidget m_EditTriggerRadius;
	private CheckBoxWidget m_ChkPatrolEnabled;
	private EditBoxWidget m_EditPatrolTime;
	private EditBoxWidget m_EditRespawnTime;
	private ButtonWidget m_BtnModeAlways;
	private ButtonWidget m_BtnModeDay;
	private ButtonWidget m_BtnModeNight;
	private EditBoxWidget m_EditPosX;
	private EditBoxWidget m_EditPosY;
	private EditBoxWidget m_EditPosZ;
	private ButtonWidget m_BtnMyPos;
	private ButtonWidget m_BtnApplyPoint;
	private ButtonWidget m_BtnSaveZone;
	private TextWidget m_CenterStatus;
	
	// РІвЂќР‚РІвЂќР‚ Center: Preset Editor РІвЂќР‚РІвЂќР‚
	private Widget m_PresetEditorContainer;
	private EditBoxWidget m_EditPresetName;
	private ButtonWidget m_BtnDifficulty;
	private TextWidget m_PresetClassCount;
	private ScrollWidget m_PresetClassScroll;
	private GridSpacerWidget m_PresetClassList;
	private TextWidget m_PresetEditorStatus;
	
	// РІвЂќР‚РІвЂќР‚ Right: Entity Browser РІвЂќР‚РІвЂќР‚
	private EditBoxWidget m_SearchInput;
	private XComboBoxWidget m_CategoryCombo;
	private TextWidget m_EntityCountText;
	private TextListboxWidget m_EntityListBox;
	private ButtonWidget m_BtnAddToPreset;
	private TextWidget m_RightStatus;
	
	// РІвЂќР‚РІвЂќР‚ Data РІвЂќР‚РІвЂќР‚
	private ref array<ref GPDA_SpawnZoneData> m_Zones;
	private ref array<ref GPDA_MutantPreset> m_Presets;
	private int m_SelectedZoneIndex = -1;
	private int m_SelectedPointIndex = -1;
	private int m_SelectedPresetIndex = -1;
	private int m_CurrentSpawnMode = 0;
	private int m_CurrentDifficulty = 1;
	private string m_SelectedPresetName = "";
	private int m_ExpectedPresetCount = 0;
	
	// РІвЂќР‚РІвЂќР‚ Dynamic rows РІвЂќР‚РІвЂќР‚
	private ref array<Widget> m_ZoneRows;
	private ref array<Widget> m_PointRows;
	private ref array<Widget> m_PresetRows;
	private ref array<Widget> m_ClassRows;
// Map
private Widget m_MapPanel;
private Widget m_RightPanel;
private MapWidget m_SpawnerMap;
private ButtonWidget m_BtnToggleMap;
private TextWidget m_MapStatus;
private bool m_MapVisible;
private float m_MapUpdateTimer;
	
	// РІвЂќР‚РІвЂќР‚ Search state РІвЂќР‚РІвЂќР‚
	private int m_SearchBoxCount = 0;
	private int m_LastCategoryIdx = 0;
	
	// Entity categories for filtering
	private static const ref array<string> ENTITY_CATEGORIES = {"All Creatures", "Zombies", "Animals", "All (scope 2)"};
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// STATIC CONTROL
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	static void Open()
	{
		if (!s_Instance)
			GetGame().GetUIManager().EnterScriptedMenu(MENU_GPDA_MUTANT_SPAWNER, null);
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
	
	static GPDA_MutantSpawnerUI GetInstance()
	{
		return s_Instance;
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// INIT
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	override Widget Init()
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/gpda_mutant_spawner.layout");
		
		// Header
		m_BtnCloseX = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnCloseX"));
		
		// Tab Buttons
		m_BtnTabZones   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnTabZones"));
		m_BtnTabPresets = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnTabPresets"));
		
		// Left: Zones
		m_ZonesContainer = m_Root.FindAnyWidget("ZonesContainer");
		m_ZoneScroll     = ScrollWidget.Cast(m_Root.FindAnyWidget("ZoneScroll"));
		m_ZoneList       = GridSpacerWidget.Cast(m_Root.FindAnyWidget("ZoneList"));
		m_BtnNewZone     = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnNewZone"));
		m_BtnDeleteZone  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnDeleteZone"));
		m_BtnToggleZone  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnToggleZone"));
		
		// Left: Presets
		m_PresetsContainer  = m_Root.FindAnyWidget("PresetsContainer");
		m_PresetScroll      = ScrollWidget.Cast(m_Root.FindAnyWidget("PresetScroll"));
		m_PresetList        = GridSpacerWidget.Cast(m_Root.FindAnyWidget("PresetList"));
		m_BtnNewPreset      = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnNewPreset"));
		m_BtnDeletePreset   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnDeletePreset"));
		m_BtnSaveAllPresets = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSaveAllPresets"));
		
		// Left: Stats
		m_StatsText1      = TextWidget.Cast(m_Root.FindAnyWidget("StatsText1"));
		m_StatsText2      = TextWidget.Cast(m_Root.FindAnyWidget("StatsText2"));
		m_BtnForceSpawnAll = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnForceSpawnAll"));
		m_BtnDespawnAll   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnDespawnAll"));
		m_BtnCloseBottom  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnCloseBottom"));
		m_StatusText      = TextWidget.Cast(m_Root.FindAnyWidget("StatusText"));
		
		// Center: Zone Editor
		m_ZoneEditorContainer = m_Root.FindAnyWidget("ZoneEditorContainer");
		m_ZoneNameEdit    = EditBoxWidget.Cast(m_Root.FindAnyWidget("ZoneNameEdit"));
		m_OriginalZoneName = "";
		m_PointScroll     = ScrollWidget.Cast(m_Root.FindAnyWidget("PointScroll"));
		m_PointList       = GridSpacerWidget.Cast(m_Root.FindAnyWidget("PointList"));
		m_BtnAddPoint     = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAddPoint"));
		m_BtnAddAtPlayer  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAddAtPlayer"));
		m_BtnRemovePoint  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnRemovePoint"));
		
		// Point Editor
		m_BtnPresetSelect = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnPresetSelect"));
		m_EditPointName   = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditPointName"));
		m_EditCountMin    = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditCountMin"));
		m_EditCountMax    = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditCountMax"));
		m_EditMaxAlive    = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditMaxAlive"));
		m_EditRadius      = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditRadius"));
		m_EditPatrol      = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditPatrol"));
		m_EditTriggerRadius = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditTriggerRadius"));
		m_ChkPatrolEnabled = CheckBoxWidget.Cast(m_Root.FindAnyWidget("ChkPatrolEnabled"));
		m_EditPatrolTime  = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditPatrolTime"));
		m_EditRespawnTime = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditRespawnTime"));
		m_BtnModeAlways   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnModeAlways"));
		m_BtnModeDay      = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnModeDay"));
		m_BtnModeNight    = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnModeNight"));
		m_EditPosX        = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditPosX"));
		m_EditPosY        = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditPosY"));
		m_EditPosZ        = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditPosZ"));
		m_BtnMyPos        = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnMyPos"));
		m_BtnApplyPoint   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnApplyPoint"));
		m_BtnSaveZone     = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSaveZone"));
		m_CenterStatus    = TextWidget.Cast(m_Root.FindAnyWidget("CenterStatus"));
		
		// Center: Preset Editor
		m_PresetEditorContainer = m_Root.FindAnyWidget("PresetEditorContainer");
		m_EditPresetName  = EditBoxWidget.Cast(m_Root.FindAnyWidget("EditPresetName"));
		m_BtnDifficulty   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnDifficulty"));
		m_PresetClassCount = TextWidget.Cast(m_Root.FindAnyWidget("PresetClassCount"));
		m_PresetClassScroll = ScrollWidget.Cast(m_Root.FindAnyWidget("PresetClassScroll"));
		m_PresetClassList = GridSpacerWidget.Cast(m_Root.FindAnyWidget("PresetClassList"));
		m_PresetEditorStatus = TextWidget.Cast(m_Root.FindAnyWidget("PresetEditorStatus"));
		
		// Right: Entity Browser
		m_SearchInput     = EditBoxWidget.Cast(m_Root.FindAnyWidget("SearchInput"));
		m_CategoryCombo   = XComboBoxWidget.Cast(m_Root.FindAnyWidget("CategoryCombo"));
		m_EntityCountText = TextWidget.Cast(m_Root.FindAnyWidget("EntityCountText"));
		m_EntityListBox   = TextListboxWidget.Cast(m_Root.FindAnyWidget("EntityListBox"));
		m_BtnAddToPreset  = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnAddToPreset"));
		m_RightStatus     = TextWidget.Cast(m_Root.FindAnyWidget("RightStatus"));
		
		// Map
		m_BtnToggleMap = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnToggleMap"));
		m_MapPanel = m_Root.FindAnyWidget("MapPanel");
		m_RightPanel = m_Root.FindAnyWidget("RightPanel");
		m_SpawnerMap = MapWidget.Cast(m_Root.FindAnyWidget("SpawnerMap"));
		m_MapStatus = TextWidget.Cast(m_Root.FindAnyWidget("MapStatus"));
		m_MapVisible = false;
		m_MapUpdateTimer = 0;
		
		// Init data arrays
		m_Zones = new array<ref GPDA_SpawnZoneData>;
		m_Presets = new array<ref GPDA_MutantPreset>;
		m_ZoneRows = new array<Widget>;
		m_PointRows = new array<Widget>;
		m_PresetRows = new array<Widget>;
		m_ClassRows = new array<Widget>;
		
		s_Instance = this;
		
		// Default tab
		SwitchTab(0);
		
		return m_Root;
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// SHOW / HIDE
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	override void OnShow()
	{
		super.OnShow();
		s_Instance = this;
		SetFocus(m_Root);
		m_Root.Show(true);
		
		GetGame().GetInput().ChangeGameFocus(1);
		GetGame().GetUIManager().ShowCursor(true);
		
		// Block ALL game input so items in hands don't activate
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		
		RequestZonesFromServer();
		RequestPresetsFromServer();
		
		// Initial entity browser scan
		UpdateEntityBrowser();
		
		SetStatus("Loading data from server...");
	}
	
	override void OnHide()
	{
		super.OnHide();
		s_Instance = null;
		
		// Re-enable all game input (null-safe)
		if (GetGame() && GetGame().GetMission())
			GetGame().GetMission().PlayerControlEnable(false);
		
		GetGame().GetInput().ChangeGameFocus(-1);
		GetGame().GetUIManager().ShowCursor(false);
		GetGame().GetInput().ResetGameFocus();
		
		ClearZoneRows();
		ClearPointRows();
		ClearPresetRows();
		ClearClassRows();
		
		m_Root.Show(false);
	}
	
	override bool UseKeyboard() { return true; }
	override bool UseMouse() { return true; }
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// UPDATE (real-time search like VPP)
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		
		// Detect search text changes РІвЂ вЂ™ refresh entity browser
		int newLen = m_SearchInput.GetText().Length();
		int newCat = m_CategoryCombo.GetCurrentItem();
		
		if (newLen != m_SearchBoxCount || newCat != m_LastCategoryIdx)
		{
			UpdateEntityBrowser();
			m_SearchBoxCount = newLen;
			m_LastCategoryIdx = newCat;
		}
		
		// Map auto-refresh (track player position)
		if (m_MapVisible && m_SpawnerMap)
		{
			m_MapUpdateTimer += timeslice;
			if (m_MapUpdateTimer > 3.0)
			{
				m_MapUpdateTimer = 0;
				RefreshMapMarkers();
			}
		}
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// TAB SWITCHING
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void SwitchTab(int tab)
	{
		m_CurrentTab = tab;
		
		// Left panel containers
		if (m_ZonesContainer) m_ZonesContainer.Show(tab == 0);
		if (m_PresetsContainer) m_PresetsContainer.Show(tab == 1);
		
		// Center panel containers
		if (m_ZoneEditorContainer) m_ZoneEditorContainer.Show(tab == 0);
		if (m_PresetEditorContainer) m_PresetEditorContainer.Show(tab == 1);
		
		// Tab button colors (VPP-style: active = red, inactive = dark)
		if (m_BtnTabZones)
		{
			if (tab == 0)
				m_BtnTabZones.SetColor(ARGB(255, 180, 40, 30));
			else
				m_BtnTabZones.SetColor(ARGB(255, 30, 30, 30));
		}
		if (m_BtnTabPresets)
		{
			if (tab == 1)
				m_BtnTabPresets.SetColor(ARGB(255, 180, 40, 30));
			else
				m_BtnTabPresets.SetColor(ARGB(255, 30, 30, 30));
		}
		
		// Refresh lists when switching
		if (tab == 0)
		{
			RefreshZoneList();
		}
		else
		{
			RefreshPresetList();
			if (m_SelectedPresetIndex >= 0 && m_SelectedPresetIndex < m_Presets.Count())
				RefreshPresetClassList();
		}
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// CLICKS
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!w) return false;
		
		// Header
		if (w == m_BtnCloseX)       { CloseMenu(); return true; }
		
		// Tabs
		if (w == m_BtnTabZones)     { SwitchTab(0); return true; }
		if (w == m_BtnTabPresets)   { SwitchTab(1); return true; }
		
		// Left: Zone actions
		if (w == m_BtnNewZone)      { CreateNewZone(); return true; }
		if (w == m_BtnDeleteZone)   { DeleteSelectedZone(); return true; }
		if (w == m_BtnToggleZone)   { ToggleSelectedZone(); return true; }
		
		// Left: Preset actions
		if (w == m_BtnNewPreset)    { CreateNewPreset(); return true; }
		if (w == m_BtnDeletePreset) { DeleteSelectedPreset(); return true; }
		if (w == m_BtnSaveAllPresets) { SendPresetsToServer(); SetStatus("Presets saved!"); return true; }
		
		// Left: Common actions
		if (w == m_BtnForceSpawnAll) { ForceSpawnAll(); return true; }
		if (w == m_BtnDespawnAll)    { DespawnAll(); return true; }
		if (w == m_BtnCloseBottom)   { CloseMenu(); return true; }
		
		// Center: Zone editor
		if (w == m_BtnAddPoint)     { AddSpawnPoint(false); return true; }
		if (w == m_BtnAddAtPlayer)  { AddSpawnPoint(true); return true; }
		if (w == m_BtnRemovePoint)  { RemoveSelectedPoint(); return true; }
		if (w == m_BtnApplyPoint)   { ApplyPointChanges(); return true; }
		if (w == m_BtnSaveZone)     { SaveCurrentZone(); return true; }
		if (w == m_BtnPresetSelect) { CyclePresetForPoint(); return true; }
		
		// Center: Spawn mode
		if (w == m_BtnModeAlways)   { SetSpawnMode(0); return true; }
		if (w == m_BtnModeDay)      { SetSpawnMode(1); return true; }
		if (w == m_BtnModeNight)    { SetSpawnMode(2); return true; }
		
		// Center: Preset editor
		if (w == m_BtnDifficulty)   { CycleDifficulty(); return true; }
		
		// Right: Entity Browser
		if (w == m_BtnAddToPreset)  { AddSelectedEntityToPreset(); return true; }
		if (w == m_CategoryCombo)   { UpdateEntityBrowser(); return true; }
		
		// MY POSITION button
		if (w == m_BtnMyPos)        { SetPositionFromPlayer(); return true; }
		
		// Map toggle
		if (w == m_BtnToggleMap)    { ToggleMap(); return true; }
		
		return false;
	}
	
	// Legacy handler removed вЂ” row selection now via OnMouseButtonDown
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		// VPP pattern: double-click entity to add to preset
		if (w == m_EntityListBox && button == MouseState.LEFT)
		{
			AddSelectedEntityToPreset();
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
	
	// в•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђ
	// MOUSE CLICK ON NON-BUTTON ROWS (zones, points, presets, classes)
	// OnClick only fires for ButtonWidget, so we use OnMouseButtonDown
	// в•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђ
	
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT) return false;
		if (!w) return false;
		
		// Walk up the widget tree to find a row that has a valid UserID
		Widget check = w;
		int maxDepth = 5;
		
		while (check && maxDepth > 0)
		{
			Widget parent = check.GetParent();
			
			// Check if parent is one of our list containers
			if (parent == m_ZoneList)
			{
				// check is a zone row
				SelectZone(check.GetUserID());
				return true;
			}
			
			if (parent == m_PointList)
			{
				// check is a point row
				SelectPoint(check.GetUserID());
				return true;
			}
			
			if (parent == m_PresetList)
			{
				// check is a preset row
				SelectPreset(check.GetUserID());
				return true;
			}
			
			if (parent == m_PresetClassList)
			{
				// check is a class row вЂ” check if clicking remove button
				string wName = w.GetName();
				if (wName == "ClassRowRemove" || wName == "TxtRemoveClass")
				{
					RemoveClassFromPreset(check.GetUserID());
					return true;
				}
				// Otherwise just select/highlight
				return true;
			}
			
			check = parent;
			maxDepth--;
		}
		
		return false;
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// ENTITY BROWSER (VPP-style CfgVehicles scan)
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void UpdateEntityBrowser()
	{
		if (!m_EntityListBox) return;
		
		m_EntityListBox.ClearItems();
		
		string strSearch = m_SearchInput.GetText();
		strSearch.ToLower();
		
		// Don't search the placeholder text
		if (strSearch == "search...") strSearch = "";
		
		int catIdx = m_CategoryCombo.GetCurrentItem();
		if (catIdx < 0) catIdx = 0;
		
		string cfgPath = "CfgVehicles";
		int nClasses = g_Game.ConfigGetChildrenCount(cfgPath);
		int foundCount = 0;
		
		for (int i = 0; i < nClasses; i++)
		{
			string className;
			GetGame().ConfigGetChildName(cfgPath, i, className);
			
			int scope = GetGame().ConfigGetInt(cfgPath + " " + className + " scope");
			
			// Skip non-visible classes
			if (scope < 2 && catIdx != 3)
				continue;
			if (catIdx == 3 && scope < 1)
				continue;
			
			string lowerName = className;
			lowerName.ToLower();
			
			// Category filter
			switch (catIdx)
			{
				case 0: // All Creatures (zombies + animals)
					if (!GetGame().IsKindOf(lowerName, "dz_lightai") && !GetGame().IsKindOf(lowerName, "animal_base"))
						continue;
					break;
				case 1: // Zombies only
					if (!GetGame().IsKindOf(lowerName, "zombiebase"))
						continue;
					break;
				case 2: // Animals only
					if (!GetGame().IsKindOf(lowerName, "animal_base"))
						continue;
					break;
				case 3: // All scope 2+ (everything spawnable)
					break;
			}
			
			// Search filter
			if (strSearch != "" && !lowerName.Contains(strSearch))
				continue;
			
			m_EntityListBox.AddItem(className, NULL, 0);
			foundCount++;
		}
		
		if (m_EntityCountText)
			m_EntityCountText.SetText(foundCount.ToString() + " entities found");
	}
	
	void AddSelectedEntityToPreset()
	{
		int row = m_EntityListBox.GetSelectedRow();
		if (row == -1)
		{
			SetRightStatus("Select an entity first!");
			return;
		}
		
		string entityClass;
		m_EntityListBox.GetItemText(row, 0, entityClass);
		
		if (m_SelectedPresetIndex < 0 || m_SelectedPresetIndex >= m_Presets.Count())
		{
			SetRightStatus("Select a preset first! (PRESETS tab)");
			return;
		}
		
		GPDA_MutantPreset preset = m_Presets[m_SelectedPresetIndex];
		
		// Check duplicates
		for (int i = 0; i < preset.ClassNames.Count(); i++)
		{
			if (preset.ClassNames[i] == entityClass)
			{
				SetRightStatus("'" + entityClass + "' already in preset!");
				return;
			}
		}
		
		// Add to preset
		preset.ClassNames.Insert(entityClass);
		
		// Refresh class list if preset editor visible
		if (m_CurrentTab == 1)
			RefreshPresetClassList();
		
		SetRightStatus("+ " + entityClass + " -> " + preset.Name);
	}
	
	void RemoveClassFromPreset(int classIndex)
	{
		if (m_SelectedPresetIndex < 0 || m_SelectedPresetIndex >= m_Presets.Count()) return;
		
		GPDA_MutantPreset preset = m_Presets[m_SelectedPresetIndex];
		if (classIndex < 0 || classIndex >= preset.ClassNames.Count()) return;
		
		string removed = preset.ClassNames[classIndex];
		preset.ClassNames.Remove(classIndex);
		
		RefreshPresetClassList();
		SetPresetStatus("Removed: " + removed);
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// ZONE MANAGEMENT
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void CreateNewZone()
	{
		// Read name from edit box if user typed something new
		string zoneName = "";
		if (m_ZoneNameEdit)
		{
			zoneName = m_ZoneNameEdit.GetText();
			zoneName = zoneName.Trim();
		}
		// Don't reuse the currently selected zone's name
		if (zoneName == "" || zoneName == m_OriginalZoneName)
		{
			int count = m_Zones.Count() + 1;
			zoneName = "Zone_" + count.ToString();
		}
		// Check for duplicate zone names
		for (int d = 0; d < m_Zones.Count(); d++)
		{
			if (m_Zones[d].ZoneName == zoneName)
			{
				zoneName = zoneName + "_" + (m_Zones.Count() + 1).ToString();
				break;
			}
		}
		
		ref GPDA_SpawnZoneData zone = new GPDA_SpawnZoneData();
		zone.ZoneName = zoneName;
		zone.IsEnabled = true;
		zone.CreatedBy = "admin";
		
		m_Zones.Insert(zone);
		
		RefreshZoneList();
		SelectZone(m_Zones.Count() - 1);
		SetStatus("Created zone: " + zoneName);
	}
	
	void DeleteSelectedZone()
	{
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count())
		{
			SetStatus("No zone selected!");
			return;
		}
		
		string name = m_Zones[m_SelectedZoneIndex].ZoneName;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			ref Param1<string> params = new Param1<string>(name);
			GetGame().RPCSingleParam(player, GPDA_RPC.MSP_DELETE_ZONE, params, true);
		}
		
		m_Zones.Remove(m_SelectedZoneIndex);
		m_SelectedZoneIndex = -1;
		m_SelectedPointIndex = -1;
		
		RefreshZoneList();
		ClearPointRows();
		ClearPointEditor();
		
		SetStatus("Deleted zone: " + name);
	}
	
	void ToggleSelectedZone()
	{
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count())
		{
			SetStatus("No zone selected!");
			return;
		}
		
		GPDA_SpawnZoneData zone = m_Zones[m_SelectedZoneIndex];
		zone.IsEnabled = !zone.IsEnabled;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			ref Param2<string, bool> params = new Param2<string, bool>(zone.ZoneName, zone.IsEnabled);
			GetGame().RPCSingleParam(player, GPDA_RPC.MSP_TOGGLE_ZONE, params, true);
		}
		
		RefreshZoneList();
		string state = "ENABLED";
		if (!zone.IsEnabled) state = "DISABLED";
		SetStatus(zone.ZoneName + " -> " + state);
	}
	
	void SelectZone(int index)
	{
		if (index < 0 || index >= m_Zones.Count()) return;
		
		m_SelectedZoneIndex = index;
		m_SelectedPointIndex = -1;
		
		GPDA_SpawnZoneData zone = m_Zones[index];
		if (m_ZoneNameEdit)
			m_ZoneNameEdit.SetText(zone.ZoneName);
		m_OriginalZoneName = zone.ZoneName;
		
		// Highlight selected zone
		for (int i = 0; i < m_ZoneRows.Count(); i++)
		{
			Widget row = m_ZoneRows[i];
			if (!row) continue;
			Widget bg = Widget.Cast(row.FindAnyWidget("ZoneRowBG"));
			if (bg)
			{
				if (i == index)
					bg.SetColor(ARGB(80, 200, 50, 30));
				else
					bg.SetColor(ARGB(0, 0, 0, 0));
			}
		}
		
		RefreshPointList();
		ClearPointEditor();
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// SPAWN POINT MANAGEMENT
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void AddSpawnPoint(bool atPlayerPos)
	{
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count())
		{
			SetCenterStatus("Select a zone first!");
			return;
		}
		
		GPDA_SpawnZoneData zone = m_Zones[m_SelectedZoneIndex];
		
		ref GPDA_SpawnPointData point = new GPDA_SpawnPointData();
		point.PointID = zone.GetNextPointID();
		
		// Always default to player position (even for "Add Point" button)
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			point.Position = player.GetPosition();
		
		if (m_SelectedPresetName != "")
			point.PresetName = m_SelectedPresetName;
		else if (m_Presets.Count() > 0)
			point.PresetName = m_Presets[0].Name;
		
		if (m_EditPointName) point.PointName = m_EditPointName.GetText();
		point.CountMin = m_EditCountMin.GetText().ToInt();
		point.CountMax = m_EditCountMax.GetText().ToInt();
		point.MaxAlive = m_EditMaxAlive.GetText().ToInt();
		point.SpawnRadius = m_EditRadius.GetText().ToFloat();
		point.PatrolRadius = m_EditPatrol.GetText().ToFloat();
		if (m_EditTriggerRadius) point.TriggerRadius = m_EditTriggerRadius.GetText().ToFloat();
		if (m_ChkPatrolEnabled) point.PatrolEnabled = m_ChkPatrolEnabled.IsChecked();
		if (m_EditPatrolTime) point.PatrolTimeSec = m_EditPatrolTime.GetText().ToFloat();
		point.RespawnTimeSec = m_EditRespawnTime.GetText().ToFloat();
		point.SpawnMode = m_CurrentSpawnMode;
		point.IsActive = true;
		
		if (point.CountMin < 1) point.CountMin = 1;
		if (point.CountMax < point.CountMin) point.CountMax = point.CountMin;
		if (point.MaxAlive < 1) point.MaxAlive = point.CountMax;
		if (point.RespawnTimeSec < 30) point.RespawnTimeSec = 300;
		if (point.TriggerRadius < 1) point.TriggerRadius = 200;
		if (point.PatrolTimeSec < 10) point.PatrolTimeSec = 120;
		
		zone.Points.Insert(point);
		
		RefreshPointList();
		SelectPoint(zone.Points.Count() - 1);
		
		string posStr = "";
		if (atPlayerPos) posStr = " at player pos";
		SetCenterStatus("Added point #" + point.PointID.ToString() + posStr);
	}
	
	void RemoveSelectedPoint()
	{
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count())
		{
			SetCenterStatus("No zone selected!");
			return;
		}
		if (m_SelectedPointIndex < 0)
		{
			SetCenterStatus("No point selected!");
			return;
		}
		
		GPDA_SpawnZoneData zone = m_Zones[m_SelectedZoneIndex];
		if (m_SelectedPointIndex >= zone.Points.Count()) return;
		
		int ptId = zone.Points[m_SelectedPointIndex].PointID;
		zone.Points.Remove(m_SelectedPointIndex);
		m_SelectedPointIndex = -1;
		
		RefreshPointList();
		ClearPointEditor();
		SetCenterStatus("Removed point #" + ptId.ToString());
	}
	
	void SelectPoint(int index)
	{
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count()) return;
		
		GPDA_SpawnZoneData zone = m_Zones[m_SelectedZoneIndex];
		if (index < 0 || index >= zone.Points.Count()) return;
		
		m_SelectedPointIndex = index;
		GPDA_SpawnPointData point = zone.Points[index];
		
		// Populate editor
		if (m_BtnPresetSelect) m_BtnPresetSelect.SetText(point.PresetName);
		m_SelectedPresetName = point.PresetName;
		if (m_EditPointName) m_EditPointName.SetText(point.PointName);
		if (m_EditCountMin) m_EditCountMin.SetText(point.CountMin.ToString());
		if (m_EditCountMax) m_EditCountMax.SetText(point.CountMax.ToString());
		if (m_EditMaxAlive) m_EditMaxAlive.SetText(point.MaxAlive.ToString());
		if (m_EditRadius) m_EditRadius.SetText(point.SpawnRadius.ToString());
		if (m_EditPatrol) m_EditPatrol.SetText(point.PatrolRadius.ToString());
		if (m_EditTriggerRadius) m_EditTriggerRadius.SetText(point.TriggerRadius.ToString());
		if (m_ChkPatrolEnabled) m_ChkPatrolEnabled.SetChecked(point.PatrolEnabled);
		if (m_EditPatrolTime) m_EditPatrolTime.SetText(point.PatrolTimeSec.ToString());
		if (m_EditRespawnTime) m_EditRespawnTime.SetText(point.RespawnTimeSec.ToString());
		if (m_EditPosX) m_EditPosX.SetText(point.Position[0].ToString());
		if (m_EditPosY) m_EditPosY.SetText(point.Position[1].ToString());
		if (m_EditPosZ) m_EditPosZ.SetText(point.Position[2].ToString());
		
		SetSpawnMode(point.SpawnMode);
		
		// Highlight selected point row
		for (int i = 0; i < m_PointRows.Count(); i++)
		{
			Widget row = m_PointRows[i];
			if (!row) continue;
			Widget bg = Widget.Cast(row.FindAnyWidget("PointRowBG"));
			if (bg)
			{
				if (i == index)
					bg.SetColor(ARGB(60, 200, 50, 30));
				else
					bg.SetColor(ARGB(0, 0, 0, 0));
			}
		}
	}
	
	void ApplyPointChanges()
	{
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count())
		{
			SetCenterStatus("No zone selected!"); return;
		}
		
		GPDA_SpawnZoneData zone = m_Zones[m_SelectedZoneIndex];
		if (m_SelectedPointIndex < 0 || m_SelectedPointIndex >= zone.Points.Count())
		{
			SetCenterStatus("No point selected!"); return;
		}
		
		GPDA_SpawnPointData point = zone.Points[m_SelectedPointIndex];
		
		point.PresetName = m_SelectedPresetName;
		if (m_EditPointName) point.PointName = m_EditPointName.GetText();
		point.CountMin = m_EditCountMin.GetText().ToInt();
		point.CountMax = m_EditCountMax.GetText().ToInt();
		point.MaxAlive = m_EditMaxAlive.GetText().ToInt();
		point.SpawnRadius = m_EditRadius.GetText().ToFloat();
		point.PatrolRadius = m_EditPatrol.GetText().ToFloat();
		if (m_EditTriggerRadius) point.TriggerRadius = m_EditTriggerRadius.GetText().ToFloat();
		if (m_ChkPatrolEnabled) point.PatrolEnabled = m_ChkPatrolEnabled.IsChecked();
		if (m_EditPatrolTime) point.PatrolTimeSec = m_EditPatrolTime.GetText().ToFloat();
		point.RespawnTimeSec = m_EditRespawnTime.GetText().ToFloat();
		point.SpawnMode = m_CurrentSpawnMode;
		
		float px = m_EditPosX.GetText().ToFloat();
		float py = m_EditPosY.GetText().ToFloat();
		float pz = m_EditPosZ.GetText().ToFloat();
		point.Position = Vector(px, py, pz);
		
		if (point.CountMin < 1) point.CountMin = 1;
		if (point.CountMax < point.CountMin) point.CountMax = point.CountMin;
		if (point.MaxAlive < 1) point.MaxAlive = point.CountMax;
		if (point.RespawnTimeSec < 10) point.RespawnTimeSec = 10;
		if (point.PatrolTimeSec < 10) point.PatrolTimeSec = 120;
		
		RefreshPointList();
		SetCenterStatus("Point #" + point.PointID.ToString() + " updated");
	}
	
	void SetPositionFromPlayer()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
		{
			SetCenterStatus("No player found!");
			return;
		}
		
		vector pos = player.GetPosition();
		if (m_EditPosX) m_EditPosX.SetText(pos[0].ToString());
		if (m_EditPosY) m_EditPosY.SetText(pos[1].ToString());
		if (m_EditPosZ) m_EditPosZ.SetText(pos[2].ToString());
		
		SetCenterStatus("Position set: " + pos[0].ToString() + ", " + pos[1].ToString() + ", " + pos[2].ToString());
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// SPAWN MODE
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void SetSpawnMode(int mode)
	{
		m_CurrentSpawnMode = mode;
		
		// Reset colors
		if (m_BtnModeAlways) m_BtnModeAlways.SetColor(ARGB(255, 30, 100, 50));
		if (m_BtnModeDay) m_BtnModeDay.SetColor(ARGB(255, 125, 115, 25));
		if (m_BtnModeNight) m_BtnModeNight.SetColor(ARGB(255, 30, 30, 115));
		
		// Bright highlight active
		switch (mode)
		{
			case 0: if (m_BtnModeAlways) m_BtnModeAlways.SetColor(ARGB(255, 40, 160, 70)); break;
			case 1: if (m_BtnModeDay) m_BtnModeDay.SetColor(ARGB(255, 200, 180, 40)); break;
			case 2: if (m_BtnModeNight) m_BtnModeNight.SetColor(ARGB(255, 50, 50, 180)); break;
		}
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// PRESET MANAGEMENT
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void SelectPreset(int index)
	{
		if (index < 0 || index >= m_Presets.Count()) return;
		
		m_SelectedPresetIndex = index;
		GPDA_MutantPreset preset = m_Presets[index];
		
		// Update preset editor fields
		if (m_EditPresetName) m_EditPresetName.SetText(preset.Name);
		m_CurrentDifficulty = preset.Difficulty;
		UpdateDifficultyButton();
		
		// Set as current for point editor too
		m_SelectedPresetName = preset.Name;
		if (m_BtnPresetSelect) m_BtnPresetSelect.SetText(preset.Name);
		
		// Highlight in list
		for (int j = 0; j < m_PresetRows.Count(); j++)
		{
			Widget row = m_PresetRows[j];
			if (!row) continue;
			Widget bg = Widget.Cast(row.FindAnyWidget("PresetRowBG"));
			if (bg)
			{
				if (j == index)
					bg.SetColor(ARGB(60, 200, 50, 30));
				else
					bg.SetColor(ARGB(0, 0, 0, 0));
			}
		}
		
		// Refresh class list in preset editor
		if (m_CurrentTab == 1)
			RefreshPresetClassList();
		
		SetRightStatus("Selected: " + preset.Name);
	}
	
	void CreateNewPreset()
	{
		ref GPDA_MutantPreset preset = new GPDA_MutantPreset();
		
		// Read name from edit box if user typed something
		string presetName = "";
		if (m_EditPresetName)
		{
			presetName = m_EditPresetName.GetText();
			presetName = presetName.Trim();
		}
		// Only use typed name if it's not from a selected preset
		if (presetName == "" || presetName.IndexOf("New Preset") == 0)
		{
			presetName = "New Preset " + (m_Presets.Count() + 1).ToString();
		}
		// Check for duplicate names
		for (int d = 0; d < m_Presets.Count(); d++)
		{
			if (m_Presets[d].Name == presetName)
			{
				presetName = presetName + "_" + (m_Presets.Count() + 1).ToString();
				break;
			}
		}
		preset.Name = presetName;
		preset.ClassNames = new array<string>;
		preset.Difficulty = 1;
		preset.IconColor = "#FF6644";
		
		m_Presets.Insert(preset);
		
		RefreshPresetList();
		SelectPreset(m_Presets.Count() - 1);
		
		// Auto-switch to presets tab
		SwitchTab(1);
		SetStatus("Created preset: " + presetName + " - add entities from browser");
	}
	
	void DeleteSelectedPreset()
	{
		if (m_SelectedPresetIndex < 0 || m_SelectedPresetIndex >= m_Presets.Count())
		{
			SetStatus("No preset selected!"); return;
		}
		
		string name = m_Presets[m_SelectedPresetIndex].Name;
		m_Presets.Remove(m_SelectedPresetIndex);
		m_SelectedPresetIndex = -1;
		
		SendPresetsToServer();
		RefreshPresetList();
		ClearClassRows();
		SetStatus("Deleted: " + name);
	}
	
	void CyclePresetForPoint()
	{
		if (m_Presets.Count() == 0)
		{
			SetCenterStatus("No presets available!"); return;
		}
		
		int curIdx = -1;
		for (int i = 0; i < m_Presets.Count(); i++)
		{
			if (m_Presets[i].Name == m_SelectedPresetName)
			{
				curIdx = i;
				break;
			}
		}
		
		curIdx = (curIdx + 1) % m_Presets.Count();
		m_SelectedPresetName = m_Presets[curIdx].Name;
		if (m_BtnPresetSelect) m_BtnPresetSelect.SetText(m_SelectedPresetName);
	}
	
	void CycleDifficulty()
	{
		m_CurrentDifficulty++;
		if (m_CurrentDifficulty > 4) m_CurrentDifficulty = 1;
		UpdateDifficultyButton();
		
		// Apply to selected preset immediately
		if (m_SelectedPresetIndex >= 0 && m_SelectedPresetIndex < m_Presets.Count())
		{
			m_Presets[m_SelectedPresetIndex].Difficulty = m_CurrentDifficulty;
			
			string newName = m_EditPresetName.GetText();
			if (newName.Length() > 0)
				m_Presets[m_SelectedPresetIndex].Name = newName;
			
			RefreshPresetList();
		}
	}
	
	void UpdateDifficultyButton()
	{
		if (!m_BtnDifficulty) return;
		string label = GPDA_DifficultyHelper.GetName(m_CurrentDifficulty);
		int color = GPDA_DifficultyHelper.GetColor(m_CurrentDifficulty);
		m_BtnDifficulty.SetText(label);
		m_BtnDifficulty.SetTextColor(color);
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// FORCE SPAWN / DESPAWN
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void ForceSpawnAll()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		GetGame().RPCSingleParam(player, GPDA_RPC.MSP_FORCE_SPAWN_ALL, null, true);
		SetStatus("Force spawning all zones...");
	}
	
	void DespawnAll()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		GetGame().RPCSingleParam(player, GPDA_RPC.MSP_DESPAWN_ALL, null, true);
		SetStatus("Despawning all creatures...");
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// LIST RENDERING
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void ClearZoneRows()
	{
		foreach (Widget w : m_ZoneRows)
			if (w) w.Unlink();
		m_ZoneRows.Clear();
	}
	
	void ClearPointRows()
	{
		foreach (Widget w : m_PointRows)
			if (w) w.Unlink();
		m_PointRows.Clear();
	}
	
	void ClearPresetRows()
	{
		foreach (Widget w : m_PresetRows)
			if (w) w.Unlink();
		m_PresetRows.Clear();
	}
	
	void ClearClassRows()
	{
		foreach (Widget w : m_ClassRows)
			if (w) w.Unlink();
		m_ClassRows.Clear();
	}
	
	void RefreshZoneList()
	{
		ClearZoneRows();
		
		int activeCount = 0;
		int totalPoints = 0;
		
		for (int i = 0; i < m_Zones.Count(); i++)
		{
			GPDA_SpawnZoneData zone = m_Zones[i];
			
			Widget row = GetGame().GetWorkspace().CreateWidgets(
				"Greshnik_PDALITE/GUI/Layouts/gpda_msp_zone_row.layout",
				m_ZoneList);
			if (!row) continue;
			
			row.SetUserID(i);
			
			TextWidget nameW = TextWidget.Cast(row.FindAnyWidget("ZoneRowName"));
			TextWidget infoW = TextWidget.Cast(row.FindAnyWidget("ZoneRowInfo"));
			Widget dotW = Widget.Cast(row.FindAnyWidget("StatusDot"));
			
			if (nameW) nameW.SetText(zone.ZoneName);
			if (infoW) infoW.SetText(zone.Points.Count().ToString() + " pts");
			
			if (dotW)
			{
				if (zone.IsEnabled)
					dotW.SetColor(ARGB(255, 40, 170, 65));
				else
					dotW.SetColor(ARGB(255, 170, 40, 40));
			}
			
			if (i == m_SelectedZoneIndex)
			{
				Widget bg = Widget.Cast(row.FindAnyWidget("ZoneRowBG"));
				if (bg) bg.SetColor(ARGB(80, 200, 50, 30));
			}
			
			if (zone.IsEnabled) activeCount++;
			totalPoints += zone.Points.Count();
			
			m_ZoneRows.Insert(row);
		}
		
		if (m_StatsText1)
			m_StatsText1.SetText("Zones: " + m_Zones.Count().ToString() + " | Active: " + activeCount.ToString());
		if (m_StatsText2)
			m_StatsText2.SetText("Points: " + totalPoints.ToString());
	}
	
	void RefreshPointList()
	{
		ClearPointRows();
		
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count()) return;
		
		GPDA_SpawnZoneData zone = m_Zones[m_SelectedZoneIndex];
		
		for (int i = 0; i < zone.Points.Count(); i++)
		{
			GPDA_SpawnPointData point = zone.Points[i];
			
			Widget row = GetGame().GetWorkspace().CreateWidgets(
				"Greshnik_PDALITE/GUI/Layouts/gpda_msp_point_row.layout",
				m_PointList);
			if (!row) continue;
			
			row.SetUserID(i);
			
			TextWidget idW      = TextWidget.Cast(row.FindAnyWidget("PointRowID"));
			TextWidget nameW    = TextWidget.Cast(row.FindAnyWidget("PointRowName"));
			TextWidget presetW  = TextWidget.Cast(row.FindAnyWidget("PointRowPreset"));
			TextWidget countW   = TextWidget.Cast(row.FindAnyWidget("PointRowCount"));
			TextWidget modeW    = TextWidget.Cast(row.FindAnyWidget("PointRowMode"));
			TextWidget respawnW = TextWidget.Cast(row.FindAnyWidget("PointRowRespawn"));
			TextWidget statusW  = TextWidget.Cast(row.FindAnyWidget("PointRowStatus"));
			Widget dotW    = Widget.Cast(row.FindAnyWidget("PointRowStatusDot"));
			
			if (idW) idW.SetText(point.PointID.ToString());
			if (nameW)
			{
				if (point.PointName.Length() > 0)
				{
					nameW.SetText(point.PointName);
					nameW.SetColor(ARGB(255, 220, 190, 130));
				}
				else
				{
					nameW.SetText("...");
					nameW.SetColor(ARGB(255, 100, 100, 100));
				}
			}
			if (presetW) presetW.SetText(point.PresetName);
			if (countW) countW.SetText(point.CountMin.ToString() + "-" + point.CountMax.ToString());
			if (modeW)
			{
				modeW.SetText(GPDA_SpawnModeHelper.GetModeName(point.SpawnMode));
				modeW.SetColor(GPDA_SpawnModeHelper.GetColor(point.SpawnMode));
			}
			if (respawnW) respawnW.SetText(point.RespawnTimeSec.ToString() + "s");
			
			if (point.IsActive)
			{
				if (statusW) { statusW.SetText("Active"); statusW.SetColor(ARGB(255, 140, 217, 115)); }
				if (dotW) dotW.SetColor(ARGB(255, 40, 170, 65));
			}
			else
			{
				if (statusW) { statusW.SetText("Paused"); statusW.SetColor(ARGB(255, 170, 130, 40)); }
				if (dotW) dotW.SetColor(ARGB(255, 170, 130, 40));
			}
			
			if (i == m_SelectedPointIndex)
			{
				Widget bg = Widget.Cast(row.FindAnyWidget("PointRowBG"));
				if (bg) bg.SetColor(ARGB(60, 200, 50, 30));
			}
			
			m_PointRows.Insert(row);
		}
	}
	
	void RefreshPresetList()
	{
		ClearPresetRows();
		
		for (int i = 0; i < m_Presets.Count(); i++)
		{
			GPDA_MutantPreset preset = m_Presets[i];
			
			Widget row = GetGame().GetWorkspace().CreateWidgets(
				"Greshnik_PDALITE/GUI/Layouts/gpda_msp_preset_row.layout",
				m_PresetList);
			if (!row) continue;
			
			row.SetUserID(i);
			
			TextWidget nameW  = TextWidget.Cast(row.FindAnyWidget("PresetRowName"));
			TextWidget diffW  = TextWidget.Cast(row.FindAnyWidget("PresetRowDifficulty"));
			TextWidget countW = TextWidget.Cast(row.FindAnyWidget("PresetRowCount"));
			Widget barW  = Widget.Cast(row.FindAnyWidget("PresetRowColorBar"));
			
			if (nameW) nameW.SetText(preset.Name);
			if (diffW)
			{
				diffW.SetText(GPDA_DifficultyHelper.GetName(preset.Difficulty));
				diffW.SetColor(GPDA_DifficultyHelper.GetColor(preset.Difficulty));
			}
			if (countW) countW.SetText(preset.ClassNames.Count().ToString() + " types");
			if (barW) barW.SetColor(GPDA_DifficultyHelper.GetColor(preset.Difficulty));
			
			if (i == m_SelectedPresetIndex)
			{
				Widget bg = Widget.Cast(row.FindAnyWidget("PresetRowBG"));
				if (bg) bg.SetColor(ARGB(60, 200, 50, 30));
			}
			
			m_PresetRows.Insert(row);
		}
	}
	
	void RefreshPresetClassList()
	{
		ClearClassRows();
		
		if (m_SelectedPresetIndex < 0 || m_SelectedPresetIndex >= m_Presets.Count()) return;
		
		GPDA_MutantPreset preset = m_Presets[m_SelectedPresetIndex];
		
		for (int i = 0; i < preset.ClassNames.Count(); i++)
		{
			Widget row = GetGame().GetWorkspace().CreateWidgets(
				"Greshnik_PDALITE/GUI/Layouts/gpda_msp_class_row.layout",
				m_PresetClassList);
			if (!row) continue;
			
			row.SetUserID(i);
			
			TextWidget nameW = TextWidget.Cast(row.FindAnyWidget("ClassRowName"));
			if (nameW) nameW.SetText(preset.ClassNames[i]);
			
			m_ClassRows.Insert(row);
		}
		
		if (m_PresetClassCount)
			m_PresetClassCount.SetText("CLASSES IN PRESET (" + preset.ClassNames.Count().ToString() + ")");
	}
	
	void ClearPointEditor()
	{
		if (m_BtnPresetSelect) m_BtnPresetSelect.SetText("-- select --");
		m_SelectedPresetName = "";
		if (m_EditPointName) m_EditPointName.SetText("");
		if (m_EditCountMin) m_EditCountMin.SetText("1");
		if (m_EditCountMax) m_EditCountMax.SetText("3");
		if (m_EditMaxAlive) m_EditMaxAlive.SetText("5");
		if (m_EditRadius) m_EditRadius.SetText("5");
		if (m_EditPatrol) m_EditPatrol.SetText("50");
		if (m_EditTriggerRadius) m_EditTriggerRadius.SetText("200");
		if (m_EditRespawnTime) m_EditRespawnTime.SetText("300");
		if (m_EditPosX) m_EditPosX.SetText("0");
		if (m_EditPosY) m_EditPosY.SetText("0");
		if (m_EditPosZ) m_EditPosZ.SetText("0");
		SetSpawnMode(0);
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// STATUS MESSAGES
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void SetStatus(string msg)
	{
		if (m_StatusText) m_StatusText.SetText(msg);
	}
	
	void SetCenterStatus(string msg)
	{
		if (m_CenterStatus) m_CenterStatus.SetText(msg);
	}
	
	void SetRightStatus(string msg)
	{
		if (m_RightStatus) m_RightStatus.SetText(msg);
	}
	
	void SetPresetStatus(string msg)
	{
		if (m_PresetEditorStatus) m_PresetEditorStatus.SetText(msg);
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// RPC: CLIENT -> SERVER
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void RequestZonesFromServer()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		GetGame().RPCSingleParam(player, GPDA_RPC.MSP_REQUEST_ZONES, null, true);
	}
	
	void RequestPresetsFromServer()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		GetGame().RPCSingleParam(player, GPDA_RPC.MSP_REQUEST_PRESETS, null, true);
	}
	
	void SaveCurrentZone()
	{
		if (m_SelectedZoneIndex < 0 || m_SelectedZoneIndex >= m_Zones.Count())
		{
			SetCenterStatus("No zone selected!"); return;
		}
		
		GPDA_SpawnZoneData zone = m_Zones[m_SelectedZoneIndex];
		
		// Read zone name from edit box (allows renaming)
		if (m_ZoneNameEdit)
		{
			string newName = m_ZoneNameEdit.GetText();
			newName = newName.Trim();
			if (newName.Length() > 0)
				zone.ZoneName = newName;
		}
		
		// Read point name from edit box if a point is selected
		if (m_SelectedPointIndex >= 0 && m_SelectedPointIndex < zone.Points.Count() && m_EditPointName)
		{
			zone.Points[m_SelectedPointIndex].PointName = m_EditPointName.GetText();
		}
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		
		string jsonData;
		JsonSerializer().WriteToString(zone, false, jsonData);
		
		// Send original name as param1 (server uses it to find/rename zone)
		string sendName = m_OriginalZoneName;
		if (sendName == "") sendName = zone.ZoneName;
		
		ref Param2<string, string> params = new Param2<string, string>(sendName, jsonData);
		GetGame().RPCSingleParam(player, GPDA_RPC.MSP_SAVE_ZONE, params, true);
		
		// Update original name after save
		m_OriginalZoneName = zone.ZoneName;
		
		// Also sync presets to server (ensure preset names match)
		SendPresetsToServer();
		
		SetCenterStatus("Zone saved: " + zone.ZoneName + " (" + zone.Points.Count().ToString() + " points)");
	}
	
	void SendPresetsToServer()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return;
		
		// Apply current preset name from edit box before sending
		if (m_SelectedPresetIndex >= 0 && m_SelectedPresetIndex < m_Presets.Count() && m_EditPresetName)
		{
			string editName = m_EditPresetName.GetText();
			editName = editName.Trim();
			if (editName.Length() > 0)
			{
				m_Presets[m_SelectedPresetIndex].Name = editName;
				m_SelectedPresetName = editName;
				if (m_BtnPresetSelect) m_BtnPresetSelect.SetText(editName);
			}
		}
		
		// Send each preset individually (avoids String CORRUPTED on large payloads)
		for (int i = 0; i < m_Presets.Count(); i++)
		{
			string jsonData;
			JsonSerializer().WriteToString(m_Presets[i], false, jsonData);
			ref Param1<string> params = new Param1<string>(jsonData);
			GetGame().RPCSingleParam(player, GPDA_RPC.MSP_SAVE_PRESETS, params, true);
		}
		
		// Send completion signal
		GetGame().RPCSingleParam(player, GPDA_RPC.MSP_SAVE_PRESETS_DONE, null, true);
		
		RefreshPresetList();
	}
	
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	// RPC: SERVER -> CLIENT (called from GPDA_ClientRPC)
	// РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’РІвЂўС’
	
	void HandleReceiveZones(ParamsReadContext ctx)
	{
		ref Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data)) return;
		
		string jsonStr = data.param1;
		
		m_Zones.Clear();
		
		string jsonErr;
		ref array<ref GPDA_SpawnZoneData> zones;
		if (JsonSerializer().ReadFromString(zones, jsonStr, jsonErr))
		{
			if (zones)
			{
				for (int i = 0; i < zones.Count(); i++)
					m_Zones.Insert(zones[i]);
			}
		}
		
		RefreshZoneList();
		
		if (m_Zones.Count() > 0 && m_SelectedZoneIndex == -1)
			SelectZone(0);
		
		// Refresh map if visible
		if (m_MapVisible && m_SpawnerMap) RefreshMapMarkers();
		
		SetStatus("Loaded " + m_Zones.Count().ToString() + " zones");
	}
	
	void HandleReceivePresetCount(ParamsReadContext ctx)
	{
		ref Param1<int> data = new Param1<int>(0);
		if (!ctx.Read(data)) return;
		
		m_ExpectedPresetCount = data.param1;
		m_Presets.Clear();
		
		Print("[GPDA][UI] MutantSpawner: Expecting " + m_ExpectedPresetCount.ToString() + " presets");
		
		if (m_ExpectedPresetCount == 0)
		{
			RefreshPresetList();
			SetRightStatus("No presets on server");
		}
	}
	
	void HandleReceiveOnePreset(ParamsReadContext ctx)
	{
		ref Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data)) return;
		
		string jsonStr = data.param1;
		if (jsonStr == "") return;
		
		string jsonErr;
		ref GPDA_MutantPreset preset;
		if (JsonSerializer().ReadFromString(preset, jsonStr, jsonErr))
		{
			if (preset)
				m_Presets.Insert(preset);
		}
		else
		{
			Print("[GPDA][UI] MutantSpawner: Failed to parse preset: " + jsonErr);
		}
		
		if (m_Presets.Count() >= m_ExpectedPresetCount)
		{
			RefreshPresetList();
			SetRightStatus("Loaded " + m_Presets.Count().ToString() + " presets");
		}
	}
	
	void HandleStatus(ParamsReadContext ctx)
	{
		ref Param1<string> data = new Param1<string>("");
		if (!ctx.Read(data)) return;
		
		string msg = data.param1;
		
		// Admin access denied вЂ” close menu automatically
		if (msg == "ACCESS_DENIED")
		{
			SetStatus("Access denied! Admin role on website required.");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.CloseMenuDelayed, 1500, false);
			return;
		}
		
		SetStatus(msg);
	}
	
	void CloseMenuDelayed()
	{
		CloseMenu();
	}
	
	// в•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђ
	// MAP вЂ” Spawn points + radius circles on game map
	// в•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђв•ђ
	
	void ToggleMap()
	{
		m_MapVisible = !m_MapVisible;
		if (m_MapPanel) m_MapPanel.Show(m_MapVisible);
		if (m_RightPanel) m_RightPanel.Show(!m_MapVisible);
		
		if (m_MapVisible)
		{
			m_MapUpdateTimer = 0;
			RefreshMapMarkers();
			CenterMapOnPlayer();
			if (m_BtnToggleMap) m_BtnToggleMap.SetColor(ARGB(255, 120, 50, 200));
			SetStatus("MAP ON");
		}
		else
		{
			if (m_BtnToggleMap) m_BtnToggleMap.SetColor(ARGB(255, 90, 38, 140));
			SetStatus("MAP OFF");
		}
	}
	
	void CenterMapOnPlayer()
	{
		if (!m_SpawnerMap) return;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			m_SpawnerMap.SetMapPos(player.GetPosition());
			m_SpawnerMap.SetScale(0.2);
		}
	}
	
	void RefreshMapMarkers()
	{
		if (!m_SpawnerMap) return;
		
		m_SpawnerMap.ClearUserMarks();
		
		int purple = ARGB(220, 160, 60, 220);
		int pointColor = ARGB(255, 255, 80, 80);
		int playerColor = ARGB(255, 50, 255, 50);
		int totalMarkers = 0;
		
		for (int z = 0; z < m_Zones.Count(); z++)
		{
			GPDA_SpawnZoneData zoneData = m_Zones[z];
			if (!zoneData.IsEnabled) continue;
			
			for (int p = 0; p < zoneData.Points.Count(); p++)
			{
				GPDA_SpawnPointData pt = zoneData.Points[p];
				if (!pt.IsActive) continue;
				
				string label = pt.PointName;
				if (label == "") label = pt.PresetName;
				m_SpawnerMap.AddUserMark(pt.Position, "[" + label + "]", pointColor, "");
				totalMarkers++;
				
				if (pt.SpawnRadius > 0 && totalMarkers < 5000)
				{
					DrawRadiusCircle(pt.Position, pt.SpawnRadius, purple);
					totalMarkers = totalMarkers + 200;
				}
			}
		}
		
		PlayerBase mapPlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (mapPlayer)
		{
			m_SpawnerMap.AddUserMark(mapPlayer.GetPosition(), ">> YOU <<", playerColor, "");
			m_SpawnerMap.SetMapPos(mapPlayer.GetPosition());
		}
		
		if (m_MapStatus)
			m_MapStatus.SetText(totalMarkers.ToString() + " markers | Purple = radius");
	}
	
	void DrawRadiusCircle(vector center, float radius, int color)
	{
		if (!m_SpawnerMap || radius < 1) return;
		
		int segments = 200;
		float angleStep = (Math.PI * 2.0) / segments;
		
		for (int i = 0; i < segments; i++)
		{
			float angle = i * angleStep;
			float dx = radius * Math.Cos(angle);
			float dz = radius * Math.Sin(angle);
			vector markPos = Vector(center[0] + dx, center[1], center[2] + dz);
			m_SpawnerMap.AddUserMark(markPos, ".", color, "");
		}
	}
}
