#pragma once

class IGameEvent;
class listeners : public singleton<listeners>
{
public:
	void setup_listeners() { if ( !event_listener ) { event_listener = new event_listeners(); } }
	void remove_listeners() const { delete event_listener; }
private:
	class event_listeners : public IGameEventListener2
	{
	public:
		event_listeners()
		{
			_( player_hurt, "player_hurt" );
			g_pGameEventManager->AddListener( this, player_hurt, false );
			_( bullet_impact, "bullet_impact" );
			g_pGameEventManager->AddListener( this, bullet_impact, false );
			_( item_purchase, "item_purchase" );
			g_pGameEventManager->AddListener( this, item_purchase, false );
			_( weapon_fire, "weapon_fire" );
			g_pGameEventManager->AddListener( this, weapon_fire, false );
		};
		~event_listeners() { g_pGameEventManager->RemoveListener( this ); };
		virtual void	FireGameEvent( IGameEvent* p_event );
		int				GetEventDebugID() override { return 0x2A; };
	private:
	};
public:
	event_listeners * event_listener = nullptr;
};

