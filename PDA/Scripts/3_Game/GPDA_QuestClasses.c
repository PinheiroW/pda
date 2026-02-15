// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Quest & Rank Classes
// Copied from GreshnikServer_ModsPack for integration
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// QUEST STATUS
// ═══════════════════════════════════════════════════════════════════════════════
class GPDA_QuestStatus
{
	static const int NOT_STARTED = 0;
	static const int IN_PROGRESS = 1;
	static const int COMPLETED = 2;
	static const int READY_TO_TURN_IN = 3;
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUEST RPC ENUM
// ═══════════════════════════════════════════════════════════════════════════════
class GPDA_QuestRPC
{
	static const int UPDATE_HUD = 0x51535433;
	static const int CLEAR_HUD = 0x51535434;
	static const int ACCEPT_QUEST = 0x51535435;
	static const int COMPLETE_QUEST = 0x51535436;
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUEST REWARD
// ═══════════════════════════════════════════════════════════════════════════════
class GPDA_QuestReward
{
	string Item;
	int Count;
	
	void GPDA_QuestReward()
	{
		Item = "";
		Count = 1;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// QUEST DATA (for display in PDA)
// ═══════════════════════════════════════════════════════════════════════════════
class GPDA_QuestData
{
	string ID;
	string Title;
	string Objective;
	int CurrentProgress;
	int RequiredProgress;
	int Status;
	string TurnInNPC;
	
	void GPDA_QuestData()
	{
		ID = "";
		Title = "";
		Objective = "";
		CurrentProgress = 0;
		RequiredProgress = 0;
		Status = 0;
		TurnInNPC = "";
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// RANK DATA (for display in PDA)
// ═══════════════════════════════════════════════════════════════════════════════
class GPDA_RankData
{
	string RankName;
	int CurrentXP;
	int NextRankXP;
	string NextRankName;
	
	void GPDA_RankData()
	{
		RankName = "Unknown";
		CurrentXP = 0;
		NextRankXP = 0;
		NextRankName = "";
	}
}

// GPDA_TicketMessage and GPDA_Ticket are defined in GPDA_DataClasses.c

