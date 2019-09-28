#include "../include_cheat.h"

std::wstring UTF8toUnicode( const std::string& s )
{
	std::wstring ws;
	wchar_t wc;
	for ( int i = 0; i < s.length(); )
	{
		char c = s[ i ];
		if ( ( c & 0x80 ) == 0 )
		{
			wc = c;
			++i;
		}
		else if ( ( c & 0xE0 ) == 0xC0 )
		{
			wc = ( s[ i ] & 0x1F ) << 6;
			wc |= ( s[ i + 1 ] & 0x3F );
			i += 2;
		}
		else if ( ( c & 0xF0 ) == 0xE0 )
		{
			wc = ( s[ i ] & 0xF ) << 12;
			wc |= ( s[ i + 1 ] & 0x3F ) << 6;
			wc |= ( s[ i + 2 ] & 0x3F );
			i += 3;
		}
		else if ( ( c & 0xF8 ) == 0xF0 )
		{
			wc = ( s[ i ] & 0x7 ) << 18;
			wc |= ( s[ i + 1 ] & 0x3F ) << 12;
			wc |= ( s[ i + 2 ] & 0x3F ) << 6;
			wc |= ( s[ i + 3 ] & 0x3F );
			i += 4;
		}
		else if ( ( c & 0xFC ) == 0xF8 )
		{
			wc = ( s[ i ] & 0x3 ) << 24;
			wc |= ( s[ i ] & 0x3F ) << 18;
			wc |= ( s[ i ] & 0x3F ) << 12;
			wc |= ( s[ i ] & 0x3F ) << 6;
			wc |= ( s[ i ] & 0x3F );
			i += 5;
		}
		else if ( ( c & 0xFE ) == 0xFC )
		{
			wc = ( s[ i ] & 0x1 ) << 30;
			wc |= ( s[ i ] & 0x3F ) << 24;
			wc |= ( s[ i ] & 0x3F ) << 18;
			wc |= ( s[ i ] & 0x3F ) << 12;
			wc |= ( s[ i ] & 0x3F ) << 6;
			wc |= ( s[ i ] & 0x3F );
			i += 6;
		}
		ws += wc;
	}
	return ws;
}

char* __fastcall hooks::process_input( void* ecx, void* edx )
{
	static auto spawntime = 0.f;
	static auto status = false;

	auto set = false;
	if ( spawntime != g_pLocalPlayer->get_spawn_time() || status != vars::misc.preservedn.get<bool>() )
	{
		set = true;
		status = vars::misc.preservedn.get<bool>();
		spawntime = g_pLocalPlayer->get_spawn_time();
	}

	for ( auto& current : g_pDeathNotice->m_nDeathNotices )
	{
		if ( !g_pLocalPlayer->get_alive() || set )
		{
			if ( current.set != 1.f && !set )
				continue;

			current.m_flStartTime = g_pGlobals->curtime;
			current.m_flStartTime -= !g_pLocalPlayer->get_alive() ? 2.f : 7.5f;
			current.set = 2.f;

			continue;
		}

		if ( current.set == 2.f )
			continue;

		if ( !status )
		{
			current.set = 1.f;
			return orig_process_input( ecx );
		}

		if ( current.set == 1.f )
			continue;

		if ( current.m_flLifeTimeModifier == 1.5f )
		{
			current.m_flStartTime = 9999999.f;
		}


		current.set = 1.f;
	}

	return orig_process_input( ecx );
}