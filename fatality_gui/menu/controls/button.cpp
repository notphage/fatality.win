#include "../external_include.h"

c_button::c_button( std::string name, bounds pbounds, uintptr_t function, c_subtab* parent )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_function = function;
	this->m_parent = parent;
}

void c_button::render()
{
	bool pressed = c_menu::get().is_holding( m_bounds ) || c_menu::get().is_pressed( m_bounds );

	render::get().bordered_box( m_bounds.x, m_bounds.y, BUTTON_WIDTH, BUTTON_HEIGHT,
								pressed ? D3DCOLOR_RGBA( 20, 179, 178, 255 ) : m_hovered ? D3DCOLOR_RGBA( 133, 133, 144, ( int )( 220 * hover_progress ) ) : D3DCOLOR_RGBA( 81, 81, 92, 190 ) );

	render::get().bordered_box( m_bounds.x + 1, m_bounds.y + 1,
								BUTTON_WIDTH - 2, BUTTON_HEIGHT - 2,
								D3DCOLOR_RGBA( 30, 32, 38, 190 ) );

	render::get().filled_box( m_bounds.x + 2, m_bounds.y + 2, BUTTON_WIDTH - 4,
							  BUTTON_HEIGHT - 4,
							  D3DCOLOR_RGBA( 42, 44, 51, 255 ) );

	_u( name, m_name );
	render::get().text( Vector2D( m_bounds.x + BUTTON_WIDTH / 2,
								  m_bounds.y + BUTTON_HEIGHT / 2 - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) ),
						name, Color( 180, 180, 187, 250 ), fonts::controlfont, c_font::centered_x );
}

void c_button::update()
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

	if ( c_menu::get().is_pressed( m_bounds ) )
	{
		auto function = m_function;
		if ( !function )
			return;

		__asm
		{
			CALL function;
		}
	}

}

void c_button::update_pos_individual( LONG xdrag, LONG ydrag )
{

}