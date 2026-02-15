// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Ticket Tool
// Interface for managing support tickets
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_TicketToolMenu extends UIScriptedMenu
{
	// New layout filename
	protected const string TICKET_LAYOUT_PATH = "Greshnik_PDALITE/GUI/Layouts/ticket_tools.layout";
	
	// Widgets
	protected Widget m_TicketMainFrame;
	protected TextListboxWidget m_TicketList;
	protected TextWidget m_TicketTitle;
	protected TextWidget m_TicketDescription;
	protected MultilineEditBoxWidget m_InputResponse;
	protected ButtonWidget m_BtnReply;
	protected ButtonWidget m_BtnClose;
	protected ButtonWidget m_BtnRefresh;
	protected ref array<ref GPDA_TicketMessageInfo> m_CurrentMessages;
	
	protected ref array<ref GPDA_TicketInfo> m_TicketsData;
	protected string m_SelectedTicketId;
	
	// Static instance
	private static ref GPDA_TicketToolMenu s_Instance;
	
	static GPDA_TicketToolMenu GetInstance()
	{
		if (!s_Instance)
		{
			s_Instance = new GPDA_TicketToolMenu();
		}
		return s_Instance;
	}
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets(TICKET_LAYOUT_PATH);
		
		if (!layoutRoot)
		{
			Print("[GPDA] ERROR: Failed to load ticket tool layout!");
			return null;
		}
		
		// Main frame (not critical)
		m_TicketMainFrame = layoutRoot.FindAnyWidget("TicketTools_Root");
		
		// Map widgets
		m_TicketList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("TicketList"));
		m_TicketTitle = TextWidget.Cast(layoutRoot.FindAnyWidget("TicketTitle"));
		m_TicketDescription = TextWidget.Cast(layoutRoot.FindAnyWidget("TicketDescription"));
		m_InputResponse = MultilineEditBoxWidget.Cast(layoutRoot.FindAnyWidget("Input_Response"));
		m_BtnReply = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Btn_Reply"));
		m_BtnClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Btn_CloseTicket"));
		m_BtnRefresh = ButtonWidget.Cast(layoutRoot.FindAnyWidget("Btn_Refresh"));
		
		m_CurrentMessages = new array<ref GPDA_TicketMessageInfo>;
		
		Print("[GPDA] Ticket tool menu loaded successfully!");
		return layoutRoot;
	}
	
	override void OnShow()
	{
		super.OnShow();
		// Lock player and show cursor while menu is open
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
		GetGame().GetUIManager().ShowCursor(true);
		GetGame().GetInput().ChangeGameFocus(1);
		Print("[GPDA] Ticket tool menu opened");
	}
	
	override void OnHide()
	{
		super.OnHide();
		// Restore controls when closing
		MissionBase mission = GetGame().GetMission();
		if (mission)
			mission.PlayerControlEnable(true);
		
		UIManager ui = GetGame().GetUIManager();
		if (ui)
			ui.ShowCursor(false);
		
		Input input = GetGame().GetInput();
		if (input)
			input.ResetGameFocus();
		
		Print("[GPDA] Ticket tool menu closed");
	}
	
	// No buttons to handle; rely on ESC/back
	
	void OpenTicketTool()
	{
		if (IsOpen())
			return;
			
		GetGame().GetUIManager().ShowScriptedMenu(this, null);
		RequestAllTickets();
	}
	
	void CloseTicketTool()
	{
		if (!IsOpen())
			return;
			
		GetGame().GetUIManager().HideScriptedMenu(this);
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			CloseTicketTool();
			return true;
		}
		return super.OnKeyDown(w, x, y, key);
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_BtnReply)
		{
			SendReply();
			return true;
		}
		if (w == m_BtnClose)
		{
			CloseSelectedTicket();
			return true;
		}
		if (w == m_BtnRefresh)
		{
			RequestAllTickets();
			return true;
		}
		return super.OnClick(w, x, y, button);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		if (w == m_TicketList)
		{
			OnTicketSelected(row);
			return true;
		}
		return super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
	}

	protected void RequestAllTickets()
	{
		ref Param1<int> data = new Param1<int>(0);
		GetRPCManager().SendRPC("GPDA_Tickets", "RequestAllTickets", data, true);
	}

	protected void RequestTicketMessages(string ticketId)
	{
		ref Param1<string> data = new Param1<string>(ticketId);
		GetRPCManager().SendRPC("GPDA_Tickets", "RequestTicketMessages", data, true);
	}

	protected void SendReply()
	{
		if (!m_SelectedTicketId || m_SelectedTicketId == "") return;
		if (!m_InputResponse) return;
		string message;
		m_InputResponse.GetText(message);
		message.Trim();
		if (message == "") return;
		ref Param2<string, string> data = new Param2<string, string>(m_SelectedTicketId, message);
		GetRPCManager().SendRPC("GPDA_Tickets", "SendMessage", data, true);
		m_InputResponse.SetText("");
		RequestTicketMessages(m_SelectedTicketId);
	}

	protected void CloseSelectedTicket()
	{
		if (!m_SelectedTicketId || m_SelectedTicketId == "") return;
		ref Param1<string> data = new Param1<string>(m_SelectedTicketId);
		GetRPCManager().SendRPC("GPDA_Tickets", "CloseTicket", data, true);
		RequestAllTickets();
	}

	protected void OnTicketSelected(int row)
	{
		if (!m_TicketsData || row < 0 || row >= m_TicketsData.Count()) return;
		GPDA_TicketInfo ticket = m_TicketsData[row];
		if (!ticket) return;
		m_SelectedTicketId = ticket.Id;
		if (m_TicketTitle) m_TicketTitle.SetText(ticket.Title);
		if (m_TicketDescription) m_TicketDescription.SetText("Status: " + ticket.Status + " | Player: " + ticket.PlayerName);
		
		// Clear previous messages from view while loading new ones
		if (m_CurrentMessages)
			m_CurrentMessages.Clear();
		RequestTicketMessages(ticket.Id);
	}

	void UpdateAllTickets(array<ref GPDA_TicketInfo> tickets)
	{
		m_TicketsData = tickets;
		if (!m_TicketList) return;
		m_TicketList.ClearItems();
		if (!tickets) return;
		foreach (GPDA_TicketInfo t : tickets)
		{
			string statusMark = "[ ] ";
			if (t.Status == "open") statusMark = "[OPEN] ";
			else if (t.Status == "assigned") statusMark = "[ASSIGNED] ";
			else if (t.Status == "closed") statusMark = "[CLOSED] ";
			m_TicketList.AddItem(statusMark + t.Title, null, 0);
		}
		if (tickets.Count() > 0)
		{
			m_TicketList.SelectRow(0);
			OnTicketSelected(0);
		}
	}

	void UpdateTicketMessages(string ticketId, array<ref GPDA_TicketMessageInfo> messages)
	{
		Print("[GPDA] Ticket " + ticketId + " messages: " + messages.Count().ToString());
		
		// Ignore updates for non-selected ticket
		if (!m_SelectedTicketId || m_SelectedTicketId == "" || ticketId != m_SelectedTicketId)
			return;
		
		if (!messages)
			return;
		
		if (m_CurrentMessages)
		{
			m_CurrentMessages.Clear();
			foreach (GPDA_TicketMessageInfo msg : messages)
				m_CurrentMessages.Insert(msg);
		}
		
		// Render conversation into description block (this layout doesn't have a list widget)
		if (!m_TicketDescription)
			return;
		
		string text = "";
		foreach (GPDA_TicketMessageInfo m : messages)
		{
			if (text != "") text += "\n";
			string prefix = "[USER] ";
			if (m.IsAdmin)
				prefix = "[ADMIN] ";
			text += prefix + m.SenderName + ": " + m.Message;
		}
		
		if (text == "")
			text = "No messages yet";
		
		m_TicketDescription.SetText(text);
	}

	static bool IsOpen()
	{
		return GetGame().GetUIManager().GetMenu() == GetInstance();
	}
}
