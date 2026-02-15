// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Group RPC Handler (Server-side)
// Handles group-related RPC requests
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_GroupRPCHandler
{
	protected static ref GPDA_GroupRPCHandler m_Instance;
	protected bool m_RPCsRegistered;
	
	static GPDA_GroupRPCHandler GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new GPDA_GroupRPCHandler();
		}
		return m_Instance;
	}
	
	void Init()
	{
		if (GetGame() && GetGame().IsServer())
		{
			RegisterRPCs();
		}
	}
	
	protected void RegisterRPCs()
	{
		if (m_RPCsRegistered) return;
		
		RPCManager rpc = GetRPCManager();
		if (!rpc) return;
		
		rpc.AddRPC("GPDA_Groups", "RequestGroupData", this, SingleplayerExecutionType.Server);
		rpc.AddRPC("GPDA_Groups", "RequestAddMember", this, SingleplayerExecutionType.Server);
		rpc.AddRPC("GPDA_Groups", "RequestKickMember", this, SingleplayerExecutionType.Server);
		rpc.AddRPC("GPDA_Groups", "CheckPlayerGroup", this, SingleplayerExecutionType.Server);
		rpc.AddRPC("GPDA_Groups", "RequestAllGroups", this, SingleplayerExecutionType.Server);
		rpc.AddRPC("GPDA_Groups", "AddSpawnPoint", this, SingleplayerExecutionType.Server);
		rpc.AddRPC("GPDA_Groups", "RemoveSpawnPoint", this, SingleplayerExecutionType.Server);
		rpc.AddRPC("GPDA_Groups", "RequestCapturedZones", this, SingleplayerExecutionType.Server);
		
		m_RPCsRegistered = true;
		Print("[GPDA] Group RPCs registered");
	}
	
	void RequestCapturedZones(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity()) return;
		
		string uid = sender.GetPlainId();
		string groupName = "";
		if (g_GPDA_GroupManager)
		{
			GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(uid);
			if (group)
				groupName = group.Name;
		}
		
		string zonesStr = "";
		if (groupName != "" && g_GPDA_CaptureManager)
		{
			array<string> zones = new array<string>;
			array<ref GPDA_CapturePoint> cps = g_GPDA_CaptureManager.GetAllZones();
			if (cps)
			{
				foreach (GPDA_CapturePoint cp : cps)
				{
					if (!cp) continue;
					if (cp.OwnerGroup == groupName)
						zones.Insert(cp.Name);
				}
			}
			foreach (string z : zones)
			{
				if (zonesStr != "") zonesStr += ";";
				zonesStr += z;
			}
		}
		
		ref Param1<string> resp = new Param1<string>(zonesStr);
		GetRPCManager().SendRPC("GPDA_Groups", "ReceiveCapturedZones", resp, true, sender, player);
	}
	
	// Check if player is in a group and send response
	void CheckPlayerGroup(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity()) return;
		
		string steam64 = data.param1;
		GPDA_GroupData group = GPDA_GroupManager.GetInstance().GetPlayerGroup(steam64);
		
		bool hasGroup = (group != null);
		string groupName = "";
		if (hasGroup)
			groupName = group.Name;
		
		// Send response to client
		ref Param2<bool, string> response = new Param2<bool, string>(hasGroup, groupName);
		GetRPCManager().SendRPC("GPDA_Groups", "GroupCheckResponse", response, true, sender, player);
	}
	
	// Auto-create Monolit group for testing if it doesn't exist
	protected void EnsureTestGroup(string playerSteam64)
	{
		GPDA_GroupManager gm = GPDA_GroupManager.GetInstance();
		
		// Check if Monolit exists
		GPDA_GroupData monolit = null;
		array<ref GPDA_GroupData> allGroups = gm.GetAllGroups();
		if (allGroups)
		{
			foreach (GPDA_GroupData g : allGroups)
			{
				if (g && g.Name == "Monolit")
				{
					monolit = g;
					break;
				}
			}
		}
		
		// Create Monolit if not exists
		if (!monolit)
		{
			gm.CreateGroup("Monolit", playerSteam64);
			Print("[GPDA Test] Created Monolit group with leader: " + playerSteam64);
		}
		else
		{
			// Add player to existing Monolit
			if (!monolit.HasMember(playerSteam64))
			{
				monolit.AddMember(playerSteam64);
				gm.Save();
				Print("[GPDA Test] Added player to Monolit: " + playerSteam64);
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// RPC HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void RequestAllGroups(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) 
		{
			Print("[GPDA] RequestAllGroups: Wrong call type or no sender");
			return;
		}
		
		// Try multiple methods to get player
		PlayerBase player = PlayerBase.Cast(sender.GetPlayer());
		if (!player)
		{
			player = GetPlayerByUID(sender.GetPlainId());
		}
		if (!player)
		{
			player = PlayerBase.Cast(target);
		}
		
		if (!player) 
		{
			Print("[GPDA] RequestAllGroups: Player not found for " + sender.GetName() + " (UID: " + sender.GetPlainId() + ")");
			return;
		}
		
		Print("[GPDA] RequestAllGroups called by " + sender.GetName());
		
		GPDA_GroupManager gm = GPDA_GroupManager.GetInstance();
		if (!gm) 
		{
			Print("[GPDA] RequestAllGroups: GroupManager not found");
			return;
		}
		
		// Get all group names
		array<string> groupNames = gm.GetAllGroupNames();
		Print("[GPDA] RequestAllGroups: Found " + groupNames.Count().ToString() + " groups");
		
		// Build string with all group names
		string groupsStr = "";
		foreach (string name : groupNames)
		{
			if (groupsStr != "") groupsStr += ";";
			groupsStr += name;
		}
		
		Print("[GPDA] RequestAllGroups: Sending groups string: " + groupsStr);
		ref Param1<string> data = new Param1<string>(groupsStr);
		GetRPCManager().SendRPC("GPDA_Groups", "ReceiveAllGroups", data, true, sender, player);
		Print("[GPDA] RequestAllGroups: RPC sent to client");
	}
	
	void RequestGroupData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity()) return;
		
		SendGroupDataToPlayer(player);
	}
	
	void RequestAddMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity()) return;
		
		HandleAddMember(player, data.param1);
	}
	
	void RequestKickMember(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity()) return;
		
		HandleKickMember(player, data.param1);
	}
	
	// Add spawn point to group
	void AddSpawnPoint(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		
		Param2<string, vector> data;
		if (!ctx.Read(data)) return;
		
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity()) return;
		
		string spawnName = data.param1;
		vector spawnPos = data.param2;
		string callerID = sender.GetPlainId();
		
		GPDA_GroupData group = GPDA_GroupManager.GetInstance().GetPlayerGroup(callerID);
		if (!group)
		{
			SendNotification(player, "You are not in a group.", "Warning");
			return;
		}
		
		// Only leader/deputy can add spawns
		if (!group.IsLeaderOrDeputy(callerID))
		{
			SendNotification(player, "Only leader or deputy can add spawn points.", "Warning");
			return;
		}
		
		bool success = GPDA_GroupManager.GetInstance().AddGroupSpawnPoint(group.Name, spawnName, spawnPos);
		if (success)
		{
			SendNotification(player, "Spawn point '" + spawnName + "' added!", "Success");
		}
		else
		{
			SendNotification(player, "Failed to add spawn point.", "Error");
		}
		
		SendGroupDataToPlayer(player);
	}
	
	// Remove spawn point from group
	void RemoveSpawnPoint(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		
		Param1<int> data;
		if (!ctx.Read(data)) return;
		
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity()) return;
		
		int spawnIndex = data.param1;
		string callerID = sender.GetPlainId();
		
		GPDA_GroupData group = GPDA_GroupManager.GetInstance().GetPlayerGroup(callerID);
		if (!group)
		{
			SendNotification(player, "You are not in a group.", "Warning");
			return;
		}
		
		// Only leader/deputy can remove spawns
		if (!group.IsLeaderOrDeputy(callerID))
		{
			SendNotification(player, "Only leader or deputy can remove spawn points.", "Warning");
			return;
		}
		
		bool success = GPDA_GroupManager.GetInstance().RemoveGroupSpawnPoint(group.Name, spawnIndex);
		if (success)
		{
			SendNotification(player, "Spawn point removed!", "Success");
		}
		else
		{
			SendNotification(player, "Failed to remove spawn point.", "Error");
		}
		
		SendGroupDataToPlayer(player);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// LOGIC
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void HandleAddMember(PlayerBase caller, string steam64)
	{
		string cleanId = NormalizeId(steam64);
		if (cleanId == "")
		{
			SendNotification(caller, "Enter a valid Steam64 ID.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		string callerID = caller.GetIdentity().GetPlainId();
		GPDA_GroupData group = GPDA_GroupManager.GetInstance().GetPlayerGroup(callerID);
		
		if (!group)
		{
			SendNotification(caller, "You are not in a group.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Check permissions
		if (!group.IsLeaderOrDeputy(callerID))
		{
			SendNotification(caller, "Only leader or deputy can add members.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Check if already in group
		if (group.HasMember(cleanId))
		{
			SendNotification(caller, "Player already in the group.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Check if in another group
		GPDA_GroupData otherGroup = GPDA_GroupManager.GetInstance().GetPlayerGroup(cleanId);
		if (otherGroup)
		{
			SendNotification(caller, "Player is already in another group.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Add member
		group.AddMember(cleanId);
		GPDA_GroupManager.GetInstance().Save();
		
		SendNotification(caller, "Member added: " + cleanId, "Success");
		BroadcastGroupData(group);
	}
	
	protected void HandleKickMember(PlayerBase caller, string steam64)
	{
		string cleanId = NormalizeId(steam64);
		if (cleanId == "")
		{
			SendNotification(caller, "Enter a valid Steam64 ID.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		string callerID = caller.GetIdentity().GetPlainId();
		GPDA_GroupData group = GPDA_GroupManager.GetInstance().GetPlayerGroup(callerID);
		
		if (!group)
		{
			SendNotification(caller, "You are not in a group.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Check permissions
		if (!group.IsLeaderOrDeputy(callerID))
		{
			SendNotification(caller, "Only leader or deputy can kick members.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Cannot kick leader
		if (cleanId == group.LeaderID)
		{
			SendNotification(caller, "Cannot kick the leader.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Check if member exists
		if (!group.HasMember(cleanId))
		{
			SendNotification(caller, "Player not found in group.", "Warning");
			SendGroupDataToPlayer(caller);
			return;
		}
		
		// Remove member
		group.RemoveMember(cleanId);
		GPDA_GroupManager.GetInstance().Save();
		
		SendNotification(caller, "Member removed: " + cleanId, "Info");
		BroadcastGroupData(group);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// DATA SENDING
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void SendGroupDataToPlayer(PlayerBase player)
	{
		if (!player || !player.GetIdentity()) return;
		
		string playerID = player.GetIdentity().GetPlainId();
		GPDA_GroupData group = GPDA_GroupManager.GetInstance().GetPlayerGroup(playerID);
		
		// Build member list
		ref array<ref Param3<string, string, bool>> members = new array<ref Param3<string, string, bool>>;
		
		string groupName = "No Group";
		int groupScore = 0;
		
		if (group)
		{
			groupName = group.Name;
			groupScore = group.Score;
			
			if (group.Members)
			{
				foreach (string memberId : group.Members)
				{
					string displayName = GPDA_GroupManager.GetInstance().GetPlayerName(memberId);
					bool isOnline = GPDA_GroupManager.GetInstance().IsPlayerOnline(memberId);
					
					members.Insert(new Param3<string, string, bool>(displayName, memberId, isOnline));
				}
			}
		}
		
		// Send RPC
		Param3<string, int, ref array<ref Param3<string, string, bool>>> payload;
		payload = new Param3<string, int, ref array<ref Param3<string, string, bool>>>(groupName, groupScore, members);
		
		GetRPCManager().SendRPC("GPDA_Groups", "ClientReceiveGroupData", payload, true, player.GetIdentity(), player);
	}
	
	protected void BroadcastGroupData(GPDA_GroupData group)
	{
		if (!group) return;
		
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (!player || !player.GetIdentity()) continue;
			
			string playerID = player.GetIdentity().GetPlainId();
			if (group.HasMember(playerID))
			{
				SendGroupDataToPlayer(player);
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// HELPERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected PlayerBase GetPlayerByUID(string uid)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity() && player.GetIdentity().GetPlainId() == uid)
				return player;
		}
		return null;
	}
	
	protected string NormalizeId(string value)
	{
		if (!value) return "";
		value.Trim();
		return value;
	}
	
	protected void SendNotification(PlayerBase player, string text, string style = "Info")
	{
		if (!player || !player.GetIdentity()) return;
		
		// Simple print notification - player sees via chat/log
		Print("[GPDA] " + style + ": " + text);
	}
}

// Global instance
static ref GPDA_GroupRPCHandler g_GPDA_GroupRPC;

