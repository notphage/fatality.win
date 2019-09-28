#pragma once

struct sequence_t
{
	sequence_t( const int instate, const int seqnr, const float time )
	{
		inreliablestate = instate;
		sequencenr = seqnr;
		realtime = time;
	}
	int inreliablestate;
	int sequencenr;
	float realtime;
};

class fake_ping : public singleton<fake_ping>
{
public:
	static void ping_spike( CNetChannel* net_channel );
	static void unhook();
	static void update( const ClientFrameStage_t );
	static void rehook();
	int32_t cur_seqnr = 0;
private:
	static void reset();
};
