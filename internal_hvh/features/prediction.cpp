#include "../include_cheat.h"
#include "../sdk/checksum_md5.h"

void prediction::start()
{
	static auto oldorigin = g_pLocalPlayer->get_origin();
	unpred_vel = ( g_pLocalPlayer->get_origin() - oldorigin ) * ( 1.0 / g_pGlobals->interval_per_tick );
	oldorigin = g_pLocalPlayer->get_origin();

	unpred_eyepos = g_pLocalPlayer->get_eye_pos();

	m_prediction_random_seed = *reinterpret_cast< int** > ( offsets::pred_random_seed );

	*m_prediction_random_seed = MD5_PseudoRandom( g_cmd->command_number ) & 0x7FFFFFFF;

	orig_currtime = g_pGlobals->curtime;
	orig_frametime = g_pGlobals->frametime;

	g_pGlobals->curtime = get_curtime();
	g_pGlobals->frametime = g_pGlobals->interval_per_tick;

	g_pGameMovement->StartTrackPredictionErrors( g_pLocalPlayer );

	memset( &move_data, 0, sizeof( move_data ) );
	g_pMoveHelper->SetHost( g_pLocalPlayer );
	g_pPrediction->SetupMove( g_pLocalPlayer, g_cmd, g_pMoveHelper, &move_data );
	g_pGameMovement->ProcessMovement( g_pLocalPlayer, &move_data );
	g_pPrediction->FinishMove( g_pLocalPlayer, g_cmd, &move_data );

	static auto pred_oldorigin = g_pLocalPlayer->get_origin();
	pred_vel = ( g_pLocalPlayer->get_origin() - pred_oldorigin ) * ( 1.0 / g_pGlobals->interval_per_tick );
	pred_oldorigin = g_pLocalPlayer->get_origin();

	unpred_eyepos = g_pLocalPlayer->get_eye_pos();
}

void prediction::finish() const
{
	g_pGameMovement->FinishTrackPredictionErrors( g_pLocalPlayer );
	g_pMoveHelper->SetHost( nullptr );

	*m_prediction_random_seed = -1;

	g_pGlobals->curtime = orig_currtime;
	g_pGlobals->frametime = orig_frametime;
}

int prediction::get_tickbase()
{
	return g_pLocalPlayer->get_tickbase();
}

float prediction::get_curtime()
{
	return get_tickbase() * g_pGlobals->interval_per_tick;
}

float prediction::get_unpred_curtime() const
{
	return orig_currtime;
}

Vector prediction::get_unpred_vel() const
{
	return unpred_vel;
}

Vector prediction::get_pred_vel() const
{
	return pred_vel;
}

Vector prediction::get_unpred_eyepos() const
{
	return unpred_eyepos;
}
