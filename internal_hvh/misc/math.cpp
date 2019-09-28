#include "../include_cheat.h"
#include <xmmintrin.h>

Vector vec_zero = Vector( 0.0f, 0.0f, 0.0f );
QAngle ang_zero = QAngle( 0.0f, 0.0f, 0.0f );

void math::vector_transform( const Vector & in1, const matrix3x4_t & in2, Vector & out )
{
	out.x = in1.Dot( in2.m_flMatVal[ 0 ] ) + in2.m_flMatVal[ 0 ][ 3 ];
	out.y = in1.Dot( in2.m_flMatVal[ 1 ] ) + in2.m_flMatVal[ 1 ][ 3 ];
	out.z = in1.Dot( in2.m_flMatVal[ 2 ] ) + in2.m_flMatVal[ 2 ][ 3 ];
}

QAngle math::calc_angle( const Vector src, const Vector dst ) const
{
	auto delta = src - dst;
	if ( delta == vec_zero )
		return ang_zero;

	const auto len = delta.Length();

	if ( delta.z == 0.0f && len == 0.0f )
		return ang_zero;

	if ( delta.y == 0.0f && delta.x == 0.0f )
		return ang_zero;

	QAngle angles;

#ifdef QUICK_MATH
	angles.x = ( fast_asin( delta.z / delta.Length() ) * M_RADPI );
	angles.y = ( fast_atan( delta.y / delta.x ) * M_RADPI );
#else
	angles.x = ( asinf( delta.z / delta.Length() ) * M_RADPI );
	angles.y = ( atanf( delta.y / delta.x ) * M_RADPI );
#endif

	angles.z = 0.0f;
	if ( delta.x >= 0.0f ) { angles.y += 180.0f; }

	return angles.Clamp();
}

void math::vector_angles( const Vector& forward, QAngle &angles ) const
{
	float tmp, yaw, pitch;

	if ( forward[ 2 ] == 0.0f && forward[ 0 ] == 0.0f )
	{
		yaw = 0;

		if ( forward[ 2 ] > 0.0f )
			pitch = 90.0f;
		else
			pitch = 270.0f;
	}
	else
	{
#ifdef QUICK_MATH
		yaw = ( fast_atan2( forward[ 1 ], forward[ 0 ] ) * 180.0f / M_PI );
#else
		yaw = ( atan2f( forward[ 1 ], forward[ 0 ] ) * 180.0f / M_PI );
#endif

		if ( yaw < 0.0f )
			yaw += 360.0f;

		float sqin = forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ];
		fast_sqrt( &tmp, &sqin );

#ifdef QUICK_MATH
		pitch = ( fast_atan2( -forward[ 2 ], tmp ) * 180.0f / M_PI );
#else
		pitch = ( atan2f( -forward[ 2 ], tmp ) * 180.0f / M_PI );
#endif

		if ( pitch < 0.0f )
			pitch += 360.0f;
	}

	pitch -= floorf( pitch / 360.0f + 0.5f ) * 360.0f;
	yaw -= floorf( yaw / 360.0f + 0.5f ) * 360.0f;

	if ( pitch > 89.0f )
		pitch = 89.0f;
	else if ( pitch < -89.0f )
		pitch = -89.0f;

	angles[ 0 ] = pitch;
	angles[ 1 ] = yaw;
	angles[ 2 ] = 0;
}

void math::angle_vectors( const QAngle &angles, Vector *forward, Vector *right, Vector *up ) const
{
	float sr, sp, sy, cr, cp, cy;

	sin_cos( DEG2RAD( angles.x ), &sp, &cp );
	sin_cos( DEG2RAD( angles.y ), &sy, &cy );
	sin_cos( DEG2RAD( angles.z ), &sr, &cr );

	if ( forward )
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if ( right )
	{
		right->x = ( -1 * sr*sp*cy + -1 * cr*-sy );
		right->y = ( -1 * sr*sp*sy + -1 * cr*cy );
		right->z = -1 * sr*cp;
	}

	if ( up )
	{
		up->x = ( cr*sp*cy + -sr * -sy );
		up->y = ( cr*sp*sy + -sr * cy );
		up->z = cr * cp;
	}
}

void math::angle_matrix( const QAngle & angles, const Vector & position, matrix3x4_t & matrix ) const
{
	angle_matrix( angles, matrix );
	matrix_set_column( position, 3, matrix );
}

void math::angle_matrix( const QAngle & angles, matrix3x4_t & matrix ) const
{
	float sr, sp, sy, cr, cp, cy;

	sin_cos( DEG2RAD( angles.x ), &sp, &cp );
	sin_cos( DEG2RAD( angles.y ), &sy, &cy );
	sin_cos( DEG2RAD( angles.z ), &sr, &cr );

	// matrix = (YAW * PITCH) * ROLL
	matrix[ 0 ][ 0 ] = cp * cy;
	matrix[ 1 ][ 0 ] = cp * sy;
	matrix[ 2 ][ 0 ] = -sp;

	const auto crcy = cr * cy;
	const auto crsy = cr * sy;
	const auto srcy = sr * cy;
	const auto srsy = sr * sy;
	matrix[ 0 ][ 1 ] = sp * srcy - crsy;
	matrix[ 1 ][ 1 ] = sp * srsy + crcy;
	matrix[ 2 ][ 1 ] = sr * cp;

	matrix[ 0 ][ 2 ] = ( sp*crcy + srsy );
	matrix[ 1 ][ 2 ] = ( sp*crsy - srcy );
	matrix[ 2 ][ 2 ] = cr * cp;

	matrix[ 0 ][ 3 ] = 0.0f;
	matrix[ 1 ][ 3 ] = 0.0f;
	matrix[ 2 ][ 3 ] = 0.0f;
}

void math::matrix_set_column( const Vector & in, int column, matrix3x4_t & out )
{
	out[ 0 ][ column ] = in.x;
	out[ 1 ][ column ] = in.y;
	out[ 2 ][ column ] = in.z;
}

void math::matrix_get_column( const matrix3x4_t& in, int column, Vector &out )
{
	out.x = in[ 0 ][ column ];
	out.y = in[ 1 ][ column ];
	out.z = in[ 2 ][ column ];
}

void math::matrix_set_origin( Vector pos, matrix3x4_t& matrix )
{
	for ( size_t i{}; i < 3; ++i )
	{
		matrix[ i ][ 3 ] = pos[ i ];
	}
}

Vector math::matrix_get_origin( const matrix3x4_t& src )
{
	return Vector( src[ 0 ][ 3 ], src[ 1 ][ 3 ], src[ 2 ][ 3 ] );
}


void math::vector_scale( const Vector& in, float scale, Vector& out )
{
	out[ 0 ] = in[ 0 ] * scale;
	out[ 1 ] = in[ 1 ] * scale;
	out[ 2 ] = in[ 2 ] * scale;
}

void math::vector_scale( const float* in, float scale, float* out )
{
	out[ 0 ] = in[ 0 ] * scale;
	out[ 1 ] = in[ 1 ] * scale;
	out[ 2 ] = in[ 2 ] * scale;
}

void math::vector_ma( const Vector& start, float scale, const Vector& direction, Vector& dest )
{
	dest[ 0 ] = start[ 0 ] + direction[ 0 ] * scale;
	dest[ 1 ] = start[ 1 ] + direction[ 1 ] * scale;
	dest[ 2 ] = start[ 2 ] + direction[ 2 ] * scale;
}

void math::vector_i_transform( const Vector& in1, const matrix3x4_t& in2, Vector& out )
{
	float in1_t[ 3 ];

	in1_t[ 0 ] = in1[ 0 ] - in2[ 0 ][ 3 ];
	in1_t[ 1 ] = in1[ 1 ] - in2[ 1 ][ 3 ];
	in1_t[ 2 ] = in1[ 2 ] - in2[ 2 ][ 3 ];

	out[ 0 ] = in1_t[ 0 ] * in2[ 0 ][ 0 ] + in1_t[ 1 ] * in2[ 1 ][ 0 ] + in1_t[ 2 ] * in2[ 2 ][ 0 ];
	out[ 1 ] = in1_t[ 0 ] * in2[ 0 ][ 1 ] + in1_t[ 1 ] * in2[ 1 ][ 1 ] + in1_t[ 2 ] * in2[ 2 ][ 1 ];
	out[ 2 ] = in1_t[ 0 ] * in2[ 0 ][ 2 ] + in1_t[ 1 ] * in2[ 1 ][ 2 ] + in1_t[ 2 ] * in2[ 2 ][ 2 ];
}

float math::dot_product( const Vector v1, const Vector v2 )
{
	return v1[ 0 ] * v2[ 0 ] + v1[ 1 ] * v2[ 1 ] + v1[ 2 ] * v2[ 2 ];
}

void math::cross_product( const Vector v1, const Vector v2, Vector& result )
{
	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;
}

void math::matrix_copy( const matrix3x4_t& in, matrix3x4_t& out )
{
	memcpy( out.Base(), in.Base(), sizeof( float ) * 3 * 4 );
}

const VMatrix& world_to_screen_matrix()
{
	static uintptr_t view_matrix = 0;
	if ( !view_matrix )
	{
		view_matrix = sig( "client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" );
		view_matrix = *reinterpret_cast< uintptr_t* >( view_matrix + 0x3 ) + 176;
	}
	return *reinterpret_cast< VMatrix* >( view_matrix );
}

bool math::screen_transform( const Vector & in, Vector & out )
{
	const auto &w2_s_matrix = world_to_screen_matrix();
	out.x = w2_s_matrix[ 0 ][ 0 ] * in[ 0 ] + w2_s_matrix[ 0 ][ 1 ] * in[ 1 ] + w2_s_matrix[ 0 ][ 2 ] * in[ 2 ] + w2_s_matrix[ 0 ][ 3 ];
	out.y = w2_s_matrix[ 1 ][ 0 ] * in[ 0 ] + w2_s_matrix[ 1 ][ 1 ] * in[ 1 ] + w2_s_matrix[ 1 ][ 2 ] * in[ 2 ] + w2_s_matrix[ 1 ][ 3 ];
	out.z = 0.0f;

	const auto w = w2_s_matrix[ 3 ][ 0 ] * in.x + w2_s_matrix[ 3 ][ 1 ] * in.y + w2_s_matrix[ 3 ][ 2 ] * in.z + w2_s_matrix[ 3 ][ 3 ];

	if ( w < 0.001f )
	{
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	const auto invw = 1.0f / w;
	out.x *= invw;
	out.y *= invw;

	return true;
}

bool math::world_to_screen( const Vector & in, Vector & out )
{
	const auto result = screen_transform( in, out );

	int w, h;
	g_pEngine->GetScreenSize( w, h );

	out.x = ( w / 2.0f ) + ( out.x * w ) / 2.0f;
	out.y = ( h / 2.0f ) - ( out.y * h ) / 2.0f;

	return result;
}

float math::get_fov( const QAngle & angles, const QAngle & target ) const
{
	Vector ang, aim;

	angle_vectors( angles, &aim );
	angle_vectors( target, &ang );

#ifdef QUICK_MATH
	return RAD2DEG( fast_acos( aim.Dot( ang ) / aim.LengthSqr() ) );
#else
	return RAD2DEG( acos( aim.Dot( ang ) / aim.LengthSqr() ) );
#endif
}

inline void math::sin_cos( const float radians, float * sine, float * cosine ) const
{
#ifdef QUICK_MATH
	*sine = fast_sin( radians );
	*cosine = fast_cos( radians );
#else
	*sine = sin( radians );
	*cosine = cos( radians );
#endif
}

inline void math::fast_sqrt( float * __restrict p_out, float * __restrict p_in )
{
	_mm_store_ss( p_out, _mm_sqrt_ss( _mm_load_ss( p_in ) ) );
	// compiles to movss, sqrtss, movss
}

inline float math::fast_asin( float x ) const
{
	const auto negate = float( x < 0 );
	x = abs( x );
	auto ret = -0.0187293;
	ret *= x;
	ret += 0.0742610;
	ret *= x;
	ret -= 0.2121144;
	ret *= x;
	ret += 1.5707288;
	ret = 3.14159265358979*0.5 - sqrt( 1.0 - x )*ret;
	return float( ret - 2 * negate * ret );
}

inline float math::fast_atan2( const float y, const float x ) const
{
	const auto xabs = fabs( x );
	const auto yabs = fabs( y );
	double t3 = xabs;
	double t1 = yabs;
	auto t0 = max( t3, t1 );
	t1 = min( t3, t1 );
	t3 = 1 / t0;
	t3 = t1 * t3;

	const auto t4 = t3 * t3;
	t0 = -0.013480470;
	t0 = t0 * t4 + 0.057477314;
	t0 = t0 * t4 - 0.121239071;
	t0 = t0 * t4 + 0.195635925;
	t0 = t0 * t4 - 0.332994597;
	t0 = t0 * t4 + 0.999995630;
	t3 = t0 * t3;

	t3 = ( yabs > xabs ) ? 1.570796327 - t3 : t3;
	t3 = ( x < 0 ) ? 3.141592654 - t3 : t3;
	t3 = ( y < 0 ) ? -t3 : t3;

	return float( t3 );
}

inline float math::fast_atan( const float x ) const
{
	return fast_atan2( x, float( 1 ) );
}

float math::fast_sin( float x ) const
{
	x *= float( 0.159155 );
	x -= floor( x );
	const auto xx = x * x;
	auto y = -6.87897;
	y = y * xx + 33.7755;
	y = y * xx - 72.5257;
	y = y * xx + 80.5874;
	y = y * xx - 41.2408;
	y = y * xx + 6.28077;
	return float( x * y );
}

inline float math::fast_cos( const float x ) const
{
	return fast_sin( x + 1.5708f );
}

float math::fast_acos( float x ) const
{
	const auto negate = float( x < 0 );
	x = abs( x );
	auto ret = float( -0.0187293 );
	ret = ret * x;
	ret = ret + 0.0742610;
	ret = ret * x;
	ret = ret - 0.2121144;
	ret = ret * x;
	ret = ret + 1.5707288;
	ret = ret * sqrt( 1.0 - x );
	ret = ret - 2 * negate * ret;
	return negate * 3.14159265358979 + ret;
}

inline void math::fast_rsqrt( float a, float* out )
{
	const auto xx = _mm_load_ss( &a );
	auto xr = _mm_rsqrt_ss( xx );
	auto xt = _mm_mul_ss( xr, xr );
	xt = _mm_mul_ss( xt, xx );
	xt = _mm_sub_ss( _mm_set_ss( 3.f ), xt );
	xt = _mm_mul_ss( xt, _mm_set_ss( 0.5f ) );
	xr = _mm_mul_ss( xr, xt );
	_mm_store_ss( out, xr );
}

float math::fast_vec_normalize( Vector& vec )
{
	const auto sqrlen = vec.LengthSqr() + 1.0e-10f;
	float invlen;
	fast_rsqrt( sqrlen, &invlen );
	vec.x *= invlen;
	vec.y *= invlen;
	vec.z *= invlen;
	return sqrlen * invlen;
}