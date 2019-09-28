#include "../include_cheat.h"

struct ClientAnimating
{
	C_BaseAnimating* entity;
	uint32_t flags;
};

typedef CUtlVector< ClientAnimating >ClientSideAnimationList;

void animations::stop( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_RENDER_START )
		return;

	static auto clientSideAnimationList = *reinterpret_cast< ClientSideAnimationList** >( util::get_sig( xorstr( "client.dll" ), xorstr( "A1 ? ? ? ? F6 44" ) ) + 1 );

	const auto list = clientSideAnimationList;

	for ( auto i = 0; i < list->Count(); i++ )
	{
		auto& animating = list->Element( i );

		if ( animating.entity && animating.entity->GetClientClass()->m_ClassID == 35 )
		{
			if ( animating.entity == g_pLocalPlayer )
			{
				if ( g_pInput->m_fCameraInThirdPerson && animating.flags & 1 )
				{
					animating.flags &= ~1;
					g_pLocalPlayer->set_abs_angles( thirdperson::get().get_tp_angle() );
				}
				else if ( !g_pInput->m_fCameraInThirdPerson )
					animating.flags |= 1;
			}
		}
	}
}

void animations::update_local_animations()
{
	const auto curtime = g_pGlobals->curtime;
	const auto frametime = g_pGlobals->frametime;

	static auto wasonground = true;

	g_pGlobals->curtime = g_pLocalPlayer->get_simtime();
	g_pGlobals->frametime = g_pGlobals->interval_per_tick;

	const auto player_flags = g_pLocalPlayer->get_flags();

	if ( !wasonground || !( g_pLocalPlayer->get_flags() & FL_ONGROUND ) )
	{
		g_pLocalPlayer->get_flags() &= ~FL_ONGROUND;
	}

	static auto absangle_recalc = *reinterpret_cast< bool** > ( util::get_sig( xorstr( "client.dll" ), xorstr( "80 3D ? ? ? ? ? 56 57 8B F9 0F 84 ? ? ? ? F7" ) ) + 2 );
	g_setup_vel_fix = true;
	*absangle_recalc = true;
	g_pLocalPlayer->update_clientside_anim();
	*absangle_recalc = false;
	g_setup_vel_fix = false;

	const auto animstate = *reinterpret_cast< CCSGOPlayerAnimState** >( uintptr_t( static_cast< C_CSPlayer* >( g_pLocalPlayer ) ) + 0x3874 );
	if ( animstate->m_bOnGround )
		thirdperson::get().set_abs_angles( QAngle( 0.f, animstate->m_flCurrentFeetYaw, 0.f ) );
	else
		thirdperson::get().set_abs_angles( QAngle( 0.f, g_pLocalPlayer->get_abs_angles().y, 0.f ) );

	g_pLocalPlayer->get_flags() = player_flags;

	wasonground = g_pLocalPlayer->get_flags() & FL_ONGROUND;

	g_pGlobals->frametime = frametime;
	g_pGlobals->curtime = curtime;
}

void animations::fix_local_anims( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_RENDER_START )
		return;

	const auto reset = ( !g_pLocalPlayer || !g_pLocalPlayer->get_alive() );

	if ( reset )
		return;

	if ( !g_pClientState->m_nChokedCommands )
		update_custom_anims( g_pLocalPlayer );

	auto &animstate = **reinterpret_cast< CCSGOPlayerAnimState** >( uintptr_t( static_cast< C_CSPlayer* >( g_pLocalPlayer ) ) + 0x3874 );

	if ( !g_pInput->m_fCameraInThirdPerson )
	{
		animstate.m_bInHitGroundAnimation = false;
		return;
	}

	if ( g_pClientState->m_nChokedCommands )
		return;

	auto tp_angle = thirdperson::get().get_tp_angle();

	g_pPrediction->SetLocalViewAngles( tp_angle );
	tp_angle.x = 0.f;
	g_pLocalPlayer->set_abs_angles( tp_angle );

	if ( animstate.m_iLastClientSideAnimationUpdateFramecount >= g_pGlobals->framecount )
		animstate.m_iLastClientSideAnimationUpdateFramecount = g_pGlobals->framecount - 1;

	update_local_animations();
}


void animations::update_player_animations( C_CSPlayer* player )
{
	const auto log = &player_log::get().get_log( player->EntIndex() );
	const auto animstate = player->get_anim_state();

	if ( player->get_spawn_time() != log->m_flSpawntime )
	{
		log->record.clear();
		log->m_flSpawntime = player->get_spawn_time();
	}

	log->record.emplace_front( lag_record_t( player ) );
	const auto record = &log->record.front();

	// backup curtime.
	float curtime = g_pGlobals->curtime;
	float frametime = g_pGlobals->frametime;

	// set curtime to animtime.
	// set frametime to ipt just like on the server during simulation.
	g_pGlobals->curtime = record->m_flAnimTime;
	g_pGlobals->frametime = g_pGlobals->interval_per_tick;

	// backup stuff that we do not want to fuck with.
	auto backup_m_origin = player->get_origin();
	auto backup_m_abs_origin = player->get_abs_origin();
	auto backup_m_velocity = player->get_velocity();
	auto backup_m_abs_velocity = player->get_abs_velocity();
	auto backup_m_flags = player->get_flags();
	auto backup_m_eflags = player->get_eflags();
	auto backup_m_duck = player->get_duck_amt();
	auto backup_m_body = player->get_lby();

	auto backup_m_layers = player->get_anim_layers();

	// is player a bot?
	bool bot = player->get_player_info().fakeplayer;

	// reset fakewalk state.
	log->fakewalking = false;
	//log->m_iMode = Resolver::Modes::RESOLVE_NONE;

	// fix velocity.
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/client/c_baseplayer.cpp#L659

	if ( record->m_lag > 0 && record->m_lag < 16 && log->record.size() >= 2 )
	{
		// get pointer to previous record.
		auto previous = &log->record[ 1 ];

		if ( previous && !previous->m_bDormant )
			record->m_vecVelocity = ( record->m_vecOrigin - previous->m_vecOrigin ) * ( 1.f / ticks_to_time( record->m_lag ) );
	}

	// set this fucker, it will get overriden.
	auto m_anim_velocity = player->get_velocity();

	// fix various issues with the game
	// these issues can only occur when a player is choking data.
	if ( record->m_lag > 1 && !bot )
	{
		// detect fakewalk.
		float speed = record->m_vecVelocity.Length();

		if ( record->m_fFlags & FL_ONGROUND &&  record->m_AnimOverLay[ 6 ].m_flWeight == 0.f && speed > 0.1f && speed < 100.f )
			log->fakewalking = true;

		if ( log->fakewalking )
			m_anim_velocity = record->m_vecVelocity = { 0.f, 0.f, 0.f };

		// we need atleast 2 updates/records
		// to fix these issues.
		if ( log->record.size() >= 2 )
		{
			// get pointer to previous record.
			auto previous = &log->record[ 1 ];

			if ( previous && !previous->m_bDormant )
			{
				// set previous flags.
				player->get_flags() = previous->m_fFlags;

				// strip the on ground flag.
				player->get_flags() &= ~FL_ONGROUND;

				// been onground for 2 consecutive ticks? fuck yeah.
				if ( record->m_fFlags & FL_ONGROUND && previous->m_fFlags & FL_ONGROUND )
					player->get_flags() |= FL_ONGROUND;

				// fix jump_fall.
				else
				{
					if ( record->m_AnimOverLay[ 4 ].m_flWeight != 1.f && previous->m_AnimOverLay[ 4 ].m_flWeight == 1.f &&  record->m_AnimOverLay[ 5 ].m_flWeight != 0.f )
						player->get_flags() |= FL_ONGROUND;

					if ( record->m_fFlags & FL_ONGROUND && !( previous->m_fFlags & FL_ONGROUND ) )
						player->get_flags() &= ~FL_ONGROUND;
				}

				// calc fraction between both records.
				float frac = ( record->m_flAnimTime - record->m_flSimulationTime ) / ( previous->m_flSimulationTime - record->m_flSimulationTime );

				player->get_duck_amt() = math::lerp( record->m_flDuckAmt, previous->m_flDuckAmt, frac );

				if ( !log->fakewalking )
					m_anim_velocity = math::lerp( record->m_vecVelocity, previous->m_vecVelocity, frac );
			}
		}
	}

	const auto simtime_delta = player->get_simtime() - player->get_oldsimtime();
	const auto choked_ticks = ( ( simtime_delta / g_pGlobals->interval_per_tick ) + 0.5 );
	const auto simulation_tick_delta = choked_ticks - 2;
	const auto delta_ticks = ( std::clamp( time_to_ticks( g_pEngine->GetNetChannelInfo()->GetLatency( 1 ) + g_pEngine->GetNetChannelInfo()->GetLatency( 0 ) ) + g_pGlobals->tickcount - time_to_ticks( player->get_simtime() + lagcomp::get_lerp_time() ), 0, 100 ) ) - simulation_tick_delta;

	if ( delta_ticks > 0 && log->record.size() >= 2 )
	{
		auto ticks_left = static_cast< int >( simulation_tick_delta );
		ticks_left = std::clamp( ticks_left, 1, 10 );
		do
		{
			auto data_origin = record->m_vecOrigin;
			auto data_velocity = record->m_vecVelocity;
			auto data_flags = record->m_fFlags;

			lagcomp::extrapolate( player, data_origin, data_velocity, data_flags, !( player->get_flags() & FL_ONGROUND ) );

			record->m_flSimulationTime += g_pGlobals->interval_per_tick;
			record->m_vecOrigin = data_origin;
			record->m_vecVelocity = data_velocity;
			--ticks_left;
		} while ( ticks_left > 0 );
	}

	// shame...
	bool lag = record->m_lag > 1;

	// and not bot, player uses fake angles.
	bool fake = vars::aim.resolver.get<bool>() && ( !player->is_enemy() || vars::aim.resolve_team.get<bool>() ) && !bot /*&& lag*/;

	// if using fake angles, correct angles.
	if ( fake )
	{
	}

	// set stuff before animating.
	player->get_origin() = record->m_vecOrigin;
	player->set_abs_origin( record->m_vecOrigin );

	player->get_velocity() = player->get_abs_velocity() = m_anim_velocity;
	player->get_lby() = record->m_flLowerBodyYawTarget;

	// EFL_DIRTY_ABSVELOCITY
	// skip call to C_BaseEntity::CalcAbsoluteVelocity
	player->get_eflags() &= ~0x1000;

	// run body prediction.
	if ( log->m_bLbyFlick )
	{
		const auto lby = math::normalize_float( player->get_lby() );
		record->m_angEyeAngles.y = lby;
		player->set_abs_angles( QAngle( 0.f, lby, 0.f ) );
	}

	// write potentially resolved angles.
	player->get_eye_angles() = record->m_angEyeAngles;

	// fix animating in same frame.
	if ( animstate->m_iLastClientSideAnimationUpdateFramecount == g_pGlobals->framecount )
		animstate->m_iLastClientSideAnimationUpdateFramecount -= 1;

	// 'm_animating' returns true if being called from SetupVelocity, passes raw velocity to animstate.
	player->get_clientside_animation() = true;
	player->update_clientside_anim();
	player->get_clientside_animation() = false;


	// correct poses if fake angles.
	if ( fake )
	{
		player->get_pose_params()[ 11 ] = ( record->m_angEyeAngles.x + 90 ) / 180;
		player->get_pose_params()[ 2 ] = ( record->m_angEyeAngles.y + 180 ) / 360;
	}

	// store updated/animated poses and rotation in lagrecord.
	record->m_flPoseParameter = player->get_pose_params();
	record->m_angAbsAngles = player->get_abs_angles();

	record->m_bLbyFlick = log->m_bLbyFlick;
	record->m_bShot = log->m_bShot;

	// restore backup data.
	player->get_origin() = backup_m_origin;
	player->get_velocity() = backup_m_velocity;
	player->get_abs_velocity() = backup_m_abs_velocity;
	player->get_flags() = backup_m_flags;
	player->get_eflags() = backup_m_eflags;
	player->get_duck_amt() = backup_m_duck;
	player->get_lby() = backup_m_body;
	player->set_abs_origin( backup_m_abs_origin );
	player->set_anim_layers( backup_m_layers );

	// IMPORTANT: do not restore poses here, since we want to preserve them for rendering.
	// also dont restore the render angles which indicate the model rotation.

	// restore globals.
	g_pGlobals->curtime = curtime;
	g_pGlobals->frametime = frametime;
}

void animations::update_custom_anims( C_CSPlayer * player )
{
	if ( !player )
		return;

	auto& info = anim_info[ player->EntIndex() ];
	const auto simtime_delta = player->get_simtime() - player->get_oldsimtime();
	if ( simtime_delta <= 0.f )
		return;

	if ( player->get_spawn_time() != info.spawn_time || player->GetRefEHandle() != info.handle )
	{
		info.spawn_time = player->get_spawn_time();
		info.handle = player->GetRefEHandle();

		if ( info.animstate )
		{
			g_pMemAlloc->Free( info.animstate );
			info.animstate = nullptr;
		}
	}
	if ( !info.animstate )
	{

		info.animstate = reinterpret_cast< CCSGOPlayerAnimState* > ( g_pMemAlloc->Alloc( 860u ) );
		player->create_anim_state( info.animstate );
	}

	// backup curtime.
	const auto curtime = g_pGlobals->curtime;
	const auto frametime = g_pGlobals->frametime;

	// set curtime to animtime.
	// set frametime to ipt just like on the server during simulation.
	g_pGlobals->curtime = player->get_oldsimtime() + g_pGlobals->interval_per_tick;
	g_pGlobals->frametime = g_pGlobals->interval_per_tick;

	// backup stuff that we do not want to fuck with.
	const auto backup_origin = player->get_origin();
	const auto backup_velocity = player->get_velocity();
	const auto backup_abs_velocity = player->get_abs_velocity();
	const auto backup_flags = player->get_flags();
	const auto backup_eflags = player->get_eflags();
	const auto backup_duck_amt = player->get_duck_amt();
	const auto backup_lby = player->get_lby();
	auto backup_layers = player->get_anim_layers();
	auto backup_poses = player->get_pose_params();

	if ( player != g_pLocalPlayer )
	{

		const auto ChokedTicks = ( ( simtime_delta / g_pGlobals->interval_per_tick ) + 0.5 );
		if ( ChokedTicks < 16 )
		{
			auto speed = player->get_velocity().Length();
			if ( ChokedTicks > 1 && player->get_flags() & FL_ONGROUND && player->get_anim_layers()[ 6 ].m_flWeight == 0.f && speed > 0.1f && speed < 100.f )
			{
				player->get_velocity() = player->get_abs_velocity() = { 0.f, 0.f, 0.f };
			}
			else
			{
				auto v17 = ( player->get_origin() - player->get_oldorigin() ) * ( 1.0 / simtime_delta );
				if ( v17 != player->get_velocity() )
					player->invalidate_physics_recursive( 4 );
				player->get_velocity() = player->get_abs_velocity() = v17;
			}
		}

		player->get_eflags() &= ~0x1000;
	}

	if ( info.animstate->m_iLastClientSideAnimationUpdateFramecount == g_pGlobals->framecount )
		info.animstate->m_iLastClientSideAnimationUpdateFramecount -= 1;

	g_setup_vel_fix = true;
	if ( player != g_pLocalPlayer )
		player->get_clientside_animation() = true;
	info.animstate->update( player->get_eye_angles() );
	if ( player != g_pLocalPlayer )
		player->get_clientside_animation() = false; // stop the game from updating animtions further
	g_setup_vel_fix = false;
	info.animvel = info.animstate->m_velocity;

	player->get_origin() = backup_origin;
	player->get_velocity() = backup_velocity;
	player->get_abs_velocity() = backup_abs_velocity;
	player->get_flags() = backup_flags;
	player->get_eflags() = backup_eflags;
	player->get_duck_amt() = backup_duck_amt;
	player->get_lby() = backup_lby;
	player->set_anim_layers( backup_layers );
	player->set_pose_params( backup_poses );

	g_pGlobals->curtime = curtime;
	g_pGlobals->frametime = frametime;
}
