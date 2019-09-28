#include "../external_include.h"

c_checkbox::c_checkbox( std::string name, bounds pbounds, std::string option, c_subtab* parent )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_parent = parent;

	_u( key, option );
	g_cfg[ key ]->set<bool>( false );
}

void c_checkbox::render()
{
	_u( key, m_value );
	render::get().filled_box( m_bounds.x, m_bounds.y, CHECKBOX_HEIGHT, CHECKBOX_HEIGHT, g_cfg[ key ]->get<bool>() ? D3DCOLOR_RGBA( 20, 179, 178, 255 ) : D3DCOLOR_RGBA( 158, 158, 158, 255 ) );
	//render::get().bordered_box(m_bounds.x, m_bounds.y, CHECKBOX_HEIGHT - 2, CHECKBOX_HEIGHT - 2, *m_value ? D3DCOLOR_RGBA(20, 179, 178, 255) : D3DCOLOR_RGBA(130, 133, 148, 255));

	_u( name, m_name );
	render::get().text( Vector2D( m_bounds.x + CHECKBOX_HEIGHT + 4, m_bounds.y + ( CHECKBOX_HEIGHT / 2 ) - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) - 1 ),
						name, Color( 180, 180, 187, 255 ), fonts::controlfont, c_font::left_aligned );

	render::get().filled_box( m_bounds.x + ( CHECKBOX_HEIGHT / 2 ) - ( ( CHECKBOX_HEIGHT - 2 ) / 2 ) + ( ( ( CHECKBOX_HEIGHT - 2 ) / 2 ) * anim_progress ),
							  m_bounds.y + ( CHECKBOX_HEIGHT / 2 ) - ( ( CHECKBOX_HEIGHT - 2 ) / 2 ) + ( ( ( CHECKBOX_HEIGHT - 2 ) / 2 ) * anim_progress ),
							  ( CHECKBOX_HEIGHT - 2 ) - ( CHECKBOX_HEIGHT - 2 ) * anim_progress,
							  ( CHECKBOX_HEIGHT - 2 ) - ( CHECKBOX_HEIGHT - 2 ) * anim_progress,
							  D3DCOLOR_RGBA( 50, 53, 68, 255 ) );

	
	if ( !g_cfg[ key ]->get<bool>() && anim_progress <= 0.f )
		render::get().filled_box( m_bounds.x + 1, m_bounds.y + 1, CHECKBOX_HEIGHT - 2, CHECKBOX_HEIGHT - 2, D3DCOLOR_RGBA( 158, 158, 158, ( int )( 60 * hover_progress ) ) );

	if ( g_cfg[ key ]->get<bool>() && anim_progress >= 1.f )
	{
		D3DCOLOR col = D3DCOLOR_RGBA( 50, 53, 68, 255 );
		if ( checkmark_progress >= 0.f )
		{
			render::get().filled_box( m_bounds.x + 3, m_bounds.y + CHECKBOX_HEIGHT - 9, 2, 2, col );
			render::get().filled_box( m_bounds.x + 4, m_bounds.y + CHECKBOX_HEIGHT - 8, 2, 2, col );
		}

		if ( checkmark_progress >= 0.1f )
		{
			render::get().filled_box( m_bounds.x + 5, m_bounds.y + CHECKBOX_HEIGHT - 7, 2, 2, col );
		}
		if ( checkmark_progress >= 0.2f )
		{
			render::get().filled_box( m_bounds.x + 6, m_bounds.y + CHECKBOX_HEIGHT - 6, 3, 2, col );
		}
		if ( checkmark_progress >= 0.3f )
		{
			render::get().filled_box( m_bounds.x + 7, m_bounds.y + CHECKBOX_HEIGHT - 5, 1, 2, col );
		}

		if ( checkmark_progress >= 0.4f )
		{
			render::get().filled_box( m_bounds.x + 8, m_bounds.y + CHECKBOX_HEIGHT - 7, 2, 2, col );
		}
		if ( checkmark_progress >= 0.5f )
		{
			render::get().filled_box( m_bounds.x + 9, m_bounds.y + CHECKBOX_HEIGHT - 8, 2, 2, col );
		}
		if ( checkmark_progress >= 0.6f )
		{
			render::get().filled_box( m_bounds.x + 10, m_bounds.y + CHECKBOX_HEIGHT - 9, 2, 2, col );
		}
		if ( checkmark_progress >= 0.7f )
		{
			render::get().filled_box( m_bounds.x + 11, m_bounds.y + CHECKBOX_HEIGHT - 10, 2, 2, col );
		}
		if ( checkmark_progress >= 0.8f )
		{
			render::get().filled_box( m_bounds.x + 12, m_bounds.y + CHECKBOX_HEIGHT - 11, 2, 2, col );
		}
		if ( checkmark_progress >= 0.9f )
		{
			render::get().filled_box( m_bounds.x + 13, m_bounds.y + CHECKBOX_HEIGHT - 12, 2, 2, col );
		}
		if ( checkmark_progress >= 1.f )
		{
			render::get().filled_box( m_bounds.x + 14, m_bounds.y + CHECKBOX_HEIGHT - 13, 2, 2, col );
		}
	}

}

void c_checkbox::update()
{
	_u( key, m_value );

	if ( c_menu::get().is_pressed( m_bounds ) )
		g_cfg[ key ]->set( !( g_cfg[ key ]->get<bool>() ) );

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

	if ( g_cfg[ key ]->get<bool>() )
	{
		if ( anim_progress < 1.f )
			anim_progress += m_frequence * g_pGlobals->frametime;
		else if ( checkmark_progress < 1.f )
			checkmark_progress += m_frequence * g_pGlobals->frametime;
	}
	else
	{
		if ( anim_progress > 0.f )
			anim_progress -= m_frequence * g_pGlobals->frametime;
		else if ( checkmark_progress > 0.f )
			checkmark_progress -= m_frequence * g_pGlobals->frametime;
	}

	if ( anim_progress < 0.f )
		anim_progress = 0.f;
	if ( anim_progress > 1.f )
		anim_progress = 1.f;

	if ( checkmark_progress < 0.f )
		checkmark_progress = 0.f;
	if ( checkmark_progress > 1.f )
		checkmark_progress = 1.f;
}

void c_checkbox::update_pos_individual( LONG xdrag, LONG ydrag )
{

}