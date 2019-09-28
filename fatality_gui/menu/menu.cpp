#include "external_include.h"

std::vector< std::string > m_slots =
{
	_w( "Config slot #1" ),
	_w( "Config slot #2" ),
	_w( "Config slot #3" ),
	_w( "Config slot #4" ),
	_w( "Config slot #5" ),
	_w( "Config slot #6" )
};

std::vector< std::string > m_sounds =
{
	_w( "bubble" ),
	_w( "cod" ),
	_w( "fatality" ),
	_w( "arena_switch" ),
};

std::vector< std::string > m_knives =
{
	_w( "Default" ),
	_w( "Karambit" ),
	_w( "M9 Bayonet" ),
	_w( "Butterfly" ),
	_w( "Bayonet" ),
	_w( "Bowie Knife" ),
	_w( "Gut Knife" ),
	_w( "Flip Knife" ),
	_w( "Falchion" ),
	_w( "Shadow Daggers" )
};

std::vector<std::string> m_baim =
{
	_w( "Off" ),
	_w( "Adaptive" ),
};

std::vector<std::string> m_baim_when =
{
	_w( "In air" ),
	_w( "Lethal" ),
	_w( "Resolver unsure" ),
};

std::vector<std::string> m_baim_when_au =
{
	_w( "aim_au_bm_air" ),
	_w( "aim_au_bm_l" ),
	_w( "aim_au_bm_ru" )
};

std::vector<std::string> m_baim_when_sc =
{
	_w( "aim_sc_bm_air" ),
	_w( "aim_sc_bm_l" ),
	_w( "aim_sc_bm_ru" )
};

std::vector<std::string> m_baim_when_awp =
{
	_w( "aim_awp_bm_air" ),
	_w( "aim_awp_bm_l" ),
	_w( "aim_awp_bm_ru" )
};

std::vector<std::string> m_baim_when_hp =
{
	_w( "aim_hp_bm_air" ),
	_w( "aim_hp_bm_l" ),
	_w( "aim_hp_bm_ru" )
};

std::vector<std::string> m_baim_when_p =
{
	_w( "aim_p_bm_air" ),
	_w( "aim_p_bm_l" ),
	_w( "aim_p_bm_ru" )
};

std::vector<std::string> m_baim_when_o =
{
	_w( "aim_o_bm_air" ),
	_w( "aim_o_bm_l" ),
	_w( "aim_o_bm_ru" )
};

std::vector<std::string> m_chams_type =
{
	_w( "Normal" ),
	_w( "Flat" ),
	_w( "Shiny" )
};

std::vector<std::string> m_autostop_type =
{
	_w( "Off" ),
	_w( "Normal" ),
	_w( "Full" ),
};

std::vector<std::string> m_spread_type =
{
	_w( "Gradient" ),
	_w( "Rainbow" ),
	_w( "Rainbow ( rotating )" )
};

std::vector<std::string> m_world_glow =
{
	_w( "Weapons" ),
_w( "Grenades" ),
_w( "C4" )
};

std::vector<std::string> m_world_glow2 =
{
	_w( "v_gl_w_weap" ),
_w( "v_gl_w_gre" ),
_w( "v_gl_w_c4" )
};

std::vector<std::string> m_choke =
{
	_w( "Peek" ),
	_w( "Reload" ),
	_w( "Weapon switch" ),
	_w( "Shot" )
};

std::vector<std::string> m_choke2 =
{
	_w( "aa_fk_on_peek" ),
	_w( "aa_ck_rl" ),
	_w( "aa_ck_sw" ),
	_w( "aa_ck_sh" )
};

std::vector<std::string> m_world_esp =
{
	 _w( "Weapons" ),
 _w( "Grenades" ),
 _w( "C4" )
};

std::vector<std::string> m_world_esp2 =
{
_w( "v_w_weap" ),
_w( "v_w_gre" ),
 _w( "v_w_c4" )
};

std::vector<std::string> m_info_esp =
{
 _w( "Scoped" ),
_w( "Bomb" )
};

std::vector<std::string> m_info_esp2 =
{
	_w( "v_i_scop" ),
_w( "v_i_bomb" ),
};

std::string weapon_filter;
std::string kit_filter;

void save_config()
{
	_( menu_slot, "menu_slot" );
	config::get().save( g_cfg[ menu_slot ]->get<int>() + 1 );
}

void load_config()
{
	_( menu_slot, "menu_slot" );
	config::get().load( g_cfg[ menu_slot ]->get<int>() + 1 );
}

void inventory_add()
{
	inventorychanger::get().add();
}

void inventory_save()
{
	inventorychanger::get().save();
}

void inventory_rm()
{
	inventorychanger::get().remove();
}

void c_menu::init()
{
	if ( m_init )
		return;

	c_window* main_window = new c_window( _w( "MAIN WINDOW" ), bounds( 100, 100, 860, 600 ), true );

	c_tab* tab_first = main_window->add_tab( _w( "Rage" ) );
	{
		c_subtab* subtab_one = tab_first->add_subtab( _w( "Aimbot" ) );
		{
			subtab_one->add_checkbox( _w( "Enabled" ), _w( "aim" ) );
			subtab_one->add_checkbox( _w( "Autofire" ), _w( "aim_af" ) );
			subtab_one->add_checkbox( _w( "Knife-bot" ), _w( "aim_bot_kn" ) );
			subtab_one->add_checkbox( _w( "Zeusbot" ), _w( "aim_bot_zs" ) );
			subtab_one->add_slider( _w( "Zeus hitchance" ), _w( "aim_zs_hc" ), 0.f, 100.f, _w( "%.2f %%" ) );
			subtab_one->add_checkbox( _w( "Baim on key" ), _w( "aim_bm_ok" ) );
			subtab_one->add_keybind( _w( "" ), _w( "k_bm" ), true );
			subtab_one->add_checkbox( _w( "Extend backtrack range" ), _w( "aim_ps_bs" ) );
			subtab_one->add_checkbox( _w( "Ping spike" ), _w( "aim_ps" ) );
			subtab_one->add_keybind( _w( "" ), _w( "k_ps" ), true );
			subtab_one->add_slider( _w( "Ping spike amount" ), _w( "aim_ps_at" ), 0, 800, _w( "%d ms" ) );
			subtab_one->add_checkbox( _w( "HS only" ), _w( "aim_hsonly" ) );
			subtab_one->add_checkbox( _w( "Backtrack" ), _w( "aim_bt" ) );
			subtab_one->add_checkbox( _w( "Resolver enabled" ), _w( "aim_rslv" ) );
			subtab_one->add_checkbox( _w( "Fakelag compensation" ), _w( "aim_fk_cmp" ) );
			subtab_one->add_checkbox( _w( "Resolve teammates" ), _w( "aim_rslv_tm" ) );
			subtab_one->add_checkbox( _w( "Override resolver" ), _w( "aim_ovr" ) );
			subtab_one->add_keybind( _w( "" ), _w( "k_ovr" ), true );
		} tab_first->pushback_subtab( subtab_one );

		c_subtab* subtab_three = tab_first->add_subtab( _w( "Anti-aim" ) );
		{
			subtab_three->add_checkbox( _w( "Enabled" ), _w( "aa" ) );
			subtab_three->add_checkbox( _w( "Fake lag when disabed" ), _w( "aa_fk_dis" ) );
			subtab_three->add_checkbox( _w( "Fakewalk" ), _w( "aa_fkwk" ) );
			subtab_three->add_keybind( _w( "" ), _w( "k_fkwk" ), true );
			subtab_three->add_multi( _w( "Additional Fakelag" ), &m_choke, &m_choke2 );
			subtab_three->add_checkbox( _w( "Prevent onshot backtrack" ), _w( "aa_onsht" ) );
			subtab_three->add_checkbox( _w( "Indicator" ), _w( "aa_indicator" ) );
			subtab_three->add_colorpicker( _w( "" ), _w( "aa_indicator_col" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ) );
			subtab_three->add_checkbox( _w( "Low performance freestanding" ), _w( "aa_fs_fps" ) );
			subtab_three->add_checkbox( _w( "Antiaim Override" ), _w( "aa_manual" ) );
			subtab_three->add_keybind( _w( "back" ), _w( "k_aa_back" ) );
			subtab_three->add_keybind( _w( "left" ), _w( "k_aa_left" ) );
			subtab_three->add_keybind( _w( "right" ), _w( "k_aa_right" ) );
		} tab_first->pushback_subtab( subtab_three );

		c_subtab* subtab_four = tab_first->add_subtab( _w( "AA Regular" ) );
		{
			subtab_four->add_checkbox( _w( "Enabled" ), _w( "aa_r" ) );
			subtab_four->add_slider( _w( "Add" ), _w( "aa_r_add" ), -180, 180, _w( "%d °" ) );
			subtab_four->add_checkbox( _w( "At fov target" ), _w( "aa_r_at_target" ) );
			subtab_four->add_slider( _w( "Fake lag amount" ), _w( "aa_r_choke" ), 3, 14, _w( "%d ticks" ) );
			subtab_four->add_checkbox( _w( "Spin" ), _w( "aa_r_spin" ) );
			subtab_four->add_slider( _w( "Spin range" ), _w( "aa_r_spin_range" ), 0, 360, _w( "%d °" ) );
			subtab_four->add_slider( _w( "Spin delta" ), _w( "aa_r_spin_speed" ), 0, 30, _w( "%d ° per tick" ) );
			subtab_four->add_checkbox( _w( "Freestand" ), _w( "aa_r_freestand" ) );
			//subtab_four->add_checkbox( _w( "Edge" ), _w( "aa_r_edge" ) );
			subtab_four->add_checkbox( _w( "Jitter" ), _w( "aa_r_jitter" ) );
			subtab_four->add_checkbox( _w( "Random jitter" ), _w( "aa_r_jitter_random" ) );
			subtab_four->add_slider( _w( "Jitter range" ), _w( "aa_r_jitter_range" ), 0, 360, _w( "%d °" ) );
			subtab_four->add_slider( _w( "Jitter swap" ), _w( "aa_r_jitter_speed" ), 0, 64, _w( "%d ° per tick" ) );
			subtab_four->add_checkbox( _w( "Fake lby" ), _w( "aa_r_lby" ) );
			subtab_four->add_checkbox( _w( "Static lby" ), _w( "aa_r_lby_static" ) );
			//subtab_four->add_checkbox( _w( "Autodirection lby" ), _w( "aa_r_lby_autodirection" ) );
			subtab_four->add_slider( _w( "Lby delta" ), _w( "aa_r_lby_delta" ), -180, 180, _w( "%d °" ) );
			subtab_four->add_slider( _w( "Lby delta first break" ), _w( "aa_r_lby_delta_first" ), -180, 180, _w( "%d °" ) );
			subtab_four->add_checkbox( _w( "Random lby delta" ), _w( "aa_r_lby_random" ) );
			subtab_four->add_slider( _w( "Random lby add" ), _w( "aa_r_lby_random_add" ), -180, 180, _w( "%d °" ) );
			subtab_four->add_slider( _w( "Random lby range" ), _w( "aa_r_lby_random_range" ), 0, 360, _w( "%d °" ) );
			//subtab_four->add_checkbox( _w( "Change lby delta when hittable" ), _w( "aa_r_lby_when_hittable" ) );
			//subtab_four->add_checkbox( _w( "Lby delta when hittable" ), _w( "aa_r_lby_delta_when_hittable" ) );
		} tab_first->pushback_subtab( subtab_four );

		c_subtab* subtab_five = tab_first->add_subtab( _w( "AA Moving" ) );
		{
			subtab_five->add_checkbox( _w( "Enabled" ), _w( "aa_m" ) );
			subtab_five->add_slider( _w( "Add" ), _w( "aa_m_add" ), -180, 180, _w( "%d °" ) );
			subtab_five->add_checkbox( _w( "At fov target" ), _w( "aa_m_at_target" ) );
			subtab_five->add_slider( _w( "Fake lag amount" ), _w( "aa_m_choke" ), 3, 14, _w( "%d ticks" ) );
			subtab_five->add_checkbox( _w( "Spin" ), _w( "aa_m_spin" ) );
			subtab_five->add_slider( _w( "Spin range" ), _w( "aa_m_spin_range" ), 0, 360, _w( "%d °" ) );
			subtab_five->add_slider( _w( "Spin delta" ), _w( "aa_m_spin_speed" ), 0, 30, _w( "%d ° per tick" ) );
			subtab_five->add_checkbox( _w( "Freestand" ), _w( "aa_m_freestand" ) );
			//subtab_five->add_checkbox( _w( "Edge" ), _w( "aa_m_edge" ) );
			subtab_five->add_checkbox( _w( "Jitter" ), _w( "aa_m_jitter" ) );
			subtab_five->add_checkbox( _w( "Random jitter" ), _w( "aa_m_jitter_random" ) );
			subtab_five->add_slider( _w( "Jitter range" ), _w( "aa_m_jitter_range" ), 0, 360, _w( "%d °" ) );
			subtab_five->add_slider( _w( "Jitter swap" ), _w( "aa_m_jitter_speed" ), 0, 64, _w( "every %d ticks" ) );
		} tab_first->pushback_subtab( subtab_five );

		c_subtab* subtab_six = tab_first->add_subtab( _w( "AA Air" ) );
		{
			subtab_six->add_checkbox( _w( "Enabled" ), _w( "aa_a" ) );
			subtab_six->add_slider( _w( "Add" ), _w( "aa_a_add" ), -180, 180, _w( "%d °" ) );
			subtab_six->add_checkbox( _w( "At fov target" ), _w( "aa_a_at_target" ) );
			subtab_six->add_slider( _w( "Fake lag amount" ), _w( "aa_a_choke" ), 3, 14, _w( "%d ticks" ) );
			subtab_six->add_checkbox( _w( "Spin" ), _w( "aa_a_spin" ) );
			subtab_six->add_slider( _w( "Spin range" ), _w( "aa_a_spin_range" ), 0, 360, _w( "%d °" ) );
			subtab_six->add_slider( _w( "Spin delta" ), _w( "aa_a_spin_speed" ), 0, 30, _w( "%d ° per tick" ) );
			subtab_six->add_checkbox( _w( "Freestand" ), _w( "aa_a_freestand" ) );
			//subtab_six->add_checkbox( _w( "Edge" ), _w( "aa_a_edge" ) );
			subtab_six->add_checkbox( _w( "Jitter" ), _w( "aa_a_jitter" ) );
			subtab_six->add_checkbox( _w( "Random jitter" ), _w( "aa_a_jitter_random" ) );
			subtab_six->add_slider( _w( "Jitter range" ), _w( "aa_a_jitter_range" ), 0, 360, _w( "%d °" ) );
			subtab_six->add_slider( _w( "Jitter swap" ), _w( "aa_a_jitter_speed" ), 0, 64, _w( "every %d ticks" ) );
		} tab_first->pushback_subtab( subtab_six );

	} main_window->pushback_tab( tab_first );

	c_tab* tab_second = main_window->add_tab( _w( "Config" ) );
	{
		c_subtab* subtab_one = tab_second->add_subtab( _w( "Slots" ) );
		{
			subtab_one->add_combobox( _w( "Slot" ), _w( "menu_slot" ), &m_slots );
			subtab_one->add_button( _w( "Load config" ), load_config );
			subtab_one->add_button( _w( "Save config" ), save_config, true );
		} tab_second->pushback_subtab( subtab_one );
		c_subtab* subtab_two = tab_second->add_subtab( _w( "Auto" ) );
		{
			subtab_two->add_slider( _w( "Minimum hitchance" ), _w( "cfg_au_hc" ), 0.f, 100.f, _w( "%.2f %%" ) );
			subtab_two->add_slider( _w( "Minimum damage" ), _w( "cfg_au_mindmg" ), 0.0f, 100.f, _w( "%.2f dmg" ) );
			subtab_two->add_slider( _w( "Hitbox scale head" ), _w( "cfg_au_scale_head" ), 20, 100, _w( "%d %%" ) );
			subtab_two->add_slider( _w( "Hitbox scale body" ), _w( "cfg_au_scale" ), 20, 100, _w( "%d %%" ) );
			subtab_two->add_checkbox( _w( "Adaptive scale" ), _w( "cfg_au_ad_scale" ) );
			subtab_two->add_checkbox( _w( "Autoscope" ), _w( "cfg_au_asc" ) );
			subtab_two->add_combobox( _w( "Autostop" ), _w( "cfg_au_as" ), &m_autostop_type );
			subtab_two->add_checkbox( _w( "on key" ), _w( "cfg_au_as_ok" ) );
			subtab_two->add_keybind( _w( "" ), _w( "k_au" ), true );
			subtab_two->add_checkbox( _w( "only when lethal" ), _w( "cfg_au_as_ol" ) );
			subtab_two->add_combobox( _w( "Baim mode" ), _w( "cfg_au_baim_md" ), &m_baim );
			subtab_two->add_multi( _w( "Baim when" ), &m_baim_when, &m_baim_when_au );

		} tab_second->pushback_subtab( subtab_two );
		c_subtab* subtab_three = tab_second->add_subtab( _w( "Scout" ) );
		{
			subtab_three->add_slider( _w( "Minimum hitchance" ), _w( "cfg_sc_hc" ), 0.f, 100.f, _w( "%.2f %%" ) );
			subtab_three->add_slider( _w( "Minimum damage" ), _w( "cfg_sc_mindmg" ), 0.0f, 100.f, _w( "%.2f dmg" ) );
			subtab_three->add_slider( _w( "Hitbox scale head" ), _w( "cfg_sc_scale_head" ), 20, 100, _w( "%d %%" ) );
			subtab_three->add_slider( _w( "Hitbox scale body" ), _w( "cfg_sc_scale" ), 20, 100, _w( "%d %%" ) );
			subtab_three->add_checkbox( _w( "Adaptive scale" ), _w( "cfg_sc_ad_scale" ) );
			subtab_three->add_checkbox( _w( "Autoscope" ), _w( "cfg_sc_asc" ) );
			subtab_three->add_combobox( _w( "Autostop" ), _w( "cfg_sc_as" ), &m_autostop_type );
			subtab_three->add_checkbox( _w( "on key" ), _w( "cfg_sc_as_ok" ) );
			subtab_three->add_keybind( _w( "" ), _w( "k_au" ), true );
			subtab_three->add_checkbox( _w( "only when lethal" ), _w( "cfg_sc_as_ol" ) );
			subtab_three->add_checkbox( _w( "Jumpscouting" ), _w( "cfg_sc_js" ) );
			subtab_three->add_combobox( _w( "Baim Mode" ), _w( "cfg_sc_baim_md" ), &m_baim );
			subtab_three->add_multi( _w( "Baim when" ), &m_baim_when, &m_baim_when_sc );
		} tab_second->pushback_subtab( subtab_three );
		c_subtab* subtab_four = tab_second->add_subtab( _w( "AWP" ) );
		{
			subtab_four->add_slider( _w( "Minimum hitchance" ), _w( "cfg_awp_hc" ), 0.f, 100.f, _w( "%.2f %%" ) );
			subtab_four->add_slider( _w( "Minimum damage" ), _w( "cfg_awp_mindmg" ), 0.0f, 100.f, _w( "%.2f dmg" ) );
			subtab_four->add_slider( _w( "Hitbox scale head" ), _w( "cfg_awp_scale_head" ), 20, 100, _w( "%d %%" ) );
			subtab_four->add_slider( _w( "Hitbox scale body" ), _w( "cfg_awp_scale" ), 20, 100, _w( "%d %%" ) );
			subtab_four->add_checkbox( _w( "Adaptive scale" ), _w( "cfg_awp_ad_scale" ) );
			subtab_four->add_checkbox( _w( "Autoscope" ), _w( "cfg_awp_asc" ) );
			subtab_four->add_combobox( _w( "Autostop" ), _w( "cfg_awp_as" ), &m_autostop_type );
			subtab_four->add_checkbox( _w( "on key" ), _w( "cfg_awp_as_ok" ) );
			subtab_four->add_keybind( _w( "" ), _w( "k_au" ), true );
			subtab_four->add_checkbox( _w( "only when lethal" ), _w( "cfg_awp_as_ol" ) );
			subtab_four->add_combobox( _w( "Baim Mode" ), _w( "cfg_awp_baim_md" ), &m_baim );
			subtab_four->add_multi( _w( "Baim when" ), &m_baim_when, &m_baim_when_awp );
		} tab_second->pushback_subtab( subtab_four );
		c_subtab* subtab_five = tab_second->add_subtab( _w( "Heavy pistols" ) );
		{
			subtab_five->add_slider( _w( "Minimum hitchance" ), _w( "cfg_hp_hc" ), 0.f, 100.f, _w( "%.2f %%" ) );
			subtab_five->add_slider( _w( "Minimum damage" ), _w( "cfg_hp_mindmg" ), 0.0f, 100.f, _w( "%.2f dmg" ) );
			subtab_five->add_slider( _w( "Hitbox scale head" ), _w( "cfg_hp_scale_head" ), 20, 100, _w( "%d %%" ) );
			subtab_five->add_slider( _w( "Hitbox scale body" ), _w( "cfg_hp_scale" ), 20, 100, _w( "%d %%" ) );
			subtab_five->add_checkbox( _w( "Adaptive scale" ), _w( "cfg_hp_ad_scale" ) );
			subtab_five->add_combobox( _w( "Autostop" ), _w( "cfg_hp_as" ), &m_autostop_type );
			subtab_five->add_checkbox( _w( "on key" ), _w( "cfg_hp_as_ok" ) );
			subtab_five->add_keybind( _w( "" ), _w( "k_au" ), true );
			subtab_five->add_checkbox( _w( "only when lethal" ), _w( "cfg_hp_as_ol" ) );
			subtab_five->add_checkbox( _w( "Autorevolver" ), _w( "cfg_hp_ar" ) );
			subtab_five->add_combobox( _w( "Baim Mode" ), _w( "cfg_hp_baim_md" ), &m_baim );
			subtab_five->add_multi( _w( "Baim when" ), &m_baim_when, &m_baim_when_hp );
		} tab_second->pushback_subtab( subtab_five );
		c_subtab* subtab_six = tab_second->add_subtab( _w( "Pistols" ) );
		{
			subtab_six->add_slider( _w( "Minimum hitchance" ), _w( "cfg_p_hc" ), 0.f, 100.f, _w( "%.2f %%" ) );
			subtab_six->add_slider( _w( "Minimum damage" ), _w( "cfg_p_mindmg" ), 0.0f, 100.f, _w( "%.2f dmg" ) );
			subtab_six->add_slider( _w( "Hitbox scale head" ), _w( "cfg_p_scale_head" ), 20, 100, _w( "%d %%" ) );
			subtab_six->add_slider( _w( "Hitbox scale body" ), _w( "cfg_p_scale" ), 20, 100, _w( "%d %%" ) );
			subtab_six->add_checkbox( _w( "Adaptive scale" ), _w( "cfg_p_ad_scale" ) );
			subtab_six->add_combobox( _w( "Autostop" ), _w( "cfg_p_as" ), &m_autostop_type );
			subtab_six->add_checkbox( _w( "on key" ), _w( "cfg_p_as_ok" ) );
			subtab_six->add_keybind( _w( "" ), _w( "k_au" ), true );
			subtab_six->add_checkbox( _w( "only when lethal" ), _w( "cfg_p_as_ol" ) );
			subtab_six->add_combobox( _w( "Baim Mode" ), _w( "cfg_p_baim_md" ), &m_baim );
			subtab_six->add_multi( _w( "Baim when" ), &m_baim_when, &m_baim_when_p );
		} tab_second->pushback_subtab( subtab_six );
		c_subtab* subtab_seven = tab_second->add_subtab( _w( "Other" ) );
		{
			subtab_seven->add_slider( _w( "Minimum hitchance" ), _w( "cfg_o_hc" ), 0.f, 100.f, _w( "%.2f %%" ) );
			subtab_seven->add_slider( _w( "Minimum damage" ), _w( "cfg_o_mindmg" ), 0.0f, 100.f, _w( "%.2f dmg" ) );
			subtab_seven->add_slider( _w( "Hitbox scale head" ), _w( "cfg_o_scale_head" ), 20, 100, _w( "%d %%" ) );
			subtab_seven->add_slider( _w( "Hitbox scale body" ), _w( "cfg_o_scale" ), 20, 100, _w( "%d %%" ) );
			subtab_seven->add_checkbox( _w( "Adaptive scale" ), _w( "cfg_o_ad_scale" ) );
			subtab_seven->add_combobox( _w( "Autostop" ), _w( "cfg_o_as" ), &m_autostop_type );
			subtab_seven->add_checkbox( _w( "on key" ), _w( "cfg_o_as_ok" ) );
			subtab_seven->add_keybind( _w( "" ), _w( "k_au" ), true );
			subtab_seven->add_checkbox( _w( "only when lethal" ), _w( "cfg_o_as_ol" ) );
			subtab_seven->add_combobox( _w( "Baim Mode" ), _w( "cfg_o_baim_md" ), &m_baim );
			subtab_seven->add_multi( _w( "Baim when" ), &m_baim_when, &m_baim_when_o );
		} tab_second->pushback_subtab( subtab_seven );
	} main_window->pushback_tab( tab_second );

	c_tab* tab_third = main_window->add_tab( _w( "Visuals" ) );
	{
		c_subtab* subtab_one = tab_third->add_subtab( _w( "Chams" ) );
		{
			subtab_one->add_checkbox( _w( "Enabled" ), _w( "v_ch" ) );
			subtab_one->add_checkbox( _w( "Extend" ), _w( "v_ch_xt" ) );
			subtab_one->add_checkbox( _w( "Hidden" ), _w( "v_ch_hid" ) );
			subtab_one->add_checkbox( _w( "History" ), _w( "v_ch_hst" ) );
			subtab_one->add_colorpicker( _w( "" ), _w( "v_ch_hst_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), false, true );

			subtab_one->add_checkbox( _w( "Team" ), _w( "v_ch_t" ) );
			subtab_one->add_combobox( _w( "Type" ), _w( "v_ch_t_t" ), &m_chams_type );
			subtab_one->add_colorpicker( _w( "" ), _w( "v_ch_t_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ) );
			subtab_one->add_colorpicker( _w( "Hidden" ), _w( "v_ch_hid_t_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true );
			subtab_one->add_colorpicker( _w( "Pearlescent" ), _w( "v_ch_t_cl_sh" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ) );

			subtab_one->add_checkbox( _w( "Enemy" ), _w( "v_ch_e" ) );
			subtab_one->add_combobox( _w( "Type" ), _w( "v_ch_e_t" ), &m_chams_type );
			subtab_one->add_colorpicker( _w( "" ), _w( "v_ch_e_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ) );
			subtab_one->add_colorpicker( _w( "Hidden" ), _w( "v_ch_hid_e_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true );
			subtab_one->add_colorpicker( _w( "Pearlescent" ), _w( "v_ch_e_cl_sh" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ) );

			subtab_one->add_checkbox( _w( "Local" ), _w( "v_ch_loc" ) );
			subtab_one->add_combobox( _w( "Type" ), _w( "v_ch_loc_t" ), &m_chams_type );
			subtab_one->add_colorpicker( _w( "" ), _w( "v_ch_loc_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), false, true );
			subtab_one->add_colorpicker( _w( "Pearlescent" ), _w( "v_ch_loc_cl_sh" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ) );
		} tab_third->pushback_subtab( subtab_one );

		c_subtab* subtab_two = tab_third->add_subtab( _w( "Esp" ) );
		{
			subtab_two->add_checkbox( _w( "Box" ), _w( "v_box" ) );
			subtab_two->add_colorpicker( _w( "" ), _w( "v_box_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true );
			subtab_two->add_checkbox( _w( "Name" ), _w( "v_i_nm" ) );
			subtab_two->add_colorpicker( _w( "" ), _w( "v_i_nm_cl" ), D3DCOLOR_RGBA( 255, 255, 255, 255 ), true );
			subtab_two->add_checkbox( _w( "Weapon" ), _w( "v_i_w_nm" ) );
			subtab_two->add_colorpicker( _w( "" ), _w( "v_i_w_nm_cl" ), D3DCOLOR_RGBA( 255, 255, 255, 255 ), true );
			subtab_two->add_checkbox( _w( "Health bar" ), _w( "v_hp" ) );
			//subtab_two->add_checkbox( xorstr( "Lby update-bar" ), xorstr( "v_lby_t" ) );
			subtab_two->add_checkbox( _w( "Armor bar" ), _w( "v_arm" ) );
			subtab_two->add_colorpicker( _w( "" ), _w( "v_arm_cl" ), D3DCOLOR_RGBA( 0, 152, 204, 255 ), true );
			subtab_two->add_checkbox( _w( "Ammo bar" ), _w( "v_amm" ) );
			subtab_two->add_colorpicker( _w( "" ), _w( "v_amm_cl" ), D3DCOLOR_RGBA( 0, 204, 102, 51 ), true );
			subtab_two->add_checkbox( _w( "Pingspike bar" ), _w( "v_png" ) );
			subtab_two->add_colorpicker( _w( "" ), _w( "v_png_cl" ), D3DCOLOR_RGBA( 204, 0, 204, 51 ), true );
			subtab_two->add_multi( _w( "Info" ), &m_info_esp, &m_info_esp2 );
			subtab_two->add_multi( _w( "World esp" ), &m_world_esp, &m_world_esp2 );
			subtab_two->add_colorpicker( _w( "" ), _w( "v_w_cl" ), D3DCOLOR_RGBA( 255, 255, 255, 255 ), true );
		} tab_third->pushback_subtab( subtab_two );

		c_subtab* subtab_three = tab_third->add_subtab( _w( "Glow" ) );
		{
			subtab_three->add_checkbox( _w( "Enable" ), _w( "v_gl" ) );
			subtab_three->add_checkbox( _w( "Team" ), _w( "v_gl_t" ) );
			subtab_three->add_colorpicker( _w( "" ), _w( "v_gl_t_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );
			subtab_three->add_checkbox( _w( "Enemy" ), _w( "v_gl_e" ) );
			subtab_three->add_colorpicker( _w( "" ), _w( "v_gl_e_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );
			subtab_three->add_checkbox( _w( "Local" ), _w( "v_gl_loc" ) );
			subtab_three->add_colorpicker( _w( "" ), _w( "v_gl_loc_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );
			subtab_three->add_multi( _w( "World glow" ), &m_world_glow, &m_world_glow2 );
			subtab_three->add_colorpicker( _w( "" ), _w( "v_gl_w_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );
		} tab_third->pushback_subtab( subtab_three );

		c_subtab* subtab_four = tab_third->add_subtab( _w( "Misc" ) );
		{

			subtab_four->add_checkbox( _w( "Enable beams" ), _w( "v_bms" ) );
			subtab_four->add_checkbox( _w( "Team" ), _w( "v_bms_t" ) );
			subtab_four->add_colorpicker( _w( "" ), _w( "v_bms_t_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );
			subtab_four->add_checkbox( _w( "Enemy" ), _w( "v_bms_e" ) );
			subtab_four->add_colorpicker( _w( "" ), _w( "v_bms_e_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );
			subtab_four->add_checkbox( _w( "Local" ), _w( "v_bms_loc" ) );
			subtab_four->add_colorpicker( _w( "" ), _w( "v_bms_loc_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );

			subtab_four->add_checkbox( _w( "Darkmode" ), _w( "v_drk" ) );
			subtab_four->add_slider( _w( "Prop Transparency" ), _w( "v_prop_tr" ), 0, 100, _w( "%d %%" ) );
			subtab_four->add_checkbox( _w( "Spread circle" ), _w( "v_spr" ) );
			subtab_four->add_combobox( _w( "Type" ), _w( "v_spr_typ" ), &m_spread_type );
			subtab_four->add_colorpicker( _w( "" ), _w( "v_spr_cl1" ), D3DCOLOR_RGBA( 10, 50, 150, 255 ), false, true );
			subtab_four->add_colorpicker( _w( "" ), _w( "v_spr_cl2" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true, true );
			subtab_four->add_checkbox( _w( "Out of fov arrows" ), _w( "v_oof" ) );
			subtab_four->add_colorpicker( _w( "" ), _w( "v_oof_cl" ), D3DCOLOR_RGBA( 100, 50, 150, 255 ), true );
			subtab_four->add_slider( _w( "Distance" ), _w( "v_oof_dst" ), 1, 100, _w( "%d %%" ) );

			subtab_four->add_checkbox( _w( "Hitmarker" ), _w( "m_hitsnd" ) );
			subtab_four->add_combobox( _w( "Hitsound" ), _w( "m_hitsnd_f" ), &m_sounds, true );
			subtab_four->add_checkbox( _w( "Show impacts" ), _w( "m_impct" ) );
		} tab_third->pushback_subtab( subtab_four );

	} main_window->pushback_tab( tab_third );

	c_tab* tab_fourth = main_window->add_tab( _w( "Gameplay" ) );
	{
		c_subtab* subtab_one = tab_fourth->add_subtab( _w( "Visual" ) );
		{
			subtab_one->add_checkbox( _w( "Thirdperson" ), _w( "m_tp" ) );
			subtab_one->add_keybind( _w( "" ), _w( "k_tp" ), true );
			subtab_one->add_checkbox( _w( "Force dead thirdperson" ), _w( "m_tp_ded" ) );
			subtab_one->add_slider( _w( "Distance" ), _w( "m_tp_dst" ), 30, 180, _w( "%d" ) );
			subtab_one->add_checkbox( _w( "Fov changer" ), _w( "m_fov" ) );
			subtab_one->add_checkbox( _w( "Force fov when scoped" ), _w( "m_fov_sc" ) );
			subtab_one->add_slider( _w( "Fov" ), _w( "m_fov_val" ), 60, 140, _w( "%d" ) );
			subtab_one->add_checkbox( _w( "Viewmodel fov changer" ), _w( "m_vm_fov" ) );
			subtab_one->add_slider( _w( "Viewmodel fov" ), _w( "m_vm_fov_val" ), 60, 140, _w( "%d" ) );
			subtab_one->add_slider( _w( "Transparency in scope" ), _w( "m_tr_sc" ), 0, 100, _w( "%d %%" ) );
			subtab_one->add_checkbox( _w( "Remove scope" ), _w( "m_rm_sc" ) );
			subtab_one->add_checkbox( _w( "Remove smoke" ), _w( "m_rm_smk" ) );
			subtab_one->add_checkbox( _w( "Remove flash" ), _w( "m_rm_flsh" ) );
			subtab_one->add_checkbox( _w( "Force crosshair" ), _w( "m_fr_ch" ) );
			subtab_one->add_checkbox( _w( "Radar" ), _w( "m_rdr" ) );

			subtab_one->add_checkbox( _w( "Auto accept" ), _w( "m_aac" ) );
			subtab_one->add_checkbox( _w( "Reveal ranks" ), _w( "m_rvr" ) );
			subtab_one->add_checkbox( _w( "Hurt log" ), _w( "m_hrt" ) );
			subtab_one->add_checkbox( _w( "Buy log" ), _w( "m_buy" ) );
			subtab_one->add_checkbox( _w( "Clan tag" ), _w( "m_cltg" ) );
			subtab_one->add_checkbox( _w( "Preserve Deathnotices" ), _w( "m_prsrv" ) );
		} tab_fourth->pushback_subtab( subtab_one );
		c_subtab* subtab_two = tab_fourth->add_subtab( _w( "Movement" ) );
		{
			subtab_two->add_checkbox( _w( "Bhop" ), _w( "m_bhp" ) );
			subtab_two->add_checkbox( _w( "Autostrafer" ), _w( "m_strf" ) );
		} tab_fourth->pushback_subtab( subtab_two );
	} main_window->pushback_tab( tab_fourth );

	c_tab* tab_five = main_window->add_tab( _w( "Legit" ) );
	{
	} main_window->pushback_tab( tab_five );

	c_tab* tab_six = main_window->add_tab( _w( "Skins" ) );
	{
		c_subtab* subtab_one = tab_six->add_subtab( _w( "Inventory" ) );
		{
			subtab_one->add_listbox( _w( "Inventory" ), inventorychanger::get().get_list_items(), _w( "inventory_list_idx" ), 400, 110 );
			subtab_one->add_button( _w( "Add" ), inventory_add );
			subtab_one->add_button( _w( "Remove" ), inventory_rm, true );
			subtab_one->add_button( _w( "Save" ), inventory_save );
			subtab_one->add_textbox( _w( "" ), weapon_filter );
			subtab_one->add_textbox( _w( "Filter" ), kit_filter, true );
			subtab_one->add_listbox( _w( "Weapons" ), k_pure_weapon_names, _w( "inventory_definition_index" ), 200, 255, weapon_filter );
			subtab_one->add_listbox( _w( "Paint kit" ), skinchanger::get().k_pure_skin_names, _w( "inventory_paint_kit" ), 200, 255, kit_filter, true );

			subtab_one->add_slider( _w( "Stattrak" ), _w( "inventory_stat_trak" ), -1, 1000, _w( "%d kills" ) );
			subtab_one->add_slider( _w( "Wear" ), _w( "inventory_wear" ), 0.f, 1.f, _w( "%.5f %%" ) );
			subtab_one->add_slider( _w( "Seed" ), _w( "inventory_seed" ), 1, 200, _w( "%d" ) );
		} tab_six->pushback_subtab( subtab_one );
	} main_window->pushback_tab( tab_six );

	this->m_windows.push_back( main_window );

	m_init = true;
}

void c_menu::render()
{
	init();

	if ( !m_opened )
		return;

	update_input();

	inventorychanger::get().update_selected();

	for ( auto window : m_windows )
		window->render();

	draw_cursor();
}

bool c_menu::is_hovering( bounds pbounds )
{
	return is_in_area( g_mouse.pos, pbounds );
}

bool c_menu::is_holding( bounds pbounds ) const
{
	if ( !g_mouse.is_holding )
		return false;

	return is_in_area( g_mouse.hold_pos, pbounds );
}

bool c_menu::mouse_is_holding()
{
	return g_mouse.is_holding;
}

void c_menu::reset_mouse_state()
{
	g_mouse.is_pressed = false;
	g_mouse.is_holding = false;
	g_mouse.needs_reset = true;
}

bool c_menu::key_updated( int key )
{
	return ( m_keystate[ key ] && !m_oldstate[ key ] );
}

bool c_menu::mouse_is_pressed()
{
	return g_mouse.is_pressed;
}

bool c_menu::is_pressed( bounds pbounds )
{
	if ( !g_mouse.is_pressed )
		return false;

	return is_in_area( g_mouse.press_pos, pbounds );
}

POINT c_menu::get_mouse_pos()
{
	return g_mouse.pos;
}

POINT c_menu::get_hold_pos()
{
	return g_mouse.hold_pos;
}

POINT c_menu::get_press_pos()
{
	return g_mouse.press_pos;
}

//if this is ghetto - change plz
bool c_menu::is_in_area( POINT p, bounds pbounds )
{
	if ( p.x >= pbounds.x && p.x <= ( pbounds.x + pbounds.w ) )
		if ( p.y > pbounds.y && p.y < ( pbounds.y + pbounds.h ) )
			return true;

	return false;
}

bool& c_menu::get_opened()
{
	return m_opened;
}

void c_menu::toggle()
{
	static bool prev_state;
	const bool state = GetAsyncKeyState( VK_INSERT );
	if ( prev_state != state )
	{
		if ( state )
			m_opened = !m_opened;

		_( cl_mouseenable, "cl_mouseenable " );
		auto msg = cl_mouseenable + std::to_string( !m_opened );
		g_pEngine->ClientCmd_Unrestricted( msg.c_str() );

		prev_state = state;
	}
}

void c_menu::release_images()
{
	{
		for ( auto window : m_windows )
		{
			SAFE_RELEASE( window->img );
			SAFE_RELEASE( window->sprite );
			SAFE_RELEASE( window->img_big );
			SAFE_RELEASE( window->sprite_big );
		}
	}
}

void c_menu::draw_cursor()
{
	/*recreation of csgo cursor (size-wise)*/
	D3DCOLOR main_color = D3DCOLOR_RGBA( 20, 179, 178, 255 );
	D3DCOLOR back_color = D3DCOLOR_RGBA( 189, 201, 221, 255 );

	POINT p1; p1.x = g_mouse.pos.x; p1.y = g_mouse.pos.y;
	POINT p2; p2.x = g_mouse.pos.x + 24; p2.y = g_mouse.pos.y + 12;
	POINT p3; p3.x = g_mouse.pos.x + 12; p3.y = g_mouse.pos.y + 24;
	render::get().filled_triangle( p1, p2, p3, main_color );

	POINT p4; p4.x = g_mouse.pos.x + 3; p4.y = g_mouse.pos.y + 3;
	POINT p5; p5.x = g_mouse.pos.x + 23; p5.y = g_mouse.pos.y + 13;
	POINT p6; p6.x = g_mouse.pos.x + 13; p6.y = g_mouse.pos.y + 23;
	render::get().filled_triangle( p4, p5, p6, back_color );
}

void c_menu::update_input()
{
	std::copy( m_keystate, m_keystate + 255, m_oldstate );

	// simple keyboard iteration
	for ( auto n = 0; n < 255; ++n )
	{
		// update current state
		m_keystate[ n ] = GetAsyncKeyState( n );
	}

	POINT p;
	if ( !GetCursorPos( &p ) )
		return;

	ScreenToClient( init::window, &p );

	if ( GetAsyncKeyState( VK_LBUTTON ) && g_mouse.needs_reset )
		return;
	else if ( g_mouse.needs_reset )
		g_mouse.needs_reset = false;

	g_mouse.pos = p;

	if ( GetAsyncKeyState( VK_LBUTTON ) && !g_mouse.is_pressed && !g_mouse.is_holding )
	{
		g_mouse.is_pressed = true;
		g_mouse.press_pos = p;
	}
	else if ( GetAsyncKeyState( VK_LBUTTON ) && !g_mouse.is_holding )
	{
		g_mouse.is_pressed = false;
		g_mouse.is_holding = true;
		g_mouse.hold_pos = p;
	}
	else if ( !GetAsyncKeyState( VK_LBUTTON ) )
	{
		g_mouse.is_pressed = false;
		g_mouse.is_holding = false;
	}
	else if ( g_mouse.is_holding )
	{
		g_mouse.hold_pos = p;
	}
}

uintptr_t c_menu::modulate_alpha( uintptr_t orig, int alpha )
{
	int a = ( orig >> 24 ) & 0xff;
	int r = ( orig >> 16 ) & 0xff;
	int g = ( orig >> 8 ) & 0xff;
	int b = orig & 0xff;

	int newAlpha = ceil( alpha * a );

	UINT newColor = r << 16;
	newColor += g << 8;
	newColor += b;
	newColor += ( newAlpha << 24 );


	return ( uintptr_t )newColor;
}