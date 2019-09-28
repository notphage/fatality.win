#include "../external_include.h"

const char* keys[] = { "NO KEY", "M1", "M2", "BRK", "M3", "M4", "M5",
"UNK", "BSPC", "TAB", "UNK", "UNK", "UNK", "ENTER", "UNK", "UNK", "SHFT",
"CTRL", "ALT","PAUSE","CAPS", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"ESC", "UNK", "UNK", "UNK", "UNK", "SPACE","PGUP", "PGDOWN", "END", "HOME", "Left",
"UP", "RIGHT", "DOWN", "UNK", "PRNT", "UNK", "PRTSCR", "INS","DEL", "UNK", "0", "1",
"2", "3", "4", "5", "6", "7", "8", "9", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"UNK", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U",
"V", "W", "X","Y", "Z", "LFTWIN", "RGHTWIN", "UNK", "UNK", "UNK", "NUM0", "NUM1",
"NUM2", "NUM3", "NUM4", "NUM5", "NUM6","NUM7", "NUM8", "NUM9", "*", "+", "UNK", "-", ".", "/", "F1", "F2", "F3",
"F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12","F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21",
"F22", "F23", "F24", "UNK", "UNK", "UNK", "UNK", "UNK","UNK", "UNK", "UNK",
"NUMLCK", "SCRLLCK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"UNK", "UNK","UNK", "UNK", "UNK", "UNK", "UNK", "LSHFT", "RSHFT", "LCTRL",
"RCTRL", "LMENU", "RMENU", "UNK","UNK", "UNK","UNK", "UNK", "UNK", "UNK",
"UNK", "UNK", "UNK", "NTRK", "PTRK", "STOP", "PLAY", "UNK", "UNK",
"UNK", "UNK", "UNK", "UNK", ";", "+", ",", "-", ".", "?", "~", "UNK", "UNK",
"UNK", "UNK","UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK","UNK",
"UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"UNK", "UNK", "UNK","UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK", "UNK",
"UNK", "UNK" };

c_keybind::c_keybind( std::string name, bounds pbounds, std::string option, c_subtab* parent )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_parent = parent;
}

void c_keybind::render()
{
	render::get().bordered_box( m_bounds.x, m_bounds.y, KEYBIND_WIDTH, BUTTON_HEIGHT,
								m_listening ? D3DCOLOR_RGBA( 20, 179, 178, 255 ) : m_hovered ? Color( 133, 133, 144, ( int )( 220 * hover_progress ) ) : D3DCOLOR_RGBA( 81, 81, 92, 190 ) );

	render::get().bordered_box( m_bounds.x + 1, m_bounds.y + 1,
								KEYBIND_WIDTH - 2, BUTTON_HEIGHT - 2,
								D3DCOLOR_RGBA( 30, 32, 38, 190 ) );

	render::get().filled_box( m_bounds.x + 2, m_bounds.y + 2, KEYBIND_WIDTH - 4,
							  KEYBIND_HEIGHT - 4,
							  D3DCOLOR_RGBA( 37, 39, 46, 255 ) );
	_u( key, m_value );

	render::get().text( Vector2D( m_bounds.x + KEYBIND_WIDTH / 2,
								  m_bounds.y + 5 ),
						keys[ g_cfg[ key ]->get<int>() ], Color( 180, 180, 187, 250 ), fonts::controlfont, c_font::centered_x );

	_u( name, m_name );
	render::get().text( Vector2D( m_bounds.x + KEYBIND_WIDTH + 12,
								  m_bounds.y + KEYBIND_HEIGHT / 2 - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) ),
						name, Color( 107, 109, 120, 250 ), fonts::controlfont );
}

void c_keybind::update()
{
	if ( c_menu::get().is_hovering( m_bounds ) )
		m_hovered = true;
	else
		m_hovered = false;

	if ( m_hovered && hover_progress < 1.f )
		hover_progress += m_frequence * g_pGlobals->frametime;
	else if ( !m_hovered && hover_progress > 0.f )
		hover_progress -= m_frequence * g_pGlobals->frametime;

	if ( hover_progress < 0.f )
		hover_progress = 0.f;
	if ( hover_progress > 1.f )
		hover_progress = 1.f;

	if ( m_listening )
	{
		_u( key, m_value );
		for ( auto i = 0; i < 255; ++i )
		{
			// a key is updated
			if ( c_menu::get().key_updated( i ) )
			{
				// menu or escape key kills the listener
				if ( i == VK_ESCAPE )
				{
					g_cfg[ key ]->set( 0 );
					m_listening = false;
					break;
				}

				// we have a valid key
				else
				{
					g_cfg[ key ]->set( i );
					m_listening = false;
					break;
				}
			}
		}
	}
	if ( c_menu::get().is_pressed( m_bounds ) && !m_listening )
	{
		m_listening = true;
	}
}

void c_keybind::update_pos_individual( LONG xdrag, LONG ydrag )
{

}