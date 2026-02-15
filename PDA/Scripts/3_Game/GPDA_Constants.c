// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - RPC Constants & Menu IDs
// Unique prefix GPDA_ to avoid conflicts with other mods
// ═══════════════════════════════════════════════════════════════════════════════

// ─── Menu IDs ────────────────────────────────────────────────────────────────
const int MENU_GPDA_MAIN = 777099;
const int MENU_GPDA_MUTANT_SPAWNER = 777100;
const int MENU_GPDA_CAPTURE = 777101;
const int MENU_GPDA_GROUP_ADMIN = 777102;

class GPDA_RPC
{
	// Chat
	static const int SEND_CHAT_MESSAGE = 777001;
	static const int RECEIVE_CHAT_MESSAGE = 777002;
	static const int REQUEST_CHAT_HISTORY = 777003;
	static const int RECEIVE_CHAT_HISTORY = 777004;
	
	// Private Messages
	static const int SEND_PM = 777011;
	static const int RECEIVE_PM = 777012;
	static const int REQUEST_PM_HISTORY = 777013;
	static const int RECEIVE_PM_HISTORY = 777014;
	
	// Profile & Stats
	static const int REQUEST_PROFILE = 777021;
	static const int RECEIVE_PROFILE = 777022;
	static const int UPDATE_KILL_STAT = 777023;
	
	// Top 100
	static const int REQUEST_TOP100 = 777031;
	static const int RECEIVE_TOP100 = 777032;
	
	// Map Markers
	static const int ADD_MARKER = 777041;
	static const int REMOVE_MARKER = 777042;
	static const int REQUEST_MARKERS = 777043;
	static const int RECEIVE_MARKERS = 777044;
	
	// Registration System
	static const int CHECK_REGISTRATION = 777051;
	static const int SHOW_REGISTRATION = 777052;
	static const int SUBMIT_REGISTRATION = 777053;
	static const int REGISTRATION_COMPLETE = 777054;
	
	// Database (Military)
	static const int REQUEST_DATABASE_ACCESS = 777061;
	static const int RECEIVE_DATABASE_ACCESS = 777062;
	static const int SEARCH_PLAYER = 777063;
	static const int RECEIVE_SEARCH_RESULTS = 777064;
	static const int ISSUE_LICENSE = 777065;
	static const int LICENSE_RESULT = 777066;
	
	// ═══════════════════════════════════════════════════════════════════
	// Mutant Spawner System (Admin only) - 777200+
	// ═══════════════════════════════════════════════════════════════════
	static const int MSP_REQUEST_ZONES    = 777200;  // Client -> Server: request all zones
	static const int MSP_RECEIVE_ZONES    = 777201;  // Server -> Client: zones data (JSON)
	static const int MSP_SAVE_ZONE        = 777202;  // Client -> Server: save zone (JSON)
	static const int MSP_DELETE_ZONE      = 777203;  // Client -> Server: delete zone by name
	static const int MSP_TOGGLE_ZONE      = 777204;  // Client -> Server: toggle zone enabled
	static const int MSP_REQUEST_PRESETS  = 777205;  // Client -> Server: request presets
	static const int MSP_RECEIVE_PRESETS  = 777206;  // Server -> Client: presets count (int)
	static const int MSP_SAVE_PRESETS     = 777207;  // Client -> Server: save single preset (JSON)
	static const int MSP_FORCE_SPAWN_ALL  = 777208;  // Client -> Server: force spawn all zones
	static const int MSP_DESPAWN_ALL      = 777209;  // Client -> Server: despawn all creatures
	static const int MSP_STATUS           = 777210;  // Server -> Client: status message
	static const int MSP_RECEIVE_ONE_PRESET = 777211; // Server -> Client: single preset (JSON)
	static const int MSP_SAVE_PRESETS_DONE  = 777212; // Client -> Server: presets save complete
	
	// ═══════════════════════════════════════════════════════════════
	// Capture System (Flag Pole) - 777300+
	// ═══════════════════════════════════════════════════════════════
	static const int CAPTURE_START         = 777300;  // Client -> Server: start capture (posHash,flagClass)
	static const int CAPTURE_PROGRESS      = 777301;  // Server -> Client: progress update
	static const int CAPTURE_COMPLETE      = 777302;  // Server -> Client: capture done
	static const int CAPTURE_CANCEL        = 777303;  // Client -> Server: cancel / Server -> Client: failed
	static const int CAPTURE_REQUEST_INFO  = 777304;  // Client -> Server: request zone info
	static const int CAPTURE_INFO          = 777305;  // Server -> Client: zone info response
	static const int CAPTURE_ATTACH_FLAG   = 777306;  // Client -> Server: attach flag to pole (lowered)
	
	// ═══════════════════════════════════════════════════════════════
	// Group Admin System (Admin only) - 777400+
	// ═══════════════════════════════════════════════════════════════
	static const int GRP_REQUEST_ALL       = 777400;  // Client -> Server: request all groups
	static const int GRP_RECEIVE_ALL       = 777401;  // Server -> Client: groups count (int)
	static const int GRP_SAVE_GROUP        = 777402;  // Client -> Server: save group (JSON)
	static const int GRP_DELETE_GROUP      = 777403;  // Client -> Server: delete group by name
	static const int GRP_STATUS            = 777404;  // Server -> Client: status message
	static const int GRP_REQUEST_PLAYERS   = 777405;  // Client -> Server: request online players
	static const int GRP_RECEIVE_PLAYERS   = 777406;  // Server -> Client: online players list
	static const int GRP_RECEIVE_ONE       = 777407;  // Server -> Client: single group (JSON)
}
