// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Capture & Rank RPC Handler (Server-side)
// FIXED: Added AdminCreateRank, AdminDeleteRank, RequestAllRanks RPCs
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_CaptureRPCHandler
{
    protected static ref GPDA_CaptureRPCHandler m_Instance;
    protected bool m_Initialized;

    static GPDA_CaptureRPCHandler GetInstance()
    {
        if (!m_Instance)
            m_Instance = new GPDA_CaptureRPCHandler();
        return m_Instance;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // XP SETTINGS RPCS
    // ═══════════════════════════════════════════════════════════════════════════

    void RequestXPConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;
        PlayerBase player = GetPlayerByIdentity(sender);
        if (!player) return;
        if (!g_GPDA_RankManager) return;
        g_GPDA_RankManager.SendXPConfigToPlayer(player);
    }

    void AdminSetXPKill(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;

        Param2<string, int> data;
        if (!ctx.Read(data)) return;
        string className = data.param1;
        int xp = data.param2;

        if (g_GPDA_RankManager)
            g_GPDA_RankManager.SetXPKill(className, xp);

        PlayerBase player = GetPlayerByIdentity(sender);
        if (player)
        {
            ref Param2<bool, string> response = new Param2<bool, string>(true, "Kill XP set: " + className + " = " + xp.ToString());
            GetRPCManager().SendRPC("GPDA_Rank", "AdminResponse", response, true, sender, player);
            if (g_GPDA_RankManager)
                g_GPDA_RankManager.SendXPConfigToPlayer(player);
        }
    }

    void AdminRemoveXPKill(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;

        Param1<string> data;
        if (!ctx.Read(data)) return;
        string className = data.param1;

        if (g_GPDA_RankManager)
            g_GPDA_RankManager.RemoveXPKill(className);

        PlayerBase player = GetPlayerByIdentity(sender);
        if (player)
        {
            ref Param2<bool, string> response = new Param2<bool, string>(true, "Kill XP removed: " + className);
            GetRPCManager().SendRPC("GPDA_Rank", "AdminResponse", response, true, sender, player);
            if (g_GPDA_RankManager)
                g_GPDA_RankManager.SendXPConfigToPlayer(player);
        }
    }

    // mode: 0=inventory, 1=hands
    void AdminSetXPItem(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;

        Param3<int, string, int> data;
        if (!ctx.Read(data)) return;
        int mode = data.param1;
        string className = data.param2;
        int xp = data.param3;

        if (g_GPDA_RankManager)
        {
            if (mode == 1)
                g_GPDA_RankManager.SetXPItemHands(className, xp);
            else
                g_GPDA_RankManager.SetXPItemInventory(className, xp);
        }

        PlayerBase player = GetPlayerByIdentity(sender);
        if (player)
        {
            string m;
            if (mode == 1)
                m = "hands";
            else
                m = "inventory";
            ref Param2<bool, string> response = new Param2<bool, string>(true, "Item XP set (" + m + "): " + className + " = " + xp.ToString());
            GetRPCManager().SendRPC("GPDA_Rank", "AdminResponse", response, true, sender, player);
            if (g_GPDA_RankManager)
                g_GPDA_RankManager.SendXPConfigToPlayer(player);
        }
    }

    // mode: 0=inventory, 1=hands
    void AdminRemoveXPItem(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;

        Param2<int, string> data;
        if (!ctx.Read(data)) return;
        int mode = data.param1;
        string className = data.param2;

        if (g_GPDA_RankManager)
        {
            if (mode == 1)
                g_GPDA_RankManager.RemoveXPItemHands(className);
            else
                g_GPDA_RankManager.RemoveXPItemInventory(className);
        }

        PlayerBase player = GetPlayerByIdentity(sender);
        if (player)
        {
            string m;
            if (mode == 1)
                m = "hands";
            else
                m = "inventory";
            ref Param2<bool, string> response = new Param2<bool, string>(true, "Item XP removed (" + m + "): " + className);
            GetRPCManager().SendRPC("GPDA_Rank", "AdminResponse", response, true, sender, player);
            if (g_GPDA_RankManager)
                g_GPDA_RankManager.SendXPConfigToPlayer(player);
        }
    }

    void Init()
    {
        if (m_Initialized) return;
        if (!GetGame().IsServer()) return;

        // Capture RPCs
        GetRPCManager().AddRPC("GPDA_Capture", "RequestCapturePoints", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Capture", "AdminCreateCapture", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Capture", "AdminUpdateCapture", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Capture", "AdminDeleteCapture", this, SingleplayerExecutionType.Server);
        
        // Rank RPCs
        GetRPCManager().AddRPC("GPDA_Rank", "RequestPlayerRank", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Rank", "RequestAllRanks", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Rank", "AdminCreateRank", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Rank", "AdminDeleteRank", this, SingleplayerExecutionType.Server);

        // XP Settings RPCs
        GetRPCManager().AddRPC("GPDA_Rank", "RequestXPConfig", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Rank", "AdminSetXPKill", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Rank", "AdminRemoveXPKill", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Rank", "AdminSetXPItem", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Rank", "AdminRemoveXPItem", this, SingleplayerExecutionType.Server);

        m_Initialized = true;
        Print("[GPDA] CaptureRPCHandler initialized with Rank admin RPCs");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // CAPTURE RPCS
    // ═══════════════════════════════════════════════════════════════════════════

    void RequestCapturePoints(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        PlayerBase player = GetPlayerByIdentity(sender);
        if (player && g_GPDA_CaptureManager)
        {
            g_GPDA_CaptureManager.SendCapturePointsToPlayer(player);
        }
    }

    void AdminCreateCapture(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        // Check admin rights
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid))
        {
            Print("[GPDA] AdminCreateCapture denied for " + uid);
            return;
        }

        Param5<string, vector, float, int, string> data;
        if (!ctx.Read(data)) return;

        string name = data.param1;
        vector pos = data.param2;
        float radius = data.param3;
        int points = data.param4;
        string owner = data.param5;

        if (g_GPDA_CaptureManager)
        {
            bool success = g_GPDA_CaptureManager.AddCapturePoint(name, pos, radius, points, owner, uid);
            
            PlayerBase player = GetPlayerByIdentity(sender);
            if (player)
            {
                string msg = "Failed: Point already exists";
                if (success) msg = "Capture point '" + name + "' created!";
                ref Param2<bool, string> response = new Param2<bool, string>(success, msg);
                GetRPCManager().SendRPC("GPDA_Capture", "AdminResponse", response, true, sender, player);
                
                // Send updated list
                if (success)
                    g_GPDA_CaptureManager.SendCapturePointsToPlayer(player);
            }
        }
    }

    void AdminUpdateCapture(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        string uid = sender.GetPlainId();
        if (!IsAdmin(uid))
        {
            Print("[GPDA] AdminUpdateCapture denied for " + uid);
            return;
        }

        // param1=originalName, param2=newName, param3=pos, param4=radius, param5=points
        // We use two RPCs: first a string for originalName, then the rest
        // Actually use Param5 with originalName packed: "origName|newName"
        Param5<string, vector, float, int, string> data;
        if (!ctx.Read(data)) return;

        // data.param1 = "originalName|newName"
        string packed = data.param1;
        vector pos = data.param2;
        float radius = data.param3;
        int points = data.param4;
        string owner = data.param5;

        array<string> nameParts = new array<string>;
        packed.Split("|", nameParts);
        string originalName = packed;
        string newName = packed;
        if (nameParts.Count() >= 2)
        {
            originalName = nameParts[0];
            newName = nameParts[1];
        }

        if (g_GPDA_CaptureManager)
        {
            bool success = g_GPDA_CaptureManager.UpdateCapturePoint(originalName, newName, pos, radius, points, owner);

            PlayerBase player = GetPlayerByIdentity(sender);
            if (player)
            {
                string msg = "Failed: Zone not found";
                if (success) msg = "Zone '" + newName + "' updated!";
                ref Param2<bool, string> response = new Param2<bool, string>(success, msg);
                GetRPCManager().SendRPC("GPDA_Capture", "AdminResponse", response, true, sender, player);

                if (success)
                    g_GPDA_CaptureManager.SendCapturePointsToPlayer(player);
            }
        }
    }

    void AdminDeleteCapture(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;

        Param1<string> data;
        if (!ctx.Read(data)) return;

        string name = data.param1;

        if (g_GPDA_CaptureManager)
        {
            bool success = g_GPDA_CaptureManager.RemoveCapturePoint(name);
            
            PlayerBase player = GetPlayerByIdentity(sender);
            if (player)
            {
                string msg = "Failed: Point not found";
                if (success) msg = "Capture point '" + name + "' deleted!";
                ref Param2<bool, string> response = new Param2<bool, string>(success, msg);
                GetRPCManager().SendRPC("GPDA_Capture", "AdminResponse", response, true, sender, player);
                
                if (success)
                    g_GPDA_CaptureManager.SendCapturePointsToPlayer(player);
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // RANK RPCS
    // ═══════════════════════════════════════════════════════════════════════════

    void RequestPlayerRank(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        PlayerBase player = GetPlayerByIdentity(sender);
        if (player && g_GPDA_RankManager)
        {
            g_GPDA_RankManager.SendRankToPlayer(player);
        }
    }
    
    // NEW: Request all ranks for admin panel
    void RequestAllRanks(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;
        
        PlayerBase player = GetPlayerByIdentity(sender);
        if (!player) return;
        
        if (!g_GPDA_RankManager) return;
        
        array<ref GPDA_RankEntry> ranks = g_GPDA_RankManager.GetAllRanks();
        if (!ranks) return;
        
        // Send count first
        ref Param1<int> countData = new Param1<int>(ranks.Count());
        GetRPCManager().SendRPC("GPDA_Rank", "ReceiveRanksCount", countData, true, sender, player);
        
        // Send each rank
        foreach (GPDA_RankEntry rank : ranks)
        {
            if (!rank) continue;
            
            ref Param4<int, string, int, string> rankData = new Param4<int, string, int, string>(rank.Order, rank.Name, rank.RequiredXP, rank.Color);
            GetRPCManager().SendRPC("GPDA_Rank", "ReceiveRankData", rankData, true, sender, player);
        }
        
        Print("[GPDA] Sent " + ranks.Count().ToString() + " ranks to admin " + uid);
    }
    
    // NEW: Admin create rank
    void AdminCreateRank(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid))
        {
            Print("[GPDA] AdminCreateRank denied for " + uid);
            return;
        }
        
        Param4<int, string, int, string> data;
        if (!ctx.Read(data)) return;
        
        int order = data.param1;
        string name = data.param2;
        int requiredXP = data.param3;
        string color = data.param4;
        
        if (g_GPDA_RankManager)
        {
            g_GPDA_RankManager.CreateRank(order, name, requiredXP, color);
            
            PlayerBase player = GetPlayerByIdentity(sender);
            if (player)
            {
                ref Param2<bool, string> response = new Param2<bool, string>(true, "Rank '" + name + "' created!");
                GetRPCManager().SendRPC("GPDA_Rank", "AdminResponse", response, true, sender, player);
            }
            
            Print("[GPDA] Admin " + uid + " created rank: " + name);
        }
    }
    
    // NEW: Admin delete rank
    void AdminDeleteRank(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;
        
        string uid = sender.GetPlainId();
        if (!IsAdmin(uid)) return;
        
        Param1<string> data;
        if (!ctx.Read(data)) return;
        
        string rankName = data.param1;
        
        if (g_GPDA_RankManager)
        {
            // Delete rank from config
            GPDA_RankConfig config = g_GPDA_RankManager.GetConfig();
            if (config && config.Ranks)
            {
                for (int i = config.Ranks.Count() - 1; i >= 0; i--)
                {
                    if (config.Ranks[i] && config.Ranks[i].Name == rankName)
                    {
                        config.Ranks.Remove(i);
                        
                        // Save config after deletion
                        g_GPDA_RankManager.SaveConfig();
                        
                        // Also delete from REST API (NOT IMPLEMENTED)
                        // GPDA_RestClient.DeleteRank(rankName);
                        Print("[GPDA] REST API delete not implemented for rank: " + rankName);
                        
                        PlayerBase player = GetPlayerByIdentity(sender);
                        if (player)
                        {
                            ref Param2<bool, string> response = new Param2<bool, string>(true, "Rank '" + rankName + "' deleted!");
                            GetRPCManager().SendRPC("GPDA_Rank", "AdminResponse", response, true, sender, player);
                        }
                        
                        Print("[GPDA] Admin " + uid + " deleted rank: " + rankName);
                        return;
                    }
                }
            }
            
            // Rank not found
            PlayerBase playerFail = GetPlayerByIdentity(sender);
            if (playerFail)
            {
                ref Param2<bool, string> failResponse = new Param2<bool, string>(false, "Rank not found: " + rankName);
                GetRPCManager().SendRPC("GPDA_Rank", "AdminResponse", failResponse, true, sender, playerFail);
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // HELPERS
    // ═══════════════════════════════════════════════════════════════════════════

    protected bool IsAdmin(string uid)
    {
        // Check admin manager first
        if (g_GPDA_AdminManager && g_GPDA_AdminManager.IsAdmin(uid))
            return true;
        
        // Check if player has DatabaseAccess (Military group)
        if (g_GPDA_GroupManager)
        {
            GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(uid);
            if (group && group.DatabaseAccess)
                return true;
        }
        
        // Also check ticket admins
        if (g_GPDA_TicketManager && g_GPDA_TicketManager.IsAdmin(uid))
            return true;
        
        return false;
    }

    PlayerBase GetPlayerByIdentity(PlayerIdentity identity)
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
}

// Global instance
static ref GPDA_CaptureRPCHandler g_GPDA_CaptureRPC;
