// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Database RPC Handler (Server-side)
// Handles access checks and basic player database queries
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_DatabaseRPCHandler
{
	protected static ref GPDA_DatabaseRPCHandler m_Instance;
	protected bool m_Initialized;

	static GPDA_DatabaseRPCHandler GetInstance()
	{
		if (!m_Instance)
			m_Instance = new GPDA_DatabaseRPCHandler();
		return m_Instance;
	}

	void Init()
	{
		if (m_Initialized) return;
		if (!GetGame().IsServer()) return;

		GetRPCManager().AddRPC("GPDA_Database", "CheckAccess", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("GPDA_Database", "CheckAdminStatus", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("GPDA_Database", "SearchPlayer", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("GPDA_Database", "IssueLicense", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("GPDA_Database", "RequestDossier", this, SingleplayerExecutionType.Server);

		m_Initialized = true;
		Print("[GPDA] DatabaseRPCHandler initialized");
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// RPC HANDLERS
	// ═══════════════════════════════════════════════════════════════════════════

	void CheckAccess(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;

		string steam64 = sender.GetPlainId();
		string groupName = "Loner";
		bool hasAccess = false;

		if (g_GPDA_GroupManager)
		{
			GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(steam64);
			if (group)
			{
				groupName = group.Name;
				hasAccess = group.DatabaseAccess;
			}
		}

		// Don't allow admins to bypass DatabaseAccess flag for Database tab
		// Admins can use Admin panel, but Database tab requires group DatabaseAccess

		PlayerBase player = GetPlayerByUID(steam64);
		if (!player && sender.GetPlayer())
		{
			player = PlayerBase.Cast(sender.GetPlayer());
		}

		if (player)
		{
			ref Param2<bool, string> response = new Param2<bool, string>(hasAccess, groupName);
			GetRPCManager().SendRPC("GPDA_Database", "ReceiveAccessCheck", response, true, sender, player);
		}
	}
	
	void CheckAdminStatus(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string steam64 = data.param1;
		bool isAdmin = false;
		
		if (g_GPDA_AdminManager)
		{
			isAdmin = g_GPDA_AdminManager.IsAdmin(steam64);
		}
		
		PlayerBase player = GetPlayerByUID(steam64);
		if (!player && sender.GetPlayer())
		{
			player = PlayerBase.Cast(sender.GetPlayer());
		}
		
		if (player)
		{
			ref Param1<bool> response = new Param1<bool>(isAdmin);
			GetRPCManager().SendRPC("GPDA_Database", "ReceiveAdminStatus", response, true, sender, player);
		}
	}

	void SearchPlayer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		if (!HasDatabaseAccess(sender)) return;

		Param1<string> data;
		if (!ctx.Read(data)) return;

		string query = data.param1;

		array<ref GPDA_PlayerProfile> results = new array<ref GPDA_PlayerProfile>;
		if (g_GPDA_PlayerDB)
		{
			results = g_GPDA_PlayerDB.SearchPlayers(query);

			// If nothing found in stored DB, try online players so admins can fetch live data
			if (results.Count() == 0)
			{
				results = g_GPDA_PlayerDB.SearchOnlinePlayers(query);
			}
		}

		string resultString = "";
		foreach (GPDA_PlayerProfile profile : results)
		{
			if (!profile) continue;
			if (resultString != "") resultString += ";";
			string callsign = profile.Callsign;
			if (callsign == "") callsign = profile.Steam64ID;
			resultString += callsign + "|" + profile.Steam64ID;
		}

		PlayerBase player = GetPlayerByUID(sender.GetPlainId());
		if (!player && sender.GetPlayer())
			player = PlayerBase.Cast(sender.GetPlayer());

		if (player)
		{
			ref Param1<string> payload = new Param1<string>(resultString);
			GetRPCManager().SendRPC("GPDA_Database", "ReceiveSearchResults", payload, true, sender, player);
		}
	}

	void IssueLicense(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		if (!HasDatabaseAccess(sender)) return;
		if (!g_GPDA_PlayerDB) return;

		Param2<string, string> data;
		if (!ctx.Read(data)) return;

		string targetSteam64 = data.param1;
		string licenseType = data.param2;

		// Toggle license: if already has -> remove, else add
		bool newValue = true;
		GPDA_PlayerProfile profileForToggle = g_GPDA_PlayerDB.GetProfile(targetSteam64);
		if (profileForToggle && profileForToggle.Licenses)
		{
			if (licenseType == "Hunting")     newValue = !profileForToggle.Licenses.Hunting;
			else if (licenseType == "Fishing") newValue = !profileForToggle.Licenses.Fishing;
			else if (licenseType == "ZoneAccess") newValue = !profileForToggle.Licenses.ZoneAccess;
			else if (licenseType == "Weapons") newValue = !profileForToggle.Licenses.Weapons;
			else if (licenseType == "Trading") newValue = !profileForToggle.Licenses.Trading;
		}

		bool success = g_GPDA_PlayerDB.IssueLicense(targetSteam64, licenseType, newValue);

		PlayerBase player = GetPlayerByUID(sender.GetPlainId());
		if (!player && sender.GetPlayer())
			player = PlayerBase.Cast(sender.GetPlayer());

		if (player)
		{
			string msg;
			if (success)
			{
				if (newValue)
					msg = "License issued: " + licenseType;
				else
					msg = "License revoked: " + licenseType;
			}
			else
			{
				msg = "Failed to update license: " + licenseType;
			}

			ref Param2<bool, string> response = new Param2<bool, string>(success, msg);
			GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveResponse", response, true, sender, player);

			// Refresh dossier for the admin so UI updates to YES/NO immediately
			if (success)
			{
				SendDossierToIdentity(sender, targetSteam64);
			}
		}
	}

	void RequestDossier(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		if (!HasDatabaseAccess(sender)) return;
		if (!g_GPDA_PlayerDB) return;

		Param1<string> data;
		if (!ctx.Read(data)) return;

		string targetSteam64 = data.param1;
		SendDossierToIdentity(sender, targetSteam64);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// HELPERS
	// ═══════════════════════════════════════════════════════════════════════════

	protected bool HasDatabaseAccess(PlayerIdentity sender)
	{
		if (!sender) return false;

		string steam64 = sender.GetPlainId();

		if (g_GPDA_AdminManager && g_GPDA_AdminManager.IsAdmin(steam64))
			return true;

		if (g_GPDA_GroupManager)
		{
			GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(steam64);
			if (group && group.DatabaseAccess)
				return true;
		}

		return false;
	}

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

	// Build and send dossier string to a specific identity
	protected void SendDossierToIdentity(PlayerIdentity identity, string targetSteam64)
	{
		if (!identity || !g_GPDA_PlayerDB) return;

		GPDA_PlayerProfile profile = g_GPDA_PlayerDB.GetProfile(targetSteam64);

		// If profile not found, try to pull live data and auto-register
		if (!profile)
		{
			array<ref GPDA_PlayerProfile> live = g_GPDA_PlayerDB.SearchOnlinePlayers(targetSteam64);
			if (live.Count() > 0)
			{
				profile = live[0];
			}
		}

		// Ensure profile exists
		if (!profile)
		{
			profile = g_GPDA_PlayerDB.CreateProfile(targetSteam64);
		}

		if (!profile) return;

		string callsign = profile.Callsign;
		if (callsign == "") callsign = "-";
		string groupName = profile.GroupName;
		if (groupName == "") groupName = "Loner";
		string bio = profile.Biography;
		if (bio == "") bio = "-";

		int licenseFlags = BuildLicenseFlags(profile);

		string dossierStr = callsign + "|" + profile.Steam64ID + "|" + profile.Age.ToString() + "|" + groupName + "|" + bio + "|" + licenseFlags.ToString();

		PlayerBase player = GetPlayerByUID(identity.GetPlainId());
		if (!player && identity.GetPlayer())
			player = PlayerBase.Cast(identity.GetPlayer());

		if (player)
		{
			ref Param1<string> payload = new Param1<string>(dossierStr);
			GetRPCManager().SendRPC("GPDA_Database", "ReceiveDossier", payload, true, identity, player);
		}
	}

	protected int BuildLicenseFlags(GPDA_PlayerProfile profile)
	{
		if (!profile) return 0;

		int licenseFlags = 0;
		if (profile.Licenses)
		{
			if (profile.Licenses.Hunting) licenseFlags |= 1;
			if (profile.Licenses.Fishing) licenseFlags |= 2;
			if (profile.Licenses.ZoneAccess) licenseFlags |= 4;
			if (profile.Licenses.Weapons) licenseFlags |= 8;
			if (profile.Licenses.Trading) licenseFlags |= 16;
		}
		return licenseFlags;
	}
}

// Global instance
static ref GPDA_DatabaseRPCHandler g_GPDA_DatabaseRPC;

