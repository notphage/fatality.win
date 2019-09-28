#include "../include_cheat.h"

void __cdecl hooks::m_angEyeAngles0Proxy( CRecvProxyData *pData, void* entity, void* output )
{
	const auto player = reinterpret_cast< C_CSPlayer* >( entity );
	const auto PlayerRecord = &player_log::get().get_log( player->EntIndex() );

	PlayerRecord->m_flProxyPitch = pData->m_Value.m_Float;
	if ( PlayerRecord->m_bCustomCorrection )
		pData->m_Value.m_Float = PlayerRecord->m_flPitch;

	return orig_m_angEyeAngles0Proxy( pData, entity, output );
}