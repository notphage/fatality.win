#pragma once

class CEconItem;
namespace util
{
	uintptr_t get_sig( std::string module_name, std::string pattern );
	HMODULE get_module( const std::string& szmodule );
	float get_hue( D3DCOLOR col );
	float get_saturation( D3DCOLOR col );
	float get_brightness( D3DCOLOR col );
	float get_fraction( float val, float min, float max );
	D3DCOLOR hsb_to_rgb( float hue, float saturation, float brightness, int alpha = 255 );
	Color hsb_to_rgbcol( float _h, float s, float b, int alpha = 255 );
	void print_dev_console( bool printincon, const Color & color, const char * format, ... );
	template <typename T> T get_vfunc( void *v_table, const int i_index ) { return ( *static_cast< T** >( v_table ) )[ i_index ]; }
	template <typename T> T* get_interface( const HMODULE module, std::string szinterface )
	{
		using create_interface_fn = T * ( *)( const char* szname, int ret );
		_( CreateInterface, "CreateInterface" );
		const create_interface_fn create_interface = reinterpret_cast< create_interface_fn >( GetProcAddress( module, CreateInterface ) );

		return create_interface( szinterface.c_str(), 0 );
	}
	inline uint8_t* find_sig_ext( uint32_t offset, const char* signature, uint32_t range = 0u )
	{
		static auto pattern_to_bytes = []( const char* pattern ) -> std::vector<int>
		{
			auto bytes = std::vector<int32_t>{};
			auto start = const_cast< char* >( pattern );
			auto end = const_cast< char* >( pattern ) + strlen( pattern );

			for ( auto current = start; current < end; ++current )
			{
				if ( *current == '?' )
				{
					current++;

					if ( *current == '?' )
						current++;

					bytes.push_back( -1 );
				}
				else
					bytes.push_back( static_cast< int32_t >( strtoul( current, &current, 0x10 ) ) );
			}

			return bytes;
		};

		const auto scan_bytes = reinterpret_cast< std::uint8_t* >( offset );
		auto pattern_bytes = pattern_to_bytes( signature );
		const auto s = pattern_bytes.size();
		const auto d = pattern_bytes.data();

		for ( auto i = 0ul; i < range - s; ++i )
		{
			auto found = true;

			for ( auto j = 0ul; j < s; ++j )
				if ( scan_bytes[ i + j ] != d[ j ] && d[ j ] != -1 )
				{
					found = false;
					break;
				}

			if ( found )
				return &scan_bytes[ i ];
		}

		return nullptr;
	}

	CEconItem* CreateEconItem();
	uintptr_t GetItemSchema();
	void DestroyEconItem( CEconItem* item );
}

#define MEMEBR_FUNC_ARGS(...) ( this, __VA_ARGS__ ); }
#define FUNCARGS(...) ( __VA_ARGS__ ); }
#define VFUNC( index, func, sig ) auto func { return util::get_vfunc< sig >( this, index ) MEMEBR_FUNC_ARGS
#define MFUNC(func, sig, offset) auto func { return reinterpret_cast< sig >( offset ) MEMEBR_FUNC_ARGS
#define FUNC(func, sig, offset) auto func { return reinterpret_cast< sig >( offset ) FUNCARGS

__forceinline void erase_function( uint8_t* function )
{
	if ( *function == 0xE9 )
	{
		auto pdFollow = ( PDWORD )( function + 1 );
		function = ( ( PBYTE )( *pdFollow + ( DWORD )function + 5 ) );
	}
	else if ( *function == 0xEB )
	{
		auto pbFollow = ( PDWORD )( function + 1 );
		function = ( ( PBYTE )( ( DWORD )*pbFollow + ( DWORD )function + 2 ) );
	}


	static const auto current_process = reinterpret_cast< HANDLE >( -1 );

	_( fn_end, "90 90 90 90 90" );
	const auto end = util::find_sig_ext( reinterpret_cast< uint32_t >( function ), fn_end, 0x2000 );
	size_t bytes = reinterpret_cast< DWORD >( end ) - reinterpret_cast< DWORD >( function ) + 6;

	void* fn = function;
	size_t size = bytes;
	DWORD old;
	VirtualProtect( fn, size, PAGE_EXECUTE_READWRITE, &old );
	fn = function;
	size = bytes;
	memset( fn, 0, size );
	VirtualProtect( fn, size, old, &old );
}

// NOLINTNEXTLINE
#define erase_fn(a) constexpr auto concat(w, __LINE__) = &a;\
    erase_function(reinterpret_cast<uint8_t*>((void*&)concat(w, __LINE__)))
#define erase_end  __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 