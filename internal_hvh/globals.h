#pragma once
#include <map>

struct Item_t;
struct WeaponName_t;
class C_CSGameRulesProxy;
struct VMatrix;
class C_locPlayer;
class C_CSPlayer;
class C_BaseEntity;
class CUserCmd;
class C_BaseCombatWeapon;
class Vector;
class ITraceFilter;
class CGameTrace;
class Color;
struct matrix3x4_t;
struct weaponconfig_t;
struct antiaimconfig_t;
typedef CGameTrace trace_t;
using CBaseHandle = unsigned long;

extern HMODULE g_module;
extern bool g_erase;

extern C_locPlayer g_pLocalPlayer;
extern C_CSPlayer* get_entity( int index );
extern C_BaseCombatWeapon* get_weapon( const CBaseHandle* handle );
extern weaponconfig_t* get_config( C_BaseCombatWeapon* weapon );
extern antiaimconfig_t* get_antiaim( const int type );
extern uintptr_t g_GCClientSystem;
extern float* g_asdasd;

extern bool* g_send_packet;
extern CUserCmd* g_cmd;
extern float g_inaccuracy;

extern void util_trace_line( const Vector& vec_start, const Vector& vec_end, unsigned int n_mask, C_BaseEntity* p_cs_ignore, trace_t* p_trace );
extern void util_trace_hull( const Vector & vec_start, const Vector & vec_end, unsigned int n_mask, const Vector & extents, trace_t * p_trace );
extern void util_clip_trace_to_players( const Vector & vec_abs_start, const Vector & vec_abs_end, unsigned int mask, ITraceFilter * filter, trace_t * tr );

extern bool g_unload;
extern bool g_in_hbp;
extern bool g_in_gl;
extern bool g_ishltv_fix;
extern bool g_setup_vel_fix;
extern bool g_in_lp_draw;

extern int g_reliable_state;
extern int g_sequence_rn;

extern VMatrix g_vmatrix;

extern int g_tick_shot;

extern C_CSGameRulesProxy** g_ppGameRules;

HRESULT CreateVertexBuffer(
	UINT Length,
	DWORD Usage,
	DWORD FVF,
	D3DPOOL Pool,
	IDirect3DVertexBuffer9** ppVertexBuffer,
	HANDLE* pHandle
);

HRESULT Lock(
	UINT OffsetToLock,
	UINT SizeToLock,
	VOID** ppbData,
	DWORD Flags
);


extern LPDIRECT3DVERTEXBUFFER9 g_pVB;

extern bool* g_disable_post_processing;

extern matrix3x4_t g_trace_hitboxbones[ 128 ];
extern C_CSPlayer* g_trace_entity;

extern float( *random_float ) ( float from, float to );
extern void( *random_seed ) ( unsigned int seed );

template <typename T>
auto get_col( const T val )
{
	Color ret;
	ret[ 0 ] = ( ( ( val ) & 0x00ff0000 ) >> 16 );
	ret[ 1 ] = ( ( ( val ) & 0x0000ff00 ) >> 8 );
	ret[ 2 ] = ( ( val ) & 0x000000ff );
	ret[ 3 ] = ( ( ( val ) & 0xff000000 ) >> 24 );

	return ret;
}

extern const std::map<size_t, Item_t> k_weapon_info;
extern const std::vector<WeaponName_t> k_knife_names;
extern const std::vector<WeaponName_t> k_weapon_names;
extern std::vector<std::string> k_pure_weapon_names;
extern std::vector<size_t> k_idx_to_defidex;