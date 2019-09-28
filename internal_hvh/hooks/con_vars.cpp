#include "../include_cheat.h"
#include <intrin.h>

bool __fastcall hooks::sv_cheats_get_bool( void* pConVar, void* edx )
{
	if ( g_unload )
		return orig_sv_cheats_get_bool( pConVar );

	if ( reinterpret_cast< uintptr_t >( _ReturnAddress() ) == offsets::cam_think )
		return false;

	return orig_sv_cheats_get_bool( pConVar );
}

int __fastcall hooks::debug_spread_show_get_int( void* pConVar, void* edx )
{
	if ( g_unload )
		return orig_debug_spread_show_get_int( pConVar );


	static auto draw_crosshair_ret = reinterpret_cast< uintptr_t* >( sig( "client.dll", "85 C0 0F 84 ? ? ? ? 8B 07 8B CF 8B 80 04 05 00 00" ) );
	if ( _ReturnAddress() == draw_crosshair_ret && !g_pLocalPlayer->get_scoped() && g_pLocalPlayer->get_alive() && vars::misc.force_crosshair.get<bool>() )
		return 3;

	return orig_debug_spread_show_get_int( pConVar );
}

bool __fastcall hooks::net_showfragments_get_bool( void* pConVar, void* ebx )
{
	static auto read_sub_channel_data_ret = reinterpret_cast< uintptr_t* >( sig( "engine.dll", "85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C" ) );
	static auto check_receiving_list_ret = reinterpret_cast< uintptr_t* >( sig( "engine.dll", "8B 1D ? ? ? ? 85 C0 74 16 FF B6" ) );

	if ( !g_pEngine->IsInGame() || !g_pEngine->IsConnected() )
		return orig_net_showfragments_get_bool( pConVar );

	static uint32_t last_fragment = 0;

	if ( _ReturnAddress() == reinterpret_cast< void* >( read_sub_channel_data_ret ) && last_fragment > 0 )
	{
		const auto data = &reinterpret_cast< uint32_t* >( g_pClientState->m_NetChannel )[ 0x56 ];
		const auto bytes_fragments = reinterpret_cast< uint32_t* >( data )[ 0x43 ];

		if ( bytes_fragments == last_fragment )
		{
			auto& buffer = reinterpret_cast< uint32_t* >( data )[ 0x42 ];
			buffer = 0;
		}
	}

	if ( _ReturnAddress() == check_receiving_list_ret )
	{
		const auto data = &reinterpret_cast< uint32_t* >( g_pClientState->m_NetChannel )[ 0x56 ];
		const auto bytes_fragments = reinterpret_cast< uint32_t* >( data )[ 0x43 ];

		last_fragment = bytes_fragments;
	}

	return orig_net_showfragments_get_bool( pConVar );
}