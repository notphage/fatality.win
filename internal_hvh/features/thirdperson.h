#pragma once

class thirdperson : public singleton<thirdperson>
{
public:
	void run( ) const;
	void set_tp_angle( const QAngle tp_angle ) const;
	void set_abs_angles( const QAngle abs_angles ) const;
	QAngle& get_tp_angle() const { return tp_angle; }
	QAngle& get_abs_angle() const { return abs_angles; }
private:
	mutable QAngle tp_angle;
	mutable QAngle abs_angles;
};