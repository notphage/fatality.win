#pragma once

#define CHECKBOX_HEIGHT 18

#define COMBOBOX_HEIGHT 24
#define COMBOBOX_WIDTH 200

#define SLIDER_HEIGHT 18
#define SLIDER_WIDTH 200

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 24

#define KEYBIND_WIDTH 60
#define KEYBIND_HEIGHT 24

#define TEXTBOX_WIDTH 200
#define TEXTBOX_HEIGHT 24

#define COLORPICKER_WIDTH 80
#define COLORPICKER_HEIGHT 24

struct mouse_info
{
	POINT pos;

	POINT press_pos;
	POINT hold_pos;

	bool is_pressed;
	bool is_holding; // used to know of its holding/moving smth

	bool needs_reset;
};

static mouse_info g_mouse;

struct bounds
{
	bounds()
	{
		this->x = 0;
		this->y = 0;
		this->w = 0;
		this->h = 0;
	}
	bounds( int x, int y, int w, int h )
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
	bounds( int x, int y )
	{
		this->x = x;
		this->y = y;
		this->w = 0;
		this->h = 0;
	}

	int x, y, w, h;
};

class c_window;
class c_menu : public singleton<c_menu>
{
public:
	void render();
	void init();

	bool is_hovering( bounds pbounds );
	bool is_holding( bounds pbounds ) const;
	bool is_pressed( bounds pbounds );

	bool mouse_is_pressed();
	bool mouse_is_holding();
	void reset_mouse_state();

	bool key_updated( int key );

	uintptr_t modulate_alpha( uintptr_t orig, int alpha );

	static POINT get_mouse_pos();
	static POINT get_hold_pos();
	static POINT get_press_pos();

	static bool is_in_area( POINT p, bounds pbounds );
	bool& get_opened();

	void toggle();

	void release_images();

protected:

	IDirect3DDevice9 * m_device;
private:



	bool m_keystate[ 256 ] = {};
	bool m_oldstate[ 256 ] = {};

	static void draw_cursor();
	void update_input();
	std::vector<c_window*> m_windows;

	bool m_opened = false;
	bool m_init = false;
};
