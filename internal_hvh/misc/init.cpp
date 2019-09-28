#include "../include_cheat.h"

HWND init::window;

void init::on_startup()
{
	_( Valve001, "Valve001" );

	while ( !( ( window = FindWindowA( Valve001, nullptr ) ) ) )
		Sleep( 200 );

	hooks::orig_wnd_proc = reinterpret_cast< WNDPROC >( SetWindowLongPtr( window, GWL_WNDPROC, reinterpret_cast< LONG_PTR >( hooks::wnd_proc ) ) );

	_( serverbrowser, "serverbrowser.dll" );

	while ( !GetModuleHandleA( serverbrowser ) )
		Sleep( 200 );

	get_offsets();
	erase_fn( get_offsets );
	get_interfaces();
	get_materials();
	erase_fn( get_materials );
	listeners::get().setup_listeners();
	skinchanger::get().parse_kits();
	erase_fn( skinchanger::parse_kits );
	inventorychanger::get().load();
	hooks();
	erase_fn( hooks );

	g_erase = true;

	erase_end;
}

void init::get_offsets()
{
	offsets::local_player = sig( "client.dll", "8B 0D ? ? ? ? 83 FF FF 74 07" ) + 0x2;
	offsets::move_helper = sig( "client.dll", "8B 0D ? ? ? ? 8B 46 08 68" ) + 0x2;
	offsets::pred_random_seed = sig( "client.dll", "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) + 0x2;
	offsets::set_abs_angles = sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8" );
	offsets::view_render = sig( "client.dll", "FF 50 14 E8 ?? ?? ?? ?? 5D" ) - 0x7;
	offsets::view_render_bms = sig( "client.dll", "A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 08" ) + 0x1;
	offsets::hbp = sig( "client.dll", "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 78 8B 53 14" );
	offsets::clip_trace_to_players = sig( "client.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 8B 43 10" );
	offsets::create_anim_state = sig( "client.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46" );
	offsets::update_anim_state = sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24" );
	offsets::cam_think = sig( "client.dll", "85 C0 75 30 38 86" );
	offsets::set_abs_origin = sig( "client.dll", "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ?? ??" );
	offsets::standard_blending_rules = sig( "client.dll", "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6" );
	offsets::build_transformations = sig( "client.dll", "55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 1C" );
	offsets::device = **reinterpret_cast< uintptr_t** >( sig( "shaderapidx9.dll", "88 56 10" ) + 17 );
	offsets::glow_obj_manager = sig( "client.dll", "0F 11 05 ? ? ? ? 83 C8 01" ) + 0x3;
	offsets::smoke_count = sig( "client.dll", "A3 ? ? ? ? 57 8B CB" ) + 0x1;
	offsets::get_weapon_name = sig( "client.dll", "55 8B EC 56 8B F1 E8 ? ? ? ? 8B 8E" );
	offsets::get_sequence_act = sig( "client.dll", "55 8B EC 83 7D 08 FF 56 8B F1 74 3D" );
	offsets::enable_invalidate_bone_cache = *reinterpret_cast< uintptr_t* >( sig( "client.dll", "C6 05 ? ? ? ? ? F3 0F 5F" ) + 0x2 );
	offsets::invalidate_physics_recursive = sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56" );
	offsets::is_breakable_entity = sig( "client.dll", "55 8B EC 51 56 8B F1 85 F6 74 68" );
	offsets::fire_bullets = *reinterpret_cast< uintptr_t* >( sig( "client.dll", "55 8B EC 51 53 56 8B F1 BB ? ? ? ? B8" ) + 0x131 );
	offsets::ccsplayer_vtable = sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C" ) + 0x47;
	offsets::cbaseplayeranimstate_vtable = sig( "client.dll", "C7 06 ? ? ? ? E8 ? ? ? ? 8D 8E 84 00 00 00 E8 ? ? ? ? F6 45 08 01 74 0E 68 AC 00 00 00 " ) + 0x2;
	offsets::netchan_vtable = sig( "engine.dll", "C7 06 ? ? ? ? 8D BE AC 00 00 00" ) + 0x2;
	offsets::print_dev_console = sig( "engine.dll", "55 8B EC 81 EC ? ? ? ? 80 3D ? ? ? ? ? 56 8B 35" );
	offsets::find_element = sig( "client.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28" );

	g_pLocalPlayer = *reinterpret_cast< C_locPlayer* >( offsets::local_player );
	g_disable_post_processing = *reinterpret_cast< bool** >( sig( "client.dll", "80 3D ? ? ? ? ? 53 56 57 0F 85" ) + 0x2 );
	g_GCClientSystem = **reinterpret_cast< uintptr_t** >( sig( "client.dll", "8B 0D ? ? ? ? 6A 00 83 EC 10" ) + 0x2 );

	erase_end;
}

template <typename T>
T* get_interface( const HMODULE module, std::string szinterface )
{
	_( CreateInterface, "CreateInterface" );
	typedef T* ( *create_interface_fn )( const char* szname, int ret );
	const create_interface_fn create_interface = static_cast< create_interface_fn >( GetProcAddress( module, CreateInterface ) );

	return create_interface( szinterface.c_str(), 0 );
}

void __forceinline __cdecl init::get_interfaces()
{
	_( engine_s, "engine.dll" );
	const auto engine = util::get_module( engine_s );
	_( client_s, "client.dll" );
	const auto client = util::get_module( client_s );
	_( materialsystem_s, "materialsystem.dll" );
	const auto materialsystem = util::get_module( materialsystem_s );
	_( vstdlib_s, "vstdlib.dll" );
	const auto vstdlib = util::get_module( vstdlib_s );
	_( vphysics_s, "vphysics.dll" );
	const auto vphysics = util::get_module( vphysics_s );
	_( vguimatsurface_s, "vguimatsurface.dll" );
	const auto vguimatsurface = util::get_module( vguimatsurface_s );
	_( vgui2_s, "vgui2.dll" );
	const auto vgui2 = util::get_module( vgui2_s );
	_( localize_s, "localize.dll" );
	const auto localize = util::get_module( localize_s );


	_( VClient018, "VClient018" );
	g_pClient = util::get_interface<IBaseClientDll>( client, VClient018 );
	_( VClientEntityList003, "VClientEntityList003" );
	g_pEntitiyList = util::get_interface<IClientEntityList>( client, VClientEntityList003 );
	g_pClientMode = **reinterpret_cast< IClientModeShared*** > ( ( *reinterpret_cast< uintptr_t** > ( g_pClient ) )[ 10 ] + 0x5 );
	_( VEngineClient014, "VEngineClient014" );
	g_pEngine = util::get_interface<IEngineClient>( engine, VEngineClient014 );
	_( VClientPrediction001, "VClientPrediction001" );
	g_pPrediction = util::get_interface<IPrediction>( client, VClientPrediction001 );
	g_pMoveHelper = **reinterpret_cast< IMoveHelper*** > ( offsets::move_helper );
	_( GameMovement001, "GameMovement001" );
	g_pGameMovement = util::get_interface<IGameMovement>( client, GameMovement001 );
	g_pGlobals = **reinterpret_cast< IGlobalVarsBase*** > ( ( *reinterpret_cast< uintptr_t** > ( g_pClient ) )[ 0 ] + 0x1B );
	_( VModelInfoClient004, "VModelInfoClient004" );
	g_pModelInfo = util::get_interface<IVModelInfo>( engine, VModelInfoClient004 );
	_( VEngineModel016, "VEngineModel016" );
	g_pModelRender = util::get_interface<IVModelRender>( engine, VEngineModel016 );
	_( VMaterialSystem080, "VMaterialSystem080" );
	g_pMaterialSystem = util::get_interface<IMaterialSystem>( materialsystem, VMaterialSystem080 );
	_( VEngineRenderView014, "VEngineRenderView014" );
	g_pRenderView = util::get_interface<IVRenderView>( engine, VEngineRenderView014 );
	_( VEngineCvar007, "VEngineCvar007" );
	g_pCVar = util::get_interface<ICVar>( vstdlib, VEngineCvar007 );
	g_pViewRender = **reinterpret_cast< IViewRender*** > ( offsets::view_render );
	g_pBeams = *reinterpret_cast< IViewRenderBeams** > ( offsets::view_render_bms );
	_( GAMEEVENTSMANAGER002, "GAMEEVENTSMANAGER002" );
	g_pGameEventManager = util::get_interface<IGameEventManager2>( engine, GAMEEVENTSMANAGER002 );
	g_pInput = *reinterpret_cast< IInput** > ( ( *reinterpret_cast< uintptr_t** >( g_pClient ) )[ 15 ] + 0x1 );
	_( VPhysicsSurfaceProps001, "VPhysicsSurfaceProps001" );
	g_pSurfaceProps = util::get_interface<IPhysicsSurfaceProps>( vphysics, VPhysicsSurfaceProps001 );
	_( EngineTraceClient004, "EngineTraceClient004" );
	g_pTrace = util::get_interface<IEngineTrace>( engine, EngineTraceClient004 );
	g_pClientState = **reinterpret_cast< IClientState*** > ( ( *reinterpret_cast< uintptr_t** > ( g_pEngine ) )[ 12 ] + 0x10 );
	_( VGUI_Surface031, "VGUI_Surface031" );
	g_pSurface = util::get_interface<ISurface>( vguimatsurface, VGUI_Surface031 );
	_( tier0, "tier0.dll" );
	_( g_pMemAlloc_s, "g_pMemAlloc" );
	g_pMemAlloc = *reinterpret_cast< IMemAlloc** >( GetProcAddress( GetModuleHandleA( tier0 ), g_pMemAlloc_s ) );
	_( VDebugOverlay004, "VDebugOverlay004" );
	g_pDebugOverlay = util::get_interface<IVDebugOverlay>( engine, VDebugOverlay004 );
	g_pGlowObjectManager = *reinterpret_cast< CGlowObjectManager** > ( offsets::glow_obj_manager );
	_( VGUI_Panel009, "VGUI_Panel009" );
	g_pPanel = util::get_interface<IVPanel>( vgui2, VGUI_Panel009 );
	_( IEngineSoundClient003, "IEngineSoundClient003" );
	g_pEngineSound = util::get_interface<IEngineSound>( engine, IEngineSoundClient003 );
	_( RandomFloat, "RandomFloat" );
	random_float = reinterpret_cast< float( *)( float, float ) >( GetProcAddress( GetModuleHandleA( vstdlib_s ), RandomFloat ) );
	_( RandomSeed, "RandomSeed" );
	random_seed = reinterpret_cast< void( *)( unsigned int ) >( GetProcAddress( GetModuleHandleA( vstdlib_s ), RandomSeed ) );
	g_ppGameRules = *reinterpret_cast< C_CSGameRulesProxy*** >( sig( "client.dll", "A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 0F 10 05" ) + 0x1 );
	g_pWeaponSystem = *reinterpret_cast< IWeaponSystem** >( sig( "client.dll", "8B 35 ? ? ? ? FF 10 0F B7 C0" ) + 0x2 );
	g_pCSInventoryManager = *reinterpret_cast< CSInventoryManager** >( sig( "client.dll", "B9 ?? ?? ?? ?? 8D 44 24 10 89 54 24 14" ) + 0x1 );
	_( Localize_001, "Localize_001" );
	g_pLocalize = util::get_interface<ILocalize>( localize, Localize_001 );
	g_pHud = *reinterpret_cast< CHud** >( sig( "client.dll", "B9 ? ? ? ? 56 68 ? ? ? ? E8 ? ? ? ?" ) + 0x1 );
	_( SFHudDeathNoticeAndBotStatus_s, "SFHudDeathNoticeAndBotStatus" );
	g_pDeathNotice = reinterpret_cast< SFHudDeathNoticeAndBotStatus* >( g_pHud->FindElement( SFHudDeathNoticeAndBotStatus_s ) );
	g_ppPlayerResource = *reinterpret_cast< C_CSPlayerResource*** >( sig( "client.dll", "A1 ? ? ? ? 57 85 C0 74 08" ) + 0x1 );

	erase_end;
}

void init::get_materials()
{
	chams::get();
	_u( tex_group, TEXTURE_GROUP_MODEL );

	_( cstm_record, "cstm_record" );
	chams::get().add_material( material_index_t::MAT_RECORD, g_pMaterialSystem->FindMaterial( cstm_record, tex_group ) );
	_( cstm_visible, "cstm_visible" );
	chams::get().add_material( material_index_t::MAT_VISIBLE, g_pMaterialSystem->FindMaterial( cstm_visible, tex_group ) );
	_( cstm_visible_flat, "cstm_visible_flat" );
	chams::get().add_material( material_index_t::MAT_VISIBLE_FLAT, g_pMaterialSystem->FindMaterial( cstm_visible_flat, tex_group ) );
	_( cstm_visible_sh, "cstm_visible_sh" );
	chams::get().add_material( material_index_t::MAT_VISIBLE_SHINE, g_pMaterialSystem->FindMaterial( cstm_visible_sh, tex_group ) );
	_( cstm_hid, "cstm_hid" );
	chams::get().add_material( material_index_t::MAT_HID, g_pMaterialSystem->FindMaterial( cstm_hid, tex_group ) );
	_( cstm_hid_flat, "cstm_hid_flat" );
	chams::get().add_material( material_index_t::MAT_HID_FLAT, g_pMaterialSystem->FindMaterial( cstm_hid_flat, tex_group ) );
	_( cstm_hid_sh, "cstm_hid_sh" );
	chams::get().add_material( material_index_t::MAT_HID_SHINE, g_pMaterialSystem->FindMaterial( cstm_hid_sh, tex_group ) );

	erase_end;
}

using namespace hooks;
void init::hooks()
{
	client = new c_hook( g_pClient );
	clientmode = new c_hook( g_pClientMode );
	engine = new c_hook( g_pEngine );
	modelrender = new c_hook( g_pModelRender );
	viewrender = new c_hook( g_pViewRender );
	enginetrace = new c_hook( g_pTrace );
	renderview = new c_hook( g_pRenderView );
	vpanel = new c_hook( g_pPanel );
	device = new c_hook( reinterpret_cast< void* >( offsets::device ) );
	fire_bullets = new c_hook( reinterpret_cast< void* >( offsets::fire_bullets ) );
	engine_sound = new c_hook( g_pEngineSound );
	surface = new c_hook( g_pSurface );
	death_notice = new c_hook( g_pDeathNotice );
	clientstateplus8 = new c_bphook( reinterpret_cast< void* >( *reinterpret_cast< uintptr_t** >( reinterpret_cast< uintptr_t >( g_pClientState ) + 0x8 ) ) );
	ccsplayer = new c_bphook( *reinterpret_cast< uintptr_t** >( offsets::ccsplayer_vtable ) );
	cbasesplayeranimstate = new c_bphook( *reinterpret_cast< uintptr_t** >( offsets::cbaseplayeranimstate_vtable ) );
	ccsplayerrenderable = new c_bphook( *reinterpret_cast< uintptr_t** >( offsets::ccsplayer_vtable + 7 ) );
	_( sv_cheats_s, "sv_cheats" );
	const auto con_sv_cheats = g_pCVar->FindVar( sv_cheats_s );
	sv_cheats = new c_hook( con_sv_cheats );
	_( weapon_debug_spread_show_s, "weapon_debug_spread_show" );
	const auto con_debug_spread_show = g_pCVar->FindVar( weapon_debug_spread_show_s );
	debug_spread_show = new c_hook( con_debug_spread_show );
	_( net_showfragments_s, "net_showfragments" );
	const auto con_net_showfragments = g_pCVar->FindVar( net_showfragments_s );
	net_showfragments = new c_hook( con_net_showfragments );
	const auto smokeeffect_prop = CSmokeGrenadeProjectile::get_smoke_effect_prop();
	smoke_prop = new recv_prop_hook( smokeeffect_prop );
	const auto m_angEyeAngles0Prop = C_CSPlayer::get_eye_angles0_prop();
	eye0_prop = new recv_prop_hook( m_angEyeAngles0Prop );
	const auto m_angEyeAngles1Prop = C_CSPlayer::get_eye_angles1_prop();
	eye1_prop = new recv_prop_hook( m_angEyeAngles1Prop );
	const auto m_flLowerBodyYawTargetProp = C_CSPlayer::get_lby_prop();
	lby_prop = new recv_prop_hook( m_flLowerBodyYawTargetProp );
	const auto view_sequence_prop = C_BaseViewModel::get_sequen_prop();
	sequence_prop = new recv_prop_hook( view_sequence_prop );
	const auto rot_prop = C_BaseEntity::get_rotation_prop();
	rotation_prop = new recv_prop_hook( rot_prop );

	orig_frame_stage_notify = client->apply<frame_stage_notify_fn>( 36, frame_stage_notify );
	orig_create_move = clientmode->apply<create_move_fn>( 24, create_move );
	orig_draw_model_execute = modelrender->apply<draw_model_execute_fn>( 21, draw_model_execute );
	orig_render_view = viewrender->apply<render_view_fn>( 6, render_view );
	orig_is_box_visible = engine->apply<is_box_visible_fn>( 32, is_box_visible );
	orig_scene_end = renderview->apply<scene_end_fn>( 9, scene_end );
	orig_do_post_screen_space_effects = clientmode->apply<do_post_screen_space_effects_fn>( 44, do_post_screen_space_effects );
	orig_reset = device->apply<reset_fn>( 16, reset );
	orig_present = device->apply<present_fn>( 17, present );
	orig_debug_spread_show_get_int = debug_spread_show->apply<get_int_fn>( 13, debug_spread_show_get_int );
	orig_sv_cheats_get_bool = sv_cheats->apply<get_bool_fn>( 13, sv_cheats_get_bool );
	orig_net_showfragments_get_bool = net_showfragments->apply<get_bool_fn>( 13, net_showfragments_get_bool );
	orig_is_hltv = engine->apply<is_hltv_fn>( 93, is_hltv );
	orig_paint_traverse = vpanel->apply<paint_traverse_fn>( 41, paint_traverse );
	orig_override_view = clientmode->apply<override_view_fn>( 18, override_view );
	orig_emit_sound = engine_sound->apply<emit_sound_fn>( 5, emit_sound );
	orig_temp_entities = clientstateplus8->apply<temp_entities_fn>( 36, temp_entities );
	orig_fire_bullets_postdataupdate = fire_bullets->apply<fire_bullets_postdataupdate_fn>( 7, naked_fire_bullets_postdataupdate );
	orig_do_extra_bone_processing = ccsplayer->apply<do_extra_bone_processing_fn>( 192, do_extra_bone_processing );
	orig_eye_angles = ccsplayer->apply<eye_angles_fn>( 164, eye_angles );
	orig_play_sound = surface->apply<play_sound_fn>( 82, play_sound );
	orig_is_connected = engine->apply<is_connected_fn>( 27, is_connected );
	orig_process_input = death_notice->apply<push_notice_call_fn>( 9, process_input );
	orig_standard_blending_rules = ccsplayer->apply<standard_blending_rules_fn>( 200, standard_blending_rules );
	orig_do_animation_event = cbasesplayeranimstate->apply<do_animation_event_fn>( 6, do_animation_event );
	//orig_maintain_sequence_transitions = ccsplayer->apply<maintain_sequence_transitions_fn>( 200, maintain_sequence_transitions );
	orig_fire_event = engine->apply<fire_event_fn>( 59, fire_event );
	orig_trace_ray = enginetrace->apply<trace_ray_fn>(5, trace_ray );

	orig_smoke_effect = smoke_prop->hook_prop( smoke_effect );
	orig_m_angEyeAngles0Proxy = eye0_prop->hook_prop( m_angEyeAngles0Proxy );
	orig_m_angEyeAngles1Proxy = eye1_prop->hook_prop( m_angEyeAngles1Proxy );
	orig_m_flLowerBodyYawTargetProxy = lby_prop->hook_prop( m_flLowerBodyYawTargetProxy );
	orig_sequence = sequence_prop->hook_prop( sequence );
	orig_m_angRotationProxy = rotation_prop->hook_prop( m_angRotationProxy );

	erase_end;
}

void init::reset_hooks()
{
	g_unload = true;

	inventorychanger::get().get_inventory()->ClearInventory();

	Sleep( 100 );

	SetWindowLongPtr( window, GWL_WNDPROC, reinterpret_cast< LONG_PTR >( orig_wnd_proc ) );

	listeners::get().remove_listeners();

	delete smoke_prop;
	delete eye0_prop;
	delete eye1_prop;
	delete lby_prop;
	delete sequence_prop;
}