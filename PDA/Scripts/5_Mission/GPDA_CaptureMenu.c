// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Capture Menu (Client-side UI)
// Opened by interacting with TerritoryFlag
// Shows zone info, capture progress, START/CANCEL buttons
// Flags are attached via TAB inventory (server validates group FlagClass)
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_CaptureMenu : UIScriptedMenu
{
	protected static ref GPDA_CaptureMenu m_Instance;
	
	// Root
	protected Widget m_Root;
	
	// Zone info
	protected TextWidget m_TxtZoneName;
	protected TextWidget m_TxtCoords;
	protected TextWidget m_TxtOwner;
	protected TextWidget m_TxtYourGroup;
	protected TextWidget m_TxtFlagInfo;
	protected TextWidget m_TxtStatus;
	protected TextWidget m_TxtAttackers;
	protected TextWidget m_TxtDefenders;
	protected TextWidget m_TxtSpeedMod;
	protected ImageWidget m_ProgressBarFill;
	protected TextWidget m_TxtProgressPct;
	protected TextWidget m_TxtTimeLeft;
	protected TextWidget m_TxtReward;
	protected TextWidget m_TxtCaptureStatus;
	
	// Buttons
	protected ButtonWidget m_BtnStartCapture;
	protected ButtonWidget m_BtnCancelCapture;
	protected ButtonWidget m_BtnClose;
	protected ButtonWidget m_BtnCloseX;
	
	// State
	protected TerritoryFlag m_TargetFlagPole;
	protected bool m_IsCapturing;
	protected float m_CaptureProgress;    // 0..100
	protected float m_CaptureTimeLeft;
	protected int m_Attackers;
	protected int m_Defenders;
	protected float m_SpeedMod;
	
	// HUD progress bar (static - persists when menu closes)
	protected static Widget s_HudRoot;
	protected static ImageWidget s_HudFill;
	protected static TextWidget s_HudTitle;
	protected static TextWidget s_HudPct;
	protected static bool s_IsCapturing;
	
	// ─── SINGLETON ─────────────────────────────────────────────────
	static GPDA_CaptureMenu GetInstance()
	{
		return m_Instance;
	}
	
	static void SetTargetFlagPole(TerritoryFlag flag)
	{
		GPDA_CapturePending.s_TargetFlagPole = flag;
		if (m_Instance)
			m_Instance.m_TargetFlagPole = flag;
	}
	
	// ─── INIT ──────────────────────────────────────────────────────
	override Widget Init()
	{
		m_Instance = this;
		m_IsCapturing = false;
		m_CaptureProgress = 0;
		
		// Pick up pending flag pole from 4_World static holder
		if (GPDA_CapturePending.s_TargetFlagPole)
			m_TargetFlagPole = GPDA_CapturePending.s_TargetFlagPole;
		
		m_Root = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/gpda_capture_menu.layout");
		if (!m_Root)
		{
			Print("[GPDA][Capture] ERROR: Failed to load layout");
			return null;
		}
		
		// Zone info widgets
		m_TxtZoneName = TextWidget.Cast(m_Root.FindAnyWidget("TxtZoneName"));
		m_TxtCoords = TextWidget.Cast(m_Root.FindAnyWidget("TxtCoords"));
		m_TxtOwner = TextWidget.Cast(m_Root.FindAnyWidget("TxtOwner"));
		m_TxtYourGroup = TextWidget.Cast(m_Root.FindAnyWidget("TxtYourGroup"));
		m_TxtFlagInfo = TextWidget.Cast(m_Root.FindAnyWidget("TxtFlagInfo"));
		m_TxtStatus = TextWidget.Cast(m_Root.FindAnyWidget("TxtStatus"));
		m_TxtAttackers = TextWidget.Cast(m_Root.FindAnyWidget("TxtAttackers"));
		m_TxtDefenders = TextWidget.Cast(m_Root.FindAnyWidget("TxtDefenders"));
		m_TxtSpeedMod = TextWidget.Cast(m_Root.FindAnyWidget("TxtSpeedMod"));
		m_ProgressBarFill = ImageWidget.Cast(m_Root.FindAnyWidget("ProgressBarFill"));
		m_TxtProgressPct = TextWidget.Cast(m_Root.FindAnyWidget("TxtProgressPct"));
		m_TxtTimeLeft = TextWidget.Cast(m_Root.FindAnyWidget("TxtTimeLeft"));
		m_TxtReward = TextWidget.Cast(m_Root.FindAnyWidget("TxtReward"));
		m_TxtCaptureStatus = TextWidget.Cast(m_Root.FindAnyWidget("TxtCaptureStatus"));
		
		// Buttons
		m_BtnStartCapture = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnStartCapture"));
		m_BtnCancelCapture = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnCancelCapture"));
		m_BtnClose = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnClose"));
		m_BtnCloseX = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnCloseX"));
		
		// Initial state
		if (m_BtnCancelCapture) m_BtnCancelCapture.Show(false);
		
		// Check flag on pole
		CheckPoleFlag();
		
		// If capture was in progress, restore UI state
		if (s_IsCapturing)
		{
			m_IsCapturing = true;
			if (m_BtnStartCapture) m_BtnStartCapture.Show(false);
			if (m_BtnCancelCapture) m_BtnCancelCapture.Show(true);
			if (m_TxtStatus) m_TxtStatus.SetText("Status: CAPTURING...");
		}
		
		UpdateZoneInfo();
		
		// Request zone info from server
		RequestCaptureInfo();
		
		return m_Root;
	}
	
	override void OnShow()
	{
		super.OnShow();
		GetGame().GetInput().ChangeGameFocus(1);
		GetGame().GetUIManager().ShowCursor(true);
	}
	
	override void OnHide()
	{
		super.OnHide();
		GetGame().GetInput().ResetGameFocus();
		GetGame().GetUIManager().ShowCursor(false);
		m_Instance = null;
	}
	
	void ~GPDA_CaptureMenu()
	{
		m_Instance = null;
	}
	
	// ─── CHECK FLAG ON POLE ───────────────────────────────────────
	void CheckPoleFlag()
	{
		if (!m_TargetFlagPole) return;
		
		int flagSlotId = InventorySlots.GetSlotIdFromString("Material_FPole_Flag");
		EntityAI poleFlag = m_TargetFlagPole.GetInventory().FindAttachment(flagSlotId);
		
		if (poleFlag)
		{
			string flagName = poleFlag.GetDisplayName();
			if (flagName == "") flagName = poleFlag.GetType();
			if (m_TxtFlagInfo) m_TxtFlagInfo.SetText("Flag on pole: " + flagName);
			if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("Flag ready! Press START CAPTURE");
		}
		else
		{
			if (m_TxtFlagInfo) m_TxtFlagInfo.SetText("Flag on pole: NONE");
			if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("Attach your group flag via TAB inventory first!");
		}
	}
	
	// ─── ZONE INFO ────────────────────────────────────────────────
	void UpdateZoneInfo()
	{
		if (!m_TargetFlagPole)
		{
			if (m_TxtZoneName) m_TxtZoneName.SetText("Zone: No flag pole");
			return;
		}
		
		vector pos = m_TargetFlagPole.GetPosition();
		string posHash = Math.Floor(pos[0]).ToString() + "_" + Math.Floor(pos[2]).ToString();
		
		if (m_TxtZoneName) m_TxtZoneName.SetText("Zone: " + posHash);
		if (m_TxtCoords) m_TxtCoords.SetText("Pos: " + Math.Floor(pos[0]).ToString() + " / " + Math.Floor(pos[2]).ToString());
		if (m_TxtOwner) m_TxtOwner.SetText("Owner: ---");
		if (m_TxtStatus) m_TxtStatus.SetText("Status: Ready");
		if (m_TxtAttackers) m_TxtAttackers.SetText("Attackers: 0");
		if (m_TxtDefenders) m_TxtDefenders.SetText("Defenders: 0");
		if (m_TxtSpeedMod) m_TxtSpeedMod.SetText("Speed: x1.0");
		if (m_TxtReward) m_TxtReward.SetText("Reward: 100 pts");
		
		if (m_TxtYourGroup) m_TxtYourGroup.SetText("Your Group: ---");
	}
	
	// ─── CAPTURE START ────────────────────────────────────────────
	void OnStartCapture()
	{
		if (!m_TargetFlagPole)
		{
			if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("ERROR: No flag pole target!");
			return;
		}
		if (m_IsCapturing)
		{
			if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("Capture already in progress...");
			return;
		}
		
		// Send RPC to server: start capture
		// Payload: flagpole_pos_hash (server reads flag from pole itself)
		vector pos = m_TargetFlagPole.GetPosition();
		string posHash = Math.Floor(pos[0]).ToString() + "_" + Math.Floor(pos[2]).ToString();
		
		Param1<string> data = new Param1<string>(posHash);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			GetGame().RPCSingleParam(player, GPDA_RPC.CAPTURE_START, data, true, null);
			Print("[GPDA][Capture] Sent CAPTURE_START: " + posHash);
		}
		
		m_IsCapturing = true;
		m_CaptureProgress = 0;
		s_IsCapturing = true;
		
		ShowHud(posHash);
		
		if (m_BtnStartCapture) m_BtnStartCapture.Show(false);
		if (m_BtnCancelCapture) m_BtnCancelCapture.Show(true);
		if (m_TxtStatus) m_TxtStatus.SetText("Status: CAPTURING...");
		if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("Capture in progress...");
	}
	
	// ─── CANCEL CAPTURE ──────────────────────────────────────────
	void OnCancelCapture()
	{
		if (!m_IsCapturing) return;
		
		Param1<string> cancelData = new Param1<string>("cancel");
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			GetGame().RPCSingleParam(player, GPDA_RPC.CAPTURE_CANCEL, cancelData, true, null);
		
		ResetCaptureUI();
		Print("[GPDA][Capture] Cancelled capture");
	}
	
	void ResetCaptureUI()
	{
		m_IsCapturing = false;
		m_CaptureProgress = 0;
		s_IsCapturing = false;
		HideHud();
		
		if (m_BtnStartCapture) m_BtnStartCapture.Show(true);
		if (m_BtnCancelCapture) m_BtnCancelCapture.Show(false);
		if (m_ProgressBarFill) m_ProgressBarFill.SetSize(0, 1.0);
		if (m_TxtProgressPct) m_TxtProgressPct.SetText("0%");
		if (m_TxtTimeLeft) m_TxtTimeLeft.SetText("Time: --");
		if (m_TxtStatus) m_TxtStatus.SetText("Status: Ready");
		if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("Attach flag via TAB, then press START CAPTURE");
	}
	
	// ─── HUD PROGRESS BAR (static - persists when menu closed) ───
	static void ShowHud(string zoneName)
	{
		if (!s_HudRoot)
		{
			s_HudRoot = GetGame().GetWorkspace().CreateWidgets("Greshnik_PDALITE/GUI/Layouts/gpda_capture_progress.layout");
			if (s_HudRoot)
			{
				s_HudFill = ImageWidget.Cast(s_HudRoot.FindAnyWidget("CaptureBarProgressFill"));
				s_HudTitle = TextWidget.Cast(s_HudRoot.FindAnyWidget("CaptureBarTitle"));
				s_HudPct = TextWidget.Cast(s_HudRoot.FindAnyWidget("CaptureBarPct"));
			}
		}
		if (s_HudRoot) s_HudRoot.Show(true);
		if (s_HudTitle) s_HudTitle.SetText("CAPTURING: " + zoneName);
		if (s_HudFill) s_HudFill.SetSize(0, 1.0);
		if (s_HudPct) s_HudPct.SetText("0%");
	}
	
	static void HideHud()
	{
		if (s_HudRoot)
		{
			s_HudRoot.Unlink();
			s_HudRoot = null;
			s_HudFill = null;
			s_HudTitle = null;
			s_HudPct = null;
		}
		s_IsCapturing = false;
	}
	
	static void UpdateHudProgress(float pct)
	{
		if (!s_HudRoot) return;
		float norm = pct / 100.0;
		if (norm > 1.0) norm = 1.0;
		if (s_HudFill) s_HudFill.SetSize(norm, 1.0);
		if (s_HudPct) s_HudPct.SetText(Math.Floor(pct).ToString() + "%");
	}
	
	static bool IsHudActive()
	{
		return s_IsCapturing;
	}
	
	// ─── RPC HANDLERS (called from ClientRPC) ─────────────────────
	void HandleCaptureProgress(ParamsReadContext ctx)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		// Format: "progress,attackers,defenders,speedmod,timeleft"
		string payload = data.param1;
		array<string> parts = new array<string>;
		payload.Split(",", parts);
		
		if (parts.Count() < 5) return;
		
		m_CaptureProgress = parts[0].ToFloat();
		m_Attackers = parts[1].ToInt();
		m_Defenders = parts[2].ToInt();
		m_SpeedMod = parts[3].ToFloat();
		m_CaptureTimeLeft = parts[4].ToFloat();
		
		// Update UI
		float pctNorm = m_CaptureProgress / 100.0;
		if (pctNorm > 1.0) pctNorm = 1.0;
		
		if (m_ProgressBarFill) m_ProgressBarFill.SetSize(pctNorm, 1.0);
		if (m_TxtProgressPct) m_TxtProgressPct.SetText(Math.Floor(m_CaptureProgress).ToString() + "%");
		if (m_TxtAttackers) m_TxtAttackers.SetText("Attackers: " + m_Attackers.ToString());
		if (m_TxtDefenders) m_TxtDefenders.SetText("Defenders: " + m_Defenders.ToString());
		if (m_TxtSpeedMod) m_TxtSpeedMod.SetText("Speed: x" + m_SpeedMod.ToString());
		if (m_TxtTimeLeft) m_TxtTimeLeft.SetText("Time: " + Math.Ceil(m_CaptureTimeLeft).ToString() + "s");
		
		// Update HUD bar too
		UpdateHudProgress(m_CaptureProgress);
	}
	
	void HandleCaptureComplete(ParamsReadContext ctx)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string payload = data.param1;
		
		m_IsCapturing = false;
		s_IsCapturing = false;
		HideHud();
		
		if (m_ProgressBarFill) m_ProgressBarFill.SetSize(1.0, 1.0);
		if (m_TxtProgressPct) m_TxtProgressPct.SetText("100%");
		if (m_TxtStatus) m_TxtStatus.SetText("Status: CAPTURED!");
		if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("ZONE CAPTURED! +" + payload + " pts");
		if (m_TxtOwner) m_TxtOwner.SetText("Owner: YOUR GROUP");
		if (m_BtnStartCapture) m_BtnStartCapture.Show(true);
		if (m_BtnCancelCapture) m_BtnCancelCapture.Show(false);
		
		Print("[GPDA][Capture] Capture complete!");
	}
	
	void HandleCaptureCancelled(ParamsReadContext ctx)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		string reason = data.param1;
		ResetCaptureUI();
		if (m_TxtCaptureStatus) m_TxtCaptureStatus.SetText("Capture failed: " + reason);
		
		Print("[GPDA][Capture] Capture cancelled: " + reason);
	}
	
	void HandleCaptureInfo(ParamsReadContext ctx)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		
		// Format: "ownerGroup,yourGroup,reward,flagInfo"
		string payload = data.param1;
		array<string> parts = new array<string>;
		payload.Split(",", parts);
		
		if (parts.Count() < 3) return;
		
		string owner = parts[0];
		string yourGroup = parts[1];
		string reward = parts[2];
		
		if (m_TxtOwner)
		{
			if (owner == "" || owner == "None")
				m_TxtOwner.SetText("Owner: No one");
			else
				m_TxtOwner.SetText("Owner: " + owner);
		}
		if (m_TxtYourGroup) m_TxtYourGroup.SetText("Your Group: " + yourGroup);
		if (m_TxtReward) m_TxtReward.SetText("Reward: " + reward + " pts");
		
		// 4th field = your group's FlagClass or flag status
		if (parts.Count() >= 4)
		{
			string flagInfo = parts[3];
			if (m_TxtFlagInfo) m_TxtFlagInfo.SetText("Your flag: " + flagInfo);
		}
		
		// Re-check pole flag
		CheckPoleFlag();
		
		Print("[GPDA][Capture] Zone info: owner=" + owner + " group=" + yourGroup + " reward=" + reward);
	}
	
	// ─── REQUEST INFO ─────────────────────────────────────────────
	void RequestCaptureInfo()
	{
		if (!m_TargetFlagPole) return;
		
		vector pos = m_TargetFlagPole.GetPosition();
		string posHash = Math.Floor(pos[0]).ToString() + "_" + Math.Floor(pos[2]).ToString();
		
		Param1<string> infoReq = new Param1<string>(posHash);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			GetGame().RPCSingleParam(player, GPDA_RPC.CAPTURE_REQUEST_INFO, infoReq, true, null);
	}
	
	// ─── ONCLICK ──────────────────────────────────────────────────
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_BtnCloseX || w == m_BtnClose)
		{
			Close();
			return true;
		}
		
		if (w == m_BtnStartCapture)
		{
			OnStartCapture();
			return true;
		}
		
		if (w == m_BtnCancelCapture)
		{
			OnCancelCapture();
			return true;
		}
		
		return false;
	}
	
	override bool OnKeyPress(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			Close();
			return true;
		}
		return false;
	}
}
