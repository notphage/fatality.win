#pragma once
#include "structs.h"

struct misc_data_t
{
	misc_data_t()
	{
		this->spread = -1.f;
	}
	misc_data_t( const float spread )
	{
		this->spread = spread;
	}
	float spread = -1.f;
};

class visuals : public singleton<visuals>
{
public:
	void run();
	void render();
	std::array<visual_data_t, 65>& get_players() { return players; }
private:
	void players_get();
	static void player_box( const visual_data_t& player );
	void player_info( const visual_data_t& player );
	bool bar_ping( const visual_data_t& player, int elements ) const;
	void bar_health( const visual_data_t& player, int elements ) const;
	static void bar_armor( const visual_data_t& player, int elements );
	void bar_ammo( const visual_data_t& player, int elements ) const;
	bool bar_lby_update( const visual_data_t& player, int elements ) const;
	void player_out_of_fov( const Vector& pos ) const;
	void queue_world_data();
	void queue_player_data();
	void queue_misc_data();
	void players_esp();
	void defuse_bar( const world_data_t& ent ) const;
	void ammo_bar( const world_data_t & ent ) const;
	void world_info( const world_data_t& ent ) const;
	void world_esp();
	static void draw_scope();
	static void aa_indicator();
	static void ping_indicator();
	void spread_circle();

	std::array<visual_data_t, 65> players;
	msg_queue<visual_data_t> player_data[ 64 ];
	msg_queue<visual_world_data_t> world_data;
	msg_queue<misc_data_t> misc_data;
};
