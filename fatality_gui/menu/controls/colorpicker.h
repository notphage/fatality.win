#pragma once

class c_subtab;
class c_colorpicker : public singleton<c_colorpicker>, public c_control
{
public:
	c_colorpicker( std::string name, bounds bounds, std::string option, D3DCOLOR default_color, c_subtab* parent, bool alpha_slider = false );

	void render();
	void update();
	void update_pos_individual( LONG xdrag, LONG ydrag );
private:
	c_subtab * m_parent;

	float m_colHSV[ 3 ];

	float hover_progress;
	float anim_progress;

	bool m_hovered;

	bounds m_picker_bounds;
	bounds m_box_bounds;
	bounds m_hue_bounds;
	bounds m_alpha_bounds;

	POINT m_alpha_mouse_pos;
	POINT m_hue_mouse_pos;
	POINT m_picker_mouse_pos;

	std::string m_value;
	bool m_active;
	bool m_showalpha;

	int m_curfocus; //0-none 1-main 2-hue 3-alpha

	float m_frequence = 1.f / .1f;

	POINT m_pickerpos;
};

