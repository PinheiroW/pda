// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Client RPC Handler
// Receives data from server and updates UI
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_ClientRPC
{
	// Icon constant for notifications
	static string ICON_CONNECT = "set:dayz_gui image:icon_connect";
	
	void GPDA_ClientRPC()
	{
		if (GetGame().IsDedicatedServer()) return;
		// Ensure chat notify HUD exists even if PDA was never opened
		GPDA_ChatNotify.GetInstance();
		
		GetDayZGame().Event_OnRPC.Insert(OnRPC);
		
		// Register RPC for Group data (standalone PDA system)
		GetRPCManager().AddRPC("GPDA_Groups", "ClientReceiveGroupData", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Groups", "GroupCheckResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Groups", "ReceiveAllGroups", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Groups", "ReceiveCapturedZones", this, SingleplayerExecutionType.Client);
		
		// Register RPC for Database (Military only)
		GetRPCManager().AddRPC("GPDA_Database", "ReceiveAccessCheck", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Database", "ReceiveAdminStatus", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Database", "ReceiveSearchResults", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Database", "ReceiveDossier", this, SingleplayerExecutionType.Client);
		
		// Register RPC for Rank System (from GreshnikServer_ModsPack)
		GetRPCManager().AddRPC("RankMod", "ClientReceivePlayerRank", this, SingleplayerExecutionType.Client);
		
		// Register RPC for Ticket System
		GetRPCManager().AddRPC("GPDA_Tickets", "ReceiveResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Tickets", "ReceiveMyTickets", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Tickets", "ReceiveAllTickets", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Tickets", "ReceiveTicketMessages", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Tickets", "ReceiveNotification", this, SingleplayerExecutionType.Client);
		
		// Register RPC for Capture System
		GetRPCManager().AddRPC("GPDA_Capture", "ClientReceivePoints", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Capture", "AdminResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Capture", "ShowCaptureNotif", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Capture", "ShowNotification", this, SingleplayerExecutionType.Client);
		
		// Register RPC for Accounts
		GetRPCManager().AddRPC("GPDA_Accounts", "AuthStatus", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Accounts", "AuthResponse", this, SingleplayerExecutionType.Client);
		
		// Register RPC for Rank System
		GetRPCManager().AddRPC("GPDA_Rank", "ClientReceiveRank", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Rank", "ReceiveRanksCount", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Rank", "ReceiveRankData", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Rank", "ReceivePlayerRank", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Rank", "AdminResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Rank", "ReceiveXPConfig", this, SingleplayerExecutionType.Client);
		
		// Register RPC for Quest System
		GetRPCManager().AddRPC("GPDA_Quests", "ClientReceiveQuests", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Quests", "ClientReceiveAvailable", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Quests", "QuestResponse", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC("GPDA_Quests", "AdminResponse", this, SingleplayerExecutionType.Client);
		
		Print("[GPDA] Client RPC handler initialized");
	}

	void ReceiveXPConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		Param3<string, string, string> data;
		if (!ctx.Read(data)) return;
		string killsStr = data.param1;
		string handsStr = data.param2;
		string invStr = data.param3;
		Print("[GPDA] Received XP config: killsLen=" + killsStr.Length().ToString() + " handsLen=" + handsStr.Length().ToString() + " invLen=" + invStr.Length().ToString());
		GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
		if (adminMenu)
		{
			adminMenu.UpdateXPConfig(killsStr, handsStr, invStr);
		}
	}
	
	void ~GPDA_ClientRPC()
	{
		GetDayZGame().Event_OnRPC.Remove(OnRPC);
	}
	
	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		if (GetGame().IsDedicatedServer()) return;
		
		switch (rpc_type)
		{
			case GPDA_RPC.RECEIVE_CHAT_MESSAGE:
				HandleChatMessage(ctx);
				break;
				
			case GPDA_RPC.RECEIVE_CHAT_HISTORY:
				HandleChatHistory(ctx);
				break;
				
			case GPDA_RPC.RECEIVE_PM:
				HandlePM(ctx);
				break;
				
			case GPDA_RPC.RECEIVE_PROFILE:
				HandleProfile(ctx);
				break;
				
			case GPDA_RPC.RECEIVE_TOP100:
				HandleTop100(ctx);
				break;
				
			case GPDA_RPC.RECEIVE_MARKERS:
				HandleMarkers(ctx);
				break;
				
			// ═══════════════════════════════════════════════════════════════
			// Mutant Spawner RPCs
			// ═══════════════════════════════════════════════════════════════
			case GPDA_RPC.MSP_RECEIVE_ZONES:
				if (GPDA_MutantSpawnerUI.GetInstance())
					GPDA_MutantSpawnerUI.GetInstance().HandleReceiveZones(ctx);
				break;
				
			case GPDA_RPC.MSP_RECEIVE_PRESETS:
				if (GPDA_MutantSpawnerUI.GetInstance())
					GPDA_MutantSpawnerUI.GetInstance().HandleReceivePresetCount(ctx);
				break;
			
			case GPDA_RPC.MSP_RECEIVE_ONE_PRESET:
				if (GPDA_MutantSpawnerUI.GetInstance())
					GPDA_MutantSpawnerUI.GetInstance().HandleReceiveOnePreset(ctx);
				break;
				
			case GPDA_RPC.MSP_STATUS:
				if (GPDA_MutantSpawnerUI.GetInstance())
					GPDA_MutantSpawnerUI.GetInstance().HandleStatus(ctx);
				break;
			
			// ═══════════════════════════════════════════════════════════
			// Group Admin RPCs
			// ═══════════════════════════════════════════════════════════
			case GPDA_RPC.GRP_RECEIVE_ALL:
				if (GPDA_GroupAdminUI.GetInstance())
					GPDA_GroupAdminUI.GetInstance().HandleReceiveCount(ctx);
				break;
			
			case GPDA_RPC.GRP_RECEIVE_ONE:
				if (GPDA_GroupAdminUI.GetInstance())
					GPDA_GroupAdminUI.GetInstance().HandleReceiveOne(ctx);
				break;
				
			case GPDA_RPC.GRP_STATUS:
				if (GPDA_GroupAdminUI.GetInstance())
					GPDA_GroupAdminUI.GetInstance().HandleStatus(ctx);
				break;
			
			case GPDA_RPC.GRP_RECEIVE_PLAYERS:
				if (GPDA_GroupAdminUI.GetInstance())
					GPDA_GroupAdminUI.GetInstance().HandleReceivePlayers(ctx);
				break;
			
			// ═══════════════════════════════════════════════════════════
			// Capture System RPCs
			// ═══════════════════════════════════════════════════════════
			case GPDA_RPC.CAPTURE_PROGRESS:
				if (GPDA_CaptureMenu.GetInstance())
					GPDA_CaptureMenu.GetInstance().HandleCaptureProgress(ctx);
				else if (GPDA_CaptureMenu.IsHudActive())
				{
					Param1<string> hudData;
					if (ctx.Read(hudData))
					{
						array<string> hudParts = new array<string>;
						hudData.param1.Split(",", hudParts);
						if (hudParts.Count() >= 1)
							GPDA_CaptureMenu.UpdateHudProgress(hudParts[0].ToFloat());
					}
				}
				break;
				
			case GPDA_RPC.CAPTURE_COMPLETE:
				if (GPDA_CaptureMenu.GetInstance())
					GPDA_CaptureMenu.GetInstance().HandleCaptureComplete(ctx);
				else
					GPDA_CaptureMenu.HideHud();
				break;
				
			case GPDA_RPC.CAPTURE_CANCEL:
				if (GPDA_CaptureMenu.GetInstance())
					GPDA_CaptureMenu.GetInstance().HandleCaptureCancelled(ctx);
				else
					GPDA_CaptureMenu.HideHud();
				break;
				
			case GPDA_RPC.CAPTURE_INFO:
				if (GPDA_CaptureMenu.GetInstance())
					GPDA_CaptureMenu.GetInstance().HandleCaptureInfo(ctx);
				break;
		}
	}
	
	void ReceiveCapturedZones(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		Param1<string> data;
		if (!ctx.Read(data)) return;
		string zonesStr = data.param1;
		array<string> zones = new array<string>;
		if (zonesStr != "")
		{
			TStringArray entries = new TStringArray;
			zonesStr.Split(";", entries);
			foreach (string z : entries)
			{
				if (z != "")
					zones.Insert(z);
			}
		}
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateGroupZones(zones);
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// GROUP DATA HANDLER (Standalone PDA System)
	// ═══════════════════════════════════════════════════════════════════════════
	
	void ClientReceiveGroupData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param3<string, int, ref array<ref Param3<string, string, bool>>> data;
		if (!ctx.Read(data)) return;
		
		string groupName = data.param1;
		int groupScore = data.param2;
		ref array<ref Param3<string, string, bool>> rawMembers = data.param3;
		
		// Convert to our format
		ref array<ref GPDA_GroupMember> members = new array<ref GPDA_GroupMember>;
		if (rawMembers)
		{
			foreach (ref Param3<string, string, bool> entry : rawMembers)
			{
				if (entry)
				{
					members.Insert(new GPDA_GroupMember(entry.param1, entry.param2, entry.param3));
				}
			}
		}
		
		// Update menu
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateGroupInfo(groupName, groupScore, 0);
			menu.UpdateGroupMembers(members);
		}
	}
	
	// Handle group check response - show/hide Groups tab
	void GroupCheckResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param2<bool, string> data;
		if (!ctx.Read(data)) return;
		
		bool hasGroup = data.param1;
		string groupName = data.param2;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.OnGroupCheckReceived(hasGroup, groupName);
		}
	}
	
	// Receive all groups list for admin panel
	void ReceiveAllGroups(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string groupsStr = data.param1;
		array<string> groups = new array<string>;
		
		if (groupsStr != "")
		{
			TStringArray entries = new TStringArray;
			groupsStr.Split(";", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
					groups.Insert(entry);
			}
		}
		
		Print("[GPDA] Received " + groups.Count().ToString() + " groups from server");
		
		// Update admin menu / ticket tool
		GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
		if (adminMenu)
		{
			Print("[GPDA] Calling UpdateGroupsList on admin menu");
			adminMenu.UpdateGroupsList(groups);
		}
		else
		{
			Print("[GPDA] Admin menu instance not found!");
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// CHAT HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	protected void HandleChatMessage(ParamsReadContext ctx)
	{
		string senderName, message;
		int timestamp;
		
		if (!ctx.Read(senderName)) return;
		if (!ctx.Read(message)) return;
		if (!ctx.Read(timestamp)) return;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
			menu.AddChatMessage(senderName, message, timestamp);
		
		if (!GPDA_Menu.IsChatNotifyEnabled())
			return;
		
		string sn = senderName;
		string msg = message;
		if (sn.Length() > 28)
			sn = sn.Substring(0, 28);
		if (msg.Length() > 80)
			msg = msg.Substring(0, 80) + "...";
		
		GPDA_ChatNotify notify = GPDA_ChatNotify.GetInstance();
		if (notify)
		{
			Print("[GPDA][CHAT] Notify route: '" + sn + "' msgLen=" + msg.Length().ToString());
			notify.PushForce(sn, msg);
		}
	}
	
	void ReceivePlayerRank(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		Param4<string, int, int, string> data;
		if (!ctx.Read(data)) return;
		
		string rankName = data.param1;
		int xp = data.param2;
		int nextXP = data.param3;
		string nextRankName = data.param4;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateProfileRank(rankName, xp, nextXP, nextRankName);
			menu.UpdateRankDisplay(rankName, xp, nextXP);
		}
	}
	
	protected void HandleChatHistory(ParamsReadContext ctx)
	{
		int count;
		if (!ctx.Read(count)) return;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.ClearChatMessages();
		}
		
		for (int i = 0; i < count; i++)
		{
			string senderName, message;
			int timestamp;
			
			if (!ctx.Read(senderName)) break;
			if (!ctx.Read(message)) break;
			if (!ctx.Read(timestamp)) break;
			
			if (menu)
			{
				menu.AddChatMessage(senderName, message, timestamp);
			}
		}
	}
	
	protected void HandlePM(ParamsReadContext ctx)
	{
		string senderName, message;
		int timestamp;
		
		if (!ctx.Read(senderName)) return;
		if (!ctx.Read(message)) return;
		if (!ctx.Read(timestamp)) return;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.AddPMMessage(senderName, message);
		}
		
		Print("[GPDA] New PM from: " + senderName);
	}
	
	protected void HandleProfile(ParamsReadContext ctx)
	{
		string nickname;
		int killsPlayers, killsZombies, killsAnimals, deaths, playtime;
		string groupName;
		int licenseFlags;
		
		if (!ctx.Read(nickname)) return;
		if (!ctx.Read(killsPlayers)) return;
		if (!ctx.Read(killsZombies)) return;
		if (!ctx.Read(killsAnimals)) return;
		if (!ctx.Read(deaths)) return;
		if (!ctx.Read(playtime)) return;
		if (!ctx.Read(groupName)) groupName = "Loner";
		if (!ctx.Read(licenseFlags)) licenseFlags = 0;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateProfile(nickname, killsPlayers, killsZombies, killsAnimals, deaths, playtime);
			menu.UpdateProfileExtended(groupName, licenseFlags);
		}
	}
	
	protected void HandleTop100(ParamsReadContext ctx)
	{
		int count;
		if (!ctx.Read(count)) return;
		
		array<ref GPDA_TopEntry> entries = new array<ref GPDA_TopEntry>;
		
		for (int i = 0; i < count; i++)
		{
			GPDA_TopEntry e = new GPDA_TopEntry();
			
			if (!ctx.Read(e.Rank)) break;
			if (!ctx.Read(e.Nickname)) break;
			if (!ctx.Read(e.KillsPlayers)) break;
			if (!ctx.Read(e.KillsZombies)) break;
			if (!ctx.Read(e.TotalKills)) break;
			
			entries.Insert(e);
		}
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateTop100(entries);
		}
	}
	
	protected void HandleMarkers(ParamsReadContext ctx)
	{
		int count;
		if (!ctx.Read(count)) return;
		
		Print("[GPDA] Received " + count + " markers");
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// DATABASE HANDLERS (Military Only)
	// ═══════════════════════════════════════════════════════════════════════════
	
	void ReceiveAccessCheck(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param2<bool, string> data;
		if (!ctx.Read(data)) return;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.SetDatabaseAccess(data.param1);
			menu.SetHeaderFaction(data.param2);
		}
	}
	
	void ReceiveAdminStatus(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<bool> data;
		if (!ctx.Read(data)) return;
		
		bool isAdmin = data.param1;
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.SetAdminStatus(isAdmin);
		}
	}
	
	void ReceiveSearchResults(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string resultString = data.param1;
		
		ref array<ref GPDA_PlayerSearchResult> results = new array<ref GPDA_PlayerSearchResult>;
		
		if (resultString != "")
		{
			// Parse "callsign|steam64;callsign|steam64;..."
			TStringArray entries = new TStringArray;
			resultString.Split(";", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
				{
					TStringArray parts = new TStringArray;
					entry.Split("|", parts);
					
					if (parts.Count() >= 2)
					{
						results.Insert(new GPDA_PlayerSearchResult(parts[0], parts[1]));
					}
				}
			}
		}
		
		Print("[GPDA] Received search results: " + results.Count().ToString() + " players");
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateSearchResults(results);
		}
	}
	
	void ReceiveDossier(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		// Parse: callsign|steam64|age|group|bio|licenses
		string dossierStr = data.param1;
		array<string> parts = new array<string>;
		dossierStr.Split("|", parts);
		
		if (parts.Count() < 6)
		{
			Print("[GPDA] ReceiveDossier ERROR: parts count < 6, got " + parts.Count().ToString());
			return;
		}
		
		string callsign = parts[0];
		string steam64 = parts[1];
		int age = parts[2].ToInt();
		string groupName = parts[3];
		string bio = parts[4];
		
		// Replace "-" placeholder with empty string
		if (callsign == "-") callsign = "";
		if (groupName == "-") groupName = "Loner";
		if (bio == "-") bio = "";
		
		// Parse license flags (integer bitmask)
		int licenseFlags = parts[5].ToInt();
		
		Print("[GPDA] ReceiveDossier: callsign=" + callsign + " steam64=" + steam64 + " licenseFlags=" + licenseFlags.ToString());
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateDossier(callsign, steam64, age, groupName, bio, licenseFlags);
		}
		else
		{
			Print("[GPDA] ReceiveDossier ERROR: menu is null!");
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// RANK SYSTEM HANDLER (from GreshnikServer_ModsPack)
	// ═══════════════════════════════════════════════════════════════════════════
	
	void ClientReceivePlayerRank(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param3<string, int, int> data;
		if (!ctx.Read(data)) return;
		
		string rankName = data.param1;
		int xp = data.param2;
		int nextXP = data.param3;
		
		Print("[GPDA] Received rank data: " + rankName + ", XP: " + xp.ToString() + ", NextXP: " + nextXP.ToString());
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateRankDisplay(rankName, xp, nextXP);
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// TICKET SYSTEM HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void ReceiveResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;

		Print("[GPDA] ReceiveResponse RPC received");
		
		Param2<bool, string> data;
		if (!ctx.Read(data))
		{
			Print("[GPDA] Failed to read ReceiveResponse data");
			return;
		}
		
		Print("[GPDA] Ticket response: " + data.param2);
		
		// Show notification
		if (GetGame().GetMission())
		{
			NotificationSystem.AddNotificationExtended(5.0, "Support", data.param2, ICON_CONNECT);
			Print("[GPDA] Notification displayed for: " + data.param2);
		}
		else
		{
			Print("[GPDA] GetMission() returned null, cannot show notification");
		}
	}
	
	void ReceiveMyTickets(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string ticketsStr = data.param1;
		
		ref array<ref GPDA_TicketInfo> tickets = new array<ref GPDA_TicketInfo>;
		
		if (ticketsStr != "")
		{
			TStringArray entries = new TStringArray;
			ticketsStr.Split(";", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
				{
					TStringArray parts = new TStringArray;
					entry.Split("|", parts);
					
					if (parts.Count() >= 3)
					{
						tickets.Insert(new GPDA_TicketInfo(parts[0], parts[1], parts[2], ""));
					}
				}
			}
		}
		
		Print("[GPDA] Received my tickets: " + tickets.Count().ToString());
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateMyTickets(tickets);
		}
	}
	
	void ReceiveAllTickets(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string ticketsStr = data.param1;
		
		ref array<ref GPDA_TicketInfo> tickets = new array<ref GPDA_TicketInfo>;
		
		if (ticketsStr != "")
		{
			TStringArray entries = new TStringArray;
			ticketsStr.Split(";", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
				{
					TStringArray parts = new TStringArray;
					entry.Split("|", parts);
					
					if (parts.Count() >= 4)
					{
						tickets.Insert(new GPDA_TicketInfo(parts[0], parts[1], parts[2], parts[3]));
					}
				}
			}
		}
		
		Print("[GPDA] Received all tickets: " + tickets.Count().ToString());
		
		// Update admin menu / ticket tool
		GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
		if (adminMenu)
		{
			adminMenu.UpdateAllTickets(tickets);
		}
		
		// Also update main menu (for support tab)
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateAllTickets(tickets);
		}
	}
	
	void ReceiveTicketMessages(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param2<string, string> data;
		if (!ctx.Read(data)) return;
		
		string ticketId = data.param1;
		string messagesStr = data.param2;
		
		ref array<ref GPDA_TicketMessageInfo> messages = new array<ref GPDA_TicketMessageInfo>;
		
		if (messagesStr != "")
		{
			TStringArray entries = new TStringArray;
			messagesStr.Split(";;", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
				{
					TStringArray parts = new TStringArray;
					entry.Split("|", parts);
					
					if (parts.Count() >= 3)
					{
						messages.Insert(new GPDA_TicketMessageInfo(parts[0], parts[1], parts[2] == "1"));
					}
				}
			}
		}
		
		Print("[GPDA] Received ticket messages for " + ticketId + ": " + messages.Count().ToString());
		
		// Update admin menu / ticket tool
		GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
		if (adminMenu)
		{
			adminMenu.UpdateTicketMessages(ticketId, messages);
		}
		
		// Also update main menu (for support tab)
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateTicketMessages(ticketId, messages);
		}
		
		// Also update standalone ticket tools window (if open)
		GPDA_TicketToolMenu toolMenu = GPDA_TicketToolMenu.GetInstance();
		if (toolMenu)
		{
			toolMenu.UpdateTicketMessages(ticketId, messages);
		}
	}
	
	void ReceiveNotification(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string message = data.param1;
		
		if (GetGame().GetMission())
		{
			NotificationSystem.AddNotificationExtended(5.0, "Support", message, ICON_CONNECT);
		}

		// Try to auto-refresh ticket chat if ID present in message like "ticket: 123"
		string ticketId = "";
		if (message.Contains("ticket:"))
		{
			TStringArray parts = new TStringArray;
			message.Split("ticket:", parts);
			if (parts.Count() > 1)
			{
				string rest = parts[1];
				rest.Trim();
				rest.Replace(" ", "");
				ticketId = rest;
			}
		}

		if (ticketId != "")
		{
			GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
			if (adminMenu)
			{
				adminMenu.RequestTicketMessages(ticketId);
			}
			GPDA_Menu menu = GPDA_Menu.GetInstance();
			if (menu)
			{
				menu.RequestTicketMessages(ticketId);
			}
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// CAPTURE SYSTEM HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void ClientReceivePoints(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string pointsStr = data.param1;
		
		ref array<ref GPDA_CapturePointInfo> points = new array<ref GPDA_CapturePointInfo>;
		
		if (pointsStr != "")
		{
			TStringArray entries = new TStringArray;
			pointsStr.Split(";", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
				{
					TStringArray parts = new TStringArray;
					entry.Split("|", parts);
					
					if (parts.Count() >= 7)
					{
						string name = parts[0];
						float posX = parts[1].ToFloat();
						float posY = parts[2].ToFloat();
						float posZ = parts[3].ToFloat();
						float radius = parts[4].ToFloat();
						string owner = parts[5];
						int rewardPts = parts[6].ToInt();
						
						vector pos = Vector(posX, posY, posZ);
						points.Insert(new GPDA_CapturePointInfo(name, pos, radius, owner, rewardPts));
					}
				}
			}
		}
		
		Print("[GPDA] Received " + points.Count().ToString() + " capture points");
		
		// Update admin menu / ticket tool
		GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
		if (adminMenu)
		{
			adminMenu.UpdateCapturePoints(points);
		}
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateCapturePointsForMap(points);
		}
	}
	
	void AuthStatus(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		Param2<bool, string> data;
		if (!ctx.Read(data)) return;
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.SetAuthStatus(data.param1, data.param2);
		}
	}
	
	void AuthResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		Param3<bool, string, string> data;
		if (!ctx.Read(data)) return;
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.OnAuthResponse(data.param1, data.param2, data.param3);
		}
	}
	
	void AdminResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param2<bool, string> data;
		if (!ctx.Read(data)) return;
		
		Print("[GPDA] Capture admin response: " + data.param2);
	}
	
	void ShowCaptureNotif(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		Print("[GPDA] Capture: " + data.param1);
	}
	
	void ShowNotification(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param2<string, string> data;
		if (!ctx.Read(data)) return;
		
		Print("[GPDA] " + data.param1);
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// RANK SYSTEM HANDLER (PDA LITE internal)
	// ═══════════════════════════════════════════════════════════════════════════
	
	void ClientReceiveRank(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param3<string, int, int> data;
		if (!ctx.Read(data)) return;
		
		string rankName = data.param1;
		int xp = data.param2;
		int nextXP = data.param3;
		
		Print("[GPDA] Received rank: " + rankName + ", XP: " + xp.ToString() + ", NextXP: " + nextXP.ToString());
		
		GPDA_Menu menu = GPDA_Menu.GetInstance();
		if (menu)
		{
			menu.UpdateRankPanel(rankName, xp, nextXP);
		}
	}
	
	// Temporary storage for receiving ranks
	protected ref array<ref GPDA_RankEntry> m_TempRanks;
	protected int m_ExpectedRanksCount;
	
	void ReceiveRanksCount(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<int> data;
		if (!ctx.Read(data)) return;
		
		m_ExpectedRanksCount = data.param1;
		m_TempRanks = new array<ref GPDA_RankEntry>;
		
		Print("[GPDA] Expecting " + m_ExpectedRanksCount.ToString() + " ranks");
	}
	
	void ReceiveRankData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param4<int, string, int, string> data;
		if (!ctx.Read(data)) return;
		
		if (!m_TempRanks)
			m_TempRanks = new array<ref GPDA_RankEntry>;
		
		GPDA_RankEntry rank = new GPDA_RankEntry();
		rank.Order = data.param1;
		rank.Name = data.param2;
		rank.RequiredXP = data.param3;
		rank.Color = data.param4;
		m_TempRanks.Insert(rank);
		
		Print("[GPDA] Received rank: " + rank.Name + " (Order: " + rank.Order.ToString() + ", XP: " + rank.RequiredXP.ToString() + ")");
		
		// Check if all ranks received
		if (m_TempRanks.Count() >= m_ExpectedRanksCount)
		{
			// Update admin menu
			GPDA_AdminMenu adminMenu = GPDA_AdminMenu.GetInstance();
			if (adminMenu)
			{
				adminMenu.UpdateRanksList(m_TempRanks);
			}
			
			Print("[GPDA] All " + m_TempRanks.Count().ToString() + " ranks received and sent to admin menu");
		}
	}
	
	void RankAdminResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param2<bool, string> data;
		if (!ctx.Read(data)) return;
		
		Print("[GPDA] Rank admin response: " + data.param2);
		
		if (GetGame().GetMission())
		{
			NotificationSystem.AddNotificationExtended(5.0, "Ranks", data.param2, ICON_CONNECT);
		}
	}
	
	// ═══════════════════════════════════════════════════════════════════════════
	// QUEST SYSTEM HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════
	
	void ClientReceiveQuests(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string questsStr = data.param1;
		ref array<ref GPDA_QuestDisplayInfo> quests = new array<ref GPDA_QuestDisplayInfo>;
		
		if (questsStr != "")
		{
			TStringArray entries = new TStringArray;
			questsStr.Split(";", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
				{
					TStringArray parts = new TStringArray;
					entry.Split("|", parts);
					
					if (parts.Count() >= 6)
					{
						GPDA_QuestDisplayInfo q = new GPDA_QuestDisplayInfo();
						q.ID = parts[0];
						q.Title = parts[1];
						q.Objective = parts[2];
						q.Progress = parts[3].ToInt();
						q.Required = parts[4].ToInt();
						q.Status = parts[5].ToInt();
						quests.Insert(q);
					}
				}
			}
		}
		
		Print("[GPDA] Received " + quests.Count().ToString() + " active quests");
		
		// Quest system moved to separate system - no longer in main menu
	}
	
	void ClientReceiveAvailable(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string questsStr = data.param1;
		ref array<ref GPDA_QuestAvailableInfo> quests = new array<ref GPDA_QuestAvailableInfo>;
		
		if (questsStr != "")
		{
			TStringArray entries = new TStringArray;
			questsStr.Split(";", entries);
			
			foreach (string entry : entries)
			{
				if (entry != "")
				{
					TStringArray parts = new TStringArray;
					entry.Split("|", parts);
					
					if (parts.Count() >= 4)
					{
						GPDA_QuestAvailableInfo q = new GPDA_QuestAvailableInfo();
						q.ID = parts[0];
						q.Title = parts[1];
						q.Description = parts[2];
						q.Objective = parts[3];
						quests.Insert(q);
					}
				}
			}
		}
		
		Print("[GPDA] Received " + quests.Count().ToString() + " available quests");
		
		// Quest system moved to separate system - no longer in main menu
	}
	
	void QuestResponse(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client) return;
		
		Param2<bool, string> data;
		if (!ctx.Read(data)) return;
		
		if (GetGame().GetMission())
		{
			NotificationSystem.AddNotificationExtended(5.0, "Quest", data.param2, ICON_CONNECT);
		}
	}
}

// Global instance
static ref GPDA_ClientRPC g_GPDA_ClientRPC;
