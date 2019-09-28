#pragma once

enum resolver_modes
{
	RMODE_MOVING = 0,
	RMODE_DELTA,
	RMODE_SPIN,
	RMODE_WALL
};

struct aim_data_t
{
	aim_data_t( C_CSPlayer* player, const int& idx )
	{
		this->player = player;
		this->index = idx;
	}
	C_CSPlayer*	player;
	int			index;
};

struct visual_data_t
{
	visual_data_t()
	{
		this->player = nullptr;
		this->index = -1;
		this->fade_time = 0.f;
		this->alpha = 0.f;
		this->valid = false;
		this->was_unseen = true;
		this->oof = false;
		this->running_timer = false;
	}

	C_CSPlayer*	player;
	bool		valid;
	int			index;
	float		fade_time;
	float		alpha;
	Vector		top;
	Vector		bot;
	float		width;
	float		height;
	bool was_unseen;
	bool oof;
	Vector pos;

	//extra
	float vel;
	int act;
	float l1_weight;
	float l1_cycle;
	bool dormant;
	float nextupdate;
	int maxclip;
	int clip;
	int armor;
	int health;
	bool scoped;
	float ping;
	bool bomb;

	//record
	bool running_timer;
	int shots;
	bool fakewalking;
	std::string name;
	std::string weapon_name;
	std::string rmode;
};

struct world_data_t
{
	int classid;
	float alpha;
	Vector pos;
	std::string name;
	float explode_time;
	int clip;
	int maxclip;
	bool defusing;
	int32 defuser;
};


struct visual_world_data_t
{
	std::vector<world_data_t> world_data;
};
	

class C_AnimationLayer;
struct lag_record_t
{
	lag_record_t() = default;
	lag_record_t( C_CSPlayer* player );
	void apply( C_CSPlayer* player, bool backup = false ) const;

	int index;
	bool valid;
	matrix3x4_t matrix[ 128 ];

	bool m_fake_walk;
	int m_mode;
	bool m_dormant;
	Vector m_velocity;
	Vector m_origin;
	Vector m_abs_origin;
	Vector m_anim_velocity;
	Vector m_obb_mins;
	Vector m_obb_maxs;
	std::array<C_AnimationLayer, 15> m_layers;
	std::array<float, 24> m_poses;
	float m_anim_time;
	float m_sim_time;
	float m_duck;
	float m_body;
	QAngle m_eye_angles;
	QAngle m_abs_ang;
	int m_flags;
	int m_lag;
	QAngle m_rotation;

	int m_cached_count;
	int m_writable_bones;

	bool m_lby_flick;
	bool m_shot;
	bool m_override;
};

struct dormant_record_t
{
	QAngle m_abs_ang;
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flLeanAmount; //0x90
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
};


struct player_log_t
{
	player_log_t()
	{
		player = nullptr;
		m_bCustomCorrection = false;
		m_bFilled = false;
		m_iPlayerListYawCorrection = 0;
		onground_nospread = false;
		m_bOverride = false;
		m_nShotsLby = 0;
		last_hit_bute = 0;
		m_iMode = 0;
		lastvalid = 0;
		lastvalid_vis = 0;
		spindelta = 0;
		spinbody = 0;
		step = 0;
	}
	C_CSPlayer*	player;
	bool m_bFilled;
	int m_iMode;

	float spindelta;
	float spinbody;
	int step;

	int lastvalid;
	int lastvalid_vis;

	float m_flSpawntime;

	float m_flLowerBodyYawTarget;
	float m_flOldLowerBodyYawTarget;
	float m_flLastLowerBodyYawTargetUpdateTime;
	float m_flLastMovingLowerBodyYawTarget;
	float m_flLastMovingLowerBodyYawTargetTime;
	float nextBodyUpdate;
	float m_flSavedLbyDelta;
	float m_flBruteStartLby;

	QAngle m_vecLastShotEyeAngles;
	float m_flLastShotSimulationTime;

	int m_iPlayerListYawCorrection;
	bool m_bCustomCorrection;
	float m_flPitch;

	float m_flProxyPitch;
	float m_flProxyYaw;

	bool onground_nospread;
	bool fakewalking;

	int m_nShots;
	int oldshots;
	int m_nShotsLby;

	bool m_bLbyFlick;
	bool m_bShot;
	bool m_bOverride;

	int last_hit_bute;

	bool m_bRunningTimer;
	Vector m_vecLastNonDormantOrig;

	std::deque<lag_record_t> record;
	dormant_record_t dormant_record;
	float last_dormant_time;
	Vector last_nondormant_origin;
};

enum material_index_t : int
{
	MAT_VISIBLE = 0,
	MAT_VISIBLE_FLAT,
	MAT_VISIBLE_SHINE,
	MAT_HID,
	MAT_HID_FLAT,
	MAT_HID_SHINE,
	MAT_RECORD,


	NUM_MATERIALS
};


struct FireBulletData
{
	Vector           src;
	trace_t          enter_trace;
	Vector           direction;
	ITraceFilter     filter;
	CCSWeaponData*  wpn_data;
	float           trace_length;
	float           trace_length_remaining;
	float           length_to_end;
	float           current_damage;
	int             penetrate_count;
};