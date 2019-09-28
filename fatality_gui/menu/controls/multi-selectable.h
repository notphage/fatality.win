#pragma once

class c_subtab;
class c_multi : public singleton<c_multi>, public c_control
{
public:
	c_multi(std::string name, bounds bounds, std::vector<std::string>* first_items, std::vector<std::string>* second_items, c_subtab* parent);

	void render();
	void update();
	void update_pos_individual( LONG xdrag, LONG ydrag );
private:
	std::string get_preview();

	c_subtab * m_parent;

	std::vector<std::string> first_items;
	std::vector<std::string> second_items;

	float hover_progress;
	float anim_progress;

	bool m_hovered;

	bool m_active;

	float m_frequence = 1.f / .1f;
};

