// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Settings Manager
// Manages mod settings from JSON file
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_Settings
{
	string RequiredItemClass;  // Item class required to open PDA menu (e.g. "Radio")
	bool RequireItem;          // Whether item is required (if false, menu always opens)
	
	void GPDA_Settings()
	{
		RequiredItemClass = "Radio";
		RequireItem = true;
	}
}

class GPDA_SettingsManager
{
	protected static ref GPDA_SettingsManager m_Instance;
	protected ref GPDA_Settings m_Settings;
	protected string m_SettingsPath;
	
	static GPDA_SettingsManager GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new GPDA_SettingsManager();
		}
		return m_Instance;
	}
	
	void GPDA_SettingsManager()
	{
		m_SettingsPath = "$profile:Greshnik_PDALITE/settings.json";
		m_Settings = new GPDA_Settings();
	}
	
	void Init()
	{
		EnsureFolders();
		Load();
		Print("[GPDA] Settings loaded - RequiredItem: " + m_Settings.RequiredItemClass + " (Required: " + m_Settings.RequireItem.ToString() + ")");
	}
	
	void EnsureFolders()
	{
		if (!FileExist("$profile:Greshnik_PDALITE"))
			MakeDirectory("$profile:Greshnik_PDALITE");
	}
	
	void Load()
	{
		if (FileExist(m_SettingsPath))
		{
			JsonFileLoader<GPDA_Settings>.JsonLoadFile(m_SettingsPath, m_Settings);
			if (!m_Settings)
				m_Settings = new GPDA_Settings();
		}
		else
		{
			// Create default settings
			m_Settings = new GPDA_Settings();
			Save();
		}
	}
	
	void Save()
	{
		if (m_Settings)
			JsonFileLoader<GPDA_Settings>.JsonSaveFile(m_SettingsPath, m_Settings);
	}
	
	GPDA_Settings GetSettings()
	{
		return m_Settings;
	}
}

// Global instance
static ref GPDA_SettingsManager g_GPDA_SettingsManager;

