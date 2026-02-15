// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Chat Notify HUD (Client-side)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_ChatNotify
{
	protected static ref GPDA_ChatNotify s_Instance;
	protected const string LAYOUT_PATH = "Greshnik_PDALITE/GUI/Layouts/pda_chatnotify.layout";

	protected Widget m_Root;
	protected Widget m_Line0;
	protected Widget m_Line1;
	protected Widget m_Line2;
	protected Widget m_Line3;
	protected TextWidget m_Line0Text;
	protected TextWidget m_Line1Text;
	protected TextWidget m_Line2Text;
	protected TextWidget m_Line3Text;
	protected ref array<int> m_HideAt;
	protected ref array<string> m_Lines;
	protected int m_ForceShowUntil;

	static GPDA_ChatNotify GetInstance()
	{
		if (!s_Instance)
			s_Instance = new GPDA_ChatNotify();
		return s_Instance;
	}

	void GPDA_ChatNotify()
	{
		m_HideAt = new array<int>;
		m_HideAt.Insert(0);
		m_HideAt.Insert(0);
		m_HideAt.Insert(0);
		m_Lines = new array<string>;
		m_Lines.Insert("");
		m_Lines.Insert("");
		m_Lines.Insert("");
		m_ForceShowUntil = 0;
		Init();
	}

	protected void Init()
	{
		if (m_Root) return;
		if (!GetGame()) return;

		m_Root = GetGame().GetWorkspace().CreateWidgets(LAYOUT_PATH);
		if (!m_Root)
		{
			Print("[GPDA][ChatNotify] ERROR: CreateWidgets failed for " + LAYOUT_PATH);
			return;
		}

		m_Line0 = m_Root.FindAnyWidget("GPDA_ChatNotifyLine0");
		m_Line1 = m_Root.FindAnyWidget("GPDA_ChatNotifyLine1");
		m_Line2 = m_Root.FindAnyWidget("GPDA_ChatNotifyLine2");
		m_Line3 = m_Root.FindAnyWidget("GPDA_ChatNotifyLine3");
		m_Line0Text = TextWidget.Cast(m_Root.FindAnyWidget("GPDA_ChatNotifyLine0Text"));
		m_Line1Text = TextWidget.Cast(m_Root.FindAnyWidget("GPDA_ChatNotifyLine1Text"));
		m_Line2Text = TextWidget.Cast(m_Root.FindAnyWidget("GPDA_ChatNotifyLine2Text"));
		m_Line3Text = TextWidget.Cast(m_Root.FindAnyWidget("GPDA_ChatNotifyLine3Text"));

		string dbg = "[GPDA][ChatNotify] Init widgets ok=" + (m_Root != null).ToString();
		dbg += " l0=" + (m_Line0 != null).ToString();
		dbg += " l0t=" + (m_Line0Text != null).ToString();
		dbg += " l1=" + (m_Line1 != null).ToString();
		dbg += " l1t=" + (m_Line1Text != null).ToString();
		dbg += " l2=" + (m_Line2 != null).ToString();
		dbg += " l2t=" + (m_Line2Text != null).ToString();
		Print(dbg);
		m_Root.SetSort(10000, true);
		m_Root.SetFlags(WidgetFlags.IGNOREPOINTER);

		m_Root.Show(false);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.Update, 200, true);
	}

	static bool HasRequiredPDAItem()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player) return false;

		// Check if player has GRESHNIK_PDA in inventory
		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		for (int i = 0; i < items.Count(); i++)
		{
			if (!items[i]) continue;
			if (items[i].IsKindOf("GRESHNIK_PDA_Base"))
				return true;
		}
		return false;
	}

	static bool CanShow()
	{
		return true;
	}
	
	protected void PushInternal(string senderName, string message, bool force)
	{
		if (!m_Root)
		{
			Print("[GPDA][ChatNotify] ERROR: Push called but m_Root is null");
			Init();
			if (!m_Root) return;
		}
		
		
		
		if (!m_Lines)
		{
			m_Lines = new array<string>;
			m_Lines.Insert("");
			m_Lines.Insert("");
			m_Lines.Insert("");
		}
		
		string sn = senderName;
		string msg = message;
		if (sn.Length() > 30)
			sn = sn.Substring(0, 30);
		if (msg.Length() > 70)
			msg = msg.Substring(0, 70) + "..";
		
		m_Lines[2] = m_Lines[1];
		m_Lines[1] = m_Lines[0];
		m_Lines[0] = sn + ": " + msg;
		
		if (m_Line0Text) m_Line0Text.SetText(m_Lines[0]);
		if (m_Line1Text) m_Line1Text.SetText(m_Lines[1]);
		if (m_Line2Text) m_Line2Text.SetText(m_Lines[2]);

		int now = GetGame().GetTime();
		if (force)
			m_ForceShowUntil = now + 8000;
		m_HideAt[2] = m_HideAt[1];
		m_HideAt[1] = m_HideAt[0];
		m_HideAt[0] = now + 8000;

		m_Root.Show(true);
		Print("[GPDA][ChatNotify] Show: " + sn + ": " + msg);
	}

	void Push(string senderName, string message)
	{
		PushInternal(senderName, message, false);
	}
	
	void PushForce(string senderName, string message)
	{
		PushInternal(senderName, message, true);
	}

	protected void Update()
	{
		if (!m_Root) return;
		int now = GetGame().GetTime();
		bool allow = CanShow() || (m_ForceShowUntil > now);

		bool any = false;
		if (m_Line0) { bool v0 = (m_HideAt[0] > now) && allow; m_Line0.Show(v0); any = any || v0; }
		if (m_Line1) { bool v1 = (m_HideAt[1] > now) && allow; m_Line1.Show(v1); any = any || v1; }
		if (m_Line2) { bool v2 = (m_HideAt[2] > now) && allow; m_Line2.Show(v2); any = any || v2; }

		m_Root.Show(any);
	}
}
