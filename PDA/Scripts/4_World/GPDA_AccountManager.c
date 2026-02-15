class GPDA_AccountEntry
{
	string Steam64ID;
	string Login;
	string Password;
	int CreatedAt;

	void GPDA_AccountEntry()
	{
		Steam64ID = "";
		Login = "";
		Password = "";
		CreatedAt = 0;
	}
}

class GPDA_AccountsConfig
{
	ref array<ref GPDA_AccountEntry> Accounts;

	void GPDA_AccountsConfig()
	{
		Accounts = new array<ref GPDA_AccountEntry>;
	}
}

class GPDA_AccountManager
{
	protected static ref GPDA_AccountManager m_Instance;
	protected ref GPDA_AccountsConfig m_Config;

	protected const string m_ProfileDir = "$profile:Greshnik_PDALITE/profiles";
	protected const string m_AccountsPath = "$profile:Greshnik_PDALITE/profiles/accounts.json";

	protected ref map<string, bool> m_LoggedIn;

	static GPDA_AccountManager GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new GPDA_AccountManager();
		}
		return m_Instance;
	}

	void GPDA_AccountManager()
	{
		m_Config = new GPDA_AccountsConfig();
		m_LoggedIn = new map<string, bool>;
	}

	void Init()
	{
		EnsureFolders();
		Load();
		Print("[GPDA] AccountManager initialized with " + m_Config.Accounts.Count().ToString() + " accounts");
	}

	protected void EnsureFolders()
	{
		if (!FileExist("$profile:Greshnik_PDALITE"))
			MakeDirectory("$profile:Greshnik_PDALITE");
		if (!FileExist(m_ProfileDir))
			MakeDirectory(m_ProfileDir);
	}

	protected void Load()
	{
		m_Config = new GPDA_AccountsConfig();

		if (FileExist(m_AccountsPath))
		{
			JsonFileLoader<GPDA_AccountsConfig>.JsonLoadFile(m_AccountsPath, m_Config);
			if (!m_Config || !m_Config.Accounts)
				m_Config = new GPDA_AccountsConfig();
		}
		else
		{
			Save();
		}
	}

	protected void Save()
	{
		EnsureFolders();
		JsonFileLoader<GPDA_AccountsConfig>.JsonSaveFile(m_AccountsPath, m_Config);
	}

	protected GPDA_AccountEntry GetBySteam64(string steam64)
	{
		if (!m_Config || !m_Config.Accounts) return null;
		foreach (GPDA_AccountEntry acc : m_Config.Accounts)
		{
			if (acc && acc.Steam64ID == steam64)
				return acc;
		}
		return null;
	}

	protected GPDA_AccountEntry GetByLogin(string login)
	{
		if (!m_Config || !m_Config.Accounts) return null;
		string needle = login;
		needle.Trim();
		needle.ToLower();
		foreach (GPDA_AccountEntry acc : m_Config.Accounts)
		{
			if (!acc) continue;
			string l = acc.Login;
			l.Trim();
			l.ToLower();
			if (l == needle)
				return acc;
		}
		return null;
	}

	bool HasAccount(string steam64)
	{
		return GetBySteam64(steam64) != null;
	}

	bool IsLoggedIn(string steam64)
	{
		if (!m_LoggedIn) m_LoggedIn = new map<string, bool>;
		if (!m_LoggedIn.Contains(steam64))
			return false;
		return m_LoggedIn.Get(steam64);
	}

	string GetLogin(string steam64)
	{
		GPDA_AccountEntry acc = GetBySteam64(steam64);
		if (acc) return acc.Login;
		return "";
	}

	string GetDisplayName(string steam64, string fallback)
	{
		if (IsLoggedIn(steam64))
		{
			string login = GetLogin(steam64);
			if (login != "")
				return login;
		}
		return fallback;
	}

	bool Register(string steam64, string login, string password, out string error)
	{
		error = "";
		if (steam64 == "")
		{
			error = "Invalid Steam64";
			return false;
		}

		string cleanLogin = login;
		cleanLogin.Trim();
		if (cleanLogin == "")
		{
			error = "Login is empty";
			return false;
		}
		if (cleanLogin.Length() < 3 || cleanLogin.Length() > 20)
		{
			error = "Login length must be 3-20";
			return false;
		}

		string cleanPass = password;
		cleanPass.Trim();
		if (cleanPass.Length() < 3)
		{
			error = "Password too short";
			return false;
		}

		GPDA_AccountEntry existingSteam = GetBySteam64(steam64);
		if (existingSteam)
		{
			error = "Account already exists";
			return false;
		}

		GPDA_AccountEntry existingLogin = GetByLogin(cleanLogin);
		if (existingLogin)
		{
			error = "Login already taken";
			return false;
		}

		GPDA_AccountEntry acc = new GPDA_AccountEntry();
		acc.Steam64ID = steam64;
		acc.Login = cleanLogin;
		acc.Password = password;
		acc.CreatedAt = GetGame().GetTime();
		m_Config.Accounts.Insert(acc);
		Save();

		m_LoggedIn.Set(steam64, true);
		return true;
	}

	bool Login(string steam64, string login, string password, out string error)
	{
		error = "";
		GPDA_AccountEntry acc = GetBySteam64(steam64);
		if (!acc)
		{
			error = "Account not found";
			return false;
		}

		string cleanLogin = login;
		cleanLogin.Trim();
		string accLogin = acc.Login;
		accLogin.Trim();
		if (cleanLogin != accLogin)
		{
			error = "Wrong login";
			return false;
		}

		if (password != acc.Password)
		{
			error = "Wrong password";
			return false;
		}

		m_LoggedIn.Set(steam64, true);
		return true;
	}

	void Logout(string steam64)
	{
		if (!m_LoggedIn) return;
		m_LoggedIn.Set(steam64, false);
	}

	string ResolvePlayerIdByName(string nameOrLogin)
	{
		if (!m_Config || !m_Config.Accounts) return "";
		string needle = nameOrLogin;
		needle.Trim();
		needle.ToLower();
		foreach (GPDA_AccountEntry acc : m_Config.Accounts)
		{
			if (!acc) continue;
			string l = acc.Login;
			l.Trim();
			l.ToLower();
			if (l == needle)
				return acc.Steam64ID;
		}
		return "";
	}
}

static ref GPDA_AccountManager g_GPDA_AccountManager;
