#pragma once

struct hitmarker_t
{
	hitmarker_t( const float& time, const int& index, const int& damage, const int& hitgroup, const Vector& pos )
	{
		this->time = time;
		this->index = index;
		this->damage = damage;
		this->hitgroup = hitgroup;
		this->pos = pos;
		moved = 0.f;
		alpha = 255.f;
	}
	float time;
	int index;
	int damage;
	int hitgroup;
	float moved;
	float alpha;
	Vector pos;
};

class hitmarker : public singleton<hitmarker>
{
public:
	void listener( IGameEvent * game_event );
	void draw_hits();
	void add_hit( hitmarker_t hit );
	static void render_hitmarker( hitmarker_t& hit, const Vector & screen_pos );
private:
	std::deque<hitmarker_t> hits;
	std::deque<shot_t> shots;
};