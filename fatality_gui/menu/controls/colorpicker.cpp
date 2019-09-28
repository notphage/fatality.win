#include "../external_include.h"

c_colorpicker::c_colorpicker( std::string name, bounds pbounds, std::string option, D3DCOLOR default_color, c_subtab* parent, bool show_alpha )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_showalpha = show_alpha;
	this->m_parent = parent;

	_u( key, m_value );
	g_cfg[ key ]->set<uintptr_t>( default_color );

	m_picker_bounds = bounds( m_bounds.x + 8, m_bounds.y + COLORPICKER_HEIGHT + 8, COLORPICKER_WIDTH * 3.5 - 16, COLORPICKER_HEIGHT * 8 - 16 );
	m_box_bounds = m_showalpha ? bounds( m_picker_bounds.x, m_picker_bounds.y, m_picker_bounds.w - 30, m_picker_bounds.h - 22 ) : bounds( m_picker_bounds.x, m_picker_bounds.y, m_picker_bounds.w - 30, m_picker_bounds.h );
	m_hue_bounds = bounds( m_picker_bounds.x + m_picker_bounds.w - 22, m_picker_bounds.y, 22, m_picker_bounds.h );
	m_alpha_bounds = bounds( m_picker_bounds.x, m_picker_bounds.y + m_box_bounds.h + 8, m_box_bounds.w, 14 );

	m_hue_mouse_pos.x = m_hue_bounds.x + m_hue_bounds.w / 2;
	m_hue_mouse_pos.y = std::clamp( m_hue_bounds.y + int( std::roundf( m_colHSV[ 0 ] * m_hue_bounds.h ) ), m_hue_bounds.y, m_hue_bounds.y + m_hue_bounds.h );

	m_picker_mouse_pos.x = m_box_bounds.x + m_box_bounds.w - 6;
	m_picker_mouse_pos.y = m_box_bounds.y;

	m_alpha_mouse_pos.x = m_alpha_bounds.x + 1;
	m_alpha_mouse_pos.y = m_alpha_bounds.y;
}

void c_colorpicker::render()
{
	render::get().bordered_box( m_bounds.x, m_bounds.y, COLORPICKER_WIDTH, COLORPICKER_HEIGHT,
								m_active ? D3DCOLOR_RGBA( 20, 179, 178, 255 ) : m_hovered ? D3DCOLOR_RGBA( 133, 133, 144, ( int )( 220 * hover_progress ) ) : D3DCOLOR_RGBA( 81, 81, 92, 190 ) );

	render::get().bordered_box( m_bounds.x + 1, m_bounds.y + 1,
								COLORPICKER_WIDTH - 2, COLORPICKER_HEIGHT - 2,
								D3DCOLOR_RGBA( 30, 32, 38, 190 ) );

	if ( m_active )
		render::get().bordered_box( m_bounds.x, m_bounds.y + COLORPICKER_HEIGHT, COLORPICKER_WIDTH * 3.5, COLORPICKER_HEIGHT * 8, D3DCOLOR_RGBA( 20, 179, 178, 255 ) );

	render::get().filled_box( m_active ? m_bounds.x + 1 : m_bounds.x + 2, m_bounds.y + 2, m_active ? COLORPICKER_WIDTH - 2 : COLORPICKER_WIDTH - 4,
							  m_active ? COLORPICKER_HEIGHT * 8 - 4 : COLORPICKER_HEIGHT - 4,
							  D3DCOLOR_RGBA( 37, 39, 46, 255 ) );

	if ( m_active )
		render::get().filled_box( m_bounds.x + 1, m_bounds.y + COLORPICKER_HEIGHT + 1, COLORPICKER_WIDTH * 3.5 - 2,
								  COLORPICKER_HEIGHT * 8 - 2,
								  D3DCOLOR_RGBA( 37, 39, 46, 255 ) );

	_u( name, m_name );
	render::get().text( Vector2D( m_bounds.x + COLORPICKER_WIDTH + 12,
								  m_bounds.y + COLORPICKER_HEIGHT / 2 - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) ),
						name, Color( 107, 109, 120, 250 ), fonts::controlfont, c_font::left_aligned );

	render::get().bordered_box( m_bounds.x + 8, m_bounds.y + 4,
								COLORPICKER_WIDTH - 28, COLORPICKER_HEIGHT - 8,
								D3DCOLOR_RGBA( 81, 81, 92, 190 ) );

	_u( key, m_value );
	render::get().filled_box( m_bounds.x + 9, m_bounds.y + 5, COLORPICKER_WIDTH - 30,
							  COLORPICKER_HEIGHT - 10,
							  g_cfg[ key ]->get<D3DCOLOR>() );


	static D3DCOLOR hue_colors[ 7 ] =
	{
		D3DCOLOR_RGBA( 255, 0, 0, 255 ),
		D3DCOLOR_RGBA( 255, 255, 0 , 255 ),
		D3DCOLOR_RGBA( 0, 255, 0 , 255 ),
		D3DCOLOR_RGBA( 0, 255, 255 , 255 ),
		D3DCOLOR_RGBA( 0, 0, 255 , 255 ),
		D3DCOLOR_RGBA( 255, 0, 255 , 255 ),
		D3DCOLOR_RGBA( 255, 0, 0 , 255 )
	};

	//das ist wirklioch schlimm aber sieht p100 aus
	if ( m_active )
	{
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 16, m_bounds.y + COLORPICKER_HEIGHT / 2 + 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 15, m_bounds.y + COLORPICKER_HEIGHT / 2 + 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 14, m_bounds.y + COLORPICKER_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 13, m_bounds.y + COLORPICKER_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 12, m_bounds.y + COLORPICKER_HEIGHT / 2 - 2, 3, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 11, m_bounds.y + COLORPICKER_HEIGHT / 2 - 3, 1, 3, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 10, m_bounds.y + COLORPICKER_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 9, m_bounds.y + COLORPICKER_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 8, m_bounds.y + COLORPICKER_HEIGHT / 2 + 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 7, m_bounds.y + COLORPICKER_HEIGHT / 2 + 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );

		render::get().line( Vector2D( m_bounds.x + 8, m_bounds.y + COLORPICKER_HEIGHT ), Vector2D( m_bounds.x + COLORPICKER_WIDTH - 21, m_bounds.y + COLORPICKER_HEIGHT ), D3DCOLOR_RGBA( 81, 81, 92, 255 ) );

		render::get().filled_box( m_box_bounds.x, m_box_bounds.y, m_box_bounds.w, m_box_bounds.h, D3DCOLOR_RGBA( 0, 0, 0, 255 ) );

		for ( auto i = 0; i < 6; ++i )
		{
			render::get().rect_filled_linear_gradient( Vector2D( m_hue_bounds.x, m_hue_bounds.y + i * ( float( m_hue_bounds.h ) / 6.f ) ),
													   Vector2D( m_hue_bounds.w, ( float( m_hue_bounds.h ) / 6.f ) )
													   , hue_colors[ i ], hue_colors[ i + 1 ] );
		}

		D3DCOLOR picker = util::hsb_to_rgb( m_colHSV[ 0 ], 1, 1 );
		render::get().rect_filled_linear_gradient( Vector2D( m_box_bounds.x, m_box_bounds.y ), Vector2D( m_box_bounds.w, m_box_bounds.h ), Color( 255, 255, 255, 255 ), picker, true );
		render::get().rect_filled_linear_gradient( Vector2D( m_box_bounds.x, m_box_bounds.y ), Vector2D( m_box_bounds.w, m_box_bounds.h ), Color( 0, 0, 0, 0 ), Color( 0, 0, 0, 255 ) );

		if ( m_showalpha )
		{
			bool nigga = false;
			D3DVIEWPORT9 backup;
			render::get().get_device()->GetViewport( &backup );
			D3DVIEWPORT9 new_vp;
			new_vp.X = m_alpha_bounds.x + 1;
			new_vp.Y = m_alpha_bounds.y;
			new_vp.Width = m_alpha_bounds.w - 1;
			new_vp.Height = m_alpha_bounds.h;

			render::get().get_device()->SetViewport( &new_vp );

			for ( int i = 0; i <= m_alpha_bounds.w - 1; i += ( m_alpha_bounds.h / 2 ) )
			{
				int pos = m_alpha_bounds.x + 1 + i;
				render::get().filled_box( pos, m_alpha_bounds.y, ( m_alpha_bounds.h / 2 ), m_alpha_bounds.h / 2, nigga ? D3DCOLOR_RGBA( 100, 100, 100, 255 ) : D3DCOLOR_RGBA( 40, 40, 40, 255 ) );
				render::get().filled_box( pos, m_alpha_bounds.y + m_alpha_bounds.h / 2, ( m_alpha_bounds.h / 2 ), m_alpha_bounds.h / 2, nigga ? D3DCOLOR_RGBA( 40, 40, 40, 255 ) : D3DCOLOR_RGBA( 100, 100, 100, 255 ) );
				nigga = !nigga;
			}
			render::get().get_device()->SetViewport( &backup );

			render::get().filled_box( m_alpha_bounds.x + 1, m_alpha_bounds.y, m_alpha_bounds.w, m_alpha_bounds.h, g_cfg[ key ]->get<D3DCOLOR>() );

			render::get().bordered_box( m_alpha_mouse_pos.x + 1, m_alpha_mouse_pos.y + 1, 4, m_alpha_bounds.h - 2, D3DCOLOR_RGBA( 150, 150, 150, 150 ) );
			render::get().bordered_box( m_alpha_mouse_pos.x, m_alpha_mouse_pos.y, 6, m_alpha_bounds.h, D3DCOLOR_RGBA( 0, 0, 0, 255 ) );
		}

		render::get().bordered_box( m_hue_mouse_pos.x - m_hue_bounds.w / 2, m_hue_mouse_pos.y + 1, m_hue_bounds.w, 4, D3DCOLOR_RGBA( 150, 150, 150, 150 ) );
		render::get().bordered_box( m_hue_mouse_pos.x - m_hue_bounds.w / 2 - 1, m_hue_mouse_pos.y, m_hue_bounds.w + 2, 6, D3DCOLOR_RGBA( 0, 0, 0, 255 ) );

		render::get().bordered_box( m_picker_mouse_pos.x + 1, m_picker_mouse_pos.y + 1, 4, 4, D3DCOLOR_RGBA( 150, 150, 150, 150 ) );
		render::get().bordered_box( m_picker_mouse_pos.x, m_picker_mouse_pos.y, 6, 6, D3DCOLOR_RGBA( 0, 0, 0, 255 ) );
	}
	else
	{
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 16, m_bounds.y + COLORPICKER_HEIGHT / 2 - 3, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 15, m_bounds.y + COLORPICKER_HEIGHT / 2 - 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 14, m_bounds.y + COLORPICKER_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 13, m_bounds.y + COLORPICKER_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 12, m_bounds.y + COLORPICKER_HEIGHT / 2 + 1, 3, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 11, m_bounds.y + COLORPICKER_HEIGHT / 2 + 1, 1, 3, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 10, m_bounds.y + COLORPICKER_HEIGHT / 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 9, m_bounds.y + COLORPICKER_HEIGHT / 2 - 1, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 8, m_bounds.y + COLORPICKER_HEIGHT / 2 - 2, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
		render::get().filled_box( m_bounds.x + COLORPICKER_WIDTH - 7, m_bounds.y + COLORPICKER_HEIGHT / 2 - 3, 2, 2, D3DCOLOR_RGBA( 133, 133, 144, 255 ) );
	}
}

#define get_a(col) (((col)&0xff000000)>>24)

void c_colorpicker::update()
{
	m_picker_bounds = bounds( m_bounds.x + 8, m_bounds.y + COLORPICKER_HEIGHT + 8, COLORPICKER_WIDTH * 3.5 - 16, COLORPICKER_HEIGHT * 8 - 16 );
	m_box_bounds = m_showalpha ? bounds( m_picker_bounds.x, m_picker_bounds.y, m_picker_bounds.w - 30, m_picker_bounds.h - 22 ) : bounds( m_picker_bounds.x, m_picker_bounds.y, m_picker_bounds.w - 30, m_picker_bounds.h );
	m_hue_bounds = bounds( m_picker_bounds.x + m_picker_bounds.w - 22, m_picker_bounds.y, 22, m_picker_bounds.h - 1 );
	m_alpha_bounds = bounds( m_picker_bounds.x, m_picker_bounds.y + m_box_bounds.h + 8, m_box_bounds.w, 14 );

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

	bounds picker_bounds = bounds( m_bounds.x + 8, m_bounds.y + COLORPICKER_HEIGHT + 8, COLORPICKER_WIDTH * 3.5 - 16, COLORPICKER_HEIGHT * 8 - 16 );
	bounds box_bounds = bounds( picker_bounds.x, picker_bounds.y, picker_bounds.w - 30, picker_bounds.h );
	bounds hue_bounds = bounds( picker_bounds.x + picker_bounds.w - 22, picker_bounds.y, 22, picker_bounds.h );

	static auto was_active = false;

	if ( c_menu::get().is_pressed( m_bounds ) )
	{
		if ( m_active )
		{
			m_active = false;
			m_parent->focussed_control = nullptr;
		}
		else
		{
			_u( key, m_value );
			D3DCOLOR col = g_cfg[ key ]->get<D3DCOLOR>();

			float hue = util::get_hue( col );
			float saturation = util::get_saturation( col );
			float brightness = util::get_brightness( col ) / 255.f;

			m_picker_mouse_pos.x = m_box_bounds.x + ( ( m_box_bounds.w - 1 ) * saturation );
			m_picker_mouse_pos.y = ( m_box_bounds.y + ( m_box_bounds.h - 1 ) ) - ( ( m_box_bounds.h - 1 ) * brightness );

			m_hue_mouse_pos.x = m_hue_bounds.x + m_hue_bounds.w / 2;
			m_hue_mouse_pos.y = m_hue_bounds.y + ( ( m_hue_bounds.h - 1 ) * hue );

			float alpha = get_a( col );
			float alpha_multiplier = alpha / 255.f;
			alpha_multiplier = std::clamp( alpha_multiplier, 0.f, 1.f );

			if ( alpha == 255 )
				m_alpha_mouse_pos.x = m_alpha_bounds.x + 1;
			else
				m_alpha_mouse_pos.x = std::clamp( ( int )( m_alpha_bounds.x + m_alpha_bounds.w + 2 - ( ( m_alpha_bounds.w ) * alpha_multiplier ) ), m_alpha_bounds.x, m_alpha_bounds.x + m_alpha_bounds.w - 6 );
			m_alpha_mouse_pos.y = m_alpha_bounds.y;

			m_active = true;
			m_parent->focussed_control = this;
		}
	}
	else if ( c_menu::get().mouse_is_pressed() && !c_menu::get().is_pressed( picker_bounds ) && m_active )
	{
		m_active = false;
		m_parent->focussed_control = nullptr;
	}

	if ( m_parent->focussed_control != this )
		m_active = false;

	if ( !m_active )
	{
		was_active = false;
		return;
	}

	_u( key, m_value );
	D3DCOLOR col = g_cfg[ key ]->get<uintptr_t>();
	POINT mouse_pos = c_menu::get().get_mouse_pos();

	if ( m_curfocus == 0 )
	{
		if ( c_menu::get().is_holding( m_box_bounds ) )
			m_curfocus = 1;
		else if ( c_menu::get().is_holding( m_hue_bounds ) )
			m_curfocus = 2;
		else if ( m_showalpha && c_menu::get().is_holding( m_alpha_bounds ) )
			m_curfocus = 3;
	}
	else if ( !c_menu::get().mouse_is_holding() )
		m_curfocus = 0;

	switch ( m_curfocus )
	{
		case 1:
		{
			m_picker_mouse_pos.x = std::clamp( ( int )c_menu::get().get_mouse_pos().x, m_box_bounds.x, m_box_bounds.x + m_box_bounds.w - 6 );
			m_picker_mouse_pos.y = std::clamp( ( int )c_menu::get().get_mouse_pos().y, m_box_bounds.y, m_box_bounds.y + m_box_bounds.h - 6 );
		}break;

		case 2:
		{
			m_hue_mouse_pos.x = m_hue_bounds.x + m_hue_bounds.w / 2;
			m_hue_mouse_pos.y = std::clamp( ( int )c_menu::get().get_mouse_pos().y, m_hue_bounds.y, m_hue_bounds.y + m_hue_bounds.h - 6 );
		}break;

		case 3:
		{
			m_alpha_mouse_pos.x = std::clamp( ( int )c_menu::get().get_mouse_pos().x, m_alpha_bounds.x + 1, m_alpha_bounds.x + m_alpha_bounds.w - 6 );
			m_alpha_mouse_pos.y = m_alpha_bounds.y;
		}break;
	}

	if ( m_active && !was_active )
	{
		was_active = true;
	}

	m_colHSV[ 0 ] = std::clamp( float( m_hue_mouse_pos.y - m_hue_bounds.y ) / float( m_hue_bounds.h - 1 ), 0.0f, 1.0f );
	m_colHSV[ 1 ] = std::clamp( float( m_picker_mouse_pos.x - m_box_bounds.x ) / float( m_box_bounds.w - 1 ), 0.0f, 1.0f );
	m_colHSV[ 2 ] = 1.0f - std::clamp( float( m_picker_mouse_pos.y - m_box_bounds.y ) / float( m_box_bounds.h - 1 ), 0.0f, 1.0f );

	g_cfg[ key ]->set( util::hsb_to_rgb( m_colHSV[ 0 ], m_colHSV[ 1 ], m_colHSV[ 2 ] ) );
	int alpha = std::clamp( int( float( m_alpha_mouse_pos.x - m_alpha_bounds.x ) / m_alpha_bounds.w * 255.f ), 0, 255 );
	if ( m_alpha_mouse_pos.x <= m_alpha_bounds.x ) alpha = 255;
	g_cfg[ key ]->set( c_menu::get().modulate_alpha( g_cfg[ key ]->get<uintptr_t>(), alpha ) );

}

void c_colorpicker::update_pos_individual( LONG xdrag, LONG ydrag )
{
	m_picker_mouse_pos.x += xdrag;
	m_picker_mouse_pos.y += ydrag;

	m_hue_mouse_pos.x += xdrag;
	m_hue_mouse_pos.y += ydrag;

	m_alpha_mouse_pos.x += xdrag;
	m_alpha_mouse_pos.y += ydrag;
}