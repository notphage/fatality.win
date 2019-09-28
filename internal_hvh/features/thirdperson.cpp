#include "../include_cheat.h"

void thirdperson::run() const
{
	static bool prev_state;
	const bool state = GetAsyncKeyState( vars::key.thirdperson.get<int>() );
	if ( prev_state != state )
	{
		if ( state )
			vars::misc.thirdperson.set( !vars::misc.thirdperson.get<bool>() );

		prev_state = state;
	}

	if ( !g_pEngine->IsInGame() || !g_pLocalPlayer )
		return;

	static QAngle view_angles;
	g_pEngine->GetViewAngles( view_angles );

	if ( g_pLocalPlayer->get_alive() && vars::misc.thirdperson.get<bool>() )
	{
		//g_pLocalPlayer->get_effects() &= ~EF_NOINTERP;

		auto &animstate = **reinterpret_cast< CCSGOPlayerAnimState** >( uintptr_t( static_cast< C_CSPlayer* >( g_pLocalPlayer ) ) + 0x3874 );

		static auto was_inlandanim = false;

		g_pInput->m_fCameraInThirdPerson = true;
		g_pInput->m_vecCameraOffset = Vector( view_angles.x, view_angles.y, vars::misc.thirdperson_dist.get<float>() );

		const auto cam_hull_offset = 12.f + ( vars::misc.fov.get<float>() / 4.8f - 18.f );
		const Vector cam_hull_extents( cam_hull_offset, cam_hull_offset, cam_hull_offset );
		Vector cam_forward;
		const auto origin = g_pLocalPlayer->get_eye_pos();

		math::get().angle_vectors( QAngle( g_pInput->m_vecCameraOffset.x, g_pInput->m_vecCameraOffset.y, 0.0f ), &cam_forward );

		trace_t trace;
		util_trace_hull( origin, origin - ( cam_forward * g_pInput->m_vecCameraOffset.z ), MASK_SOLID & ~CONTENTS_MONSTER, cam_hull_extents, &trace );

		g_pInput->m_vecCameraOffset.z *= trace.fraction;

		g_pLocalPlayer->get_tp_angle() = tp_angle;
		g_pPrediction->SetLocalViewAngles( tp_angle );
	}
	else if ( g_pInput->m_fCameraInThirdPerson )
	{
		g_pInput->m_fCameraInThirdPerson = false;
	}

	static auto b_once = false;

	if ( g_pLocalPlayer->get_alive() || !vars::misc.thirdperson_dead.get<bool>() )
	{
		b_once = false;
		return;
	}

	if ( b_once )
		g_pLocalPlayer->get_observer_mode() = 5;

	if ( g_pLocalPlayer->get_observer_mode() == 4 )
		b_once = true;
}

void thirdperson::set_tp_angle( const QAngle tp_angle ) const
{
	this->tp_angle = tp_angle;
}

void thirdperson::set_abs_angles( const QAngle abs_angles ) const
{
	this->abs_angles = abs_angles;
}
