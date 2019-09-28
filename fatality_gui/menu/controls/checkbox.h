#pragma once

class c_groupwindow;
class c_checkbox : public singleton<c_checkbox>, public c_control
{
public:
	c_checkbox( std::string name, bounds bounds, std::string option, c_subtab* parent );

	void render();
	void update();
	void update_pos_individual( LONG xdrag, LONG ydrag );
private:
	float anim_progress;
	float hover_progress;
	float checkmark_progress;

	c_subtab* m_parent;

	bool m_hovered;
	std::string m_value;

	float m_frequence = 1.f / .1f;
};
