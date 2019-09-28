#include "../include_cheat.h"

long __stdcall hooks::reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp )
{
	render::get().invalidate_device_objects();
	c_menu::get().release_images();

	if ( g_unload )
	{
		return orig_reset( device, pp );
	}

	c_menu::get().get_opened() = false;

	const auto hr = orig_reset( device, pp );

	render::get().init_device_objects( device );

	return hr;
}
