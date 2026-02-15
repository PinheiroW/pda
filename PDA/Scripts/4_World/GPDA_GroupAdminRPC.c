// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA - GROUP ADMIN RPC HANDLER (Server)
// Handles RPC from Group Admin UI, delegates to GroupManager
// Follows MutantSpawnerHandler pattern exactly
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_GroupAdminRPC
{
	private static ref GPDA_GroupAdminRPC s_Instance;
	
	static GPDA_GroupAdminRPC GetInstance()
	{
		if (!s_Instance)
			s_Instance = new GPDA_GroupAdminRPC();
		return s_Instance;
	}
	
	void Init()
	{
		if (!GetGame().IsServer()) return;
		
		GetDayZGame().Event_OnRPC.Insert(OnRPC);
		Print("[GPDA] GroupAdminRPC initialized");
	}
	
	void ~GPDA_GroupAdminRPC()
	{
		GetDayZGame().Event_OnRPC.Remove(OnRPC);
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// ADMIN CHECK (same as MutantSpawnerHandler)
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
			case GPDA_RPC.GRP_REQUEST_ALL:
				HandleRequestAll(sender, ctx);
				break;
			case GPDA_RPC.GRP_SAVE_GROUP:
				HandleSaveGroup(sender, ctx);
				break;
			case GPDA_RPC.GRP_DELETE_GROUP:
				HandleDeleteGroup(sender, ctx);
				break;
			case GPDA_RPC.GRP_REQUEST_PLAYERS:
				HandleRequestPlayers(sender, ctx);
				break;
		}
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// SEND ALL GROUPS
	// ══════════════════════════════════════════════════════════════════════
	
	void HandleRequestAll(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender))
		{
			SendStatus(sender, "ACCESS_DENIED");
			return;
		}
		
		GPDA_GroupManager mgr = GPDA_GroupManager.GetInstance();
		array<ref GPDA_GroupData> groups = mgr.GetAllGroups();
		
		int cnt = 0;
		if (groups) cnt = groups.Count();
		
		// Send count header first
		ref Param1<int> countData = new Param1<int>(cnt);
		GetGame().RPCSingleParam(null, GPDA_RPC.GRP_RECEIVE_ALL, countData, true, sender);
		
		// Send each group individually (avoids String CORRUPTED on large payloads)
		if (groups)
		{
			for (int i = 0; i < groups.Count(); i++)
			{
				string jsonStr;
				JsonSerializer().WriteToString(groups[i], false, jsonStr);
				ref Param1<string> groupData = new Param1<string>(jsonStr);
				GetGame().RPCSingleParam(null, GPDA_RPC.GRP_RECEIVE_ONE, groupData, true, sender);
			}
		}
		
		Print("[GPDA] GroupAdmin: Sent " + cnt.ToString() + " groups to " + sender.GetName());
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// SAVE GROUP (create or update)
	// ══════════════════════════════════════════════════════════════════════
	
	void HandleSaveGroup(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender))
		{
			SendStatus(sender, "ACCESS_DENIED");
			return;
		}
		
		ref Param2<string, string> params = new Param2<string, string>("", "");
		if (!ctx.Read(params)) return;
		
		string originalName = params.param1;
		string jsonData = params.param2;
		
		string jsonError;
		ref GPDA_GroupData incoming;
		if (!JsonSerializer().ReadFromString(incoming, jsonData, jsonError))
		{
			SendStatus(sender, "Error: Failed to parse group! " + jsonError);
			return;
		}
		
		if (!incoming || incoming.Name == "")
		{
			SendStatus(sender, "Error: Group data is null or empty name!");
			return;
		}
		
		GPDA_GroupManager mgr = GPDA_GroupManager.GetInstance();
		array<ref GPDA_GroupData> groups = mgr.GetAllGroups();
		if (!groups) return;
		
		// Check if renamed → delete old file
		if (originalName != "" && incoming.Name != originalName)
		{
			DeleteGroupFile(originalName);
			Print("[GPDA] GroupAdmin: Renamed '" + originalName + "' -> '" + incoming.Name + "'");
		}
		
		// Find existing or add new
		bool found = false;
		for (int i = 0; i < groups.Count(); i++)
		{
			if (groups[i].Name == originalName || groups[i].Name == incoming.Name)
			{
				groups[i] = incoming;
				found = true;
				break;
			}
		}
		
		if (!found)
			groups.Insert(incoming);
		
		mgr.Save();
		
		SendStatus(sender, "Group saved: " + incoming.Name);
		Print("[GPDA] GroupAdmin: Group '" + incoming.Name + "' saved by " + sender.GetName());
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// DELETE GROUP
	// ══════════════════════════════════════════════════════════════════════
	
	void HandleDeleteGroup(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender))
		{
			SendStatus(sender, "ACCESS_DENIED");
			return;
		}
		
		ref Param1<string> params = new Param1<string>("");
		if (!ctx.Read(params)) return;
		
		string groupName = params.param1;
		
		GPDA_GroupManager mgr = GPDA_GroupManager.GetInstance();
		array<ref GPDA_GroupData> groups = mgr.GetAllGroups();
		if (groups)
		{
			for (int i = 0; i < groups.Count(); i++)
			{
				if (groups[i].Name == groupName)
				{
					groups.Remove(i);
					break;
				}
			}
		}
		
		DeleteGroupFile(groupName);
		
		SendStatus(sender, "Group deleted: " + groupName);
		Print("[GPDA] GroupAdmin: Group '" + groupName + "' deleted by " + sender.GetName());
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// FILE OPERATIONS
	// ══════════════════════════════════════════════════════════════════════
	
	private void DeleteGroupFile(string groupName)
	{
		string safeName = groupName;
		safeName.Replace(" ", "_");
		safeName.Replace("/", "_");
		safeName.Replace("\\", "_");
		safeName.Replace("<", "_");
		safeName.Replace(">", "_");
		safeName.Replace(":", "_");
		safeName.Replace("\"", "_");
		safeName.Replace("|", "_");
		safeName.Replace("?", "_");
		safeName.Replace("*", "_");
		
		string fullPath = "$profile:Greshnik_PDALITE/groups/" + safeName + ".json";
		if (FileExist(fullPath))
		{
			DeleteFile(fullPath);
			Print("[GPDA] GroupAdmin: Deleted file '" + fullPath + "'");
		}
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// SEND ONLINE PLAYERS LIST
	// Vanilla: GetGame().GetPlayers() + PlayerBase.GetIdentity()
	// ══════════════════════════════════════════════════════════════════════
	
	void HandleRequestPlayers(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!IsAdmin(sender))
		{
			SendStatus(sender, "ACCESS_DENIED");
			return;
		}
		
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		string result = "";
		int count = 0;
		
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = PlayerBase.Cast(players[i]);
			if (!player || !player.GetIdentity()) continue;
			
			string name = player.GetIdentity().GetName();
			string steam64 = player.GetIdentity().GetPlainId();
			
			if (count > 0) result = result + "|";
			result = result + name + ":" + steam64;
			count++;
		}
		
		ref Param1<string> data = new Param1<string>(result);
		GetGame().RPCSingleParam(null, GPDA_RPC.GRP_RECEIVE_PLAYERS, data, true, sender);
		
		Print("[GPDA] GroupAdmin: Sent " + count.ToString() + " online players to " + sender.GetName());
	}
	
	// ══════════════════════════════════════════════════════════════════════
	// HELPER
	// ══════════════════════════════════════════════════════════════════════
	
	void SendStatus(PlayerIdentity identity, string msg)
	{
		if (!identity) return;
		ref Param1<string> data = new Param1<string>(msg);
		GetGame().RPCSingleParam(null, GPDA_RPC.GRP_STATUS, data, true, identity);
	}
}
