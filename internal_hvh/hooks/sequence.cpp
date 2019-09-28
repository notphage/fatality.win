#include "../include_cheat.h"

enum e_sequence
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,

	SEQUENCE_BUTTERFLY_DRAW = 0,
	SEQUENCE_BUTTERFLY_DRAW2 = 1,
	SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
	SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

	SEQUENCE_FALCHION_IDLE1 = 1,
	SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
	SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
	SEQUENCE_FALCHION_LOOKAT01 = 12,
	SEQUENCE_FALCHION_LOOKAT02 = 13,

	SEQUENCE_DAGGERS_IDLE1 = 1,
	SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
	SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
	SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
	SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

	SEQUENCE_BOWIE_IDLE1 = 1,
};

inline int random_sequence( const int low, const int high )
{
	return rand() % ( high - low + 1 ) + low;
}

const static std::unordered_map<fnv_t, int( *)( int )> animation_fix_map
{
	{ FnvHash( "models/weapons/v_knife_butterfly.mdl" ), []( int sequence ) -> int
{
	switch ( sequence )
	{
		case SEQUENCE_DEFAULT_DRAW:
			return random_sequence( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence( SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03 );
		default:
			return sequence + 1;
	}
} },
{ FnvHash( "models/weapons/v_knife_falchion_advanced.mdl" ), []( int sequence ) -> int
{
	switch ( sequence )
	{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence( SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP );
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random_sequence( SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02 );
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
	}
} },
{ FnvHash( "models/weapons/v_knife_push.mdl" ), []( int sequence ) -> int
{
	switch ( sequence )
	{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random_sequence( SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5 );
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random_sequence( SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1 );
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
	}
} },
{ FnvHash( "models/weapons/v_knife_survival_bowie.mdl" ), []( int sequence ) -> int
{
	switch ( sequence )
	{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
	}
} }
};

void __cdecl hooks::sequence( CRecvProxyData* proxy_data, void* entity, void* output )
{
	if ( !g_pLocalPlayer || !g_pLocalPlayer->get_alive() )
		return orig_sequence( proxy_data, entity, output );

	auto view_model = static_cast< C_BaseViewModel* >( entity );
	if ( view_model && view_model->get_owner() && view_model->get_owner() != INVALID_EHANDLE_INDEX )
	{
		const auto owner = reinterpret_cast< C_CSPlayer* >( g_pEntitiyList->GetClientEntityFromHandle( view_model->get_owner() ) );
		if ( owner == g_pLocalPlayer )
		{
			// Get the filename of the current view model.
			const auto knife_model = g_pModelInfo->GetModel( view_model->get_model_idx() );
			const auto model_name = g_pModelInfo->GetModelName( knife_model );

			const auto name = FnvHash( model_name );

			if ( animation_fix_map.count( name ) )
				proxy_data->m_Value.m_Int = animation_fix_map.at( name )( proxy_data->m_Value.m_Int );
		}
	}

	orig_sequence( proxy_data, entity, output );
}