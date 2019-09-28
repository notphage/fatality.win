#include "../include_cheat.h"

void antiaim::run()
{
	if ( !g_pLocalPlayer->get_alive() || ( *g_ppGameRules )->m_bFreezePeriod() )
	{
		freestanding::get().last_fs_time = 0.f;
		return;
	}

	if ( !vars::aa.enabled.get<bool>() && !vars::aa.fakelag_when_disabled.get<bool>() )
		return;

	if ( !vars::aa.enabled.get<bool>() )
		fake_lag();

	if ( !vars::aa.enabled.get<bool>() )
		return;

	choose_real();

	choose_fk();

	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( !weapon )
		return;

	const auto revert_aa = []()
	{
		QAngle viewangles;
		g_pEngine->GetViewAngles( viewangles );
		g_cmd->viewangles.x = viewangles.x;
		g_cmd->viewangles.y = viewangles.y;
	};

	const auto movetype = g_pLocalPlayer->get_move_type();

	if ( g_cmd->buttons & IN_ATTACK && !weapon->is_grenade() && aimbot::can_shoot() && weapon->get_weapon_id() != WEAPON_REVOLVER || movetype == MOVETYPE_LADDER || movetype == MOVETYPE_NOCLIP || GetAsyncKeyState( 0x45 ) )
		revert_aa();


	if ( weapon->is_being_thrown() )
	{
		revert_aa();
		*g_send_packet = true;
	}
}

void antiaim::fake_lag()
{
	static auto unchoke = false;
	static auto onpeek_called = 0.f;

	to_choke = get_antiaim( type )->choke->get<int>();

	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( weapon )
	{
		if ( vars::aa.choke_reload.get<bool>() && weapon->in_reload() )
			to_choke = 13;

		else if ( vars::aa.choke_weapon_switch.get<bool>() && g_pLocalPlayer->get_next_attack() > prediction::get_curtime() )
			to_choke = 13;
	}

	if ( onpeek_called >= g_pGlobals->curtime && onpeek_called < g_pGlobals->curtime + ticks_to_time( 15 ) )
		to_choke = 14;

	if ( get_fake_walk_state() )
		to_choke = 13;

	to_choke = std::clamp( to_choke, 1u, 15u );
	if ( ( *g_ppGameRules )->m_bIsValveDS() )
		to_choke = std::clamp( to_choke, 1u, 10u );

	*g_send_packet = g_pClientState->m_nChokedCommands >= to_choke;

	if ( get_fake_walk_state() )
		return;

	if ( unchoke )
	{
		*g_send_packet = true;
		unchoke = false;
		return;
	}

	if ( *g_send_packet )
		onpeek_called = 0.f;

	if ( on_peek_fakelag() )
	{
		if ( *g_send_packet )
			return;

		if ( onpeek_called < g_pGlobals->curtime )
			onpeek_called = g_pGlobals->curtime + ticks_to_time( 14 );
		else if ( g_pClientState->m_nChokedCommands > 1 && onpeek_called < g_pGlobals->curtime )
			unchoke = true;
	}
}

void antiaim::choose_fk()
{
	fake_walk();

	fake_lag();

	if ( *g_send_packet )
		g_cmd->viewangles.y += RandomFlt( 130.f, 260.f );
}

bool antiaim::lby_update()
{
	if ( g_pClientState->m_nChokedCommands || !( g_pLocalPlayer->get_flags() & FL_ONGROUND ) )
		return false;

	const auto updated = update_lby;

	if ( update_lby )
	{
		if ( get_antiaim( type )->lby_random->get<bool>() )
		{
			const auto lby_random_range = get_antiaim( type )->lby_random_range->get<int>() * 0.5f;
			const auto lby_random_add = get_antiaim( type )->lby_random_add->get<int>();
			const auto lby = random_float( -lby_random_range, lby_random_range );
			initial_lby = g_cmd->viewangles.y + lby + lby_random_add;
		}

		auto angles = g_cmd->viewangles.y;

		target_lby = initial_lby;
		g_cmd->viewangles.y = initial_lby;
		g_cmd->viewangles.Clamp();
		update_lby = false;

		if ( secondupdate || !get_antiaim( type )->lby_static->get<bool>() )
		{
			if ( get_antiaim( type )->lby_static->get<bool>() )
			{
				initial_lby += -get_antiaim( type )->lby_delta_first->get<float>() + get_antiaim( type )->lby_delta->get<float>();
			}
			else
			{
				initial_lby = angles + get_antiaim( type )->lby_delta->get<float>();
			}
			secondupdate = false;
		}
	}

	return updated;
}

void antiaim::lby_prediction()
{
	const auto animstate = *reinterpret_cast< CCSGOPlayerAnimState** >( uintptr_t( static_cast< C_CSPlayer* >( g_pLocalPlayer ) ) + 0x3874 );
	if ( !animstate )
		return;

	if ( g_pClientState->m_nChokedCommands )
		return;

	if ( animstate->m_velocity > 0.1f )
	{
		next_lby_update = prediction::get().get_curtime() + 0.22f;
		firstupdate = true;
	}
	else if ( prediction::get().get_curtime() > next_lby_update )
	{
		update_lby = true;
		next_lby_update = prediction::get().get_curtime() + 1.1f;
	}

	const auto get_add_by_choke = [ & ]() -> float
	{
		static auto max = 137.f;
		static auto min = 100.f;

		auto mult = 1.f / 0.2f * ticks_to_time( to_choke );

		return 100.f + ( max - min ) * mult;
	};

	if ( firstupdate && animstate->m_velocity <= 0.1f )
	{
		initial_lby = g_cmd->viewangles.y + get_antiaim( type )->lby_delta_first->get<float>();
		secondupdate = true;
		firstupdate = false;
	}

	if ( !firstupdate && prediction::get().get_curtime() + ticks_to_time( to_choke + 1 ) > next_lby_update
		 &&  fabsf( math::get().normalize_float( g_cmd->viewangles.y - initial_lby ) ) < get_add_by_choke() )
	{
		g_cmd->viewangles.y = initial_lby + get_add_by_choke();
	}


}

void antiaim::fix_movement( const QAngle wish_angle )
{
	if ( aimbot::get().get_quick_stop_state() )
		return;

	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = g_cmd->viewangles.Clamp();
	viewangles.Normalize();

	math::get().angle_vectors( wish_angle, &view_fwd, &view_right, &view_up );
	math::get().angle_vectors( viewangles, &cmd_fwd, &cmd_right, &cmd_up );

	const auto v8 = sqrtf( ( view_fwd.x * view_fwd.x ) + ( view_fwd.y * view_fwd.y ) );
	const auto v10 = sqrtf( ( view_right.x * view_right.x ) + ( view_right.y * view_right.y ) );
	const auto v12 = sqrtf( view_up.z * view_up.z );

	const Vector norm_view_fwd( ( 1.f / v8 ) * view_fwd.x, ( 1.f / v8 ) * view_fwd.y, 0.f );
	const Vector norm_view_right( ( 1.f / v10 ) * view_right.x, ( 1.f / v10 ) * view_right.y, 0.f );
	const Vector norm_view_up( 0.f, 0.f, ( 1.f / v12 ) * view_up.z );

	const auto v14 = sqrtf( ( cmd_fwd.x * cmd_fwd.x ) + ( cmd_fwd.y * cmd_fwd.y ) );
	const auto v16 = sqrtf( ( cmd_right.x * cmd_right.x ) + ( cmd_right.y * cmd_right.y ) );
	const auto v18 = sqrtf( cmd_up.z * cmd_up.z );

	const Vector norm_cmd_fwd( ( 1.f / v14 ) * cmd_fwd.x, ( 1.f / v14 ) * cmd_fwd.y, 0.f );
	const Vector norm_cmd_right( ( 1.f / v16 ) * cmd_right.x, ( 1.f / v16 ) * cmd_right.y, 0.f );
	const Vector norm_cmd_up( 0.f, 0.f, ( 1.f / v18 ) * cmd_up.z );

	const auto v22 = norm_view_fwd.x * g_cmd->forwardmove;
	const auto v26 = norm_view_fwd.y * g_cmd->forwardmove;
	const auto v28 = norm_view_fwd.z * g_cmd->forwardmove;
	const auto v24 = norm_view_right.x * g_cmd->sidemove;
	const auto v23 = norm_view_right.y * g_cmd->sidemove;
	const auto v25 = norm_view_right.z * g_cmd->sidemove;
	const auto v30 = norm_view_up.x * g_cmd->upmove;
	const auto v27 = norm_view_up.z * g_cmd->upmove;
	const auto v29 = norm_view_up.y * g_cmd->upmove;

	g_cmd->forwardmove = ( ( ( ( norm_cmd_fwd.x * v24 ) + ( norm_cmd_fwd.y * v23 ) ) + ( norm_cmd_fwd.z * v25 ) )
						   + ( ( ( norm_cmd_fwd.x * v22 ) + ( norm_cmd_fwd.y * v26 ) ) + ( norm_cmd_fwd.z * v28 ) ) )
		+ ( ( ( norm_cmd_fwd.y * v30 ) + ( norm_cmd_fwd.x * v29 ) ) + ( norm_cmd_fwd.z * v27 ) );
	g_cmd->sidemove = ( ( ( ( norm_cmd_right.x * v24 ) + ( norm_cmd_right.y * v23 ) ) + ( norm_cmd_right.z * v25 ) )
						+ ( ( ( norm_cmd_right.x * v22 ) + ( norm_cmd_right.y * v26 ) ) + ( norm_cmd_right.z * v28 ) ) )
		+ ( ( ( norm_cmd_right.x * v29 ) + ( norm_cmd_right.y * v30 ) ) + ( norm_cmd_right.z * v27 ) );
	g_cmd->upmove = ( ( ( ( norm_cmd_up.x * v23 ) + ( norm_cmd_up.y * v24 ) ) + ( norm_cmd_up.z * v25 ) )
					  + ( ( ( norm_cmd_up.x * v26 ) + ( norm_cmd_up.y * v22 ) ) + ( norm_cmd_up.z * v28 ) ) )
		+ ( ( ( norm_cmd_up.x * v30 ) + ( norm_cmd_up.y * v29 ) ) + ( norm_cmd_up.z * v27 ) );
}

void antiaim::fake_walk()
{
	static auto wasfakewalking = false;

	if ( !vars::aa.fakewalk.get<bool>() || !GetAsyncKeyState( vars::key.fakewalk.get<int>() ) )
	{
		wasfakewalking = false;
		return;
	}

	if ( !wasfakewalking && prediction::get().get_unpred_vel().Length2D() < 0.1f )
		wasfakewalking = true;
	if ( !wasfakewalking )
	{
		aimbot::get().quick_stop();
		return;
	}

	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );

	fake_walk_called = prediction::get().get_unpred_curtime();

	const auto choked = g_pClientState->m_nChokedCommands;
	auto newmax = ( ( *g_ppGameRules )->m_bIsValveDS() ? 6 : 8 );
	static auto max = newmax;
	if ( ( weapon->get_weapon_id() == WEAPON_SCAR20 || weapon->get_weapon_id() == WEAPON_G3SG1 || weapon->get_weapon_id() == WEAPON_AWP ) && g_pLocalPlayer->get_scoped() && !( g_pLocalPlayer->get_flags() & FL_DUCKING ) )
		newmax += 1;
	if ( g_pLocalPlayer->get_flags() & FL_DUCKING )
		newmax += 2;
	if ( g_pLocalPlayer->get_next_attack() > prediction::get().get_curtime() )
		newmax -= 1;
	if ( choked == to_choke || !choked || choked > max )
		aimbot::get().quick_stop();

	if ( !choked )
		max = newmax;

	/**g_send_packet = !choked;*/

	//static auto predict_velocity = []( Vector* velocity )
	//{
	//	float smove, fmove, wishspeed;
	//	const auto speed = velocity->Length();
	//	Vector forward, right, up, wishvel;
	//	math::get().angle_vectors( g_cmd->viewangles, &forward, &right, &up );
	//	if ( speed >= 0.1f )
	//	{
	//		QAngle dir;
	//		math::get().vector_angles( *velocity, dir );
	//		dir.y = g_cmd->viewangles.y - dir.y;

	//		Vector new_move;
	//		math::get().angle_vectors( dir, &new_move );
	//		new_move *= -449.f;

	//		fmove = new_move.x;
	//		smove = new_move.y;
	//	}

	//	for ( auto i = 0; i < 2; i++ )       // Determine x and y parts of velocity
	//		wishvel[ i ] = forward[ i ] * fmove + right[ i ] * smove;
	//	wishvel[ 2 ] = 0;

	//	auto wishdir = wishvel;
	//	wishspeed = math::get().fast_vec_normalize( wishdir );

	//	*velocity += wishdir * -wishspeed;
	//	if ( speed >= 0.1f )
	//	{
	//		auto friction = g_pCVar->FindVar( "sv_friction" )->get_float();
	//		auto stop_speed = std::max<float>( speed, g_pCVar->FindVar( "sv_stopspeed" )->get_float() );
	//		auto time = std::max<float>( g_pGlobals->interval_per_tick, g_pGlobals->frametime );
	//		//*velocity *= std::max<float>( 0.f, speed - friction * stop_speed * time / speed );
	//	}
	//};

	//int ticks_to_stop;
	//for ( ticks_to_stop = 0; ticks_to_stop < 16; ++ticks_to_stop )
	//{
	//	if ( velocity.Length2D() < 0.1f )
	//		break;

	//	predict_velocity( &velocity );
	//	//predict_velocity( &velocity );
	//}

	//choked = choked > 12 ? 0 : choked + 1;

	//if ( ticks_to_stop > 12 - choked || choked == 1 /*|| !choked*/ )
	//{
	//	aimbot::get().quick_stop();
	//}



	//*g_send_packet = !choked;
}

bool antiaim::get_fake_walk_state() const
{
	return fake_walk_called == prediction::get().get_unpred_curtime();
}

bool antiaim::on_peek_fakelag()
{
	if ( !vars::aa.on_peek.get<bool>() )
		return false;

	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( weapon && weapon->get_item_definiton_index() == WEAPON_REVOLVER )
		return false;

	if ( g_pLocalPlayer->get_velocity().Length() < 2.f )
		return false;

	const auto time = prediction::get_curtime() + ticks_to_time( 3 );
	if ( !aimbot::get().can_shoot( time ) )
		return false;

	const auto mindmg = get_config( get_weapon( g_pLocalPlayer->get_active_weapon() ) )->mindmg->get<float>();

	auto newpos = g_pLocalPlayer->get_origin() + g_pLocalPlayer->get_velocity() * g_pGlobals->interval_per_tick * 3; //3 == prediction amount
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || player->IsDormant() || !player->is_enemy() || player == g_pLocalPlayer || player->get_immunity() )
			continue;

		auto& log = player_log::get().get_log( i );
		if ( !log.m_bFilled || log.record.empty() )
			continue;

		auto player_damage = 0.f;
		aimbot::get().get_best_damage( log.record[ 0 ], player_damage, &newpos );

		if ( player_damage > mindmg )
			return true;
	}

	return false;
}

void antiaim::choose_real()
{
	const auto air = vars::aa.air.enabled.get<bool>() && !( g_pLocalPlayer->get_flags() & FL_ONGROUND );
	const auto moving = vars::aa.moving.enabled.get<bool>() && g_pLocalPlayer->get_flags() & FL_ONGROUND && prediction::get().get_unpred_vel().Length() > 0.1f;
	const auto fakewalking = vars::aa.fakewalk.get<bool>() && GetAsyncKeyState( vars::key.fakewalk.get<int>() );
	const auto regular = ( !air && !moving ) || ( fakewalking && !air );

	if ( air ) { type = 2; }
	if ( moving ) { type = 1; }
	if ( regular ) { type = 0; }

	if ( get_antiaim( type )->at_target->get<bool>() )
		do_at_target();

	if ( get_antiaim( type )->spin->get<bool>() )
		do_spin();

	if ( get_antiaim( type )->jitter->get<bool>() )
		do_jitter();

	do_add();

	if ( !get_antiaim( type )->edge->get<bool>() || !do_edge() )
		do_freestand();

	if ( get_antiaim( type )->lby->get<bool>() )
		do_lby();

	g_cmd->viewangles.x = 89.f;
}

void antiaim::do_spin() const
{
	const auto speed = get_antiaim( type )->spin_speed->get<int>();

	static auto add = 0.f;

	if ( add > get_antiaim( type )->spin_range->get<int>() )
	{
		add = 0.f;
	}

	if ( type == 2 && g_pLocalPlayer->get_flags() & FL_ONGROUND )
	{
		add = 0.f;
	}

	add += speed;
	g_cmd->viewangles.y += add;
}

void antiaim::do_add()
{
	const auto add = get_antiaim( type )->add->get<int>();

	g_cmd->viewangles.y += add;
}

void antiaim::do_jitter()
{
	auto jitter_range = get_antiaim( type )->jitter_range->get<int>() * 0.5f;
	const auto jitter_speed = get_antiaim( type )->jitter_speed->get<int>();

	static auto last_set_tick = 0;
	static auto flip = false;

	static auto add = 0.f;

	if ( last_set_tick + jitter_speed < g_pLocalPlayer->get_tickbase() || last_set_tick > g_pLocalPlayer->get_tickbase() )
	{
		last_set_tick = g_pLocalPlayer->get_tickbase();

		if ( get_antiaim( type )->jitter_random->get<int>() )
		{
			jitter_range = random_float( -jitter_range, jitter_range );
			flip = true;
		}

		add = flip ? jitter_range : -jitter_range;

		flip = !flip;
	}

	g_cmd->viewangles.y += add;
}

void antiaim::do_freestand()
{
	static auto yaw = 0.f;
	if ( freestanding::get().get_real( yaw ) )
		g_cmd->viewangles.y = yaw;
}

bool antiaim::do_edge()
{
	return false;
}

void antiaim::do_at_target() const
{
	C_CSPlayer* target = nullptr;
	QAngle target_angle;

	QAngle original_viewangles;
	g_pEngine->GetViewAngles( original_viewangles );

	auto lowest_fov = 90.f;
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || !player->is_enemy() || player == g_pLocalPlayer )
			continue;

		if ( player->IsDormant() && ( player->get_simtime() > g_pGlobals->curtime || player->get_simtime() + 5.f < g_pGlobals->curtime ) )
			continue;

		auto enemy_pos = player->get_origin();
		enemy_pos.z += 64.f;

		const auto angle = math::get().calc_angle( g_pLocalPlayer->get_eye_pos(), enemy_pos );
		const auto fov = math::get().get_fov( original_viewangles, angle );

		if ( fov < lowest_fov )
		{
			target = player;
			lowest_fov = fov;
			target_angle = angle;
		}
	}

	if ( !target )
		return;

	g_cmd->viewangles.y = target_angle.y;
}

bool antiaim::do_lby()
{
	lby_prediction();

	return lby_update();
}