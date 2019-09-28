#pragma once

class freestanding : public singleton<freestanding>
{
public:
	bool override( float & yaw ) const;
	bool get_real( float& yaw );
	void get_targets();
	void update_hotkeys( ClientFrameStage_t stage );
	static float trace_freestanding( C_CSPlayer* player, const Vector point );
	int get_mode() const { return direction; }
	float last_fs_time = 0.f;
private:
	int direction = -1;
	std::vector<C_CSPlayer*> players;
};
