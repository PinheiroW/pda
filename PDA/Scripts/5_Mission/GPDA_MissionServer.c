// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Mission Server Init
// Initializes server-side manager
// ═══════════════════════════════════════════════════════════════════════════════

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		
		Print("[GPDA] MissionServer::OnInit - Initializing PDA LITE Server...");
		
		// Initialize Server Manager
		if (!g_GPDA_Server)
		{
			g_GPDA_Server = GPDA_ServerManager.GetInstance();
			Print("[GPDA] Server Manager created successfully!");
		}
		
		// Initialize Group Manager (standalone system)
		if (!g_GPDA_GroupManager)
		{
			g_GPDA_GroupManager = GPDA_GroupManager.GetInstance();
			g_GPDA_GroupManager.Init();
			Print("[GPDA] Group Manager initialized!");
		}
		
		// Initialize Group RPC Handler
		if (!g_GPDA_GroupRPC)
		{
			g_GPDA_GroupRPC = GPDA_GroupRPCHandler.GetInstance();
			g_GPDA_GroupRPC.Init();
			Print("[GPDA] Group RPC Handler initialized!");
		}
		
		// Initialize Player Database
		if (!g_GPDA_PlayerDB)
		{
			g_GPDA_PlayerDB = GPDA_PlayerDatabase.GetInstance();
			g_GPDA_PlayerDB.Init();
			Print("[GPDA] Player Database initialized!");
		}
		
		// Initialize Account Manager
		if (!g_GPDA_AccountManager)
		{
			g_GPDA_AccountManager = GPDA_AccountManager.GetInstance();
			g_GPDA_AccountManager.Init();
			Print("[GPDA] Account Manager initialized!");
		}
		
		
		// Initialize Ticket Manager
		if (!g_GPDA_TicketManager)
		{
			g_GPDA_TicketManager = GPDA_TicketManager.GetInstance();
			g_GPDA_TicketManager.Init();
			Print("[GPDA] Ticket Manager initialized!");
		}
		
		// Initialize Ticket RPC Handler
		if (!g_GPDA_TicketRPC)
		{
			g_GPDA_TicketRPC = GPDA_TicketRPCHandler.GetInstance();
			g_GPDA_TicketRPC.Init();
			Print("[GPDA] Ticket RPC Handler initialized!");
		}

		// Initialize Database RPC Handler
		if (!g_GPDA_DatabaseRPC)
		{
			g_GPDA_DatabaseRPC = GPDA_DatabaseRPCHandler.GetInstance();
			g_GPDA_DatabaseRPC.Init();
			Print("[GPDA] Database RPC Handler initialized!");
		}
		
		// Initialize Rank Manager
		if (!g_GPDA_RankManager)
		{
			g_GPDA_RankManager = GPDA_RankManager.GetInstance();
			g_GPDA_RankManager.Init();
			Print("[GPDA] Rank Manager initialized!");
		}
		
		// Initialize Capture Manager
		if (!g_GPDA_CaptureManager)
		{
			g_GPDA_CaptureManager = GPDA_CaptureManager.GetInstance();
			g_GPDA_CaptureManager.Init();
			Print("[GPDA] Capture Manager initialized!");
		}

		// Initialize Admin Manager
		if (!g_GPDA_AdminManager)
		{
			g_GPDA_AdminManager = GPDA_AdminManager.GetInstance();
			g_GPDA_AdminManager.Init();
			Print("[GPDA] Admin Manager initialized!");
		}

		// Initialize Mutant Spawner Handler
		if (!g_GPDA_MSPHandler)
		{
			g_GPDA_MSPHandler = GPDA_MutantSpawnerHandler.GetInstance();
			g_GPDA_MSPHandler.Init();
			Print("[GPDA] Mutant Spawner Handler initialized!");
		}

		// Initialize Capture RPC Handler
		if (!g_GPDA_CaptureRPC)
		{
			g_GPDA_CaptureRPC = GPDA_CaptureRPCHandler.GetInstance();
			g_GPDA_CaptureRPC.Init();
			Print("[GPDA] Capture RPC Handler initialized!");
		}
		
		// Initialize Account RPC Handler
		if (!g_GPDA_AccountRPC)
		{
			g_GPDA_AccountRPC = GPDA_AccountRPCHandler.GetInstance();
			g_GPDA_AccountRPC.Init();
			Print("[GPDA] Account RPC Handler initialized!");
		}
		
		// Initialize Group Admin RPC Handler
		GPDA_GroupAdminRPC.GetInstance().Init();
		
	}
	
	override void OnMissionStart()
	{
		super.OnMissionStart();
		Print("[GPDA] MissionServer::OnMissionStart - PDA LITE active");
	}
}

