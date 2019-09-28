#pragma once

class c_subtab;
struct bounds;
class c_groupwindow;
class c_button : public singleton<c_button>, public c_control
{
public:
	c_button( std::string name, bounds bounds, uintptr_t function, c_subtab* parent );

	void render();
	void update();
	void update_pos_individual( LONG xdrag, LONG ydrag );
private:
	float hover_progress;

	c_subtab* m_parent;

	bool m_hovered;

	uintptr_t m_function = 0;

	float m_frequence = 1.f / .1f;
};
