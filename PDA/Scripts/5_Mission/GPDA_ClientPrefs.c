class GPDA_ClientPrefs
{
	bool ChatNotifyEnabled;
	
	void GPDA_ClientPrefs()
	{
		ChatNotifyEnabled = true;
	}
}

class GPDA_ClientPrefsManager
{
	protected static ref GPDA_ClientPrefsManager s_Instance;
	protected ref GPDA_ClientPrefs m_Prefs;
	protected string m_Path;
	
	static GPDA_ClientPrefsManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new GPDA_ClientPrefsManager();
		return s_Instance;
	}
	
	void GPDA_ClientPrefsManager()
	{
		m_Path = "$profile:Greshnik_PDALITE/client_prefs.json";
		m_Prefs = new GPDA_ClientPrefs();
	}
	
	void Init()
	{
		EnsureFolders();
		Load();
	}
	
	protected void EnsureFolders()
	{
		if (!FileExist("$profile:Greshnik_PDALITE"))
			MakeDirectory("$profile:Greshnik_PDALITE");
	}
	
	protected void Load()
	{
		if (FileExist(m_Path))
		{
			JsonFileLoader<GPDA_ClientPrefs>.JsonLoadFile(m_Path, m_Prefs);
			if (!m_Prefs)
				m_Prefs = new GPDA_ClientPrefs();
		}
		else
		{
			m_Prefs = new GPDA_ClientPrefs();
			Save();
		}
	}
	
	protected void Save()
	{
		if (m_Prefs)
			JsonFileLoader<GPDA_ClientPrefs>.JsonSaveFile(m_Path, m_Prefs);
	}
	
	GPDA_ClientPrefs GetPrefs()
	{
		return m_Prefs;
	}
	
	void SetChatNotifyEnabled(bool enabled)
	{
		if (!m_Prefs)
			m_Prefs = new GPDA_ClientPrefs();
		m_Prefs.ChatNotifyEnabled = enabled;
		Save();
	}
}

static ref GPDA_ClientPrefsManager g_GPDA_ClientPrefsManager;
