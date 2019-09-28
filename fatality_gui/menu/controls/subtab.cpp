#include "../external_include.h"

c_subtab::c_subtab( std::string name, int id, c_window* window, c_tab* tab )
{
	this->m_name = name;
	this->m_id = id;
	this->parent_window = window;
	this->parent_tab = tab;
}

void c_subtab::render( bool selected )
{
	bool hovered = c_menu::get().is_hovering( bounds( parent_window->m_subtabarea.x + 22, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id, parent_window->m_subtabarea.w - 44, 28 ) );

	_u( parent_name, parent_tab->m_name );
	render::get().text( Vector2D( parent_window->m_subtabarea.x + 20, parent_window->m_subtabarea.y + 12 ), parent_name, D3DCOLOR_RGBA( 240, 240, 240, 255 ), fonts::subtabfont, c_font::left_aligned );

	_u( name, m_name );
	if ( selected )
	{
		render::get().line( Vector2D( parent_window->m_subtabarea.x + 20, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id ),
							Vector2D( parent_window->m_subtabarea.x + 20,
									  parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id + 28 ), D3DCOLOR_RGBA( 20, 179, 178, 255 ) );
		render::get().line( Vector2D( parent_window->m_subtabarea.x + 21, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id ),
							Vector2D( parent_window->m_subtabarea.x + 21,
									  parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id + 28 ), D3DCOLOR_RGBA( 20, 179, 178, 255 ) );

				  //render::get().filled_box(parent_window->m_subtabarea.x + 22, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id, parent_window->m_subtabarea.w - 44, 28, D3DCOLOR_RGBA(20, 179, 178, 255));

		render::get().text( Vector2D( parent_window->m_subtabarea.x + 28, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id + 14 - ( render::get().get_text_size( name, fonts::subtabfont ).y / 2 ) ),
							name, Color( 20, 179, 178, 255 ), fonts::subtabfont, c_font::left_aligned );

		for ( auto control : m_controls )
		{
			if ( control == focussed_control )
			{
				control->update();
				break;
			}

			control->update();
		}

		for ( auto control : m_controls )
		{
			if ( control == focussed_control ) //prevent overlapping of comboboxes/colorpickers etc.
				continue;

			control->render();
		}

		if ( focussed_control )
			focussed_control->render();
		/*render::get().text(m_name, parent_window->m_tabarea.x + (parent_window->m_tabarea.w / 2),
		parent_window->m_tabarea.y + 32 + 45 * m_id + 8 * m_id + 22 - (render::get().string_height(fonts::tabfontthicc, m_name) / 2),
		CENTERED, fonts::tabfontthicc, false, D3DCOLOR_RGBA(215, 240, 250, 255));*/
		//render::get().filled_box(parent_window->m_tabarea.x + 1, parent_window->m_tabarea.y + 32 + 45 * m_id + 8 * m_id, parent_window->m_tabarea.w - 1, 45, D3DCOLOR_RGBA(255, 255, 255, 255));
	}
	else
	{
		render::get().text( Vector2D( parent_window->m_subtabarea.x + 28, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id + 14 - ( render::get().get_text_size( name, fonts::subtabfont ).y / 2 ) ),
							name, hovered ? D3DCOLOR_RGBA( 219, 218, 225, 255 ) : D3DCOLOR_RGBA( 130, 130, 140, 255 ), fonts::subtabfont, c_font::left_aligned );

		//render::get().filled_box(parent_window->m_subtabarea.x + 22, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id, parent_window->m_subtabarea.w - 44, 28,
		//	hovered ? D3DCOLOR_RGBA(81, 87, 120, 255) : D3DCOLOR_RGBA(0, 0, 0, 0));
		/*render::get().text(m_name, parent_window->m_tabarea.x + (parent_window->m_tabarea.w / 2),
		parent_window->m_tabarea.y + 32 + 45 * m_id + 8 * m_id + 22 - (render::get().string_height(fonts::tabfont, m_name) / 2),
		CENTERED, fonts::tabfont, false, D3DCOLOR_RGBA(210, 220, 205, 255));*/
	}
}

bool c_subtab::update()
{
	if ( c_menu::get().is_pressed( bounds( parent_window->m_subtabarea.x + 22, parent_window->m_subtabarea.y + 42 + 28 * m_id + 8 * m_id, parent_window->m_subtabarea.w - 44, 28 ) ) )
		return true;

	return false;
}

void c_subtab::add_checkbox( std::string name, std::string option, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	_u( realname, name );
	m_controls.push_back( new c_checkbox( name, bounds( xpos, ypos, CHECKBOX_HEIGHT + 4 + render::get().get_text_size( realname, fonts::controlfont ).x + 8, CHECKBOX_HEIGHT ), option, this ) );

	if ( !sameline )
	{
		m_current_ypos += CHECKBOX_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = CHECKBOX_HEIGHT + 8;
	last_xpos = xpos + CHECKBOX_HEIGHT + 4 + render::get().get_text_size( realname, fonts::controlfont ).x + 8;
}

void c_subtab::add_combobox( std::string name, std::string option, std::vector<std::string>* items, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	_u( realname, name );
	m_controls.push_back( new c_combobox( name, bounds( xpos, ypos, COMBOBOX_WIDTH + 12 + render::get().get_text_size( realname, fonts::controlfont ).x, COMBOBOX_HEIGHT ), option, items, this ) );

	if ( !sameline )
	{
		m_current_ypos += COMBOBOX_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = COMBOBOX_HEIGHT + 8;
	last_xpos = xpos + COMBOBOX_WIDTH + 12 + render::get().get_text_size( realname, fonts::controlfont ).x + 8;
}

void c_subtab::add_slider( std::string name, std::string option, float min, float max, std::string format, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	_u( realname, name );
	m_controls.push_back( new c_slider( name, bounds( xpos, ypos, SLIDER_WIDTH + 8 + render::get().get_text_size( realname, fonts::controlfont ).x + 40, COMBOBOX_HEIGHT ), option, min, max, this, format ) );

	if ( !sameline )
	{
		m_current_ypos += SLIDER_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = SLIDER_HEIGHT + 8;
	last_xpos = xpos + SLIDER_WIDTH + 8 + render::get().get_text_size( realname, fonts::controlfont ).x + 8 + 40;
}

void c_subtab::add_slider( std::string name, std::string option, int min, int max, std::string format, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	_u( realname, name );
	m_controls.push_back( new c_slider( name, bounds( xpos, ypos, SLIDER_WIDTH + 12 + render::get().get_text_size( realname, fonts::controlfont ).x + 40, COMBOBOX_HEIGHT ), option, min, max, this, format ) );

	if ( !sameline )
	{
		m_current_ypos += SLIDER_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = SLIDER_HEIGHT + 8;
	last_xpos = xpos + SLIDER_WIDTH + 12 + render::get().get_text_size( realname, fonts::controlfont ).x + 8 + 40;
}

void c_subtab::add_button( std::string name, void* function, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	m_controls.push_back( new c_button( name, bounds( xpos, ypos, BUTTON_WIDTH + 8, BUTTON_HEIGHT ), ( uintptr_t )function, this ) );

	if ( !sameline )
	{
		m_current_ypos += BUTTON_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = BUTTON_HEIGHT + 8;
	last_xpos = xpos + BUTTON_WIDTH + 8;
}

void c_subtab::add_keybind( std::string name, std::string option, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	m_controls.push_back( new c_keybind( name, bounds( xpos, ypos, KEYBIND_WIDTH + 8, KEYBIND_HEIGHT ), option, this ) );

	if ( !sameline )
	{
		m_current_ypos += KEYBIND_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = KEYBIND_HEIGHT + 8;
	last_xpos = xpos + KEYBIND_WIDTH + 8;
}

void c_subtab::add_colorpicker( std::string name, std::string option, D3DCOLOR default_color, bool sameline, bool show_alpha )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	m_controls.push_back( new c_colorpicker( name, bounds( xpos, ypos, COLORPICKER_WIDTH + 8, COLORPICKER_HEIGHT ), option, default_color, this, show_alpha ) );

	if ( !sameline )
	{
		m_current_ypos += COLORPICKER_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = COLORPICKER_HEIGHT + 8;
	last_xpos = xpos + COLORPICKER_WIDTH + 8;
}

void c_subtab::add_multi( std::string name, std::vector<std::string>* first_items, std::vector<std::string>* second_items, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	_u( realname, name );
	m_controls.push_back( new c_multi( name, bounds( xpos, ypos, COMBOBOX_WIDTH + 12 + render::get().get_text_size( realname, fonts::controlfont ).x, COMBOBOX_HEIGHT ), first_items, second_items, this ) );

	if ( !sameline )
	{
		m_current_ypos += COMBOBOX_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = COMBOBOX_HEIGHT + 8;
	last_xpos = xpos + COMBOBOX_WIDTH + 12 + render::get().get_text_size( realname, fonts::controlfont ).x + 8;
}

void c_subtab::add_listbox( std::string name, std::vector<std::string>& items, std::string option, float width, float height, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	m_controls.push_back( new c_listbox( name, bounds( xpos, ypos, width, height ), option, items, this ) );

	if ( !sameline )
	{
		m_current_ypos += height;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = height + 8;
	last_xpos = xpos + width + 8;
}

void c_subtab::add_listbox( std::string name, std::vector<std::string>& items, std::string option, float width, float height, std::string& filter, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	m_controls.push_back( new c_listbox( name, bounds( xpos, ypos, width, height ), option, items, filter, this ) );

	if ( !sameline )
	{
		m_current_ypos += height;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = height + 8;
	last_xpos = xpos + width + 8;
}

void c_subtab::add_textbox( std::string name, std::string& option, bool sameline )
{
	int xpos = sameline ? last_xpos + 8 : parent_window->m_area.x + 16;
	int ypos = sameline ? parent_window->m_area.y + 16 + m_current_ypos - last_ysize : parent_window->m_area.y + 16 + m_current_ypos;

	m_controls.push_back( new c_textbox( name, bounds( xpos, ypos, TEXTBOX_WIDTH + 8, TEXTBOX_HEIGHT ), option, this ) );

	if ( !sameline )
	{
		m_current_ypos += TEXTBOX_HEIGHT;
		m_current_ypos += 8; //maybe add settings for item padding, window padding etc.
	}

	last_ysize = TEXTBOX_HEIGHT + 8;
	last_xpos = xpos + TEXTBOX_WIDTH + 8;
}