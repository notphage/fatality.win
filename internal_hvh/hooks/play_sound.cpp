#include "../include_cheat.h"

void __stdcall hooks::play_sound( const char *folderIme )
{
	_( competitive_accept_beep, "!UI/competitive_accept_beep.wav" );

	if ( strcmp( folderIme, competitive_accept_beep ) )
		return orig_play_sound( g_pSurface, folderIme );

	if ( !vars::misc.autoaccept.get<bool>() || g_pEngine->IsInGame() )
		return orig_play_sound( g_pSurface, folderIme );

	static auto is_ready = reinterpret_cast< void( *)( ) >( sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE" ) );
	is_ready();

	_( light_power_on_switch_01, "buttons/light_power_on_switch_01.wav" );
	g_pSurface->PlaySound( light_power_on_switch_01 );

	FLASHWINFO flash;
	flash.cbSize = sizeof( FLASHWINFO );
	flash.hwnd = init::window;
	flash.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	flash.uCount = 0;
	flash.dwTimeout = 0;
	FlashWindowEx( &flash );
}