#include "../include_cheat.h"

void aimbot::no_recoil()
{
	if ( !vars::aim.enabled.get<bool>() )
		return;

	_( weapon_recoil_scale_s, "weapon_recoil_scale" );
	static auto weapon_recoil_scale = g_pCVar->FindVar( weapon_recoil_scale_s );

	g_cmd->viewangles -= g_pLocalPlayer->get_aim_punch() * weapon_recoil_scale->get_float();
}

void aimbot::no_visual_recoil( CViewSetup& v_view )
{
	if ( !g_pLocalPlayer || !vars::aim.enabled.get<bool>() || g_pInput->m_fCameraInThirdPerson || !g_pLocalPlayer->get_alive() )
		return;

	v_view.angles -= g_pLocalPlayer->get_aim_punch() * 0.9f + g_pLocalPlayer->get_view_punch();
}

void aimbot::run()
{
	targets.clear();

	if ( !vars::aim.enabled.get<bool>() || ( *g_ppGameRules )->m_bFreezePeriod() )
		return;

	if ( !g_pLocalPlayer->get_alive() )
		return;

	if ( !can_shoot() )
		return;

	select_targets();

	scan_targets();
}

void aimbot::select_targets()
{
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || player->IsDormant() || !player->is_enemy() || player == g_pLocalPlayer || player->get_immunity() )
			continue;

		targets.emplace_back( player, i );
	}
}

void aimbot::scan_targets()
{
	auto best_damage = 0.f;
	lag_record_t* best_record = nullptr;
	aimpoint_t best_point( Vector( 0.f, 0.f, 0.f ), Vector( 0.f, 0.f, 0.f ), -1 );
	C_CSPlayer* best_player = nullptr;

	player_log::get().filter_records( true );

	for ( size_t i = 0; i < enabled_hitboxes.size(); i++ )
	{
		switch ( i )
		{
			case HITBOX_HEAD:
			case HITBOX_NECK:
			{
				if ( GetAsyncKeyState( vars::key.baim.get<int>() ) && vars::key.baim.get<bool>() )
					enabled_hitboxes[ i ] = false;
				else
					enabled_hitboxes[ i ] = true;
				break;
			}
			case HITBOX_UPPER_CHEST:
			case HITBOX_RIGHT_HAND:
			case HITBOX_LEFT_HAND:
			case HITBOX_RIGHT_UPPER_ARM:
			case HITBOX_LEFT_UPPER_ARM:
			case HITBOX_LEFT_CALF:
			case HITBOX_RIGHT_CALF:
				enabled_hitboxes[ i ] = false;
				break;
			default:
				enabled_hitboxes[ i ] = true;
				break;
		}
	}

	if ( vars::aim.hsonly.get<bool>() )
	{
		for ( size_t i = 0; i < enabled_hitboxes.size(); i++ )
		{
			switch ( i )
			{
				case HITBOX_HEAD:
					enabled_hitboxes[ i ] = true;
					break;
				default:
					enabled_hitboxes[ i ] = false;
					break;
			}
		}
	}

	for ( auto& target : targets )
	{
		auto& log = player_log::get().get_log( target.player->EntIndex() );
		if ( !log.player )
			continue;


		unsigned int j = 0;
		auto wasvalid = false;
		for ( auto& record : log.record )
		{
			j++;
			if ( !record.valid )
				continue;

			if ( wasvalid && ( record.m_velocity.Length2D() <= 0.1f || j != log.lastvalid ) && !record.m_lby_flick && !record.m_shot )
				continue;

			if ( record.m_lby_flick && log.m_nShotsLby )
				continue;

			if ( !record.m_lby_flick && !record.m_shot )
				wasvalid = true;

			auto damage = 0.f;
			const auto point = get_best_damage( record, damage );

			if ( damage > best_damage )
			{
				best_point = point;
				best_damage = damage;
				best_record = &record;
				best_player = target.player;
			}
		}
	}

	if ( !best_record )
		return;

	const auto inair = !( g_pLocalPlayer->get_flags() & FL_ONGROUND );
	const auto aim_angles = math::get().calc_angle( prediction::get().get_unpred_eyepos(), best_point.point );
	const auto hc_angles = math::get().calc_angle( prediction::get().get_unpred_eyepos(), best_point.center );
	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;

	lag_record_t backup( best_player );
	best_record->apply( best_player );
	auto hitchance = calc_hc( hc_angles, best_point, best_player );
	backup.apply( best_player, true );

	if ( get_config( weapon )->index == 1 && get_config( weapon )->scout_jumpshot->get<bool>() && inair )
	{
		if ( weapon->get_inaccuracy() < 0.009f )
			hitchance = 100;
	}

	if ( weapon->get_wpn_data()->flInaccuracyStand * 1.1f >= weapon->get_inaccuracy() )
		hitchance = 100;

	const auto do_autostop = [ & ]() -> int
	{
		if ( !get_config( weapon )->autostop->get<int>() )
			return 0;

		if ( get_config( weapon )->autostop->get<int>() == 1 && ( !get_config( weapon )->autostop_key->get<bool>() || GetAsyncKeyState( vars::key.autostop.get<int>() ) ) )
			return 1;

		if ( get_config( weapon )->autostop->get<int>() == 2 && ( !get_config( weapon )->autostop_key->get<bool>() || GetAsyncKeyState( vars::key.autostop.get<int>() ) ) )
			return 2;

		return 0;
	};

	if ( do_autostop() == 2 && !antiaim::get().get_fake_walk_state() )
		quick_stop();

	if ( is_zeus )
	{
		if ( hitchance < vars::aim.zeus_hc.get<float>() )
		{
			return;
		}
	}

	if ( hitchance < get_config( weapon )->hitchance->get<float>() && !is_knife && vars::aim.autofire.get<bool>() && !is_zeus )
	{
		if ( ( get_config( weapon )->index == 0 || get_config( weapon )->index == 1 || get_config( weapon )->index == 2 ) && get_config( weapon )->autoscope->get<int>() )
		{
			if ( !g_pLocalPlayer->get_scoped() )
			{
				if ( g_cmd->buttons & IN_ATTACK && do_autostop() )
					g_cmd->buttons &= ~IN_ATTACK;

				g_cmd->buttons |= IN_ATTACK2;
			}
		}

		if ( !do_autostop() || antiaim::get().get_fake_walk_state() || do_autostop() == 2 )
			return;

		if ( ( best_damage >= best_player->get_health() || !get_config( weapon )->autostop_lethal->get<bool>() ) && !inair )
			quick_stop();

		return;
	}

	if ( vars::aa.prevent_onshot.get<bool>() && !g_pClientState->m_nChokedCommands && weapon->get_weapon_id() != WEAPON_REVOLVER )
		return;

	if ( is_knife && prediction::get().get_unpred_eyepos().Dist( best_point.point ) <= 32.f + 16.0f )// autoknife
		g_cmd->buttons |= IN_ATTACK2;
	else if ( is_knife )
		g_cmd->buttons |= IN_ATTACK;

	if ( ( !is_knife && vars::aim.autofire.get<bool>() && !( g_cmd->buttons & IN_ATTACK2 ) || is_zeus ) )// autoshoot
		g_cmd->buttons |= IN_ATTACK;

	if ( g_cmd->buttons & IN_ATTACK2 && !is_knife ) // scope fix
		g_cmd->buttons &= ~IN_ATTACK;

	if ( !( g_cmd->buttons & IN_ATTACK ) && !is_knife )
		return;

	if ( weapon->get_weapon_id() == WEAPON_REVOLVER )
	{
		const auto fl_postpone_fire_ready = weapon->get_postpone_fire_ready_time();
		if ( !fl_postpone_fire_ready || fl_postpone_fire_ready > prediction::get().get_curtime() )
			return;
	}

	g_cmd->tick_count = lagcomp::get().fix_tickcount( best_record->m_sim_time );
	g_cmd->viewangles = aim_angles;

	g_tick_shot = g_pGlobals->tickcount;

	if ( !inair && !antiaim::get().get_fake_walk_state() && get_config( weapon )->autostop->get<bool>() )
		quick_stop();

	if ( !is_knife )
	{
		resolver::get().add_shot( prediction::get().get_unpred_eyepos(), best_record, best_player->EntIndex() );
		if ( vars::misc.impacts.get<bool>() )
			g_pDebugOverlay->AddBoxOverlay( best_point.point, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ), QAngle( 0, 0, 0 ), 255, 0, 0, 127, 4 );
	}

	if ( !antiaim::get().get_fake_walk_state() )
		*g_send_packet = true;

	if ( !vars::aa.choke_shot.get<bool>() )
		*g_send_packet = false;

	aimbot_called = prediction::get().get_unpred_curtime();
}

bool aimbot::can_shoot( const float time, bool check )
{
	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( !weapon )
		return false;

	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;

	if ( !check )
	{
		if ( !vars::aim.knife_bot.get<bool>() && is_knife )
			return false;

		if ( !vars::aim.zeus_bot.get<bool>() && is_zeus )
			return false;
	}

	const auto weapontype = weapon->get_weapon_type();
	if ( weapontype == WEAPONTYPE_C4 || weapon->is_grenade() )
		return false;

	if ( weapon->get_clip1() < 1 && !is_knife )
		return false;

	if ( weapon->in_reload() )
		return false;

	if ( weapon->get_next_primary_attack() > time )
		return false;

	if ( g_pLocalPlayer->get_next_attack() > time )
		return false;

	if ( !check )
	{
		if ( weapon->get_weapon_id() == WEAPON_REVOLVER && get_config( weapon )->autorevolver->get<bool>() )
		{
			const auto fl_postpone_fire_ready = weapon->get_postpone_fire_ready_time();
			if ( !fl_postpone_fire_ready || fl_postpone_fire_ready > prediction::get().get_curtime() + ticks_to_time( antiaim::get().get_rest_choke() ) )
				g_cmd->buttons |= IN_ATTACK;
		}
	}
	else
	{
		if ( weapon->get_weapon_id() == WEAPON_REVOLVER )
		{
			const auto fl_postpone_fire_ready = weapon->get_postpone_fire_ready_time();
			if ( fl_postpone_fire_ready > prediction::get().get_curtime() )
				return false;
		}
	}

	return true;
}

void aimbot::quick_stop()
{
	quick_stop_called = prediction::get().get_unpred_curtime();
	g_cmd->viewangles.Clamp();

	auto vel2 = prediction::get().get_unpred_vel();
	const auto speed = vel2.Length();
	if ( speed > 15.f )
	{
		QAngle dir;
		math::get().vector_angles( vel2, dir );
		dir.y = g_cmd->viewangles.y - dir.y;

		Vector new_move;
		math::get().angle_vectors( dir, &new_move );
		const auto max = std::max( std::fabs( g_cmd->forwardmove ), std::fabs( g_cmd->sidemove ) );
		const auto mult = 450.f / max;
		new_move *= -mult;

		g_cmd->forwardmove = new_move.x;
		g_cmd->sidemove = new_move.y;
	}
	else
	{
		g_cmd->forwardmove = 0.f;
		g_cmd->sidemove = 0.f;
	}
}

bool aimbot::get_quick_stop_state() const
{
	return quick_stop_called == prediction::get().get_unpred_curtime();
}

aimpoint_t aimbot::get_best_damage( lag_record_t& log, float& damage, Vector* pos )
{
	const auto player = get_entity( log.index );
	const auto& player_log = player_log::get().get_log( log.index );
	const auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );

	lag_record_t backup( player );
	log.apply( player );

	std::vector<aimpoint_t> points;
	const auto fakelag_hitscan = [ & ]()
	{
		const auto model = player->get_model();
		if ( !model )
			return;

		const auto studio_hdr = g_pModelInfo->GetStudioModel( model );
		if ( !studio_hdr )
			return;

		for ( auto i = 0; i < HITBOX_MAX; i++ )
		{
			if ( !( i == HITBOX_HEAD || i == HITBOX_PELVIS || i == HITBOX_CHEST ) )
				continue;

			auto hitbox = studio_hdr->pHitbox( i, 0 );
			if ( !hitbox )
				continue;

			auto& mat = log.matrix[ hitbox->bone ];
			const auto mod = hitbox->radius != -1.f ? hitbox->radius : 0.f;

			Vector max;
			Vector min;
			math::get().vector_transform( hitbox->bbmax + mod, mat, max );
			math::get().vector_transform( hitbox->bbmin - mod, mat, min );

			const auto center = ( min + max ) * 0.5f;

			const auto cur_angles = math::get().calc_angle( center, prediction::get().get_unpred_eyepos() );

			Vector forward;
			math::get().angle_vectors( cur_angles, &forward );

			const auto right = forward.Cross( Vector( 0, 0, 1 ) );
			const auto left = Vector( -right.x, -right.y, right.z );
			const auto top = Vector( 0, 0, 1 );

			if ( i == HITBOX_HEAD )
				points.emplace_back( center + top * hitbox->radius, center, i );
			points.emplace_back( center + right * hitbox->radius, center, i );
			points.emplace_back( center + left * hitbox->radius, center, i );
		}
	};

	if ( pos )
		fakelag_hitscan();
	else
		multipoint_hitboxes( log, points );

	auto was_repeated = false;

	auto best_damage = 0.f;
	aimpoint_t best_point( Vector( 0.f, 0.f, 0.f ), Vector( 0.f, 0.f, 0.f ), -1 );

	const auto is_zeus = get_weapon( g_pLocalPlayer->get_active_weapon() )->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && get_weapon( g_pLocalPlayer->get_active_weapon() )->get_weapon_type() == WEAPONTYPE_KNIFE;
repeat:
	auto prevhitbox = -1;


	for ( auto& point : points )
	{
		float point_damage;
		const auto dist = pos ? pos->Dist( point.point ) : prediction::get().get_unpred_eyepos().Dist( point.point );

		//g_pDebugOverlay->AddBoxOverlay( point.point, Vector( 1.f, 1.f, 1.f ), Vector( -1.f, -1.f, -1.f ), Vector( 0.f, 0.f, 0.f ), random_float( 0.f, 255.f ), random_float( 0.f, 255.f ), random_float( 0.f, 255.f ), 255, g_pGlobals->interval_per_tick );

		if ( is_knife && dist > ( 48.f + 16.0f ) )
			continue;

		if ( !is_knife && !penetration::get().can_hit( player, point.point, point_damage, pos ? *pos : prediction::get().get_unpred_eyepos() ) )
			continue;

		if ( is_zeus && point_damage < 100.f )
			continue;

		if ( is_knife )
			point_damage = 100.f / dist;

		if ( ( point_damage > best_damage && prevhitbox != point.hitbox || point_damage > best_damage + 30.f ) )
		{
			best_point = point;
			best_damage = point_damage;
			prevhitbox = point.hitbox;
		}
	}

	const auto baim_when = !( log.m_flags & FL_ONGROUND ) && get_config( weapon )->baim.air->get<bool>()
		|| player_log.m_iMode == RMODE_WALL && get_config( weapon )->baim.resolver_unsure->get<bool>()
		|| best_damage && get_config( weapon )->baim_mode->get<int>() == 1 && player_log.m_nShots >= 1
		|| get_config( weapon )->baim.lethal->get<bool>() && best_damage > player->get_health();

	if ( !was_repeated && !baim_when )
	{
		points.clear();
		multipoint_hitboxes( log, points, true );
		was_repeated = true;
		goto repeat;
	}

	backup.apply( player, true );

	damage = best_damage;
	return best_point;
};

void aimbot::multipoint_hitboxes( lag_record_t& log, std::vector<aimpoint_t>& points, const bool second_go )
{
	const auto player = get_entity( log.index );
	auto& player_log = player_log::get().get_log( log.index );

	const auto model = player->get_model();
	if ( !model )
		return;

	const auto studio_hdr = g_pModelInfo->GetStudioModel( model );
	if ( !studio_hdr )
		return;

	const auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	const auto baim = GetAsyncKeyState( vars::key.baim.get<int>() ) && vars::aim.baim_on_key.get<bool>();
	const auto is_zeus = weapon->get_weapon_id() == WEAPON_TASER;
	const auto is_knife = !is_zeus && weapon->get_weapon_type() == WEAPONTYPE_KNIFE;
	const auto moving = log.m_velocity.Length() > 0.1f;

	for ( auto i = 0; i < HITBOX_MAX; i++ )
	{
		if ( ( i == HITBOX_RIGHT_FOOT || i == HITBOX_LEFT_FOOT ) && ( moving || player_log.m_nShots > 3 || ( !( log.m_flags & FL_ONGROUND ) && get_config( weapon )->baim.air->get<bool>() ) ) )
			continue;

		if ( second_go && i > HITBOX_NECK )
			continue;

		if ( !second_go && i <= HITBOX_NECK )
			continue;

		if ( is_zeus && ( i != HITBOX_BODY && i != HITBOX_PELVIS ) )
			continue;

		if ( is_knife && i != HITBOX_UPPER_CHEST )
			continue;

		if ( !is_knife && !is_zeus && !enabled_hitboxes[ i ] )
			continue;

		auto hitbox = studio_hdr->pHitbox( i, 0 );
		if ( !hitbox )
			continue;

		auto& mat = log.matrix[ hitbox->bone ];
		const auto mod = hitbox->radius != -1.f ? hitbox->radius : 0.f;

		Vector max;
		Vector min;
		math::get().vector_transform( hitbox->bbmax + mod, mat, max );
		math::get().vector_transform( hitbox->bbmin - mod, mat, min );

		const auto center = ( min + max ) * 0.5f;

		points.emplace_back( center, center, i );

		if ( !hitbox->radius || ( i != HITBOX_HEAD && i != HITBOX_CHEST && i != HITBOX_PELVIS ) || is_zeus && !is_knife )
			continue;

		const auto cur_angles = math::get().calc_angle( center, prediction::get().get_unpred_eyepos() );

		Vector forward;
		math::get().angle_vectors( cur_angles, &forward );

		auto rs = hitbox->radius * ( get_config( weapon )->scale->get<float>() / 100.f );
		if ( i == HITBOX_HEAD )
		{
			rs = hitbox->radius * ( get_config( weapon )->scale_head->get<float>() / 100.f );
		}

		player_log.m_nShots = std::clamp( player_log.m_nShots, 0, player_log.m_nShots );

		if ( get_config( weapon )->adaptive_scale->get<bool>() )
		{
			rs *= 1 - ( player_log.m_nShots * 0.1 );
		}

		if ( baim || !( log.m_flags & FL_ONGROUND ) /*|| log.m_velocity.Length2D() > 210.f*/ || log.m_lby_flick )
			rs *= 0.5f;

		if ( rs < 0.2 )
			continue;

		if ( is_knife )
		{
			const auto back = forward * hitbox->radius;
			points.emplace_back( center + back, center, i );
			continue;
		}

		const auto right = forward.Cross( Vector( 0, 0, 1 ) ) * rs;
		const auto left = Vector( -right.x, -right.y, right.z );
		const auto top = Vector( 0, 0, 1 ) * rs;

		if ( i == HITBOX_HEAD )
			points.emplace_back( center + top, center + top * 0.95f, i );
		points.emplace_back( center + right, center + top * 0.95f, i );
		points.emplace_back( center + left, center + top * 0.95f, i );
	}
}


float aimbot::calc_hc( QAngle vangles, const aimpoint_t& point, C_CSPlayer* player ) const
{
	auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( !weapon )
		return 0.f;

	const auto hitchance_cfg = get_config( weapon )->hitchance->get<float>();

	const auto backupvel = g_pLocalPlayer->get_velocity();
	const auto backupabsvel = g_pLocalPlayer->get_abs_velocity();
	g_pLocalPlayer->get_abs_velocity() = g_pLocalPlayer->get_velocity() = prediction::get().get_unpred_vel();
	weapon->update_accuracy();
	g_pLocalPlayer->get_velocity() = backupvel;
	g_pLocalPlayer->get_abs_velocity() = backupabsvel;

	Vector forward, right, up;
	const auto eyepos = prediction::get().get_unpred_eyepos();
	const auto spread = weapon->get_spread();
	const auto inaccuracy = weapon->get_inaccuracy();
	const auto angles = vangles.Clamp();
	math::get().angle_vectors( angles, &forward, &right, &up );
	math::get().fast_vec_normalize( forward );
	math::get().fast_vec_normalize( right );
	math::get().fast_vec_normalize( up );
	auto endpoint = point.center;

	const auto needed = static_cast< int >( 256 * ( hitchance_cfg / 100.f ) );
	const auto allowed_misses = 256 - needed;
	auto hits = 0;
	auto i = 0;

	while ( i < 256 )
	{
		random_seed( ( i & 255 ) + 1 );
		const auto b = random_float( 0.f, 2.0f * M_PI );
		const auto c = random_float( 0.0f, 1.0f );
		const auto d = random_float( 0.f, 2.0f * M_PI );

		const auto spread_val = c * spread;
		const auto inaccuracy_val = c * inaccuracy;

		const Vector v_spread( ( cos( b ) * spread_val ) + ( cos( d ) * inaccuracy_val ), ( sin( b ) * spread_val ) + ( sin( d ) * inaccuracy_val ), 0 );
		Vector dir;

		dir.x = forward.x + ( right.x * v_spread.x ) + ( up.x * v_spread.y );
		dir.y = forward.y + ( right.y * v_spread.x ) + ( up.y * v_spread.y );
		dir.z = forward.z + ( right.z * v_spread.x ) + ( up.z * v_spread.y );

		math::get().fast_vec_normalize( dir );

		QAngle spread_view;
		Vector end;
		math::get().vector_angles( dir, spread_view );
		spread_view.NormalizeNoClamp();
		math::get().angle_vectors( angles - ( spread_view - angles ), &end );
		math::get().fast_vec_normalize( end );

		trace_t tr;
		Ray_t ray;
		const auto trace_end = eyepos + ( end * endpoint.Dist( eyepos ) );
		ray.Init( eyepos, trace_end );
		//g_pDebugOverlay->AddLineOverlay( eyepos, trace_end, 180, 240, 0, true, 0.1f );
		g_pTrace->ClipRayToEntity( ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr );

		if ( tr.m_pEnt == player )
			hits++;

		const auto hitchance = ( static_cast< float >( hits ) / 255.f ) * 100.f;
		if ( hitchance >= hitchance_cfg )
			return hitchance;

		if ( i - hits > allowed_misses )
			return hitchance;

		i++;
	}

	return 0.0f;
}
