#pragma once

class c_tab;
class c_groupwindow;
class c_subtab;
class c_window : public singleton<c_window>, public c_menu
{
public:
	c_window( std::string name, bounds bounds, bool tabs );
	void set_active( bool active );
	void render();
	void update_controls( LONG xdrag, LONG ydrag );
	void handle_movement();

	c_tab* add_tab( std::string name );
	void pushback_tab( c_tab* tab );

	c_groupwindow* add_groupwindow( std::string name, bounds bounds );
	void pushback_groupwindow( c_groupwindow* window );

	std::vector<c_tab*> m_tabs;
	std::vector<c_groupwindow*> m_groups;

	bounds m_area;
	bounds m_tabarea;
	bounds m_subtabarea;

	c_groupwindow* focussed_window;

	LPDIRECT3DTEXTURE9 img;
	LPD3DXSPRITE sprite;
	LPDIRECT3DTEXTURE9 img_big;
	LPD3DXSPRITE sprite_big;
private:
	std::string m_name;

	bounds m_bounds;

	POINT m_drag_offset;
	bool m_is_dragged;

	c_tab* m_selected_tab;
	int m_tabamount;

	bool m_hastabs;
	bool m_active;
};
