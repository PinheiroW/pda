class GPDA_AccountRPCHandler
{
	protected static ref GPDA_AccountRPCHandler m_Instance;
	protected bool m_Initialized;

	static GPDA_AccountRPCHandler GetInstance()
	{
		if (!m_Instance)
			m_Instance = new GPDA_AccountRPCHandler();
		return m_Instance;
	}

	void Init()
	{
		if (m_Initialized) return;
		if (!GetGame().IsServer()) return;

		GetRPCManager().AddRPC("GPDA_Accounts", "RequestStatus", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("GPDA_Accounts", "Register", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("GPDA_Accounts", "Login", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("GPDA_Accounts", "Logout", this, SingleplayerExecutionType.Server);

		m_Initialized = true;
		Print("[GPDA] AccountRPCHandler initialized");
	}

	protected PlayerBase GetPlayerByIdentity(PlayerIdentity identity)
	{
		if (!identity) return null;

		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity() && player.GetIdentity().GetPlainId() == identity.GetPlainId())
				return player;
		}
		return null;
	}

	void RequestStatus(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		Print("[GPDA][ACCOUNTS] RequestStatus from: " + sender.GetName() + " (" + sender.GetPlainId() + ")");

		PlayerBase player = GetPlayerByIdentity(sender);
		if (!player) return;

		string steam64 = sender.GetPlainId();
		bool isLoggedIn = false;
		string login = "";

		if (g_GPDA_AccountManager)
		{
			isLoggedIn = g_GPDA_AccountManager.IsLoggedIn(steam64);
			login = g_GPDA_AccountManager.GetLogin(steam64);
		}

		ref Param2<bool, string> data = new Param2<bool, string>(isLoggedIn, login);
		GetRPCManager().SendRPC("GPDA_Accounts", "AuthStatus", data, true, sender, player);
	}

	void Register(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		Print("[GPDA][ACCOUNTS] Register RPC from: " + sender.GetName() + " (" + sender.GetPlainId() + ")");

		PlayerBase player = GetPlayerByIdentity(sender);
		if (!player) return;

		Param2<string, string> data;
		if (!ctx.Read(data))
		{
			Print("[GPDA][ACCOUNTS] Register ctx.Read failed");
			return;
		}

		string login = data.param1;
		string password = data.param2;
		Print("[GPDA][ACCOUNTS] Register data: login='" + login + "' passLen=" + password.Length().ToString());

		string steam64 = sender.GetPlainId();
		bool success = false;
		string msg = "";
		string finalLogin = "";

		if (g_GPDA_AccountManager)
		{
			success = g_GPDA_AccountManager.Register(steam64, login, password, msg);
			finalLogin = g_GPDA_AccountManager.GetLogin(steam64);
		}
		else
		{
			msg = "AccountManager not initialized";
		}

		ref Param3<bool, string, string> resp = new Param3<bool, string, string>(success, msg, finalLogin);
		GetRPCManager().SendRPC("GPDA_Accounts", "AuthResponse", resp, true, sender, player);
	}

	void Login(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		Print("[GPDA][ACCOUNTS] Login RPC from: " + sender.GetName() + " (" + sender.GetPlainId() + ")");

		PlayerBase player = GetPlayerByIdentity(sender);
		if (!player) return;

		Param2<string, string> data;
		if (!ctx.Read(data))
		{
			Print("[GPDA][ACCOUNTS] Login ctx.Read failed");
			return;
		}

		string login = data.param1;
		string password = data.param2;
		Print("[GPDA][ACCOUNTS] Login data: login='" + login + "' passLen=" + password.Length().ToString());

		string steam64 = sender.GetPlainId();
		bool success = false;
		string msg = "";
		string finalLogin = "";

		if (g_GPDA_AccountManager)
		{
			success = g_GPDA_AccountManager.Login(steam64, login, password, msg);
			finalLogin = g_GPDA_AccountManager.GetLogin(steam64);
		}
		else
		{
			msg = "AccountManager not initialized";
		}

		ref Param3<bool, string, string> resp = new Param3<bool, string, string>(success, msg, finalLogin);
		GetRPCManager().SendRPC("GPDA_Accounts", "AuthResponse", resp, true, sender, player);
	}

	void Logout(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server || !sender) return;
		Print("[GPDA][ACCOUNTS] Logout RPC from: " + sender.GetName() + " (" + sender.GetPlainId() + ")");

		PlayerBase player = GetPlayerByIdentity(sender);
		if (!player) return;

		string steam64 = sender.GetPlainId();
		if (g_GPDA_AccountManager)
			g_GPDA_AccountManager.Logout(steam64);

		ref Param3<bool, string, string> resp = new Param3<bool, string, string>(true, "Logged out", "");
		GetRPCManager().SendRPC("GPDA_Accounts", "AuthResponse", resp, true, sender, player);
	}
}

static ref GPDA_AccountRPCHandler g_GPDA_AccountRPC;
