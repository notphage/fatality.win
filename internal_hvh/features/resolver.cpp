#include "../include_cheat.h"

std::vector<float>brutelist
{
	118.f,
	-120.f,
	180.f,
	-30.f,
	98,
	-2,
	+150.f,
	-119.f
};

void resolver::resolve( C_CSPlayer* player, lag_record_t* record )
{
	record->m_shot = false;

	__( weapon_accuracy_nospread, "weapon_accuracy_nospread" );

	static auto nospread = g_pCVar->FindVar( weapon_accuracy_nospread );

	if ( !vars::aim.resolve_team.get<bool>() && !player->is_enemy() )
		return;

	auto& log = player_log::get().get_log( player->EntIndex() );

	log.m_bOverride = false;
	log.m_bShot = false;

	if ( log.record.empty() || record->m_velocity.Length() > 75.f && record->m_flags & FL_ONGROUND )
	{
		if ( log.record.size() > 1 )
		{
			if ( log.record[ 1 ].m_flags & FL_ONGROUND )
			{
				log.m_nShots = 0;
				log.m_nShotsLby = 0;
			}
		}
		else
		{
			log.m_nShots = 0;
			log.m_nShotsLby = 0;
		}

	}

	if ( !vars::aim.resolver.get<bool>() || log.record.empty() )
		return;

	if ( player->IsDormant() )
		return;

	const auto simtime = record->m_sim_time;
	const auto choked_ticks = record->m_lag - 1;
	if ( choked_ticks < 0 )
		return;

	if ( log.m_flLastShotSimulationTime == record->m_sim_time && choked_ticks < 2 )
	{
		record->m_eye_angles.y = log.m_vecLastShotEyeAngles.y;
		record->m_shot = true;
		return;
	}

	record->m_eye_angles.y = math::normalize_float( log.m_flLowerBodyYawTarget );

	if ( nospread->get_bool() )
	{
		nospread_resolve( player, record );
		return;
	}

	const auto velocity = record->m_velocity.Length2D();

	if ( velocity > 0.1f && !log.m_nShots && ( log.record.size() < 2 || !log.record[ 1 ].m_dormant ) )
	{
		if ( is_spin( &log ) )
		{
			log.m_iMode = RMODE_SPIN;
		}
		else
		{
			log.m_iMode = RMODE_MOVING;
		}
		return;
	}

	if ( log.m_iMode == RMODE_SPIN )
	{
		record->m_eye_angles.y = ( log.spinbody + log.spindelta * record->m_lag ) * log.step++;
	}

	if ( log.m_vecLastNonDormantOrig != record->m_origin && g_pLocalPlayer->get_alive() )
	{
		log.m_iMode = RMODE_WALL;
	}

	if ( simtime - log.m_flLastLowerBodyYawTargetUpdateTime > 1.35f && log.m_vecLastNonDormantOrig == record->m_origin && log.m_iMode == RMODE_MOVING )
	{
		if ( simtime - log.m_flLastLowerBodyYawTargetUpdateTime > 1.65f )
		{
			log.m_iMode = RMODE_WALL;
		}
		else
		{
			log.m_iMode = RMODE_DELTA;
			log.m_flSavedLbyDelta = log.m_flLowerBodyYawTarget - log.m_flOldLowerBodyYawTarget;
		}
	}

	if ( log.m_iMode == RMODE_WALL )
	{
		if ( !wall_detect( &log, record, record->m_eye_angles.y ) )
			log.m_iMode = RMODE_MOVING;
	}

	if ( log.m_iMode == RMODE_MOVING )
	{
		record->m_eye_angles.y = math::normalize_float( log.m_flLastMovingLowerBodyYawTarget );
	}

	if ( log.m_iMode == RMODE_DELTA )
	{
		record->m_eye_angles.y = math::normalize_float( log.m_flLowerBodyYawTarget - log.m_flSavedLbyDelta );
	}

	if ( log.m_nShots > 0 )
	{
		auto index = log.m_nShots + log.last_hit_bute;
		while ( index > 6 )
			index -= 6;

		if ( log.m_nShots != log.oldshots )
		{
			log.last_hit_bute = index;
			log.oldshots = log.m_nShots;
		}

		if ( log.m_nShots == 1 )
		{
			log.m_flBruteStartLby = log.m_flLowerBodyYawTarget;

			if ( log.m_iMode != RMODE_WALL && wall_detect( &log, record, record->m_eye_angles.y ) )
				return;
		}

		const auto add = math::normalize_float( brutelist[ index ] );
		record->m_eye_angles.y = log.m_flBruteStartLby + add;
	}
}

bool resolver::is_spin( player_log_t * log )
{
	log->step = 0;

	if ( log->record.size() < 3 )
		return false;

	log->spindelta = ( log->record[ 0 ].m_body - log->record[ 1 ].m_body ) / log->record[ 1 ].m_lag;
	log->spinbody = log->record[ 0 ].m_body;
	const auto delta2 = ( log->record[ 1 ].m_body - log->record[ 2 ].m_body ) / log->record[ 2 ].m_lag;

	return false;

	return log->spindelta == delta2 && log->spindelta > 0.5f;
}

void resolver::collect_wall_detect( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		return;

	if ( !g_pLocalPlayer )
		return;

	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto& log = player_log::get().get_log( i );
		auto player = get_entity( i );

		if ( !player || player == g_pLocalPlayer )
		{
			continue;
		}

		if ( !player->is_enemy() && !vars::aim.resolve_team.get<bool>() )
		{
			continue;
		}

		if ( !player->get_alive() )
		{
			continue;
		}

		if ( player->IsDormant() )
		{
			continue;
		}

		if ( player->get_velocity().Length2D() > 0.1f )
		{
			continue;
		}

		if ( !log.record.empty() && player->get_simtime() - log.record[ 0 ].m_sim_time == 0 )
			continue;

		if ( log.m_vecLastNonDormantOrig != player->get_origin() && g_pLocalPlayer->get_alive() )
		{
			log.m_iMode = RMODE_WALL;
		}

		if ( player->get_simtime() - log.m_flLastLowerBodyYawTargetUpdateTime > 1.35f && log.m_vecLastNonDormantOrig == player->get_origin() && log.m_iMode == RMODE_MOVING )
		{
			if ( player->get_simtime() - log.m_flLastLowerBodyYawTargetUpdateTime > 1.65f )
			{
				log.m_iMode = RMODE_WALL;
			}
		}

		if ( log.m_iMode == RMODE_WALL )
		{
			const auto at_target_angle = math::get().calc_angle( player->get_origin(), last_eye );

			Vector left_dir, right_dir, back_dir;
			math::get().angle_vectors( Vector( 0.f, at_target_angle.y - 90.f, 0.f ), &left_dir );
			math::get().angle_vectors( Vector( 0.f, at_target_angle.y + 90.f, 0.f ), &right_dir );
			math::get().angle_vectors( Vector( 0.f, at_target_angle.y + 180.f, 0.f ), &back_dir );

			const auto eye_pos = player->get_eye_pos();
			auto left_eye_pos = eye_pos + ( left_dir * 16.f );
			auto right_eye_pos = eye_pos + ( right_dir * 16.f );
			auto back_eye_pos = eye_pos + ( back_dir * 16.f );

			static CCSWeaponData big_fucking_gun{};
			auto get_big_fucking_gun = [ & ]() -> CCSWeaponData*
			{
				big_fucking_gun.iDamage = 200;
				big_fucking_gun.flRangeModifier = 1.0f;
				big_fucking_gun.flPenetration = 6.0f;
				big_fucking_gun.flArmorRatio = 2.0f;
				big_fucking_gun.flRange = 8192.f;
				return &big_fucking_gun;
			};

			penetration::get().get_damage( g_pLocalPlayer, player, left_eye_pos, &left_damage[ i ], get_big_fucking_gun(), &last_eye );
			penetration::get().get_damage( g_pLocalPlayer, player, right_eye_pos, &right_damage[ i ], get_big_fucking_gun(), &last_eye );
			penetration::get().get_damage( g_pLocalPlayer, player, back_eye_pos, &back_damage[ i ], get_big_fucking_gun(), &last_eye );
		}
	}
}

void resolver::extrpolate_players( const ClientFrameStage_t stage ) const
{
	if ( stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		return;

	if ( !g_pLocalPlayer )
		return;

	if ( !vars::aim.fakelag_compensation.get<bool>() )
		return;

	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto& log = player_log::get().get_log( i );
		auto player = get_entity( i );

		if ( !player || player == g_pLocalPlayer )
			continue;

		if ( player->get_simtime() <= player->get_oldsimtime() )
			continue;

		if ( log.record.empty() )
			continue;

		const auto previous = &log.record[ 0 ];

		if ( previous->m_dormant )
			continue;

		if ( !player->is_enemy() && !vars::aim.resolve_team.get<bool>() )
			continue;

		if ( !player->get_alive() )
			continue;

		if ( player->IsDormant() )
			continue;

		const auto simulation_tick_delta = time_to_ticks( player->get_simtime() - player->get_oldsimtime() );
		if ( simulation_tick_delta > 15 && simulation_tick_delta < 2 )
			return;

		const auto nci = g_pEngine->GetNetChannelInfo();
		const auto delta_ticks = ( std::clamp( time_to_ticks( nci->GetLatency( FLOW_INCOMING ) + nci->GetLatency( FLOW_INCOMING ) ) + g_pGlobals->tickcount - time_to_ticks( player->get_simtime() + lagcomp::get_lerp_time() ), 0, 100 ) ) - simulation_tick_delta;

		auto in_air = false;
		if ( !( player->get_flags() & FL_ONGROUND ) || !( previous->m_flags & FL_ONGROUND ) )
			in_air = true;

		auto ticks_left = static_cast< int >( delta_ticks );
		ticks_left = std::clamp( ticks_left, 0, 10 );
		while ( ticks_left > 0 )
		{
			auto data_origin = player->get_origin();
			auto data_velocity = player->get_velocity();
			auto data_flags = player->get_flags();

			lagcomp::extrapolate( player, data_origin, data_velocity, data_flags, in_air );

			player->get_simtime() += g_pGlobals->interval_per_tick;
			player->get_origin() = data_origin;
			player->get_velocity() = data_velocity;
			player->get_flags() = data_flags;
			--ticks_left;
		}
	}
}

bool resolver::wall_detect( player_log_t* log, lag_record_t* record, float& angle ) const
{
	if ( !g_pLocalPlayer->get_alive() )
		return false;

	const auto at_target_angle = math::get().calc_angle( record->m_origin, last_eye );

	auto set = false;

	const auto left = left_damage[ record->index ];
	const auto right = right_damage[ record->index ];
	const auto back = back_damage[ record->index ];

	auto max_dmg = std::max( left, std::max( right, back ) ) - 1.f;
	if ( left < max_dmg )
	{
		max_dmg = left;
		angle = math::normalize_float( at_target_angle.y + 90.f );
		set = true;
	}
	if ( right < max_dmg )
	{
		max_dmg = right;
		angle = math::normalize_float( at_target_angle.y - 90.f );
		set = true;
	}
	if ( back < max_dmg || !set )
	{
		max_dmg = back;
		angle = math::normalize_float( at_target_angle.y + 180.f );
	}

	return true;
}

void resolver::nospread_resolve( C_CSPlayer* player, lag_record_t* record )
{
	auto& log = player_log::get().get_log( player->EntIndex() );


	const auto simtime = record->m_sim_time;
	const auto choked_ticks = record->m_lag - 1;
	if ( choked_ticks || log.m_flProxyPitch == 180.f )
	{
		if ( log.m_nShots % 5 == 4 )
		{
			const float random_pitch = RandomFlt( -90.0f, 0.0f );
			record->m_eye_angles.x = random_pitch;
		}
		else
		{
			record->m_eye_angles.x = 90.f;
		}
	}

	const auto lby_update_delta = simtime - log.m_flLastLowerBodyYawTargetUpdateTime;
	const auto lby_moving_update_delta = simtime - log.m_flLastMovingLowerBodyYawTargetTime;

	if ( lby_update_delta > 0.1875f && choked_ticks )
	{
		const bool onground = record->m_flags & FL_ONGROUND;
		auto speed = record->m_velocity.Length();
		if ( log.fakewalking ) speed = 0.f;

		if ( onground && speed <= 0.1 && lby_moving_update_delta <= 1.33f )
		{
			if ( log.m_nShots > 0 )
			{
				auto index = log.m_nShots + log.last_hit_bute;
				while ( index > 6 )
					index -= 6;

				if ( log.m_nShots != log.oldshots )
				{
					log.last_hit_bute = index;
					log.oldshots = log.m_nShots;
				}

				log.m_flLastMovingLowerBodyYawTarget = math::normalize_float( log.m_flLowerBodyYawTarget + brutelist[ index ] );
			}

			record->m_eye_angles.y = log.m_flLastMovingLowerBodyYawTarget;
		}
		if ( onground && speed > 0.1f )
		{
			log.m_flLastMovingLowerBodyYawTargetTime = record->m_anim_time;
		}

		if ( speed <= 0.1f || onground )
		{
			if ( onground && speed <= 0.1 && lby_moving_update_delta > 1.11f )
			{
				lag_record_t* ongroundrecord = nullptr;
				for ( auto& current : log.record )
				{
					if ( current.m_flags & FL_ONGROUND )
					{
						ongroundrecord = &current;
						break;
					}
				}
				if ( ongroundrecord )
				{
					if ( speed > 260.f )
					{
						log.onground_nospread = true;
					}
					else
					{
						auto average_lby = log.m_flLowerBodyYawTarget + log.m_flLowerBodyYawTarget - log.m_flOldLowerBodyYawTarget;

						if ( log.m_nShots > 0 )
						{
							auto index = log.m_nShots + log.last_hit_bute;
							while ( index > 6 )
								index -= 6;

							if ( log.m_nShots != log.oldshots )
							{
								log.last_hit_bute = index;
								log.oldshots = log.m_nShots;
							}


							average_lby = log.m_flLowerBodyYawTarget + brutelist[ index ];
						}

						record->m_eye_angles.y = math::get().normalize_float( average_lby );
					}

				}
			}
		}
		else if ( !onground && log.m_nShots > 0 )
		{
			auto index = log.m_nShots;
			while ( index > 6 )
				index -= 6;
			const auto ang = log.m_flLowerBodyYawTarget + brutelist[ index ];

			record->m_eye_angles.y = math::get().normalize_float( ang );
		}

		const auto random_correction = RandomFlt( -35.f, 35.f );

		record->m_eye_angles.y += random_correction;
	}
}

void resolver::override( C_CSPlayer* player, lag_record_t* record )
{
	record->m_override = false;

	static auto had_taget = false;
	if ( !g_pLocalPlayer || !vars::aim.override.get<bool>() || !GetAsyncKeyState( vars::key.override.get<int>() ) )
	{
		had_taget = false;
		return;
	}

	static std::vector<C_CSPlayer*> targets;

	_( weapon_recoil_scale_s, "weapon_recoil_scale" );
	static auto weapon_recoil_scale = g_pCVar->FindVar( weapon_recoil_scale_s );

	static auto last_checked = 0.f;
	if ( last_checked != g_pGlobals->curtime )
	{
		last_checked = g_pGlobals->curtime;
		targets.clear();

		QAngle viewangles;
		g_pEngine->GetViewAngles( viewangles );

		const auto needed_fov = 20.f;
		for ( auto i = 1; i <= g_pGlobals->maxClients; i++ )
		{
			auto ent = get_entity( i );
			if ( !ent || !ent->get_alive() || ent->IsDormant() || !ent->is_enemy() || ent == g_pLocalPlayer || ent->get_player_info().fakeplayer )
				continue;

			const auto fov = math::get().get_fov( viewangles + g_pLocalPlayer->get_aim_punch() * weapon_recoil_scale->get_float(), math::get().calc_angle( g_pLocalPlayer->get_eye_pos(), ent->get_eye_pos() ) );
			if ( fov < needed_fov )
			{
				targets.push_back( ent );
			}
		}
	}

	if ( targets.empty() )
	{
		had_taget = false;
		return;
	}

	auto found = false;
	for ( auto& target : targets )
	{
		if ( player == target )
		{
			found = true;
			break;
		}
	}

	if ( !found )
		return;

	const auto log = &player_log::get().get_log( player->EntIndex() );
	log->m_bOverride = true;

	QAngle viewangles;
	g_pEngine->GetViewAngles( viewangles );

	static auto last_delta = 0.f;
	static auto last_angle = 0.f;

	const auto at_target_yaw = math::get().calc_angle( last_eye, player->get_eye_pos() ).y;
	auto delta = math::get().normalize_float( viewangles.y - at_target_yaw );

	if ( had_taget && fabsf( viewangles.y - last_angle ) < 0.1f )
	{
		viewangles.y = last_angle;
		delta = last_delta;
	}

	had_taget = true;

	if ( player->get_velocity().Length2D() < 75.f )
	{
		if ( delta > 1.2f )
			record->m_eye_angles.y = math::get().normalize_float( at_target_yaw + 90.f );
		else if ( delta < -1.2f )
			record->m_eye_angles.y = math::get().normalize_float( at_target_yaw - 90.f );
		else
			record->m_eye_angles.y = math::get().normalize_float( at_target_yaw );

		record->m_override = true;
	}

	last_angle = viewangles.y;
	last_delta = delta;
}

void resolver::resolve_proxy( const C_CSPlayer * player, float * m_float )
{
	auto PlayerRecord = &player_log::get().get_log( player->EntIndex() );

	PlayerRecord->m_flProxyYaw = math::get().normalize_float( *m_float );
}

void resolver::on_lby_proxy( C_CSPlayer * entity, float* LowerBodyYaw )
{
	float oldBodyYaw; // xmm4_4
	float nextPredictedSimtime; // xmm3_4
	float nextBodyUpdate = 0.f; // xmm3_4

	auto PlayerRecord = &player_log::get().get_log( entity->EntIndex() );

	oldBodyYaw = PlayerRecord->m_flLowerBodyYawTarget;
	if ( oldBodyYaw != *LowerBodyYaw )
	{
		if ( entity != g_pLocalPlayer && entity->get_flags() & FL_ONGROUND )
		{
			nextPredictedSimtime = entity->get_oldsimtime() + g_pGlobals->interval_per_tick;
			float vel = entity->get_velocity().Length2D();
			if ( vel > 0.1f )
				nextBodyUpdate = nextPredictedSimtime + 0.22f;
			else /*if ( PlayerRecord->nextBodyUpdate <= nextPredictedSimtime )*/
				nextBodyUpdate = nextPredictedSimtime + 1.1f;

			if ( nextBodyUpdate != 0.f )
				PlayerRecord->nextBodyUpdate = nextBodyUpdate;
		}

		PlayerRecord->m_flLastLowerBodyYawTargetUpdateTime = entity->get_oldsimtime() + g_pGlobals->interval_per_tick;
		PlayerRecord->m_flOldLowerBodyYawTarget = oldBodyYaw;
		PlayerRecord->m_flLowerBodyYawTarget = math::normalize_float( *LowerBodyYaw );

		//util::print_dev_console( true, Color::Green(), "update time = %f\n", PlayerRecord->m_flLastLowerBodyYawTargetUpdateTime );
	}

	if ( entity->get_velocity().Length2D() > 75.f || entity->get_flags() & FL_DUCKING && entity->get_velocity().Length2D() > 20.f )
	{
		PlayerRecord->m_flLastMovingLowerBodyYawTarget = math::normalize_float( *LowerBodyYaw );
		PlayerRecord->m_flLastMovingLowerBodyYawTargetTime = entity->get_oldsimtime() + g_pGlobals->interval_per_tick;
	}
}

void resolver::resolve_poses( C_CSPlayer * player, player_log_t* log )
{
	auto flPitch = log->record[ 0 ].m_eye_angles.x;
	if ( log->record.size() > 2 && vars::aim.resolver.get<bool>() )
	{
		if ( std::fabsf( flPitch - log->record[ 1 ].m_eye_angles.x ) > 10.f && std::fabsf( flPitch - log->record[ 2 ].m_eye_angles.x ) > 10.f )
		{
			flPitch = log->record[ 1 ].m_eye_angles.x;
			if ( player->get_velocity().Length2D() > 0.1 && player->get_velocity().Length2D() < 75.f || log->record[ 0 ].m_fake_walk || player->get_flags() & FL_DUCKING && player->get_velocity().Length2D() > 20.f || log->record[ 0 ].m_lag > 12 )
				log->record[ 0 ].m_eye_angles.x = flPitch;
		}
	}

	if ( flPitch > 180.0f )
	{
		flPitch -= 360.0f;
	}
	flPitch = std::clamp( flPitch, -90.f, 90.f );
	player->set_pose_param( BODY_PITCH, flPitch );
}

bool resolver::update_lby_timer( C_CSPlayer* player )
{
	const auto playerRecord = &player_log::get().get_log( player->EntIndex() );

	const auto simtime_delta = player->get_simtime() - player->get_oldsimtime();
	if ( !simtime_delta )
		return false;

	const auto animtime = player->get_oldsimtime() + g_pGlobals->interval_per_tick;

	playerRecord->m_bRunningTimer = true;

	if ( player->get_velocity().Length2D() > 0.1f )
	{
		playerRecord->m_bRunningTimer = false;
		return false;
	}

	const auto update_delta = animtime - playerRecord->nextBodyUpdate;
	const auto recorded_update_delta = animtime - playerRecord->m_flLastLowerBodyYawTargetUpdateTime;
	if ( update_delta <= 0 && update_delta > -1.35f && recorded_update_delta < 0.245f )
	{
		playerRecord->nextBodyUpdate += 1.1f;
		return true;
	}

	return false;
}

void resolver::add_shot( const Vector & shotpos, lag_record_t * record, const int& enemy_index )
{
	static auto last_tick = 0;
	if ( g_pGlobals->tickcount != last_tick )
	{
		shots.emplace_back( shotpos, g_pGlobals->tickcount, enemy_index, record );
		last_tick = g_pGlobals->tickcount;
	}
}

void resolver::update_missed_shots( const ClientFrameStage_t& stage )
{
	if ( stage != FRAME_NET_UPDATE_START )
		return;

	auto it = shots.begin();
	while ( it != shots.end() )
	{
		const auto shot = *it;
		if ( shot.tick + 64 < g_pGlobals->tickcount || shot.tick - 2 > g_pGlobals->tickcount )
		{
			it = shots.erase( it );
		}
		else
		{
			++it;
		}
	}

	auto it2 = current_shots.begin();
	while ( it2 != current_shots.end() )
	{
		const auto shot = *it2;
		if ( shot.tick + 64 < g_pGlobals->tickcount || shot.tick - 2 > g_pGlobals->tickcount )
		{
			it2 = current_shots.erase( it2 );
		}
		else
		{
			++it2;
		}
	}
}

std::deque<shot_t>& resolver::get_shots()
{
	return shots;
}

void resolver::hurt_listener( IGameEvent * game_event )
{
	if ( shots.empty() )
		return;

	_( attacker_s, "attacker" );
	_( userid_s, "userid" );
	_( hitgroup_s, "hitgroup" );
	_( dmg_health_s, "dmg_health" );

	const auto attacker = g_pEngine->GetPlayerForUserID( game_event->GetInt( attacker_s ) );
	const auto victim = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid_s ) );
	const auto hitgroup = game_event->GetInt( hitgroup_s );
	const auto damage = game_event->GetInt( dmg_health_s );

	if ( attacker != g_pEngine->GetLocalPlayer() )
		return;

	if ( victim == g_pEngine->GetLocalPlayer() )
		return;

	auto player = get_entity( victim );
	if ( !player || !player->is_enemy() )
		return;

	if ( unapproved_shots.empty() )
		return;

	for ( auto& shot : unapproved_shots )
	{
		if ( !shot.hurt )
		{
			shot.hurt = true;
			shot.hitinfo.victim = victim;
			shot.hitinfo.hitgroup = hitgroup;
			shot.hitinfo.damage = damage;
			return;
		}
	}
}

shot_t* resolver::closest_shot( int tickcount )
{
	shot_t* closest_shot = nullptr;
	auto closest_diff = 64;
	for ( auto& shot : shots )
	{
		const auto diff = abs( tickcount - shot.tick );
		if ( diff <= closest_diff )
		{
			closest_shot = &shot;
			closest_diff = diff;
			continue;
		}


		break;
	}

	return closest_shot;
}

void resolver::record_shot( IGameEvent * game_event )
{
	_( userid_s, "userid" );

	const auto userid = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid_s ) );
	const auto player = get_entity( userid );

	if ( player != g_pLocalPlayer )
		return;

	const auto shot = closest_shot( g_pGlobals->tickcount - time_to_ticks( g_pEngine->GetNetChannelInfo()->GetLatency( FLOW_OUTGOING ) ) );
	if ( !shot )
		return;

	current_shots.push_back( *shot );
}

void resolver::listener( IGameEvent * game_event )
{
	_( weapon_fire, "weapon_fire" );

	if ( !strcmp( game_event->GetName(), weapon_fire ) )
	{
		record_shot( game_event );
		return;
	}

	if ( current_shots.empty() )
		return;

	_( userid_s, "userid" );

	const auto userid = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid_s ) );
	const auto player = get_entity( userid );
	if ( !player || player != g_pLocalPlayer )
		return;

	_( x, "x" );
	_( y, "y" );
	_( z, "z" );

	const Vector pos( game_event->GetFloat( x ), game_event->GetFloat( y ), game_event->GetFloat( z ) );

	if ( vars::misc.impacts.get<bool>() )
		g_pDebugOverlay->AddBoxOverlay( pos, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ), QAngle( 0, 0, 0 ), 0, 0, 155, 127, 4 );

	auto shot = &current_shots[ 0 ];

	static auto last_curtime = 0.f;
	static auto last_length = 0.f;
	static auto counter = 0;

	if ( last_curtime == g_pGlobals->curtime )
		counter++;
	else
	{
		counter = 0;
		last_length = 0.f;
	}

	if ( last_curtime == g_pGlobals->curtime && shot->shotpos.Dist( pos ) <= last_length )
		return;

	last_length = shot->shotpos.Dist( pos );

	if ( counter )
		unapproved_shots.pop_front();

	shot->hitpos = pos;
	unapproved_shots.push_back( *shot );

	last_curtime = g_pGlobals->curtime;
}

void resolver::approve_shots( const ClientFrameStage_t& stage )
{
	if ( stage != FRAME_NET_UPDATE_START )
		return;

	for ( auto& shot : unapproved_shots )
	{
		const auto angles = math::get().calc_angle( shot.shotpos, shot.hitpos );
		Vector direction;
		math::get().angle_vectors( angles, &direction );
		direction.Normalize();

		if ( shot.record.index == -1 )
		{
			if ( shot.hurt )
			{
				const auto player = get_entity( shot.hitinfo.victim );
				if ( player )
				{
					auto origin = player->get_origin();
					origin.z = Vector( shot.shotpos + direction * shot.shotpos.Dist( origin ) ).z;
					auto hitpos = shot.shotpos + direction * shot.shotpos.Dist( origin );

					trace_t tr;
					Ray_t ray;
					ray.Init( shot.shotpos, shot.hitpos );
					g_pTrace->ClipRayToEntity( ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr );

					if ( tr.m_pEnt == player )
						hitpos = tr.endpos;

					hitmarker::get().add_hit( hitmarker_t( g_pGlobals->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, hitpos ) );

					if ( vars::visuals.beams.local.enabled.get<bool>() && vars::visuals.beams.enabled.get<bool>() )
						beams::get().add_local_beam( impact_info_t( g_pGlobals->curtime, shot.shotpos, hitpos, g_pLocalPlayer, get_col( vars::visuals.beams.local.color.get<uintptr_t>() ) ) );
					continue;
				}

			}

			if ( vars::visuals.beams.local.enabled.get<bool>() && vars::visuals.beams.enabled.get<bool>() )
				beams::get().add_local_beam( impact_info_t( g_pGlobals->curtime, shot.shotpos, shot.hitpos, g_pLocalPlayer, get_col( vars::visuals.beams.local.color.get<uintptr_t>() ) ) );
			continue;
		}

		const auto hitpos = shot.hitpos;

		shot.hitpos = shot.shotpos + direction * shot.shotpos.Dist( shot.record.m_origin ) * 1.1f;

		auto player = get_entity( shot.enemy_index );
		if ( !player || player->IsDormant() || !player->get_alive() )
		{
			if ( shot.hurt )
			{
				hitmarker::get().add_hit( hitmarker_t( g_pGlobals->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, shot.shotpos + direction * shot.shotpos.Dist( shot.record.m_origin ) ) );

				if ( vars::visuals.beams.local.enabled.get<bool>() && vars::visuals.beams.enabled.get<bool>() && !beams::get().beam_exists( g_pLocalPlayer, g_pGlobals->curtime ) )
					beams::get().add_local_beam( impact_info_t( g_pGlobals->curtime, shot.shotpos, shot.shotpos + direction * shot.shotpos.Dist( shot.record.m_origin ), g_pLocalPlayer, get_col( vars::visuals.beams.local.color.get<uintptr_t>() ) ) );
			}
			else if ( vars::visuals.beams.local.enabled.get<bool>() && vars::visuals.beams.enabled.get<bool>() )
				beams::get().add_local_beam( impact_info_t( g_pGlobals->curtime, shot.shotpos, hitpos, g_pLocalPlayer, get_col( vars::visuals.beams.local.color.get<uintptr_t>() ) ) );

			continue;
		}

		lag_record_t backup( player );
		shot.record.apply( player );

		trace_t tr;
		Ray_t ray;
		ray.Init( shot.shotpos, shot.hitpos );
		g_pTrace->ClipRayToEntity( ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr );

		backup.apply( player, true );

		if ( tr.m_pEnt == player )
		{
			shot.hitpos = tr.endpos;
			shot.hit = true;
		}

		if ( vars::visuals.beams.local.enabled.get<bool>() && vars::visuals.beams.enabled.get<bool>() )
			beams::get().add_local_beam( impact_info_t( g_pGlobals->curtime, shot.shotpos, shot.hit ? shot.hitpos : hitpos, g_pLocalPlayer, get_col( vars::visuals.beams.local.color.get<uintptr_t>() ) ) );

		if ( shot.hurt )
			hitmarker::get().add_hit( hitmarker_t( g_pGlobals->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, shot.hit ? shot.hitpos : shot.shotpos + direction * shot.shotpos.Dist( shot.record.m_origin ) ) );


		_( MISS, "[ MISS ] " );
		_( missed, "missed due to spread\n" );

		if ( player->get_player_info().fakeplayer )
		{
			if ( !shot.hurt && !shot.hit )
			{
				g_pCVar->ConsoleColorPrintf( Color( 51, 171, 249, 255 ), MISS );
				util::print_dev_console( true, Color( 255, 255, 255, 255 ), missed );
			}

			continue;
		}

		calc_missed_shots( &shot );
	}

	current_shots.clear();
	unapproved_shots.clear();
}


void resolver::calc_missed_shots( shot_t* shot )
{
	if ( !vars::aim.resolver.get<bool>() )
		return;

	const auto log = &player_log::get().get_log( shot->enemy_index );
	if ( shot->record.m_override || shot->record.m_lby_flick || shot->hurt )
		return;

	if ( shot->hit )
	{
		log->m_nShots++;
		return;
	}

	__( weapon_accuracy_nospread, "weapon_accuracy_nospread" );
	static auto nospread = g_pCVar->FindVar( weapon_accuracy_nospread );

	if ( nospread->get_bool() )
		return;

	_( MISS, "[ MISS ] " );
	_( missed, "missed due to spread\n" );

	g_pCVar->ConsoleColorPrintf( Color( 51, 171, 249, 255 ), MISS );
	util::print_dev_console( true, Color( 255, 255, 255, 255 ), missed );
}

float resolver::get_next_update( int index )
{
	return player_log::get().get_log( index ).nextBodyUpdate;
}
