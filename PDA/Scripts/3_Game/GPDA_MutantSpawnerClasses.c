// ============================================================================
// GPDA Mutant Spawner - Data Classes
// Shared between Client and Server
// ============================================================================

// --- Mutant Preset: template for a group of creatures ---
class GPDA_MutantPreset
{
	string Name;
	ref array<string> ClassNames;
	string IconColor;
	int Difficulty; // 1=Easy, 2=Medium, 3=Hard, 4=Boss

	void GPDA_MutantPreset()
	{
		ClassNames = new array<string>();
		Name = "";
		IconColor = "#FF6644";
		Difficulty = 1;
	}
}

// --- Single Spawn Point within a Zone ---
class GPDA_SpawnPointData
{
	int PointID;
	string PointName;
	vector Position;
	float SpawnRadius;
	string PresetName;
	int CountMin;
	int CountMax;
	int MaxAlive;
	float RespawnTimeSec;
	int SpawnMode; // 0=Always, 1=DayOnly, 2=NightOnly
	bool IsActive;
	float PatrolRadius;
	float TriggerRadius; // Player must be within this distance to activate spawning
	bool PatrolEnabled;     // If true, entities despawn after PatrolTimeSec
	float PatrolTimeSec;    // Seconds entities stay alive (patrol duration)

	void GPDA_SpawnPointData()
	{
		PointID = 0;
		PointName = "";
		SpawnRadius = 5.0;
		PresetName = "";
		CountMin = 1;
		CountMax = 3;
		MaxAlive = 5;
		RespawnTimeSec = 300.0;
		SpawnMode = 0;
		IsActive = true;
		PatrolRadius = 50.0;
		TriggerRadius = 200.0;
		PatrolEnabled = false;
		PatrolTimeSec = 120.0;
	}
}

// --- Spawn Zone: named collection of spawn points ---
class GPDA_SpawnZoneData
{
	string ZoneName;
	string CreatedBy;
	string CreatedByName;
	bool IsEnabled;
	ref array<ref GPDA_SpawnPointData> Points;

	void GPDA_SpawnZoneData()
	{
		ZoneName = "New Zone";
		CreatedBy = "";
		CreatedByName = "";
		IsEnabled = true;
		Points = new array<ref GPDA_SpawnPointData>();
	}

	int GetNextPointID()
	{
		int maxID = 0;
		for (int i = 0; i < Points.Count(); i++)
		{
			if (Points[i].PointID > maxID)
				maxID = Points[i].PointID;
		}
		return maxID + 1;
	}

	GPDA_SpawnPointData FindPoint(int pointID)
	{
		for (int i = 0; i < Points.Count(); i++)
		{
			if (Points[i].PointID == pointID)
				return Points[i];
		}
		return null;
	}

	bool RemovePoint(int pointID)
	{
		for (int i = 0; i < Points.Count(); i++)
		{
			if (Points[i].PointID == pointID)
			{
				Points.Remove(i);
				return true;
			}
		}
		return false;
	}
}

// --- Full config saved to server JSON ---
class GPDA_MutantSpawnerConfig
{
	ref array<ref GPDA_MutantPreset> Presets;
	ref array<ref GPDA_SpawnZoneData> Zones;

	void GPDA_MutantSpawnerConfig()
	{
		Presets = new array<ref GPDA_MutantPreset>();
		Zones = new array<ref GPDA_SpawnZoneData>();
	}

	static GPDA_MutantSpawnerConfig CreateDefault()
	{
		GPDA_MutantSpawnerConfig cfg = new GPDA_MutantSpawnerConfig();

		GPDA_MutantPreset p1 = new GPDA_MutantPreset();
		p1.Name = "Zombies - Regular";
		p1.Difficulty = 1;
		p1.IconColor = "#88AA66";
		p1.ClassNames.Insert("ZmbM_NBC_Grey");
		p1.ClassNames.Insert("ZmbM_PatrolNormal_Summer");
		p1.ClassNames.Insert("ZmbF_JournalistNormal_White");
		p1.ClassNames.Insert("ZmbM_HermitSkinny_Beige");
		p1.ClassNames.Insert("ZmbM_CitizenASkinny_Brown");
		cfg.Presets.Insert(p1);

		GPDA_MutantPreset p2 = new GPDA_MutantPreset();
		p2.Name = "Zombies - Military";
		p2.Difficulty = 2;
		p2.IconColor = "#CCAA33";
		p2.ClassNames.Insert("ZmbM_SoldierNormal");
		p2.ClassNames.Insert("ZmbM_usSoldier_normal_Woodland");
		p2.ClassNames.Insert("ZmbM_PatrolNormal_PautRev");
		p2.ClassNames.Insert("ZmbM_PatrolNormal_Autumn");
		cfg.Presets.Insert(p2);

		GPDA_MutantPreset p3 = new GPDA_MutantPreset();
		p3.Name = "Bloodsuckers";
		p3.Difficulty = 3;
		p3.IconColor = "#CC3333";
		p3.ClassNames.Insert("Bloodsucker_Green");
		p3.ClassNames.Insert("Bloodsucker_Strong");
		p3.ClassNames.Insert("Bloodsucker_Dark");
		cfg.Presets.Insert(p3);

		GPDA_MutantPreset p4 = new GPDA_MutantPreset();
		p4.Name = "Snorks";
		p4.Difficulty = 3;
		p4.IconColor = "#DD5522";
		p4.ClassNames.Insert("Snork");
		p4.ClassNames.Insert("Snork_Strong");
		cfg.Presets.Insert(p4);

		GPDA_MutantPreset p5 = new GPDA_MutantPreset();
		p5.Name = "Mutant Dogs";
		p5.Difficulty = 2;
		p5.IconColor = "#AA7744";
		p5.ClassNames.Insert("Wolf_White");
		p5.ClassNames.Insert("Wolf_Grey");
		p5.ClassNames.Insert("PseudoDog");
		cfg.Presets.Insert(p5);

		GPDA_MutantPreset p6 = new GPDA_MutantPreset();
		p6.Name = "Boss - Chimera";
		p6.Difficulty = 4;
		p6.IconColor = "#FF1111";
		p6.ClassNames.Insert("Chimera_Alpha");
		p6.ClassNames.Insert("Pseudogiant");
		cfg.Presets.Insert(p6);

		return cfg;
	}

	GPDA_MutantPreset FindPreset(string name)
	{
		for (int i = 0; i < Presets.Count(); i++)
		{
			if (Presets[i].Name == name)
				return Presets[i];
		}
		return null;
	}

	GPDA_SpawnZoneData FindZone(string zoneName)
	{
		for (int i = 0; i < Zones.Count(); i++)
		{
			if (Zones[i].ZoneName == zoneName)
				return Zones[i];
		}
		return null;
	}

	bool RemoveZone(string zoneName)
	{
		for (int i = 0; i < Zones.Count(); i++)
		{
			if (Zones[i].ZoneName == zoneName)
			{
				Zones.Remove(i);
				return true;
			}
		}
		return false;
	}
}

// --- Runtime tracker for spawned entities (server-only) ---
class GPDA_SpawnedCreatureInfo
{
	int PointID;
	string ZoneName;
	Object CreatureObj;
	float SpawnTime;
	string ClassName;
	vector HomePosition;
	float PatrolRadius;

	void GPDA_SpawnedCreatureInfo()
	{
		PointID = 0;
		ZoneName = "";
		CreatureObj = null;
		SpawnTime = 0;
		ClassName = "";
		PatrolRadius = 0;
	}
}

// --- RPC data packets ---
class GPDA_MSP_ZoneListPacket
{
	ref array<string> ZoneNames;
	ref array<bool> ZoneEnabled;
	ref array<int> ZonePointCounts;
	ref array<int> ZoneAliveCreatures;

	void GPDA_MSP_ZoneListPacket()
	{
		ZoneNames = new array<string>();
		ZoneEnabled = new array<bool>();
		ZonePointCounts = new array<int>();
		ZoneAliveCreatures = new array<int>();
	}
}

// Spawn mode helpers
class GPDA_SpawnModeHelper
{
	static string GetModeName(int mode)
	{
		switch (mode)
		{
			case 0: return "Always";
			case 1: return "Day Only";
			case 2: return "Night Only";
		}
		return "Unknown";
	}

	static int GetModeCount()
	{
		return 3;
	}

	static int GetColor(int mode)
	{
		switch (mode)
		{
			case 0: return ARGB(255, 170, 210, 170);
			case 1: return ARGB(255, 220, 195, 75);
			case 2: return ARGB(255, 130, 130, 210);
		}
		return ARGB(255, 190, 190, 190);
	}
}

// Difficulty helpers
class GPDA_DifficultyHelper
{
	static string GetName(int diff)
	{
		switch (diff)
		{
			case 1: return "Easy";
			case 2: return "Medium";
			case 3: return "Hard";
			case 4: return "Boss";
		}
		return "?";
	}

	static int GetColor(int diff)
	{
		switch (diff)
		{
			case 1: return ARGB(255, 136, 204, 102);
			case 2: return ARGB(255, 204, 204, 68);
			case 3: return ARGB(255, 221, 102, 51);
			case 4: return ARGB(255, 255, 34, 34);
		}
		return ARGB(255, 170, 170, 170);
	}
}
