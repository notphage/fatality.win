#include "../external_include.h"

const char* text_keys[ 254 ] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
"y", "z", nullptr, nullptr, nullptr, nullptr, nullptr, "0", "1", "2", "3", "4", "5", "6",
"7", "8", "9", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, ";", "+", ",", "-", ".", "/?", "~", nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "[", "\\", "]", "'", nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

c_textbox::c_textbox( std::string name, bounds pbounds, std::string& option, c_subtab* parent )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = &option;
	this->m_parent = parent;
}

void c_textbox::render()
{
	render::get().bordered_box( m_bounds.x, m_bounds.y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
								m_listening ? D3DCOLOR_RGBA( 20, 179, 178, 255 ) : m_hovered ? D3DCOLOR_RGBA( 133, 133, 144, ( int )( 220 * hover_progress ) ) : D3DCOLOR_RGBA( 81, 81, 92, 190 ) );

	render::get().bordered_box( m_bounds.x + 1, m_bounds.y + 1,
								TEXTBOX_WIDTH - 2, TEXTBOX_HEIGHT - 2,
								D3DCOLOR_RGBA( 30, 32, 38, 190 ) );

	render::get().filled_box( m_bounds.x + 2, m_bounds.y + 2, TEXTBOX_WIDTH - 4,
							  TEXTBOX_HEIGHT - 4,
							  D3DCOLOR_RGBA( 37, 39, 46, 255 ) );

	render::get().text( Vector2D( m_bounds.x + TEXTBOX_WIDTH / 2,
								  m_bounds.y + TEXTBOX_HEIGHT / 2 - ( render::get().get_text_size( m_name, fonts::controlfont ).y / 2 ) ),
						m_listening ? *m_value + "_" : *m_value, D3DCOLOR_RGBA( 180, 180, 187, 250 ), fonts::controlfont );

	_u( name, m_name );
	render::get().text( Vector2D( m_bounds.x + TEXTBOX_WIDTH + 12,
								  m_bounds.y + TEXTBOX_HEIGHT / 2 - ( render::get().get_text_size( m_name, fonts::controlfont ).y / 2 ) ),
						name, D3DCOLOR_RGBA( 107, 109, 120, 250 ), fonts::controlfont );
}

void c_textbox::update()
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
		for ( auto i = 0; i < 255; ++i )
		{
			if ( c_menu::get().key_updated( i ) )
			{
				if ( i == VK_BACK && strlen( m_value->c_str() ) != 0 )
				{
					*m_value = m_value->substr( 0, strlen( m_value->c_str() ) - 1 );
					break;
				}
				else if ( i == VK_SPACE && strlen( m_value->c_str() ) != 0 )
				{
					*m_value += " ";
					break;
				}
				else if ( strlen( m_value->c_str() ) < 20 && text_keys[ i ] != nullptr )
				{
					*m_value += text_keys[ i ];
					break;
				}
			}
		}
	}

	if ( c_menu::get().is_pressed( m_bounds ) && !m_listening )
	{
		m_listening = true;
	}
	if ( m_listening && c_menu::get().mouse_is_pressed() && !c_menu::get().is_in_area( c_menu::get().get_mouse_pos(), m_bounds ) )
	{
		m_listening = false;
	}
}

void c_textbox::update_pos_individual( LONG xdrag, LONG ydrag )
{

}