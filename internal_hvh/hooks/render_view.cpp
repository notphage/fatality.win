#include "../include_cheat.h"

void __fastcall hooks::render_view( void* thisptr, void*, CViewSetup& setup, CViewSetup& hudViewSetup, unsigned int nClearFlags, int whatToDraw )
{
	if ( g_unload )
		return orig_render_view( thisptr, setup, hudViewSetup, nClearFlags, whatToDraw );

	aimbot::get().no_visual_recoil( setup );

	beams::get().draw_impacts();

	if ( vars::misc.vm_fov_enabled.get<bool>() )
		setup.fovViewmodel = vars::misc.vm_fov.get<float>();

	orig_render_view( thisptr, setup, hudViewSetup, nClearFlags, whatToDraw );
}