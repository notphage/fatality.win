#pragma once

class Color;
class C_CSPlayerResource
{
public:
	OFFSETRS( get_ping, int, 0x0AE4 )
		OFFSET( get_c4_player, int, 0x161C )
};

extern C_CSPlayerResource** g_ppPlayerResource;