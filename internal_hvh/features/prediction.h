#pragma once

class prediction : public singleton<prediction>
{
public:
	void start();
	void finish() const;
	static int get_tickbase();
	static float get_curtime();
	float get_unpred_curtime() const;
	Vector get_unpred_vel() const;
	Vector get_pred_vel() const;
	Vector get_unpred_eyepos() const;
private:
	float orig_currtime = 0.f;
	float orig_frametime = 0.f;
	CMoveData move_data;

	int* m_prediction_random_seed = nullptr;
	Vector unpred_vel;
	Vector unpred_eyepos;
	Vector pred_vel;
};