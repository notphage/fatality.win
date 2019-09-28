#include "../include_cheat.h"

void player_log::log( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_NET_UPDATE_END )
		return;

	if ( !g_pLocalPlayer )
		return;

	auto changed = false;

	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto& curlog = logs[ i ];
		auto player = get_entity( i );

		if ( curlog.player != player )
		{
			curlog.record.clear();
		}
		curlog.player = player;

		if ( !player || player == g_pLocalPlayer )
		{
			curlog.record.clear();
			curlog.m_bRunningTimer = false;
			curlog.m_bShot = false;
			curlog.m_iMode = RMODE_WALL;
			curlog.m_nShots = 0;
			curlog.m_nShotsLby = 0;
			continue;
		}

		if ( !player->is_enemy() )
		{
			if ( curlog.record.size() > 1 )
				curlog.record.pop_back();
		}

		bool reset = ( !player->get_alive() );

		// if this happens, delete all the lagrecords.
		if ( reset )
		{
			player->get_clientside_animation() = true;
			curlog.record.clear();
			curlog.m_bRunningTimer = false;
			curlog.m_bShot = false;
			curlog.m_iMode = RMODE_MOVING;
			curlog.m_nShots = 0;
			curlog.m_nShotsLby = 0;
			continue;
		}

		// indicate that this player has been out of pvs.
		// insert dummy record to separate records
		// to fix stuff like animation and prediction.
		if ( player->IsDormant() )
		{
			curlog.last_dormant_time = g_pGlobals->curtime;

			bool insert = true;

			// we have any records already?
			if ( !curlog.record.empty() )
			{
				const auto front = curlog.record.front();

				// we already have a dormancy separator.
				if ( front.m_dormant )
					insert = false;
			}

			if ( insert )
			{
				// add new record.
				curlog.record.push_front( lag_record_t( player ) );

				// get reference to newly added record.
				const auto current = &curlog.record.front();

				// mark as dormant.
				current->m_dormant = true;
			}
		}

		bool update = ( curlog.record.empty() || player->get_simtime() > curlog.record[ 0 ].m_sim_time );
		// this is the first data update we are receving
		// OR we received data with a newer simulation context.
		if ( update )
		{
			if ( !curlog.record.empty() && curlog.record.front().m_dormant )
			{
				player->get_rotation() = curlog.record.front().m_rotation;
			}
			// add new record.
			curlog.record.push_front( lag_record_t( player ) );

			// get reference to newly added record.
			auto current = &curlog.record.front();

			// mark as non dormant.
			current->m_dormant = false;

			// update animations on current record.
			// call resolver.
			animations::update_player_animations( current, player );

			//// create bone matrix for this record.
			player->setup_bones( *current, BONE_USED_BY_ANYTHING, current->matrix );

			changed = true;
		}

		//apply poses to rendering
		/*if ( !curlog.record.empty() )
			player->set_pose_params_scaled( curlog.record[ 0 ].m_poses );*/

		const auto hdr = player->get_model_ptr()->m_pStudioHdr;
		/*hdr->numbonecontrollers = 0;*/
		hdr->numlocalanim = 0;
		hdr->numlocalseq = 0;

		// no need to store insane amt of data.
		while ( curlog.record.size() > 256 )
			curlog.record.pop_back();
	}

	if ( changed )
		filter_records();
}

player_log_t& player_log::get_log( const int index )
{
	return logs[ index ];
}

void player_log::filter_records( bool cm )
{
	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto& curlog = logs[ i ];
		auto &record = curlog.record;
		if ( record.empty() )
		{
			curlog.m_bFilled = false;
			continue;
		}

		if ( !curlog.player )
		{
			record.clear();
			curlog.m_bFilled = false;
			continue;
		}

		auto wasvalid = false;

		while ( record.size() > 64 )
			record.pop_back();

		for ( auto j = 0u; j < record.size(); j++ )
		{
			auto& currecord = record[ j ];
			if ( currecord.m_dormant && j == 0 )
				continue;

			if ( currecord.m_dormant )
			{
				record.erase( record.begin() + j );
				j--;
				continue;
			}

			if ( ( currecord.valid = lagcomp::get().valid_simtime( currecord.m_sim_time ) ) )
			{
				wasvalid = true;
			}
			else if ( wasvalid )
			{
				wasvalid = false;
				if ( !cm )
					curlog.lastvalid_vis = j - 1;
				curlog.lastvalid = j - 1;
			}
		}
	}
}
