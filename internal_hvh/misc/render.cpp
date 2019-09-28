#include <fstream>
#include <menu/external_include.h>
#include "../resource.h"


bool file_exists( const char *fileName )
{
	std::ifstream infile( fileName );
	return infile.good();
}

namespace fonts
{
	std::shared_ptr<c_font> esp = std::make_shared<c_font>( "Verdana", 8, 400 );
	std::shared_ptr<c_font> esp_small = std::make_shared<c_font>( "Smallest Pixel-7", 9, 0 );
	std::shared_ptr<c_font> lby = std::make_shared<c_font>( "Verdana", 11, 400 );

	std::shared_ptr<c_font> controlfont = std::make_shared<c_font>( "Verdana", 10, 700 );
	std::shared_ptr<c_font> tabfont = std::make_shared<c_font>( "Tahoma", 10, 400 );
	std::shared_ptr<c_font> tabfontthicc = std::make_shared<c_font>( "Tahoma", 10, 800 );
	std::shared_ptr<c_font> subtabfont = std::make_shared<c_font>( "Verdana", 10, 800 );
	std::shared_ptr<c_font> smallfont = std::make_shared<c_font>( "Tahoma", 6, 400 );
	std::shared_ptr<c_font> keybindfont = std::make_shared<c_font>( "Verdana", 6, 400 );
}

static void AddFont( std::string name )
{
	if ( file_exists( name.c_str() ) ) return;

	// We miss error handling on most of them - be careful!
	_( BINARY, "BINARY" );
	const auto res = FindResourceA( g_module, MAKEINTRESOURCEA( IDR_BINARY1 ), BINARY );
	if ( !res ) return;
	const auto res_handle = LoadResource( g_module, res );
	if ( !res_handle ) return;
	const auto res_data = static_cast< char* >( LockResource( res_handle ) );
	const auto res_size = SizeofResource( g_module, res );

	std::ofstream stream;
	stream.open( name, std::ios::binary );
	stream.write( res_data, res_size );
	stream.close();

	AddFontResourceExA( name.c_str(), FR_PRIVATE, NULL );
}

void render::init( IDirect3DDevice9* dev )
{
	if ( m_init )
		return;

	_( smallest, "smallest_pixel - 7.ttf" );

	AddFont( smallest );

	fonts.push_back( fonts::esp );
	fonts.push_back( fonts::esp_small );
	fonts.push_back( fonts::lby );

	fonts.push_back( fonts::controlfont );
	fonts.push_back( fonts::tabfont );
	fonts.push_back( fonts::tabfontthicc );
	fonts.push_back( fonts::subtabfont );
	fonts.push_back( fonts::smallfont );
	fonts.push_back( fonts::keybindfont );

	// Init fonts
	init_device_objects( dev );
	m_init = true;
}

void render::limit( Vector2D pos, Vector2D size ) const
{
	RECT rec;
	rec.left = static_cast<LONG>( pos.x );
	rec.top = static_cast<LONG>( pos.y );
	rec.right = static_cast<LONG>( pos.x + size.x );
	rec.bottom = static_cast<LONG>( pos.y + size.y );

	dev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
	dev->SetScissorRect( &rec );
}

void render::reset_limit() const
{
	dev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
}

void render::line( Vector2D from, Vector2D to, Color color ) const
{
	const auto col = color.direct();

	vertex vert[ 2 ] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
	{ to.x, to.y, 0.0f, 1.0f, col }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_LINELIST, 1, &vert, sizeof vertex );
}

void render::rect( Vector2D from, Vector2D size, Color color ) const
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[ 5 ] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
	{ to.x, from.y, 0.0f, 1.0f, col },
	{ to.x, to.y, 0.0f, 1.0f, col },
	{ from.x, to.y, 0.0f, 1.0f, col },
	{ from.x, from.y, 0.0f, 1.0f, col }
	};

	dev->SetTexture( 0, nullptr );
	dev->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, false );
	dev->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, &vert, sizeof vertex );
	dev->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, true );
}

void render::rect_linear_gradient( Vector2D from, Vector2D size, Color color1, Color color2, bool horizontal ) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[ 5 ] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col2 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ to.x, to.y, 0.0f, 1.0f, col2 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetTexture( 0, nullptr );
	dev->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, false );
	dev->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, &vert, sizeof vertex );
	dev->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, true );
}

void render::rect_filled( Vector2D from, Vector2D size, Color color ) const
{
	const auto to = from + size;
	const auto col = color.direct();

	vertex vert[ 4 ] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col },
	{ to.x, from.y, 0.0f, 1.0f, col },
	{ from.x, to.y, 0.0f, 1.0f, col },
	{ to.x, to.y, 0.0f, 1.0f, col }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &vert, sizeof vertex );
}

void render::rect_filled_linear_gradient( Vector2D from, Vector2D size, Color color1, Color color2, bool horizontal ) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[ 4 ] =
	{
		{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, horizontal ? col2 : col1 },
	{ from.x, to.y, 0.0f, 1.0f, horizontal ? col1 : col2 },
	{ to.x, to.y, 0.0f, 1.0f, col2 }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, &vert, sizeof vertex );
}

void render::rect_filled_radial_gradient( Vector2D from, Vector2D size, Color color1, Color color2 )
{
	const auto center = from + size / 2.0f;
	const auto radius = ( center - from ).Length();

	D3DVIEWPORT9 new_port;
	new_port.X = static_cast< uint32_t >( from.x );
	new_port.Y = static_cast< uint32_t >( from.y );
	new_port.Width = static_cast< uint32_t >( size.x );
	new_port.Height = static_cast< uint32_t >( size.y );

	dev->SetViewport( &new_port );
	circle_filled_radial_gradient( center, radius, color1, color2 );
	dev->SetViewport( &port );
}

void render::rect_filled_diamond_gradient( Vector2D from, Vector2D size, Color color1, Color color2 ) const
{
	const auto to = from + size;
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();

	vertex vert[ 6 ] =
	{
		{ ( from.x + to.x ) / 2.0f, ( from.y + to.y ) / 2.0f, 0.0f, 1.0f, col2 },
	{ from.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, from.y, 0.0f, 1.0f, col1 },
	{ to.x, to.y, 0.0f, 1.0f, col1 },
	{ from.x, to.y, 0.0f, 1.0f, col1 },
	{ from.x, from.y, 0.0f, 1.0f, col1 }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 5, &vert, sizeof vertex );
}

void render::triangle( Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color ) const
{
	const auto col = color.direct();

	vertex vert[ 4 ] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col },
	{ pos1.x, pos1.y, 0.0f, 1.0f, col }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_LINESTRIP, 3, &vert, sizeof vertex );
}

void render::triangle_filled( Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color ) const
{
	const auto col = color.direct();

	vertex vert[ 4 ] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vert, sizeof vertex );
}

void render::triangle_filled_linear_gradient( Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color1, Color color2, Color color3 ) const
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	const auto col3 = color3.direct();

	vertex vert[ 4 ] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, col1 },
	{ pos2.x, pos2.y, 0.0f, 1.0f, col2 },
	{ pos3.x, pos3.y, 0.0f, 1.0f, col3 }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vert, sizeof vertex );
}

void render::circle( Vector2D center, float radius, Color color )
{
	const auto col = color.direct();
	build_lookup_table();

	vertex vert[ points + 1 ] = {};

	for ( auto i = 0; i <= points; i++ )
		vert[ i ] =
	{
		center.x + radius * lookup_table[ i ].x,
		center.y - radius * lookup_table[ i ].y,
		0.0f,
		1.0f,
		col
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_LINESTRIP, points, &vert, sizeof vertex );
}

void render::circle_filled( Vector2D center, float radius, Color color )
{
	const auto col = color.direct();
	build_lookup_table();

	vertex vert[ points + 1 ] = {};

	for ( auto i = 0; i <= points; i++ )
		vert[ i ] =
	{
		center.x + radius * lookup_table[ i ].x,
		center.y - radius * lookup_table[ i ].y,
		0.0f,
		1.0f,
		col
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, points, &vert, sizeof vertex );
}

void render::circle_filled_radial_gradient( Vector2D center, float radius, Color color1, Color color2 )
{
	const auto col1 = color1.direct();
	const auto col2 = color2.direct();
	build_lookup_table();

	vertex vert[ points + 2 ] = {};

	for ( auto i = 1; i <= points; i++ )
		vert[ i ] =
	{
		center.x + radius * lookup_table[ i ].x,
		center.y - radius * lookup_table[ i ].y,
		0.0f,
		1.0f,
		col1
	};

	vert[ 0 ] = { center.x, center.y, 0.0f, 1.0f, col2 };
	vert[ points + 1 ] = vert[ 1 ];

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, points, &vert, sizeof vertex );
}

void render::circle_filled_rainbow( Vector2D center, float radius, bool rotate ) const
{
	static float base_hue = 0.f;
	if ( rotate )
	{
		if ( base_hue >= 1.f )
			base_hue = 0.f;

		base_hue += ( 1.f / 5.f ) * g_pGlobals->frametime;
	}

	vertex vert[ 128 + 2 ] = {};

	float new_hue = base_hue;
	for ( auto i = 1; i <= 128; i++ )
	{
		if ( new_hue >= 1.f )
			new_hue = 0.f;

		new_hue += 1.f / 128.f;
		new_hue = std::clamp( new_hue, 0.f, 1.f );

		auto temp = util::hsb_to_rgbcol( new_hue, 1.f, 1.f );
		auto col = D3DCOLOR_RGBA( temp.r(), temp.g(), temp.b(), 100 );

		vert[ i ] =
		{
			center.x + radius * cos( D3DX_PI * ( i / ( 128 / 2.0f ) ) ),
			center.y + radius * sin( D3DX_PI * ( i / ( 128 / 2.0f ) ) ),
			0.0f,
			1.0f,
			col
		};
	}

	vert[ 0 ] = { center.x, center.y, 0.0f, 1.0f, D3DCOLOR_RGBA( 100, 100, 100, 10 ) };
	vert[ 128 + 1 ] = vert[ 1 ];

	dev->SetTexture( 0, NULL );
	dev->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 128, &vert, sizeof( vertex ) );
}

void render::filled_triangle( POINT pos1, POINT pos2, POINT pos3, Color color ) const
{
	vertex vert[ 3 ] =
	{
		{ pos1.x, pos1.y, 0.0f, 1.0f, color.direct() },
	{ pos2.x, pos2.y, 0.0f, 1.0f, color.direct() },
	{ pos3.x, pos3.y, 0.0f, 1.0f, color.direct() }
	};

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, &vert, sizeof( vertex ) );
}

void render::filled_box( const int x, const int y, const int w, const int h, const Color color ) const
{
	vertex vertices2[ 4 ] = { { x, y + h, 0.0f, 1.0f, color.direct() },{ x,  y, 0.0f, 1.0f, color.direct() },{ ( x + w ),  ( y + h ), 0.0f, 1.0f, color.direct() },{ ( x + w ), y, 0.0f, 1.0f, color.direct() } };

	dev->SetTexture( 0, nullptr );
	dev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices2, sizeof( vertex ) );
}

void render::filled_box_outlined( const int x, const int y, const int w, const int h, const Color color, const Color outline, const int thickness ) const
{
	filled_box( x, y, w, h, color );
	bordered_box( x, y, w, h, outline, thickness );
}

void render::bordered_box( const int x, const int y, const int w, const int h, const Color color, const int thickness ) const
{
	filled_box( x, y, w, thickness, color );
	filled_box( x, y, thickness, h, color );
	filled_box( x + w - thickness, y, thickness, h, color );
	filled_box( x, y + h - thickness, w, thickness, color );
}

void render::bordered_box_outlined( const int x, const int y, const int w, const int h, const Color color, const Color outline, const int thickness ) const
{
	bordered_box( x, y, w, h, outline, thickness );
	bordered_box( x + thickness, y + thickness, w - ( thickness * 2 ), h - ( thickness * 2 ), color, thickness );
	bordered_box( x + ( thickness * 2 ), y + ( thickness * 2 ), w - ( thickness * 4 ), h - ( thickness * 4 ), outline, thickness );
}

void render::text( Vector2D pos, std::wstring string, Color color, std::shared_ptr<c_font>& font, uint8_t flags )
{
	font->draw_string( std::roundf( pos.x ), std::roundf( pos.y ), color.direct(), string.c_str(), flags );
}

void render::text( Vector2D pos, std::string string, Color color, std::shared_ptr<c_font>& font, uint8_t flags ) const
{
	text( pos, std::wstring( string.begin(), string.end() ), color, font, flags );
}

void render::text_radial( Vector2D pos, std::wstring string, Color color1, Color color2, std::shared_ptr<c_font>& font, uint8_t flags )
{
	font->draw_radial( std::roundf( pos.x ), std::roundf( pos.y ), color1.direct(), color2.direct(), string.c_str(), flags );
}

void render::text_radial( Vector2D pos, std::string string, Color color1, Color color2, std::shared_ptr<c_font>& font, uint8_t flags ) const
{
	text_radial( pos, std::wstring( string.begin(), string.end() ), color1, color2, font, flags );
}

Vector2D render::get_text_size( std::wstring string, std::shared_ptr<c_font>& font )
{
	SIZE size;
	font->get_text_extent( string.c_str(), &size );
	return Vector2D( static_cast< float >( size.cx ), static_cast< float >( size.cy ) );
}

Vector2D render::get_text_size( std::string string, std::shared_ptr<c_font>& font ) const
{
	return get_text_size( std::wstring( string.begin(), string.end() ), font );
}

void render::scale( Vector2D& vec ) const
{
	vec.x = ( vec.x + 1.f ) * port.Width / 2.f;
	vec.y = ( -vec.y + 1.f ) * port.Height / 2.f;
}

Vector2D render::get_center() const
{
	return Vector2D( static_cast< float >( port.X + port.Width / 2.f ), static_cast< float >( port.Y + port.Height / 2.f ) );
}

D3DVIEWPORT9 render::get_dimensions() const
{
	return port;
}

void render::init_device_objects( IDirect3DDevice9* dev )
{
	if ( !dev )
		std::runtime_error( "Invalid device provided" );

	this->dev = dev;
	dev->GetViewport( &port );

	for ( auto& font : fonts )
	{
		font->init_device_objects( dev );
		font->restore_device_objects();
	}
}

void render::invalidate_device_objects()
{
	dev = nullptr;

	for ( auto& font : fonts )
		font->invalidate_device_objects();
}

void render::setup_render_state() const
{
	dev->SetVertexShader( nullptr );
	dev->SetPixelShader( nullptr );
	dev->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	dev->SetRenderState( D3DRS_LIGHTING, false );
	dev->SetRenderState( D3DRS_FOGENABLE, false );
	dev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	dev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	dev->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	dev->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
	dev->SetRenderState( D3DRS_ZWRITEENABLE, false );
	dev->SetRenderState( D3DRS_STENCILENABLE, false );

	dev->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, true );
	dev->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, true );

	dev->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	dev->SetRenderState( D3DRS_ALPHATESTENABLE, false );
	dev->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, true );
	dev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	dev->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA );
	dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	dev->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );

	dev->SetRenderState( D3DRS_SRGBWRITEENABLE, false );
	dev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );

	dev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	dev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	dev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	dev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	dev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
}

void render::build_lookup_table()
{
	if ( !lookup_table.empty() )
		return;

	for ( auto i = 0; i <= points; i++ )
		lookup_table.emplace_back(
			std::cos( 2.f * D3DX_PI * ( i / static_cast< float >( points ) ) ),
			std::sin( 2.f * D3DX_PI * ( i / static_cast< float >( points ) ) )
		);
}