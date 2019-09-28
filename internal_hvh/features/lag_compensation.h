#pragma once

class lagcomp : public singleton<lagcomp>
{
public:
	static int fix_tickcount( const float& simtime );
	static bool valid_simtime( const float & simtime );
	static bool check_ping( const float & simtime, int * wish_sequence_nr, int * wish_reliable_state );
	static float get_lerp_time();
	static void extrapolate( C_CSPlayer* player, Vector& origin, Vector& velocity, int& flags, bool wasonground );
};
