// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Mutant Spawner Handler (Server)
// Handles RPC from admin, JSON persistence, and spawner configuration
// Storage: $profile:Greshnik_PDALITE/MutantSpawner/
//   - presets.json      (all presets)
//   - zones/*.json      (one file per zone)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_MutantSpawnerHandler
{
	private static ref GPDA_MutantSpawnerHandler s_Instance;
	
	// Data
	private ref array<ref GPDA_MutantPreset> m_Presets;
	private ref array<ref GPDA_SpawnZoneData> m_Zones;
	
	// Paths
	private string m_BasePath;
	private string m_PresetsPath;
	private string m_ZonesPath;
	
	// ══════════════════════════════════════════════════════════════════════
	// SINGLETON
	// ══════════════════════════════════════════════════════════════════════
	
	static GPDA_MutantSpawnerHandler GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new GPDA_MutantSpawnerHandler();
		}
		return s_Instance;
	}
	
	void GPDA_MutantSpawnerHandler()
	{
		m_Presets = new array<ref GPDA_MutantPreset>;
		m_Zones = new array<ref GPDA_SpawnZoneData>;
		
		m_BasePath    = "$profile:Greshnik_PDALITE/MutantSpawner/";
		m_PresetsPath = m_BasePath + "presets.json";
		m_ZonesPath   = m_BasePath + "zones/";
	}
	
	void Init()
	{
		if (!GetGame().IsServer()) return;
		
		EnsureFolders();
		LoadPresets();
		LoadAllZones();
		
		// Register RPC handler
		GetDayZGame().Event_OnRPC.Insert(OnRPC);
		
		// Start spawn logic
		GPDA_MutantSpawnerLogic.GetInstance().RefreshZones(m_Zones);
		
		Print("[GPDA] MutantSpawnerHandler initialized (JSON): " + m_Zones.Count().ToString() + " zones, " + m_Presets.Count().ToString() + " presets");
	}
	
	void ~GPDA_MutantSpawnerHandler()
	{
		GetDayZGame().Event_OnRPC.Remove(OnRPC);
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// FOLDER MANAGEMENT
	// ══════════════════════════════════════════════════════════════════════
	
	private void EnsureFolders()
	{
		if (!FileExist("$profile:Greshnik_PDALITE"))
			MakeDirectory("$profile:Greshnik_PDALITE");
		if (!FileExist(m_BasePath))
			MakeDirectory(m_BasePath);
		if (!FileExist(m_ZonesPath))
			MakeDirectory(m_ZonesPath);
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// PRESETS LOAD / SAVE
	// ══════════════════════════════════════════════════════════════════════
	
	private void LoadPresets()
	{
		m_Presets.Clear();
		
		if (FileExist(m_PresetsPath))
		{
			string jsonErr;
			ref array<ref GPDA_MutantPreset> loaded;
			
			string fileContent = "";
			FileHandle fh = OpenFile(m_PresetsPath, FileMode.READ);
			if (fh != 0)
			{
				string line;
				while (FGets(fh, line) >= 0)
				{
					fileContent += line;
				}
				CloseFile(fh);
			}
			
			if (fileContent != "")
			{
				if (JsonSerializer().ReadFromString(loaded, fileContent, jsonErr))
				{
					if (loaded)
					{
						for (int i = 0; i < loaded.Count(); i++)
							m_Presets.Insert(loaded[i]);
					}
				}
				else
				{
					Print("[GPDA] MutantSpawnerHandler: Failed to parse presets: " + jsonErr);
				}
			}
			
			Print("[GPDA] MutantSpawnerHandler: Loaded " + m_Presets.Count().ToString() + " presets");
		}
		else
		{
			// First run - create default presets
			GPDA_MutantSpawnerConfig defaultConfig = GPDA_MutantSpawnerConfig.CreateDefault();
			for (int j = 0; j < defaultConfig.Presets.Count(); j++)
				m_Presets.Insert(defaultConfig.Presets[j]);
			
			SavePresets();
			Print("[GPDA] MutantSpawnerHandler: Created default presets (first run)");
		}
	}
	
	private void SavePresets()
	{
		string jsonStr;
		JsonSerializer().WriteToString(m_Presets, false, jsonStr);
		
		FileHandle fh = OpenFile(m_PresetsPath, FileMode.WRITE);
		if (fh != 0)
		{
			FPrint(fh, jsonStr);
			CloseFile(fh);
		}
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// ZONES LOAD / SAVE (each zone = separate JSON file)
	// ══════════════════════════════════════════════════════════════════════
	
	private void LoadAllZones()
	{
		m_Zones.Clear();
		
		string fileName;
		FileAttr fileAttr;
		FindFileHandle findHandle = FindFile(m_ZonesPath + "*.json", fileName, fileAttr, FindFileFlags.ALL);
		
		if (findHandle)
		{
			LoadZoneFile(fileName);
			
			while (FindNextFile(findHandle, fileName, fileAttr))
			{
				LoadZoneFile(fileName);
			}
			CloseFindFile(findHandle);
		}
		
		Print("[GPDA] MutantSpawnerHandler: Loaded " + m_Zones.Count().ToString() + " zones");
	}
	
	private void LoadZoneFile(string fileName)
	{
		string fullPath = m_ZonesPath + fileName;
		if (!FileExist(fullPath)) return;
		
		string fileContent = "";
		FileHandle fh = OpenFile(fullPath, FileMode.READ);
		if (fh != 0)
		{
			string line;
			while (FGets(fh, line) >= 0)
			{
				fileContent += line;
			}
			CloseFile(fh);
		}
		
		if (fileContent == "") return;
		
		string jsonErr;
		ref GPDA_SpawnZoneData zone;
		if (JsonSerializer().ReadFromString(zone, fileContent, jsonErr))
		{
			if (zone && zone.ZoneName != "")
			{
				m_Zones.Insert(zone);
				Print("[GPDA] MutantSpawnerHandler: Loaded zone '" + zone.ZoneName + "' (" + zone.Points.Count().ToString() + " points)");
			}
		}
		else
		{
			Print("[GPDA] MutantSpawnerHandler: Failed to parse zone " + fileName + ": " + jsonErr);
		}
	}
	
	private void SaveZone(GPDA_SpawnZoneData zone)
	{
		if (!zone || zone.ZoneName == "") return;
		
		string safeName = zone.ZoneName;
		safeName.Replace(" ", "_");
		safeName.Replace("/", "_");
		safeName.Replace("\\", "_");
		
		string fullPath = m_ZonesPath + safeName + ".json";
		
		string jsonStr;
		JsonSerializer().WriteToString(zone, false, jsonStr);
		
		FileHandle fh = OpenFile(fullPath, FileMode.WRITE);
		if (fh != 0)
		{
			FPrint(fh, jsonStr);
			CloseFile(fh);
		}
		
		Print("[GPDA] MutantSpawnerHandler: Saved zone '" + zone.ZoneName + "'");
	}
	
	private void DeleteZoneFile(string zoneName)
	{
		string safeName = zoneName;
		safeName.Replace(" ", "_");
		safeName.Replace("/", "_");
		safeName.Replace("\\", "_");
		
		string fullPath = m_ZonesPath + safeName + ".json";
		if (FileExist(fullPath))
		{
			DeleteFile(fullPath);
			Print("[GPDA] MutantSpawnerHandler: Deleted zone file '" + fullPath + "'");
		}
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// ADMIN CHECK
	// ══════════════════════════════════════════════════════════════════════
	
	private bool IsAdmin(PlayerIdentity identity)
	{
		if (!identity) return false;
		
		if (g_GPDA_AdminManager)
			return g_GPDA_AdminManager.IsAdmin(identity.GetPlainId());
		
		return false;
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// RPC DISPATCHER
	// ══════════════════════════════════════════════════════════════════════
	
	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (!GetGame().IsServer()) return;
		
		switch (rpc_type)
		{
			case GPDA_RPC.MSP_REQUEST_ZONES:
				HandleRequestZones(sender, ctx);
				break;
				
			case GPDA_RPC.MSP_REQUEST_PRESETS:
				HandleRequestPresets(sender, ctx);
				break;
				
			case GPDA_RPC.MSP_SAVE_ZONE:
				HandleSaveZone(sender, ctx);
				break;
				
			case GPDA_RPC.MSP_DELETE_ZONE:
				HandleDeleteZone(sender, ctx);
				break;
				
			case GPDA_RPC.MSP_TOGGLE_ZONE:
				HandleToggleZone(sender, ctx);
				break;
				
			case GPDA_RPC.MSP_SAVE_PRESETS:
				HandleSaveOnePreset(sender, ctx);
				break;
			
			case GPDA_RPC.MSP_SAVE_PRESETS_DONE:
				HandleSavePresetsDone(sender, ctx);
				break;
				
			case GPDA_RPC.MSP_FORCE_SPAWN_ALL:
				HandleForceSpawnAll(sender);
				break;
				
			case GPDA_RPC.MSP_DESPAWN_ALL:
				HandleDespawnAll(sender);
				break;
		}
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// ZONE OPERATIONS
	// ══════════════════════════════════════════════════════════════════════
	
	void HandleRequestZones(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender))
		{
			SendStatus(sender, "ACCESS_DENIED");
			return;
		}
		
		string jsonStr;
		JsonSerializer().WriteToString(m_Zones, false, jsonStr);
		
		ref Param1<string> data = new Param1<string>(jsonStr);
		GetGame().RPCSingleParam(null, GPDA_RPC.MSP_RECEIVE_ZONES, data, true, sender);
		
		Print("[GPDA] MutantSpawner: Sent " + m_Zones.Count().ToString() + " zones to " + sender.GetName());
	}
	
	void HandleSaveZone(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender)) return;
		
		ref Param2<string, string> params = new Param2<string, string>("", "");
		if (!ctx.Read(params)) return;
		
		string zoneName = params.param1;
		string jsonData = params.param2;
		
		string jsonError;
		ref GPDA_SpawnZoneData zone;
		if (!JsonSerializer().ReadFromString(zone, jsonData, jsonError))
		{
			SendStatus(sender, "Error: Failed to parse zone data! " + jsonError);
			return;
		}
		
		if (!zone)
		{
			SendStatus(sender, "Error: Zone data is null!");
			return;
		}
		
		// Check if zone was renamed
		bool isRename = false;
		if (zoneName != "" && zone.ZoneName != zoneName)
		{
			isRename = true;
			DeleteZoneFile(zoneName);
			Print("[GPDA] MutantSpawner: Zone renamed '" + zoneName + "' -> '" + zone.ZoneName + "'");
		}
		
		// Update local cache
		bool found = false;
		for (int i = 0; i < m_Zones.Count(); i++)
		{
			if (m_Zones[i].ZoneName == zoneName || m_Zones[i].ZoneName == zone.ZoneName)
			{
				m_Zones[i] = zone;
				found = true;
				break;
			}
		}
		
		if (!found)
		{
			zone.CreatedBy = sender.GetPlainId();
			zone.CreatedByName = sender.GetName();
			m_Zones.Insert(zone);
		}
		
		// Save to file
		SaveZone(zone);
		
		// Refresh spawn logic
		GPDA_MutantSpawnerLogic.GetInstance().RefreshZones(m_Zones);
		GPDA_MutantSpawnerLogic.GetInstance().ForceSpawnAll();
		
		SendStatus(sender, "Zone saved: " + zone.ZoneName + " (" + zone.Points.Count().ToString() + " points)");
		Print("[GPDA] MutantSpawner: Zone '" + zone.ZoneName + "' saved by " + sender.GetName());
	}
	
	void HandleDeleteZone(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender)) return;
		
		ref Param1<string> params = new Param1<string>("");
		if (!ctx.Read(params)) return;
		
		string zoneName = params.param1;
		
		for (int i = 0; i < m_Zones.Count(); i++)
		{
			if (m_Zones[i].ZoneName == zoneName)
			{
				m_Zones.Remove(i);
				break;
			}
		}
		
		DeleteZoneFile(zoneName);
		
		GPDA_MutantSpawnerLogic.GetInstance().RefreshZones(m_Zones);
		GPDA_MutantSpawnerLogic.GetInstance().ForceSpawnAll();
		
		SendStatus(sender, "Zone deleted: " + zoneName);
		Print("[GPDA] MutantSpawner: Zone '" + zoneName + "' deleted by " + sender.GetName());
	}
	
	void HandleToggleZone(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender)) return;
		
		ref Param2<string, bool> params = new Param2<string, bool>("", false);
		if (!ctx.Read(params)) return;
		
		string zoneName = params.param1;
		bool enabled = params.param2;
		
		for (int i = 0; i < m_Zones.Count(); i++)
		{
			if (m_Zones[i].ZoneName == zoneName)
			{
				m_Zones[i].IsEnabled = enabled;
				SaveZone(m_Zones[i]);
				break;
			}
		}
		
		GPDA_MutantSpawnerLogic.GetInstance().RefreshZones(m_Zones);
		GPDA_MutantSpawnerLogic.GetInstance().ForceSpawnAll();
		
		string state = "ENABLED";
		if (!enabled) state = "DISABLED";
		SendStatus(sender, zoneName + " -> " + state);
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// PRESET OPERATIONS
	// ══════════════════════════════════════════════════════════════════════
	
	void HandleRequestPresets(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender)) return;
		
		int cnt = m_Presets.Count();
		
		// Send count header
		ref Param1<int> countData = new Param1<int>(cnt);
		GetGame().RPCSingleParam(null, GPDA_RPC.MSP_RECEIVE_PRESETS, countData, true, sender);
		
		// Send each preset individually (avoids String CORRUPTED)
		for (int i = 0; i < m_Presets.Count(); i++)
		{
			string jsonStr;
			JsonSerializer().WriteToString(m_Presets[i], false, jsonStr);
			ref Param1<string> presetData = new Param1<string>(jsonStr);
			GetGame().RPCSingleParam(null, GPDA_RPC.MSP_RECEIVE_ONE_PRESET, presetData, true, sender);
		}
		
		Print("[GPDA] MutantSpawner: Sent " + cnt.ToString() + " presets to " + sender.GetName());
	}
	
	// Temporary storage for incoming presets from client (one-by-one)
	private ref array<ref GPDA_MutantPreset> m_IncomingPresets;
	
	void HandleSaveOnePreset(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender)) return;
		
		ref Param1<string> params = new Param1<string>("");
		if (!ctx.Read(params)) return;
		
		string jsonData = params.param1;
		string jsonErr;
		ref GPDA_MutantPreset preset;
		if (JsonSerializer().ReadFromString(preset, jsonData, jsonErr))
		{
			if (preset)
			{
				if (!m_IncomingPresets)
					m_IncomingPresets = new array<ref GPDA_MutantPreset>;
				m_IncomingPresets.Insert(preset);
			}
		}
		else
		{
			Print("[GPDA] MutantSpawner: Failed to parse preset: " + jsonErr);
		}
	}
	
	void HandleSavePresetsDone(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender)) return;
		
		if (!m_IncomingPresets || m_IncomingPresets.Count() == 0)
		{
			SendStatus(sender, "No presets received!");
			return;
		}
		
		// Detect changed presets for respawning
		ref array<string> changedPresetNames = new array<string>;
		DetectChangedPresets(m_IncomingPresets, changedPresetNames);
		
		m_Presets.Clear();
		for (int i = 0; i < m_IncomingPresets.Count(); i++)
			m_Presets.Insert(m_IncomingPresets[i]);
		
		m_IncomingPresets = null;
		
		// Respawn creatures for changed presets
		if (changedPresetNames.Count() > 0)
		{
			GPDA_MutantSpawnerLogic.GetInstance().RespawnForPresets(changedPresetNames);
			Print("[GPDA] MutantSpawner: Respawning " + changedPresetNames.Count().ToString() + " changed presets");
		}
		
		SavePresets();
		
		SendStatus(sender, "Presets saved: " + m_Presets.Count().ToString() + " total");
		Print("[GPDA] MutantSpawner: Presets saved by " + sender.GetName());
	}
	
	private void DetectChangedPresets(array<ref GPDA_MutantPreset> newPresets, array<string> outChanged)
	{
		for (int n = 0; n < newPresets.Count(); n++)
		{
			GPDA_MutantPreset newP = newPresets[n];
			bool isChanged = true;
			
			for (int o = 0; o < m_Presets.Count(); o++)
			{
				GPDA_MutantPreset oldP = m_Presets[o];
				if (oldP.Name != newP.Name) continue;
				
				if (oldP.ClassNames.Count() != newP.ClassNames.Count())
				{
					isChanged = true;
					break;
				}
				
				isChanged = false;
				for (int c = 0; c < oldP.ClassNames.Count(); c++)
				{
					if (oldP.ClassNames[c] != newP.ClassNames[c])
					{
						isChanged = true;
						break;
					}
				}
				break;
			}
			
			if (isChanged)
				outChanged.Insert(newP.Name);
		}
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// FORCE SPAWN / DESPAWN
	// ══════════════════════════════════════════════════════════════════════
	
	void HandleForceSpawnAll(PlayerIdentity sender)
	{
		if (!IsAdmin(sender)) return;
		
		GPDA_MutantSpawnerLogic.GetInstance().ForceSpawnAll();
		SendStatus(sender, "Force spawning all enabled zones...");
		Print("[GPDA] MutantSpawner: Force spawn all by " + sender.GetName());
	}
	
	void HandleDespawnAll(PlayerIdentity sender)
	{
		if (!IsAdmin(sender)) return;
		
		GPDA_MutantSpawnerLogic.GetInstance().DespawnAll();
		SendStatus(sender, "All creatures despawned.");
		Print("[GPDA] MutantSpawner: Despawn all by " + sender.GetName());
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// DATA ACCESSORS
	// ══════════════════════════════════════════════════════════════════════
	
	array<ref GPDA_SpawnZoneData> GetZones()
	{
		return m_Zones;
	}
	
	array<ref GPDA_MutantPreset> GetPresets()
	{
		return m_Presets;
	}
	
	GPDA_MutantPreset FindPreset(string name)
	{
		for (int i = 0; i < m_Presets.Count(); i++)
		{
			if (m_Presets[i].Name == name)
				return m_Presets[i];
		}
		return null;
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// HELPER
	// ══════════════════════════════════════════════════════════════════════
	
	void SendStatus(PlayerIdentity identity, string msg)
	{
		if (!identity) return;
		
		ref Param1<string> data = new Param1<string>(msg);
		GetGame().RPCSingleParam(null, GPDA_RPC.MSP_STATUS, data, true, identity);
	}
}

static ref GPDA_MutantSpawnerHandler g_GPDA_MSPHandler;
