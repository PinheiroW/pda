// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Group Manager (Server-side)
// Standalone group management system
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// DATA CLASSES
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_GroupMemberData
{
	string Steam64ID;
	string Nickname;
	bool IsLeader;
	bool IsDeputy;
	
	void GPDA_GroupMemberData()
	{
		Steam64ID = "";
		Nickname = "";
		IsLeader = false;
		IsDeputy = false;
	}
}

class GPDA_ReadmeField
{
	string Key;
	string Description;

	void GPDA_ReadmeField(string key = "", string description = "")
	{
		Key = key;
		Description = description;
	}
}

class GPDA_GroupsReadmeFile
{
	int JsonVersion;
	string Title;
	string ForWho;
	string WhatIsThis;
	string HowToEdit;
	string Notes;
	ref array<ref GPDA_ReadmeField> Fields;
	ref GPDA_GroupData Example;

	void GPDA_GroupsReadmeFile()
	{
		JsonVersion = 1;
		Title = "Greshnik_PDALITE / groups";
		ForWho = "Этот файл для админов сервера. Игрокам его менять не нужно.";
		WhatIsThis = "В этой папке лежат JSON-файлы группировок (по одному файлу на группировку). Эти файлы задают: состав участников, точки спавна и список предметов (loadout), который будет выдаваться игроку после смерти при респавне на базе своей группировки.";
		HowToEdit = "Открой JSON нужной группировки и редактируй поля. После изменения можно сделать /restart сервера или Reload (если у тебя есть отдельная команда/кнопка).";
		Notes = "ВАЖНО: loadout выдаётся только после смерти (new character) и только если игрок состоит в группировке и у группировки есть SpawnPoints. Перед выдачей loadout мод чистит стандартный fresh-spawn лут DayZ, чтобы не смешивалось.";
		Fields = new array<ref GPDA_ReadmeField>;
		Example = new GPDA_GroupData();
	}
}

class GPDA_SpawnPoint
{
	string Name;           // Name of spawn point (e.g. "Base", "Outpost")
	vector Position;       // X Y Z coordinates
	
	void GPDA_SpawnPoint()
	{
		Name = "";
		Position = "0 0 0";
	}
	
	void SetData(string name, vector pos)
	{
		Name = name;
		Position = pos;
	}
}

class GPDA_GroupData
{
	string Name;
	string LeaderID;
	string DeputyID;
	int Score;
	bool DatabaseAccess;  // Can access player database (issue licenses, search players)
	string FlagClass;     // DayZ flag classname for capture (e.g. "Flag_Chernarus")
	ref array<string> Members;
	ref array<string> Items;
	bool ItemsInitialized;
	int JsonVersion;
	ref array<ref GPDA_SpawnPoint> SpawnPoints;  // Spawn coordinates for group
	
	void GPDA_GroupData()
	{
		Name = "";
		LeaderID = "";
		DeputyID = "";
		Score = 0;
		DatabaseAccess = false;
		FlagClass = "";
		Members = new array<string>;
		Items = new array<string>;
		ItemsInitialized = false;
		JsonVersion = 0;
		SpawnPoints = new array<ref GPDA_SpawnPoint>;
	}
	
	// Get random spawn point for this group
	vector GetRandomSpawnPoint()
	{
		if (!SpawnPoints || SpawnPoints.Count() == 0)
			return "0 0 0"; // No spawn point configured
		
		int idx = Math.RandomInt(0, SpawnPoints.Count());
		GPDA_SpawnPoint sp = SpawnPoints[idx];
		if (sp)
			return sp.Position;
		
		return "0 0 0";
	}
	
	bool HasSpawnPoints()
	{
		return (SpawnPoints && SpawnPoints.Count() > 0);
	}

	bool HasMember(string steam64)
	{
		if (!Members) return false;
		return (Members.Find(steam64) != -1);
	}

	bool IsLeaderOrDeputy(string steam64)
	{
		return (steam64 == LeaderID || steam64 == DeputyID);
	}

	void AddMember(string steam64)
	{
		if (!HasMember(steam64))
			Members.Insert(steam64);
	}
	
	bool RemoveMember(string steam64)
	{
		if (!Members) return false;
		
		for (int i = 0; i < Members.Count(); i++)
		{
			if (Members[i] == steam64)
			{
				Members.Remove(i);
				return true;
			}
		}
		return false;
	}
}

class GPDA_GroupsConfig
{
	ref array<ref GPDA_GroupData> Groups;
	
	void GPDA_GroupsConfig()
	{
		Groups = new array<ref GPDA_GroupData>;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// GROUP MANAGER (Singleton)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_GroupManager
{
	protected static ref GPDA_GroupManager m_Instance;
	protected ref GPDA_GroupsConfig m_Config;
	protected string m_GroupsPath;
	protected string m_GroupsFolder;
	
	static GPDA_GroupManager GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new GPDA_GroupManager();
		}
		return m_Instance;
	}
	
	void Init()
	{
        m_GroupsFolder = "$profile:Greshnik_PDALITE/groups/";
        m_GroupsPath = m_GroupsFolder + "groups.json"; // legacy not used after split
        EnsureFolders();
        Load();

        // Auto-create default example groups if they don't exist
        EnsureDefaultGroups();

        Print("[GPDA] GroupManager loaded " + m_Config.Groups.Count() + " groups (profile JSON)");
	}
	
	void EnsureFolders()
	{
		if (!FileExist("$profile:Greshnik_PDALITE"))
			MakeDirectory("$profile:Greshnik_PDALITE");
		if (!FileExist(m_GroupsFolder))
			MakeDirectory(m_GroupsFolder);
		EnsureReadme();
	}

	protected void EnsureReadme()
	{
		string readmePath = m_GroupsFolder + "README.json";
		if (FileExist(readmePath))
			return;
		
		GPDA_GroupsReadmeFile readme = new GPDA_GroupsReadmeFile();
		
		readme.Fields.Insert(new GPDA_ReadmeField("Name", "Название группировки. Используется как имя файла (санация) и как отображаемое имя в UI."));
		readme.Fields.Insert(new GPDA_ReadmeField("LeaderID", "Steam64 ID лидера (строка). Права лидера/заместителя зависят от логики мода."));
		readme.Fields.Insert(new GPDA_ReadmeField("DeputyID", "Steam64 ID заместителя (строка)."));
		readme.Fields.Insert(new GPDA_ReadmeField("Score", "Очки/рейтинг группировки (если используется в UI/логике)."));
		readme.Fields.Insert(new GPDA_ReadmeField("DatabaseAccess", "true/false. Доступ к базе игроков (если включено в твоём моде)."));
		readme.Fields.Insert(new GPDA_ReadmeField("Members", "Массив Steam64 ID участников. Если игрока нет в Members — вкладка Groups скрыта и групповая выдача не сработает."));
		readme.Fields.Insert(new GPDA_ReadmeField("SpawnPoints", "Массив точек спавна. Каждая точка: { Name: \"Base\", Position: \"X Y Z\" }. Используется для телепорта после смерти."));
		readme.Fields.Insert(new GPDA_ReadmeField("FlagClass", "Класснейм флага DayZ для захвата территории. Участники группировки могут повесить ТОЛЬКО этот флаг на флагшток через TAB. Примеры: Flag_Chernarus, Flag_Livonia, Flag_DayZ, Flag_Pirates, Flag_Bear и т.д."));
		readme.Fields.Insert(new GPDA_ReadmeField("Items", "Массив класнеймов предметов DayZ. Выдаются после смерти при групповом спавне. Одежда/броня лучше добавлять тоже сюда."));
		readme.Fields.Insert(new GPDA_ReadmeField("ItemsInitialized", "Внутренний флаг. Для дефолтных групп заполняется один раз, чтобы админские правки не перетирались при каждом запуске."));
		readme.Fields.Insert(new GPDA_ReadmeField("JsonVersion", "Версия схемы JSON. Если файл старый — мод обновит структуру и пересохранит."));
		
		readme.Example.Name = "ExampleGroup";
		readme.Example.LeaderID = "7656119XXXXXXXXXX";
		readme.Example.DeputyID = "7656119YYYYYYYYYY";
		readme.Example.Score = 0;
		readme.Example.DatabaseAccess = false;
		readme.Example.Members.Clear();
		readme.Example.Members.Insert("7656119AAAAAAAAAA");
		readme.Example.Members.Insert("7656119BBBBBBBBBB");
		readme.Example.Items.Clear();
		readme.Example.Items.Insert("BandageDressing");
		readme.Example.Items.Insert("FirstAidKit");
		readme.Example.Items.Insert("Apple");
		readme.Example.Items.Insert("SodaCan_Cola");
		readme.Example.Items.Insert("Glock19");
		readme.Example.Items.Insert("Mag_Glock_15Rnd");
		readme.Example.Items.Insert("Ammo_9x19");
		readme.Example.Items.Insert("Hoodie_Grey");
		readme.Example.Items.Insert("Jeans_Blue");
		readme.Example.Items.Insert("TaloonBag_Blue");
		readme.Example.Items.Insert("Sneakers_White");
		readme.Example.FlagClass = "Flag_DayZ";
		readme.Example.ItemsInitialized = true;
		readme.Example.JsonVersion = 2;
		
		readme.Example.SpawnPoints.Clear();
		GPDA_SpawnPoint sp = new GPDA_SpawnPoint();
		sp.SetData("Base", "5000 0 5000");
		readme.Example.SpawnPoints.Insert(sp);
		
		JsonFileLoader<GPDA_GroupsReadmeFile>.JsonSaveFile(readmePath, readme);
		Print("[GPDA] Created README.json in groups folder: " + readmePath);
	}
	
	void Load()
	{
		m_Config = new GPDA_GroupsConfig();
		bool needsResave = false;
		// load each group file
		string path;
		int fileAttr;
		FindFileHandle fh = FindFile(m_GroupsFolder + "*.json", path, fileAttr, 0);
		if (fh)
		{
			bool hasFile = true;
			string current = path;
			int loadedCount = 0;
			while (hasFile)
			{
				string full = m_GroupsFolder + current;
				string fn = current;
				fn.ToLower();
				if (fn == "readme.json" || fn == "groups.json")
				{
					hasFile = FindNextFile(fh, current, fileAttr);
					continue;
				}
				if (FileExist(full))
				{
					GPDA_GroupData gdata = new GPDA_GroupData();
					JsonFileLoader<GPDA_GroupData>.JsonLoadFile(full, gdata);
					if (gdata && gdata.Name != "")
					{
						// Migration marker (older JSON won't have JsonVersion, default 0)
						if (gdata.JsonVersion < 2)
							needsResave = true;
						gdata.JsonVersion = 2;

						// Migration / defaults for older JSON files (ensure keys exist)
						if (!gdata.Members)
						{
							gdata.Members = new array<string>;
							needsResave = true;
						}
						if (!gdata.Items)
						{
							gdata.Items = new array<string>;
							needsResave = true;
						}
						if (!gdata.SpawnPoints)
						{
							gdata.SpawnPoints = new array<ref GPDA_SpawnPoint>;
							needsResave = true;
						}
						// Migration: add FlagClass if missing (empty string = not set)
						if (!gdata.FlagClass || gdata.FlagClass == "")
						{
							gdata.FlagClass = GetDefaultFlagClass(gdata.Name);
							needsResave = true;
						}

						m_Config.Groups.Insert(gdata);
						loadedCount++;
						Print("[GPDA] Loaded group: " + gdata.Name + " from " + current);
					}
				}
				hasFile = FindNextFile(fh, current, fileAttr);
			}
			CloseFindFile(fh);
			Print("[GPDA] Loaded " + loadedCount.ToString() + " groups from files");
		}

		// Re-save groups if we migrated missing keys (so admins see full schema in JSON)
		if (needsResave)
			Save();
	}

	void EnsureDefaultGroups()
	{
		// Create 5 default example groups if they don't exist
		array<string> defaultGroups = {"Svoboda", "Monolit", "Bandits", "Military", "Killers"};
		array<int> defaultScores = {500, 1000, 300, 1500, 800};
		array<string> defaultSpawns = {
			"3701.929932 402.899994 8912.959961",  // Svoboda
			"4655.408203 57.189999 1581.759644",   // Monolit
			"11395.299805 155.800003 11368.700195", // Bandits
			"4923.450195 341.549988 10296.200195",  // Military
			"7974.680176 326.769989 5483.220215"    // Killers
		};
		string userSteamID = "76561198002174265"; // User SteamID for Monolit
		
		int createdCount = 0;
		for (int i = 0; i < defaultGroups.Count(); i++)
		{
			string groupName = defaultGroups[i];
			bool touchedExisting = false;
			
			// Check if group already exists
			GPDA_GroupData existingGroup = GetGroupByName(groupName);
			if (existingGroup)
			{
				touchedExisting = false;
				// Fill default items only once (so admins can later clear Items intentionally)
				if (!existingGroup.ItemsInitialized)
				{
					if (!existingGroup.Items)
						existingGroup.Items = new array<string>;
					FillDefaultItems(existingGroup);
					existingGroup.ItemsInitialized = true;
					touchedExisting = true;
				}
				
				// If Monolit exists, ensure spawn point exists (do NOT overwrite LeaderID)
				if (groupName == "Monolit")
				{
					bool needsUpdate = false;
					
					// Ensure schema keys exist
					if (!existingGroup.Items)
					{
						existingGroup.Items = new array<string>;
						needsUpdate = true;
					}
					
					// NOTE: LeaderID is NOT overwritten here anymore.
					// Admin can set any Steam64 as leader via JSON.
					
					// Add spawn point if not exists
					bool hasSpawnPoint = false;
					if (existingGroup.SpawnPoints)
					{
						foreach (GPDA_SpawnPoint sp : existingGroup.SpawnPoints)
						{
							if (sp && sp.Name == "Base")
							{
								hasSpawnPoint = true;
								break;
							}
						}
					}
					
					if (!hasSpawnPoint)
					{
						if (!existingGroup.SpawnPoints)
							existingGroup.SpawnPoints = new array<ref GPDA_SpawnPoint>;
						
						GPDA_SpawnPoint newSpawnPoint = new GPDA_SpawnPoint();
						newSpawnPoint.SetData("Base", "4655.408203 57.189999 1581.759644");
						existingGroup.SpawnPoints.Insert(newSpawnPoint);
						needsUpdate = true;
						Print("[GPDA] Added spawn point to Monolit: 4655.408203 57.189999 1581.759644");
					}
					
					if (needsUpdate)
					{
						Save();
						Print("[GPDA] Updated Monolit group structure");
					}
				}
				
				if (touchedExisting)
					Save();
				continue; // Already exists
			}

			// Create new group
			GPDA_GroupData newGroup = new GPDA_GroupData();
			newGroup.Name = groupName;
			newGroup.LeaderID = ""; // No leader initially
			newGroup.Score = defaultScores[i];
			newGroup.DatabaseAccess = (groupName == "Military"); // Only Military has database access by default
			newGroup.FlagClass = GetDefaultFlagClass(groupName);
			FillDefaultItems(newGroup);
			newGroup.ItemsInitialized = true;
			newGroup.JsonVersion = 2;
			
			// Add spawn point for all groups
			GPDA_SpawnPoint spawnPoint = new GPDA_SpawnPoint();
			string spawnStr = defaultSpawns[i];
			// Use switch to convert string to vector (DayZ limitation)
			vector spawnPos;
			switch (i)
			{
				case 0: spawnPos = "3701.929932 402.899994 8912.959961"; break;  // Svoboda
				case 1: spawnPos = "4655.408203 57.189999 1581.759644"; break;   // Monolit
				case 2: spawnPos = "11395.299805 155.800003 11368.700195"; break; // Bandits
				case 3: spawnPos = "4923.450195 341.549988 10296.200195"; break;  // Military
				case 4: spawnPos = "7974.680176 326.769989 5483.220215"; break;    // Killers
			}
			spawnPoint.SetData("Base", spawnPos);
			newGroup.SpawnPoints.Insert(spawnPoint);
			
			// For Monolit, set user as leader
			if (groupName == "Monolit")
			{
				newGroup.LeaderID = userSteamID;
				newGroup.AddMember(userSteamID);
				Print("[GPDA] Created Monolit with user as leader and spawn point");
			}
			else
			{
				Print("[GPDA] Created " + groupName + " with spawn point: " + defaultSpawns[i]);
			}

			// Add to groups
			m_Config.Groups.Insert(newGroup);
			createdCount++;
			Print("[GPDA] Created default group: " + groupName);
			Save(); // Save new group
		}
		
		if (createdCount > 0)
		{
			// Save all new groups
			Save();
			Print("[GPDA] Created " + createdCount.ToString() + " default groups");
		}
	}

	// Default flag classname for each known group
	protected string GetDefaultFlagClass(string groupName)
	{
		if (groupName == "Svoboda")  return "Flag_Livonia";
		if (groupName == "Monolit")  return "Flag_NSahrani";
		if (groupName == "Bandits")  return "Flag_Pirates";
		if (groupName == "Military") return "Flag_CDF";
		if (groupName == "Killers")  return "Flag_Bear";
		return "Flag_DayZ"; // default
	}

	protected void FillDefaultItems(GPDA_GroupData group)
	{
		if (!group) return;
		if (!group.Items)
			group.Items = new array<string>;
		
		group.Items.Clear();
		
		group.Items.Insert("BandageDressing");
		group.Items.Insert("FirstAidKit");
		group.Items.Insert("Apple");
		group.Items.Insert("SodaCan_Cola");
		group.Items.Insert("Glock19");
		group.Items.Insert("Mag_Glock_15Rnd");
		group.Items.Insert("Ammo_9x19");
		
		string g = group.Name;
		if (g == "Svoboda")
		{
			group.Items.Insert("M65Jacket_Olive");
			group.Items.Insert("TacticalPants_Olive");
			group.Items.Insert("AssaultBag_Green");
			group.Items.Insert("CombatBoots_Brown");
		}
		else if (g == "Monolit")
		{
			group.Items.Insert("Hoodie_Grey");
			group.Items.Insert("CargoPants_Grey");
			group.Items.Insert("TaloonBag_Grey");
			group.Items.Insert("CombatBoots_Black");
		}
		else if (g == "Bandits")
		{
			group.Items.Insert("Hoodie_Black");
			group.Items.Insert("Jeans_Black");
			group.Items.Insert("TaloonBag_Black");
			group.Items.Insert("Sneakers_Black");
		}
		else if (g == "Military")
		{
			group.Items.Insert("GorkaEJacket_Summer");
			group.Items.Insert("GorkaPants_Summer");
			group.Items.Insert("AssaultBag_Green");
			group.Items.Insert("CombatBoots_Beige");
		}
		else if (g == "Killers")
		{
			group.Items.Insert("HunterJacket_Brown");
			group.Items.Insert("HunterPants_Brown");
			group.Items.Insert("MountainBag_Green");
			group.Items.Insert("CombatBoots_Black");
		}
		else
		{
			group.Items.Insert("Hoodie_Grey");
			group.Items.Insert("Jeans_Blue");
			group.Items.Insert("TaloonBag_Blue");
			group.Items.Insert("Sneakers_White");
		}
	}

	// Sanitize filename - replace invalid characters with underscore
	protected string SanitizeFileName(string name)
	{
		if (!name) return "Unknown";
		
		string result = name;
		// Replace invalid Windows file system characters: < > : " / \ | ? *
		result.Replace("<", "_");
		result.Replace(">", "_");
		result.Replace(":", "_");
		result.Replace("\"", "_");
		result.Replace("/", "_");
		result.Replace("\\", "_");
		result.Replace("|", "_");
		result.Replace("?", "_");
		result.Replace("*", "_");
		// Replace spaces with underscore for cleaner filenames
		result.Replace(" ", "_");
		
		// Remove leading/trailing dots and spaces
		result.Trim();
		
		if (result == "") result = "Unknown";
		
		return result;
	}
	
	void Save()
	{
        EnsureFolders();
		// save each group separately with group name as filename
		foreach (GPDA_GroupData group : m_Config.Groups)
		{
			if (!group || group.Name == "") continue;
			if (group.JsonVersion < 2)
				group.JsonVersion = 2;
			
			// Use sanitized group name as filename
			string safeName = SanitizeFileName(group.Name);
			string fname = m_GroupsFolder + safeName + ".json";
			JsonFileLoader<GPDA_GroupData>.JsonSaveFile(fname, group);
			Print("[GPDA] Saved group: " + group.Name + " to " + safeName + ".json");
		}
	}

	void Reload()
	{
		Load();
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// GROUP OPERATIONS
	// ═══════════════════════════════════════════════════════════════════════════
	
	GPDA_GroupData GetPlayerGroup(string steam64)
	{
		if (!m_Config || !m_Config.Groups) return null;
		
		foreach (GPDA_GroupData group : m_Config.Groups)
		{
			if (!group) continue;
			
			if (group.HasMember(steam64))
				return group;
		}
		return null;
	}
	
	GPDA_GroupData GetGroupByName(string name)
	{
		if (!m_Config || !m_Config.Groups) return null;
		
		foreach (GPDA_GroupData group : m_Config.Groups)
		{
			if (group && group.Name == name)
				return group;
		}
		return null;
	}
	
	array<ref GPDA_GroupData> GetAllGroups()
	{
		if (m_Config)
			return m_Config.Groups;
		return null;
	}
	
	array<string> GetAllGroupNames()
	{
		array<string> names = new array<string>;
		if (m_Config && m_Config.Groups)
		{
			foreach (GPDA_GroupData group : m_Config.Groups)
			{
				if (group && group.Name != "")
					names.Insert(group.Name);
			}
		}
		return names;
	}
	
	// Create a new group with leader
	GPDA_GroupData CreateGroup(string name, string leaderSteam64)
	{
		if (!m_Config) return null;
		
		// Check if group already exists
		if (GetGroupByName(name))
			return null;
		
		// Check if player is already in a group
		if (GetPlayerGroup(leaderSteam64))
			return null;
		
		GPDA_GroupData newGroup = new GPDA_GroupData();
		newGroup.Name = name;
		newGroup.LeaderID = leaderSteam64;
		newGroup.DeputyID = "";
		newGroup.Score = 0;
		newGroup.DatabaseAccess = false;
		newGroup.Members = new array<string>;
		newGroup.Members.Insert(leaderSteam64);
		newGroup.SpawnPoints = new array<ref GPDA_SpawnPoint>;
		
		m_Config.Groups.Insert(newGroup);
		Save();
		
		Print("[GPDA] Created new group: " + name + " with leader " + leaderSteam64);
		return newGroup;
	}
	
	bool AddMemberToGroup(string groupName, string steam64)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group) return false;
		
		// Check if already in another group
		GPDA_GroupData existingGroup = GetPlayerGroup(steam64);
		if (existingGroup) return false;
		
		group.AddMember(steam64);
		Save();
		
		Print("[GPDA Groups] Added member " + steam64 + " to " + groupName);
		return true;
	}
	
	bool RemoveMemberFromGroup(string groupName, string steam64)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group) return false;
		
		// Cannot remove leader
		if (steam64 == group.LeaderID) return false;
		
		bool result = group.RemoveMember(steam64);
		if (result)
		{
			Save();
			Print("[GPDA Groups] Removed member " + steam64 + " from " + groupName);
		}
		return result;
	}
	
	void AddScore(string groupName, int amount)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (group)
		{
			group.Score = group.Score + amount;
			Save();
			
			Print("[GPDA Groups] Added " + amount.ToString() + " points to " + groupName + " (Total: " + group.Score.ToString() + ")");
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// SPAWN POINT SYSTEM
	// ═══════════════════════════════════════════════════════════════════════════
	
	// Get spawn point for player (returns "0 0 0" if no group or no spawn points)
	vector GetPlayerSpawnPoint(string steam64)
	{
		GPDA_GroupData group = GetPlayerGroup(steam64);
		if (!group) return "0 0 0";
		
		return group.GetRandomSpawnPoint();
	}
	
	// Check if player has group spawn
	bool HasGroupSpawn(string steam64)
	{
		GPDA_GroupData group = GetPlayerGroup(steam64);
		if (!group) return false;
		
		return group.HasSpawnPoints();
	}
	
	// Add spawn point to group
	bool AddGroupSpawnPoint(string groupName, string spawnName, vector pos)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group) return false;
		
		GPDA_SpawnPoint sp = new GPDA_SpawnPoint();
		sp.Name = spawnName;
		sp.Position = pos;
		group.SpawnPoints.Insert(sp);
		Save();
		
		Print("[GPDA Groups] Added spawn '" + spawnName + "' to " + groupName + " at " + pos.ToString());
		return true;
	}
	
	// Remove spawn point from group
	bool RemoveGroupSpawnPoint(string groupName, int spawnIndex)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group || !group.SpawnPoints) return false;
		
		if (spawnIndex < 0 || spawnIndex >= group.SpawnPoints.Count()) return false;
		
		GPDA_SpawnPoint sp = group.SpawnPoints[spawnIndex];
		if (!sp) return false;
		
		string spawnName = sp.Name;
		group.SpawnPoints.Remove(spawnIndex);
		Save();
		
		Print("[GPDA Groups] Removed spawn '" + spawnName + "' from " + groupName);
		return true;
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// PLAYER NAME LOOKUP
	// ═══════════════════════════════════════════════════════════════════════════
	
	string GetPlayerName(string steam64)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity())
			{
				if (player.GetIdentity().GetPlainId() == steam64)
					return player.GetIdentity().GetName();
			}
		}
		
		return steam64; // Return Steam64 if player not found
	}
	
	bool IsPlayerOnline(string steam64)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity())
			{
				if (player.GetIdentity().GetPlainId() == steam64)
					return true;
			}
		}
		return false;
	}

	// Import groups from REST API
	void ImportFromRest(array<ref GPDA_GroupData> restGroups)
	{
		if (!restGroups) return;

		foreach (GPDA_GroupData restGroup : restGroups)
		{
			if (!restGroup || restGroup.Name == "") continue;

			// Check if group already exists
			GPDA_GroupData existingGroup = GetGroupByName(restGroup.Name);
			if (existingGroup)
			{
				// Update existing group data
				existingGroup.LeaderID = restGroup.LeaderID;
				existingGroup.DeputyID = restGroup.DeputyID;
				existingGroup.Score = restGroup.Score;
				existingGroup.DatabaseAccess = restGroup.DatabaseAccess;

				// Update members
				existingGroup.Members.Clear();
				foreach (string member : restGroup.Members)
				{
					existingGroup.Members.Insert(member);
				}

				// Update spawn points
				existingGroup.SpawnPoints.Clear();
				foreach (GPDA_SpawnPoint sp : restGroup.SpawnPoints)
				{
					existingGroup.SpawnPoints.Insert(sp);
				}

				Print("[GPDA] Updated existing group from REST: " + restGroup.Name);
			}
			else
			{
				// Create new group
				GPDA_GroupData newGroup = new GPDA_GroupData();
				newGroup.Name = restGroup.Name;
				newGroup.LeaderID = restGroup.LeaderID;
				newGroup.DeputyID = restGroup.DeputyID;
				newGroup.Score = restGroup.Score;
				newGroup.DatabaseAccess = restGroup.DatabaseAccess;

				// Copy members
				foreach (string newMember : restGroup.Members)
				{
					newGroup.Members.Insert(newMember);
				}

				// Copy spawn points
				foreach (GPDA_SpawnPoint spawnPoint : restGroup.SpawnPoints)
				{
					newGroup.SpawnPoints.Insert(spawnPoint);
				}

				// Add to groups array
				m_Config.Groups.Insert(newGroup);
				Print("[GPDA] Imported new group from REST: " + restGroup.Name);
			}
		}

		// Save changes
		Save();
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// DEPUTY MANAGEMENT
	// ═══════════════════════════════════════════════════════════════════════════

	bool SetDeputy(string groupName, string deputySteam64)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group) return false;

		// Check if deputy is a member
		if (!group.HasMember(deputySteam64))
		{
			return false; // Deputy must be a member first
		}

		group.DeputyID = deputySteam64;
		Save();
		Print("[GPDA] Set deputy for group " + groupName + ": " + deputySteam64);
		return true;
	}

	bool RemoveDeputy(string groupName)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group) return false;

		group.DeputyID = "";
		Save();
		Print("[GPDA] Removed deputy from group " + groupName);
		return true;
	}

	string GetDeputy(string groupName)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group) return "";

		return group.DeputyID;
	}

	bool CanManageMembers(string groupName, string steam64)
	{
		GPDA_GroupData group = GetGroupByName(groupName);
		if (!group) return false;

		// Leader or deputy can manage members
		return (steam64 == group.LeaderID || steam64 == group.DeputyID);
	}
}

// Global instance
static ref GPDA_GroupManager g_GPDA_GroupManager;

