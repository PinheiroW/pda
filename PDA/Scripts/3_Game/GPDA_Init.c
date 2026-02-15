// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Init Script
// This file is loaded first to confirm mod is working
// ═══════════════════════════════════════════════════════════════════════════════

void GPDA_PrintInit()
{
	Print("═══════════════════════════════════════════════════════════════════");
	Print("[GPDA] ██████╗ ██████╗  █████╗     ██╗     ██╗████████╗███████╗");
	Print("[GPDA] ██╔══██╗██╔══██╗██╔══██╗    ██║     ██║╚══██╔══╝██╔════╝");
	Print("[GPDA] ██████╔╝██║  ██║███████║    ██║     ██║   ██║   █████╗  ");
	Print("[GPDA] ██╔═══╝ ██║  ██║██╔══██║    ██║     ██║   ██║   ██╔══╝  ");
	Print("[GPDA] ██║     ██████╔╝██║  ██║    ███████╗██║   ██║   ███████╗");
	Print("[GPDA] ╚═╝     ╚═════╝ ╚═╝  ╚═╝    ╚══════╝╚═╝   ╚═╝   ╚══════╝");
	Print("[GPDA] GRESHNIK PDA LITE v1.0 - Scripts Loaded!");
	Print("[GPDA] Use PDA item to open menu (LMB interaction)");
	Print("═══════════════════════════════════════════════════════════════════");
}

// Auto-execute on script load
static void GPDA_AutoInit()
{
	GPDA_PrintInit();
}

// Call it
class GPDA_Loader
{
	void GPDA_Loader()
	{
		GPDA_PrintInit();
	}
}

static ref GPDA_Loader g_GPDA_Loader = new GPDA_Loader();

