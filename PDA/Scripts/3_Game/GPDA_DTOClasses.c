// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - Data Transfer Objects (DTO)
// Classes for REST API data transfer
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
// RANK SYSTEM DTOs
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_RankDTO
{
	string name;
	int rank_order;
	int required_xp;
	string color;
	string icon;
	string next_rank_name;
}

class GPDA_RankXPItemDTO
{
	string item_class;
	int xp_amount;
}

class GPDA_RankXPKillDTO
{
	string entity_class;
	int xp_amount;
}

class GPDA_RankConfigDTO
{
	ref array<ref GPDA_RankDTO> ranks;
	ref array<ref GPDA_RankXPItemDTO> items;
	ref array<ref GPDA_RankXPKillDTO> kills;
	
	void GPDA_RankConfigDTO()
	{
		ranks = new array<ref GPDA_RankDTO>();
		items = new array<ref GPDA_RankXPItemDTO>();
		kills = new array<ref GPDA_RankXPKillDTO>();
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// CAPTURE SYSTEM DTOs
// ═══════════════════════════════════════════════════════════════════════════════

class GPDA_CaptureZoneDTO
{
	string zone_id;
	string name;
	float pos_x;
	float pos_y;
	float pos_z;
	float radius;
	string owner_group;
	int reward_points;
	bool is_active;
}
