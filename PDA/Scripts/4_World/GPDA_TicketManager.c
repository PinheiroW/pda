// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Ticket Manager (Server-side)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_TicketManager
{
    protected static ref GPDA_TicketManager m_Instance;
    protected ref array<ref GPDA_Ticket> m_Tickets;
    protected ref array<string> m_AdminUIDs;
    protected ref map<string, int> m_PlayerCooldowns;

    protected string m_ProfileDir;
    protected string m_SettingsPath;
    protected string m_TicketsPath;

    protected int m_MaxTicketsPerPlayer;
    protected int m_TicketCooldownMinutes;
    protected int m_MaxMessageLength;

    void GPDA_TicketManager()
    {
        // profile/json persistence
        m_ProfileDir = "$profile:Greshnik_PDALITE/Tickets";
        m_SettingsPath = "$profile:Greshnik_PDALITE/Tickets/admins.txt";
        m_TicketsPath = "$profile:Greshnik_PDALITE/Tickets/tickets.txt";

        m_Tickets = new array<ref GPDA_Ticket>;
        m_AdminUIDs = new array<string>;
        m_PlayerCooldowns = new map<string, int>;
        m_MaxTicketsPerPlayer = 5;
        m_TicketCooldownMinutes = 5;
        m_MaxMessageLength = 500;
    }

    static GPDA_TicketManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new GPDA_TicketManager();
        }
        return m_Instance;
    }

    void Init()
    {
        EnsureFolders();
        LoadSettings();
        LoadTickets();
        Print("[GPDA] TicketManager initialized with " + m_Tickets.Count() + " tickets, " + m_AdminUIDs.Count() + " admins");
    }

    protected void EnsureFolders()
    {
        if (!FileExist("$profile:Greshnik_PDALITE"))
            MakeDirectory("$profile:Greshnik_PDALITE");
        if (!FileExist(m_ProfileDir))
            MakeDirectory(m_ProfileDir);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // SETTINGS
    // ═══════════════════════════════════════════════════════════════════════════

    void LoadSettings()
    {
        m_AdminUIDs.Clear();

        if (!FileExist(m_SettingsPath))
        {
            CreateDefaultSettings();
            return;
        }

        FileHandle file = OpenFile(m_SettingsPath, FileMode.READ);
        if (file == 0)
        {
            CreateDefaultSettings();
            return;
        }

        string line;
        while (FGets(file, line) > 0)
        {
            line = line.Trim();
            if (line.Length() > 0 && line.IndexOf("#") != 0)
            {
                m_AdminUIDs.Insert(line);
            }
        }
        CloseFile(file);

        if (m_AdminUIDs.Count() == 0)
        {
            CreateDefaultSettings();
        }
    }

    void CreateDefaultSettings()
    {
        m_AdminUIDs.Clear();
        m_AdminUIDs.Insert("76561198002174265"); // Default admin

        FileHandle file = OpenFile(m_SettingsPath, FileMode.WRITE);
        if (file != 0)
        {
            FPrintln(file, "# PDA Ticket System - Admin Steam64 IDs");
            FPrintln(file, "# Add one Steam64 ID per line");
            FPrintln(file, "76561198002174265");
            CloseFile(file);
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // TICKET MANAGEMENT
    // ═══════════════════════════════════════════════════════════════════════════

    bool CreateTicket(string playerUID, string playerName, string title, string description)
    {
        // Check cooldown (not for admins)
        if (!IsAdmin(playerUID))
        {
            if (m_PlayerCooldowns.Contains(playerUID))
            {
                int lastTime = m_PlayerCooldowns.Get(playerUID);
                int cooldownMs = m_TicketCooldownMinutes * 60 * 1000;
                if (GetGame().GetTime() - lastTime < cooldownMs)
                {
                    return false;
                }
            }
        }

        // Check max tickets
        int playerTicketCount = 0;
        foreach (GPDA_Ticket t : m_Tickets)
        {
            if (t.GetPlayerUID() == playerUID && t.GetStatus() != "closed")
                playerTicketCount++;
        }

        if (playerTicketCount >= m_MaxTicketsPerPlayer)
            return false;

        // Create ticket
        GPDA_Ticket newTicket = new GPDA_Ticket(playerUID, playerName, title, description);
        m_Tickets.Insert(newTicket);

        // Set cooldown
        m_PlayerCooldowns.Set(playerUID, GetGame().GetTime());

        SaveTickets();
        
        Print("[GPDA] Ticket created: " + newTicket.GetId() + " by " + playerName);

        return true;
    }

    bool AssignTicket(string ticketId, string adminUID, string adminName)
    {
        if (!IsAdmin(adminUID))
            return false;

        GPDA_Ticket ticket = GetTicketById(ticketId);
        if (!ticket || ticket.GetStatus() == "closed")
            return false;

        ticket.AssignToAdmin(adminUID, adminName);
        SaveTickets();
        
        Print("[GPDA] Ticket " + ticketId + " assigned to " + adminName);

        return true;
    }

    bool CloseTicket(string ticketId, string adminUID)
    {
        if (!IsAdmin(adminUID))
            return false;

        GPDA_Ticket ticket = GetTicketById(ticketId);
        if (!ticket)
            return false;

        ticket.CloseTicket();
        SaveTickets();
        
        Print("[GPDA] Ticket " + ticketId + " closed");

        return true;
    }

    bool DeleteTicket(string ticketId, string adminUID)
    {
        if (!IsAdmin(adminUID))
            return false;

        for (int i = 0; i < m_Tickets.Count(); i++)
        {
            if (m_Tickets[i].GetId() == ticketId)
            {
                m_Tickets.Remove(i);
                SaveTickets();
                Print("[GPDA] Ticket " + ticketId + " deleted");
                return true;
            }
        }
        return false;
    }

    bool AddMessage(string ticketId, string senderUID, string senderName, string message, bool isAdmin)
    {
        GPDA_Ticket ticket = GetTicketById(ticketId);
        if (!ticket || ticket.GetStatus() == "closed")
            return false;

        // Check permissions
        if (!isAdmin && ticket.GetPlayerUID() != senderUID)
            return false;

        if (isAdmin && !IsAdmin(senderUID))
            return false;

        // Trim message
        if (message.Length() > m_MaxMessageLength)
            message = message.Substring(0, m_MaxMessageLength);

        ticket.AddMessage(senderUID, senderName, message, isAdmin);
        SaveTickets();
        
        return true;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // GETTERS
    // ═══════════════════════════════════════════════════════════════════════════

    GPDA_Ticket GetTicketById(string ticketId)
    {
        foreach (GPDA_Ticket t : m_Tickets)
        {
            if (t.GetId() == ticketId)
                return t;
        }
        return null;
    }

    array<ref GPDA_Ticket> GetPlayerTickets(string playerUID)
    {
        array<ref GPDA_Ticket> result = new array<ref GPDA_Ticket>;
        foreach (GPDA_Ticket t : m_Tickets)
        {
            if (t.GetPlayerUID() == playerUID)
                result.Insert(t);
        }
        return result;
    }

    array<ref GPDA_Ticket> GetAllTickets()
    {
        return m_Tickets;
    }

    array<ref GPDA_Ticket> GetOpenTickets()
    {
        array<ref GPDA_Ticket> result = new array<ref GPDA_Ticket>;
        foreach (GPDA_Ticket t : m_Tickets)
        {
            if (t.GetStatus() != "closed")
                result.Insert(t);
        }
        return result;
    }

    bool IsAdmin(string playerUID)
    {
        // Check admin manager first
        if (g_GPDA_AdminManager && g_GPDA_AdminManager.IsAdmin(playerUID))
        {
            return true;
        }

        // Also check if player has DatabaseAccess in groups
        if (g_GPDA_GroupManager)
        {
            GPDA_GroupData group = g_GPDA_GroupManager.GetPlayerGroup(playerUID);
            if (group && group.DatabaseAccess)
                return true;
        }

        return false;
    }

    int GetRemainingCooldownMinutes(string playerUID)
    {
        if (IsAdmin(playerUID) || !m_PlayerCooldowns.Contains(playerUID))
            return 0;

        int lastTime = m_PlayerCooldowns.Get(playerUID);
        int cooldownMs = m_TicketCooldownMinutes * 60 * 1000;
        int elapsedMs = GetGame().GetTime() - lastTime;

        if (elapsedMs >= cooldownMs)
            return 0;

        return ((cooldownMs - elapsedMs) / 60000) + 1;
    }

    int GetPlayerOpenTicketCount(string playerUID)
    {
        int count = 0;
        foreach (GPDA_Ticket t : m_Tickets)
        {
            if (t.GetPlayerUID() == playerUID && t.GetStatus() != "closed")
                count++;
        }
        return count;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // PERSISTENCE
    // ═══════════════════════════════════════════════════════════════════════════

    void SaveTickets()
    {
        FileHandle file = OpenFile(m_TicketsPath, FileMode.WRITE);
        if (file == 0) return;

        foreach (GPDA_Ticket t : m_Tickets)
        {
            FPrintln(file, "TICKET_START");
            FPrintln(file, "id=" + t.GetId());
            FPrintln(file, "playerUID=" + t.GetPlayerUID());
            FPrintln(file, "playerName=" + t.GetPlayerName());
            FPrintln(file, "title=" + t.GetTitle());
            FPrintln(file, "description=" + t.GetDescription());
            FPrintln(file, "status=" + t.GetStatus());
            FPrintln(file, "assignedAdminUID=" + t.GetAssignedAdminUID());
            FPrintln(file, "assignedAdminName=" + t.GetAssignedAdminName());
            FPrintln(file, "createdTime=" + t.GetCreatedTime().ToString());
            FPrintln(file, "lastUpdateTime=" + t.GetLastUpdateTime().ToString());

            array<ref GPDA_TicketMessage> messages = t.GetMessages();
            FPrintln(file, "messageCount=" + messages.Count().ToString());

            foreach (GPDA_TicketMessage msg : messages)
            {
                FPrintln(file, "MSG_START");
                FPrintln(file, "senderUID=" + msg.GetSenderUID());
                FPrintln(file, "senderName=" + msg.GetSenderName());
                FPrintln(file, "message=" + msg.GetMessage());
                string isAdminStr = "0";
                if (msg.IsAdmin()) isAdminStr = "1";
                FPrintln(file, "isAdmin=" + isAdminStr);
                FPrintln(file, "timestamp=" + msg.GetTimestamp().ToString());
                FPrintln(file, "MSG_END");
            }

            FPrintln(file, "TICKET_END");
        }

        CloseFile(file);
    }

    void LoadTickets()
    {
        m_Tickets.Clear();

        if (!FileExist(m_TicketsPath)) return;

        FileHandle file = OpenFile(m_TicketsPath, FileMode.READ);
        if (file == 0) return;

        string line;
        string ticketId, playerUID, playerName, title, description, status;
        string assignedAdminUID, assignedAdminName;
        int createdTime, lastUpdateTime;

        string msgSenderUID, msgSenderName, msgText;
        bool msgIsAdmin;
        int msgTimestamp;

        ref array<ref Param5<string, string, string, bool, int>> messageBuffer = new array<ref Param5<string, string, string, bool, int>>;
        GPDA_Ticket currentTicket = null;

        while (FGets(file, line) > 0)
        {
            line = line.Trim();

            if (line == "TICKET_START")
            {
                messageBuffer.Clear();
                ticketId = ""; playerUID = ""; playerName = ""; title = ""; description = ""; status = "open";
                assignedAdminUID = ""; assignedAdminName = "";
                createdTime = 0; lastUpdateTime = 0;
            }
            else if (line == "TICKET_END")
            {
                if (playerUID.Length() > 0 && title.Length() > 0)
                {
                    currentTicket = new GPDA_Ticket(playerUID, playerName, title, description);
                    currentTicket.SetId(ticketId);
                    currentTicket.SetStatusDirect(status);
                    currentTicket.SetAssignedAdmin(assignedAdminUID, assignedAdminName);
                    currentTicket.SetTimes(createdTime, lastUpdateTime);

                    foreach (Param5<string, string, string, bool, int> msgData : messageBuffer)
                    {
                        currentTicket.AddMessageDirect(msgData.param1, msgData.param2, msgData.param3, msgData.param4, msgData.param5);
                    }
                    messageBuffer.Clear();
                    m_Tickets.Insert(currentTicket);
                }
            }
            else if (line == "MSG_START")
            {
                msgSenderUID = ""; msgSenderName = ""; msgText = ""; msgIsAdmin = false; msgTimestamp = 0;
            }
            else if (line == "MSG_END")
            {
                if (msgSenderUID.Length() > 0)
                {
                    messageBuffer.Insert(new Param5<string, string, string, bool, int>(msgSenderUID, msgSenderName, msgText, msgIsAdmin, msgTimestamp));
                }
            }
            else if (line.IndexOf("=") > 0)
            {
                int eqPos = line.IndexOf("=");
                string key = line.Substring(0, eqPos);
                string value = line.Substring(eqPos + 1, line.Length() - eqPos - 1);

                if (key == "id") ticketId = value;
                else if (key == "playerUID") playerUID = value;
                else if (key == "playerName") playerName = value;
                else if (key == "title") title = value;
                else if (key == "description") description = value;
                else if (key == "status") status = value;
                else if (key == "assignedAdminUID") assignedAdminUID = value;
                else if (key == "assignedAdminName") assignedAdminName = value;
                else if (key == "createdTime") createdTime = value.ToInt();
                else if (key == "lastUpdateTime") lastUpdateTime = value.ToInt();
                else if (key == "senderUID") msgSenderUID = value;
                else if (key == "senderName") msgSenderName = value;
                else if (key == "message") msgText = value;
                else if (key == "isAdmin") msgIsAdmin = (value == "1");
                else if (key == "timestamp") msgTimestamp = value.ToInt();
            }
        }

        CloseFile(file);
    }
}

// Global instance
static ref GPDA_TicketManager g_GPDA_TicketManager;

