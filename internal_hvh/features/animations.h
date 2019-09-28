#pragma once

struct anim_state_info_t
{
	CBaseHandle	handle;
	float spawn_time;
	CCSGOPlayerAnimState* animstate;
	float animvel;
	std::array<float, 24> poses;
};

class animations : public singleton<animations>
{
public:
	static void stop( const ClientFrameStage_t stage );
	void update_local_animations();
	void fix_local_anims( const ClientFrameStage_t stage );
	static void update_player_animations( lag_record_t* record, C_CSPlayer* m_player );
	void update_custom_anims( C_CSPlayer* player );
private:
	anim_state_info_t anim_info[ 65 ];
};