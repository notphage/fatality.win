#pragma once

class IVPanel
{
public:
	VFUNC( 36, GetName( int panel ), const char*( __thiscall* )( void*, int ) )( panel )
};

extern IVPanel* g_pPanel;