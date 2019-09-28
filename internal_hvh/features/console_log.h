#pragma once

class console_log : public singleton<console_log>
{
public:
	static void buy_event( IGameEvent* game_event );
	static void hurt_event( IGameEvent* game_event );
	void setup();
	void listener( IGameEvent * game_event );
private:
	bool m_setup = false;
};
