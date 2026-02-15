// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Playtime Tracker
// Tracks player online time
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_PlaytimeTracker
{
	protected static ref GPDA_PlaytimeTracker s_Instance;
	protected ref map<string, int> m_SessionStart; // PlayerID -> session start time
	
	static GPDA_PlaytimeTracker GetInstance()
	{
		if (!s_Instance)
			s_Instance = new GPDA_PlaytimeTracker();
		return s_Instance;
	}
	
	void GPDA_PlaytimeTracker()
	{
		m_SessionStart = new map<string, int>;
	}
	
	void OnPlayerConnect(string steamID, string nickname)
	{
		if (!GetGame().IsServer()) return;
		
		int currentTime = GetGame().GetTime() / 1000;
		m_SessionStart.Set(steamID, currentTime);
		
		GPDA_ServerManager.GetInstance().GetOrCreatePlayer(steamID, nickname);
		
		Print("[GPDA] Player connected: " + nickname + " - tracking playtime");
	}
	
	void OnPlayerDisconnect(string steamID)
	{
		if (!GetGame().IsServer()) return;
		
		if (m_SessionStart.Contains(steamID))
		{
			int startTime = m_SessionStart.Get(steamID);
			int currentTime = GetGame().GetTime() / 1000;
			int sessionMinutes = (currentTime - startTime) / 60;
			
			if (sessionMinutes > 0)
			{
				GPDA_ServerManager.GetInstance().AddPlaytime(steamID, sessionMinutes);
			}
			
			m_SessionStart.Remove(steamID);
			Print("[GPDA] Player disconnected - added " + sessionMinutes + " minutes playtime");
		}
	}
}

static ref GPDA_PlaytimeTracker g_GPDA_PlaytimeTracker;

modded class PlayerBase
{
	override void OnConnect()
	{
		super.OnConnect();
		
		if (GetGame().IsServer() && GetIdentity())
		{
			if (!g_GPDA_PlaytimeTracker)
				g_GPDA_PlaytimeTracker = GPDA_PlaytimeTracker.GetInstance();
			
			g_GPDA_PlaytimeTracker.OnPlayerConnect(GetIdentity().GetPlainId(), GetIdentity().GetName());
		}
	}
	
	override void OnDisconnect()
	{
		if (GetGame().IsServer() && GetIdentity())
		{
			if (g_GPDA_PlaytimeTracker)
				g_GPDA_PlaytimeTracker.OnPlayerDisconnect(GetIdentity().GetPlainId());
			if (g_GPDA_RankManager)
				g_GPDA_RankManager.FlushPlayerXPSave();
		}
		
		super.OnDisconnect();
	}
}

