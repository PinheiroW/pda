// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Server Manager
// Handles all server-side logic: stats, chat, messages, markers
// Data stored in: profiles/Greshnik_PDA/
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_ServerManager
{
	private static ref GPDA_ServerManager s_Instance;
	
	// Data storage (markers removed)
	protected ref GPDA_AllPlayersData m_PlayersData;
	protected ref GPDA_ChatHistory m_ChatHistory;
	protected ref map<string, ref array<ref GPDA_PrivateMessage>> m_PrivateMessages; // Key = recipientID
	protected int m_LastChatRotation;
	
	// Paths (markers removed)
	protected string m_DataPath;
	protected string m_PlayersFile;
	protected string m_ChatFile;
	protected string m_PMFolder;
	protected string m_ChatArchiveFolder;
	
	// Chat limit
	protected const int MAX_CHAT_MESSAGES = 100;
	
	// ═══════════════════════════════════════════════════════════════════════════
	// SINGLETON
	// ═══════════════════════════════════════════════════════════════════════════
	
	static GPDA_ServerManager GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new GPDA_ServerManager();
		}
		return s_Instance;
	}
	
	void GPDA_ServerManager()
	{
		if (!GetGame().IsServer()) return;
		
		m_PlayersData = new GPDA_AllPlayersData();
		m_ChatHistory = new GPDA_ChatHistory();
		m_PrivateMessages = new map<string, ref array<ref GPDA_PrivateMessage>>;
		m_LastChatRotation = 0;
		
		// Setup paths for profile JSON
		m_DataPath = "$profile:Greshnik_PDALITE/";
		m_PlayersFile = m_DataPath + "players.json";
		m_ChatFile = m_DataPath + "chat.json";
		m_PMFolder = m_DataPath + "pm/";
		m_ChatArchiveFolder = m_DataPath + "chat/";
		
		// Create folders
		if (!FileExist(m_DataPath))
			MakeDirectory(m_DataPath);
		if (!FileExist(m_PMFolder))
			MakeDirectory(m_PMFolder);
		if (!FileExist(m_ChatArchiveFolder))
			MakeDirectory(m_ChatArchiveFolder);
		
		// Load data
		LoadAllData();
		
		// Register RPC handlers
		GetDayZGame().Event_OnRPC.Insert(OnRPC);
		
		Print("[GPDA] Server Manager initialized");
	}
	
	void ~GPDA_ServerManager()
	{
		SaveAllData();
		GetDayZGame().Event_OnRPC.Remove(OnRPC);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// DATA LOADING/SAVING
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void LoadAllData()
	{
		// Load players
		if (FileExist(m_PlayersFile))
		{
			JsonFileLoader<GPDA_AllPlayersData>.JsonLoadFile(m_PlayersFile, m_PlayersData);
		}
		if (!m_PlayersData) m_PlayersData = new GPDA_AllPlayersData();
		if (!m_PlayersData.Players) m_PlayersData.Players = new array<ref GPDA_PlayerStats>;
		
		// Load chat
		if (FileExist(m_ChatFile))
		{
			JsonFileLoader<GPDA_ChatHistory>.JsonLoadFile(m_ChatFile, m_ChatHistory);
		}
		if (!m_ChatHistory) m_ChatHistory = new GPDA_ChatHistory();
		if (!m_ChatHistory.Messages) m_ChatHistory.Messages = new array<ref GPDA_ChatMessage>;
		
		Print("[GPDA] Data loaded (profile JSON): " + m_PlayersData.Players.Count() + " players, " + m_ChatHistory.Messages.Count() + " chat messages");
	}
	
	protected void SaveAllData()
	{
		JsonFileLoader<GPDA_AllPlayersData>.JsonSaveFile(m_PlayersFile, m_PlayersData);
		JsonFileLoader<GPDA_ChatHistory>.JsonSaveFile(m_ChatFile, m_ChatHistory);
		Print("[GPDA] All data saved to profile");
	}
	
	void SavePlayersData()
	{
		JsonFileLoader<GPDA_AllPlayersData>.JsonSaveFile(m_PlayersFile, m_PlayersData);
	}
	
	void SaveChatData()
	{
		JsonFileLoader<GPDA_ChatHistory>.JsonSaveFile(m_ChatFile, m_ChatHistory);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// PLAYER STATS
	// ═══════════════════════════════════════════════════════════════════════════
	
	GPDA_PlayerStats GetOrCreatePlayer(string steam64, string nickname)
	{
		// Find existing
		foreach (GPDA_PlayerStats p : m_PlayersData.Players)
		{
			if (p && p.Steam64ID == steam64)
			{
				// Update nickname if changed
				if (nickname != "" && p.Nickname != nickname)
				{
					p.Nickname = nickname;
				}
				return p;
			}
		}
		
		// Create new
		GPDA_PlayerStats newPlayer = new GPDA_PlayerStats();
		newPlayer.Steam64ID = steam64;
		newPlayer.Nickname = nickname;
		m_PlayersData.Players.Insert(newPlayer);
		SavePlayersData();
		return newPlayer;
	}
	
	void AddKill(string steam64, string nickname, int killType)
	{
		GPDA_PlayerStats player = GetOrCreatePlayer(steam64, nickname);
		if (!player) return;
		
		if (killType == 1)
		{
			player.KillsPlayers++;
		}
		else if (killType == 2)
		{
			player.KillsZombies++;
		}
		else if (killType == 3)
		{
			player.KillsAnimals++;
		}
		
		SavePlayersData();
	}
	
	void AddDeath(string steam64, string nickname)
	{
		GPDA_PlayerStats player = GetOrCreatePlayer(steam64, nickname);
		if (player)
		{
			player.Deaths++;
			SavePlayersData();
		}
	}
	
	void AddPlaytime(string steam64, int minutes)
	{
		int i;
		int cnt = m_PlayersData.Players.Count();
		for (i = 0; i < cnt; i++)
		{
			GPDA_PlayerStats p = m_PlayersData.Players[i];
			if (p && p.Steam64ID == steam64)
			{
				p.PlaytimeMinutes = p.PlaytimeMinutes + minutes;
				SavePlayersData();
				return;
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// TOP 100
	// ═══════════════════════════════════════════════════════════════════════════
	
	ref array<ref GPDA_TopEntry> GetTop100(int sortType)
	{
		ref array<ref GPDA_TopEntry> result = new array<ref GPDA_TopEntry>;
		ref array<ref GPDA_PlayerStats> sorted = new array<ref GPDA_PlayerStats>;
		
		// Copy players
		foreach (GPDA_PlayerStats p : m_PlayersData.Players)
		{
			if (p) sorted.Insert(p);
		}
		
		// Sort based on type
		// Simple bubble sort (good enough for 100 entries)
		for (int i = 0; i < sorted.Count() - 1; i++)
		{
			for (int j = 0; j < sorted.Count() - i - 1; j++)
			{
				bool swap = false;
				
				switch (sortType)
				{
					case GPDA_SortType.BY_PLAYER_KILLS:
						swap = sorted[j].KillsPlayers < sorted[j+1].KillsPlayers;
						break;
					case GPDA_SortType.BY_ZOMBIE_KILLS:
						swap = sorted[j].KillsZombies < sorted[j+1].KillsZombies;
						break;
					case GPDA_SortType.BY_TOTAL_KILLS:
						swap = sorted[j].GetTotalKills() < sorted[j+1].GetTotalKills();
						break;
				}
				
				if (swap)
				{
					GPDA_PlayerStats temp = sorted[j];
					sorted[j] = sorted[j+1];
					sorted[j+1] = temp;
				}
			}
		}
		
		// Take top 100
		int count = Math.Min(100, sorted.Count());
		for (int k = 0; k < count; k++)
		{
			GPDA_TopEntry entry = new GPDA_TopEntry();
			entry.Rank = k + 1;
			entry.Nickname = sorted[k].Nickname;
			entry.KillsPlayers = sorted[k].KillsPlayers;
			entry.KillsZombies = sorted[k].KillsZombies;
			entry.TotalKills = sorted[k].GetTotalKills();
			result.Insert(entry);
		}
		
		return result;
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// CHAT
	// ═══════════════════════════════════════════════════════════════════════════
	
	void AddChatMessage(string senderID, string senderName, string message)
	{
		GPDA_ChatMessage msg = new GPDA_ChatMessage();
		msg.SenderID = senderID;
		
		string finalName = senderName;
		string finalMessage = message;
		
		if (finalMessage.Length() >= 5 && finalMessage.Substring(0, 5) == "/anon")
		{
			// Allow /anon and /anon <text>
			finalName = "Anonim";
			finalMessage = finalMessage.Substring(5, finalMessage.Length() - 5);
			finalMessage.Trim();
			if (finalMessage.Length() > 0 && finalMessage.Substring(0, 1) == " ")
			{
				finalMessage = finalMessage.Substring(1, finalMessage.Length() - 1);
				finalMessage.Trim();
			}
		}
		else
		{
			string groupName = "";
			if (g_GPDA_GroupManager)
			{
				GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(senderID);
				if (group)
					groupName = group.Name;
			}
			
			string rankName = "";
			if (g_GPDA_RankManager)
			{
				rankName = g_GPDA_RankManager.GetRankName(senderID);
			}
			
			if (groupName != "" && rankName != "")
				finalName = finalName + " [" + rankName + "|" + groupName + "]";
			else if (groupName != "")
				finalName = finalName + " [" + groupName + "]";
			else if (rankName != "")
				finalName = finalName + " [" + rankName + "]";
		}
		
		msg.SenderName = finalName;
		msg.Message = finalMessage;
		msg.Timestamp = GetGame().GetTime();
		
		m_ChatHistory.Messages.Insert(msg);
		
		// Limit history
		while (m_ChatHistory.Messages.Count() > MAX_CHAT_MESSAGES)
		{
			m_ChatHistory.Messages.Remove(0);
		}
		
		MaybeRotateChat();
		SaveChatData();
		
		// Broadcast to all players
		BroadcastChatMessage(msg);
	}
	
	protected void MaybeRotateChat()
	{
		int now = GetGame().GetTime();
		int diff = now - m_LastChatRotation;
		// rotate every 30 minutes (1800000 ms)
		if (diff < 1800000 && m_LastChatRotation != 0)
			return;
		
		m_LastChatRotation = now;
		
		// archive current chat if not empty
		if (m_ChatHistory && m_ChatHistory.Messages && m_ChatHistory.Messages.Count() > 0)
		{
			string fname = m_ChatArchiveFolder + "chat_" + now.ToString() + ".json";
			JsonFileLoader<GPDA_ChatHistory>.JsonSaveFile(fname, m_ChatHistory);
		}
		
		// keep only last MAX_CHAT_MESSAGES in active file (already capped), no clear to avoid empty view
		SaveChatData();
	}
	
	protected void BroadcastChatMessage(GPDA_ChatMessage msg)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(msg.SenderName);
		rpc.Write(msg.Message);
		rpc.Write(msg.Timestamp);
		rpc.Send(null, GPDA_RPC.RECEIVE_CHAT_MESSAGE, true, null);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// PRIVATE MESSAGES
	// ═══════════════════════════════════════════════════════════════════════════
	
	bool SendPrivateMessage(string senderID, string senderName, string recipientName, string message)
	{
		string recipientID = "";
		if (g_GPDA_AccountManager)
		{
			recipientID = g_GPDA_AccountManager.ResolvePlayerIdByName(recipientName);
		}
		
		// Find recipient by nickname
		foreach (GPDA_PlayerStats p : m_PlayersData.Players)
		{
			if (recipientID == "" && p && p.Nickname == recipientName)
			{
				recipientID = p.Steam64ID;
				break;
			}
		}
		
		if (recipientID == "")
		{
			return false; // Recipient not found
		}
		
		GPDA_PrivateMessage pm = new GPDA_PrivateMessage();
		pm.SenderID = senderID;
		pm.SenderName = senderName;
		pm.RecipientID = recipientID;
		pm.RecipientName = recipientName;
		pm.Message = message;
		pm.Timestamp = GetGame().GetTime();
		pm.IsRead = false;
		
		// Store for recipient
		if (!m_PrivateMessages.Contains(recipientID))
		{
			m_PrivateMessages.Set(recipientID, new array<ref GPDA_PrivateMessage>);
		}
		m_PrivateMessages.Get(recipientID).Insert(pm);
		
		// Also store for sender (so they can see sent messages)
		if (!m_PrivateMessages.Contains(senderID))
		{
			m_PrivateMessages.Set(senderID, new array<ref GPDA_PrivateMessage>);
		}
		m_PrivateMessages.Get(senderID).Insert(pm);
		
		// Try to send to online recipient
		SendPMToPlayer(recipientID, pm);
		
		return true;
	}
	
	protected void SendPMToPlayer(string recipientID, GPDA_PrivateMessage pm)
	{
		// Find online player
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity())
			{
				if (player.GetIdentity().GetPlainId() == recipientID)
				{
					ScriptRPC rpc = new ScriptRPC();
					rpc.Write(pm.SenderName);
					rpc.Write(pm.Message);
					rpc.Write(pm.Timestamp);
					rpc.Send(player, GPDA_RPC.RECEIVE_PM, true, player.GetIdentity());
					break;
				}
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// RPC HANDLER
	// ═══════════════════════════════════════════════════════════════════════════
	
	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (!GetGame().IsServer()) return;
		
		switch (rpc_type)
		{
			case GPDA_RPC.SEND_CHAT_MESSAGE:
				HandleSendChat(sender, ctx);
				break;
				
			case GPDA_RPC.REQUEST_CHAT_HISTORY:
				HandleRequestChatHistory(sender);
				break;
				
			case GPDA_RPC.SEND_PM:
				HandleSendPM(sender, ctx);
				break;
				
			case GPDA_RPC.REQUEST_PROFILE:
				HandleRequestProfile(sender);
				break;
				
			case GPDA_RPC.REQUEST_TOP100:
				HandleRequestTop100(sender, ctx);
				break;
				
			case GPDA_RPC.ADD_MARKER:
				// markers disabled
				break;
				
			case GPDA_RPC.REQUEST_MARKERS:
				// markers disabled
				break;
			
			// ═══════════════════════════════════════════════════════════
			// Capture System RPCs
			// ═══════════════════════════════════════════════════════════
			case GPDA_RPC.CAPTURE_START:
				HandleCaptureStart(sender, ctx);
				break;
				
			case GPDA_RPC.CAPTURE_CANCEL:
				HandleCaptureCancel(sender, ctx);
				break;
				
			case GPDA_RPC.CAPTURE_REQUEST_INFO:
				HandleCaptureRequestInfo(sender, ctx);
				break;
				
			case GPDA_RPC.CAPTURE_ATTACH_FLAG:
				HandleCaptureAttachFlag(sender, ctx);
				break;
		}
	}
	
	protected void HandleSendChat(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender) return;
		
		string message;
		if (!ctx.Read(message)) return;
		
		// Sanitize message
		message = message.Trim();
		if (message == "" || message.Length() > 200) return;
		
		string senderID = sender.GetPlainId();
		string senderName = sender.GetName();
		
		if (g_GPDA_AccountManager)
		{
			if (!g_GPDA_AccountManager.IsLoggedIn(senderID))
				return;
			senderName = g_GPDA_AccountManager.GetDisplayName(senderID, senderName);
		}
		
		AddChatMessage(senderID, senderName, message);
	}
	
	protected void HandleRequestChatHistory(PlayerIdentity sender)
	{
		if (!sender) return;
		
		// Send last 50 messages
		ScriptRPC rpc = new ScriptRPC();
		
		int count = Math.Min(50, m_ChatHistory.Messages.Count());
		int startIdx = m_ChatHistory.Messages.Count() - count;
		
		rpc.Write(count);
		
		for (int i = startIdx; i < m_ChatHistory.Messages.Count(); i++)
		{
			GPDA_ChatMessage msg = m_ChatHistory.Messages[i];
			rpc.Write(msg.SenderName);
			rpc.Write(msg.Message);
			rpc.Write(msg.Timestamp);
		}
		
		// Find player object
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity() && player.GetIdentity().GetPlainId() == sender.GetPlainId())
			{
				rpc.Send(player, GPDA_RPC.RECEIVE_CHAT_HISTORY, true, sender);
				break;
			}
		}
	}
	
	protected void HandleSendPM(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender) return;
		
		string recipientName, message;
		if (!ctx.Read(recipientName)) return;
		if (!ctx.Read(message)) return;
		
		message = message.Trim();
		if (message == "" || message.Length() > 500) return;
		
		SendPrivateMessage(sender.GetPlainId(), sender.GetName(), recipientName, message);
	}
	
	protected void HandleRequestProfile(PlayerIdentity sender)
	{
		if (!sender) return;
		
		string steam64 = sender.GetPlainId();
		GPDA_PlayerStats stats = GetOrCreatePlayer(steam64, sender.GetName());
		if (!stats) return;
		
		string nickname = stats.Nickname;
		if (g_GPDA_AccountManager)
		{
			nickname = g_GPDA_AccountManager.GetDisplayName(steam64, nickname);
		}
		
		// Get group name
		string groupName = "Loner";
		if (g_GPDA_GroupManager)
		{
			GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(steam64);
			if (group)
				groupName = group.Name;
		}
		
		// Get licenses as bitmask
		int licenseFlags = 0;
		if (g_GPDA_PlayerDB)
		{
			GPDA_PlayerProfile profile = g_GPDA_PlayerDB.GetProfile(steam64);
			if (profile && profile.Licenses)
			{
				if (profile.Licenses.Hunting) licenseFlags = licenseFlags | 1;
				if (profile.Licenses.Fishing) licenseFlags = licenseFlags | 2;
				if (profile.Licenses.ZoneAccess) licenseFlags = licenseFlags | 4;
				if (profile.Licenses.Weapons) licenseFlags = licenseFlags | 8;
				if (profile.Licenses.Trading) licenseFlags = licenseFlags | 16;
			}
		}
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(nickname);
		rpc.Write(stats.KillsPlayers);
		rpc.Write(stats.KillsZombies);
		rpc.Write(stats.KillsAnimals);
		rpc.Write(stats.Deaths);
		rpc.Write(stats.PlaytimeMinutes);
		rpc.Write(groupName);
		rpc.Write(licenseFlags);
		
		// Find player
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity() && player.GetIdentity().GetPlainId() == steam64)
			{
				rpc.Send(player, GPDA_RPC.RECEIVE_PROFILE, true, sender);
				break;
			}
		}
	}
	
	protected void HandleRequestTop100(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender) return;
		
		int sortType;
		if (!ctx.Read(sortType)) sortType = GPDA_SortType.BY_TOTAL_KILLS;
		
		ref array<ref GPDA_TopEntry> top = GetTop100(sortType);
		
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(top.Count());
		
		foreach (GPDA_TopEntry e : top)
		{
			rpc.Write(e.Rank);
			rpc.Write(e.Nickname);
			rpc.Write(e.KillsPlayers);
			rpc.Write(e.KillsZombies);
			rpc.Write(e.TotalKills);
		}
		
		// Find player
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity() && player.GetIdentity().GetPlainId() == sender.GetPlainId())
			{
				rpc.Send(player, GPDA_RPC.RECEIVE_TOP100, true, sender);
				break;
			}
		}
	}
	
	// markers removed
	
	// ═══════════════════════════════════════════════════════════════════════════
	// CAPTURE SYSTEM RPC HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void HandleCaptureStart(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender) return;
		PlayerBase player = FindPlayerByIdentity(sender);
		if (!player) return;
		if (g_GPDA_CaptureManager)
			g_GPDA_CaptureManager.HandleCaptureStart(player, ctx);
	}
	
	protected void HandleCaptureCancel(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender) return;
		PlayerBase player = FindPlayerByIdentity(sender);
		if (!player) return;
		if (g_GPDA_CaptureManager)
			g_GPDA_CaptureManager.HandleCaptureCancel(player, ctx);
	}
	
	protected void HandleCaptureRequestInfo(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender) return;
		PlayerBase player = FindPlayerByIdentity(sender);
		if (!player) return;
		if (g_GPDA_CaptureManager)
			g_GPDA_CaptureManager.HandleCaptureRequestInfo(player, ctx);
	}
	
	protected void HandleCaptureAttachFlag(PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender) return;
		PlayerBase player = FindPlayerByIdentity(sender);
		if (!player) return;
		if (g_GPDA_CaptureManager)
			g_GPDA_CaptureManager.HandleCaptureAttachFlag(player, ctx);
	}
	
	protected PlayerBase FindPlayerByIdentity(PlayerIdentity identity)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (pb && pb.GetIdentity() && pb.GetIdentity().GetPlainId() == identity.GetPlainId())
				return pb;
		}
		return null;
	}
}

// Global instance
static ref GPDA_ServerManager g_GPDA_Server;

