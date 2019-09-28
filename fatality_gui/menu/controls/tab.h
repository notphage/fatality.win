#pragma once

class c_tab : public singleton<c_tab>
{
public:
	c_tab( std::string name, int id, c_window* window );
	void render( bool selected );
	bool update();

	bool is_selected();

	c_subtab* add_subtab( std::string name );
	void pushback_subtab( c_subtab* tab );

	std::string m_name;

	std::vector<c_subtab*> m_subtabs;
private:
	int m_tabamount;

	c_subtab * m_selected_subtab;

	c_window * parent_window;
	int m_id;
	bool* m_selected;
	int add = 80;
};
