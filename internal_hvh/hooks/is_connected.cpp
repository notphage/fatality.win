#include "../include_cheat.h"
#include <intrin.h>

bool __fastcall hooks::is_connected( void* ecx, void* edx )
{
	static auto ret = reinterpret_cast< uintptr_t* >( sig( "client.dll", "75 04 B0 01 5F" ) - 2);

	if ( _ReturnAddress() == ret ) // enable inventory
		return false;

	return orig_is_connected( ecx );
}
