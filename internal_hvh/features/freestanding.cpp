#include "../include_cheat.h"

void freestanding::update_hotkeys( ClientFrameStage_t stage )
{
	if ( stage != FRAME_RENDER_START || !g_pLocalPlayer || !g_pLocalPlayer->get_alive() )
		return;

	if ( !vars::aa.manual.get<bool>() )
	{
		direction = -1;
		return;
	}

	auto current = -1;

	if ( GetAsyncKeyState( vars::key.aa_left.get<int>() ) )
		current = 1;
	else if ( GetAsyncKeyState( vars::key.aa_right.get<int>() ) )
		current = 2;
	else if ( GetAsyncKeyState( vars::key.aa_back.get<int>() ) )
		current = 0;

	static bool prev_state;
	const auto state = current >= 0;
	if ( prev_state != state )
	{
		if ( state )
		{
			if ( current == direction )
				direction = -1;
			else
				direction = current;
		}

		prev_state = state;
	}
}

bool freestanding::override( float& yaw ) const
{
	if ( !vars::aa.manual.get<bool>() )
		return false;

	QAngle angles;
	g_pEngine->GetViewAngles( angles );
	switch ( direction )
	{
		case 0:
			yaw = math::normalize_float( angles.y + 180.f );
			break;
		case 1:
			yaw = math::normalize_float( angles.y + 90.f );
			break;
		case 2:
			yaw = math::normalize_float( angles.y - 90.f );
			break;
		default: return false;
	}

	return true;
}

bool freestanding::get_real( float& yaw )
{
	if ( override( yaw ) )
		return true;

	if ( !get_antiaim( antiaim::get().type )->freestand->get<bool>() )
		return false;

	if ( g_pClientState->m_nChokedCommands )
		return true;

	get_targets();

	if ( players.empty() )
		return false;

	auto target = players[ 0 ];

	Vector eye_pos = g_pLocalPlayer->get_eye_pos();
	QAngle angle = math::get().calc_angle( eye_pos, target->get_origin() + Vector( 0.f, 0.f, 64.f ) );

	static auto get_rotated_pos = []( Vector start, float rotation, float distance )
	{
		float rad = DEG2RAD( rotation );
		start.x += math::get().fast_cos( rad ) * distance;
		start.y += math::get().fast_sin( rad ) * distance;

		return start;
	};

	static auto get_rotated_damage = []( C_CSPlayer* shooter, Vector shooter_position, Vector local_position, QAngle angle, float* dmg_left, float* dmg_right, float dist )
	{
		*dmg_left = 0.f;
		*dmg_right = 0.f;


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

		CCSWeaponData* freestand_rifle = get_big_fucking_gun();
		bool first_run = true;


		Vector pos_left = get_rotated_pos( local_position, angle.y + 90.f, dist );
		Vector pos_right = get_rotated_pos( local_position, angle.y + 90.f, -dist );

		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;

		filter.pSkip = g_pLocalPlayer;

		auto rightfrac = false;

		ray.Init( local_position, pos_left );
		g_pTrace->TraceRay( ray, MASK_SHOT, &filter, &tr );
		if ( tr.fraction <= 0.98f )
			rightfrac = true;

		tr = trace_t{};
		ray = Ray_t{};

		ray.Init( local_position, pos_right );
		g_pTrace->TraceRay( ray, MASK_SHOT, &filter, &tr );
		if ( tr.fraction <= 0.98f && rightfrac )
			return true;

		float temp_dmg_left;
		float temp_dmg_right;

		penetration::get().get_damage( shooter, g_pLocalPlayer, pos_left, &temp_dmg_left, freestand_rifle, &shooter_position );
		penetration::get().get_damage( shooter, g_pLocalPlayer, pos_right, &temp_dmg_right, freestand_rifle, &shooter_position );

		/*g_pDebugOverlay->AddBoxOverlay( pos_left, Vector( -1, -1, -1 ), Vector( 1, 1, 1 ), QAngle( 0, 0, 0 ), 0, 255, 0, 255, 0.1f );
		g_pDebugOverlay->AddBoxOverlay( pos_right, Vector( -1, -1, -1 ), Vector( 1, 1, 1 ), QAngle( 0, 0, 0 ), 0, 0, 255, 255, 0.1f );*/

		if ( first_run && temp_dmg_left && temp_dmg_right )
			return false; //enemies can hit both sides

		*dmg_left += temp_dmg_left;
		*dmg_right += temp_dmg_right;

		return true;
	};

	float dmg_left_total{};
	float dmg_right_total{};

	const auto target_position = target->get_origin() + Vector( 0.f, 0.f, 64.f );

	int counter_left = 0;
	int counter_right = 0;

	float max = 230.f;
	if ( vars::aa.freestanding_fps.get<bool>() )
		max = 140.f;

	//couldnt hide head and we are not hittable by target, predict target to left/right to see if there is a possible position to hit
	for ( float it = 20.f; it < max; it += 30.f )
	{
		dmg_left_total = 0.f;
		dmg_right_total = 0.f;

		const auto target_position_left = get_rotated_pos( target_position, angle.y + 90.f, it );
		const auto target_position_right = get_rotated_pos( target_position, angle.y + 90.f, -it );

		/*g_pDebugOverlay->AddBoxOverlay( target_position_left, Vector( -1, -1, -1 ), Vector( 1, 1, 1 ), QAngle( 0, 0, 0 ), 0, 255, 0, 255, 0.1f );
		g_pDebugOverlay->AddBoxOverlay( target_position_right, Vector( -1, -1, -1 ), Vector( 1, 1, 1 ), QAngle( 0, 0, 0 ), 0, 0, 255, 255, 0.1f );
*/
		if ( !get_rotated_damage( target, target_position_left, eye_pos, angle, &dmg_left_total, &dmg_right_total, it ) )
			return false; //enemies can hit both side

		//lets see if we can hide our head
		if ( std::max( dmg_left_total, dmg_right_total ) - std::min( dmg_left_total, dmg_right_total ) > 40.f )
		{
			/*if ( !counter_left )
			{
				counter_left++;
			}
			else*/
			{
				yaw = dmg_left_total > dmg_right_total ? angle.y - 90.f : angle.y + 90.f;
				return true;
			}
		}

		if ( !get_rotated_damage( target, target_position_right, eye_pos, angle, &dmg_left_total, &dmg_right_total, it ) )
			return false; //enemies can hit both side

		//lets see if we can hide our head
		if ( std::max( dmg_left_total, dmg_right_total ) - std::min( dmg_left_total, dmg_right_total ) > 40.f )
		{
			/*if ( !counter_right )
			{
				counter_right++;
			}
			else*/
			{
				yaw = dmg_left_total > dmg_right_total ? angle.y - 90.f : angle.y + 90.f;
				return true;
			}
		}
	}

	return false;
}

bool IsVisible( Vector start, Vector end, C_CSPlayer* filter, C_CSPlayer* ent )
{
	trace_t trace;
	util_trace_line( start, end, MASK_SHOT_HULL, filter, &trace );

	return trace.m_pEnt == ent;
}

void freestanding::get_targets()
{
	static float spawntime = 0.f;
	if ( spawntime != g_pLocalPlayer->get_spawn_time() )
	{
		spawntime = g_pLocalPlayer->get_spawn_time();
		players.clear();
	}

	if ( g_pLocalPlayer->get_velocity().Length2D() < 5.f )
		return;

	players.clear();

	C_CSPlayer * target = nullptr;

	QAngle original_viewangles;
	g_pEngine->GetViewAngles( original_viewangles );

	float lowest_distance = 99999.f;
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || !player->is_enemy() || player == g_pLocalPlayer )
			continue;

		if ( player->IsDormant() && ( player->get_simtime() > g_pGlobals->curtime || player->get_simtime() + 5.f < g_pGlobals->curtime ) )
			continue;

		Vector enemy_pos = player->get_origin();
		enemy_pos.z += 64.f;

		if ( IsVisible( g_pLocalPlayer->get_eye_pos(), enemy_pos, g_pLocalPlayer, player ) )
		{
			players.push_back( player );
			break;
		}

		QAngle angle = math::get().calc_angle( g_pLocalPlayer->get_eye_pos(), enemy_pos );
		QAngle delta = angle - original_viewangles;
		delta.Normalize();
		delta.z = 0.f;

		float dist = delta.Length();

		if ( dist < lowest_distance )
		{
			target = player;
			lowest_distance = dist;
		}
	}

	if ( !target )
		return;

	players.push_back( target );
}