#include "../include_cheat.h"
#include <intrin.h>

bool __stdcall hooks::create_move( float input_sample_time, CUserCmd* cmd )
{
	g_cmd = cmd;
	g_reliable_state = -1;
	g_sequence_rn = -1;

	if ( !cmd->command_number )
		return orig_create_move( input_sample_time, cmd );

	misc::clan_tag();
	misc::reveal_ranks();

	uintptr_t* frame_pointer;
	__asm mov frame_pointer, ebp;
	g_send_packet = reinterpret_cast< bool* >( *frame_pointer - 0x1C );
	*g_send_packet = true;
	auto wish_angle = cmd->viewangles;

	if ( g_unload )
		return orig_create_move( input_sample_time, cmd );

	fake_ping::rehook();

	misc::get().bhop();
	autostrafer::strafe();
	misc::get().radar();

	prediction::get().start();
	{
		antiaim::get().run();
		aimbot::get().run();
		aimbot::get().no_recoil();
	}
	prediction::get().finish();

	if ( cmd->buttons & IN_ATTACK && !aimbot::get().get_aimbot_state() && aimbot::get().can_shoot( prediction::get().get_curtime(), true ) )
		resolver::get().add_shot( prediction::get().get_unpred_eyepos(), nullptr, -1 );

	antiaim::get().fix_movement( wish_angle );

	if ( !g_pClientState->m_nChokedCommands )
	{
		resolver::get().set_last_eye();
		thirdperson::get().set_tp_angle( cmd->viewangles );
	}


	cmd->viewangles.Clamp();
	cmd->sidemove = clamp( cmd->sidemove, -450.f, 450.f );
	cmd->forwardmove = clamp( cmd->forwardmove, -450.f, 450.f );
	cmd->upmove = clamp( cmd->upmove, -320.f, 320.f );

	if ( antiaim::get().get_fake_walk_state() )
	{
		cmd->buttons &= ~IN_MOVERIGHT;
		cmd->buttons &= ~IN_MOVELEFT;
		cmd->buttons &= ~IN_FORWARD;
		cmd->buttons &= ~IN_BACK;
	}

	return false;
}