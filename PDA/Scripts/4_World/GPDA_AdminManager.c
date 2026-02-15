class GPDA_AdminData
{
    string steam64;
    string name;
    string level;

    void GPDA_AdminData(string steam64Id = "", string adminName = "", string adminLevel = "admin")
    {
        steam64 = steam64Id;
        name = adminName;
        level = adminLevel;
    }
}

class GPDA_AdminManager
{
    protected static ref GPDA_AdminManager m_Instance;
    protected ref array<ref GPDA_AdminData> m_Admins;
    protected string m_ProfileDir;
    protected string m_AdminsPath;

    static GPDA_AdminManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new GPDA_AdminManager();
        }
        return m_Instance;
    }

    void GPDA_AdminManager()
    {
        m_Admins = new array<ref GPDA_AdminData>();
        m_ProfileDir = "$profile:Greshnik_PDALITE/";
        m_AdminsPath = m_ProfileDir + "admins.json";
    }

    void Init()
    {
        EnsureFolders();
        LoadAdmins();

        // Add default admin if no admins exist
        if (m_Admins.Count() == 0)
        {
            AddDefaultAdmin();
            SaveAdmins();
        }

        Print("[GPDA] AdminManager initialized with " + m_Admins.Count() + " admins");
    }

    protected void EnsureFolders()
    {
        if (!FileExist("$profile:Greshnik_PDALITE"))
        {
            MakeDirectory("$profile:Greshnik_PDALITE");
        }
    }

    protected void LoadAdmins()
    {
        m_Admins.Clear();

        if (!FileExist(m_AdminsPath))
        {
            Print("[GPDA] admins.json not found, will create default");
            return;
        }

        FileHandle file = OpenFile(m_AdminsPath, FileMode.READ);
        if (file == 0)
        {
            Print("[GPDA] Failed to open admins.json");
            return;
        }

        string line;
        while (FGets(file, line) > 0)
        {
            line.Trim();

            // Skip comments and empty lines
            if (line == "" || line.IndexOf("#") == 0) continue;

            // Parse JSON or simple format: steam64|name|level
            string steam64 = "";
            string name = "";
            string level = "admin";

            if (line.IndexOf("{") == 0)
            {
                // JSON format - simple parsing

                // Extract steam64
                int steamStart = line.IndexOf("\"steam64\":\"");
                if (steamStart >= 0)
                {
                    steamStart += 11; // Skip "\"steam64\":\""
                    string steamRemaining = line.Substring(steamStart, line.Length() - steamStart);
                    int steamEnd = steamRemaining.IndexOf("\"");
                    if (steamEnd > 0)
                    {
                        steam64 = steamRemaining.Substring(0, steamEnd);
                    }
                }

                // Extract name
                int nameStart = line.IndexOf("\"name\":\"");
                if (nameStart >= 0)
                {
                    nameStart += 8; // Skip "\"name\":\""
                    string nameRemaining = line.Substring(nameStart, line.Length() - nameStart);
                    int nameEnd = nameRemaining.IndexOf("\"");
                    if (nameEnd > 0)
                    {
                        name = nameRemaining.Substring(0, nameEnd);
                    }
                }

                // Extract level
                int levelStart = line.IndexOf("\"level\":\"");
                if (levelStart >= 0)
                {
                    levelStart += 9; // Skip "\"level\":\""
                    string levelRemaining = line.Substring(levelStart, line.Length() - levelStart);
                    int levelEnd = levelRemaining.IndexOf("\"");
                    if (levelEnd > 0)
                    {
                        level = levelRemaining.Substring(0, levelEnd);
                    }
                }

                if (steam64 != "")
                {
                    m_Admins.Insert(new GPDA_AdminData(steam64, name, level));
                }
            }
            else
            {
                // Simple format: steam64|name|level
                array<string> parts = new array<string>();
                line.Split("|", parts);

                if (parts.Count() >= 1)
                {
                    steam64 = parts[0];
                    name = "";
                    if (parts.Count() > 1)
                        name = parts[1];
                    level = "admin";
                    if (parts.Count() > 2)
                        level = parts[2];

                    m_Admins.Insert(new GPDA_AdminData(steam64, name, level));
                }
            }
        }

        CloseFile(file);
        Print("[GPDA] Loaded " + m_Admins.Count() + " admins from " + m_AdminsPath);
    }

    protected void SaveAdmins()
    {
        FileHandle file = OpenFile(m_AdminsPath, FileMode.WRITE);
        if (file == 0)
        {
            Print("[GPDA] Failed to create admins.json");
            return;
        }

        FPrintln(file, "# Greshnik PDA LITE - Admin Configuration");
        FPrintln(file, "# Format: steam64|name|level");
        FPrintln(file, "# Example: 76561198002174265|Greshnik|superadmin");
        FPrintln(file, "");

        foreach (GPDA_AdminData admin : m_Admins)
        {
            if (admin)
            {
                string line = admin.steam64 + "|" + admin.name + "|" + admin.level;
                FPrintln(file, line);
            }
        }

        CloseFile(file);
        Print("[GPDA] Saved " + m_Admins.Count() + " admins to " + m_AdminsPath);
    }

    protected void AddDefaultAdmin()
    {
        // Add the user's SteamID as superadmin
        GPDA_AdminData defaultAdmin = new GPDA_AdminData("76561198002174265", "Greshnik", "superadmin");
        m_Admins.Insert(defaultAdmin);
        Print("[GPDA] Added default admin: Greshnik (76561198002174265)");
    }

    bool IsAdmin(string steam64)
    {
        foreach (GPDA_AdminData admin : m_Admins)
        {
            if (admin && admin.steam64 == steam64)
            {
                return true;
            }
        }
        return false;
    }

    bool IsSuperAdmin(string steam64)
    {
        foreach (GPDA_AdminData admin : m_Admins)
        {
            if (admin && admin.steam64 == steam64 && admin.level == "superadmin")
            {
                return true;
            }
        }
        return false;
    }

    array<ref GPDA_AdminData> GetAllAdmins()
    {
        return m_Admins;
    }

    bool AddAdmin(string steam64, string name = "", string level = "admin")
    {
        if (IsAdmin(steam64))
        {
            return false; // Already exists
        }

        GPDA_AdminData newAdmin = new GPDA_AdminData(steam64, name, level);
        m_Admins.Insert(newAdmin);
        SaveAdmins();
        Print("[GPDA] Added admin: " + name + " (" + steam64 + ") level: " + level);
        return true;
    }

    bool RemoveAdmin(string steam64)
    {
        for (int i = 0; i < m_Admins.Count(); i++)
        {
            if (m_Admins[i] && m_Admins[i].steam64 == steam64)
            {
                string name = m_Admins[i].name;
                m_Admins.Remove(i);
                SaveAdmins();
                Print("[GPDA] Removed admin: " + name + " (" + steam64 + ")");
                return true;
            }
        }
        return false;
    }
}

// Global instance
static ref GPDA_AdminManager g_GPDA_AdminManager;
