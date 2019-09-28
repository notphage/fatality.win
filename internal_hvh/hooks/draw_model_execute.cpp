#include "../include_cheat.h"

void __stdcall hooks::draw_model_execute( IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld )
{
	if ( g_unload || g_in_gl )
		return orig_draw_model_execute( g_pModelRender, context, state, info, pCustomBoneToWorld );

	chams::get().draw_record( context, state, info, pCustomBoneToWorld );

	g_pModelRender->ForcedMaterialOverride();
}