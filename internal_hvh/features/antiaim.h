#pragma once

class antiaim : public singleton<antiaim>
{
public:
	void run();
	static void fix_movement( QAngle wish_angle );
	int get_rest_choke() const { return to_choke - g_pClientState->m_nChokedCommands; }
	int get_to_choke() const { return to_choke; }
	float get_target_lby() const { return target_lby; }
	void choose_real();
	bool get_fake_walk_state() const;
	int type = 0;

private:
	void choose_fk();
	void fake_lag();
	void fake_walk();

	static bool on_peek_fakelag();
	void get_regular();
	void get_air();
	void get_moving();
	void do_spin() const;
	void do_add();
	void do_jitter( );
	void do_freestand();
	bool do_edge( );
	void do_at_target() const;
	bool do_lby( );

	bool lby_update( );
	void lby_prediction();

	float next_lby_update = 0.f;

	uint32_t to_choke = 0;
	float fake_walk_called = 0.f;
	bool update_lby = false;
	float initial_lby = 0.f;
	float target_lby = 0.f;
	bool firstupdate = true;
	bool secondupdate = true;
};
