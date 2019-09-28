#pragma once

class CBoneAccessor;
class bone_setup : public singleton<bone_setup>
{
public:
	static bool handle_bone_setup( lag_record_t& record, const int bone_mask, matrix3x4_t* bone_out );
	static bool handle_bone_setup_replace( C_CSPlayer* player, const int max_bones, const int bone_mask, matrix3x4_t* bone_out );
};

class C_BaseAnimating;
class CBoneAccessor
{
public:
	inline matrix3x4_t *get_bone_array_for_write( void ) const
	{
		return m_pBones;
	}
	inline void set_bone_array_for_write( matrix3x4_t* bonearray )
	{
		m_pBones = bonearray;
	}
	alignas( 16 ) matrix3x4_t * m_pBones;

	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
};