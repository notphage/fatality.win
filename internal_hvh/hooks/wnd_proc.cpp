#include "../include_cheat.h"

LRESULT __stdcall  hooks::wnd_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( g_unload )
		return CallWindowProc( orig_wnd_proc, hWnd, uMsg, wParam, lParam );

	if ( render::get().get_init() && c_menu::get().get_opened() )
	{
		return true;
	}

	return CallWindowProc( orig_wnd_proc, hWnd, uMsg, wParam, lParam );
}