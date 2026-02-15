// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA - MUTANT SPAWNER LOGIC
// Timer-based spawn loop: day/night checks, respawn, max alive tracking
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_MutantSpawnerLogic
{
	private static ref GPDA_MutantSpawnerLogic s_Instance;
	
	// Zone data (synced from handler)
	private ref array<ref GPDA_SpawnZoneData> m_Zones;
	
	// Spawned creatures tracker: key = "ZoneName_PointID"
	private ref map<string, ref array<ref GPDA_SpawnedCreatureInfo>> m_SpawnedCreatures;
	
	// Respawn timers: key = "ZoneName_PointID", value = next spawn time (game time in MS)
	private ref map<string, float> m_RespawnTimers;
	
	// Patrol timers: key = "ZoneName_PointID", value = time when patrol expires (game time in MS)
	private ref map<string, float> m_PatrolTimers;
	
	// Main loop timer
	private ref Timer m_SpawnTimer;
	private bool m_Initialized = false;
	
	// Config
	private static const float CHECK_INTERVAL = 10.0; // seconds between checks
	private static const float DAY_START_HOUR = 6.0;
	private static const float DAY_END_HOUR = 20.0;
	
	// ══════════════════════════════════════════════════════════════════════
	// SINGLETON
	// ══════════════════════════════════════════════════════════════════════
	
	static GPDA_MutantSpawnerLogic GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new GPDA_MutantSpawnerLogic();
		}
		return s_Instance;
	}
	
	void GPDA_MutantSpawnerLogic()
	{
		m_Zones = new array<ref GPDA_SpawnZoneData>;
		m_SpawnedCreatures = new map<string, ref array<ref GPDA_SpawnedCreatureInfo>>;
		m_RespawnTimers = new map<string, float>;
		m_PatrolTimers = new map<string, float>;
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// START / STOP
	// ══════════════════════════════════════════════════════════════════════
	
	void Start()
	{
		if (m_Initialized) return;
		m_Initialized = true;
		
		// Start main loop
		m_SpawnTimer = new Timer();
		m_SpawnTimer.Run(CHECK_INTERVAL, this, "OnSpawnCheck", null, true);
		
		Print("[GPDA] MutantSpawnerLogic: Started (check every " + CHECK_INTERVAL + "s)");
	}
	
	void Stop()
	{
		if (m_SpawnTimer)
			m_SpawnTimer.Stop();
		
		m_Initialized = false;
		Print("[GPDA] MutantSpawnerLogic: Stopped");
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// DATA SYNC FROM HANDLER
	// ══════════════════════════════════════════════════════════════════════
	
	void RefreshZones(array<ref GPDA_SpawnZoneData> zones)
	{
		m_Zones = zones;
		
		if (!m_Initialized)
			Start();
		
		Print("[GPDA] MutantSpawnerLogic: Zones refreshed (" + m_Zones.Count() + " zones)");
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// MAIN SPAWN CHECK LOOP (called every CHECK_INTERVAL seconds)
	// ══════════════════════════════════════════════════════════════════════
	
	void OnSpawnCheck()
	{
		if (!GetGame().IsServer()) return;
		
		float gameTime = GetGame().GetTime(); // ms since server start
		bool isDaytime = IsDaytime();
		
		// Clean up dead creatures first
		CleanupDead();
		
		// Get all players once for trigger radius checks
		array<Man> allPlayers = new array<Man>;
		GetGame().GetPlayers(allPlayers);
		
		// Check each zone
		for (int z = 0; z < m_Zones.Count(); z++)
		{
			GPDA_SpawnZoneData zone = m_Zones[z];
			if (!zone || !zone.IsEnabled) continue;
			
			// Check each point in zone
			for (int p = 0; p < zone.Points.Count(); p++)
			{
				GPDA_SpawnPointData point = zone.Points[p];
				if (!point || !point.IsActive) continue;
				
				string pointKey = zone.ZoneName + "_" + point.PointID.ToString();
				
				// Check spawn mode vs time of day
				if (!ShouldSpawnForMode(point.SpawnMode, isDaytime))
				{
					// If wrong time of day, despawn creatures for this point
					DespawnPointCreatures(pointKey);
					continue;
				}
				
				// --- TRIGGER RADIUS CHECK ---
				// Only spawn if at least one player is within TriggerRadius
				float triggerR = point.TriggerRadius;
				if (triggerR <= 0) triggerR = 200.0; // fallback for old data without field
				
				bool playerNearby = false;
				foreach (Man man : allPlayers)
				{
					if (!man || !man.IsAlive()) continue;
					float dist = vector.Distance(man.GetPosition(), point.Position);
					if (dist <= triggerR)
					{
						playerNearby = true;
						break;
					}
				}
				
				if (!playerNearby)
				{
					// No players in trigger radius — despawn creatures and reset timers
					DespawnPointCreatures(pointKey);
					m_RespawnTimers.Remove(pointKey);
					m_PatrolTimers.Remove(pointKey);
					continue;
				}
				
				// --- PATROL TIMER CHECK ---
				if (point.PatrolEnabled && m_PatrolTimers.Contains(pointKey))
				{
					float patrolExpiry = m_PatrolTimers.Get(pointKey);
					if (gameTime >= patrolExpiry)
					{
						// Patrol time expired — despawn and wait for respawn
						Print("[GPDA] MutantSpawner: Patrol expired for " + pointKey + ", despawning");
						DespawnPointCreatures(pointKey);
						m_PatrolTimers.Remove(pointKey);
						float nextRespawn = gameTime + (point.RespawnTimeSec * 1000.0);
						m_RespawnTimers.Set(pointKey, nextRespawn);
						continue;
					}
				}
				
				// Check alive count
				int aliveCount = GetAliveCount(pointKey);
				
				if (aliveCount >= point.MaxAlive)
					continue; // Already at max capacity
				
				// Check respawn timer
				if (m_RespawnTimers.Contains(pointKey))
				{
					float nextSpawn = m_RespawnTimers.Get(pointKey);
					if (gameTime < nextSpawn)
						continue; // Not time yet
				}
				
				// Calculate how many to spawn
				int targetCount = Math.RandomIntInclusive(point.CountMin, point.CountMax);
				int toSpawn = targetCount - aliveCount;
				if (toSpawn <= 0) continue;
				if (aliveCount + toSpawn > point.MaxAlive)
					toSpawn = point.MaxAlive - aliveCount;
				
				// Find the preset
				GPDA_MutantPreset preset = GPDA_MutantSpawnerHandler.GetInstance().FindPreset(point.PresetName);
				if (!preset || preset.ClassNames.Count() == 0)
				{
					Print("[GPDA] MutantSpawner WARNING: Preset '" + point.PresetName + "' not found for point " + pointKey);
					continue;
				}
				
				// Spawn creatures
				SpawnCreatures(zone.ZoneName, point, preset, toSpawn);
				
				// Start patrol timer if patrol enabled and not already running
				if (point.PatrolEnabled && !m_PatrolTimers.Contains(pointKey))
				{
					float patrolEnd = gameTime + (point.PatrolTimeSec * 1000.0);
					m_PatrolTimers.Set(pointKey, patrolEnd);
					Print("[GPDA] MutantSpawner: Patrol started for " + pointKey + " (" + point.PatrolTimeSec + "s)");
				}
				
				// Set respawn timer
				float nextTime = gameTime + (point.RespawnTimeSec * 1000.0);
				m_RespawnTimers.Set(pointKey, nextTime);
			}
		}
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// SPAWN CREATURES
	// ══════════════════════════════════════════════════════════════════════
	
	void SpawnCreatures(string zoneName, GPDA_SpawnPointData point, GPDA_MutantPreset preset, int count)
	{
		string pointKey = zoneName + "_" + point.PointID.ToString();
		
		if (!m_SpawnedCreatures.Contains(pointKey))
		{
			m_SpawnedCreatures.Set(pointKey, new array<ref GPDA_SpawnedCreatureInfo>);
		}
		
		ref array<ref GPDA_SpawnedCreatureInfo> creatures = m_SpawnedCreatures.Get(pointKey);
		
		for (int i = 0; i < count; i++)
		{
			// Pick random class from preset
			int classIdx = Math.RandomInt(0, preset.ClassNames.Count());
			string className = preset.ClassNames[classIdx];
			
			// Calculate random position within radius
			vector spawnPos = GetRandomPosInRadius(point.Position, point.SpawnRadius);
			
			// Snap to ground
			spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
			
			// Create the entity
			Object obj = GetGame().CreateObject(className, spawnPos, false, true, true);
			if (!obj)
			{
				Print("[GPDA] MutantSpawner ERROR: Failed to spawn " + className + " at " + spawnPos.ToString());
				continue;
			}
			
			// If it's a DayZInfected (zombie), set up AI behavior
			DayZInfected infected = DayZInfected.Cast(obj);
			if (infected)
			{
				// The infected will automatically activate its AI upon creation
				// No additional setup needed for basic behavior
			}
			
			// Track spawned creature
			ref GPDA_SpawnedCreatureInfo info = new GPDA_SpawnedCreatureInfo();
			info.PointID = point.PointID;
			info.ZoneName = zoneName;
			info.CreatureObj = EntityAI.Cast(obj);
			info.SpawnTime = GetGame().GetTime();
			info.ClassName = className;
			info.HomePosition = point.Position;
			info.PatrolRadius = point.PatrolRadius;
			
			creatures.Insert(info);
		}
		
		Print("[GPDA] MutantSpawner: Spawned " + count + "x at point " + pointKey + " (preset: " + preset.Name + ")");
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// CLEANUP & DESPAWN
	// ══════════════════════════════════════════════════════════════════════
	
	void CleanupDead()
	{
		ref array<string> keys = new array<string>;
		
		for (int i = 0; i < m_SpawnedCreatures.Count(); i++)
		{
			keys.Insert(m_SpawnedCreatures.GetKey(i));
		}
		
		foreach (string key : keys)
		{
			ref array<ref GPDA_SpawnedCreatureInfo> creatures = m_SpawnedCreatures.Get(key);
			if (!creatures) continue;
			
			for (int c = creatures.Count() - 1; c >= 0; c--)
			{
				GPDA_SpawnedCreatureInfo info = creatures[c];
				if (!info || !info.CreatureObj || !info.CreatureObj.IsAlive())
				{
					// Entity is dead or deleted
					if (info && info.CreatureObj && !info.CreatureObj.IsAlive())
					{
						// Dead entity - will be cleaned up by engine eventually
					}
					creatures.Remove(c);
				}
			}
		}
	}
	
	void DespawnPointCreatures(string pointKey)
	{
		if (!m_SpawnedCreatures.Contains(pointKey)) return;
		
		ref array<ref GPDA_SpawnedCreatureInfo> creatures = m_SpawnedCreatures.Get(pointKey);
		if (!creatures) return;
		
		for (int i = creatures.Count() - 1; i >= 0; i--)
		{
			GPDA_SpawnedCreatureInfo info = creatures[i];
			if (info && info.CreatureObj)
			{
				GetGame().ObjectDelete(info.CreatureObj);
			}
		}
		creatures.Clear();
	}
	
	void DespawnAll()
	{
		ref array<string> keys = new array<string>;
		
		for (int i = 0; i < m_SpawnedCreatures.Count(); i++)
		{
			keys.Insert(m_SpawnedCreatures.GetKey(i));
		}
		
		foreach (string key : keys)
		{
			DespawnPointCreatures(key);
		}
		
		m_SpawnedCreatures.Clear();
		m_RespawnTimers.Clear();
		m_PatrolTimers.Clear();
		
		Print("[GPDA] MutantSpawnerLogic: All creatures despawned");
	}
	
	void ForceSpawnAll()
	{
		// Reset all timers so everything spawns immediately
		m_RespawnTimers.Clear();
		
		// Run immediate check
		OnSpawnCheck();
		
		Print("[GPDA] MutantSpawnerLogic: Force spawn triggered");
	}
	
	// Respawn creatures for specific presets (when preset classes changed)
	void RespawnForPresets(array<string> presetNames)
	{
		if (!presetNames || presetNames.Count() == 0) return;
		
		for (int z = 0; z < m_Zones.Count(); z++)
		{
			GPDA_SpawnZoneData zone = m_Zones[z];
			if (!zone || !zone.IsEnabled) continue;
			
			for (int p = 0; p < zone.Points.Count(); p++)
			{
				GPDA_SpawnPointData point = zone.Points[p];
				if (!point || !point.IsActive) continue;
				
				string pointPreset = point.PresetName;
				bool needsRespawn = false;
				
				for (int n = 0; n < presetNames.Count(); n++)
				{
					if (pointPreset == presetNames[n])
					{
						needsRespawn = true;
						break;
					}
				}
				
				if (needsRespawn)
				{
					string pointKey = zone.ZoneName + "_" + point.PointID.ToString();
					DespawnPointCreatures(pointKey);
					m_RespawnTimers.Remove(pointKey);
				}
			}
		}
		
		// Trigger immediate spawn check
		OnSpawnCheck();
		
		Print("[GPDA] MutantSpawnerLogic: Respawned for " + presetNames.Count().ToString() + " changed presets");
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// HELPER METHODS
	// ══════════════════════════════════════════════════════════════════════
	
	int GetAliveCount(string pointKey)
	{
		if (!m_SpawnedCreatures.Contains(pointKey)) return 0;
		
		ref array<ref GPDA_SpawnedCreatureInfo> creatures = m_SpawnedCreatures.Get(pointKey);
		if (!creatures) return 0;
		
		int count = 0;
		for (int i = 0; i < creatures.Count(); i++)
		{
			GPDA_SpawnedCreatureInfo info = creatures[i];
			if (info && info.CreatureObj && info.CreatureObj.IsAlive())
				count++;
		}
		return count;
	}
	
	int GetTotalAliveCount()
	{
		int total = 0;
		for (int i = 0; i < m_SpawnedCreatures.Count(); i++)
		{
			string key = m_SpawnedCreatures.GetKey(i);
			total += GetAliveCount(key);
		}
		return total;
	}
	
	bool IsDaytime()
	{
		if (!GetGame() || !GetGame().GetWorld()) return true;
		
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		
		float timeDecimal = hour + (minute / 60.0);
		return (timeDecimal >= DAY_START_HOUR && timeDecimal < DAY_END_HOUR);
	}
	
	bool ShouldSpawnForMode(int spawnMode, bool isDaytime)
	{
		switch (spawnMode)
		{
			case 0: return true;          // Always
			case 1: return isDaytime;     // Day only
			case 2: return !isDaytime;    // Night only
		}
		return true;
	}
	
	vector GetRandomPosInRadius(vector center, float radius)
	{
		if (radius <= 0) return center;
		
		float angle = Math.RandomFloat(0, Math.PI2);
		float dist = Math.RandomFloat(0, radius);
		
		vector pos = center;
		pos[0] = center[0] + Math.Cos(angle) * dist;
		pos[2] = center[2] + Math.Sin(angle) * dist;
		
		return pos;
	}
}
