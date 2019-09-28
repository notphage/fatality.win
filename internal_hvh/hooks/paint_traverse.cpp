#include "../include_cheat.h"

void __fastcall hooks::paint_traverse( void* pPanels, int edx, unsigned int panel, bool forceRepaint, bool allowForce )
{
	if ( g_unload )
		return orig_paint_traverse( pPanels, panel, forceRepaint, allowForce );

	static unsigned int scope_panel;
	if ( !scope_panel )
	{
		const auto panelname = g_pPanel->GetName( panel );

		_( HudZoom, "HudZoom" );
			if ( !strcmp( panelname, HudZoom ) )
			scope_panel = panel;
	}

	if ( panel == scope_panel && vars::misc.remove_scope.get<bool>() )
		return;

	g_vmatrix = g_pEngine->WorldToScreenMatrix();

	orig_paint_traverse( pPanels, panel, forceRepaint, allowForce );

	static unsigned int draw_panel;
	if ( !draw_panel )
	{
		const auto panelname = g_pPanel->GetName( panel );

		_( MatSystemTopPanel, "MatSystemTopPanel" );
		if ( !strcmp( panelname, MatSystemTopPanel ) )
			draw_panel = panel;
	}

	if ( panel != draw_panel )
		return;

	visuals::get().run();
}