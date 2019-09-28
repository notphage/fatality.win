#include "../include_cheat.h"
#include <menu/external_include.h>

std::string hitgroup_to_name( const int hitgroup )
{
	switch ( hitgroup )
	{
		case HITGROUP_HEAD:
			return _w( "HEAD" );
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			return _w( "LEG" );
		case HITGROUP_STOMACH:
			return _w( "STOMACH" );
		default:
			return _w( "BODY" );;
	}
}

void console_log::buy_event( IGameEvent * game_event )
{
	_( team, "team" );
	_( userid, "userid" );
	_( weapon_s, "weapon" );
	_( weapon_, "weapon_" );
	_( item_, "item_" );
	_( PURCHASE, "[ PURCHASE ] " );
	_( s, "%s " );
	_( bought, "bought " );
	_( sn, "%s\n" );

	if ( !g_pLocalPlayer || g_pLocalPlayer->get_team() == game_event->GetInt( team ) )
		return;

	const auto index = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid ) );

	player_info_t pinfo;
	g_pEngine->GetPlayerInfo( index, &pinfo );

	std::string weapon( game_event->GetString( weapon_s ) );
	if ( weapon.find( weapon_ ) != std::string::npos )
		weapon.replace( 0, 7, "" );
	else if ( weapon.find( item_ ) != std::string::npos )
		weapon.replace( 0, 5, "" );

	g_pCVar->ConsoleColorPrintf( Color( 200, 255, 0, 255 ), PURCHASE );
	util::print_dev_console( true, Color( 255, 229, 204, 255 ), s, pinfo.name );
	util::print_dev_console( true, Color( 255, 255, 255, 255 ), bought );
	util::print_dev_console( true, Color( 173, 216, 230, 255 ), sn, weapon.c_str() );
}

void console_log::hurt_event( IGameEvent * game_event )
{
	_( attacker_s, "attacker" );
	_( userid, "userid" );

	const auto attacker = g_pEngine->GetPlayerForUserID( game_event->GetInt( attacker_s ) );
	const auto victim = g_pEngine->GetPlayerForUserID( game_event->GetInt( userid ) );
	if ( attacker != g_pEngine->GetLocalPlayer() || victim == g_pEngine->GetLocalPlayer() )
		return;

	_( dmg_health, "dmg_health" );

	auto damage = game_event->GetInt( dmg_health );
	damage = std::clamp( damage, 0, 100 );
	const int green = 255 - damage * 2.55;
	const int red = damage * 2.55;

	player_info_t pinfo;
	g_pEngine->GetPlayerInfo( victim, &pinfo );

	_( DAMAGE, "[ DAMAGE ] " );
	_( d, "-%d " );
	_( in, "in " );
	_( s, "%s " );
	_( hitgroup, "hitgroup" );
	_( to, "to " );
	_( sn, "%s\n" );
	_u( spot, hitgroup_to_name( game_event->GetInt( hitgroup ) ) );

	g_pCVar->ConsoleColorPrintf( Color( 205, 92, 92, 255 ), DAMAGE );
	util::print_dev_console( true, Color( red, green, 0, 255 ), d, game_event->GetInt( dmg_health ) );
	util::print_dev_console( true, Color( 255, 255, 255, 255 ), in );
	util::print_dev_console( true, Color( 255, 204, 204, 255 ), s, spot );
	util::print_dev_console( true, Color( 255, 255, 255, 255 ), to );
	util::print_dev_console( true, Color( 255, 229, 204, 255 ), sn, pinfo.name );
}

void console_log::setup()
{
	if ( m_setup )
		return;

	_( developer_s, "developer" );
	_( con_filter_text_s, "con_filter_text" );
	_( con_filter_text_out_s, "con_filter_text_out" );
	_( con_filter_enable_s, "con_filter_enable" );
	_( contimes_s, "contimes" );

	static auto developer = g_pCVar->FindVar( developer_s );
	developer->set_value( 1 );
	static auto con_filter_text = g_pCVar->FindVar( con_filter_text_s );
	static auto con_filter_text_out = g_pCVar->FindVar( con_filter_text_out_s );
	static auto con_filter_enable = g_pCVar->FindVar( con_filter_enable_s );
	static auto contimes = g_pCVar->FindVar( contimes_s );

	_( L, "L " );
	_( s, " " );
	_( clear, "clear" );

	contimes->set_value( 15 );
	con_filter_text->set_value( L );
	con_filter_text_out->set_value( s );
	con_filter_enable->set_value( 2 );
	g_pEngine->ClientCmd_Unrestricted( clear );

	m_setup = true;
}

void console_log::listener( IGameEvent * game_event )
{
	setup();

	_( player_hurt, "player_hurt" );
	_( item_purchase, "item_purchase" );

	if ( vars::misc.hurtlog.get<bool>() && !strcmp( game_event->GetName(), player_hurt ) )
		hurt_event( game_event );
	else if ( vars::misc.buylog.get<bool>() && !strcmp( game_event->GetName(), item_purchase ) )
		buy_event( game_event );
}
