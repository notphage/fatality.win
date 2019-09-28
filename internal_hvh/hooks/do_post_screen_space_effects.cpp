#include "../include_cheat.h"

bool _fastcall hooks::do_post_screen_space_effects( void* ecx, void* edx, CViewSetup* setup )
{
	if ( g_unload )
		return orig_do_post_screen_space_effects( ecx, setup );

	glow::get().draw();

	if ( g_pLocalPlayer && g_pLocalPlayer->get_alive() && g_pInput->m_fCameraInThirdPerson )
	{
		g_in_lp_draw = true;
		g_pLocalPlayer->DrawModel( STUDIO_RENDER, 1 );
		g_in_lp_draw = false;
	}

	g_in_gl = true;
	const auto ret = orig_do_post_screen_space_effects( ecx , setup );
	g_in_gl = false;

	return ret;
}