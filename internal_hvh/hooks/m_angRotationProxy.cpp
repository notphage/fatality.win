#include "../include_cheat.h"

void __cdecl hooks::m_angRotationProxy( CRecvProxyData *pData, void* entity, void* output )
{
	return orig_m_angRotationProxy( pData, entity, output );
}