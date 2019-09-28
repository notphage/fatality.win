#pragma once

class misc : public singleton<misc>
{
public:
	static void bhop();
	static void remove_smoke( ClientFrameStage_t stage );
	static void remove_flash( ClientFrameStage_t stage );
	static void radar();
	static void reveal_ranks();
	static void soundesp( int idx, const char* sample, const Vector& origin );
	static void clan_tag();
	static void play_freezetime_music();
};
