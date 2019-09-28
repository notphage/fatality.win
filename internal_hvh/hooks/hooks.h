#pragma once

namespace hooks
{
	extern c_hook* client;
	extern c_hook* clientmode;
	extern c_hook* engine;
	extern c_hook* modelrender;
	extern c_hook* viewrender;
	extern c_hook* enginetrace;
	extern c_hook* renderview;
	extern c_hook* vpanel;
	extern c_hook* device;
	extern c_hook* fire_bullets;
	extern c_hook* engine_sound;
	extern c_hook* surface;
	extern c_hook* death_notice;
	extern c_bphook* clientstateplus8;
	extern c_bphook* ccsplayer;
	extern c_bphook* cbasesplayeranimstate;
	extern c_bphook* ccsplayerrenderable;
	extern c_hook* net_channel;
	extern c_hook* sv_cheats;
	extern c_hook* debug_spread_show;
	extern c_hook* net_showfragments;

	extern recv_prop_hook* smoke_prop;
	extern recv_prop_hook* eye0_prop;
	extern recv_prop_hook* eye1_prop;
	extern recv_prop_hook* lby_prop;
	extern recv_prop_hook* sequence_prop;
	extern recv_prop_hook* rotation_prop;

	using get_bool_fn = bool( __thiscall * )( void* );
	extern get_bool_fn orig_sv_cheats_get_bool;
	bool __fastcall sv_cheats_get_bool( void*, void* );

	using get_int_fn = int( __thiscall * )( void* );
	extern get_int_fn orig_debug_spread_show_get_int;
	int __fastcall debug_spread_show_get_int( void*, void* );

	extern get_bool_fn orig_net_showfragments_get_bool;
	bool __fastcall net_showfragments_get_bool( void*, void* );

	extern WNDPROC orig_wnd_proc;
	LRESULT __stdcall wnd_proc( HWND, UINT, WPARAM, LPARAM );

	using create_move_fn = bool( __stdcall* )( float, CUserCmd* );
	extern create_move_fn orig_create_move;
	bool __stdcall create_move( float, CUserCmd* );

	using frame_stage_notify_fn = void( __stdcall* )( ClientFrameStage_t );
	extern frame_stage_notify_fn orig_frame_stage_notify;
	void __stdcall frame_stage_notify( ClientFrameStage_t );

	using draw_model_execute_fn = void( __thiscall* )( IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );
	extern draw_model_execute_fn orig_draw_model_execute;
	void __stdcall draw_model_execute( IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t* );

	using render_view_fn = void( __thiscall* )( void*, CViewSetup&, CViewSetup&, unsigned int, int );
	extern render_view_fn orig_render_view;
	void __fastcall render_view( void*, void*, CViewSetup&, CViewSetup&, unsigned int, int );

	using is_box_visible_fn = int( __stdcall* )( const Vector&, const Vector& );
	extern is_box_visible_fn orig_is_box_visible;
	int	__stdcall is_box_visible( const Vector&, const Vector& );

	using scene_end_fn = void( __thiscall* )( void* );
	extern scene_end_fn orig_scene_end;
	void __fastcall scene_end( void*, void* );

	using do_post_screen_space_effects_fn = bool( __thiscall* )( void*, CViewSetup* );
	extern do_post_screen_space_effects_fn orig_do_post_screen_space_effects;
	bool _fastcall do_post_screen_space_effects( void*, void*, CViewSetup* );

	using present_fn = long( __stdcall* )( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );
	extern present_fn orig_present;
	long __stdcall present( IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA* );

	using reset_fn = long( __stdcall* )( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
	extern reset_fn orig_reset;
	long __stdcall reset( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );

	using send_datagram_fn = int( __thiscall* )( void*, void* );
	extern send_datagram_fn orig_send_datagram;
	int __fastcall send_datagram( void* netchan, void*, void *datagram );

	using send_net_msg_fn = int( __thiscall* )( void*, INetMessage&, bool, bool );
	extern send_net_msg_fn orig_send_net_msg;
	bool  __fastcall send_net_msg( void* netchan, void * edx, INetMessage & msg, bool reliable, bool voice );

	using is_hltv_fn = bool( __thiscall* )( void* );
	extern is_hltv_fn orig_is_hltv;
	bool __fastcall is_hltv( void * ecx, void * edx );

	using override_view_fn = void( __stdcall* )( CViewSetup* );
	extern override_view_fn orig_override_view;
	void __stdcall override_view( CViewSetup* setup );

	using paint_traverse_fn = void( __thiscall* )( void*, unsigned int, bool, bool );
	extern paint_traverse_fn orig_paint_traverse;
	void __fastcall paint_traverse( void* pPanels, int edx, unsigned int panel, bool forceRepaint, bool allowForce );


	using emit_sound_fn = void( __thiscall* )( void*, void*, int, int, const char *, unsigned int, const char *, float, float, int, int, int, const void *, const void *, void *, bool, float, int );
	extern emit_sound_fn orig_emit_sound;
	void __fastcall emit_sound( void* pThis, int edx, void* filter, int iEntIndex, int iChannel,
								const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample,
								float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
								const void *pOrigin, const void *pDirection, void * pUtlVecOrigins,
								bool bUpdatePositions, float soundtime, int speakerentity );

	using temp_entities_fn = bool( __thiscall* )( void*, void* );
	extern temp_entities_fn orig_temp_entities;
	bool __fastcall temp_entities( void* ECX, void* EDX, void* msg );

	using fire_bullets_postdataupdate_fn = void( __thiscall* )( C_TEFireBullets*, DataUpdateType_t );
	extern fire_bullets_postdataupdate_fn orig_fire_bullets_postdataupdate;
	void __stdcall naked_fire_bullets_postdataupdate( DataUpdateType_t updateType );

	using do_extra_bone_processing_fn = void( __thiscall* )( void*, CStudioHdr * hdr, Vector * pos, Quaternion * q, const matrix3x4_t & matrix, byte * boneComputed, CIKContext * context );
	extern do_extra_bone_processing_fn orig_do_extra_bone_processing;
	void __fastcall do_extra_bone_processing( void * ecx, void * edx, CStudioHdr * hdr, Vector * pos, Quaternion * q, const matrix3x4_t & matrix, byte * boneComputed, CIKContext * context );

	using play_sound_fn = void( __thiscall* )( void*, const char* );
	extern play_sound_fn orig_play_sound;
	void __stdcall play_sound( const char *folderIme );

	using is_connected_fn = bool( __thiscall* )( void* );
	extern is_connected_fn orig_is_connected;
	bool __fastcall is_connected( void* ecx, void* edx );

	using push_notice_call_fn = char*( __thiscall* )( void* );
	extern push_notice_call_fn orig_process_input;
	char* __fastcall process_input( void* ecx, void* edx );

	using eye_angles_fn = QAngle * ( __thiscall* )( void* );
	extern eye_angles_fn orig_eye_angles;
	QAngle* __fastcall eye_angles( void* ecx, void* edx );

	using standard_blending_rules_fn = void * ( __thiscall* )( void*, CStudioHdr*, Vector*, Quaternion*, float, int );
	extern standard_blending_rules_fn orig_standard_blending_rules;
	void __fastcall standard_blending_rules( void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion *q, float curTime, int boneMask );

	using do_animation_event_fn = void * ( __thiscall* )( void*, unsigned int, int );
	extern do_animation_event_fn orig_do_animation_event;
	void __fastcall do_animation_event( void* ecx, void* edx, unsigned int event, int data );

	using maintain_sequence_transitions_fn = void( __thiscall* )( void*, void*, float, Vector *, Quaternion * );
	extern maintain_sequence_transitions_fn orig_maintain_sequence_transitions;
	void __fastcall maintain_sequence_transitions( void * ecx, void * edx, void* bonesetup, float a3, Vector * pos, Quaternion * q );

	using fire_event_fn = void( __thiscall* )( void* );
	extern fire_event_fn orig_fire_event;
	void __fastcall fire_event( void* ecx, void* edx );

	using trace_ray_fn = void( __thiscall* )( void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t* );
	extern trace_ray_fn orig_trace_ray;
	void __fastcall trace_ray( void*, void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t* );
	
	//netprops
	extern RecvVarProxyFn orig_smoke_effect;
	void __cdecl smoke_effect( CRecvProxyData* proxy_data_const, void* entity, void* output );

	extern RecvVarProxyFn orig_m_angEyeAngles0Proxy;
	void __cdecl m_angEyeAngles0Proxy( CRecvProxyData* proxy_data_const, void* entity, void* output );

	extern RecvVarProxyFn orig_m_angEyeAngles1Proxy;
	void __cdecl m_angEyeAngles1Proxy( CRecvProxyData* proxy_data_const, void* entity, void* output );

	extern RecvVarProxyFn orig_m_flLowerBodyYawTargetProxy;
	void __cdecl m_flLowerBodyYawTargetProxy( CRecvProxyData* proxy_data_const, void* entity, void* output );

	extern RecvVarProxyFn orig_sequence;
	void __cdecl sequence( CRecvProxyData* proxy_data_const, void* entity, void* output );

	extern RecvVarProxyFn orig_m_angRotationProxy;
	void __cdecl m_angRotationProxy( CRecvProxyData* proxy_data_const, void* entity, void* output );
}