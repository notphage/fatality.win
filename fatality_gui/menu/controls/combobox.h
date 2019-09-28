#pragma once

struct bounds;
class c_subtab;
class c_combobox : public singleton<c_combobox>, public c_control
{
public:
	c_combobox( std::string name, bounds bounds, std::string option, std::vector<std::string>* items, c_subtab* parent );

	void render();
	void update();
	void update_pos_individual( LONG xdrag, LONG ydrag );
private:
	c_subtab * m_parent;

	std::vector<std::string> m_items;

	float hover_progress;
	float anim_progress;

	bool m_hovered;

	std::string m_value;
	bool m_active;

	float m_frequence = 1.f / .1f;
};

