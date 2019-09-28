#include "../include_cheat.h"

void __stdcall fire_bullets_postdataupdate( C_TEFireBullets* thisptr, DataUpdateType_t updateType )
{
	if ( g_pLocalPlayer && g_pLocalPlayer->get_alive() && g_pEngine->IsInGame() )
	{
		auto player = get_entity( thisptr->m_iPlayer + 1 );
		if ( player && player->is_enemy() && player->GetClientClass()->m_ClassID == 35 )
		{
			auto playerRecord = &player_log::get().get_log( thisptr->m_iPlayer + 1 );
			playerRecord->m_flLastShotSimulationTime = player->get_simtime();
			playerRecord->m_vecLastShotEyeAngles = thisptr->m_vecAngles;
		}
	}

	hooks::orig_fire_bullets_postdataupdate( thisptr, updateType );
}

__declspec ( naked ) void __stdcall hooks::naked_fire_bullets_postdataupdate( DataUpdateType_t updateType )
{
	__asm
	{
		push[ esp + 4 ]
		push ecx
		call fire_bullets_postdataupdate
		retn 4
	}
}