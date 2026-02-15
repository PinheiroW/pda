// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Data Classes for serialization
// ═══════════════════════════════════════════════════════════════════════════════

// Player profile/stats
class GPDA_PlayerStats
{
	string Steam64ID;
	string Nickname;
	int KillsPlayers;
	int KillsZombies;
	int KillsAnimals;
	int Deaths;
	int PlaytimeMinutes;
	int LastOnline; // Unix timestamp
	
	void GPDA_PlayerStats()
	{
		Steam64ID = "";
		Nickname = "Unknown";
		KillsPlayers = 0;
		KillsZombies = 0;
		KillsAnimals = 0;
		Deaths = 0;
		PlaytimeMinutes = 0;
		LastOnline = 0;
	}
	
	int GetTotalKills()
	{
		return KillsPlayers + KillsZombies + KillsAnimals;
	}
}

// Chat message
class GPDA_ChatMessage
{
	string SenderName;
	string SenderID;
	string Message;
	int Timestamp;
	
	void GPDA_ChatMessage()
	{
		SenderName = "";
		SenderID = "";
		Message = "";
		Timestamp = 0;
	}
}

// Private message
class GPDA_PrivateMessage
{
	string SenderName;
	string SenderID;
	string RecipientName;
	string RecipientID;
	string Message;
	int Timestamp;
	bool IsRead;
	
	void GPDA_PrivateMessage()
	{
		SenderName = "";
		SenderID = "";
		RecipientName = "";
		RecipientID = "";
		Message = "";
		Timestamp = 0;
		IsRead = false;
	}
}

// Map marker
class GPDA_MapMarker
{
	string ID;
	string OwnerID;
	string Name;
	float PosX;
	float PosZ;
	int Color; // ARGB
	int Icon; // 0-9 different icons
	bool IsShared; // Visible to all or only owner
	
	void GPDA_MapMarker()
	{
		ID = "";
		OwnerID = "";
		Name = "Marker";
		PosX = 0;
		PosZ = 0;
		Color = 0xFFFF8800; // Orange
		Icon = 0;
		IsShared = false;
	}
}

// Top 100 entry (simplified for transfer)
class GPDA_TopEntry
{
	int Rank;
	string Nickname;
	int KillsPlayers;
	int KillsZombies;
	int TotalKills;
	
	void GPDA_TopEntry()
	{
		Rank = 0;
		Nickname = "";
		KillsPlayers = 0;
		KillsZombies = 0;
		TotalKills = 0;
	}
}

// Container for all player data (saved to JSON)
class GPDA_AllPlayersData
{
	ref array<ref GPDA_PlayerStats> Players;
	
	void GPDA_AllPlayersData()
	{
		Players = new array<ref GPDA_PlayerStats>;
	}
}

// Container for chat history (saved to JSON)
class GPDA_ChatHistory
{
	ref array<ref GPDA_ChatMessage> Messages;
	
	void GPDA_ChatHistory()
	{
		Messages = new array<ref GPDA_ChatMessage>;
	}
}

// Container for markers (saved to JSON)
class GPDA_MarkersData
{
	ref array<ref GPDA_MapMarker> Markers;
	
	void GPDA_MarkersData()
	{
		Markers = new array<ref GPDA_MapMarker>;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// TICKET SYSTEM DATA CLASSES
// ═══════════════════════════════════════════════════════════════════════════════

// Ticket info for display in list
class GPDA_TicketInfo
{
	string Id;
	string Title;
	string Status;
	string PlayerName;
	
	void GPDA_TicketInfo(string id, string title, string status, string playerName)
	{
		Id = id;
		Title = title;
		Status = status;
		PlayerName = playerName;
	}
}

// Ticket message for display in chat
class GPDA_TicketMessageInfo
{
	string SenderName;
	string Message;
	bool IsAdmin;
	
	void GPDA_TicketMessageInfo(string senderName, string message, bool isAdmin)
	{
		SenderName = senderName;
		Message = message;
		IsAdmin = isAdmin;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// TICKET SYSTEM CORE CLASSES (moved here to ensure early loading)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_TicketMessage
{
	protected string m_SenderUID;
	protected string m_SenderName;
	protected string m_Message;
	protected bool m_IsAdmin;
	protected int m_Timestamp;

	void GPDA_TicketMessage(string senderUID, string senderName, string message, bool isAdmin)
	{
		m_SenderUID = senderUID;
		m_SenderName = senderName;
		m_Message = message;
		m_IsAdmin = isAdmin;
		m_Timestamp = GetGame().GetTime();
	}

	string GetSenderUID() { return m_SenderUID; }
	string GetSenderName() { return m_SenderName; }
	string GetMessage() { return m_Message; }
	bool IsAdmin() { return m_IsAdmin; }
	int GetTimestamp() { return m_Timestamp; }
	void SetTimestamp(int ts) { m_Timestamp = ts; }
}

class GPDA_Ticket
{
	protected string m_Id;
	protected string m_PlayerUID;
	protected string m_PlayerName;
	protected string m_Title;
	protected string m_Description;
	protected string m_Status;
	protected string m_AssignedAdminUID;
	protected string m_AssignedAdminName;
	protected int m_CreatedTime;
	protected int m_LastUpdateTime;
	protected ref array<ref GPDA_TicketMessage> m_Messages;

	void GPDA_Ticket(string playerUID, string playerName, string title, string description)
	{
		m_Id = GenerateTicketId();
		m_PlayerUID = playerUID;
		m_PlayerName = playerName;
		m_Title = title;
		m_Description = description;
		m_Status = "open";
		m_AssignedAdminUID = "";
		m_AssignedAdminName = "";
		m_CreatedTime = GetGame().GetTime();
		m_LastUpdateTime = m_CreatedTime;
		m_Messages = new array<ref GPDA_TicketMessage>;
	}

	string GenerateTicketId()
	{
		int time = GetGame().GetTime();
		int random = Math.RandomInt(1000, 9999);
		return "TKT_" + time.ToString() + "_" + random.ToString();
	}

	// Getters
	string GetId() { return m_Id; }
	string GetPlayerUID() { return m_PlayerUID; }
	string GetPlayerName() { return m_PlayerName; }
	string GetTitle() { return m_Title; }
	string GetDescription() { return m_Description; }
	string GetStatus() { return m_Status; }
	string GetAssignedAdminUID() { return m_AssignedAdminUID; }
	string GetAssignedAdminName() { return m_AssignedAdminName; }
	int GetCreatedTime() { return m_CreatedTime; }
	int GetLastUpdateTime() { return m_LastUpdateTime; }
	array<ref GPDA_TicketMessage> GetMessages() { return m_Messages; }

	// Setters
	void SetId(string id) { m_Id = id; }
	void SetStatusDirect(string status) { m_Status = status; }
	void SetAssignedAdmin(string uid, string name)
	{
		m_AssignedAdminUID = uid;
		m_AssignedAdminName = name;
	}
	void SetTimes(int created, int updated)
	{
		m_CreatedTime = created;
		m_LastUpdateTime = updated;
	}

	// Actions
	void SetStatus(string status)
	{
		m_Status = status;
		m_LastUpdateTime = GetGame().GetTime();
	}

	void AssignToAdmin(string adminUID, string adminName)
	{
		m_AssignedAdminUID = adminUID;
		m_AssignedAdminName = adminName;
		m_Status = "assigned";
		m_LastUpdateTime = GetGame().GetTime();
	}

	void CloseTicket()
	{
		m_Status = "closed";
		m_LastUpdateTime = GetGame().GetTime();
	}

	void AddMessage(string senderUID, string senderName, string message, bool isAdmin)
	{
		GPDA_TicketMessage newMessage = new GPDA_TicketMessage(senderUID, senderName, message, isAdmin);
		m_Messages.Insert(newMessage);
		m_LastUpdateTime = GetGame().GetTime();
	}

	void AddMessageDirect(string senderUID, string senderName, string message, bool isAdmin, int timestamp)
	{
		GPDA_TicketMessage newMessage = new GPDA_TicketMessage(senderUID, senderName, message, isAdmin);
		newMessage.SetTimestamp(timestamp);
		m_Messages.Insert(newMessage);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// CAPTURE SYSTEM DATA CLASSES
// ═══════════════════════════════════════════════════════════════════════════════

// Capture point info for display
class GPDA_CapturePointInfo
{
	string Name;
	vector Position;
	float Radius;
	string OwnerGroup;
	int RewardPoints;
	
	void GPDA_CapturePointInfo(string name, vector pos, float radius, string owner, int points)
	{
		Name = name;
		Position = pos;
		Radius = radius;
		OwnerGroup = owner;
		RewardPoints = points;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// GROUP MEMBER DATA CLASSES
// ═══════════════════════════════════════════════════════════════════════════════

// Group member info for display
class GPDA_GroupMember
{
	string Steam64;
	string Name;
	bool IsOnline;
	
	void GPDA_GroupMember(string steam64 = "", string nickname = "", bool isOnline = false)
	{
		Steam64 = steam64;
		Name = nickname;
		IsOnline = isOnline;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// RANK SYSTEM DATA CLASSES
// ═══════════════════════════════════════════════════════════════════════════════

// GPDA_RankData and GPDA_QuestData are defined in GPDA_QuestClasses.c

// ═══════════════════════════════════════════════════════════════════════════════
// QUEST DISPLAY CLASSES (for UI)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_QuestDisplayInfo
{
	string ID;
	string Title;
	string Objective;
	int Progress;
	int Required;
	int Status;
	
	void GPDA_QuestDisplayInfo()
	{
		ID = "";
		Title = "";
		Objective = "";
		Progress = 0;
		Required = 1;
		Status = 0;
	}
}

class GPDA_QuestAvailableInfo
{
	string ID;
	string Title;
	string Description;
	string Objective;
	
	void GPDA_QuestAvailableInfo()
	{
		ID = "";
		Title = "";
		Description = "";
		Objective = "";
	}
}

