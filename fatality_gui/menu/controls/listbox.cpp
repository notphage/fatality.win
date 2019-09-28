#include "../external_include.h"
#include <cctype>
#include <algorithm>

c_listbox::c_listbox( std::string name, bounds pbounds, std::string option, std::vector<std::string>& items, c_subtab* parent )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_scrollbounds = bounds( pbounds.x + pbounds.w - 16, pbounds.y + 19, 15, pbounds.h - 21 );
	this->m_value = option;
	this->m_items = &items;
	this->m_parent = parent;
	this->m_maxitems = floor( ( pbounds.h - 21.f ) / 16.f );
	this->filter = nullptr;
}

c_listbox::c_listbox( std::string name, bounds pbounds, std::string option, std::vector<std::string>& items, std::string& filter, c_subtab* parent )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_items = &items;
	this->m_parent = parent;
	this->m_maxitems = floor( ( pbounds.h - 21.f ) / 16.f );
	this->filter = &filter;
}

bool contains( const std::string &word, const std::string &sentence )
{
	if ( word == "" || sentence == "" )
		return true;

	return sentence.find( word ) != std::string::npos;
}

std::string to_lower( std::string str )
{
	std::transform( str.begin(), str.end(), str.begin(), ( int( *)( int ) )std::tolower );

	return str;
}

void c_listbox::render()
{
	m_scrollbounds = bounds( m_bounds.x + m_bounds.w - 16, m_bounds.y + 19, 15, m_bounds.h - 21 );

	render::get().bordered_box( m_bounds.x, m_bounds.y, m_bounds.w, m_bounds.h,
								m_hovered ? D3DCOLOR_RGBA( 133, 133, 144, ( int )( 220 * hover_progress ) ) : D3DCOLOR_RGBA( 81, 81, 92, 190 ) );

	render::get().bordered_box( m_bounds.x + 1, m_bounds.y + 1,
								m_bounds.w - 2, m_bounds.h - 2,
								D3DCOLOR_RGBA( 30, 32, 38, 190 ) );

	render::get().filled_box( m_bounds.x + 2, m_bounds.y + 19, m_bounds.w - 4, m_bounds.h - 21, D3DCOLOR_RGBA( 37, 39, 46, 255 ) );

	render::get().filled_box( m_bounds.x + 1, m_bounds.y + 1, m_bounds.w - 2, 18, D3DCOLOR_RGBA( 42, 44, 51, 255 ) );

	_u( name, m_name );
	render::get().text( Vector2D( m_bounds.x + ( m_bounds.w / 2 ), m_bounds.y + 10 - ( render::get().get_text_size( name, fonts::controlfont ).y / 2 ) ),
						name, Color( 180, 180, 187, 250 ), fonts::controlfont, c_font::centered_x );

	handle_scrollbar();

	get_items();

	_u( key, m_value );
	if ( !temp.empty() )
	{
		int items_to_draw = ( m_bounds.h - 21 ) / 16;
		int drawnItems = 0;
		for ( int i = m_scrollpos; ( i < temp.size() && drawnItems < items_to_draw ); i++ )
		{
			Color text_color;

			if ( temp[ i ].second == g_cfg[ key ]->get<int>() )
				text_color = Color( 20, 179, 178, 255 );
			else if ( c_menu::get().is_hovering( bounds( m_bounds.x, m_bounds.y + 19 + drawnItems * 16, m_bounds.w - 18, 16 ) ) )
				text_color = Color( 0, 159, 158, 255 );
			else
				text_color = Color( 107, 109, 120, 250 );

			if ( c_menu::get().is_pressed( bounds( m_bounds.x, m_bounds.y + 19 + drawnItems * 16, m_bounds.w - 18, 16 ) ) )
				g_cfg[ key ]->set( temp[ i ].second );

			std::string temp_s = temp[ i ].first;
			_u( first, temp_s );
			render::get().text( Vector2D( m_bounds.x + 8, m_bounds.y + 19 + drawnItems * 16 ),
								first, text_color, fonts::controlfont, c_font::left_aligned );
			drawnItems++;
		}

		float size_ratio = float( items_to_draw ) / float( temp.size() );
		if ( size_ratio > 1.f ) size_ratio = 1.f;
		float pos_ratio = float( m_scrollpos ) / float( temp.size() );
		if ( pos_ratio > 1.f ) pos_ratio = 1.f;

		size_ratio *= m_scrollbounds.h;
		pos_ratio *= m_scrollbounds.h;

		render::get().filled_box( m_scrollbounds.x, m_scrollbounds.y + pos_ratio, 14, size_ratio, D3DCOLOR_RGBA( 62, 64, 81, 255 ) );
	}
}

void c_listbox::handle_scrollbar()
{
	int items_to_draw = ( m_bounds.h - 21 ) / 16;

	if ( c_menu::get().is_holding( m_scrollbounds ) && !m_scrolling )
		m_scrolling = true;

	if ( m_scrolling && !c_menu::get().mouse_is_holding() )
		m_scrolling = false;

	if ( m_scrolling )
	{
		float ratio = float( items_to_draw ) / float( temp.size() );
		POINT m = c_menu::get().get_mouse_pos();
		m.y -= m_scrollbounds.y;

		float size_ratio = float( items_to_draw ) / float( temp.size() );
		size_ratio *= m_scrollbounds.h;
		float height_delta = m.y + size_ratio - m_scrollbounds.h;
		if ( height_delta > 0 )
			m.y -= height_delta;

		float pos_ratio = float( m.y ) / float( m_scrollbounds.h );
		m_scrollpos = pos_ratio * temp.size();
		if ( m_scrollpos < 0 )
			m_scrollpos = 0;
	}
}

void c_listbox::update()
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
}

bool c_listbox::should_show( std::string xd )
{
	for ( auto lower_string : lower_items )
	{
		if ( lower_string.find( to_lower( xd ) ) )
			return true;
	}
	return false;
}

void c_listbox::get_items()
{
	if ( lower_items.empty() || lower_items.size() != m_items->size() )
	{
		lower_items.clear();
		for ( int i = 0; i < m_items->size(); i++ )
		{
			std::string temp_s = ( *m_items )[ i ];
			_u( item, temp_s );
			lower_items.push_back( _w2( to_lower( item ) ) );
		}
	}

	if ( !filter && temp.empty() || temp.size() != m_items->size() && ( !filter || filter->empty() ) )
	{
		temp.clear();
		for ( int i = 0; i < m_items->size(); i++ )
		{
			temp.emplace_back( ( *m_items )[ i ], i );
		}
	}

	if ( !filter )
		return;

	if ( prevfilter == *filter )
		return;

	prevfilter = *filter;

	const auto l_filter = to_lower( *filter );



	temp.clear();
	for ( int i = 0; i < m_items->size(); i++ )
	{
		if ( filter != nullptr )
		{
			std::string temp_s = lower_items[ i ];
			_u( item, temp_s );
			if ( !( std::string( item ).find( l_filter ) != std::string::npos ) )
				continue;
		}

		temp.emplace_back( m_items->at( i ), i );
	}
}

void c_listbox::update_pos_individual( LONG xdrag, LONG ydrag )
{

}