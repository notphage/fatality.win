#include "../include_cheat.h"

inline bool invalidChar( char c )
{
	return !( c >= 0 && c < 128 );
}
inline void stripUnicode( std::string & str )
{
	str.erase( remove_if( str.begin(), str.end(), invalidChar ), str.end() );
}

model_t* C_BaseEntity::get_model() { return *reinterpret_cast< model_t** > ( reinterpret_cast< uintptr_t >( this ) + 0x6C ); }
IClientRenderable * C_BaseEntity::get_renderable() { return  reinterpret_cast< IClientRenderable* > ( reinterpret_cast< uintptr_t >( this ) + 0x4 ); };

bool C_BasePlayer::get_alive() { return this->get_lifestate() == LifeState::ALIVE; }
Vector C_BasePlayer::get_eye_pos()
{
	Vector out;
	util::get_vfunc<void( __thiscall* )( void*, Vector& )>( this, 277 )( this, out );
	return out;
}
bool C_BasePlayer::is_enemy() { return this->get_team() != g_pLocalPlayer->get_team(); }
player_info_t C_BasePlayer::get_player_info() const { player_info_t pinfo; g_pEngine->GetPlayerInfo( this->EntIndex(), &pinfo ); return pinfo; }
std::string C_BasePlayer::get_name() const { auto name = std::string( this->get_player_info().name ); stripUnicode( name ); return name; }
bool C_BaseCombatWeapon::is_grenade() { if ( !this ) return false; return this->get_weapon_id() >= WEAPON_FLASHBANG && this->get_weapon_id() <= WEAPON_INCGRENADE; }
int C_BaseCombatWeapon::get_weapon_id() { if ( !this ) return -1; return this->get_item_definiton_index(); }
int C_BaseCombatWeapon::get_weapon_type() { if ( !this ) return -1; return this->get_wpn_data()->WeaponType; }
CCSWeaponData* C_BaseCombatWeapon::get_wpn_data_safe()
{
	if ( !this ) return nullptr; return g_pWeaponSystem->GetWpnData( this->get_item_definiton_index() );
}

C_CSPlayer * C_BasePlayer::get_observer()
{
	C_CSPlayer* observer = nullptr;
	const auto spec_handle = this->get_observer_target();
	if ( spec_handle )
		observer = reinterpret_cast< C_CSPlayer* >( g_pEntitiyList->GetClientEntityFromHandle( spec_handle ) );

	return observer;
}

bool C_CSPlayer::setup_bones( lag_record_t& record, int boneMask, matrix3x4_t * pBoneToWorldOut )
{
	alignas( 16 ) matrix3x4_t bone_out[ 128 ];
	const auto ret = bone_setup::get().handle_bone_setup( record, boneMask, bone_out );
	memcpy( pBoneToWorldOut, bone_out, sizeof( matrix3x4_t[ 128 ] ) );
	return ret;
}

bool C_BaseCombatWeapon::is_being_thrown()
{
	if ( this->is_grenade() )
	{
		if ( !this->get_pin_pulled() )
		{
			const auto f_throw_time = this->get_throw_time();

			if ( ( f_throw_time > 0 ) && ( f_throw_time < g_pGlobals->curtime ) )
				return true;
		}
	}

	return false;
}

void C_CSPlayer::set_anim_layers( std::array<C_AnimationLayer, 15>& layers )
{
	auto this_layers = this->get_anim_layers();
	std::copy( std::begin( this_layers ), std::end( this_layers ), std::begin( layers ) );
}

void C_CSPlayer::set_pose_params( std::array<float, 24>& params )
{
	auto this_params = this->get_pose_params();
	std::copy( std::begin( this_params ), std::end( this_params ), std::begin( params ) );
}

void C_CSPlayer::set_pose_params_scaled( std::array<float, 24>& params )
{
	typedef bool( __thiscall* GetPoseParameterRange_t )( void*, int, float&, float& );
	static auto GetPoseParameterRange = reinterpret_cast< GetPoseParameterRange_t >( sig( "client.dll", "55 8B EC 56 8B F1 83 BE ? ? ? ? ? 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0 74 07 8B CE E8 ? ? ? ? 8B 8E ? ? ? ? 85 C9 74 49 83 39 00 74 44 8B 55 08" ) );

	std::array<float, 24> unscaled_params;
	for ( auto i = 0; i < 24; i++ )
	{
		auto min = 0.0f, max = 0.0f;
		if ( !GetPoseParameterRange( this, params[ i ], min, max ) )
			continue;

		const auto value = ( 1.0 - params[ i ] ) * min + params[ i ] * max;

		unscaled_params[ i ] = value;
	}

	for ( auto i = 0; i < 24; i++ )
		this->set_pose_param( i, unscaled_params[ i ] );
}

void C_CSPlayer::set_pose_param( int param, float value )
{
	static auto Studio_SetPoseParameter = reinterpret_cast< void* >( sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 08 F3 0F 11 54 24 ? 85 D2" ) );

	auto result = 0.0f;
	auto hdr = this->get_model_ptr();

	__asm {
		pushad
		movss xmm2, [ value ]
		lea eax, [ result ]
		push eax
		mov edx, param
		mov ecx, hdr
		call Studio_SetPoseParameter
		pop eax
		popad
	}

	this->get_pose_params()[ param ] = result;
}

int C_CSPlayer::get_sequence_act( int sequence )
{
	const auto model = this->GetModel();
	if ( !model )
		return -1;

	const auto hdr = g_pModelInfo->GetStudioModel( model );
	if ( !hdr )
		return -1;

	static auto get_sequence_act = reinterpret_cast< int( __fastcall* )( void*, studiohdr_t*, int ) >( offsets::get_sequence_act );
	return get_sequence_act( this, hdr, sequence );
}
