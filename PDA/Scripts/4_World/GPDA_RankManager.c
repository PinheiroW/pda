// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Rank System Manager
// Server-side rank and XP management with REST API integration
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_RankEntry
{
    int Order;
    string Name;
    int RequiredXP;
    string Color;
    string Icon;
    
    void GPDA_RankEntry()
    {
        Order = 0;
        Name = "";
        RequiredXP = 0;
        Color = "#FFFFFF";
        Icon = "";
    }

}

class GPDA_PlayerXPEntry
{
	string UID;
	int XP;
	
	void GPDA_PlayerXPEntry()
	{
		UID = "";
		XP = 0;
	}
}

class GPDA_XPItem
{
    string ClassName;
    int XP;
    
    void GPDA_XPItem()
    {
        ClassName = "";
        XP = 0;
    }
}

class GPDA_XPKill
{
    string ClassName;
    int XP;
    
    void GPDA_XPKill()
    {
        ClassName = "";
        XP = 0;
    }
}

class GPDA_RankConfig
{
    ref array<ref GPDA_RankEntry> Ranks;
    ref array<ref GPDA_XPItem> Items;
    ref array<ref GPDA_XPItem> ItemsHands;
    ref array<ref GPDA_XPItem> ItemsInventory;
    ref array<ref GPDA_XPKill> Kills;
    
    void GPDA_RankConfig()
    {
        Ranks = new array<ref GPDA_RankEntry>;
        Items = new array<ref GPDA_XPItem>;
        ItemsHands = new array<ref GPDA_XPItem>;
        ItemsInventory = new array<ref GPDA_XPItem>;
        Kills = new array<ref GPDA_XPKill>;
    }
}

class GPDA_RankManager
{
    protected static ref GPDA_RankManager m_Instance;
    protected ref GPDA_RankConfig m_Config;
    protected ref map<string, int> m_PlayerXP;              // Steam64 -> XP
    protected ref map<string, bool> m_ProcessedItems;       // Item key -> processed
    protected string m_RankFolder;
    protected string m_ItemPath;
    protected string m_ItemHandsPath;
    protected string m_ItemInventoryPath;
    protected string m_KillPath;
	protected string m_PlayerXPPath;
	protected bool m_PlayerXPDirty;
	protected bool m_PlayerXPSaveScheduled;
    
    // Icon constant
    static string ICON_CONNECT = "set:dayz_gui image:icon_connect";
    
    static GPDA_RankManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new GPDA_RankManager();
        }
        return m_Instance;
    }
    
    void GPDA_RankManager()
    {
        m_PlayerXP = new map<string, int>;
        m_ProcessedItems = new map<string, bool>;
        m_RankFolder = "$profile:Greshnik_PDALITE/Rank/ranks/";
        m_ItemPath = "$profile:Greshnik_PDALITE/Rank/items.json";
        m_ItemHandsPath = "$profile:Greshnik_PDALITE/Rank/items_hands.json";
        m_ItemInventoryPath = "$profile:Greshnik_PDALITE/Rank/items_inventory.json";
        m_KillPath = "$profile:Greshnik_PDALITE/Rank/kills.json";
		m_PlayerXPPath = "$profile:Greshnik_PDALITE/Rank/players_xp.json";
		m_PlayerXPDirty = false;
		m_PlayerXPSaveScheduled = false;
    }
    
    void Init()
    {
        EnsureFolders();
        LoadConfig();
		LoadPlayerXP();
        Print("[GPDA RankManager] Initialized");
    }
    
    void EnsureFolders()
    {
        if (!FileExist("$profile:Greshnik_PDALITE"))
            MakeDirectory("$profile:Greshnik_PDALITE");
        if (!FileExist("$profile:Greshnik_PDALITE/Rank"))
            MakeDirectory("$profile:Greshnik_PDALITE/Rank");
        if (!FileExist(m_RankFolder))
            MakeDirectory(m_RankFolder);
    }
    
    void LoadConfig()
    {
        m_Config = new GPDA_RankConfig();
        
        bool hasExistingRanks = false;
        
        // Try loading ranks from files first
        string fname;
        int attr;
        FindFileHandle fh = FindFile(m_RankFolder + "*.json", fname, attr, 0);
        if (fh)
        {
            bool hasFile = true;
            string current = fname;
            while (hasFile)
            {
                string full = m_RankFolder + current;
                GPDA_RankEntry r = new GPDA_RankEntry();
                JsonFileLoader<GPDA_RankEntry>.JsonLoadFile(full, r);
                if (r)
                {
                    m_Config.Ranks.Insert(r);
                    hasExistingRanks = true;
                }
                hasFile = FindNextFile(fh, current, attr);
            }
            CloseFindFile(fh);
            SortRanks();
        }
        
        // Only add defaults if NO rank files exist (first run)
        if (!hasExistingRanks)
        {
            LoadDefaultConfig();
            // Save defaults so they exist on disk for next time
            SaveRanksOnly();
        }
        
        // items (migration / legacy) — load from disk, DO NOT overwrite with empty defaults
        if (FileExist(m_ItemHandsPath))
        {
            ref array<ref GPDA_XPItem> itemsHands = new array<ref GPDA_XPItem>;
            JsonFileLoader<array<ref GPDA_XPItem>>.JsonLoadFile(m_ItemHandsPath, itemsHands);
            if (itemsHands)
                m_Config.ItemsHands = itemsHands;
        }

        if (FileExist(m_ItemInventoryPath))
        {
            ref array<ref GPDA_XPItem> itemsInv = new array<ref GPDA_XPItem>;
            JsonFileLoader<array<ref GPDA_XPItem>>.JsonLoadFile(m_ItemInventoryPath, itemsInv);
            if (itemsInv)
                m_Config.ItemsInventory = itemsInv;
        }

        if (FileExist(m_ItemPath) && (!m_Config.ItemsInventory || m_Config.ItemsInventory.Count() == 0))
        {
            ref array<ref GPDA_XPItem> itemsLegacy = new array<ref GPDA_XPItem>;
            JsonFileLoader<array<ref GPDA_XPItem>>.JsonLoadFile(m_ItemPath, itemsLegacy);
            if (itemsLegacy)
            {
                m_Config.Items = itemsLegacy;
                m_Config.ItemsInventory = itemsLegacy;
            }
        }
        // kills — only load, never overwrite with empty
        if (FileExist(m_KillPath))
        {
            ref array<ref GPDA_XPKill> kills = new array<ref GPDA_XPKill>;
            JsonFileLoader<array<ref GPDA_XPKill>>.JsonLoadFile(m_KillPath, kills);
            if (kills)
                m_Config.Kills = kills;
        }
        // NOTE: Removed unconditional SaveConfig() — data is only saved on admin edits now
    }
    
    // Save only rank files (used for first-run default creation)
    void SaveRanksOnly()
    {
        EnsureFolders();
        foreach (GPDA_RankEntry r : m_Config.Ranks)
        {
            if (!r) continue;
            string fileName = r.Order.ToString() + ".json";
            string full = m_RankFolder + fileName;
            JsonFileLoader<GPDA_RankEntry>.JsonSaveFile(full, r);
        }
    }
    
    void SaveConfig()
    {
        EnsureFolders();
        // Delete ALL existing rank files first to prevent orphans
        string delName;
        int delAttr;
        FindFileHandle delFH = FindFile(m_RankFolder + "*.json", delName, delAttr, 0);
        if (delFH)
        {
            ref array<string> filesToDelete = new array<string>;
            filesToDelete.Insert(m_RankFolder + delName);
            while (FindNextFile(delFH, delName, delAttr))
                filesToDelete.Insert(m_RankFolder + delName);
            CloseFindFile(delFH);
            foreach (string df : filesToDelete)
                DeleteFile(df);
        }
        // Now write current ranks
        foreach (GPDA_RankEntry r : m_Config.Ranks)
        {
            if (!r) continue;
			string fileName = r.Order.ToString() + ".json";
			string full = m_RankFolder + fileName;
			JsonFileLoader<GPDA_RankEntry>.JsonSaveFile(full, r);
        }
        // items
        if (!m_Config.ItemsHands) m_Config.ItemsHands = new array<ref GPDA_XPItem>;
        if (!m_Config.ItemsInventory) m_Config.ItemsInventory = new array<ref GPDA_XPItem>;
        JsonFileLoader<array<ref GPDA_XPItem>>.JsonSaveFile(m_ItemHandsPath, m_Config.ItemsHands);
        JsonFileLoader<array<ref GPDA_XPItem>>.JsonSaveFile(m_ItemInventoryPath, m_Config.ItemsInventory);
        JsonFileLoader<array<ref GPDA_XPItem>>.JsonSaveFile(m_ItemPath, m_Config.ItemsInventory);
        // kills
        JsonFileLoader<array<ref GPDA_XPKill>>.JsonSaveFile(m_KillPath, m_Config.Kills);
    }
    
    void LoadDefaultConfig()
    {
        // NOTE: m_Config is already created in LoadConfig()
        // Only add default ranks — used only on first run when no rank files exist
        
        // Default ranks
        AddDefaultRank(1, "Новичок", 0, "#808080");
        AddDefaultRank(2, "Выживший", 500, "#00FF00");
        AddDefaultRank(3, "Опытный", 1500, "#0088FF");
        AddDefaultRank(4, "Ветеран", 4000, "#AA00FF");
        AddDefaultRank(5, "Мастер", 9000, "#FF8800");
        AddDefaultRank(6, "Легенда", 16000, "#FF0000");
        
		// XP kills/items are configured via admin panel JSON
    }
    
    protected void AddDefaultRank(int order, string name, int xp, string color)
    {
        GPDA_RankEntry r = new GPDA_RankEntry();
        r.Order = order;
        r.Name = name;
        r.RequiredXP = xp;
        r.Color = color;
        m_Config.Ranks.Insert(r);
    }
    
    protected void AddDefaultKill(string className, int xp)
    {
        GPDA_XPKill k = new GPDA_XPKill();
        k.ClassName = className;
        k.XP = xp;
        m_Config.Kills.Insert(k);
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // XP FUNCTIONS
    // ═══════════════════════════════════════════════════════════════════════════
    
    void AddXP(string playerUID, int amount)
    {
        if (!playerUID || playerUID == "" || amount == 0) return;
        
        int current = GetLocalXP(playerUID);
        current += amount;
        if (current < 0) current = 0;
        m_PlayerXP.Set(playerUID, current);

        Print("[GPDA Rank] Player " + playerUID + " gained " + amount.ToString() + " XP (total: " + current.ToString() + ")");
		ScheduleSavePlayerXP();
    }
    
    int GetLocalXP(string playerUID)
    {
        if (!m_PlayerXP) return 0;
        if (m_PlayerXP.Contains(playerUID))
            return m_PlayerXP.Get(playerUID);
        return 0;
    }
    
    void SetLocalXP(string playerUID, int xp)
    {
        m_PlayerXP.Set(playerUID, xp);
		ScheduleSavePlayerXP();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // RANK FUNCTIONS
    // ═══════════════════════════════════════════════════════════════════════════
    
    GPDA_RankEntry GetRankForXP(int xp)
    {
        if (!m_Config || !m_Config.Ranks || m_Config.Ranks.Count() == 0)
            return null;
        
        GPDA_RankEntry best = null;
        foreach (GPDA_RankEntry r : m_Config.Ranks)
        {
            if (!r) continue;
            if (xp >= r.RequiredXP)
                best = r;
            else
                break;
        }
        return best;
    }
    
    GPDA_RankEntry GetNextRank(int xp)
    {
        if (!m_Config || !m_Config.Ranks) return null;
        
        foreach (GPDA_RankEntry r : m_Config.Ranks)
        {
            if (r && r.RequiredXP > xp)
                return r;
        }
        return null;
    }
    
    string GetRankName(string playerUID)
    {
        int xp = GetLocalXP(playerUID);
        GPDA_RankEntry rank = GetRankForXP(xp);
        if (rank)
            return rank.Name;
        return "Unknown";
    }
    
    int GetNextRankXP(string playerUID)
    {
        int xp = GetLocalXP(playerUID);
        GPDA_RankEntry next = GetNextRank(xp);
        if (next)
            return next.RequiredXP;
        return xp; // Already max rank
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // XP TRIGGERS
    // ═══════════════════════════════════════════════════════════════════════════
    
    // Called when entity is killed
    int GetKillXP(string entityClassName)
    {
        if (!m_Config || !m_Config.Kills) return 0;
        
        foreach (GPDA_XPKill k : m_Config.Kills)
        {
            if (!k) continue;
			// Strict classname match only
			if (k.ClassName == entityClassName)
                return k.XP;
        }
        return 0;
    }

	protected void ScheduleSavePlayerXP()
	{
		if (!GetGame() || !GetGame().IsServer()) return;
		m_PlayerXPDirty = true;
		if (m_PlayerXPSaveScheduled) return;
		m_PlayerXPSaveScheduled = true;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SavePlayerXPInternal, 500, false);
	}

	protected void SavePlayerXPInternal()
	{
		m_PlayerXPSaveScheduled = false;
		if (!m_PlayerXPDirty) return;
		m_PlayerXPDirty = false;
		SavePlayerXP();
	}

	void LoadPlayerXP()
	{
		if (!GetGame() || !GetGame().IsServer()) return;
		if (!m_PlayerXP)
			m_PlayerXP = new map<string, int>;
		
		if (!FileExist(m_PlayerXPPath))
			return;
		
		ref array<ref GPDA_PlayerXPEntry> entries = new array<ref GPDA_PlayerXPEntry>;
		JsonFileLoader<array<ref GPDA_PlayerXPEntry>>.JsonLoadFile(m_PlayerXPPath, entries);
		if (!entries) return;
		
		m_PlayerXP.Clear();
		foreach (GPDA_PlayerXPEntry e : entries)
		{
			if (!e) continue;
			if (e.UID == "") continue;
			m_PlayerXP.Set(e.UID, e.XP);
		}
		Print("[GPDA RankManager] Loaded player XP: " + m_PlayerXP.Count().ToString());
	}

	void SavePlayerXP()
	{
		if (!GetGame() || !GetGame().IsServer()) return;
		EnsureFolders();
		ref array<ref GPDA_PlayerXPEntry> entries = new array<ref GPDA_PlayerXPEntry>;
		if (m_PlayerXP)
		{
			foreach (string uid, int xp : m_PlayerXP)
			{
				GPDA_PlayerXPEntry e = new GPDA_PlayerXPEntry();
				e.UID = uid;
				e.XP = xp;
				entries.Insert(e);
			}
		}
		JsonFileLoader<array<ref GPDA_PlayerXPEntry>>.JsonSaveFile(m_PlayerXPPath, entries);
		Print("[GPDA RankManager] Saved player XP: " + entries.Count().ToString());
	}

	void FlushPlayerXPSave()
	{
		if (!GetGame() || !GetGame().IsServer()) return;
		m_PlayerXPDirty = true;
		SavePlayerXPInternal();
	}
    
    // Called when item is picked up
    int GetItemXP(string itemClassName)
    {
        if (!m_Config || !m_Config.ItemsInventory) return 0;
        
        foreach (GPDA_XPItem i : m_Config.ItemsInventory)
        {
            if (!i) continue;
            if (itemClassName == i.ClassName)
                return i.XP;
        }
        return 0;
    }

    int GetItemXPHands(string itemClassName)
    {
        if (!m_Config || !m_Config.ItemsHands) return 0;
        foreach (GPDA_XPItem i : m_Config.ItemsHands)
        {
            if (!i) continue;
            if (itemClassName == i.ClassName)
                return i.XP;
        }
        return 0;
    }

    int GetItemXPInventory(string itemClassName)
    {
        return GetItemXP(itemClassName);
    }
    
    // Alias for GetKillXP - used by kill tracker
    int GetXPForAnimal(string entityClassName)
    {
        return GetKillXP(entityClassName);
    }
    
    // Track processed items to avoid duplicate XP
    bool IsItemProcessed(string itemKey)
    {
        if (!m_ProcessedItems) return false;
        return m_ProcessedItems.Contains(itemKey) && m_ProcessedItems.Get(itemKey);
    }
    
    void MarkItemProcessed(string itemKey)
    {
        if (!m_ProcessedItems)
            m_ProcessedItems = new map<string, bool>;
        m_ProcessedItems.Set(itemKey, true);
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ADMIN FUNCTIONS
    // ═══════════════════════════════════════════════════════════════════════════
    
    void CreateRank(int order, string name, int requiredXP, string color)
    {
        // Duplicate check by name AND order
        foreach (GPDA_RankEntry existing : m_Config.Ranks)
        {
            if (!existing) continue;
            if (existing.Name == name || existing.Order == order)
            {
                Print("[GPDA Rank] Duplicate rank blocked: name=" + name + " order=" + order.ToString());
                return;
            }
        }
        
        GPDA_RankEntry r = new GPDA_RankEntry();
        r.Order = order;
        r.Name = name;
        r.RequiredXP = requiredXP;
        r.Color = color;
        m_Config.Ranks.Insert(r);
        
        SortRanks();
        SaveConfig();
        
        Print("[GPDA Rank] Created rank: " + name);
    }
    
    void AddXPItem(string itemClass, int xpAmount)
    {
        GPDA_XPItem item = new GPDA_XPItem();
        item.ClassName = itemClass;
        item.XP = xpAmount;
        m_Config.ItemsInventory.Insert(item);
        SaveConfig();
        
        Print("[GPDA Rank] Added XP item: " + itemClass + " = " + xpAmount.ToString() + " XP");
    }

    void SetXPItemHands(string itemClass, int xpAmount)
    {
        if (!m_Config.ItemsHands) m_Config.ItemsHands = new array<ref GPDA_XPItem>;
        if (!SetXPItemInternal(m_Config.ItemsHands, itemClass, xpAmount))
        {
            GPDA_XPItem item = new GPDA_XPItem();
            item.ClassName = itemClass;
            item.XP = xpAmount;
            m_Config.ItemsHands.Insert(item);
        }
        SaveConfig();
    }

    void SetXPItemInventory(string itemClass, int xpAmount)
    {
        if (!m_Config.ItemsInventory) m_Config.ItemsInventory = new array<ref GPDA_XPItem>;
        if (!SetXPItemInternal(m_Config.ItemsInventory, itemClass, xpAmount))
        {
            GPDA_XPItem item = new GPDA_XPItem();
            item.ClassName = itemClass;
            item.XP = xpAmount;
            m_Config.ItemsInventory.Insert(item);
        }
        SaveConfig();
    }

    void RemoveXPItemHands(string itemClass)
    {
        RemoveXPItemInternal(m_Config.ItemsHands, itemClass);
        SaveConfig();
    }

    void RemoveXPItemInventory(string itemClass)
    {
        RemoveXPItemInternal(m_Config.ItemsInventory, itemClass);
        SaveConfig();
    }
    
    void AddXPKill(string entityClass, int xpAmount)
    {
        GPDA_XPKill kill = new GPDA_XPKill();
        kill.ClassName = entityClass;
        kill.XP = xpAmount;
        m_Config.Kills.Insert(kill);
        SaveConfig();
        
        Print("[GPDA Rank] Added XP kill: " + entityClass + " = " + xpAmount.ToString() + " XP");
    }

    void SetXPKill(string entityClass, int xpAmount)
    {
        if (!m_Config.Kills) m_Config.Kills = new array<ref GPDA_XPKill>;
        bool updated = false;
        foreach (GPDA_XPKill k : m_Config.Kills)
        {
            if (!k) continue;
            if (k.ClassName == entityClass)
            {
                k.XP = xpAmount;
                updated = true;
                break;
            }
        }
        if (!updated)
        {
            GPDA_XPKill kill = new GPDA_XPKill();
            kill.ClassName = entityClass;
            kill.XP = xpAmount;
            m_Config.Kills.Insert(kill);
        }
        SaveConfig();
    }

    void RemoveXPKill(string entityClass)
    {
        if (!m_Config.Kills) return;
        for (int i = m_Config.Kills.Count() - 1; i >= 0; i--)
        {
            if (m_Config.Kills[i] && m_Config.Kills[i].ClassName == entityClass)
            {
                m_Config.Kills.Remove(i);
            }
        }
        SaveConfig();
    }

    protected bool SetXPItemInternal(array<ref GPDA_XPItem> arr, string itemClass, int xpAmount)
    {
        if (!arr) return false;
        foreach (GPDA_XPItem i : arr)
        {
            if (!i) continue;
            if (i.ClassName == itemClass)
            {
                i.XP = xpAmount;
                return true;
            }
        }
        return false;
    }

    protected void RemoveXPItemInternal(array<ref GPDA_XPItem> arr, string itemClass)
    {
        if (!arr) return;
        for (int i = arr.Count() - 1; i >= 0; i--)
        {
            if (arr[i] && arr[i].ClassName == itemClass)
                arr.Remove(i);
        }
    }
    
    protected void SortRanks()
    {
        if (!m_Config || !m_Config.Ranks) return;
        
        // Bubble sort by Order
        int n = m_Config.Ranks.Count();
        for (int i = 0; i < n - 1; i++)
        {
            for (int j = 0; j < n - i - 1; j++)
            {
                GPDA_RankEntry a = m_Config.Ranks[j];
                GPDA_RankEntry b = m_Config.Ranks[j + 1];
                if (a && b && a.Order > b.Order)
                {
                    m_Config.Ranks[j] = b;
                    m_Config.Ranks[j + 1] = a;
                }
            }
        }
    }
    
    GPDA_RankConfig GetConfig()
    {
        return m_Config;
    }
    
    array<ref GPDA_RankEntry> GetAllRanks()
    {
        if (m_Config)
            return m_Config.Ranks;
        return null;
    }
    
    array<ref GPDA_XPItem> GetAllXPItems()
    {
        if (m_Config)
            return m_Config.ItemsInventory;
        return null;
    }

    array<ref GPDA_XPItem> GetAllXPItemsHands()
    {
        if (m_Config)
            return m_Config.ItemsHands;
        return null;
    }

    array<ref GPDA_XPItem> GetAllXPItemsInventory()
    {
        if (m_Config)
            return m_Config.ItemsInventory;
        return null;
    }
    
    array<ref GPDA_XPKill> GetAllXPKills()
    {
        if (m_Config)
            return m_Config.Kills;
        return null;
    }

    string BuildXPKillsString()
    {
        string outStr = "";
        if (!m_Config || !m_Config.Kills) return outStr;
        foreach (GPDA_XPKill k : m_Config.Kills)
        {
            if (!k) continue;
            if (outStr != "") outStr += ";";
            outStr += k.ClassName + "|" + k.XP.ToString();
        }
        return outStr;
    }

    string BuildXPItemsString(array<ref GPDA_XPItem> arr)
    {
        string outStr = "";
        if (!arr) return outStr;
        foreach (GPDA_XPItem i : arr)
        {
            if (!i) continue;
            if (outStr != "") outStr += ";";
            outStr += i.ClassName + "|" + i.XP.ToString();
        }
        return outStr;
    }

    void SendXPConfigToPlayer(PlayerBase player)
    {
        if (!player || !player.GetIdentity()) return;
        string killsStr = BuildXPKillsString();
        string handsStr = BuildXPItemsString(m_Config.ItemsHands);
        string invStr = BuildXPItemsString(m_Config.ItemsInventory);
        ref Param3<string, string, string> data = new Param3<string, string, string>(killsStr, handsStr, invStr);
        GetRPCManager().SendRPC("GPDA_Rank", "ReceiveXPConfig", data, true, player.GetIdentity(), player);
    }
    
    // Send player XP notification
    void SendXPNotification(PlayerBase player, int amount, string reason)
    {
        if (!player || !player.GetIdentity()) return;
        
        string uid = player.GetIdentity().GetPlainId();
        int currentXP = GetLocalXP(uid);
        string rankName = GetRankName(uid);
        int nextXP = GetNextRankXP(uid);
        
        string msg = "+" + amount.ToString() + " XP (" + reason + ")\n";
        msg += "Rank: " + rankName + " | XP: " + currentXP.ToString() + "/" + nextXP.ToString();
        
        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5.0, "XP Gained", msg, ICON_CONNECT);
    }
    
    // Send rank data to player via RPC
    void SendRankToPlayer(PlayerBase player)
    {
        if (!player || !player.GetIdentity()) return;
        
        string uid = player.GetIdentity().GetPlainId();
        int xp = GetLocalXP(uid);
        string rankName = GetRankName(uid);
        int nextXP = GetNextRankXP(uid);
        
        GPDA_RankEntry nextRank = GetNextRank(xp);
        string nextRankName = "MAX";
        if (nextRank)
            nextRankName = nextRank.Name;
        
        ref Param4<string, int, int, string> rankData = new Param4<string, int, int, string>(rankName, xp, nextXP, nextRankName);
        GetRPCManager().SendRPC("GPDA_Rank", "ReceivePlayerRank", rankData, true, player.GetIdentity(), player);
    }

    // Import rank config from REST API
    void ImportFromRest(GPDA_RankConfigDTO dto)
    {
        if (!dto || !m_Config) return;

        // Clear existing config
        m_Config.Ranks.Clear();
        m_Config.Items.Clear();
        m_Config.Kills.Clear();

        // Import ranks
        foreach (GPDA_RankDTO rankDto : dto.ranks)
        {
            if (!rankDto || rankDto.name == "") continue;

            GPDA_RankEntry rankEntry = new GPDA_RankEntry();
            rankEntry.Order = rankDto.rank_order;
            rankEntry.Name = rankDto.name;
            rankEntry.RequiredXP = rankDto.required_xp;
            rankEntry.Color = rankDto.color;
            rankEntry.Icon = rankDto.icon;

            m_Config.Ranks.Insert(rankEntry);
            Print("[GPDA] Imported rank from REST: " + rankDto.name);
        }

        // Import XP items
        foreach (GPDA_RankXPItemDTO itemDto : dto.items)
        {
            if (!itemDto || itemDto.item_class == "") continue;

            GPDA_XPItem xpItem = new GPDA_XPItem();
            xpItem.ClassName = itemDto.item_class;
            xpItem.XP = itemDto.xp_amount;

            m_Config.Items.Insert(xpItem);
            Print("[GPDA] Imported XP item from REST: " + itemDto.item_class + " = " + itemDto.xp_amount.ToString() + " XP");
        }

        // Import XP kills
        foreach (GPDA_RankXPKillDTO killDto : dto.kills)
        {
            if (!killDto || killDto.entity_class == "") continue;

            GPDA_XPKill xpKill = new GPDA_XPKill();
            xpKill.ClassName = killDto.entity_class;
            xpKill.XP = killDto.xp_amount;

            m_Config.Kills.Insert(xpKill);
            Print("[GPDA] Imported XP kill from REST: " + killDto.entity_class + " = " + killDto.xp_amount.ToString() + " XP");
        }

        // Save changes
        SaveConfig();
        Print("[GPDA] Rank system imported from REST: " + dto.ranks.Count().ToString() + " ranks, " + dto.items.Count().ToString() + " items, " + dto.kills.Count().ToString() + " kills");
    }

}

// Global instance
static ref GPDA_RankManager g_GPDA_RankManager;
