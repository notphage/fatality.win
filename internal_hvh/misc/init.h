#pragma once

namespace init
{
	void on_startup();
	void get_offsets();
	void get_interfaces();
	void get_materials();
	void hooks();
	void reset_hooks();

	extern HWND window;
}