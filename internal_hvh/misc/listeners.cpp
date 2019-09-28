#include "../include_cheat.h"

void listeners::event_listeners::FireGameEvent( IGameEvent * game_event )
{
	console_log::get().listener( game_event );

	_( player_hurt, "player_hurt" );
	_( bullet_impact, "bullet_impact" );
	_( weapon_fire, "weapon_fire" );
	if ( !strcmp( game_event->GetName(), player_hurt ) )
	{
		hitmarker::get().listener( game_event );
		resolver::get().hurt_listener( game_event );
	}
	else if ( !strcmp( game_event->GetName(), bullet_impact ) )
	{
		resolver::get().listener( game_event );
		beams::get().listener( game_event );
	}
	else if ( !strcmp( game_event->GetName(), weapon_fire ) )
	{
		resolver::get().listener( game_event );
	}
};
