// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Player Database
// Stores player profiles with callsign, bio, age, licenses
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// LICENSE DATA
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_PlayerLicenses
{
	bool Hunting;      // Лицензия на охоту
	bool Fishing;      // Лицензия на рыбалку
	bool ZoneAccess;   // Доступ в закрытую зону
	bool Weapons;      // Лицензия на оружие
	bool Trading;      // Лицензия на торговлю
	
	void GPDA_PlayerLicenses()
	{
		Hunting = false;
		Fishing = false;
		ZoneAccess = false;
		Weapons = false;
		Trading = false;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// PLAYER PROFILE DATA
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_PlayerProfile
{
	string Steam64ID;
	string Callsign;          // Позывной
	string Biography;         // Биография
	int Age;                  // Возраст персонажа
	bool IsRegistered;        // Прошёл регистрацию
	string GroupName;         // Название группировки
	ref GPDA_PlayerLicenses Licenses;
	
	// Stats
	int KillsPlayers;
	int KillsZombies;
	int KillsAnimals;
	int Deaths;
	int PlaytimeMinutes;
	
	void GPDA_PlayerProfile()
	{
		Steam64ID = "";
		Callsign = "";
		Biography = "";
		Age = 25;
		IsRegistered = false;
		GroupName = "";
		Licenses = new GPDA_PlayerLicenses();
		
		KillsPlayers = 0;
		KillsZombies = 0;
		KillsAnimals = 0;
		Deaths = 0;
		PlaytimeMinutes = 0;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// DATABASE CONFIG
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_DatabaseConfig
{
	ref array<ref GPDA_PlayerProfile> Players;
	
	void GPDA_DatabaseConfig()
	{
		Players = new array<ref GPDA_PlayerProfile>;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// PLAYER DATABASE MANAGER (Singleton)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_PlayerDatabase
{
	protected static ref GPDA_PlayerDatabase m_Instance;
	protected ref GPDA_DatabaseConfig m_Config;
	protected const string m_ProfileDir = "$profile:Greshnik_PDALITE/Database";
	protected const string m_ConfigPath = "$profile:Greshnik_PDALITE/Database/players.json";
	
	// Database access is controlled by group's DatabaseAccess flag in groups.json
	
	static GPDA_PlayerDatabase GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new GPDA_PlayerDatabase();
		}
		return m_Instance;
	}
	
	void Init()
	{
		EnsureFolders();
		Load();
		Print("[GPDA] PlayerDatabase initialized with " + m_Config.Players.Count() + " profiles");
	}
	
	protected void EnsureFolders()
	{
		if (!FileExist("$profile:Greshnik_PDALITE"))
			MakeDirectory("$profile:Greshnik_PDALITE");
		if (!FileExist(m_ProfileDir))
			MakeDirectory(m_ProfileDir);
	}
	
	void Load()
	{
		m_Config = new GPDA_DatabaseConfig();
		
		if (FileExist(m_ConfigPath))
		{
			JsonFileLoader<GPDA_DatabaseConfig>.JsonLoadFile(m_ConfigPath, m_Config);
			
			if (!m_Config || !m_Config.Players)
			{
				m_Config = new GPDA_DatabaseConfig();
			}
			
			Print("[GPDA] Loaded " + m_Config.Players.Count() + " player profiles");
		}
		else
		{
			Save();
			Print("[GPDA] Created empty player database");
		}
	}
	
	void Save()
	{
		EnsureFolders();
		JsonFileLoader<GPDA_DatabaseConfig>.JsonSaveFile(m_ConfigPath, m_Config);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// PROFILE OPERATIONS
	// ═══════════════════════════════════════════════════════════════════════════
	
	GPDA_PlayerProfile GetProfile(string steam64)
	{
		if (!m_Config || !m_Config.Players) return null;
		
		foreach (GPDA_PlayerProfile profile : m_Config.Players)
		{
			if (profile && profile.Steam64ID == steam64)
				return profile;
		}
		return null;
	}
	
	GPDA_PlayerProfile GetProfileByCallsign(string callsign)
	{
		if (!m_Config || !m_Config.Players) return null;
		
		string searchLower = callsign;
		searchLower.ToLower();
		
		foreach (GPDA_PlayerProfile profile : m_Config.Players)
		{
			if (profile)
			{
				string profileCallsign = profile.Callsign;
				profileCallsign.ToLower();
				
				if (profileCallsign == searchLower || profileCallsign.Contains(searchLower))
					return profile;
			}
		}
		return null;
	}
	
	bool IsRegistered(string steam64)
	{
		GPDA_PlayerProfile profile = GetProfile(steam64);
		if (profile)
			return profile.IsRegistered;
		return false;
	}
	
	GPDA_PlayerProfile CreateProfile(string steam64)
	{
		GPDA_PlayerProfile existing = GetProfile(steam64);
		if (existing) return existing;
		
		GPDA_PlayerProfile newProfile = new GPDA_PlayerProfile();
		newProfile.Steam64ID = steam64;
		newProfile.IsRegistered = false;
		
		m_Config.Players.Insert(newProfile);
		Save();
		
		return newProfile;
	}
	
	void RegisterPlayer(string steam64, string callsign, string bio, int age)
	{
		GPDA_PlayerProfile profile = GetProfile(steam64);
		if (!profile)
		{
			profile = CreateProfile(steam64);
		}
		
		profile.Callsign = callsign;
		profile.Biography = bio;
		profile.Age = age;
		profile.IsRegistered = true;
		
		// Check if player is in a group
		if (g_GPDA_GroupManager)
		{
			GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(steam64);
			if (group)
				profile.GroupName = group.Name;
		}
		
		Save();
		Print("[GPDA] Player registered: " + callsign + " (Steam64: " + steam64 + ")");
	}
	
	void UpdateGroupName(string steam64, string groupName)
	{
		GPDA_PlayerProfile profile = GetProfile(steam64);
		if (profile)
		{
			profile.GroupName = groupName;
			Save();
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// LICENSE OPERATIONS (Military only)
	// ═══════════════════════════════════════════════════════════════════════════
	
	bool CanAccessDatabase(string steam64)
	{
		// Check if player's group has DatabaseAccess enabled
		if (!g_GPDA_GroupManager) return false;
		
		GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(steam64);
		if (!group) return false;
		
		return group.DatabaseAccess;
	}
	
	bool IssueLicense(string targetSteam64, string licenseType, bool value)
	{
		GPDA_PlayerProfile profile = GetProfile(targetSteam64);
		if (!profile) return false;
		
		if (licenseType == "Hunting")
			profile.Licenses.Hunting = value;
		else if (licenseType == "Fishing")
			profile.Licenses.Fishing = value;
		else if (licenseType == "ZoneAccess")
			profile.Licenses.ZoneAccess = value;
		else if (licenseType == "Weapons")
			profile.Licenses.Weapons = value;
		else if (licenseType == "Trading")
			profile.Licenses.Trading = value;
		else
			return false;
		
		Save();
		return true;
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// STATS OPERATIONS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void AddKill(string steam64, int killType)
	{
		GPDA_PlayerProfile profile = GetProfile(steam64);
		if (!profile)
		{
			profile = CreateProfile(steam64);
		}
		
		if (killType == 1)
			profile.KillsPlayers++;
		else if (killType == 2)
			profile.KillsZombies++;
		else if (killType == 3)
			profile.KillsAnimals++;
		
		Save();
	}
	
	void AddDeath(string steam64)
	{
		GPDA_PlayerProfile profile = GetProfile(steam64);
		if (!profile)
		{
			profile = CreateProfile(steam64);
		}
		
		profile.Deaths++;
		Save();
	}
	
	void AddPlaytime(string steam64, int minutes)
	{
		GPDA_PlayerProfile profile = GetProfile(steam64);
		if (!profile)
		{
			profile = CreateProfile(steam64);
		}
		
		profile.PlaytimeMinutes = profile.PlaytimeMinutes + minutes;
		Save();
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// SEARCH (for Database tab)
	// ═══════════════════════════════════════════════════════════════════════════
	
	array<ref GPDA_PlayerProfile> SearchPlayers(string query)
	{
		array<ref GPDA_PlayerProfile> results = new array<ref GPDA_PlayerProfile>;
		
		if (!m_Config || !m_Config.Players) return results;
		
		string queryLower = query;
		queryLower.ToLower();
		
		foreach (GPDA_PlayerProfile profile : m_Config.Players)
		{
			if (profile)
			{
				string callsignLower = profile.Callsign;
				callsignLower.ToLower();
				
				if (callsignLower.Contains(queryLower) || profile.Steam64ID.Contains(query))
				{
					results.Insert(profile);
				}
			}
		}
		
		return results;
	}
	
	// Auto-register player when they connect
	void AutoRegisterPlayer(string steam64, string nickname)
	{
		GPDA_PlayerProfile profile = GetProfile(steam64);
		if (!profile)
		{
			profile = CreateProfile(steam64);
		}
		
		// Update callsign if not set
		if (profile.Callsign == "")
		{
			profile.Callsign = nickname;
			Save();
		}
	}
	
	// Search online players by name
	array<ref GPDA_PlayerProfile> SearchOnlinePlayers(string query)
	{
		array<ref GPDA_PlayerProfile> results = new array<ref GPDA_PlayerProfile>;
		
		string queryLower = query;
		queryLower.ToLower();
		
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity())
			{
				string playerName = player.GetIdentity().GetName();
				string steam64 = player.GetIdentity().GetPlainId();
				
				string nameLower = playerName;
				nameLower.ToLower();
				
				if (nameLower.Contains(queryLower) || steam64.Contains(query))
				{
					// Auto-register if not exists
					AutoRegisterPlayer(steam64, playerName);
					
					GPDA_PlayerProfile profile = GetProfile(steam64);
					if (profile)
						results.Insert(profile);
				}
			}
		}
		
		return results;
	}
	
	array<ref GPDA_PlayerProfile> GetAllPlayers()
	{
		if (m_Config && m_Config.Players)
			return m_Config.Players;
		return new array<ref GPDA_PlayerProfile>;
	}
}

// Global instance
static ref GPDA_PlayerDatabase g_GPDA_PlayerDB;


