#include "../include_cheat.h"

void fake_ping::update( const ClientFrameStage_t stage )
{
	static bool prev_state;
	const bool state = GetAsyncKeyState( vars::key.ping_spike.get<int>() );
	if ( prev_state != state )
	{
		if ( state )
			vars::aim.ping_spike.set( !vars::aim.ping_spike.get<bool>() );

		prev_state = state;
	}

	if ( stage == FRAME_START )
		unhook();
}

void fake_ping::reset()
{
	if ( hooks::net_channel )
	{
		delete hooks::net_channel;
		hooks::net_channel = nullptr;
	}
}

void fake_ping::ping_spike( CNetChannel * net_channel )
{
	if ( g_pLocalPlayer->get_health() < 1 )
		return;

	static auto oldseqnum = 0;

	if ( oldseqnum == net_channel->m_nInSequenceNr )
		return;

	oldseqnum = net_channel->m_nInSequenceNr;

	if ( vars::aim.ping_spike.get<bool>() )
	{
		net_channel->m_nInSequenceNr += NET_FRAMES_MASK * 2 - static_cast< uint32_t >( NET_FRAMES_MASK * ( vars::aim.ping_spike_amt.get<float>() / 1000.f ) );
		return;
	}

	if ( vars::aim.ping_spike_base.get<bool>() )
	{
		net_channel->m_nInSequenceNr += NET_FRAMES_MASK * 2 - static_cast< uint32_t >( NET_FRAMES_MASK * ( 160.f / 1000.f ) );
	}
}

void fake_ping::unhook()
{
	const auto clientstate = *reinterpret_cast< uintptr_t* >( g_pClientState );
	if ( clientstate && g_pEngine->IsInGame() && g_pClientState->m_NetChannel && !g_unload )
		return;

	reset();
}

void fake_ping::rehook()
{
	if ( hooks::net_channel || g_unload )
		return;

	const auto net_channel = g_pClientState->m_NetChannel;
	if ( !net_channel )
		return;

	hooks::net_channel = new c_hook( net_channel );
	hooks::orig_send_datagram = hooks::net_channel->apply<hooks::send_datagram_fn>( 48, hooks::send_datagram );
	hooks::orig_send_net_msg = hooks::net_channel->apply< hooks::send_net_msg_fn>( 42, hooks::send_net_msg );
}
