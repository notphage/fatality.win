#include "../include_cheat.h"

void beams::draw_impacts()
{
	if ( !g_pEngine->IsInGame() || !g_pLocalPlayer )
	{
		impacts.clear();
		return;
	}

	if ( impacts.size() > 30 )
		impacts.pop_back();

	_( modelname, "sprites/purplelaser1.vmt" );

	for ( size_t i = 0; i < impacts.size(); i++ )
	{
		auto impact = impacts[ i ];

		if ( !impact.player || impact.player->IsDormant() )
			continue;

		BeamInfo_t beam_info;
		beam_info.m_vecStart = impact.start;
		if ( impact.player == g_pLocalPlayer )
			beam_info.m_vecStart.z -= 2.f;
		beam_info.m_vecEnd = impact.pos;
		beam_info.m_nType = TE_BEAMPOINTS;
		beam_info.m_pszModelName = modelname;
		beam_info.m_nModelIndex = -1;
		beam_info.m_flHaloScale = 0.0f;
		beam_info.m_flLife = 4.f;
		beam_info.m_flWidth = 2.0f;
		beam_info.m_flEndWidth = 2.0f;
		beam_info.m_flFadeLength = 0.0f;
		beam_info.m_flAmplitude = 2.0f;
		beam_info.m_flBrightness = 255.f;
		beam_info.m_flSpeed = 0.2f;
		beam_info.m_nStartFrame = 0;
		beam_info.m_flFrameRate = 0.f;
		beam_info.m_flRed = float( impact.color.r() );
		beam_info.m_flGreen = float( impact.color.g() );
		beam_info.m_flBlue = float( impact.color.b() );
		beam_info.m_nSegments = 2;
		beam_info.m_bRenderable = true;
		beam_info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

		const auto beam = g_pBeams->CreateBeamPoints( beam_info );
		if ( beam )
			g_pBeams->DrawBeam( beam );
	}

	impacts.clear();
}

void beams::listener( IGameEvent * game_event )
{
	_( uid, "userid" );

	const auto userid = g_pEngine->GetPlayerForUserID( game_event->GetInt( uid ) );

	static float last_curtime;

	const auto player = get_entity( userid );
	if ( !player )
		return;

	if ( player->IsDormant() )
		return;

	if ( player == g_pLocalPlayer )
		return;
	if ( !vars::visuals.beams.enemy.enabled.get<bool>() && player->is_enemy() )
		return;
	if ( !vars::visuals.beams.team.enabled.get<bool>() && !player->is_enemy() && player != g_pLocalPlayer )
		return;

	_( x, "x" );
	_( y, "y" );
	_( z, "z" );

	const Vector pos( game_event->GetFloat( x ), game_event->GetFloat( y ), game_event->GetFloat( z ) );
	Color col;

	/*if ( player == g_pLocalPlayer )
		col = get_col( vars::visuals.beams.local.color.get<uintptr_t>() );*/
	if ( player->is_enemy() )
		col = get_col( vars::visuals.beams.enemy.color.get<uintptr_t>() );
	else if ( !player->is_enemy() )
		col = get_col( vars::visuals.beams.team.color.get<uintptr_t>() );

	impact_info_t impact( g_pGlobals->curtime, player->get_eye_pos(), pos, player, col );

	if ( last_curtime == g_pGlobals->curtime )
	{
		if ( !impacts.empty() && impact.length() < ( impacts.end() - 1 )->length() || impacts.empty() )
			return;

		impacts.pop_back();
	}

	last_curtime = g_pGlobals->curtime;

	impacts.push_back( impact );
}

void beams::add_local_beam( const impact_info_t info )
{
	if ( !vars::visuals.beams.local.enabled.get<bool>() )
		return;

	impacts.push_back( info );
}

bool beams::beam_exists( C_CSPlayer* player, const float curtime )
{
	for ( auto& impact : impacts )
	{
		if ( impact.time == curtime && player == impact.player )
			return true;
	}

	return false;
}
