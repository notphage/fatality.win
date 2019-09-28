#include "../include_cheat.h"
#include <intrin.h>

int	__stdcall hooks::is_box_visible( const Vector& mins, const Vector& maxs )
{
	if ( g_unload )
		return orig_is_box_visible( mins, maxs );

	__( addr, "\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C" );
	static auto retaddr = addr;

	if ( !memcmp( _ReturnAddress(), retaddr, 10 ) )
		return 1;

	return orig_is_box_visible( mins, maxs );
}