#include "../include_cheat.h"

void __stdcall hooks::frame_stage_notify( ClientFrameStage_t stage )
{
	if ( g_erase )
	{
		erase_fn( init::on_startup );
		g_erase = false;
	}

	chams::get().dark_mode();

	if ( g_unload )
		return orig_frame_stage_notify( stage );

	freestanding::get().update_hotkeys( stage );

	misc::remove_smoke( stage );

	misc::remove_flash( stage );

	resolver::get().approve_shots( stage );

	resolver::get().update_missed_shots( stage );

	fake_ping::get().update( stage );

	animations::get().stop( stage );

	animations::get().fix_local_anims( stage );

	inventorychanger::get().update_equipped( stage );

	skinchanger::get().run( stage );

	orig_frame_stage_notify( stage );

	//resolver::get().extrpolate_players( stage );

	resolver::get().collect_wall_detect( stage );

	player_log::get().log( stage );
}