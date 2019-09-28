#include "../external_include.h"

c_combobox::c_combobox( std::string name, bounds pbounds, std::string option, std::vector<std::string>* items, c_subtab* parent )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_items = *items;
	this->m_parent = parent;

	_u( key, m_value );
	g_cfg[ key ]->set<int>( 0 );
}

void c_combobox::render()
{
	int total_size = COMBOBOX_HEIGHT + 8 + m_items.size() * 18;

	render::get().bordered_box( m_bounds.x, m_bounds.y, COMBOBOX_WIDTH, m_active ? COMBOBOX_HEIGHT + ( ( 8 + m_items.size() * 18 ) * anim_progress ) : COMBOBOX_HEIGHT,
								m_active ? D3DCOLOR_RGBA( 20, 179, 178, 255 ) : m_hovered ? D3DCOLOR_RGBA( 133, 133, 144, ( int )( 220 * hover_progress ) ) : D3DCOLOR_RGBA( 81, 81, 92, 190 ) );

	render::get().bordered_box( m_bounds.x + 1, m_bounds.y + 1,
								COMBOBOX_WIDTH - 2, m_active ? ( COMBOBOX_HEIGHT - 2 ) + ( ( 8 + m_items.size() * 18 ) * anim_progress ) : COMBOBOX_HEIGHT - 2,
								D3DCOLOR_RGBA( 30, 32, 38, 190 ) );

	render::get().filled_box( m_bounds.x + 2, m_bounds.y + 2, COMBOBOX_WIDTH - 4,
							  m_active ? ( COMBOBOX_HEIGHT - 4 ) + ( ( 8 + m_items.size() * 18 ) * anim_progress ) : COMBOBOX_HEIGHT - 4,
							  D3DCOLOR_RGBA( 37, 39, 46, 255 ) );

	_u( name, m_name );
	render::get().text( Vector2D( m_bounds.x + COMBOBOX_WIDTH + 12,
								  m_bounds.y + COMBOBOX_HEIGHT / 2 - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) ),
						name, Color( 107, 109, 120, 250 ), fonts::controlfont );

	_u( key, m_value );
	g_cfg[ key ]->set( std::min( g_cfg[ key ]->get<int>(), static_cast< int >( m_items.size() ) -1 ) );
	_u( preview, m_items[ g_cfg[ key ]->get<int>() ] );
	render::get().text( Vector2D( m_bounds.x + 8,
								  m_bounds.y + COMBOBOX_HEIGHT / 2 - ( render::get().get_text_size( preview, fonts::controlfont ).y / 2 ) ),
						preview, m_active ? Color( 119, 118, 126, 250 ) : Color( 180, 180, 187, 250 ), fonts::controlfont );

	//das ist wirklioch schlimm aber sieht p100 aus
	if ( m_active )
	{
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 16, m_bounds.y + COMBOBOX_HEIGHT / 2 + 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 15, m_bounds.y + COMBOBOX_HEIGHT / 2 + 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 14, m_bounds.y + COMBOBOX_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 13, m_bounds.y + COMBOBOX_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 12, m_bounds.y + COMBOBOX_HEIGHT / 2 - 2, 3, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 11, m_bounds.y + COMBOBOX_HEIGHT / 2 - 3, 1, 3, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 10, m_bounds.y + COMBOBOX_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 9, m_bounds.y + COMBOBOX_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 8, m_bounds.y + COMBOBOX_HEIGHT / 2 + 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 7, m_bounds.y + COMBOBOX_HEIGHT / 2 + 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );

		render::get().line( Vector2D( m_bounds.x + 8, m_bounds.y + COMBOBOX_HEIGHT ), Vector2D( m_bounds.x + COMBOBOX_WIDTH - 8, m_bounds.y + COMBOBOX_HEIGHT ), Color( 81, 81, 92, 255 ) );

		int ypos = m_bounds.y + COMBOBOX_HEIGHT + 4;
		for ( int i = 0; i < m_items.size(); i++ )
		{
			if ( m_bounds.y + 2 + ( COMBOBOX_HEIGHT - 4 ) + ( ( 8 + m_items.size() * 18 ) * anim_progress ) < ypos + 18 )
				break;

			std::string temp = m_items[ i ];

			_u( preview, temp );
			bool hovered = c_menu::get().is_hovering( bounds( m_bounds.x, ypos, COMBOBOX_WIDTH, 18 ) );
			render::get().text( Vector2D( m_bounds.x + 12, ypos + 9 - ( render::get().get_text_size( preview, fonts::tabfontthicc ).y / 2 ) ),
								preview, hovered ? Color( 20, 179, 178, 255 ) : Color( 180, 180, 187, 250 ), fonts::tabfontthicc );
			ypos += 18;
		}
	}
	else
	{
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 16, m_bounds.y + COMBOBOX_HEIGHT / 2 - 3, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 15, m_bounds.y + COMBOBOX_HEIGHT / 2 - 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 14, m_bounds.y + COMBOBOX_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 13, m_bounds.y + COMBOBOX_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 12, m_bounds.y + COMBOBOX_HEIGHT / 2 + 1, 3, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 11, m_bounds.y + COMBOBOX_HEIGHT / 2 + 1, 1, 3, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 10, m_bounds.y + COMBOBOX_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 9, m_bounds.y + COMBOBOX_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 8, m_bounds.y + COMBOBOX_HEIGHT / 2 - 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COMBOBOX_WIDTH - 7, m_bounds.y + COMBOBOX_HEIGHT / 2 - 3, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
	}
}

void c_combobox::update()
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

	int size = m_items.size() * 18;
	bounds combo_area = bounds( m_bounds.x, m_bounds.y + COMBOBOX_HEIGHT + 4, m_bounds.x + COMBOBOX_WIDTH, size + 4 );
	if ( c_menu::get().is_pressed( m_bounds ) )
	{
		if ( m_active )
		{
			m_active = false;
			m_parent->focussed_control = nullptr;
		}
		else
		{
			m_active = true;
			m_parent->focussed_control = this;
		}
	}
	else if ( c_menu::get().mouse_is_pressed() && !c_menu::get().is_pressed( combo_area ) && m_parent->focussed_control == this ) //mouse was pressed but not in combo area
	{
		m_active = false;
		m_parent->focussed_control = nullptr;
	}

	if ( m_parent->focussed_control != this )
		m_active = false;

	if ( m_active )
	{
		if ( anim_progress < 1.f )
			anim_progress += m_frequence * g_pGlobals->frametime;
	}
	else
	{
		if ( anim_progress > 0.f )
			anim_progress -= m_frequence * g_pGlobals->frametime;
	}

	if ( anim_progress < 0.f )
		anim_progress = 0.f;
	if ( anim_progress > 1.f )
		anim_progress = 1.f;

	if ( !m_active )
		return;

	if ( c_menu::get().is_pressed( combo_area ) ) //user clicked somewhere in open combo - find out where
	{
		_u( key, m_value );
		for ( int i = 0; i < m_items.size(); i++ )
		{
			if ( c_menu::get().is_pressed( bounds( combo_area.x, combo_area.y + i * 18, combo_area.w, 18 ) ) )
			{
				g_cfg[ key ]->set( i );
				m_active = false;
				m_parent->focussed_control = nullptr;
				c_menu::get().reset_mouse_state(); //ghetto fix to fix sliders acting under combos
				break;
			}
			continue;
		}
	}
}

void c_combobox::update_pos_individual( LONG xdrag, LONG ydrag )
{

}