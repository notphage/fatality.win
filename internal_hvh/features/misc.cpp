#include "../include_cheat.h"

void misc::bhop()
{
	if ( !vars::misc.bhop.get<bool>() )
		return;

	static auto last_jumped = false;
	static auto should_fake = false;

	if ( !last_jumped && should_fake )
	{
		should_fake = false;
		g_cmd->buttons |= IN_JUMP;
	}
	else if ( g_cmd->buttons & IN_JUMP )
	{
		if ( g_pLocalPlayer->get_flags() & FL_ONGROUND )
		{
			last_jumped = true;
			should_fake = true;
		}
		else
		{
			g_cmd->buttons &= ~IN_JUMP;
			last_jumped = false;
		}
	}
	else
	{
		last_jumped = false;
		should_fake = false;
	}
}

void misc::remove_smoke( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_RENDER_START || !vars::misc.remove_smoke.get<bool>() )
		return;

	static auto smoke_count = *reinterpret_cast< uintptr_t* >( offsets::smoke_count );
	*reinterpret_cast< int* >( smoke_count ) = 0;
}

void misc::remove_flash( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_RENDER_START || !vars::misc.remove_flash.get<bool>() || !g_pLocalPlayer )
		return;

	g_pLocalPlayer->get_flash_alpha() = 0.f;
}

void misc::radar()
{
	if ( !vars::misc.radar.get<bool>() )
		return;

	for ( auto i = 1; i <= g_pGlobals->maxClients; i++ )
	{
		if ( i == g_pEngine->GetLocalPlayer() )
			continue;

		auto player = get_entity( i );
		if ( !player )
			continue;

		player->get_spotted() = true;
	}
}

void misc::reveal_ranks()
{
	if ( !vars::misc.reveal_ranks.get<bool>() || !( g_cmd->buttons & IN_SCORE ) )
		return;

	static auto reveal_ranks = reinterpret_cast< bool( __cdecl* )( float* ) >( sig( "client.dll", "55 8B EC 8B 0D ? ? ? ? 68" ) );
	float array[ 3 ] = { 0,0,0 };

	reveal_ranks( array );
}

void misc::soundesp( const int idx, const char* sample, const Vector& origin )
{
	if ( INRANGE( idx, 1, g_pGlobals->maxClients ) )
	{
		auto player = get_entity( idx );

		if ( !player || !player->get_alive() || player == g_pLocalPlayer || player->GetClientClass()->m_ClassID != 35 || !player->IsDormant() )
			return;

		_( footsteps, "player/footsteps" );
		_( land, "player/land" );

		if ( !strstr( sample, footsteps ) && !strstr( sample, land ) )
			return;

		player->get_origin() = origin;
		player->get_oldorigin() = origin;
		player->set_abs_origin( origin );
	}
}

void misc::clan_tag()
{
	const auto setclantag = []( const char *tag )
	{
		static auto set_clantag_fn = reinterpret_cast< int( __fastcall* )( const char*, const char* ) >( sig( "engine.dll", "53 56 57 8B DA 8B F9 FF 15" ) );

		set_clantag_fn( tag, tag );
	};

	static auto was_on = false;

	if ( !vars::misc.clantag.get<bool>() )
	{
		if ( was_on )
			setclantag( "" );
		was_on = false;
		return;
	}

	was_on = true;

	static int prevtime;
	static int prevval;

	if ( int( g_pGlobals->curtime * 3.1 ) != prevtime )
	{
		if ( prevval != int( g_pGlobals->curtime * 3.1 ) % 31 )
		{
			prevval = int( g_pGlobals->curtime * 3.1 ) % 31;
			switch ( int( g_pGlobals->curtime * 3.1 ) % 31 )
			{
				case 30: { _( s30, "fatality.win   " ); setclantag( s30 ); break; }
				case 24: { _( s24, " fatality.win  " ); setclantag( s24 ); break; }
				case 23: { _( s23, "  fatality.win " ); setclantag( s23 ); break; }
				case 22: { _( s22, "   fatality.win" ); setclantag( s22 ); break; }
				case 21: { _( s21, "    fatality.wi" ); setclantag( s21 ); break; }
				case 20: { _( s20, "     fatality.w" ); setclantag( s20 ); break; }
				case 19: { _( s19, "      fatality." ); setclantag( s19 ); break; }
				case 18: { _( s18, "       fatality" ); setclantag( s18 ); break; }
				case 17: { _( s17, "        fatalit" ); setclantag( s17 ); break; }
				case 16: { _( s16, "         fatali" ); setclantag( s16 ); break; }
				case 15: { _( s15, "          fatal" ); setclantag( s15 ); break; }
				case 14: { _( s14, "           fata" ); setclantag( s14 ); break; }
				case 13: { _( s13, "            fat" ); setclantag( s13 ); break; }
				case 12: { _( s12, "             fa" ); setclantag( s12 ); break; }
				case 11: { _( s11, "              f" ); setclantag( s11 ); break; }
				case 10: { _( s10, "n              " ); setclantag( s10 ); break; }
				case 9: { _( s9, "in             " ); setclantag( s9 ); break; }
				case 8: { _( s8, "win            " ); setclantag( s8 ); break; }
				case 7: { _( s7, ".win           " ); setclantag( s7 ); break; }
				case 6: { _( s6, "y.win          " ); setclantag( s6 ); break; }
				case 5: { _( s5, "ty.win         " ); setclantag( s5 ); break; }
				case 4: { _( s4, "ity.win        " ); setclantag( s4 ); break; }
				case 3: { _( s3, "lity.win       " ); setclantag( s3 ); break; }
				case 2: { _( s2, "ality.win      " ); setclantag( s2 ); break; }
				case 1: { _( s1, "tality.win     " ); setclantag( s1 ); break; }
				case 0: { _( s0, "atality.win    " ); setclantag( s0 ); break; }
				default:;
			}
		}
	}

	prevtime = int( g_pGlobals->curtime );
}

void misc::play_freezetime_music()
{
	_( techno_overpass, "ambient/misc/techno_overpass.wav" );

	static float time;
	if ( *g_ppGameRules && ( *g_ppGameRules )->m_bFreezePeriod() )
	{
		if ( !time )
			g_pSurface->PlaySound( techno_overpass );

		time += g_pGlobals->frametime;

		/*if ( time >= 6.f )
			time = 0.f;*/
	}
	else
		time = 0.f;
}
