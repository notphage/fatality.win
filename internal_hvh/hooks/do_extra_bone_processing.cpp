#include "../include_cheat.h"

void _fastcall hooks::do_extra_bone_processing( void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, byte* boneComputed, CIKContext* context )
{
	if ( g_pLocalPlayer && ecx == g_pLocalPlayer )
		return;

	const auto player = reinterpret_cast< C_CSPlayer* >( ecx );
	if ( player )
		return;

	orig_do_extra_bone_processing( ecx, hdr, pos, q, matrix, boneComputed, context );
}