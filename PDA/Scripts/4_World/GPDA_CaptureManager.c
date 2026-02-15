// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Capture Zone Manager (Flag Pole Based)
// Server-side capture logic: player interacts with TerritoryFlag → GUI → capture
// Vanilla ref: P:\scripts\4_world\entities\itembase\basebuildingbase\totem.c
// Vanilla ref: P:\scripts\4_world\entities\itembase\flag_base.c
// ═══════════════════════════════════════════════════════════════════════════════

// Ownership record per flag pole (keyed by position hash)
class GPDA_FlagPoleData
{
	string PosHash;
	string OwnerGroup;
	string FlagClass;
	float PosX;
	float PosZ;
	int RewardPoints;
	float Radius;
	
	void GPDA_FlagPoleData()
	{
		PosHash = "";
		OwnerGroup = "";
		FlagClass = "";
		PosX = 0;
		PosZ = 0;
		RewardPoints = 100;
		Radius = 50.0;
	}
}

// All flag pole ownership data saved to one file
class GPDA_FlagPoleConfig
{
	ref array<ref GPDA_FlagPoleData> FlagPoles;
	
	void GPDA_FlagPoleConfig()
	{
		FlagPoles = new array<ref GPDA_FlagPoleData>;
	}
}

// Active capture state per player
class GPDA_ActiveCapture
{
	string PlayerUID;
	string PosHash;       // Flag pole position hash (for finding the pole)
	string ZoneName;      // Admin zone name (for ownership records)
	string FlagClass;
	float Progress;       // 0..100
	float ElapsedTime;
	
	void GPDA_ActiveCapture()
	{
		Progress = 0;
		ElapsedTime = 0;
	}
}

class GPDA_CaptureManager
{
	protected static ref GPDA_CaptureManager m_Instance;
	protected ref GPDA_FlagPoleConfig m_Config;
	protected ref map<string, ref GPDA_ActiveCapture> m_ActiveCaptures; // uid -> capture
	protected ref map<string, float> m_AttachCooldown; // uid -> last attach time
	
	static const float CAPTURE_TIME = 60.0;     // Base 60 seconds
	static const float CAPTURE_RADIUS = 50.0;   // Radius to count players
	static const int   REWARD_POINTS = 100;
	
	protected string m_SavePath;
	
	// Icon constants
	static string ICON_CONNECT = "set:dayz_gui image:icon_connect";
	static string ICON_X = "set:dayz_gui image:icon_x";
	
	static GPDA_CaptureManager GetInstance()
	{
		if (!m_Instance)
			m_Instance = new GPDA_CaptureManager();
		return m_Instance;
	}
	
	void GPDA_CaptureManager()
	{
		m_Config = new GPDA_FlagPoleConfig();
		m_ActiveCaptures = new map<string, ref GPDA_ActiveCapture>;
		m_AttachCooldown = new map<string, float>;
		m_SavePath = "$profile:Greshnik_PDALITE/capture/flagpoles.json";
	}
	
	void Init()
	{
		EnsureFolders();
		LoadFromFile();
		
		if (GetGame() && GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.UpdateLoop, 1000, true);
			Print("[GPDA][Capture] Initialized - flag pole capture system started");
		}
	}
	
	void EnsureFolders()
	{
		if (!FileExist("$profile:Greshnik_PDALITE"))
			MakeDirectory("$profile:Greshnik_PDALITE");
		if (!FileExist("$profile:Greshnik_PDALITE/capture"))
			MakeDirectory("$profile:Greshnik_PDALITE/capture");
	}
	
	void LoadFromFile()
	{
		if (FileExist(m_SavePath))
		{
			JsonFileLoader<GPDA_FlagPoleConfig>.JsonLoadFile(m_SavePath, m_Config);
			Print("[GPDA][Capture] Loaded " + m_Config.FlagPoles.Count().ToString() + " flag pole records");
		}
		else
		{
			m_Config = new GPDA_FlagPoleConfig();
			Print("[GPDA][Capture] No save file, starting fresh");
		}
	}
	
	void SaveToFile()
	{
		EnsureFolders();
		JsonFileLoader<GPDA_FlagPoleConfig>.JsonSaveFile(m_SavePath, m_Config);
	}
	
	// ─── POSITION HASH ────────────────────────────────────────────
	static string GetPosHash(vector pos)
	{
		return Math.Floor(pos[0]).ToString() + "_" + Math.Floor(pos[2]).ToString();
	}
	
	// ─── GET / SET OWNERSHIP DATA ─────────────────────────────────
	GPDA_FlagPoleData GetFlagPoleData(string posHash)
	{
		foreach (GPDA_FlagPoleData fpd : m_Config.FlagPoles)
		{
			if (fpd && fpd.PosHash == posHash)
				return fpd;
		}
		return null;
	}
	
	// Find zone that contains the given position (for flag poles inside admin zones)
	GPDA_FlagPoleData GetFlagPoleDataByPosition(vector pos)
	{
		float minDist = 999999;
		GPDA_FlagPoleData nearest = null;
		
		foreach (GPDA_FlagPoleData fpd : m_Config.FlagPoles)
		{
			if (!fpd) continue;
			vector zonePos = Vector(fpd.PosX, 0, fpd.PosZ);
			vector flatPos = Vector(pos[0], 0, pos[2]);
			float dist = vector.Distance(flatPos, zonePos);
			
			// Use per-zone radius (fallback to constant for old data)
			float zoneRadius = fpd.Radius;
			if (zoneRadius <= 0) zoneRadius = CAPTURE_RADIUS;
			if (dist < zoneRadius && dist < minDist)
			{
				minDist = dist;
				nearest = fpd;
			}
		}
		
		if (nearest)
			Print("[GPDA][Capture] Found zone " + nearest.PosHash + " at dist=" + minDist.ToString());
		
		return nearest;
	}
	
	// ─── FIND TERRITORY FLAG BY POSITION HASH ─────────────────────
	TerritoryFlag FindFlagPoleByHash(string posHash)
	{
		// Search all objects near the expected position
		array<string> hashParts = new array<string>;
		posHash.Split("_", hashParts);
		if (hashParts.Count() < 2) return null;
		
		float px = hashParts[0].ToFloat();
		float pz = hashParts[1].ToFloat();
		vector searchPos = Vector(px, 0, pz);
		
		// Get terrain height
		searchPos[1] = GetGame().SurfaceY(px, pz);
		
		array<Object> objects = new array<Object>;
		array<CargoBase> cargos = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(searchPos, 5.0, objects, cargos);
		
		foreach (Object obj : objects)
		{
			TerritoryFlag tf = TerritoryFlag.Cast(obj);
			if (tf)
			{
				string objHash = GetPosHash(tf.GetPosition());
				if (objHash == posHash)
					return tf;
			}
		}
		return null;
	}
	
	// ─── RPC HANDLERS (called from ServerRPC) ─────────────────────
	
	// Client requests capture start: payload = "posHash" (flag pole position)
	void HandleCaptureStart(PlayerBase player, ParamsReadContext ctx)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		if (!player || !player.GetIdentity()) return;
		
		// Client sends flag pole posHash
		string posHash = data.param1;
		string uid = player.GetIdentity().GetPlainId();
		
		Print("[GPDA][Capture] START request: uid=" + uid + " flagpole pos=" + posHash);
		
		// Validate: player is in a group
		GPDA_GroupData group = GetPlayerGroup(uid);
		if (!group)
		{
			SendCaptureCancel(player, "You must be in a group!");
			return;
		}
		
		// Validate: flag pole exists
		TerritoryFlag flagPole = FindFlagPoleByHash(posHash);
		if (!flagPole)
		{
			SendCaptureCancel(player, "Flag pole not found!");
			return;
		}
		
		// CRITICAL: Find admin zone that contains this flag pole (by position, not by hash!)
		vector polePos = flagPole.GetPosition();
		GPDA_FlagPoleData fpData = GetFlagPoleDataByPosition(polePos);
		if (!fpData)
		{
			SendCaptureCancel(player, "No capture zone here! Admin must create zone first.");
			return;
		}
		
		// Validate: player is near the flag pole (use per-zone radius)
		float zoneR = fpData.Radius;
		if (zoneR <= 0) zoneR = CAPTURE_RADIUS;
		float dist = vector.Distance(player.GetPosition(), flagPole.GetPosition());
		if (dist > zoneR)
		{
			SendCaptureCancel(player, "Too far from flag pole!");
			return;
		}
		
		string zoneName = fpData.PosHash;  // Admin's zone name (e.g. "35")
		Print("[GPDA][Capture] Flag pole inside zone: " + zoneName);
		
		// Validate: not already owned by this group
		if (fpData.OwnerGroup == group.Name)
		{
			SendCaptureCancel(player, "Your group already owns this zone!");
			return;
		}
		
		// Validate: flag is already attached to the pole (player attached it via TAB drag)
		// Vanilla ref: totem.c — slot "Material_FPole_Flag"
		int flagSlotId = InventorySlots.GetSlotIdFromString("Material_FPole_Flag");
		EntityAI poleFlag = flagPole.GetInventory().FindAttachment(flagSlotId);
		if (!poleFlag)
		{
			SendCaptureCancel(player, "No flag on pole! Attach your group flag via TAB first.");
			return;
		}
		
		// Read flagClass from the pole (server authority — not from client!)
		string flagClass = poleFlag.GetType();
		
		// Validate: flag matches player's group FlagClass (if set)
		if (group.FlagClass != "" && !poleFlag.IsKindOf(group.FlagClass))
		{
			SendCaptureCancel(player, "Wrong flag! Your group needs: " + group.FlagClass);
			return;
		}
		
		Print("[GPDA][Capture] Flag on pole: " + flagClass + " group.FlagClass=" + group.FlagClass);
		
		// Validate: not already capturing
		if (m_ActiveCaptures.Contains(uid))
		{
			SendCaptureCancel(player, "Already capturing!");
			return;
		}
		
		// Start capture
		GPDA_ActiveCapture capture = new GPDA_ActiveCapture();
		capture.PlayerUID = uid;
		capture.PosHash = posHash;
		capture.ZoneName = zoneName;  // Store admin zone name!
		capture.FlagClass = flagClass;
		capture.Progress = 0;
		capture.ElapsedTime = 0;
		m_ActiveCaptures.Insert(uid, capture);
		
		Print("[GPDA][Capture] Started: " + group.Name + " capturing zone " + zoneName + " with " + flagClass);
		
		// Send initial progress
		SendCaptureProgress(player, capture, 0, 0, 1.0, CAPTURE_TIME);
	}
	
	// Client selects a flag in UI -> attach it to the pole LOWERED (phase 1.0)
	// Payload: "posHash,flagClassName"
	void HandleCaptureAttachFlag(PlayerBase player, ParamsReadContext ctx)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		if (!player || !player.GetIdentity()) return;
		
		string payload = data.param1;
		array<string> parts = new array<string>;
		payload.Split(",", parts);
		if (parts.Count() < 2)
		{
			SendCaptureCancel(player, "Invalid attach request");
			return;
		}
		
		string posHash = parts[0];
		string flagClass = parts[1];
		string uid = player.GetIdentity().GetPlainId();
		
		Print("[GPDA][Capture] ATTACH_FLAG: uid=" + uid + " pos=" + posHash + " flag=" + flagClass);
		
		// Cooldown: prevent double execution (2 second window)
		if (m_AttachCooldown.Contains(uid))
		{
			float lastTime = m_AttachCooldown.Get(uid);
			if (GetGame().GetTickTime() - lastTime < 2.0)
			{
				Print("[GPDA][Capture] ATTACH_FLAG: ignoring duplicate from " + uid);
				return;
			}
		}
		m_AttachCooldown.Set(uid, GetGame().GetTickTime());
		
		// Validate: player in a group
		GPDA_GroupData group = GetPlayerGroup(uid);
		if (!group)
		{
			SendCaptureCancel(player, "You must be in a group!");
			return;
		}
		
		// Validate: flag pole exists
		TerritoryFlag flagPole = FindFlagPoleByHash(posHash);
		if (!flagPole)
		{
			SendCaptureCancel(player, "Flag pole not found!");
			return;
		}
		
		// Validate: near pole (use per-zone radius)
		float attachRadius = CAPTURE_RADIUS;
		GPDA_FlagPoleData fpDataCheck = GetFlagPoleDataByPosition(flagPole.GetPosition());
		if (fpDataCheck && fpDataCheck.Radius > 0) attachRadius = fpDataCheck.Radius;
		float dist = vector.Distance(player.GetPosition(), flagPole.GetPosition());
		if (dist > attachRadius)
		{
			SendCaptureCancel(player, "Too far from flag pole!");
			return;
		}
		
		// Validate: not owned by this group
		GPDA_FlagPoleData fpData = GetFlagPoleData(posHash);
		if (fpData && fpData.OwnerGroup == group.Name)
		{
			SendCaptureCancel(player, "Your group already owns this zone!");
			return;
		}
		
		// Validate: flag is already attached to the pole
		// (player attached it via ATTACH_FLAG RPC before clicking START)
		// Vanilla ref: slot Material_FPole_Flag
		bool hasFlag = false;
		int checkSlotId = InventorySlots.GetSlotIdFromString("Material_FPole_Flag");
		EntityAI checkFlag = flagPole.GetInventory().FindAttachment(checkSlotId);
		if (checkFlag && checkFlag.GetType() == flagClass)
		{
			hasFlag = true;
		}
		
		// Fallback: check player inventory (flag not yet attached)
		if (!hasFlag)
		{
			array<EntityAI> items = new array<EntityAI>;
			player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
			foreach (EntityAI item : items)
			{
				if (item && item.GetType() == flagClass)
				{
					hasFlag = true;
					// Flag still in inventory — attach it now
					AttachFlagToFlagPole(player, flagPole, flagClass, 1.0);
					break;
				}
			}
		}
		
		if (!hasFlag)
		{
			SendCaptureCancel(player, "You don't have that flag!");
			return;
		}
		
		// Send confirmation via CAPTURE_INFO with ATTACHED marker
		string ownerGroup = "";
		string reward = REWARD_POINTS.ToString();
		if (fpData)
		{
			ownerGroup = fpData.OwnerGroup;
			reward = fpData.RewardPoints.ToString();
		}
		string infoPayload = ownerGroup + "," + group.Name + "," + reward + ",ATTACHED";
		Param1<string> infoData = new Param1<string>(infoPayload);
		GetGame().RPCSingleParam(player, GPDA_RPC.CAPTURE_INFO, infoData, true, player.GetIdentity());
		
		Print("[GPDA][Capture] Flag " + flagClass + " confirmed on pole at " + posHash);
	}
	
	// Client requests cancel
	void HandleCaptureCancel(PlayerBase player, ParamsReadContext ctx)
	{
		if (!player || !player.GetIdentity()) return;
		string uid = player.GetIdentity().GetPlainId();
		
		if (m_ActiveCaptures.Contains(uid))
		{
			m_ActiveCaptures.Remove(uid);
			Print("[GPDA][Capture] Player " + uid + " cancelled capture");
		}
	}
	
	// Client requests zone info: payload = "posHash" (flag pole position)
	void HandleCaptureRequestInfo(PlayerBase player, ParamsReadContext ctx)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		if (!player || !player.GetIdentity()) return;
		
		string posHash = data.param1;
		string uid = player.GetIdentity().GetPlainId();
		
		// Find flag pole to get actual position
		TerritoryFlag flagPole = FindFlagPoleByHash(posHash);
		GPDA_FlagPoleData fpData = null;
		
		if (flagPole)
		{
			// Find zone by position (not by posHash!)
			fpData = GetFlagPoleDataByPosition(flagPole.GetPosition());
		}
		
		string ownerGroup = "";
		string reward = REWARD_POINTS.ToString();
		string zoneName = "No zone";
		
		if (fpData)
		{
			ownerGroup = fpData.OwnerGroup;
			reward = fpData.RewardPoints.ToString();
			zoneName = fpData.PosHash;
		}
		
		// Get player's group name
		string yourGroup = "None";
		GPDA_GroupData group = GetPlayerGroup(uid);
		if (group) yourGroup = group.Name;
		
		// Send info: "ownerGroup,yourGroup,reward,flagClass"
		string flagInfo = "";
		if (group)
			flagInfo = group.FlagClass;
		string infoPayload = ownerGroup + "," + yourGroup + "," + reward + "," + flagInfo;
		Param1<string> infoData = new Param1<string>(infoPayload);
		GetGame().RPCSingleParam(player, GPDA_RPC.CAPTURE_INFO, infoData, true, player.GetIdentity());
		
		Print("[GPDA][Capture] Sent info: zone=" + zoneName + " owner=" + ownerGroup + " to " + uid);
	}
	
	// ─── UPDATE LOOP (1 second tick — server only) ────────────────
	void UpdateLoop()
	{
		if (!GetGame() || !GetGame().IsServer()) return;
		if (m_ActiveCaptures.Count() == 0) return;
		
		array<Man> allPlayers = new array<Man>;
		GetGame().GetPlayers(allPlayers);
		
		// Iterate active captures (copy keys to avoid modification during iteration)
		array<string> uids = new array<string>;
		for (int k = 0; k < m_ActiveCaptures.Count(); k++)
			uids.Insert(m_ActiveCaptures.GetKey(k));
		
		foreach (string uid : uids)
		{
			if (!m_ActiveCaptures.Contains(uid)) continue;
			
			GPDA_ActiveCapture capture = m_ActiveCaptures.Get(uid);
			if (!capture) continue;
			
			// Find the capturing player
			PlayerBase capturer = null;
			foreach (Man man : allPlayers)
			{
				PlayerBase pb = PlayerBase.Cast(man);
				if (pb && pb.GetIdentity() && pb.GetIdentity().GetPlainId() == uid)
				{
					capturer = pb;
					break;
				}
			}
			
			if (!capturer || !capturer.IsAlive())
			{
				// Player disconnected or dead
				m_ActiveCaptures.Remove(uid);
				if (capturer) SendCaptureCancel(capturer, "You died or disconnected");
				continue;
			}
			
			// Check distance to flag pole
			TerritoryFlag flagPole = FindFlagPoleByHash(capture.PosHash);
			if (!flagPole)
			{
				m_ActiveCaptures.Remove(uid);
				SendCaptureCancel(capturer, "Flag pole destroyed!");
				continue;
			}
			
			// Get per-zone radius (look up fpData early)
			GPDA_FlagPoleData fpData = GetFlagPoleData(capture.PosHash);
			float captureZoneR = CAPTURE_RADIUS;
			if (fpData && fpData.Radius > 0) captureZoneR = fpData.Radius;
			
			float dist = vector.Distance(capturer.GetPosition(), flagPole.GetPosition());
			if (dist > captureZoneR)
			{
				m_ActiveCaptures.Remove(uid);
				SendCaptureCancel(capturer, "You left the capture zone!");
				continue;
			}
			
			// Count attackers (same group, in radius) and defenders (owner group, in radius)
			GPDA_GroupData attackerGroup = GetPlayerGroup(uid);
			if (!attackerGroup)
			{
				m_ActiveCaptures.Remove(uid);
				SendCaptureCancel(capturer, "You are no longer in a group!");
				continue;
			}
			
			string ownerGroupName = "";
			if (fpData) ownerGroupName = fpData.OwnerGroup;
			
			int attackers = 0;
			int defenders = 0;
			
			foreach (Man m : allPlayers)
			{
				PlayerBase p = PlayerBase.Cast(m);
				if (!p || !p.GetIdentity() || !p.IsAlive()) continue;
				
				float pDist = vector.Distance(p.GetPosition(), flagPole.GetPosition());
				if (pDist > captureZoneR) continue;
				
				string pUID = p.GetIdentity().GetPlainId();
				GPDA_GroupData pGroup = GetPlayerGroup(pUID);
				if (!pGroup) continue;
				
				if (pGroup.Name == attackerGroup.Name)
					attackers++;
				else if (ownerGroupName != "" && pGroup.Name == ownerGroupName)
					defenders++;
			}
			
			// Calculate speed modifier:
			// Base speed = 1.0 (100% in 60 seconds)
			// Each extra attacker adds 0.3x
			// Each defender reduces by 0.5x (min speed = 0.1)
			float speedMod = 1.0 + (attackers - 1) * 0.3 - defenders * 0.5;
			if (speedMod < 0.1) speedMod = 0.1;
			if (speedMod > 3.0) speedMod = 3.0;
			
			// Progress increment per second = (100 / CAPTURE_TIME) * speedMod
			float increment = (100.0 / CAPTURE_TIME) * speedMod;
			capture.Progress += increment;
			capture.ElapsedTime += 1.0;
			
			float timeLeft = 0;
			if (increment > 0)
				timeLeft = (100.0 - capture.Progress) / increment;
			
			if (capture.Progress >= 100.0)
			{
				// CAPTURE COMPLETE!
				capture.Progress = 100.0;
				CompleteCapture(capturer, capture, attackerGroup, flagPole);
			}
			else
			{
				// Animate flag rising: phase goes from 1.0 (bottom) to 0.0 (top) as progress 0->100
				// Vanilla ref: totem.c AnimateFlag(1 - GetRefresherTime01())
				float flagPhase = 1.0 - (capture.Progress / 100.0);
				flagPole.AnimateFlag(flagPhase);
				
				// Send progress update
				SendCaptureProgress(capturer, capture, attackers, defenders, speedMod, timeLeft);
			}
		}
	}
	
	// ─── COMPLETE CAPTURE ─────────────────────────────────────────
	void CompleteCapture(PlayerBase capturer, GPDA_ActiveCapture capture, GPDA_GroupData attackerGroup, TerritoryFlag flagPole)
	{
		string uid = capturer.GetIdentity().GetPlainId();
		string zoneName = capture.ZoneName;  // Admin zone name!
		string flagClass = capture.FlagClass;
		
		Print("[GPDA][Capture] COMPLETE! Group=" + attackerGroup.Name + " zone=" + zoneName + " flag=" + flagClass);
		
		// 1. Update ownership data (use zone name, not posHash!)
		GPDA_FlagPoleData fpData = GetFlagPoleData(zoneName);
		if (!fpData)
		{
			// This shouldn't happen — zone should exist from HandleCaptureStart
			Print("[GPDA][Capture] ERROR: Zone " + zoneName + " not found!");
			SendCaptureCancel(capturer, "ERROR: Zone no longer exists!");
			m_ActiveCaptures.Remove(uid);
			return;
		}
		
		string oldOwner = fpData.OwnerGroup;
		fpData.OwnerGroup = attackerGroup.Name;
		fpData.FlagClass = flagClass;
		SaveToFile();
		
		// 2. Flag is already on pole (attached via ATTACH_FLAG). Raise it fully.
		// Vanilla ref: AnimateFlag(0) = top, AddRefresherTime01(1) = prevent decay
		flagPole.AnimateFlag(0.0);
		flagPole.AddRefresherTime01(1.0);
		
		// 3. Award points
		if (g_GPDA_GroupManager)
			g_GPDA_GroupManager.AddScore(attackerGroup.Name, fpData.RewardPoints);
		
		// 4. Notify capturer
		Param1<string> completeData = new Param1<string>(fpData.RewardPoints.ToString());
		GetGame().RPCSingleParam(capturer, GPDA_RPC.CAPTURE_COMPLETE, completeData, true, capturer.GetIdentity());
		
		// 5. Notify all online players
		string notifyMsg = "Zone " + zoneName + " captured by " + attackerGroup.Name + " (+" + fpData.RewardPoints.ToString() + " pts)";
		array<Man> allPlayers = new array<Man>;
		GetGame().GetPlayers(allPlayers);
		foreach (Man man : allPlayers)
		{
			PlayerBase p = PlayerBase.Cast(man);
			if (p && p.GetIdentity())
			{
				NotificationSystem.SendNotificationToPlayerIdentityExtended(
					p.GetIdentity(), 10.0, "Zone Captured", notifyMsg, ICON_CONNECT);
			}
		}
		
		// 6. Remove active capture
		m_ActiveCaptures.Remove(uid);
		
		Print("[GPDA][Capture] " + attackerGroup.Name + " captured zone " + zoneName + " (was: " + oldOwner + ")");
	}
	
	// ─── ATTACH FLAG TO FLAG POLE ─────────────────────────────────
	// Vanilla ref: TerritoryFlag has slot "Material_FPole_Flag"
	// Vanilla ref: AnimateFlag(0) = raise flag (top), AnimateFlag(1) = lower (bottom)
	// Vanilla ref: Flag_Base.OnWasAttached() switches visual folded->unfolded
	// Vanilla ref: GameInventory.LocationSyncMoveEntity(src, dst) for server-side moves
	// phase: 0.0 = raised/top, 1.0 = lowered/bottom
	void AttachFlagToFlagPole(PlayerBase player, TerritoryFlag flagPole, string flagClass, float phase)
	{
		int flagSlotId = InventorySlots.GetSlotIdFromString("Material_FPole_Flag");
		
		// 1. Remove existing flag from pole (if any)
		EntityAI existingFlag = flagPole.GetInventory().FindAttachment(flagSlotId);
		if (existingFlag)
		{
			flagPole.AnimateFlag(1.0);
			// Unlock slot first so we can remove
			flagPole.GetInventory().SetSlotLock(flagSlotId, false);
			GetGame().ObjectDelete(existingFlag);
			Print("[GPDA][Capture] Removed old flag from pole");
		}
		
		// 2. Find the ACTUAL flag entity in player inventory
		EntityAI flagItem = null;
		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI item : items)
		{
			if (item && item.GetType() == flagClass)
			{
				flagItem = item;
				break;
			}
		}
		
		if (!flagItem)
		{
			Print("[GPDA][Capture] Flag " + flagClass + " not found in player inventory");
			return;
		}
		
		// 3. MOVE the flag entity from player to pole attachment slot
		// Vanilla ref: inventory.c LocationSyncMoveEntity() — server-side sync move
		// This preserves the original item + triggers Flag_Base.OnWasAttached()
		// which calls HideSelection("folded") + ShowSelection("unfolded") for correct visual
		InventoryLocation src = new InventoryLocation;
		if (!flagItem.GetInventory().GetCurrentInventoryLocation(src))
		{
			Print("[GPDA][Capture] Cannot get source location for flag");
			return;
		}
		
		InventoryLocation dst = new InventoryLocation;
		dst.SetAttachment(flagPole, flagItem, flagSlotId);
		
		bool moved = GameInventory.LocationSyncMoveEntity(src, dst);
		if (moved)
		{
			Print("[GPDA][Capture] Moved " + flagClass + " to pole via LocationSyncMoveEntity");
		}
		else
		{
			// Fallback: delete from player + create on pole
			Print("[GPDA][Capture] LocationSyncMoveEntity failed, using fallback...");
			GetGame().ObjectDelete(flagItem);
			EntityAI newFlag = flagPole.GetInventory().CreateAttachment(flagClass);
			if (newFlag)
			{
				Print("[GPDA][Capture] Fallback CreateAttachment OK: " + flagClass);
			}
			else
			{
				Print("[GPDA][Capture] FATAL: Cannot attach " + flagClass + " to pole!");
				return;
			}
		}
		
		// 4. Set flag position and lock slot
		flagPole.AnimateFlag(phase);
		flagPole.AddRefresherTime01(1.0);
		// Lock the flag slot so nobody can remove it via TAB
		// Vanilla ref: totem.c SetSlotLock — phase != 1 locks slot
		flagPole.GetInventory().SetSlotLock(flagSlotId, true);
		flagPole.SetSynchDirty();
		
		Print("[GPDA][Capture] Flag attached phase=" + phase.ToString() + " slot LOCKED");
	}
	
	// ─── SEND HELPERS ─────────────────────────────────────────────
	void SendCaptureProgress(PlayerBase player, GPDA_ActiveCapture capture, int attackers, int defenders, float speedMod, float timeLeft)
	{
		// Format: "progress,attackers,defenders,speedmod,timeleft"
		string payload = capture.Progress.ToString() + "," + attackers.ToString() + "," + defenders.ToString() + "," + speedMod.ToString() + "," + timeLeft.ToString();
		
		Param1<string> data = new Param1<string>(payload);
		GetGame().RPCSingleParam(player, GPDA_RPC.CAPTURE_PROGRESS, data, true, player.GetIdentity());
	}
	
	void SendCaptureCancel(PlayerBase player, string reason)
	{
		Param1<string> cancelData = new Param1<string>(reason);
		GetGame().RPCSingleParam(player, GPDA_RPC.CAPTURE_CANCEL, cancelData, true, player.GetIdentity());
		Print("[GPDA][Capture] Cancel sent: " + reason);
	}
	
	// ─── GROUP HELPER ─────────────────────────────────────────────
	GPDA_GroupData GetPlayerGroup(string uid)
	{
		if (g_GPDA_GroupManager)
			return g_GPDA_GroupManager.GetPlayerGroup(uid);
		return null;
	}
	
	// ─── ADMIN FUNCTIONS (kept for compatibility) ─────────────────
	
	// Admin functions for CaptureRPC.c (creates FlagPoleData from admin panel)
	bool AddCapturePoint(string name, vector pos, float radius, int points, string ownerGroup = "", string requesterUID = "")
	{
		// Check duplicate
		foreach (GPDA_FlagPoleData ex : m_Config.FlagPoles)
		{
			if (ex && ex.PosHash == name)
			{
				Print("[GPDA][Capture] AddCapturePoint duplicate: " + name);
				return false;
			}
		}
		
		GPDA_FlagPoleData fpd = new GPDA_FlagPoleData();
		fpd.PosHash = name;
		fpd.PosX = pos[0];
		fpd.PosZ = pos[2];
		fpd.RewardPoints = points;
		fpd.Radius = radius;
		if (fpd.Radius <= 0) fpd.Radius = CAPTURE_RADIUS;
		string ow = ownerGroup;
		ow.Trim();
		if (ow == "None" || ow == "none") ow = "";
		fpd.OwnerGroup = ow;
		m_Config.FlagPoles.Insert(fpd);
		SaveToFile();
		Print("[GPDA][Capture] AddCapturePoint OK name=" + name + " pos=" + pos.ToString());
		return true;
	}
	
	bool RemoveCapturePoint(string name)
	{
		for (int i = m_Config.FlagPoles.Count() - 1; i >= 0; i--)
		{
			if (m_Config.FlagPoles[i] && m_Config.FlagPoles[i].PosHash == name)
			{
				m_Config.FlagPoles.Remove(i);
				SaveToFile();
				Print("[GPDA][Capture] RemoveCapturePoint OK name=" + name);
				return true;
			}
		}
		Print("[GPDA][Capture] RemoveCapturePoint not found: " + name);
		return false;
	}
	
	bool UpdateCapturePoint(string originalName, string newName, vector pos, float radius, int points, string ownerGroup)
	{
		for (int i = 0; i < m_Config.FlagPoles.Count(); i++)
		{
			if (m_Config.FlagPoles[i] && m_Config.FlagPoles[i].PosHash == originalName)
			{
				GPDA_FlagPoleData fpd = m_Config.FlagPoles[i];
				fpd.PosHash = newName;
				fpd.PosX = pos[0];
				fpd.PosZ = pos[2];
				fpd.Radius = radius;
				if (fpd.Radius <= 0) fpd.Radius = CAPTURE_RADIUS;
				fpd.RewardPoints = points;
				string ow = ownerGroup;
				ow.Trim();
				if (ow == "None" || ow == "none") ow = "";
				fpd.OwnerGroup = ow;
				SaveToFile();
				Print("[GPDA][Capture] UpdateCapturePoint OK name=" + originalName + " -> " + newName);
				return true;
			}
		}
		Print("[GPDA][Capture] UpdateCapturePoint not found: " + originalName);
		return false;
	}
	
	array<ref GPDA_CapturePoint> GetAllZones()
	{
		// Convert flag pole data to old format for compatibility
		array<ref GPDA_CapturePoint> result = new array<ref GPDA_CapturePoint>;
		foreach (GPDA_FlagPoleData fpd : m_Config.FlagPoles)
		{
			if (!fpd) continue;
			GPDA_CapturePoint cp = new GPDA_CapturePoint();
			cp.Name = fpd.PosHash;
			cp.Position = Vector(fpd.PosX, 0, fpd.PosZ);
			if (fpd.Radius > 0)
				cp.Radius = fpd.Radius;
			else
				cp.Radius = CAPTURE_RADIUS;
			cp.OwnerGroup = fpd.OwnerGroup;
			cp.RewardPoints = fpd.RewardPoints;
			cp.IsActive = true;
			result.Insert(cp);
		}
		return result;
	}
	
	void SendCapturePointsToPlayer(PlayerBase player)
	{
		if (!player || !player.GetIdentity()) return;
		
		string pointsStr = "";
		foreach (GPDA_FlagPoleData fpd : m_Config.FlagPoles)
		{
			if (!fpd) continue;
			if (pointsStr != "") pointsStr += ";";
			string ownerStr = fpd.OwnerGroup;
			if (ownerStr == "") ownerStr = "None";
			float sendRadius = CAPTURE_RADIUS;
			if (fpd.Radius > 0) sendRadius = fpd.Radius;
			pointsStr += fpd.PosHash + "|" + fpd.PosX.ToString() + "|0|" + fpd.PosZ.ToString() + "|" + sendRadius.ToString() + "|" + ownerStr + "|" + fpd.RewardPoints.ToString();
		}
		
		Param1<string> sendData = new Param1<string>(pointsStr);
		GetRPCManager().SendRPC("GPDA_Capture", "ClientReceivePoints", sendData, true, player.GetIdentity(), player);
	}
}

// Keep old class for backward compatibility (compile safety)
class GPDA_CapturePoint
{
	string ZoneId;
	string Name;
	vector Position;
	float Radius;
	string OwnerGroup;
	int RewardPoints;
	bool IsActive;
	
	void GPDA_CapturePoint()
	{
		ZoneId = "";
		Name = "";
		Position = "0 0 0";
		Radius = 100;
		OwnerGroup = "";
		RewardPoints = 100;
		IsActive = true;
	}
}

class GPDA_CaptureConfig
{
	ref array<ref GPDA_CapturePoint> Zones;
	
	void GPDA_CaptureConfig()
	{
		Zones = new array<ref GPDA_CapturePoint>;
	}
}

// Global instance
static ref GPDA_CaptureManager g_GPDA_CaptureManager;
