#include "../include_cheat.h"

void autostrafer::strafe()
{
	if ( !vars::misc.autostrafe.get<bool>() )
		return;

	if ( g_pLocalPlayer->get_flags() & FL_ONGROUND || !GetAsyncKeyState( VK_SPACE ) )
		return;

	if ( g_pLocalPlayer->get_move_type() == MOVETYPE_NOCLIP || g_pLocalPlayer->get_move_type() == MOVETYPE_LADDER || !g_pLocalPlayer->get_alive() ) return;

	// If we're not jumping or want to manually move out of the way/jump over an obstacle don't strafe.
	if ( g_cmd->buttons & IN_MOVELEFT ||
		 g_cmd->buttons & IN_MOVERIGHT ||
		 g_cmd->buttons & IN_BACK )
		return;


	if ( g_cmd->mousedx > 1 || g_cmd->mousedx < -1 )
	{
		g_cmd->sidemove = std::clamp( g_cmd->mousedx < 0.f ? -400.f : 400.f, -400.f, 400.f );
	}
	else
	{
		const auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
		if ( !( get_config( weapon )->index == 1 && get_config( weapon )->scout_jumpshot->get<bool>() ) )
		{
			if ( g_pLocalPlayer->get_velocity().Length2D() == 0 || g_pLocalPlayer->get_velocity().Length2D() == NAN || g_pLocalPlayer->get_velocity().Length2D() == INFINITE )
			{
				g_cmd->forwardmove = 400;
				return;
			}
			g_cmd->forwardmove = std::clamp( 5850.f / g_pLocalPlayer->get_velocity().Length2D(), -400.f, 400.f );
		}
		if ( g_cmd->forwardmove < -400 || g_cmd->forwardmove > 400 )
			g_cmd->forwardmove = 0;
		g_cmd->sidemove = std::clamp( ( g_cmd->command_number % 2 ) == 0 ? -400.f : 400.f, -400.f, 400.f );
		if ( g_cmd->sidemove < -400 || g_cmd->sidemove > 400 )
			g_cmd->sidemove = 0;
	}
}
