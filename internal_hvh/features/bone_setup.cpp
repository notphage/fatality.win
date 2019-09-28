#include "../include_cheat.h"

bool bone_setup::handle_bone_setup( lag_record_t& record, const int bone_mask, matrix3x4_t * bone_out )
{
	auto player = get_entity( record.index );
	if ( !player )
		return false;

	const auto hdr = player->get_model_ptr();
	if ( !hdr )
		return false;

	auto bone_accessor = player->get_bone_accessor();
	if ( !bone_accessor )
		return false;

	const auto backup_matrix = bone_accessor->get_bone_array_for_write();
	if ( !backup_matrix )
		return false;

	// back up
	const auto backup_absorigin = player->get_abs_origin();
	const auto backup_absangles = player->get_abs_angles();

	auto backup_poses = player->get_pose_params();
	auto backup_layers = player->get_anim_layers();
	const auto backup_effects = player->get_effects();

	ALIGN16 matrix3x4_t parent_transform;
	math::get().angle_matrix( record.m_abs_ang, record.m_origin, parent_transform );

	Vector pos[ 128 ];
	QuaternionAligned q[ 128 ];
	ZeroMemory( pos, sizeof( Vector[ 128 ] ) );
	ZeroMemory( q, sizeof( QuaternionAligned[ 128 ] ) );

	player->get_effects() |= EF_NOINTERP;
	player->set_abs_origin( record.m_origin );
	player->set_abs_angles( record.m_abs_ang );
	player->set_pose_params( record.m_poses );
	player->set_anim_layers( record.m_layers );

	// force game to call AccumulateLayers - pvs fix.
	g_ishltv_fix = true;

	// set bone array for write.
	bone_accessor->set_bone_array_for_write( bone_out );

	// compute and build bones.
	player->standard_blending_rules( hdr, pos, q, record.m_sim_time, bone_mask );

	byte computed[ 32 ];
	ZeroMemory( computed, sizeof( byte[ 0x20 ] ) );
	player->build_transformations( hdr, pos, q, parent_transform, bone_mask, &computed[ 0 ] );

	// restore old matrix.
	bone_accessor->set_bone_array_for_write( backup_matrix );

	// restore original interpolated entity data.
	player->get_effects() = backup_effects;
	player->set_abs_origin( backup_absorigin );
	player->set_abs_angles( backup_absangles );
	player->set_pose_params( backup_poses );
	player->set_anim_layers( backup_layers );

	// revert to old game behavior.
	g_ishltv_fix = false;

	return true;
}

bool bone_setup::handle_bone_setup_replace( C_CSPlayer* player, const int max_bones, const int bone_mask, matrix3x4_t * bone_out )
{
	const auto hdr = player->get_model_ptr();
	if ( !hdr )
		return false;

	const auto backup_effects = player->get_effects();

	//set & calc
	const auto origin = player->get_abs_origin();
	const auto angles = player->get_abs_angles();

	ALIGN16 matrix3x4_t parent_transform;
	math::get().angle_matrix( angles, origin, parent_transform );

	Vector pos[ 128 ];
	QuaternionAligned q[ 128 ];
	ZeroMemory( pos, sizeof( Vector[ 128 ] ) );
	ZeroMemory( q, sizeof( QuaternionAligned[ 128 ] ) );

	player->get_effects() |= EF_NOINTERP;

	// force game to call AccumulateLayers - pvs fix.
	g_ishltv_fix = true;

	// compute and build bones.
	player->standard_blending_rules( hdr, pos, q, player->get_simtime(), bone_mask );

	byte computed[ 32 ];
	ZeroMemory( computed, sizeof( byte[ 0x20 ] ) );
	player->build_transformations( hdr, pos, q, parent_transform, bone_mask, &computed[ 0 ] );

	// restore original interpolated entity data.
	//player->get_effects() = backup_effects;

	memcpy( bone_out, player->get_bone_accessor()->m_pBones, sizeof( matrix3x4_t ) * max_bones );

	// revert to old game behavior.
	g_ishltv_fix = false;

	return true;
}
