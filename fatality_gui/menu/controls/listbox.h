#pragma once

class c_subtab;
class c_listbox : public singleton<c_listbox>, public c_control
{
public:
	c_listbox(std::string name, bounds bounds, std::string option, std::vector<std::string>& items, c_subtab* paren);
	c_listbox(std::string name, bounds bounds, std::string option, std::vector<std::string>& items, std::string& filter, c_subtab* paren);

	void render();
	void update();
	void update_pos_individual( LONG xdrag, LONG ydrag );
private:
	void handle_scrollbar( );

	void get_items( );
	bool should_show( std::string xd );
	std::vector<std::string> lower_items;

	void render_scroll(int x, int y, int h, float min, float max, void* value);
	c_subtab * m_parent;

	bounds m_scrollbounds;

	std::vector<std::string>* m_items;
	std::string* filter;
	std::string prevfilter = "123123123123";

	float hover_progress;
	float anim_progress;

	bool m_hovered;

	std::string m_value;
	bool m_active;

	int m_maxitems = 12;
	float m_cur = 0;

	bool m_scrolling;
	int m_scrollpos;

	std::vector< std::pair<std::string, int >> temp;

	float m_frequence = 1.f / .1f;
};

