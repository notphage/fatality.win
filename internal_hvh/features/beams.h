#pragma once

struct impact_info_t
{
	Vector			start;
	Vector			pos;
	Color			color;
	C_CSPlayer*		player;
	float			time;

	impact_info_t( const float time, const Vector start, const Vector pos, C_CSPlayer* player, const Color col = Color::White() )
	{
		this->player = player;
		this->start = start;
		this->pos = pos;
		this->color = col;
		this->time = time;
	}
	float length() const
	{
		return start.Dist( this->pos );
	}
	bool equal( const impact_info_t& info ) const
	{
		if ( this->player != info.player )
			return false;

		if ( this->time != info.time )
			return false;

		return true;
	}
};

class beams : public singleton<beams>
{
public:
	void draw_impacts();
	void listener( IGameEvent * game_event );
	void add_local_beam( impact_info_t info );
	bool beam_exists( C_CSPlayer* player, const float curtime );
private:
	std::deque<impact_info_t> impacts;
};
