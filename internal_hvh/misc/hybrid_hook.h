#pragma once

#include <mutex>

static std::unordered_map<uint32_t, uint32_t> originals = {};
static std::once_flag initialzed = {}, destructed = {};

class c_vehhook
{
public:
	explicit c_vehhook( void* ent )
	{
		vtable.reset( *reinterpret_cast< uint32_t** >( ent ) );
		std::call_once( initialzed, []() -> void
		{
			AddVectoredExceptionHandler( 1, handler );
		} );
	}

	~c_vehhook()
	{
		for ( const auto& org : vtable_overrides )
		{
			for ( auto i = 0; vtable[ i ]; i++ )
			{
				if ( vtable[ i ] == org )
				{
					++vtable[ i ];
					break;
				}
			}

			originals.erase( org );
		}

		vtable.release();

		std::call_once( destructed, []() -> void
		{
			RemoveVectoredExceptionHandler( handler );
		} );
	}

	template<typename orig, typename hook>
	orig apply( uint32_t index, hook func )
	{
		const auto original = vtable[ index ];
		const auto breakpoint = original - 1;

		vtable_overrides.push_back( breakpoint );
		originals.insert_or_assign( breakpoint, reinterpret_cast< uint32_t >( func ) );

		patch_pointer( &vtable[ index ], breakpoint );
		return reinterpret_cast< orig >( original );
	}
private:
	static LONG WINAPI handler( EXCEPTION_POINTERS* info )
	{
		switch ( info->ExceptionRecord->ExceptionCode )
		{
			case EXCEPTION_BREAKPOINT:
			{
				const auto mapping = originals.find( info->ContextRecord->Eip );
				if ( mapping != originals.end() )
				{
					info->ContextRecord->Eip = mapping->second;
					return EXCEPTION_CONTINUE_EXECUTION;
				}
			}
			default:
				return EXCEPTION_CONTINUE_SEARCH;
		}
	}

	static void patch_pointer( uint32_t* original, uint32_t hook )
	{
		DWORD old;
		VirtualProtect( original, sizeof uint32_t, PAGE_READWRITE, &old );
		*original = hook;
		VirtualProtect( original, sizeof uint32_t, old, &old );
	}

	std::unique_ptr<uint32_t[]> vtable;
	std::vector<uint32_t> vtable_overrides;
};

class c_hook
{
public:
	explicit c_hook( void* ent )
	{
		base = reinterpret_cast< uintptr_t** >( ent );
		original = *base;

		const auto l = length();
		current = std::make_unique<uint32_t[]>( l + 1 );
		std::memcpy( &current.get()[ 1 ], original, l * sizeof uint32_t );
		current.get()[ 0 ] = static_cast< uintptr_t >( original[ -1 ] );

		patch_pointer( current.get() );
	}

	~c_hook()
	{
		patch_pointer( original );
	}

	template<typename function, typename original_function>
	function apply( uint32_t index, original_function func )
	{
		auto old = original[ index ];
		current.get()[ index + 1 ] = reinterpret_cast< uintptr_t >( func );
		return reinterpret_cast< function >( old );
	}
private:
	uint32_t length() const
	{
		uint32_t index = 0;
		const auto vmt = original;

		for ( index = 0; vmt[ index ]; index++ )
			if ( IS_INTRESOURCE( vmt[ index ] ) )
				break;

		return index;
	}

	void patch_pointer( uintptr_t* replacement ) const
	{
		if ( !base )
			return;

		DWORD old;
		VirtualProtect( base, sizeof uintptr_t, PAGE_READWRITE, &old );
		replacement == original ? *base = replacement : *base = &replacement[ 1 ];
		VirtualProtect( base, sizeof uintptr_t, old, &old );
	}

	std::uintptr_t** base;
	std::uintptr_t* original;
	std::unique_ptr<uint32_t[]> current;
};

class c_bphook
{
public:
	explicit c_bphook( void* ent )
	{
		base = reinterpret_cast< uintptr_t** >( ent );
	}

	template<typename function, typename original_function>
	function apply( uint32_t index, original_function func )
	{
		auto old_func = base[ index ];
		DWORD old;
		VirtualProtect( &base[ index ], sizeof uintptr_t, PAGE_EXECUTE_READWRITE, &old );
		base[ index ] = reinterpret_cast< uintptr_t* >( func );
		VirtualProtect( &base[ index ], sizeof uintptr_t, old, &old );
		return reinterpret_cast< function >( old_func );
	}
private:

	std::uintptr_t** base;
};