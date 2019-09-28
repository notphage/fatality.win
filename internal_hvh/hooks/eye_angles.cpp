#include "../include_cheat.h"

QAngle a = { 0.f,0.f,0.f };

QAngle* _fastcall hooks::eye_angles( void* ecx, void* edx )
{
	if ( g_pLocalPlayer && ecx == g_pLocalPlayer && g_pInput->m_fCameraInThirdPerson )
		return &thirdperson::get().get_tp_angle();

	return orig_eye_angles( ecx );
}

void __fastcall hooks::maintain_sequence_transitions( void * ecx, void * edx, void* bonesetup, float a3, Vector * pos, Quaternion * q )
{
	return;
}

void __fastcall hooks::standard_blending_rules( void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion *q, float curTime, int boneMask )
{
	auto player = static_cast< C_CSPlayer* >( ecx );
	player->get_effects() |= EF_NOINTERP;
	orig_standard_blending_rules( ecx, hdr, pos, q, curTime, boneMask );
	player->get_effects() &= ~EF_NOINTERP;
}

void __fastcall hooks::fire_event( void* ecx, void* edx )
{
	if ( !g_pLocalPlayer || !g_pLocalPlayer->get_alive() )
		return orig_fire_event( ecx );

	auto ei = g_pClientState->m_Events; //0x4DEC
	CEventInfo *next = nullptr;

	if ( !ei )
		return orig_fire_event( ecx );

	do
	{
		next = *reinterpret_cast< CEventInfo** >( reinterpret_cast< uintptr_t >( ei ) + 0x38 );

		const uint16_t class_id = ei->classID - 1;

		const auto m_p_create_event_fn = ei->pClientClass->m_pCreateEventFn;
		if ( !m_p_create_event_fn )
			continue;

		const auto p_ce = m_p_create_event_fn();
		if ( !p_ce )
			continue;

		ei->fire_delay = 0.f;

		ei = next;
	} while ( next != nullptr );

	return orig_fire_event( ecx );
}

enum PlayerAnimEvent_t
{
	PLAYERANIMEVENT_FIRE_GUN_PRIMARY = 0,
	PLAYERANIMEVENT_FIRE_GUN_SECONDARY,
	PLAYERANIMEVENT_THROW_GRENADE,
	PLAYERANIMEVENT_JUMP,
	PLAYERANIMEVENT_RELOAD,

	PLAYERANIMEVENT_COUNT
};

void __fastcall hooks::do_animation_event( void* ecx, void* edx, unsigned int event, int data )
{
	if ( event == PLAYERANIMEVENT_FIRE_GUN_PRIMARY || event == PLAYERANIMEVENT_FIRE_GUN_SECONDARY )
		return;

	orig_do_animation_event( ecx, event, data );
}