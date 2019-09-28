#pragma once

class c_control : public singleton<c_control>
{
public:
	virtual void render( ) = 0;
	virtual void update( ) = 0;
	virtual void update_pos_individual( LONG xdrag, LONG ydrag ) = 0;

	void update_pos( LONG xdrag, LONG ydrag );

	int get_size();
protected:
	std::string m_name;
	bounds m_bounds;
};