#include "../external_include.h"
#include "../picture_data.h"

c_window::c_window( std::string name, bounds pbounds, bool tabs )
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_hastabs = tabs;
	this->m_tabarea = bounds( pbounds.x + 1, pbounds.y + 21, 120, pbounds.h - 22 );
	this->m_subtabarea = bounds( pbounds.x + 121, pbounds.y + 21, 201, pbounds.h - 22 );
	this->m_area = bounds( pbounds.x + 322, pbounds.y + 21, pbounds.w - 324, pbounds.h - 22 );
}

void c_window::render()
{
	//do here cuz gay
	handle_movement();

	int center = m_bounds.x + m_bounds.w / 2;

	render::get().filled_box( 0, 0, render::get().get_dimensions().Width, render::get().get_dimensions().Height, D3DCOLOR_RGBA( 10, 10, 10, 60 ) );

	render::get().filled_box( m_bounds.x, m_bounds.y, m_bounds.w, m_bounds.h, D3DCOLOR_RGBA( 50, 53, 68, 255 ) );
	render::get().bordered_box( m_bounds.x, m_bounds.y, m_bounds.w, m_bounds.h, D3DCOLOR_RGBA( 0, 0, 0, 255 ), 1 );

	render::get().filled_box( m_tabarea.x, m_tabarea.y, m_tabarea.w, m_tabarea.h, D3DCOLOR_RGBA( 69, 74, 102, 255 ) );

	render::get().line( Vector2D( m_bounds.x + 1, m_bounds.y + 20 ), Vector2D( m_bounds.x + m_bounds.w - 1, m_bounds.y + 20 ), D3DCOLOR_RGBA( 37, 39, 46, 255 ) );

	render::get().filled_box( m_subtabarea.x, m_subtabarea.y, m_subtabarea.w, m_subtabarea.h, D3DCOLOR_RGBA( 37, 39, 46, 255 ) );

	if ( *reinterpret_cast< bool* >( g_module ) )
	{
		_( fatality, "fatality.win beta" );
		render::get().text( Vector2D( m_bounds.x + 8, m_bounds.y + 5 ), fatality, Color( 210, 210, 210, 210 ), fonts::esp );
	}
	else
	{
		_( fatality, "fatality.win" );
		render::get().text( Vector2D( m_bounds.x + 8, m_bounds.y + 5 ), fatality, Color( 210, 210, 210, 210 ), fonts::esp );
	}
	

	if ( img == NULL )
		D3DXCreateTextureFromFileInMemory( render::get().get_device(), &DATA_SMALL, sizeof( DATA_SMALL ), &img );
	if ( sprite == NULL )
		D3DXCreateSprite( render::get().get_device(), &sprite );

	if ( img && sprite )
	{
		D3DXMATRIX world;
		D3DXMATRIX rotation;
		D3DXMATRIX scale;
		D3DXMATRIX translation;
		D3DXMatrixIdentity( &world );

		D3DXMatrixScaling( &scale, 1.f, .6f, 1.f );
		D3DXMatrixRotationYawPitchRoll( &rotation, 0.f, 0.f, 0.f );
		D3DXMatrixTranslation( &translation, 0.f, 0.f, 0.f );
		world = rotation * scale * translation;


		D3DSURFACE_DESC img_info;
		img->GetLevelDesc( 0, &img_info );

		sprite->SetTransform( &world );
		sprite->Begin( D3DXSPRITE_ALPHABLEND );
		sprite->Draw( img, NULL, NULL, &D3DXVECTOR3( m_tabarea.x + ( m_tabarea.w / 2 ) - static_cast< int >( img_info.Width / 2 ) + 1, m_tabarea.y * 1.6666666f + 25.f, 0.f ), D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
		sprite->End();
	}


	if ( img_big == NULL )
		D3DXCreateTextureFromFileInMemory( render::get().get_device(), &DATA_BIG, sizeof( DATA_BIG ), &img_big );
	if ( sprite_big == NULL )
		D3DXCreateSprite( render::get().get_device(), &sprite_big );

	if ( img_big && sprite_big )
	{
		D3DXMATRIX world;
		D3DXMATRIX rotation;
		D3DXMATRIX scale;
		D3DXMATRIX translation;
		D3DXMatrixIdentity( &world );

		D3DXMatrixScaling( &scale, .55f, .6f, 1.f );
		D3DXMatrixRotationYawPitchRoll( &rotation, 0.f, 0.f, 0.f );
		D3DXMatrixTranslation( &translation, 0.f, 0.f, 0.f );
		world = rotation * scale * translation;

		D3DSURFACE_DESC img_info;
		img_big->GetLevelDesc( 0, &img_info );

		D3DVIEWPORT9 backup;
		render::get().get_device()->GetViewport( &backup );
		D3DVIEWPORT9 new_vp;
		new_vp.X = m_area.x;
		new_vp.Y = m_area.y;
		new_vp.Width = m_area.w;
		new_vp.Height = m_area.h;

		render::get().get_device()->SetViewport( &new_vp );

		sprite_big->SetTransform( &world );
		sprite_big->Begin( D3DXSPRITE_ALPHABLEND );
		sprite_big->Draw( img_big, NULL, NULL, &D3DXVECTOR3( m_area.x * 1.8333333f + 16, m_area.y * 1.6666666f + 16, 0.f ), D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
		sprite_big->End();

		render::get().get_device()->SetViewport( &backup );
	}

	for ( auto tab : m_tabs )
	{
		if ( tab->update() )
			m_selected_tab = tab;

		tab->render( ( m_selected_tab == tab ) );
	}
}

void c_window::handle_movement()
{
	if ( c_menu::is_holding( bounds( m_bounds.x, m_bounds.y, m_bounds.w, 24 ) ) && !m_is_dragged )
	{
		m_drag_offset.x = render::get().get_dimensions().Width - c_menu::get().get_mouse_pos().x;
		m_drag_offset.y = render::get().get_dimensions().Height - c_menu::get().get_mouse_pos().y;
		m_is_dragged = true;
	}
	else if ( !c_menu::mouse_is_holding() )
		m_is_dragged = false;

	if ( !m_is_dragged )
		return;

	POINT offset;
	offset.x = render::get().get_dimensions().Width - c_menu::get().get_mouse_pos().x;
	offset.y = render::get().get_dimensions().Height - c_menu::get().get_mouse_pos().y;

	POINT dif;
	dif.x = offset.x - m_drag_offset.x;
	dif.y = offset.y - m_drag_offset.y;

	auto resultx = m_bounds.x - dif.x;
	auto resulty = m_bounds.y - dif.y;

	if ( resultx < 0 || resultx + m_bounds.w > render::get().get_dimensions().Width )
	{
		dif.x = 0;
		resultx = m_bounds.x;
	}

	if ( resulty < 0 || resulty + m_bounds.h > render::get().get_dimensions().Height )
	{
		dif.y = 0;
		resulty = m_bounds.y;
	}

	update_controls( -dif.x, -dif.y );

	m_bounds.x = resultx;
	m_bounds.y = resulty;

	m_tabarea = bounds( m_bounds.x + 1, m_bounds.y + 21, 120, m_bounds.h - 22 );
	m_subtabarea = bounds( m_bounds.x + 121, m_bounds.y + 21, 201, m_bounds.h - 22 );
	m_area = bounds( m_bounds.x + 322, m_bounds.y + 21, m_bounds.w - 324, m_bounds.h - 22 );

	m_drag_offset.x = render::get().get_dimensions().Width - c_menu::get().get_mouse_pos().x;
	m_drag_offset.y = render::get().get_dimensions().Height - c_menu::get().get_mouse_pos().y;
}

void c_window::update_controls( LONG xdrag, LONG ydrag )
{
	for ( auto &tab : m_tabs )
		for ( auto &subtab : tab->m_subtabs )
			for ( auto &control : subtab->m_controls )
			{
				control->update_pos( xdrag, ydrag );
				control->update_pos_individual( xdrag, ydrag );
			}
}


c_groupwindow* c_window::add_groupwindow( std::string name, bounds bound )
{
	//return new c_groupwindow(name, bounds(m_area.x + bound.x, m_area.y + bound.y, bound.w, bound.h));
	return nullptr;
}

void c_window::pushback_groupwindow( c_groupwindow* window )
{
	m_groups.push_back( window );
}

c_tab* c_window::add_tab( std::string name )
{
	return new c_tab( name, m_tabamount++, this );
}

void c_window::pushback_tab( c_tab* tab )
{
	m_tabs.push_back( tab );
}

void c_window::set_active( bool active )
{
	m_active = active;
}
