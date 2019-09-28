#include "include_cheat.h"

HMODULE g_module;
bool g_erase = false;
C_locPlayer g_pLocalPlayer;
C_CSPlayer* get_entity( const int index ) { return reinterpret_cast< C_CSPlayer* >( g_pEntitiyList->GetClientEntity( index ) ); }
C_BaseCombatWeapon* get_weapon( const CBaseHandle * handle ) { if ( !handle ) { return nullptr; } return reinterpret_cast< C_BaseCombatWeapon* >( g_pEntitiyList->GetClientEntityFromHandle( *handle ) ); }
uintptr_t g_GCClientSystem = 0;
float* g_asdasd;


int get_config_index( int weaponindex )
{
	switch ( weaponindex )
	{
		case WEAPON_G3SG1:
		case WEAPON_SCAR20:
			return 0;
		case WEAPON_SSG08:
			return 1;
		case WEAPON_AWP:
			return 2;
		case WEAPON_DEAGLE:
		case WEAPON_REVOLVER:
			return 3;
		case WEAPON_CZ75A:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_GLOCK:
		case WEAPON_HKP2000:
		case WEAPON_P250:
		case WEAPON_USP_SILENCER:
		case WEAPON_TEC9:
			return 4;
		default:
			return 5;
	}
}

weaponconfig_t* get_config( C_BaseCombatWeapon* weapon )
{
	return &config::get().weapon_config[ get_config_index( weapon->get_weapon_id() ) ];
}

antiaimconfig_t* get_antiaim( const int type )
{
	return &config::get().antiaim_config[ type ];
}

CUserCmd* g_cmd = nullptr;
bool* g_send_packet = nullptr;

bool g_unload = false;
bool g_in_hbp = false;
bool g_in_gl = false;
bool g_ishltv_fix = false;
bool g_setup_vel_fix = false;
bool g_in_lp_draw = false;

int g_reliable_state = -1;
int g_sequence_rn = -1;

int g_tick_shot = 0;

C_CSGameRulesProxy** g_ppGameRules = nullptr;
VMatrix g_vmatrix;

msg_queue<std::pair<int, LPDIRECT3DVERTEXBUFFER9 >> g_vertex_buffer;
LPDIRECT3DVERTEXBUFFER9 g_pVB;

bool* g_disable_post_processing = nullptr;

float( *random_float ) ( float from, float to );
void( *random_seed ) ( unsigned int seed );

void util_trace_line( const Vector & vec_start, const Vector & vec_end, const unsigned int n_mask, C_BaseEntity * p_cs_ignore, trace_t * p_trace )
{
	Ray_t ray;
	ray.Init( vec_start, vec_end );

	if ( p_cs_ignore )
	{
		CTraceFilter filter;
		filter.pSkip = p_cs_ignore;
		g_pTrace->TraceRay( ray, n_mask, &filter, p_trace );
		return;
	}

	CTraceFilterWorldOnly filter;
	g_pTrace->TraceRay( ray, n_mask, &filter, p_trace );
}

void util_trace_hull( const Vector & vec_start, const Vector & vec_end, const unsigned int n_mask, const Vector & extents, trace_t * p_trace )
{
	CTraceFilterNoPlayers filter;

	Ray_t ray;
	ray.Init( vec_start, vec_end );
	ray.m_Extents = extents;
	ray.m_IsRay = false;

	g_pTrace->TraceRay( ray, n_mask, &filter, p_trace );
}

void util_clip_trace_to_players( const Vector & vec_abs_start, const Vector & vec_abs_end, unsigned int mask, ITraceFilter * filter, trace_t * tr )
{
	static auto dw_address = offsets::clip_trace_to_players;

	_asm
	{
		mov		eax, filter
		lea		ecx, tr
		push	ecx
		push	eax
		push	mask
		lea		edx, vec_abs_end
		lea		ecx, vec_abs_start
		call	dw_address
		add		esp, 0xC
	}
}

const std::map<size_t, Item_t> k_weapon_info =
{
	{ WEAPON_KNIFE,{ _w( "models/weapons/v_knife_default_ct.mdl" ),_w( "knife_default_ct" ) } },
{ WEAPON_KNIFE_T,{ _w( "models/weapons/v_knife_default_t.mdl" ),_w( "knife_t" ) } },
{ WEAPON_KNIFE_BAYONET,{ _w( "models/weapons/v_knife_bayonet.mdl" ), _w( "bayonet" ) } },
{ WEAPON_KNIFE_FLIP,{ _w( "models/weapons/v_knife_flip.mdl" ), _w( "knife_flip" ) } },
{ WEAPON_KNIFE_GUT,{ _w( "models/weapons/v_knife_gut.mdl" ), _w( "knife_gut" ) } },
{ WEAPON_KNIFE_KARAMBIT,{ _w( "models/weapons/v_knife_karam.mdl" ), _w( "knife_karambit" ) } },
{ WEAPON_KNIFE_M9_BAYONET,{ _w( "models/weapons/v_knife_m9_bay.mdl" ),_w( "knife_m9_bayonet" ) } },
{ WEAPON_KNIFE_TACTICAL,{ _w( "models/weapons/v_knife_tactical.mdl" ), _w( "knife_tactical" ) } },
{ WEAPON_KNIFE_FALCHION,{ _w( "models/weapons/v_knife_falchion_advanced.mdl" ),_w( "knife_falchion" ) } },
{ WEAPON_KNIFE_SURVIVAL_BOWIE,{ _w( "models/weapons/v_knife_survival_bowie.mdl" ), _w( "knife_survival_bowie" ) } },
{ WEAPON_KNIFE_BUTTERFLY,{ _w( "models/weapons/v_knife_butterfly.mdl" ),_w( "knife_butterfly" ) } },
{ WEAPON_KNIFE_PUSH,{ _w( "models/weapons/v_knife_push.mdl" ),_w( "knife_push" ) } },
{ GLOVE_STUDDED_BLOODHOUND,{ _w( "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" ) } },
{ GLOVE_T_SIDE,{ _w( "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl" ) } },
{ GLOVE_CT_SIDE,{ _w( "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl" ) } },
{ GLOVE_SPORTY,{ _w( "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" ) } },
{ GLOVE_SLICK,{ _w( "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" ) } },
{ GLOVE_LEATHER_WRAP,{ _w( "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" ) } },
{ GLOVE_MOTORCYCLE,{ _w( "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" ) } },
{ GLOVE_SPECIALIST,{ _w( "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" ) } }
};

const std::vector<WeaponName_t> k_knife_names =
{
{ 0,_w( "Default" ) },
{ WEAPON_KNIFE_BAYONET,_w( "Bayonet" ) },
{ WEAPON_KNIFE_FLIP, _w( "Flip Knife" ) },
{ WEAPON_KNIFE_GUT, _w( "Gut Knife" ) },
{ WEAPON_KNIFE_KARAMBIT, _w( "Karambit" ) },
{ WEAPON_KNIFE_M9_BAYONET, _w( "M9 Bayonet" ) },
{ WEAPON_KNIFE_TACTICAL, _w( "Huntsman Knife" ) },
{ WEAPON_KNIFE_FALCHION, _w( "Falchion Knife" ) },
{ WEAPON_KNIFE_SURVIVAL_BOWIE,_w( "Bowie Knife" ) },
{ WEAPON_KNIFE_BUTTERFLY, _w( "Butterfly Knife" ) },
{ WEAPON_KNIFE_PUSH, _w( "Shadow Daggers" ) }
};

const std::vector<WeaponName_t> k_glove_names =
{
	{ 0, _w( "Default" ) },
{ GLOVE_STUDDED_BLOODHOUND, _w( "Bloodhound" ) },
{ GLOVE_T_SIDE,_w( "Default (Terrorists)" ) },
{ GLOVE_CT_SIDE, _w( "Default (Counter-Terrorists)" ) },
{ GLOVE_SPORTY,_w( "Sporty" ) },
{ GLOVE_SLICK, _w( "Slick" ) },
{ GLOVE_LEATHER_WRAP,_w( "Handwrap" ) },
{ GLOVE_MOTORCYCLE, _w( "Motorcycle" ) },
{ GLOVE_SPECIALIST, _w( "Specialist" ) }
};

const std::vector<WeaponName_t> k_weapon_names =
{
{ GLOVE_STUDDED_BLOODHOUND, _w( "Bloodhound" ) },
{ GLOVE_SPORTY,_w( "Sporty" ) },
{ GLOVE_SLICK, _w( "Slick" ) },
{ GLOVE_LEATHER_WRAP, _w( "Handwrap" ) },
{ GLOVE_MOTORCYCLE, _w( "Motorcycle" ) },
{ GLOVE_SPECIALIST, _w( "Specialist" ) },
{ WEAPON_KNIFE_BAYONET, _w( "Bayonet" ) },
{ WEAPON_KNIFE_FLIP, _w( "Flip Knife" ) },
{ WEAPON_KNIFE_GUT, _w( "Gut Knife" ) },
{ WEAPON_KNIFE_KARAMBIT,_w( "Karambit" ) },
{ WEAPON_KNIFE_M9_BAYONET, _w( "M9 Bayonet" ) },
{ WEAPON_KNIFE_TACTICAL,_w( "Huntsman Knife" ) },
{ WEAPON_KNIFE_FALCHION,_w( "Falchion Knife" ) },
{ WEAPON_KNIFE_SURVIVAL_BOWIE, _w( "Bowie Knife" ) },
{ WEAPON_KNIFE_BUTTERFLY, _w( "Butterfly Knife" ) },
{ WEAPON_KNIFE_PUSH, _w( "Shadow Daggers" ) },
{ 7,_w( "AK-47" ) },
{ 8, _w( "AUG" ) },
{ 9, _w( "AWP" ) },
{ 63,_w( "CZ75 Auto" ) },
{ 1,_w( "Desert Eagle" ) },
{ 2,_w( "Dual Berettas" ) },
{ 10,_w( "FAMAS" ) },
{ 3, _w( "Five-SeveN" ) },
{ 11,_w( "G3SG1" ) },
{ 13,_w( "Galil AR" ) },
{ 4, _w( "Glock-18" ) },
{ 14,_w( "M249" ) },
{ 60,_w( "M4A1-S" ) },
{ 16,_w( "M4A4" ) },
{ 17,_w( "MAC-10" ) },
{ 27,_w( "MAG-7" ) },
{ 33,_w( "MP7" ) },
{ 34,_w( "MP9" ) },
{ 28,_w( "Negev" ) },
{ 35, _w( "Nova" ) },
{ 32,_w( "P2000" ) },
{ 36, _w( "P250" ) },
{ 19,_w( "P90" ) },
{ 26,_w( "PP-Bizon" ) },
{ 64,_w( "R8 Revolver" ) },
{ 29,_w( "Sawed-Off" ) },
{ 38,_w( "SCAR-20" ) },
{ 40,_w( "SSG 08" ) },
{ 39,_w( "SG 553" ) },
{ 30,_w( "Tec-9" ) },
{ 24,_w( "UMP-45" ) },
{ 61,_w( "USP-S" ) },
{ 25, _w( "XM1014" ) },
};

std::vector<std::string> k_pure_weapon_names;
std::vector<size_t> k_idx_to_defidex;