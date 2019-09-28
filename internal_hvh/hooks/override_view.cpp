#include "../include_cheat.h"

void __stdcall hooks::override_view( CViewSetup* setup )
{
	if ( g_unload )
		return orig_override_view( setup );

	thirdperson::get().run();

	orig_override_view( setup );

	if ( g_pEngine->IsInGame() )
	{
		*g_disable_post_processing = true;

		if ( !g_pLocalPlayer->get_scoped() )
		{
			if ( vars::misc.fov_enabled.get<bool>() )
				setup->fov = vars::misc.fov.get<float>();
		}
		else
		{
			if ( vars::misc.fov_enabled.get<bool>() && vars::misc.fov_scope.get<bool>() )
				setup->fov = vars::misc.fov.get<float>();
		}
	}
}