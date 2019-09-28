#pragma once

class c_subtab : public singleton<c_subtab>
{
public:
	c_subtab( std::string name, int id, c_window* window, c_tab* tab );
	void render( bool active );
	bool update();

	void add_checkbox( std::string name, std::string option, bool sameline = false );
	void add_combobox( std::string name, std::string option, std::vector<std::string>* items, bool sameline = false );
	void add_slider( std::string name, std::string option, float min, float max, std::string format = "%.2f", bool sameline = false );
	void add_slider( std::string name, std::string option, int min, int max, std::string format = "", bool sameline = false );
	void add_button( std::string name, void* function, bool sameline = false );
	void add_keybind( std::string name, std::string option, bool sameline = false );
	void add_colorpicker( std::string name, std::string option, D3DCOLOR default_color, bool sameline = false, bool show_alpha = false );
	void add_multi( std::string name, std::vector<std::string>* first_items, std::vector<std::string>* second_items, bool sameline = false );
	void add_listbox( std::string name, std::vector<std::string>& items, std::string option, float width, float height, bool sameline = false );
	void add_listbox( std::string name, std::vector<std::string>& items, std::string option, float width, float height, std::string& filter, bool sameline = false );
	void add_textbox( std::string name, std::string& option, bool sameline = false );

	std::vector<c_control*> m_controls;

	c_control* focussed_control;
private:
	c_tab * parent_tab;
	c_window * parent_window;

	int m_id;
	std::string m_name;
	bool* m_selected;

	int m_current_ypos;
	int last_xpos;
	int last_ysize;
};
