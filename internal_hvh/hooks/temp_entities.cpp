#include "../include_cheat.h"

bool __fastcall hooks::temp_entities( void* ECX, void* EDX, void* msg )
{
	if ( !g_pLocalPlayer || !g_pEngine->IsInGame() )
		return orig_temp_entities( ECX, msg );

	const bool ret = orig_temp_entities( ECX, msg );

	// Filtering events
	if ( !g_pLocalPlayer->get_alive() )
		return ret;

	CEventInfo *ei = g_pClientState->m_Events; //0x4DEC
	CEventInfo *next = nullptr;

	if ( !ei )
		return ret;

	do
	{
		next = *reinterpret_cast< CEventInfo** >( reinterpret_cast< uintptr_t >( ei ) + 0x38 );

		uint16_t classID = ei->classID - 1;

		auto m_pCreateEventFn = ei->pClientClass->m_pCreateEventFn; // ei->pClientClass->m_pCreateEventFn ptr
		if ( !m_pCreateEventFn )
			continue;

		IClientNetworkable *pCE = m_pCreateEventFn();
		if ( !pCE )
			continue;

		if ( classID == 170 )
		{
			// set fire_delay to zero to send out event so its not here later.
			ei->fire_delay = 0.0f;
		}
		ei = next;
	} while ( next != nullptr );

	return ret;
}