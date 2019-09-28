#pragma once

struct aimpoint_t
{
	aimpoint_t( const Vector& point, const Vector& center, const int hitbox )
	{
		this->point = point;
		this->center = center;
		this->hitbox = hitbox;
	}
	Vector point;
	Vector center;
	int hitbox;
};

class aimbot : public singleton<aimbot>
{
public:
	void run();
	static void no_recoil();
	static void no_visual_recoil( CViewSetup & v_view );
	void quick_stop();
	bool get_quick_stop_state() const;
	static bool can_shoot( float time = prediction::get().get_curtime(), bool check = false );
	aimpoint_t get_best_damage( lag_record_t& log, float& damage, Vector* pos = nullptr );
	void multipoint_hitboxes( lag_record_t & log, std::vector<aimpoint_t>& points, bool second_go = false );
	bool get_aimbot_state() const
	{
		return aimbot_called == prediction::get().get_unpred_curtime();
	}
private:
	void select_targets();
	void scan_targets();

	float calc_hc( QAngle vangles, const aimpoint_t& point, C_CSPlayer * player ) const;

	std::vector<aim_data_t> targets;
	std::array<bool, 21> enabled_hitboxes;
	float quick_stop_called = 0.f;
	float spike_shot_called = 0.f;
	float aimbot_called = 0.f;
};
