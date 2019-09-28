#include "../include_cheat.h"
#include "../misc/render.h"

void visuals::run()
{
	if ( !g_pEngine->IsInGame() || !g_pLocalPlayer )
		return;

	queue_player_data();

	queue_world_data();

	queue_misc_data();
}

void visuals::render()
{
	if ( !g_pEngine->IsInGame() || !g_pLocalPlayer )
		return;

	players_esp();

	world_esp();

	spread_circle();

	draw_scope();

	hitmarker::get().draw_hits();

	aa_indicator();

	ping_indicator();

#ifdef _DEBUG
	render::get().text( Vector2D( 10, 90 ), tfm::format( "%f  :  %f", g_pEngine->GetNetChannelInfo()->GetLatency( FLOW_OUTGOING ), g_pEngine->GetNetChannelInfo()->GetLatency( FLOW_INCOMING ) ), get_col( vars::aa.indicator_col.get<uintptr_t>() ), fonts::lby, c_font::left_aligned | c_font::centered_y | c_font::drop_shadow );
#endif
}

void visuals::players_get()
{
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		players[ i ].index = i;
		players[ i ].player = player;
		if ( !player || !player->get_alive() || !player->is_enemy() || player == g_pLocalPlayer || ( player->IsDormant() && players[ i ].was_unseen ) )
		{
			players[ i ].alpha = 0.f;
			players[ i ].valid = false;
			players[ i ].was_unseen = true;
			continue;
		}

		players[ i ].valid = true;

		if ( ( *g_ppGameRules )->m_bFreezePeriod() )
		{
			if ( player->IsDormant() )
				players[ i ].valid = false;
			players[ i ].was_unseen = true;
			continue;
		}

		players[ i ].was_unseen = false;
		if ( player->IsDormant() )
		{
			if ( players[ i ].alpha < 160.f )
			{
				players[ i ].alpha -= 255.f / 16.f * g_pGlobals->frametime;
				players[ i ].alpha = std::clamp( players[ i ].alpha, 0.f, 160.f );
			}
			else
			{
				players[ i ].alpha -= 255.f / 1.f * g_pGlobals->frametime;
			}
		}
		else
		{
			players[ i ].alpha += 255.f / 0.68f * g_pGlobals->frametime;
			players[ i ].alpha = std::clamp( players[ i ].alpha, 0.f, 255.f );
		}
	}
}

void visuals::player_box( const visual_data_t & player )
{
	if ( !vars::visuals.esp.box.get<bool>() )
		return;

	const auto color = Color( vars::visuals.esp.box_color.get<D3DCOLOR>(), player.alpha );
	render::get().bordered_box_outlined( player.top.x - player.width, player.top.y, player.width * 2, player.height, color, Color( 0, 0, 0, player.alpha ) );
}

void visuals::player_info( const visual_data_t & player )
{
	const auto info_bottom = [ & ]()
	{
		auto elements = 0;

		if ( vars::visuals.esp.lby_timer.get<bool>() )
		{
			if ( bar_lby_update( player, elements ) )
				elements++;
		}

		if ( vars::visuals.esp.armor.get<bool>() )
		{
			bar_armor( player, elements );
			elements++;
		}

		if ( vars::visuals.esp.ammo.get<bool>() )
		{
			bar_ammo( player, elements );
			elements++;
		}

		if ( vars::visuals.esp.weapon.get<bool>() )
		{
			auto color = Color( vars::visuals.esp.weapon_color.get<D3DCOLOR>(), player.alpha );
			render::get().text( Vector2D( player.top.x, player.top.y + player.height + 6 * elements++ ), player.weapon_name, color, fonts::esp_small, c_font::centered_x | c_font::drop_shadow );
		}
	};

	const auto info_left = [ & ]()
	{
		auto elements = 0;

		if ( vars::visuals.esp.health.get<bool>() )
		{
			bar_health( player, elements );
			elements++;
		}
	};

	const auto info_top = [ & ]( const visual_data_t & player ) // bug? have to pass as arg else undefined
	{
		auto elements = 0;

		if ( vars::visuals.esp.ping.get<bool>() )
		{
			if ( bar_ping( player, elements ) )
				elements++;
		}

		if ( vars::visuals.esp.name.get<bool>() )
		{
			auto color = Color( vars::visuals.esp.name_color.get<D3DCOLOR>(), player.alpha );
			render::get().text( Vector2D( player.top.x, player.top.y - 16 - 6 * elements++ ), player.name, color, fonts::esp_small, c_font::centered_x | c_font::drop_shadow );
		}
	};

	const auto info_right = [ & ]()
	{
		auto elements = 0;

#ifdef _DEBUG

		render::get().text( Vector2D( player.top.x + player.width + 4, player.top.y + 8 * elements++ ), std::to_string( player.shots ), player.shots ? D3DCOLOR_ARGB( static_cast< int >( player.alpha ), 255, 0, 0 ) : D3DCOLOR_ARGB( static_cast< int >( player.alpha ), 180, 255, 0 ), fonts::esp_small, c_font::left_aligned | c_font::drop_shadow );
		if ( player.vel < 0.1f )
			render::get().text( Vector2D( player.top.x + player.width + 4, player.top.y + 8 * elements++ ), player.rmode, D3DCOLOR_ARGB( static_cast< int >( player.alpha ), 180, 255, 0 ), fonts::esp_small, c_font::left_aligned | c_font::drop_shadow );
#endif

		if ( player.scoped && vars::visuals.esp.scoped.get<bool>() )
		{
			_( SCOPED, "SCOPED" );
			render::get().text( Vector2D( player.top.x + player.width + 4, player.top.y + 8 * elements++ ), SCOPED, Color( 0, 153, 204, player.alpha ), fonts::esp_small, c_font::left_aligned | c_font::drop_shadow );
		}
		if ( player.bomb && vars::visuals.esp.bomb.get<bool>() )
		{
			_( BOMB, "BOMB" );
			render::get().text( Vector2D( player.top.x + player.width + 4, player.top.y + 8 * elements++ ), BOMB, Color( 255, 0, 0, player.alpha ), fonts::esp_small, c_font::left_aligned | c_font::drop_shadow );
		}
	};


	info_left();
	info_right();

	if ( ( player.width * 2 - 2 ) < 1.f )
		return;

	info_bottom();
	info_top( player );


}

bool visuals::bar_ping( const visual_data_t& player, int elements ) const
{
	if ( player.ping < 0.2f )
		return false;

	const auto ping = std::clamp( player.ping, 0.0f, 1.f );
	const auto width = ( player.width * 2.f - 2 ) * ping;

	const auto color = vars::visuals.esp.ping_color.get<D3DCOLOR>();

	render::get().filled_box_outlined( player.top.x - player.width, player.top.y - 6 - 4 * elements, player.width * 2, 4, Color( 0, 0, 0, static_cast< int >( player.alpha * 0.3f ) ), Color( 0, 0, 0, static_cast< int >( player.alpha ) ) );
	render::get().filled_box( player.top.x - player.width + 1, player.top.y - 5 - 4 * elements, width, 2, Color( color, player.alpha ) );

	return true;
}


void visuals::bar_health( const visual_data_t& player, const int elements ) const
{
	auto health = player.health;
	health = std::clamp( health, 0, 100 );
	auto multiplier = health / 100.f;
	multiplier = std::clamp( multiplier, 0.f, 1.f );
	const auto height = ( player.height - 2 ) * multiplier;

	const int red = 255 - health * 2.55;
	const int green = health * 2.55;

	render::get().filled_box_outlined( player.top.x - player.width - 5 - elements * 6, player.top.y, 4, player.height, Color( 0, 0, 0, static_cast< int >( player.alpha * 0.3f ) ), Color( 0, 0, 0, static_cast< int >( player.alpha ) ) );
	render::get().filled_box( player.top.x - player.width - 4 - elements * 6, player.top.y + player.height - height - 1, 2, height, Color( red, green, 0, static_cast< int >( player.alpha ) ) );

	_( d, "%d" );

	if ( health != 100 )
		render::get().text( Vector2D( player.top.x - player.width - 3, player.top.y + player.height - height - 5 ), tfm::format( d, player.health ), Color( 255, 255, 255, static_cast< int >( player.alpha ) ), fonts::esp_small, c_font::centered_x | c_font::centered_y | c_font::drop_shadow );
}

void visuals::bar_armor( const visual_data_t& player, const int elements )
{
	const auto width = ( player.width * 2.f - 2 ) * player.armor * 0.01f;

	const auto color = vars::visuals.esp.armor_color.get<D3DCOLOR>();

	render::get().filled_box_outlined( player.top.x - player.width, player.top.y + player.height + 1 + elements * 6, player.width * 2, 4, Color( 0, 0, 0, static_cast< int >( player.alpha * 0.3f ) ), Color( 0, 0, 0, static_cast< int >( player.alpha * 0.8f ) ) );
	if ( !player.dormant )
		render::get().filled_box( player.top.x - player.width + 1, player.top.y + player.height + 2 + elements * 6, width, 2, Color( color, player.alpha ) );
}

void visuals::bar_ammo( const visual_data_t& player, const int elements ) const
{
	auto clip = player.clip;
	auto maxclip = player.maxclip;
	clip = std::clamp( clip, 0, 1000 );
	maxclip = std::clamp( maxclip, 1, 1000 );

	auto width = player.width * 2 - 2;
	if ( player.act == ACT_CSGO_RELOAD && player.l1_weight != 0.f )
		width *= player.l1_cycle;
	else
		width = width * clip / maxclip;

	const auto color = vars::visuals.esp.ammo_color.get<D3DCOLOR>();

	render::get().filled_box_outlined( player.top.x - player.width, player.top.y + player.height + 1 + elements * 6, player.width * 2, 4, D3DCOLOR_ARGB( static_cast< int >( player.alpha * 0.3f ), 0, 0, 0 ), D3DCOLOR_ARGB( static_cast< int >( player.alpha * 0.8f ), 0, 0, 0 ) );
	if ( !player.dormant )
	{
		render::get().filled_box( player.top.x - player.width + 1, player.top.y + player.height + 2 + elements * 6, width, 2, Color( color, player.alpha ) );
		_( d, "%d" );

		if ( clip < maxclip * 0.5 && player.clip != -1 && clip >= 0 )
			render::get().text( Vector2D( player.top.x - player.width + 1 + width, player.top.y + player.height + 1 + elements * 6 ), tfm::format( d, clip ), Color( 255, 255, 255, static_cast< int >( player.alpha ) ), fonts::esp_small, c_font::centered_x | c_font::centered_y | c_font::drop_shadow );
	}

}

bool visuals::bar_lby_update( const visual_data_t& player, const int elements ) const
{
	return false;

	if ( !player.running_timer )
		return false;

	auto next_update = player.nextupdate - g_pGlobals->curtime;
	next_update = std::clamp( next_update, 0.f, 1.1f );
	next_update = -next_update + 1.1f;

	const auto width = ( player.width * 2 - 2 ) * next_update / 1.1f;
	std::clamp( width, 0.f, player.width * 2 - 2 );

	render::get().filled_box_outlined( player.top.x - player.width, player.top.y + player.height + 1 + elements * 6, player.width * 2, 4, D3DCOLOR_ARGB( static_cast< int >( player.alpha * 0.3f ), 0, 0, 0 ), D3DCOLOR_ARGB( static_cast< int >( player.alpha * 0.8f ), 0, 0, 0 ) );
	if ( !player.dormant )
		render::get().filled_box( player.top.x - player.width + 1, player.top.y + player.height + 2 + elements * 6, width, 2, D3DCOLOR_ARGB( static_cast< int >( player.alpha ), 255, 130, 0 ) );

	return true;
}

void visuals::player_out_of_fov( const Vector& pos ) const
{
	if ( !vars::visuals.out_of_fov.get<bool>() )
		return;

	Vector screenpos;
	math::get().world_to_screen( pos, screenpos );


	const auto screen_center2D = render::get().get_center();
	const auto screen_center = Vector( screen_center2D.x, screen_center2D.y, 0 );
	const auto dimensions = render::get().get_dimensions();

	const auto angle = math::get().calc_angle( screen_center, screenpos );
	Vector forward;
	math::get().angle_vectors( angle, &forward );
	const auto width = dimensions.Width / 130;
	const auto height = dimensions.Width / 150 * 2;
	const auto dist = ( vars::visuals.out_of_fov_dst.get<int>() / 100.f ) * dimensions.Height + height;

	const auto result = screen_center + forward * dist;
	const auto extended = forward * ( dist - height );
	const auto right = forward.Cross( Vector( 0, 0, 1 ) );
	const auto left = Vector( -right.x, -right.y, right.z );
	POINT one;
	one.x = result.x;
	one.y = result.y;
	POINT two;
	two.x = screen_center.x + extended.x + right.x * width;
	two.y = screen_center.y + extended.y + right.y * width;
	POINT three;
	three.x = screen_center.x + extended.x + left.x * width;
	three.y = screen_center.y + extended.y + left.y * width;

	static auto swap = false;
	static auto alpha = 255.f;
	static auto last_framecount = 0;

	if ( alpha == 255.f )
	{
		swap = false;
	}
	if ( alpha == 0.f )
	{
		swap = true;
	}
	if ( last_framecount != g_pGlobals->framecount )
	{
		if ( swap )
			alpha += 255.f / 0.73f * g_pGlobals->frametime;
		else
			alpha -= 255.f / 0.73f * g_pGlobals->frametime;
		alpha = std::clamp( alpha, 0.f, 255.f );
		last_framecount = g_pGlobals->framecount;
	}

	const auto col = Color( vars::visuals.out_of_fov_col.get<D3DCOLOR>(), alpha );
	render::get().filled_triangle( one, two, three, col );
}

void visuals::queue_world_data()
{
	const auto is_grenade = []( const int id )
	{
		return id == ClassId_CDecoyGrenade || id == ClassId_CHEGrenade || id == ClassId_CIncendiaryGrenade || id == ClassId_CMolotovGrenade || id == ClassId_CSmokeGrenade || id == ClassId_CFlashbang || id == ClassId_CDecoyProjectile || id == ClassId_CBaseCSGrenadeProjectile || id == ClassId_CMolotovProjectile || id == ClassId_CSmokeGrenadeProjectile || id == ClassId_CFlashbang;
	};

	const auto is_grenade_projectile = []( const int id )
	{
		return id == ClassId_CDecoyProjectile || id == ClassId_CBaseCSGrenadeProjectile || id == ClassId_CMolotovProjectile || id == ClassId_CSmokeGrenadeProjectile || id == ClassId_CFlashbang;
	};

	const auto is_weapon = []( const int id )
	{
		return id >= ClassId_CWeaponAug && id <= ClassId_CWeaponXM1014 || id == ClassId_CAK47 || id == ClassId_CDEagle;
	};

	visual_world_data_t post;

	for ( auto i = 1; i < g_pEntitiyList->GetHighestEntityIndex(); i++ )
	{
		const auto ent = get_entity( i );
		if ( !ent )
			continue;

		const auto id = ent->GetClientClass()->m_ClassID;
		if ( ( !is_grenade( id ) || !vars::visuals.esp.world.grenades.get<bool>() ) && ( !is_weapon( id ) || !vars::visuals.esp.world.weapons.get<bool>() ) && ( ( id != ClassId_CC4 && id != ClassId_CPlantedC4 ) || !vars::visuals.esp.world.c4.get<bool>() ) )
			continue;

		auto dist = g_pLocalPlayer->get_abs_origin().Dist( ent->get_origin() );
		if ( !g_pLocalPlayer->get_alive() )
		{
			const auto observer = g_pLocalPlayer->get_observer();
			if ( observer )
				dist = observer->get_abs_origin().Dist( ent->get_origin() );
		}
		auto alpha = 255.f;
		if ( ( is_grenade( id ) && !is_grenade_projectile( id ) ) || is_weapon( id ) )
		{
			const auto cl_dist = std::clamp( dist - 300.f, 0.f, 510.f );
			alpha = 255.f - cl_dist / 2;
		}

		if ( !alpha )
			continue;

		Vector screenpos;
		if ( ent->get_origin() == Vector( 0.f, 0.f, 0.f ) || !math::get().world_to_screen( ent->get_origin(), screenpos ) )
			continue;

		world_data_t data;
		data.alpha = alpha;
		data.classid = id;
		data.pos = screenpos;
		if ( is_weapon( id ) )
		{
			auto weapon = reinterpret_cast< C_BaseCombatWeapon* >( ent );
			std::wstring conv( weapon->get_diplay_name() );
			data.name = std::string( conv.begin(), conv.end() );
			if ( is_weapon( id ) )
			{
				data.clip = weapon->get_clip1();
				data.maxclip = weapon->get_wpn_data()->iMaxClip1;
			}
		}
		else if ( is_grenade( id ) )
		{
			const auto weapon = reinterpret_cast< C_BaseEntity* >( ent );
			const auto model = weapon->GetModel();
			if ( model )
			{
				const auto hdr = g_pModelInfo->GetStudioModel( model );
				if ( hdr )
				{
					const auto hdr_name = hdr->name;
					_( smokegrenade, "smokegrenade" );
					_( molotov, "molotov" );
					_( incendiarygrenade, "incendiarygrenade" );
					_( fraggrenade, "fraggrenade" );
					_( flashbang, "flashbang" );
					_( decoy, "decoy" );
					if ( strstr( hdr_name, smokegrenade ) )
					{
						_( Smoke, "Smoke" );
						data.name = Smoke;
					}
					else if ( strstr( hdr_name, molotov ) )
					{
						_( Molotov, "Molotov" );
						data.name = Molotov;
					}
					else if ( strstr( hdr_name, incendiarygrenade ) )
					{
						_( Incendiary, "Incendiary" );
						data.name = Incendiary;
					}
					else if ( strstr( hdr_name, fraggrenade ) )
					{
						_( Frag, "Frag grenade" );
						data.name = Frag;
					}
					else if ( strstr( hdr_name, flashbang ) )
					{
						_( Flashbang, "Flashbang" );
						data.name = Flashbang;
					}
					else if ( strstr( hdr_name, decoy ) )
					{
						_( Decoy, "Decoy" );
						data.name = Decoy;
					}
				}
			}
		}
		else
		{
			_( C4, "C4" );
			data.name = C4;
			if ( id == ClassId_CPlantedC4 )
			{
				auto c4 = reinterpret_cast< C_PlantedC4* >( ent );

				data.defusing = c4->get_defuser() != -1 || c4->get_defused();
				data.defuser = c4->get_defuser();
				if ( c4->get_ticking() && c4->get_timer() > 0.f )
					data.explode_time = c4->get_timer() - g_pGlobals->curtime;
				else
					data.explode_time = 0.f;
			}
		}

		post.world_data.push_back( data );
	}

	world_data.post( visual_world_data_t( post ) );
}

void visuals::queue_player_data()
{
	players_get();

	for ( auto& player : players )
	{
		if ( !player.valid )
			continue;

		player.oof = false;
		const auto max = player.player->GetCollideable()->OBBMaxs();
		Vector pos;
		const auto pos_3d = player.player->get_abs_origin();
		const auto top_3d = pos_3d + Vector( 0, 0, max.z );
		math::get().world_to_screen( pos_3d, pos );
		math::get().world_to_screen( top_3d, player.top );
		player.height = pos.y - player.top.y + 6;
		player.width = player.height / 3.4f;

		const auto size = render::get().get_dimensions();

		player.pos = pos_3d;
		if ( pos.x + player.width + 20 < 0 || pos.x - player.width - 20 > size.Width || pos.y + 20 < 0 || pos.y - player.height - 20 > size.Height )
			player.oof = true;

		player.vel = player.player->get_velocity().Length2D();
		const auto& layers = player.player->get_anim_layers();
		const auto act = player.player->get_sequence_act( layers[ 1 ].m_nSequence );
		player.act = act;
		player.l1_weight = layers[ 1 ].m_flWeight;
		player.l1_cycle = layers[ 1 ].m_flCycle;
		player.dormant = player.player->IsDormant();
		player.nextupdate = resolver::get().get_next_update( player.index );
		const auto weapon = get_weapon( player.player->get_active_weapon() );
		if ( weapon && weapon->get_wpn_data() )
		{
			player.maxclip = weapon->get_wpn_data()->iMaxClip1;
			player.clip = weapon->get_clip1();
		}
		player.armor = player.player->get_armor();
		player.health = player.player->get_health();
		player.scoped = player.player->get_scoped();
		player.ping = ( *g_ppPlayerResource )->get_ping( player.index ) / 1000.f;
		player.bomb = ( *g_ppPlayerResource )->get_c4_player() == player.index;

		auto& record = player_log::get().get_log( player.index );
#ifdef _DEBUG
		switch ( record.m_iMode )
		{
			case RMODE_MOVING:
				player.rmode = "moving";
				break;
			case RMODE_DELTA:
				player.rmode = "delta";
				break;
			case RMODE_SPIN:
				player.rmode = "spin";
				break;
			case RMODE_WALL:
				player.rmode = "wall";
				break;
			default:
				player.rmode = "";
				break;
		}
#endif
		player.running_timer = record.m_bRunningTimer;
		player.shots = record.m_nShots;
		player.fakewalking = record.fakewalking;
		auto name = player.player->get_name();
		if ( name.size() > 10 )
		{
			name.resize( 10 );
			_( dot, "..." );
			name.append( dot );
		}
		player.name = name;
		if ( weapon )
		{
			std::wstring conv( weapon->get_diplay_name() );
			player.weapon_name = std::string( conv.begin(), conv.end() );
		}

		player_data[ player.index ].post( visual_data_t( player ) );
	}
}

void visuals::queue_misc_data()
{
	if ( !vars::visuals.spread_circle.get<bool>() || !g_pLocalPlayer || !g_pLocalPlayer->get_alive() )
		return;

	const auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );
	if ( !weapon )
		return;

	misc_data.post( misc_data_t( weapon->get_spread() + weapon->get_inaccuracy() ) );
}

void visuals::players_esp()
{
	for ( auto &current_data : player_data )
	{
		if ( !current_data.size() )
			continue;

		visual_data_t player;
		if ( !current_data.wait_for( player, std::chrono::milliseconds( 5 ) ) )
			continue;

		if ( !player.valid )
		{
			current_data.clear();
			continue;
		}

		if ( player.alpha > 0.f )
		{
			if ( player.oof )
			{
				player_out_of_fov( player.pos );
				continue;
			}

			player_box( player );
			player_info( player );
		}

		if ( current_data.size() > 1 )
			current_data.clear();
	}
}

void visuals::defuse_bar( const world_data_t & ent ) const
{
	static auto was_defusing = false;
	static float defusetime;
	if ( !was_defusing && ent.defusing )
	{
		const auto defuser = reinterpret_cast< C_CSPlayer* >( g_pEntitiyList->GetClientEntityFromHandle( ent.defuser ) );
		if ( defuser )
		{
			if ( defuser->has_defuser() )
				defusetime = prediction::get_curtime() + 5.f;
			else
				defusetime = prediction::get_curtime() + 10.f;

			was_defusing = true;
		}
	}
	else if ( !ent.defusing )
		was_defusing = false;

	if ( !ent.defusing )
		return;

	auto multiplier = ( defusetime - prediction::get().get_curtime() ) / 10.f;
	multiplier = std::clamp( multiplier, 0.f, 1.f );

	const auto red = std::clamp( static_cast< int >( ( 1 - multiplier ) * 160 ) + 160, 0, 255 );
	const auto green = std::clamp( static_cast< int >( multiplier * 255 ), 0, 255 );

	auto width = 50.f - 50.f * multiplier;
	width = std::clamp( width, 0.f, 50.f );

	render::get().filled_box_outlined( ent.pos.x - 26, ent.pos.y + 23, 52, 4, D3DCOLOR_ARGB( static_cast< int >( ent.alpha * 0.3f ), 0, 0, 0 ), D3DCOLOR_ARGB( static_cast< int >( ent.alpha * 0.8f ), 0, 0, 0 ) );
	render::get().filled_box( ent.pos.x - 25, ent.pos.y + 24, width, 2, D3DCOLOR_ARGB( static_cast< int >( ent.alpha ), red, green, 0 ) );
}

void visuals::ammo_bar( const world_data_t & ent ) const
{
	auto clip = ent.clip;
	auto maxclip = ent.maxclip;
	clip = std::clamp( clip, 0, 1000 );
	maxclip = std::clamp( maxclip, 1, 1000 );

	const auto width = std::clamp( 50.f * clip / maxclip, 0.f, 50.f );

	render::get().filled_box_outlined( ent.pos.x - 26, ent.pos.y + 17, 52, 4, D3DCOLOR_ARGB( static_cast< int >( ent.alpha * 0.3f ), 0, 0, 0 ), D3DCOLOR_ARGB( static_cast< int >( ent.alpha * 0.8f ), 0, 0, 0 ) );
	render::get().filled_box( ent.pos.x - 25, ent.pos.y + 18, width, 2, D3DCOLOR_ARGB( static_cast< int >( ent.alpha ), 206, 147, 216 ) );
	//render::get().text( tfm::format( "%d / %d", ent.clip, ent.maxclip ), ent.pos.x, ent.pos.y + 20, CENTERED, fonts::esp, true, D3DCOLOR_ARGB( static_cast< int >( ent.alpha ), 255, 255, 255 ) );
}

void visuals::world_info( const world_data_t & ent ) const
{
	const auto name = ent.name;
	if ( ent.classid == ClassId_CPlantedC4 )
	{
		_( time, "%.2fs" );
		render::get().text( Vector2D( ent.pos.x, ent.pos.y + 8 ), tfm::format( time, ent.explode_time ), Color( 255, 0, 0, static_cast< int >( ent.alpha ) ), fonts::esp, c_font::centered_x | c_font::centered_y | c_font::drop_shadow );
		defuse_bar( ent );
	}

	const auto color = Color( vars::visuals.esp.world.color.get<D3DCOLOR>(), ent.alpha );

	if ( ent.classid == ClassId_CPlantedC4 || ent.classid == ClassId_CC4 )
		render::get().text( Vector2D( ent.pos.x, ent.pos.y - 4 ), name, Color( 255, 255, 255, ent.alpha ), fonts::esp, c_font::centered_x | c_font::centered_y | c_font::drop_shadow );
	else
		render::get().text( Vector2D( ent.pos.x, ent.pos.y + 4 ), name, color, fonts::esp, c_font::centered_x | c_font::centered_y | c_font::drop_shadow );

	const auto is_weapon = []( const int id )
	{
		return id >= ClassId_CWeaponAug && id <= ClassId_CWeaponXM1014 || id == ClassId_CAK47 || id == ClassId_CDEagle;
	};

	if ( is_weapon( ent.classid ) )
	{
		ammo_bar( ent );
	}
}

void visuals::world_esp()
{
	if ( !world_data.size() )
		return;

	visual_world_data_t data;
	if ( !world_data.wait_for( data, std::chrono::milliseconds( 5 ) ) )
		return;

	for ( auto & current : data.world_data )
		world_info( current );

	if ( world_data.size() > 2 )
		world_data.clear();
}

void visuals::draw_scope()
{
	auto observer = g_pLocalPlayer->get_observer();
	if ( ( observer && observer->get_scoped() || g_pLocalPlayer->get_scoped() ) && vars::misc.remove_scope.get<bool>() )
	{
		const auto size = render::get().get_dimensions();

		render::get().line( Vector2D( 0, size.Y + size.Height / 2 ), Vector2D( size.Width, size.X + size.Height / 2 ), Color( 0, 0, 0, 255 ) );
		render::get().line( Vector2D( size.Width / 2, 0 ), Vector2D( size.Width / 2, size.Height ), Color( 0, 0, 0, 255 ) );
	}
}

void visuals::aa_indicator()
{
	if ( !vars::aa.indicator.get<bool>() || !vars::aa.enabled.get<bool>() || !g_pLocalPlayer->get_alive() )
		return;

	const auto size = render::get().get_dimensions();

	const auto mode = freestanding::get().get_mode();

	std::string text;
	switch ( mode )
	{
		case -1:
		{
			_( auto_s, "auto" );
			text = auto_s;
			break;
		}
		case 0:
		{
			_( back, "back" );
			text = back;
			break;
		}
		case 1:
		{
			_( left, "left" );
			text = left;
			break;
		}
		case 2:
		{
			_( right, "right" );
			text = right;
			break;
		}
		default:
			break;
	}

	render::get().text( Vector2D( 10, size.Height / 2.f ), text, get_col( vars::aa.indicator_col.get<uintptr_t>() ), fonts::lby, c_font::left_aligned | c_font::centered_y | c_font::drop_shadow );
}

void visuals::ping_indicator()
{
	if ( !vars::aim.ping_spike.get<bool>() && !vars::aim.ping_spike_base.get<bool>() || !vars::aim.enabled.get<bool>() || !g_pEngine->IsInGame() )
		return;

	const auto nci = g_pEngine->GetNetChannelInfo();
	if ( !nci )
		return;

	auto ping = vars::aim.ping_spike_amt.get<float>() / 1000.f;
	if ( !vars::aim.ping_spike.get<bool>() )
		ping = 160.f / 1000.f;
	auto multiplier = nci->GetLatency( FLOW_INCOMING ) / ping;
	multiplier = std::clamp( multiplier, 0.f, 1.f );

	const auto red = std::clamp( static_cast< int >( ( 1 - multiplier ) * 120 ) + 160, 0, 255 );
	const auto green = std::clamp( static_cast< int >( multiplier * 255 ) + 160, 0, 255 );

	const auto size = render::get().get_dimensions();

	_( ping_s, "spike" );
	_( pingex_s, "extended" );
	render::get().text( Vector2D( 10, size.Height / 2.f + 18 ), vars::aim.ping_spike.get<bool>() ? ping_s : pingex_s, Color( red, green, 0, 255 ), fonts::lby, c_font::left_aligned | c_font::centered_y | c_font::drop_shadow );
}

void visuals::spread_circle()
{
	if ( !vars::visuals.spread_circle.get<bool>() || !g_pLocalPlayer || !g_pLocalPlayer->get_alive() )
		return;

	const auto ssize = render::get().get_dimensions();

	misc_data_t data;
	if ( !misc_data.wait_for( data, std::chrono::milliseconds( 5 ) ) )
		return;

	if ( data.spread == -1.f )
		return;

	if ( misc_data.size() > 2 )
		misc_data.clear();

	auto cone = data.spread;
	if ( cone > 0.0f )
	{
		const int cross_x = ssize.X + ssize.Width / 2;
		const int cross_y = ssize.Y + ssize.Height / 2;
		if ( cone < 0.004f ) cone = 0.004f;
		const auto size = ( cone * ssize.Height ) * 0.7f;
		const auto color1 = vars::visuals.spread_circle_color1.get<D3DCOLOR>();
		const auto color2 = vars::visuals.spread_circle_color2.get<D3DCOLOR>();

		switch ( vars::visuals.spread_circle_type.get<int>() )
		{
			case 0:
				render::get().circle_filled_radial_gradient( Vector2D( cross_x, cross_y ), size, color1, color2 );
				break;
			case 1:
				render::get().circle_filled_rainbow( Vector2D( cross_x, cross_y ), size, false );
				break;
			case 2:
				render::get().circle_filled_rainbow( Vector2D( cross_x, cross_y ), size, true );
				break;
			default:
				break;
		}
	}
}
