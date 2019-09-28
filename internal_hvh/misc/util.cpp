#include "../include_cheat.h"

uintptr_t util::get_sig( std::string module_name, std::string pattern )
{
	static auto pattern_to_byte = []( const char* pattern )
	{
		auto bytes = std::vector<int>{};
		auto start = const_cast< char* >( pattern );
		auto end = const_cast< char* >( pattern ) + strlen( pattern );

		for ( auto current = start; current < end; ++current )
		{
			if ( *current == '?' )
			{
				++current;
				if ( *current == '?' )
					++current;
				bytes.push_back( -1 );
			}
			else
			{
				bytes.push_back( strtoul( current, &current, 16 ) );
			}
		}
		return bytes;
	};

	const auto module = GetModuleHandleA( module_name.c_str() );

	if ( module )
	{
		const auto dosHeader = PIMAGE_DOS_HEADER( module );
		const auto ntHeaders = PIMAGE_NT_HEADERS( reinterpret_cast< std::uint8_t* >( module ) + dosHeader->e_lfanew );

		const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte( pattern.c_str() );
		const auto scanBytes = reinterpret_cast< std::uint8_t* >( module );

		const auto s = patternBytes.size();
		const auto d = patternBytes.data();

		for ( auto i = 0ul; i < sizeOfImage - s; ++i )
		{
			auto found = true;
			for ( auto j = 0ul; j < s; ++j )
			{
				if ( scanBytes[ i + j ] != d[ j ] && d[ j ] != -1 )
				{
					found = false;
					break;
				}
			}

			if ( found )
				return uintptr_t( &scanBytes[ i ] );
		}
	}

	return 0;
}

HMODULE util::get_module( const std::string& szmodule )
{
	return GetModuleHandleA( szmodule.c_str() );
}

float util::get_hue( D3DCOLOR col )
{
	float r = ( ( ( col ) & 0x00ff0000 ) >> 16 );
	float g = ( ( ( col ) & 0x0000ff00 ) >> 8 );
	float b = ( ( col ) & 0x000000ff );

	float mx = std::max( r, std::max( g, b ) );
	float mn = std::min( r, std::min( g, b ) );
	if ( mx == mn )
		return 0.f;

	float delta = mx - mn;

	float hue = 0.f;
	if ( mx == r )
		hue = ( g - b ) / delta;
	else if ( mx == g )
		hue = 2.f + ( b - r ) / delta;
	else
		hue = 4.f + ( r - g ) / delta;

	hue *= 60.f;
	if ( hue < 0.f )
		hue += 360.f;

	return hue / 360.f;
}

float util::get_saturation( D3DCOLOR col )
{
	float r = ( ( ( col ) & 0x00ff0000 ) >> 16 );
	float g = ( ( ( col ) & 0x0000ff00 ) >> 8 );
	float b = ( ( col ) & 0x000000ff );

	float mx = std::max( r, std::max( g, b ) );
	float mn = std::min( r, std::min( g, b ) );

	float delta = mx - mn;

	if ( mx == 0.f )
		return delta;

	return delta / mx;
}

float util::get_brightness( D3DCOLOR col )
{
	float r = ( ( ( col ) & 0x00ff0000 ) >> 16 );
	float g = ( ( ( col ) & 0x0000ff00 ) >> 8 );
	float b = ( ( col ) & 0x000000ff );

	return std::max( r, std::max( g, b ) );
}

float util::get_fraction( float val, float min, float max )
{
	float ratio = 1.f / ( max - min );
	return ratio * ( val - min );
}

D3DCOLOR util::hsb_to_rgb( float hue, float saturation, float brightness, int alpha )
{
	hue = clamp( hue, 0.f, 1.f );
	saturation = clamp( saturation, 0.f, 1.f );
	brightness = clamp( brightness, 0.f, 1.f );

	float h = ( hue == 1.f ) ? 0.f : ( hue * 6.f );
	float f = h - static_cast< int >( h );
	float p = brightness * ( 1.f - saturation );
	float q = brightness * ( 1.f - saturation * f );
	float t = brightness * ( 1.f - ( saturation * ( 1.f - f ) ) );

	if ( h < 1.f )
		return D3DCOLOR_RGBA( ( int )( brightness * 255 ), ( int )( t * 255 ), ( int )( p * 255 ), alpha );
	else if ( h < 2.f )
		return D3DCOLOR_RGBA( ( int )( q * 255 ), ( int )( brightness * 255 ), ( int )( p * 255 ), alpha );
	else if ( h < 3.f )
		return D3DCOLOR_RGBA( ( int )( p * 255 ), ( int )( brightness * 255 ), ( int )( t * 255 ), alpha );
	else if ( h < 4 )
		return D3DCOLOR_RGBA( ( int )( p * 255 ), ( int )( q * 255 ), ( int )( brightness * 255 ), alpha );
	else if ( h < 5 )
		return D3DCOLOR_RGBA( ( int )( t * 255 ), ( int )( p * 255 ), ( int )( brightness * 255 ), alpha );
	else
		return D3DCOLOR_RGBA( ( int )( brightness * 255 ), ( int )( p * 255 ), ( int )( q * 255 ), alpha );
}

Color util::hsb_to_rgbcol( float _h, float s, float b, int alpha )
{
	float h = _h == 1.0f ? 0 : _h * 6.0f;
	float f = h - ( int )h;
	float p = b * ( 1.0f - s );
	float q = b * ( 1.0f - s * f );
	float t = b * ( 1.0f - ( s * ( 1.0f - f ) ) );

	if ( h < 1 )
	{
		return Color(
			( unsigned char )( b * 255 ),
			( unsigned char )( t * 255 ),
			( unsigned char )( p * 255 )
		);
	}
	if ( h < 2 )
	{
		return Color(
			( unsigned char )( q * 255 ),
			( unsigned char )( b * 255 ),
			( unsigned char )( p * 255 )
		);
	}
	if ( h < 3 )
	{
		return Color(
			( unsigned char )( p * 255 ),
			( unsigned char )( b * 255 ),
			( unsigned char )( t * 255 )
		);
	}
	if ( h < 4 )
	{
		return Color(
			( unsigned char )( p * 255 ),
			( unsigned char )( q * 255 ),
			( unsigned char )( b * 255 )
		);
	}
	if ( h < 5 )
	{
		return Color(
			( unsigned char )( t * 255 ),
			( unsigned char )( p * 255 ),
			( unsigned char )( b * 255 )
		);
	}
	return Color(
		( unsigned char )( b * 255 ),
		( unsigned char )( p * 255 ),
		( unsigned char )( q * 255 )
	);

}

void util::print_dev_console( bool printincon, const Color& color, const char * format, ... )
{
	using print_dev_console_fn = void( __stdcall* )( const Color&, char const* );
	static auto print_dev_console = reinterpret_cast< print_dev_console_fn >( offsets::print_dev_console );

	auto temp = std::vector<char>{};
	auto length = std::size_t{ 63 };
	va_list args;
	while ( temp.size() <= length )
	{
		temp.resize( length + 1 );
		va_start( args, format );
		const auto status = std::vsnprintf( temp.data(), temp.size(), format, args );
		va_end( args );

		length = static_cast< std::size_t >( status );
	}
	std::string out{ temp.data(), length };

	print_dev_console( Color( 255, 255, 255, 255 ), out.c_str() );

	if ( printincon )
		g_pCVar->ConsoleColorPrintf( color, out.c_str() );
}

uintptr_t util::GetItemSchema()
{
	static auto fnGetItemSchema = reinterpret_cast< uintptr_t( __stdcall* )( ) >( sig( "client.dll", "A1 ? ? ? ? 85 C0 75 53" ) );
	return fnGetItemSchema();
}

void util::DestroyEconItem( CEconItem* item )
{
	static auto fn_CEconItem = reinterpret_cast< int( __thiscall* )( void*, bool ) >( sig( "client.dll", "55 8B EC 56 8B F1 8B 4E 18 C7 06 ? ? ? ? C7 46 ? ? ? ? ? 85 C9 74 05" ) );
	fn_CEconItem( item, true );
}

CEconItem* util::CreateEconItem()
{
	static auto fnCreateSharedObjectSubclass_EconItem_ = reinterpret_cast< CEconItem*( __stdcall* )( ) >( *reinterpret_cast< uintptr_t* >( sig( "client.dll", "C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4" ) + 3 ) );
	return fnCreateSharedObjectSubclass_EconItem_();
}