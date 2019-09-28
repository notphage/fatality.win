#include "../include_cheat.h"

void glow::draw()
{
	if ( !vars::visuals.glow.enabled.get<bool>() )
		return;

	g_pGlowObjectManager->RenderGlowEffects();
}
