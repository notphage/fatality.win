#include "../include_cheat.h"

int __fastcall hooks::send_datagram( void* netchan, void*, void *datagram )
{
	if ( !g_pEngine->IsInGame() || g_unload )
		return orig_send_datagram( netchan, datagram );


	const auto net_channel = reinterpret_cast< CNetChannel* > ( netchan );

	const auto backup_seqnr = net_channel->m_nInSequenceNr;

	fake_ping::get().ping_spike( net_channel );

	const auto ret = orig_send_datagram( net_channel, datagram );

	net_channel->m_nInSequenceNr = backup_seqnr;

	return ret;
}

bool __fastcall hooks::send_net_msg( void* netchan, void* edx, INetMessage& msg, bool reliable, bool voice )
{
	if ( msg.GetGroup() == 9 )
		voice = true;

	return orig_send_net_msg( netchan, msg, reliable, voice );
}