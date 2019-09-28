#include "../include_cheat.h"

lag_record_t::lag_record_t( C_CSPlayer* player )
{
	index = player->EntIndex();
	valid = true;
	m_fake_walk = false;
	m_mode = RMODE_MOVING;
	m_dormant = player->IsDormant();
	m_velocity = player->get_velocity();
	m_origin = player->get_origin();
	m_abs_origin = player->get_abs_origin();
	m_layers = player->get_anim_layers();
	m_poses = player->get_pose_params();
	m_anim_time = player->get_oldsimtime() + g_pGlobals->interval_per_tick;
	m_sim_time = player->get_simtime();
	m_duck = player->get_duck_amt();
	m_body = player_log::get().get_log( index ).m_flLowerBodyYawTarget;
	m_eye_angles = player->get_eye_angles();
	m_abs_ang = player->get_abs_angles();
	m_flags = player->get_flags();
	m_rotation = player->get_rotation();
	m_lag = time_to_ticks( player->get_simtime() - player->get_oldsimtime() );
	m_lby_flick = false;
	m_override = false;
	const auto collideable = player->GetCollideable();
	m_obb_maxs = collideable->OBBMaxs();
	m_obb_mins = collideable->OBBMins();
	m_cached_count = player->get_bone_cache()->m_CachedBoneCount;
	if ( m_cached_count )
	{
		memcpy( matrix, player->get_bone_cache()->m_pCachedBones, 48 * m_cached_count );
		m_writable_bones = *( &player->get_bone_accessor()->m_WritableBones + 8 );
	}
	else m_writable_bones = 0;
}

void lag_record_t::apply( C_CSPlayer* player, bool backup ) const
{
	player->get_flags() = m_flags;
	player->get_simtime() = m_sim_time;
	player->get_eye_angles() = m_eye_angles;
	player->set_abs_angles( m_abs_ang );
	player->get_velocity() = m_velocity;
	player->set_abs_origin( backup ? m_abs_origin : m_origin );
	player->get_origin() = m_origin;
	player->get_lby() = m_body;
	player->get_duck_amt() = m_duck;
	const auto collideable = player->GetCollideable();
	collideable->OBBMaxs() = m_obb_maxs;
	collideable->OBBMins() = m_obb_mins;

	if ( m_cached_count && m_cached_count > 0 )
	{
		memcpy( player->get_bone_cache()->m_pCachedBones, matrix, 48 * m_cached_count );
		*( &player->get_bone_accessor()->m_WritableBones + 8 ) = m_writable_bones;
	}
}

int lagcomp::fix_tickcount( const float& simtime )
{
	return time_to_ticks( simtime + get_lerp_time() );
}

float lagcomp::get_lerp_time()
{
	__( cl_updaterate_s, "cl_updaterate" );
	__( cl_interp_s, "cl_interp" );

	static auto cl_updaterate = g_pCVar->FindVar( cl_updaterate_s );
	static auto cl_interp = g_pCVar->FindVar( cl_interp_s );

	const auto update_rate = cl_updaterate->get_int();
	const auto interp_ratio = cl_interp->get_float();

	auto lerp = interp_ratio / update_rate;

	if ( lerp <= cl_interp->get_float() )
		lerp = cl_interp->get_float();

	return lerp;
}

void lagcomp::extrapolate( C_CSPlayer* player, Vector& origin, Vector& velocity, int& flags, bool wasonground )
{
	__( sv_gravity_s, "sv_gravity" );
	__( sv_jump_impulse_s, "sv_jump_impulse" );

	static auto sv_gravity = g_pCVar->FindVar( sv_gravity_s );
	static auto sv_jump_impulse = g_pCVar->FindVar( sv_jump_impulse_s );

	if ( !( flags & FL_ONGROUND ) )
		velocity.z -= ( g_pGlobals->frametime * sv_gravity->get_float() );
	else if ( wasonground )
		velocity.z = sv_jump_impulse->get_float();

	const Vector mins = player->GetCollideable()->OBBMins();
	const Vector max = player->GetCollideable()->OBBMaxs();

	const Vector src = origin;
	Vector end = src + ( velocity * g_pGlobals->frametime );

	Ray_t ray;
	ray.Init( src, end, mins, max );

	trace_t trace;
	CTraceFilter filter;
	filter.pSkip = ( void* )( player );

	g_pTrace->TraceRay( ray, MASK_PLAYERSOLID, &filter, &trace );

	if ( trace.fraction != 1.f )
	{
		for ( int i = 0; i < 2; i++ )
		{
			velocity -= trace.plane.normal * velocity.Dot( trace.plane.normal );

			const float dot = velocity.Dot( trace.plane.normal );
			if ( dot < 0.f )
			{
				velocity.x -= dot * trace.plane.normal.x;
				velocity.y -= dot * trace.plane.normal.y;
				velocity.z -= dot * trace.plane.normal.z;
			}

			end = trace.endpos + ( velocity * ( g_pGlobals->interval_per_tick * ( 1.f - trace.fraction ) ) );

			ray.Init( trace.endpos, end, mins, max );
			g_pTrace->TraceRay( ray, MASK_PLAYERSOLID, &filter, &trace );

			if ( trace.fraction == 1.f )
				break;
		}
	}

	origin = trace.endpos;
	end = trace.endpos;
	end.z -= 2.f;

	ray.Init( origin, end, mins, max );
	g_pTrace->TraceRay( ray, MASK_PLAYERSOLID, &filter, &trace );

	flags &= ~( 1 << 0 );

	if ( trace.DidHit() && trace.plane.normal.z > 0.7f )
		flags |= ( 1 << 0 );
}

bool lagcomp::valid_simtime( const float & simtime )
{
	const auto nci = g_pEngine->GetNetChannelInfo();
	if ( !nci )
		return false;

	float correct = 0;

	correct += nci->GetLatency( FLOW_OUTGOING );
	correct += nci->GetLatency( FLOW_INCOMING );
	correct += get_lerp_time();

	const auto delta_time = correct - ( g_pGlobals->curtime - simtime );

	return fabsf( delta_time ) <= 0.2f && correct < 1.f;
}
