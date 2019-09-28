#include "../external_include.h"

void c_control::update_pos( LONG xdrag, LONG ydrag )
{
	m_bounds.x += xdrag;
	m_bounds.y += ydrag;
}

int c_control::get_size()
{
	return 0;
}