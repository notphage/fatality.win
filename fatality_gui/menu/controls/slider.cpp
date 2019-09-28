#include "../external_include.h"

c_slider::c_slider( std::string name, bounds pbounds, std::string option, float min, float max, c_subtab* parent, std::string format )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_parent = parent;
	this->m_format = format;
	this->m_max = max;
	this->m_min = min;
	this->m_roundvalue = false;

	_u( key, m_value );
	g_cfg[ key ]->set<float>( min );
}

c_slider::c_slider( std::string name, bounds pbounds, std::string option, int min, int max, c_subtab* parent, std::string format )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_parent = parent;
	this->m_format = format;
	this->m_max = max;
	this->m_min = min;
	this->m_roundvalue = true;

	_u( key, m_value );
	g_cfg[ key ]->set<int>( min );
}

void c_slider::render()
{
	render::get().rect_filled_linear_gradient( Vector2D( m_bounds.x, m_bounds.y + 6 ), Vector2D( SLIDER_WIDTH, SLIDER_HEIGHT - 11 ), Color( 19, 22, 22, 255 ), Color( 29, 32, 33, 255 ) );

	_u( name, m_name );

	render::get().text( Vector2D( m_bounds.x + SLIDER_WIDTH + 12,
								  m_bounds.y + SLIDER_HEIGHT / 2 - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) ),
						name, Color( 107, 109, 120, 250 ), fonts::controlfont );

	char buffer[ 24 ];
	_u( key, m_value );
	_u( format_s, m_format );
	if ( m_roundvalue )
		sprintf_s( buffer, format_s, g_cfg[ key ]->get<int>() );
	else
		sprintf_s( buffer, format_s, g_cfg[ key ]->get<float>() );
	std::string format( buffer );

	render::get().text( Vector2D( m_bounds.x + SLIDER_WIDTH + 16 + render::get().get_text_size( name, fonts::controlfont ).x,
								  m_bounds.y + SLIDER_HEIGHT / 2 - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) ),
						format, Color( 167, 169, 180, 250 ), fonts::controlfont );

	float slider_ratio = ( g_cfg[ key ]->get<float>() - m_min ) / ( m_max - m_min );
	float slider_location = slider_ratio * SLIDER_WIDTH;

	render::get().circle_filled( Vector2D( m_bounds.x + slider_location, m_bounds.y + SLIDER_HEIGHT / 2 ), 8, Color( 200, 203, 218, 255 ) );
	render::get().circle( Vector2D( m_bounds.x + slider_location, m_bounds.y + SLIDER_HEIGHT / 2 ), 8, Color( 81, 81, 92, 255 ) );
	//render::get().circle(m_bounds.x + slider_location, m_bounds.y + SLIDER_HEIGHT / 2, 8, 64, D3DCOLOR_RGBA(25, 30, 50, 255));
}

void c_slider::update()
{
	_u( key, m_value );
	float slider_ratio = ( g_cfg[ key ]->get<float>() - m_min ) / ( m_max - m_min );
	float slider_location = slider_ratio * SLIDER_WIDTH;

	if ( !c_menu::get().is_in_area( c_menu::get().get_press_pos(), bounds( m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT ) )
		 && !c_menu::get().is_in_area( c_menu::get().get_press_pos(), bounds( slider_location - 8, m_bounds.y + ( SLIDER_HEIGHT / 2 ) - 8, 16, 16 ) ) )
		return;

	if ( c_menu::get().is_pressed( bounds( m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT ) )
		 || c_menu::get().is_pressed( bounds( slider_location - 8, m_bounds.y + ( SLIDER_HEIGHT / 2 ) - 8, 16, 16 ) ) )
	{
		float new_pos, slider_ratio;
		new_pos = ( float )( c_menu::get().get_mouse_pos().x - m_bounds.x - 1 );

		if ( new_pos < 0 )
			new_pos = 0;

		if ( new_pos > SLIDER_WIDTH )
			new_pos = ( int )SLIDER_WIDTH;

		slider_ratio = new_pos / ( float )( SLIDER_WIDTH );
		g_cfg[ key ]->set( m_min + ( m_max - m_min ) * slider_ratio );
	}
	else if ( c_menu::get().is_holding( bounds( m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT ) )
			  || c_menu::get().is_holding( bounds( slider_location - 8, m_bounds.y + ( SLIDER_HEIGHT / 2 ) - 8, 16, 16 ) ) )
		m_active = true;

	if ( m_active )
	{
		if ( !c_menu::get().mouse_is_holding() )
			m_active = false;

		m_wasactive = true;

		m_parent->focussed_control = this;

		float new_pos, slider_ratio;
		new_pos = ( float )( c_menu::get().get_mouse_pos().x - m_bounds.x - 1 );

		if ( new_pos < 0 )
			new_pos = 0;

		if ( new_pos > SLIDER_WIDTH )
			new_pos = ( int )SLIDER_WIDTH;

		slider_ratio = new_pos / ( float )( SLIDER_WIDTH );
		g_cfg[ key ]->set( m_min + ( m_max - m_min ) * slider_ratio );
	}
	else
	{
		if ( m_roundvalue && m_wasactive )
		{
			g_cfg[ key ]->set( round_value() );
			m_wasactive = false;
		}
		m_parent->focussed_control = nullptr;
	}
}

float c_slider::round_value()
{
	_u( key, m_value );
	int rounded = g_cfg[ key ]->get<float>();
	return rounded;
}

void c_slider::update_pos_individual( LONG xdrag, LONG ydrag )
{

}