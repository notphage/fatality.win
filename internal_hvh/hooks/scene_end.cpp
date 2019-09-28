#include "../include_cheat.h"

void __fastcall hooks::scene_end( void * ecx, void * edx )
{
	if ( g_unload )
		return orig_scene_end( ecx );

	orig_scene_end( ecx );

	chams::get().extend_drawing();
}