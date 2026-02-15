// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Ticket RPC Handler (Server-side)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_TicketRPCHandler
{
    protected static ref GPDA_TicketRPCHandler m_Instance;
    protected bool m_Initialized;

    static GPDA_TicketRPCHandler GetInstance()
    {
        if (!m_Instance)
            m_Instance = new GPDA_TicketRPCHandler();
        return m_Instance;
    }

    void Init()
    {
        if (m_Initialized) return;
        
        if (!GetGame().IsServer()) return;

        GetRPCManager().AddRPC("GPDA_Tickets", "CreateTicket", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Tickets", "SendMessage", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Tickets", "RequestMyTickets", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Tickets", "RequestTicketMessages", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Tickets", "AssignTicket", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Tickets", "CloseTicket", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("GPDA_Tickets", "RequestAllTickets", this, SingleplayerExecutionType.Server);

        m_Initialized = true;
        Print("[GPDA] TicketRPCHandler initialized");
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // RPC HANDLERS
    // ═══════════════════════════════════════════════════════════════════════════

    void CreateTicket(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        Param2<string, string> data;
        if (!ctx.Read(data)) return;

        string title = data.param1;
        string description = data.param2;
        string playerUID = sender.GetPlainId();
        string playerName = sender.GetName();

        bool success = g_GPDA_TicketManager.CreateTicket(playerUID, playerName, title, description);

        // Send response
        PlayerBase player = PlayerBase.Cast(sender.GetPlayer());
        if (!player)
        {
            // Fallback to searching all players
            player = GetPlayerByUID(playerUID);
        }

        if (player)
        {
            string ticketMsg;
            if (success)
            {
                ticketMsg = "Ticket created successfully!";
            }
            else
            {
                // Provide specific error message
                int remainingCooldown = g_GPDA_TicketManager.GetRemainingCooldownMinutes(playerUID);
                int openTickets = g_GPDA_TicketManager.GetPlayerOpenTicketCount(playerUID);

                if (remainingCooldown > 0)
                {
                    ticketMsg = "Please wait " + remainingCooldown.ToString() + " minutes before creating another ticket.";
                }
                else if (openTickets >= 5)
                {
                    ticketMsg = "You have reached the maximum of 5 open tickets. Please close some tickets first.";
                }
                else
                {
                    ticketMsg = "Failed to create ticket. Please try again.";
                }
            }

            ref Param2<bool, string> response = new Param2<bool, string>(success, ticketMsg);
            GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveResponse", response, true, sender, player);

            // Send updated tickets list
            if (success)
                SendPlayerTickets(player, sender);
        }
    }

    void SendMessage(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        Param2<string, string> data;
        if (!ctx.Read(data)) return;

        string ticketId = data.param1;
        string message = data.param2;
        string playerUID = sender.GetPlainId();
        string playerName = sender.GetName();

        bool isAdmin = g_GPDA_TicketManager.IsAdmin(playerUID);
        bool success = g_GPDA_TicketManager.AddMessage(ticketId, playerUID, playerName, message, isAdmin);

        PlayerBase player = GetPlayerByUID(playerUID);
        if (player && success)
        {
            // Send updated messages
            SendTicketMessages(player, sender, ticketId);

            // Notify other party
            GPDA_Ticket ticket = g_GPDA_TicketManager.GetTicketById(ticketId);
            if (ticket)
            {
                string otherUID = ticket.GetAssignedAdminUID();
                if (isAdmin) otherUID = ticket.GetPlayerUID();
                if (otherUID != "")
                {
                    PlayerBase otherPlayer = GetPlayerByUID(otherUID);
                    if (otherPlayer && otherPlayer.GetIdentity())
                    {
                        ref Param1<string> notification = new Param1<string>("New message in ticket: " + ticket.GetTitle());
                        GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveNotification", notification, true, otherPlayer.GetIdentity(), otherPlayer);
                    }
                }
            }
        }
    }

    void RequestMyTickets(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        string playerUID = sender.GetPlainId();
        PlayerBase player = GetPlayerByUID(playerUID);
        if (player)
            SendPlayerTickets(player, sender);
    }

    void RequestTicketMessages(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        Param1<string> data;
        if (!ctx.Read(data)) return;

        string ticketId = data.param1;
        string playerUID = sender.GetPlainId();

        PlayerBase player = GetPlayerByUID(playerUID);
        if (player)
            SendTicketMessages(player, sender, ticketId);
    }

    void AssignTicket(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        Param1<string> data;
        if (!ctx.Read(data)) return;

        string ticketId = data.param1;
        string adminUID = sender.GetPlainId();
        string adminName = sender.GetName();

        bool success = g_GPDA_TicketManager.AssignTicket(ticketId, adminUID, adminName);

        PlayerBase player = GetPlayerByUID(adminUID);
        if (player)
        {
            string assignMsg = "Failed";
            if (success) assignMsg = "Ticket assigned!";
            ref Param2<bool, string> response = new Param2<bool, string>(success, assignMsg);
            GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveResponse", response, true, sender, player);
        }
    }

    void CloseTicket(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        Param1<string> data;
        if (!ctx.Read(data)) return;

        string ticketId = data.param1;
        string adminUID = sender.GetPlainId();

        bool success = g_GPDA_TicketManager.CloseTicket(ticketId, adminUID);

        PlayerBase player = GetPlayerByUID(adminUID);
        if (player)
        {
            string closeMsg = "Failed";
            if (success) closeMsg = "Ticket closed!";
            ref Param2<bool, string> response = new Param2<bool, string>(success, closeMsg);
            GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveResponse", response, true, sender, player);
        }
    }

    void RequestAllTickets(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender) return;

        string playerUID = sender.GetPlainId();

        // Only admins can see all tickets
        if (!g_GPDA_TicketManager.IsAdmin(playerUID)) return;

        PlayerBase player = GetPlayerByUID(playerUID);
        if (player)
            SendAllTickets(player, sender);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // SEND DATA TO CLIENT
    // ═══════════════════════════════════════════════════════════════════════════

    void SendPlayerTickets(PlayerBase player, PlayerIdentity identity)
    {
        if (!player || !identity) return;

        string playerUID = identity.GetPlainId();
        array<ref GPDA_Ticket> tickets = g_GPDA_TicketManager.GetPlayerTickets(playerUID);

        // Build tickets string: id|title|status;id|title|status;...
        string ticketsStr = "";
        foreach (GPDA_Ticket t : tickets)
        {
            if (ticketsStr != "") ticketsStr += ";";
            ticketsStr += t.GetId() + "|" + t.GetTitle() + "|" + t.GetStatus();
        }

        ref Param1<string> data = new Param1<string>(ticketsStr);
        GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveMyTickets", data, true, identity, player);
    }

    void SendAllTickets(PlayerBase player, PlayerIdentity identity)
    {
        if (!player || !identity) return;

        array<ref GPDA_Ticket> tickets = g_GPDA_TicketManager.GetOpenTickets();

        // Build tickets string
        string ticketsStr = "";
        foreach (GPDA_Ticket t : tickets)
        {
            if (ticketsStr != "") ticketsStr += ";";
            ticketsStr += t.GetId() + "|" + t.GetTitle() + "|" + t.GetStatus() + "|" + t.GetPlayerName();
        }

        ref Param1<string> data = new Param1<string>(ticketsStr);
        GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveAllTickets", data, true, identity, player);
    }

    void SendTicketMessages(PlayerBase player, PlayerIdentity identity, string ticketId)
    {
        if (!player || !identity) return;

        GPDA_Ticket ticket = g_GPDA_TicketManager.GetTicketById(ticketId);
        if (!ticket) return;

        // Check permission
        string playerUID = identity.GetPlainId();
        bool isAdmin = g_GPDA_TicketManager.IsAdmin(playerUID);
        if (!isAdmin && ticket.GetPlayerUID() != playerUID) return;

        // Build messages string: sender|message|isAdmin;sender|message|isAdmin;...
        string messagesStr = "";
        array<ref GPDA_TicketMessage> messages = ticket.GetMessages();
        foreach (GPDA_TicketMessage msg : messages)
        {
            if (messagesStr != "") messagesStr += ";;";
            string adminFlag = "0";
            if (msg.IsAdmin()) adminFlag = "1";
            messagesStr += msg.GetSenderName() + "|" + msg.GetMessage() + "|" + adminFlag;
        }

        ref Param2<string, string> data = new Param2<string, string>(ticketId, messagesStr);
        GetRPCManager().SendRPC("GPDA_Tickets", "ReceiveTicketMessages", data, true, identity, player);
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // HELPERS
    // ═══════════════════════════════════════════════════════════════════════════

    PlayerBase GetPlayerByUID(string uid)
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
}

// Global instance
static ref GPDA_TicketRPCHandler g_GPDA_TicketRPC;

