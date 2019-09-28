#pragma once

class c_subtab;
class c_slider : public singleton<c_slider>, public c_control
{
public:
	c_slider( std::string name, bounds bounds, std::string option, float min, float max, c_subtab* parent, std::string format = "%.2f" );
	c_slider( std::string name, bounds bounds, std::string option, int min, int max, c_subtab* parent, std::string format = "" );

	void render();
	void update();
	void update_pos_individual( LONG xdrag, LONG ydrag );
private:
	float round_value();

	c_subtab * m_parent;

	float hover_progress;

	bool m_hovered;

	std::string m_format;

	float m_min;
	float m_max;

	bool m_roundvalue;

	std::string m_value;
	bool m_active;
	bool m_wasactive;
	bool m_cantuse;
};