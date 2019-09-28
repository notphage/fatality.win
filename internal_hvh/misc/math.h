#pragma once

#define QUICK_MATH

class QAngle;
class Vector;
struct matrix3x4_t;
class math : public singleton<math>
{
public:
	static void vector_transform( const Vector & in1, const matrix3x4_t & in2, Vector & out );
	QAngle calc_angle( const Vector src, const Vector dst ) const;
	void vector_angles( const Vector & forward, QAngle & angles ) const;
	void angle_vectors( const QAngle & angles, Vector * forward, Vector *right = nullptr, Vector *up = nullptr ) const;

	void angle_matrix( const QAngle &angles, const Vector &position, matrix3x4_t& matrix ) const;
	void angle_matrix( const QAngle &angles, matrix3x4_t& matrix ) const;
	static void matrix_set_column( const Vector &in, int column, matrix3x4_t& out );
	static void matrix_get_column( const matrix3x4_t & in, int column, Vector & out );
	static void matrix_set_origin( Vector pos, matrix3x4_t & matrix );
	static Vector matrix_get_origin( const matrix3x4_t & src );
	static void matrix_copy( const matrix3x4_t & in, matrix3x4_t & out );

	static void vector_scale( const Vector& in, float scale, Vector& out );
	static void vector_scale( const float * in, float scale, float * out );

	template < typename t >
	static t lerp( const t& t1, const t& t2, float progress )
	{
		return t1 + ( t2 - t1 ) * progress;
	}

	static void vector_ma( const Vector & start, float scale, const Vector & direction, Vector & dest );
	static void vector_i_transform( const Vector& in1, const matrix3x4_t & in2, Vector& out );
	static float dot_product( const Vector v1, const Vector v2 );
	static void cross_product( const Vector v1, const Vector v2, Vector & result );

	static bool screen_transform( const Vector & in, Vector & out );
	static bool world_to_screen( const Vector& in, Vector& out );

	float get_fov( const QAngle& angles, const QAngle& target ) const;

	inline void sin_cos( float radians, float *sine, float *cosine ) const;
	static inline void fast_sqrt( float * __restrict p_out, float * __restrict p_in );
	float fast_asin( float x ) const;
	float fast_atan2( float y, float x ) const;
	float fast_atan( float x ) const;
	float fast_cos( float x ) const;
	float fast_acos( float x ) const;
	static void fast_rsqrt( float a, float * out );
	static float fast_vec_normalize( Vector & vec );
	float fast_sin( float x ) const;

	static float normalize_float( float angle )
	{
		auto revolutions = angle / 360.f;
		if ( angle > 180.f || angle < -180.f )
		{
			revolutions = round( abs( revolutions ) );
			if ( angle < 0.f )
				angle = ( angle + 360.f * revolutions );
			else
				angle = ( angle - 360.f * revolutions );
			return angle;
		}
		return angle;
	}
};