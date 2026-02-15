// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Static holder for pending capture target
// Lives in 4_World so ActionOpenCaptureMenu can write it,
// and GPDA_CaptureMenu (5_Mission) can read it.
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_CapturePending
{
	static TerritoryFlag s_TargetFlagPole;
}
