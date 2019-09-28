#include "../include_cheat.h"

void __fastcall hooks::trace_ray( void *thisptr, void*, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace )
{
	if ( !g_in_hbp || g_unload )
		return orig_trace_ray( thisptr, ray, fMask, pTraceFilter, pTrace );

	orig_trace_ray( thisptr, ray, fMask, pTraceFilter, pTrace );

	pTrace->surface.flags |= SURF_SKY;
}
