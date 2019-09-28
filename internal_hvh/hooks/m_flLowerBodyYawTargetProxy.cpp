#include "../include_cheat.h"

struct stack_frame
{
	stack_frame* next;
	DWORD ret;
};

__forceinline DWORD get_ret_addr( int depth = 0 )
{
	stack_frame* fp;

	_asm mov fp, ebp;

	for ( int i = 0; i < depth; i++ )
	{
		if ( !fp )
			break;

		fp = fp->next;
	}

	return fp ? fp->ret : 0;
}

void __cdecl hooks::m_flLowerBodyYawTargetProxy( CRecvProxyData *pData, void* entity, void* output )
{
	const auto player = reinterpret_cast< C_CSPlayer* >( entity );

	static DWORD fnCopyNewEntity = sig( "engine.dll", "EB 3F FF 77 10" );

	// skip "unwanted" lowerbody updates originating from CopyNewEntity.
	// happens on entity creation + pvs re-enter.

	if ( fnCopyNewEntity != get_ret_addr( 2 ) )
		resolver::on_lby_proxy( player, &pData->m_Value.m_Float );

	return orig_m_flLowerBodyYawTargetProxy( pData, entity, output );
}