#include "../include_cheat.h"
#include <fstream>
#include <cctype>

bool strstric( const std::string & strHaystack, const std::string & strNeedle )
{
	auto it = std::search(
		strHaystack.begin(), strHaystack.end(),
		strNeedle.begin(), strNeedle.end(),
		[]( char ch1, char ch2 ) { return std::toupper( ch1 ) == std::toupper( ch2 ); }
	);
	return ( it != strHaystack.end() );
}

void chams::extend_drawing()
{
	if ( !vars::visuals.chams.enabled.get<bool>() || !vars::visuals.chams.hidden.get<bool>() || !vars::visuals.chams.extended.get<bool>() )
		return;

	for ( auto i = 1; i < g_pGlobals->maxClients; i++ )
	{
		auto player = get_entity( i );
		if ( !player || !player->get_alive() || player->IsDormant() || !player->is_enemy() || player == g_pLocalPlayer )
			continue;

		player->DrawModel( STUDIO_RENDER, 255 );
	}
}

void chams::draw_record( IMatRenderContext * context, const DrawModelState_t & state, const ModelRenderInfo_t & info, matrix3x4_t * custom_bone_to_world )
{
	if ( !info.pModel || !g_pLocalPlayer )
		return hooks::orig_draw_model_execute( g_pModelRender, context, state, info, custom_bone_to_world );

	const auto name = g_pModelInfo->GetModelName( info.pModel );

	_( sleeve, "sleeve" );
	if ( strstr( name, sleeve ) )
		return;

	const auto player = get_entity( info.entity_index );
	if ( !player || player->GetClientClass()->m_ClassID != 35 )
		return hooks::orig_draw_model_execute( g_pModelRender, context, state, info, custom_bone_to_world );

	if ( player == g_pLocalPlayer && !g_in_lp_draw )
		return;

	if ( player == g_pLocalPlayer )
	{
		g_pModelRender->ForcedMaterialOverride();
		float a[ 3 ] = { 1.f,1.f,1.f };
		g_pRenderView->SetColorModulation( a );
	}


	if ( vars::misc.transparency_in_scope.get<float>() && player == g_pLocalPlayer && g_pLocalPlayer->get_scoped() )
		g_pRenderView->SetBlend( 1.f - vars::misc.transparency_in_scope.get<float>() * 0.01f );

	if ( !vars::visuals.chams.enabled.get<bool>() )
		return hooks::orig_draw_model_execute( g_pModelRender, context, state, info, custom_bone_to_world );

	if ( strlen( name ) > 19 && name[ 13 ] == '/' && name[ 7 ] == 'p' &&  name[ 0 ] == 'm' )
	{
		auto & log = player_log::get().get_log( player->EntIndex() );
		auto& records = log.record;
		if ( log.player && log.player->is_enemy() && vars::visuals.chams.history.enabled.get<bool>() && records.size() > 1 && records.size() < 105 && log.lastvalid_vis > 0 && log.lastvalid_vis < records.size() )
		{
			matrix3x4_t matrix[ 128 ];
			if ( draw_hst( records[ log.lastvalid_vis ], matrix ) )
			{
				hooks::orig_draw_model_execute( g_pModelRender, context, state, info, matrix );
				g_pModelRender->ForcedMaterialOverride();
			}
		}

		if ( !draw_ch( context, state, info, custom_bone_to_world ) )
			return hooks::orig_draw_model_execute( g_pModelRender, context, state, info, custom_bone_to_world );
	}

	return hooks::orig_draw_model_execute( g_pModelRender, context, state, info, custom_bone_to_world );
}

void chams::add_material( const material_index_t idx, IMaterial* mat )
{
	materials[ idx ] = mat;
}

bool chams::draw_hst( lag_record_t& record, matrix3x4_t* out )
{
	const auto player = get_entity( record.index );

	if ( !player )
		return false;

	auto & log = player_log::get().get_log( record.index );

	if ( log.lastvalid_vis + 1 >= log.record.size() )
		return false;

	const auto itp1 = &log.record[ log.lastvalid_vis ];
	const auto it = &log.record[ log.lastvalid_vis + 1 ];

	if ( it->m_dormant )
		return false;

	if ( ( itp1 )->m_sim_time - it->m_sim_time > 0.5f )
		return false;

	const auto nci = g_pEngine->GetNetChannelInfo();

	bool end = log.lastvalid_vis == 0;
	Vector next = end ? player->get_origin() : ( itp1 )->m_origin;
	float  time_next = end ? player->get_simtime() : ( itp1 )->m_sim_time;

	const float correct = nci->GetLatency( FLOW_OUTGOING ) + nci->GetLatency( FLOW_INCOMING ) + lagcomp::get_lerp_time() - ( it->m_sim_time - ( itp1 )->m_sim_time );
	float time_delta = time_next - it->m_sim_time;
	float add = end ? 0.2f : time_delta;
	float deadtime = it->m_sim_time + correct + add;

	float curtime = g_pGlobals->curtime;
	float delta = deadtime - curtime;

	float mul = 1.f / add;

	Vector lerp = math::lerp( next, it->m_origin, std::clamp( delta * mul, 0.f, 1.f ) );

	if ( ( math::matrix_get_origin( it->matrix[ 1 ] ) - it->m_origin + lerp ).Dist( player->get_abs_origin() ) < 7.5f )
		return false;

	matrix3x4_t ret[ 128 ];
	memcpy( ret, it->matrix, sizeof( matrix3x4_t[ 128 ] ) );

	for ( size_t i{}; i < 128; ++i )
	{
		const Vector matrix_delta = math::matrix_get_origin( it->matrix[ i ] ) - it->m_origin;
		math::matrix_set_origin( matrix_delta + lerp, ret[ i ] );
	}

	memcpy( out, ret, sizeof( matrix3x4_t[ 128 ] ) );

	auto col = Color( vars::visuals.chams.history.color.get<D3DCOLOR>() );
	g_pRenderView->SetColorModulation( col.Base() );
	g_pRenderView->SetBlend( col.aBase() );
	g_pModelRender->ForcedMaterialOverride( materials[ material_index_t::MAT_RECORD ] );

	return true;
}

bool chams::draw_ch( IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld )
{
	auto player = get_entity( info.entity_index );

	if ( !vars::visuals.chams.local.enabled.get<bool>() && player == g_pLocalPlayer )
		return false;

	if ( !vars::visuals.chams.enemy.enabled.get<bool>() && player->is_enemy() )
		return false;

	if ( !vars::visuals.chams.team.enabled.get<bool>() && !player->is_enemy() && player != g_pLocalPlayer )
		return false;

	IMaterial* material;
	Color col;
	Color shine_col;
	auto shine = false;

	_( envmaptint, "$envmaptint" );

	if ( vars::visuals.chams.hidden.get<bool>() && player != g_pLocalPlayer )
	{
		if ( player->is_enemy() )
		{
			const auto type = vars::visuals.chams.enemy.type.get<int>() + 3;
			shine = type == MAT_HID_SHINE || type == MAT_VISIBLE_SHINE;
			material = materials[ type ];
			col = Color( vars::visuals.chams.enemy.hidden_color.get<D3DCOLOR>() );
			if ( shine )
				shine_col = Color( vars::visuals.chams.enemy.color_shine.get<D3DCOLOR>() );
		}
		else
		{
			const auto type = vars::visuals.chams.enemy.type.get<int>() + 3;
			shine = type == MAT_HID_SHINE || type == MAT_VISIBLE_SHINE;
			material = materials[ type ];
			col = Color( vars::visuals.chams.team.hidden_color.get<D3DCOLOR>() );
			if ( shine )
				shine_col = Color( vars::visuals.chams.team.color_shine.get<D3DCOLOR>() );
		}

		if ( shine )
		{

			auto var = material->FindVar( envmaptint, nullptr );
			var->set_vec_val( shine_col.rBase(), shine_col.gBase(), shine_col.bBase() );
		}
		g_pRenderView->SetColorModulation( col.Base() );
		g_pRenderView->SetBlend( col.aBase() );
		g_pModelRender->ForcedMaterialOverride( material );
		hooks::orig_draw_model_execute( g_pModelRender, context, state, info, pCustomBoneToWorld );
	}

	auto transparency = 0.f;
	if ( player == g_pLocalPlayer )
	{
		const auto type = vars::visuals.chams.local.type.get<int>();
		shine = type == MAT_HID_SHINE || type == MAT_VISIBLE_SHINE;
		material = materials[ type ];
		col = Color( vars::visuals.chams.local.color.get<D3DCOLOR>() );
		transparency = 100.f - col.aBase() * 100.f;
		if ( g_pLocalPlayer->get_scoped() && vars::misc.transparency_in_scope.get<float>() )
			transparency += ( col.aBase() * 100 ) * ( vars::misc.transparency_in_scope.get<float>() / 100 );
		transparency = std::clamp( transparency, 0.f, 100.f );
		if ( shine )
			shine_col = Color( vars::visuals.chams.local.color_shine.get<D3DCOLOR>() );

	}
	else if ( player->is_enemy() )
	{
		const auto type = vars::visuals.chams.enemy.type.get<int>();
		shine = type == MAT_HID_SHINE || type == MAT_VISIBLE_SHINE;
		material = materials[ type ];
		col = Color( vars::visuals.chams.enemy.color.get<D3DCOLOR>() );
		if ( shine )
			shine_col = Color( vars::visuals.chams.enemy.color_shine.get<D3DCOLOR>() );
	}
	else
	{
		const auto type = vars::visuals.chams.team.type.get<int>();
		shine = type == MAT_HID_SHINE || type == MAT_VISIBLE_SHINE;
		material = materials[ type ];
		col = Color( vars::visuals.chams.team.color.get<D3DCOLOR>() );
		if ( shine )
			shine_col = Color( vars::visuals.chams.team.color_shine.get<D3DCOLOR>() );
	}

	if ( shine )
	{
		auto var = material->FindVar( envmaptint, nullptr );
		var->set_vec_val( shine_col.rBase(), shine_col.gBase(), shine_col.bBase() );
	}
	g_pRenderView->SetColorModulation( col.Base() );
	g_pRenderView->SetBlend( 1.f - transparency * 0.01f );
	g_pModelRender->ForcedMaterialOverride( material );
	hooks::orig_draw_model_execute( g_pModelRender, context, state, info, pCustomBoneToWorld );
	return true;
}

void chams::dark_mode() const
{

	const auto reset = [ & ]()
	{
		_( r_DrawSpecificStaticProp, "r_DrawSpecificStaticProp" );
		g_pCVar->FindVar( r_DrawSpecificStaticProp )->set_value( 1 );

		_( World, "World" );
		_( SkyBox, "SkyBox" );
		_( StaticProp, "StaticProp" );

		for ( auto i = g_pMaterialSystem->FirstMaterial(); i != g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem->NextMaterial( i ) )
		{
			auto mat = g_pMaterialSystem->GetMaterial( i );
			if ( !mat )
				continue;

			if ( mat->IsErrorMaterial() )
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();


			if ( strstr( tex_name, World ) || strstr( tex_name, StaticProp ) || strstr( tex_name, SkyBox ) )
			{
				mat->ColorModulate( 1.f, 1.f, 1.f );
				mat->AlphaModulate( 1.f );
			}
		}
	};

	_( sky_csgo_night02, "sky_csgo_night02" );
	const auto set = [ & ]()
	{
		static auto load_named_sky = reinterpret_cast< void( __fastcall* )( const char* ) >( sig( "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45" ) );
		load_named_sky( sky_csgo_night02 );

		_( r_DrawSpecificStaticProp, "r_DrawSpecificStaticProp" );
		g_pCVar->FindVar( r_DrawSpecificStaticProp )->set_value( 0 );

		_( World, "World" );
		_( SkyBox, "SkyBox" );
		_( StaticProp, "StaticProp" );
		_( wall, "wall" );

		for ( auto i = g_pMaterialSystem->FirstMaterial(); i != g_pMaterialSystem->InvalidMaterial(); i = g_pMaterialSystem->NextMaterial( i ) )
		{
			auto mat = g_pMaterialSystem->GetMaterial( i );
			if ( !mat )
				continue;

			if ( mat->IsErrorMaterial() )
				continue;

			std::string name = mat->GetName();
			auto tex_name = mat->GetTextureGroupName();

			if ( vars::visuals.dark.get<bool>() && strstr( tex_name, World ) )
			{
				mat->ColorModulate( 0.15f, 0.15f, 0.15f );
			}
			if ( strstr( tex_name, StaticProp ) )
			{
				/*if ( !strstric( name, "box" ) && !strstric( name, "crate" ) && !strstric( name, "door" ) && !
					strstric( name, "stoneblock" ) && !strstric( name, "tree" ) && !strstric( name, "flower" ) && !
					strstric( name, "light" ) && !strstric( name, "lamp" ) && !strstric( name, "props_junk" ) && !
					strstric( name, "props_pipe" ) && !strstric( name, "chair" ) && !strstric( name, "furniture" ) && !
					strstric( name, "debris" ) && !strstric( name, "tire" ) && !strstric( name, "refrigerator" ) && !
					strstric( name, "fence" ) && !strstric( name, "pallet" ) && !strstric( name, "barrel" ) && !strstric(
						 name, "wagon" ) && !strstric( name, "wood" ) && !strstric( name, "wall" ) && !strstric( name, "pole" ) && !strstric( name, "props_urban" ) && !strstric( name, "bench" ) && !strstric( name, "trashcan" ) && !strstric( name, "infwll" ) && !strstric( name, "cash_register" ) && !strstric( name, "prop_vehicles" ) && !strstric( name, "rocks" ) && !strstric( name, "artillery" ) && !strstric( name, "plaster_brick" ) && !strstric( name, "props_interiors" ) && !strstric( name, "props_farm" ) && !strstric( name, "props_highway" ) && !strstric( name, "orange" ) && !strstric( name, "wheel" ) )
					continue;*/

				if ( vars::visuals.dark.get<bool>() )
					mat->ColorModulate( 0.4f, 0.4f, 0.4f );
				if ( /*!strstric( name, "wood" ) &&*/ !strstric( name, wall ) )
					mat->AlphaModulate( 1.f - vars::visuals.prop_transparency.get<float>() * 0.01f );
			}

			if ( vars::visuals.dark.get<bool>() && strstr( tex_name, SkyBox ) )
			{
				mat->ColorModulate( 228.f / 255.f, 35.f / 255.f, 157.f / 255.f );
			}

		}


	};

	static auto done = true;
	static auto last_setting = false;
	static auto last_transparency = 0.f;
	static auto was_ingame = false;
	static auto unloaded = false;

	if ( !done )
	{
		if ( last_setting || last_transparency )
		{
			reset();
			set();
			done = true;
		}
		else
		{
			reset();
			done = true;
		}
	}

	if ( unloaded )
		return;

	if ( g_unload || was_ingame != g_pEngine->IsInGame() || last_setting != vars::visuals.dark.get<bool>() || last_transparency != vars::visuals.prop_transparency.get<float>() )
	{
		last_setting = vars::visuals.dark.get<bool>();
		last_transparency = vars::visuals.prop_transparency.get<float>();
		was_ingame = g_pEngine->IsInGame();
		unloaded = g_unload;

		if ( g_unload )
			last_setting = false;

		done = false;
	}
}

chams::chams()
{
	_( cstm_record, "csgo\\materials\\cstm_record.vmt" );
	__( cstm_record_s, R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#" );

	std::ofstream( cstm_record ) << cstm_record_s;

	_( cstm_visible, "csgo\\materials\\cstm_visible.vmt" );
	__( cstm_visible_s, R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       "env_cubemap"
  "$envmaptint"   "[0.2 0.2 0.2]"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#" );
	std::ofstream( cstm_visible ) << cstm_visible_s;


	_( cstm_hid, "csgo\\materials\\cstm_hid.vmt" );
	__( cstm_hid_s, R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       "env_cubemap"
  "$envmaptint"   "[0.2 0.2 0.2]"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#" );

	std::ofstream( cstm_hid ) << cstm_hid_s;


	_( cstm_visible_sh, "csgo\\materials\\cstm_visible_sh.vmt" );
	__( cstm_visible_sh_s, R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       "env_cubemap"
  "$envmaptint"   "[0.6 0.6 0.6]"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#" );


	std::ofstream( cstm_visible_sh ) << cstm_visible_sh_s;


	_( cstm_hid_sh, "csgo\\materials\\cstm_hid_sh.vmt" );
	__( cstm_hid_sh_s, R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       "env_cubemap"
  "$envmaptint"   "[0.6 0.6 0.6]"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#" );

	std::ofstream( cstm_hid_sh ) << cstm_hid_sh_s;

	_( cstm_visible_flat, "csgo\\materials\\cstm_visible_flat.vmt" );
	__( cstm_visible_flat_s, R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#" );

	std::ofstream( cstm_visible_flat ) << cstm_visible_flat_s;

	_( cstm_hid_flat, "csgo\\materials\\cstm_hid_flat.vmt" );
	__( cstm_hid_flat_s, R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#" );

	std::ofstream( cstm_hid_flat ) << cstm_hid_flat_s;
}

chams::~chams()
{
	__( cstm_record, "csgo\\materials\\cstm_record.vmt" );
	std::remove( cstm_record );
	__( cstm_visible, "csgo\\materials\\cstm_visible.vmt" );
	std::remove( cstm_visible );
	__( cstm_hid, "csgo\\materials\\cstm_hid.vmt" );
	std::remove( cstm_hid );
	__( cstm_visible_sh, "csgo\\materials\\cstm_visible_sh.vmt" );
	std::remove( cstm_visible_sh );
	__( cstm_hid_sh, "csgo\\materials\\cstm_hid_sh.vmt" );
	std::remove( cstm_hid_sh );
	__( cstm_visible_flat, "csgo\\materials\\cstm_visible_flat.vmt" );
	std::remove( cstm_visible_flat );
	__( cstm_hid_flat, "csgo\\materials\\cstm_hid_flat.vmt" );
	std::remove( cstm_hid_flat );
}
