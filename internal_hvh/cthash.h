#pragma once

#include <array>

namespace random
{
	constexpr auto time = __TIME__;
	constexpr auto seed = static_cast< unsigned >( time[ 7 ] ) + static_cast< unsigned >( time[ 6 ] ) * 10 + static_cast< unsigned >( time[ 4 ] ) * 60 + static_cast< unsigned >( time[ 3 ] ) * 600 + static_cast< unsigned >( time[ 1 ] ) * 3600 + static_cast< unsigned >( time[ 0 ] ) * 36000;

	template <int n>
	struct gen
	{
	private:
		static constexpr unsigned a = 16807;
		static constexpr unsigned m = 2147483647;

		static constexpr unsigned s = gen<n - 1>::value;
		static constexpr unsigned lo = a * ( s & 0xFFFFu );
		static constexpr unsigned hi = a * ( s >> 16u );
		static constexpr unsigned lo2 = lo + ( ( hi & 0x7FFFu ) << 16u );
		static constexpr unsigned hi2 = hi >> 15u;
		static constexpr unsigned lo3 = lo2 + hi;

	public:
		static constexpr unsigned max = m;
		static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
	};

	template <>
	struct gen<0>
	{
		static constexpr unsigned value = seed;
	};

	template <int n, int m>
	struct _int
	{
		static constexpr auto value = gen<n + 1>::value % m;
	};

	template <int n>
	struct _char
	{
		static const char value = static_cast< char >( 1 + _int<n, 0x7F - 1>::value );
	};
}


template <size_t n, unsigned k>
struct xorstr
{
private:
	constexpr char enc( const char c ) const
	{
		return c ^ key; // NOLINT
	}

public:
	template <size_t... s>
	constexpr __forceinline xorstr( const char* str, std::index_sequence<s...> ) :
		key( random::_char<k>::value ), encrypted{ enc( str[ s ] )... }
	{
	}

	const size_t size = n;
	const char key;
	std::array<char, n + 1> encrypted;
};

#define concat_impl(x, y) x##y
#define concat(x, y) concat_impl(x, y)

#define _i(s) xorstr<sizeof(s) - 1, __COUNTER__>(s, std::make_index_sequence<sizeof(s) - 1>())

#define _w(s) []() noexcept -> std::string { \
    constexpr auto e = _i(s); \
    std::string res; \
    res += e.key; \
    for (auto& j : e.encrypted) \
        res += j; \
    return res; \
}()

// Only use this when there is no other way - this is dangerous!
#define _w2(s) [&]() noexcept -> std::string { \
    const auto key = rand() % 0x7F;\
    std::string res; \
    res += key; \
    for (size_t i = 0; i < (s).length() + 1; i++) \
        res += (s)[i] ^ key; \
    return res; \
}()

#define _rt(n, s) char* (n) = reinterpret_cast<char*>(alloca(((s).size + 1) * sizeof(char))); \
    for (size_t i = 0; i < (s).size; i++) \
        (n)[i] = (s).encrypted[i] ^ (s).key; \
    (n)[(s).size] = '\0'

#define _(n, s) constexpr auto concat(e, __LINE__) = _i(s); \
    _rt(n, concat(e, __LINE__))

#define ___(n, s) constexpr auto concat(g, __LINE__) = _i(s); \
    _rt(n, concat(g, __LINE__))

#define _u(n, s) const auto concat(t, __LINE__) = (s).substr(1); \
    char* (n) = reinterpret_cast<char*>(alloca(((s).length() + 2) * sizeof(char))); \
    for (size_t i = 0; i < (s).length(); i++) \
        (n)[i] = (s)[i + 1] ^ (s)[0]; \
    (n)[(s).length() - 2] = '\0'

#define __(n, s) _(n, s)

#define _n(s) (s)

#define sig(name, signature) []{ __(s, signature); ___(n, name); return util::get_sig(n, s); }()