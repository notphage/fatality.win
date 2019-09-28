#include "../include_cheat.h"

void __cdecl hooks::m_angEyeAngles1Proxy( CRecvProxyData *pData, void* entity, void* output )
{
	const auto player = reinterpret_cast< C_CSPlayer* >( entity );

	resolver::resolve_proxy( player, &pData->m_Value.m_Float );

	return orig_m_angEyeAngles1Proxy( pData, entity, output );
}