#pragma once
#include "../misc/netvars.h"
#include "../misc/datamap.h"
#include "../features/structs.h"

class C_BaseEntity : public IClientEntity
{
public:
	VFUNC( 17, get_pred_desc_map(), datamap_t*( __thiscall * )( void* ) )( )

		NETVAR( get_team, int, "CBaseEntity->m_iTeamNum" )
		NETVAR( get_origin, Vector, "CBaseEntity->m_vecOrigin" )
		NETVARA( get_oldorigin, Vector, "CBaseEntity->m_vecOrigin", 0x274 )
		NETVAR( get_simtime, float, "CBaseEntity->m_flSimulationTime" )
		NETVARA( get_oldsimtime, float, "CBaseEntity->m_flSimulationTime", 4 )
		NETVAR( get_spotted, bool, "CBaseEntity->m_bSpotted" )
		NETVAR( get_model_idx, unsigned, "CBaseEntity->m_nModelIndex" )
		NETVAR( get_owner_ent, uint32, "CBaseEntity->m_hOwnerEntity" )
		NETVAR( get_rotation, QAngle, "CBaseEntity->m_angRotation" )
		OFFSET( get_take_damage, int, 0x27C )
		DATAMAPVAR( get_effects, int, "m_fEffects" )
		DATAMAPVAR( get_duck_amt, float, "m_flDuckAmount" )
		DATAMAPVAR( get_eflags, int, "m_iEFlags" )
		DATAMAPVAR( get_abs_velocity, Vector, "m_vecAbsVelocity" )
		DATAMAPVAR( get_abs_rotation, Vector, "m_angAbsRotation" )
		DATAMAPVAR( get_abs_origin, Vector, "m_vecAbsOrigin" )
		MFUNC( set_abs_origin( Vector origin ), void( __thiscall* )( void*, const Vector& ), offsets::set_abs_origin )( origin )
		MFUNC( is_breakable_entity(), bool( __thiscall* )( void* ), offsets::is_breakable_entity )( )
		VFUNC( 75, set_model_idx( int index ), void( __thiscall* )( void*, int ) )( index )
		NETPROP( get_rotation_prop, "CBaseEntity->m_angRotation" )


		model_t* get_model();
	IClientRenderable* get_renderable();
};

class C_BaseCombatCharacter : public C_BaseEntity
{
public:
	PNETVAR( get_weapons, CBaseHandle, "CBaseCombatCharacter->m_hMyWeapons" )
		PNETVAR( get_wearables, CBaseHandle, "CBaseCombatCharacter->m_hMyWearables" )
		PNETVAR( get_active_weapon, CBaseHandle, "CBaseCombatCharacter->m_hActiveWeapon" )
};

class CEconItemDefinition;
class C_BaseAttributableItem : public C_BaseEntity
{
private:
	using str_32 = char[ 32 ];
public:
	NETVAR( get_account_id, int, "CBaseAttributableItem->m_iAccountID" )
		NETVAR( get_item_id, uint64_t, "CBaseAttributableItem->m_iItemIDLow" )
		NETVAR( get_item_id_high, uint32_t, "CBaseAttributableItem->m_iItemIDHigh" )
		NETVAR( get_item_id_low, uint32_t, "CBaseAttributableItem->m_iItemIDLow" )
		NETVAR( get_entity_quality, int, "CBaseAttributableItem->m_iEntityQuality" )
		NETVAR( get_custom_name, str_32, "CBaseAttributableItem->m_szCustomName" )
		NETVAR( get_fallback_paint_kit, unsigned, "CBaseAttributableItem->m_nFallbackPaintKit" )
		NETVAR( get_fallback_seed, unsigned, "CBaseAttributableItem->m_nFallbackSeed" )
		NETVAR( get_fallback_wear, float, "CBaseAttributableItem->m_flFallbackWear" )
		NETVAR( get_fallback_stat_trak, unsigned, "CBaseAttributableItem->m_nFallbackStatTrak" )
		NETVAR( get_item_definiton_index, int, "CBaseAttributableItem->m_iItemDefinitionIndex" )
		NETVAR( get_xuid_low, int, "CBaseAttributableItem->m_OriginalOwnerXuidLow" )
		NETVAR( get_xuid_high, int, "CBaseAttributableItem->m_OriginalOwnerXuidHigh" )
		POFFSET( get_econ_item_def, CEconItemDefinition, 0x2DB0 )
		FUNC( get_diplay_name(), wchar_t*( __thiscall* )( void*, bool ), offsets::get_weapon_name )( this->get_econ_item_def(), true )
};

class C_BaseWeaponWorldModel : public C_BaseEntity {};


class CEconItemDefinition
{
public:
	virtual const char * get_definition_index() = 0;
	virtual const char * get_prefab_name() = 0;
	virtual const char * get_item_base_name() = 0;
	virtual const char * get_item_type_name() = 0;
	virtual const char * get_item_desc() = 0;
	virtual const char * get_inventory_image() = 0;
	int GetEquippedPosition()
	{
		return *reinterpret_cast< int* >( ( uintptr_t )this + 0x24C );
	}
};

class IRefCounted;
class CEconItem;

class C_EconItemView
{
public:
	CEconItemDefinition * get_static_data()
	{
		static auto fnGetStaticData = reinterpret_cast< CEconItemDefinition*( __thiscall* )( void* ) >( sig( "client.dll", "55 8B EC 51 56 57 8B F1 E8 ? ? ? ? 0F B7 8E" ) );
		return fnGetStaticData( this );
	}
	CEconItem* GetSOCData()
	{
		static auto fnGetSOCData = reinterpret_cast< CEconItem*( __thiscall* )( C_EconItemView* ) >( sig( "client.dll", "55 8B EC 83 E4 F0 83 EC 18 56 8B F1 57 8B 86" ) );

		return fnGetSOCData( this );
	}
	int InitializeAttributes()
	{
		static auto fnInitializeAttributes = reinterpret_cast< int( __thiscall* )( void* ) >( sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86" ) );

		return fnInitializeAttributes( this );
	}
	char _pad_0x0000[ 0x14 ];
	CUtlVector<IRefCounted*>	m_CustomMaterials; //0x0014
	char _pad_0x0034[ 0x160 ];
	__int32                     m_iItemDefinitionIndex; //0x0194 
	__int32                     m_iEntityQuality; //0x0198 
	__int32                     m_iEntityLevel; //0x019C 
	char _pad_0x01A0[ 0x8 ]; //0x01A0
	__int32                     m_iItemIDHigh; //0x01A8 
	__int32                     m_iItemIDLow; //0x01AC 
	__int32                     m_iAccountID; //0x01B0 
	char pad_0x01B4[ 0x8 ]; //0x01B4
	unsigned char               m_bInitialized; //0x01BC 
	char pad_0x01BD[ 0x63 ]; //0x01BD
	CUtlVector<IRefCounted*>    m_VisualsDataProcessors; //0x0220
};

class C_AttributeManager
{
public:
	char	_pad_0x0000[ 0x18 ];
	__int32						m_iReapplyProvisionParity; //0x0018 
	DWORD						m_hOuter; //0x001C 
	char	_pad_0x0020[ 0x4 ];
	__int32						m_ProviderType; //0x0024 
	char	_pad_0x0028[ 0x18 ];
	C_EconItemView				m_Item; //0x0040 
};

class C_WeaponCSBase : public C_BaseAttributableItem
{
public:
	char	_pad_0x0000[ 0x09CC ];
	CUtlVector<IRefCounted*>	m_CustomMaterials; //0x09DC
	char	_pad_0x09F0[ 0x2380 ];
	C_AttributeManager			m_AttributeManager; //0x2D70
	char	_pad_0x2D84[ 0x2F9 ];
	bool						m_bCustomMaterialInitialized; //0x32DD
};

class C_BaseCombatWeapon : public C_WeaponCSBase
{
public:
	NETVAR( get_next_primary_attack, float, "CBaseCombatWeapon->m_flNextPrimaryAttack" )
		NETVAR( get_postpone_fire_ready_time, float, "CWeaponCSBase->m_flPostponeFireReadyTime" )
		NETVAR( get_clip1, int, "CBaseCombatWeapon->m_iClip1" )
		NETVAR( get_max_clip, int, "CBaseCombatWeapon->m_iPrimaryReserveAmmoCount" )
		OFFSET( in_reload, bool, 0x3235 )
		NETVAR( get_throw_time, float, "CBaseCSGrenade->m_fThrowTime" )
		NETVAR( get_pin_pulled, bool, "CBaseCSGrenade->m_bPinPulled" )
		NETVAR( get_world_model_handle, CBaseHandle, "CBaseCombatWeapon->m_hWeaponWorldModel" )
		VFUNC( 437, get_spread(), float( __thiscall* )( void* ) )( )
		VFUNC( 468, get_inaccuracy(), float( __thiscall* )( void* ) )( )
		VFUNC( 469, update_accuracy(), void( __thiscall* )( void* ) )( )
		VFUNC( 445, get_wpn_data(), CCSWeaponData*( __thiscall* )( void* ) )( )

		bool is_grenade();
	int get_weapon_id();
	int get_weapon_type();
	bool is_knife();
	bool is_being_thrown();
	CCSWeaponData* get_wpn_data_safe();
	C_EconItemView* get_econ_item_view()
	{
		static auto Address = sig( "client.dll", "E8 ? ? ? ? 89 44 24 28 85 C0 0F 84 ? ? ? ? 8B C8" );
		static auto fnGetEconItemView = reinterpret_cast< C_EconItemView*( __thiscall* )( void* ) >( *reinterpret_cast< uintptr_t* >( Address + 1 ) + Address + 5 );

		return fnGetEconItemView( this );
	}
};

class C_BasePlayer : public C_BaseCombatCharacter
{
public:
	NETVAR( get_lifestate, LifeState, "CBasePlayer->m_lifeState" )
		NETVAR( get_health, int, "CBasePlayer->m_iHealth" )
		NETVAR( get_flags, int, "CBasePlayer->m_fFlags" )
		NETVAR( get_tickbase, int, "CBasePlayer->m_nTickBase" )
		NETVAR( get_velocity, Vector, "CBasePlayer->m_vecVelocity[0]" )
		NETVAR( get_aim_punch, QAngle, "CBasePlayer->m_aimPunchAngle" )
		NETVAR( get_view_punch, QAngle, "CBasePlayer->m_viewPunchAngle" )
		NETVAR( get_observer_mode, int, "CBasePlayer->m_iObserverMode" )
		NETVAR( get_observer_target, CBaseHandle, "CBasePlayer->m_hObserverTarget" )
		NETVAR( get_view_model, CBaseHandle, "CBasePlayer->m_hViewModel[0]" )
		DATAMAPVAR( get_move_type, int, "m_MoveType" )
		DATAMAPVAR( get_next_attack, float, "m_flNextAttack" )
		OFFSET( get_spawn_time, float, 0xA290 )
		VFUNC( 11, get_abs_angles(), QAngle&( __thiscall* )( void* ) )( )
		MFUNC( set_abs_angles( QAngle ang ), void( __thiscall* )( void*, const QAngle& ), offsets::set_abs_angles )( ang )
		MFUNC( invalidate_physics_recursive( int physbit ), void( __thiscall* )( void*, int ), offsets::invalidate_physics_recursive )( physbit )

		bool get_alive();
	Vector get_eye_pos();
	bool is_enemy();
	player_info_t get_player_info() const;
	std::string get_name() const;
	C_CSPlayer* get_observer();
};

class CBoneAccessor;
class CBoneCache;
class CStudioHdr;
class CThreadFastMutex;
class C_BaseAnimating : public C_BasePlayer
{
public:
	NETVAR( get_hitbox_set, int, "CBaseAnimating->m_nHitboxSet" )
		ANETVAR( get_pose_params, float, 24, "CBaseAnimating->m_flPoseParameter" )
		NETVAR( get_model_scale, float, "CBaseAnimating->m_flModelScale" )
		NETVAR( get_clientside_animation, bool, "CBaseAnimating->m_bClientSideAnimation" )
		DATAMAPVAR( get_playback_rate, int, "m_flPlaybackRate" )
		OFFSET( get_model_ptr, CStudioHdr*, 0x293C )
		OFFSET( get_enable_invalidate_bone_cache, bool, offsets::enable_invalidate_bone_cache )
		OFFSET( get_writable_bones, int, 0x2694 );
	POFFSET( get_bone_accessor, CBoneAccessor, 0x2698 )
		POFFSET( get_bone_cache, CBoneCache, 0x2900 )
		POFFSET( get_mutex, CThreadFastMutex, 0x2660 )
};

class CCSGOPlayerAnimState;
class QuaternionAligned;
class Quaternion;
class C_AnimationLayer;
class CIKContext;
struct studiohdr_t;
class C_CSPlayer : public C_BaseAnimating
{
public:
	NETVAR( get_lby, float, "CCSPlayer->m_flLowerBodyYawTarget" )
		NETVAR( get_helmet, bool, "CCSPlayer->m_bHasHelmet" )
		NETVAR( get_armor, int, "CCSPlayer->m_ArmorValue" )
		NETVAR( get_heavy_armor, bool, "CCSPlayer->m_bHasHeavyArmor" )
		NETVAR( get_eye_angles, QAngle, "CCSPlayer->m_angEyeAngles" )
		NETVAR( get_flash_alpha, float, "CCSPlayer->m_flFlashMaxAlpha" )
		NETVAR( get_scoped, bool, "CCSPlayer->m_bIsScoped" )
		NETVAR( get_immunity, bool, "CCSPlayer->m_bGunGameImmunity" )
		NETVAR( has_defuser, bool, "CCSPlayer->m_bHasDefuser" )
		OFFSET( get_tp_angle, QAngle, 0x31C8 )
		OFFSET( get_last_lby, float, 0x31C8 + 4 )
		OFFSET( get_context, CIKContext*, 0x2660 )
		OFFSET( get_occlusion_flags, int, 0xA28 )
		APOFFSET( get_anim_layers, C_AnimationLayer, 15, 0x2970 )
		OFFSET( get_anim_layer_count, int, 0x2970 + 12 )
		OFFSET( get_anim_state, CCSGOPlayerAnimState*, 0x3874 )
		FUNC( create_anim_state( CCSGOPlayerAnimState* state ), void( __thiscall* )( CCSGOPlayerAnimState*, C_BasePlayer* ), offsets::create_anim_state )( state, this )
		FUNC( standard_blending_rules( CStudioHdr* hdr, Vector* pos, Quaternion *q, float curTime, int boneMask ), void( __thiscall* )( void*, CStudioHdr*, Vector*, Quaternion*, float, int ), offsets::standard_blending_rules )( this, hdr, pos, q, curTime, boneMask )
		FUNC( build_transformations( CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& cameraTransform, int boneMask, byte* boneComputed ), void( __thiscall* )( void*, CStudioHdr*, Vector*, Quaternion*, matrix3x4_t const&, int, byte* ), offsets::build_transformations )( this, hdr, pos, q, cameraTransform, boneMask, boneComputed )
		VFUNC( 218, update_clientside_anim(), void( __thiscall* )( void* ) )( )

		NETPROP( get_lby_prop, "CCSPlayer->m_flLowerBodyYawTarget" )
		NETPROP( get_eye_angles0_prop, "CCSPlayer->m_angEyeAngles[0]" )
		NETPROP( get_eye_angles1_prop, "CCSPlayer->m_angEyeAngles[1]" )

		static bool setup_bones( lag_record_t& record, int boneMask, matrix3x4_t* pBoneToWorldOut );
	void set_anim_layers( std::array<C_AnimationLayer, 15>& layers );
	void set_pose_params( std::array<float, 24>& params );
	void set_pose_params_scaled( std::array<float, 24>& params );
	void set_pose_param( int param, float value );
	int get_sequence_act( int sequence );
};

class C_PlantedC4 : public C_BaseEntity
{
public:
	NETVAR( get_defuser, int32, "CPlantedC4->m_hBombDefuser" )
		NETVAR( get_defused, bool, "CPlantedC4->m_bBombDefused" )
		NETVAR( get_ticking, bool, "CPlantedC4->m_bBombTicking" )
		NETVAR( get_timer, float, "CPlantedC4->m_flC4Blow" )
};

class CSmokeGrenadeProjectile
{
public:
	NETPROP( get_smoke_effect_prop, "CSmokeGrenadeProjectile->m_bDidSmokeEffect" )
};

class C_BaseViewModel : public C_BaseEntity
{
public:
	NETVAR( get_owner, int32, "CBaseViewModel->m_hOwner" )
		NETVAR( get_weapon, CBaseHandle, "CBaseViewModel->m_hWeapon" )
		NETPROP( get_sequen_prop, "CBaseViewModel->m_nSequence" )
};



class C_locPlayer
{
	friend bool operator==( const C_locPlayer& lhs, void* rhs ) { return *lhs.m_loc == rhs; }
public:
	C_locPlayer() : m_loc( nullptr ) {}

	operator bool() const { return *m_loc != nullptr; }
	operator C_CSPlayer*( ) const { return *m_loc; }

	C_CSPlayer* operator->() const { return *m_loc; }

private:
	C_CSPlayer * *m_loc;
};
