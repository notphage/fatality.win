#include "../include_cheat.h"
#include <intrin.h>

bool __fastcall hooks::is_hltv( void* ecx, void* edx )
{
	// if wanted, force the game to always call AccumulateLayers.

	if ( g_ishltv_fix )
		return true;

	// if wanted, force game to use abs velocity instead of estimating it.

	static auto setup_velocity = reinterpret_cast< uintptr_t* >( sig( "client.dll", "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80" ) );

	if ( g_setup_vel_fix && _ReturnAddress() == setup_velocity )
		return true;

	return orig_is_hltv( ecx );
}