#pragma once

#include "tsl/robin_map.h"

class value
{
public:
	template <typename T = float>
	T get() { return static_cast< T >( m_value ); }

	template <typename T = float>
	void set( T in ) { m_value = static_cast< double >( in ); }

private:
	double m_value = 0.0;
};

namespace vars
{
	inline struct
	{
		value enabled;
		value autofire;
		value baim_on_key;
		value backtrack;
		value fakelag_compensation;
		value ping_spike;
		value ping_spike_amt;
		value ping_spike_base;
		value ping_spike_base_amt;
		value resolver;
		value resolve_team;
		value knife_bot;
		value zeus_bot;
		value override;
		value hsonly;
		value zeus_hc;
	} aim;


	struct wconfig_types_t
	{
		value hitchance;
		value mindmg;
		value scale;
		value scale_head;
		value adaptive_scale;
		value autostop;
		value autostop_on_key;
		value autostop_lethal;
		value baim_mode;
		value autorevolver;
		value scout_jumpshot;
		value autoscope;
		struct
		{
			value air;
			value resolver_unsure;
			value lethal;
		}baim;
	};

	inline struct
	{
		wconfig_types_t autosniper, scout, awp, heavy_pistol, pistol, other;
	} config;

	struct aa_types_t
	{
		value enabled;
		value spin;
		value spin_speed;
		value spin_range;
		value jitter;
		value jitter_range;
		value jitter_speed;
		value jitter_random;
		value lby;
		value lby_static;
		value lby_autodirection;
		value at_target;
		value edge;
		value freestand;
		value add;
		value choke;
		value lby_delta_first;
		value lby_delta;
		value lby_random;
		value lby_random_add;
		value lby_random_range;
		value lby_when_hittable;
		value lby_delta_when_hittable;
	};

	inline struct
	{
		value enabled;
		value fakelag_when_disabled;
		value fakewalk;
		value prevent_onshot;
		value on_peek;
		value fs_dormant_timeout;
		value manual;
		value indicator;
		value indicator_col;
		value choke_reload;
		value choke_weapon_switch;
		value choke_shot;
		value freestanding_fps;

		aa_types_t regular, moving, air;
	} aa;

	inline struct
	{
		value autostop;
		value baim;
		value fakewalk;
		value ping_spike;
		value thirdperson;
		value override;
		value aa_back;
		value aa_left;
		value aa_right;
	} key;

	inline struct
	{
		value bhop;
		value fov_enabled;
		value fov_scope;
		value fov;
		value vm_fov_enabled;
		value vm_fov;
		value hitmarker;
		value hitsound_select;
		value radar;
		value remove_flash;
		value remove_scope;
		value remove_smoke;
		value autostrafe;
		value thirdperson;
		value thirdperson_dead;
		value thirdperson_dist;
		value transparency_in_scope;
		value force_crosshair;
		value autoaccept;
		value reveal_ranks;
		value hurtlog;
		value buylog;
		value clantag;
		value preservedn;
		value impacts;
	} misc;

	inline struct
	{
		struct
		{
			value ammo;
			value ammo_color;
			value armor;
			value armor_color;
			value ping;
			value ping_color;
			value health;
			value name;
			value name_color;
			value weapon;
			value weapon_color;
			value lby_timer;
			value scoped;
			value bomb;
			value box;
			value box_color;

			struct
			{
				value weapons;
				value grenades;
				value c4;
				value color;
			} world;
		} esp;

		struct
		{
			value enabled;
			struct
			{
				value enabled;
				value color;
			} enemy;

			struct
			{
				value enabled;
				value color;
			} team;

			struct
			{
				value enabled;
				value color;
			} local;

		} beams;
		struct
		{
			struct
			{
				value enabled;
				value type;
				value color;
				value hidden_color;
				value color_shine;
			} enemy;

			struct
			{
				value enabled;
				value type;
				value color;
				value hidden_color;
				value color_shine;
			} team;

			struct
			{
				value enabled;
				value type;
				value color;
				value color_shine;
			} local;

			struct
			{
				value enabled;
				value color;
			} history;

			value enabled;
			value extended;
			value hidden;
		} chams;

		struct
		{
			value enabled;
			struct
			{
				value enabled;
				value color;

			} enemy;

			struct
			{
				value enabled;
				value color;
			} team;

			struct
			{
				value enabled;
				value color;
			} local;

			struct
			{
				value weapons;
				value grenades;
				value c4;
				value color;
			} world;
		} glow;

		value dark;
		value prop_transparency;
		value spread_circle;
		value spread_circle_color1;
		value spread_circle_color2;
		value spread_circle_type;
		value out_of_fov;
		value out_of_fov_col;
		value out_of_fov_dst;
	} visuals;

	inline struct
	{
		value inventory_list_idx;
		value slot;
	} menu;

	inline struct
	{
		value definition_index;
		value paint_kit;
		value wear;
		value seed;
		value stat_trak;
	} inventory;
}

extern tsl::robin_map< std::string, value* > g_cfg;

struct antiaimconfig_t
{
	antiaimconfig_t( const int current )
	{
		index = current;

		vars::aa_types_t* config = nullptr;

		switch ( current )
		{
			case 0:
			{
				config = &vars::aa.regular;
				break;
			}
			case 1:
			{
				config = &vars::aa.moving;
				break;
			}
			case 2:
			{
				config = &vars::aa.air;
				break;
			}
		}

		enabled = &config->enabled;
		spin = &config->spin;
		spin_speed = &config->spin_speed;
		spin_range = &config->spin_range;
		jitter = &config->jitter;
		jitter_range = &config->jitter_range;
		jitter_speed = &config->jitter_speed;
		jitter_random = &config->jitter_random;
		at_target = &config->at_target;
		lby = &config->lby;
		lby_static = &config->lby_static;
		lby_autodirection = &config->lby_autodirection;
		edge = &config->edge;
		freestand = &config->freestand;
		add = &config->add;
		choke = &config->choke;
		lby_delta_first = &config->lby_delta_first;
		lby_delta = &config->lby_delta;
		lby_random = &config->lby_random;
		lby_random_add = &config->lby_random_add;
		lby_random_range = &config->lby_random_range;
	}

	value* enabled;
	value* spin;
	value* spin_speed;
	value* spin_range;
	value* jitter;
	value* jitter_range;
	value* jitter_speed;
	value* jitter_random;
	value* at_target;
	value* lby;
	value* lby_static;
	value* lby_autodirection;
	value* edge;
	value* freestand;
	value* add;
	value* choke;
	value* lby_delta_first;
	value* lby_delta;
	value* lby_random;
	value* lby_random_add;
	value* lby_random_range;
	int index;
};

struct weaponconfig_t
{
	weaponconfig_t( const int current )
	{
		index = current;

		vars::wconfig_types_t* config = nullptr;

		switch ( current )
		{
			case 0:
			{
				config = &vars::config.autosniper;
				break;
			}
			case 1:
			{
				config = &vars::config.scout;;
				break;
			}
			case 2:
			{
				config = &vars::config.awp;
				break;
			}
			case 3:
			{
				config = &vars::config.heavy_pistol;
				break;
			}
			case 4:
			{
				config = &vars::config.pistol;
				break;
			}
			case 5:
			{
				config = &vars::config.other;
				break;
			}
		}
		hitchance = &config->hitchance;
		mindmg = &config->mindmg;
		scale = &config->scale;
		scale_head = &config->scale_head;
		adaptive_scale = &config->adaptive_scale;
		autostop = &config->autostop;
		autostop_key = &config->autostop_on_key;
		autostop_lethal = &config->autostop_lethal;
		baim_mode = &config->baim_mode;
		scout_jumpshot = &config->scout_jumpshot;
		autorevolver = &config->autorevolver;
		autoscope = &config->autoscope;
		baim.lethal = &config->baim.lethal;
		baim.air = &config->baim.air;
		baim.resolver_unsure = &config->baim.resolver_unsure;
	}
	value* hitchance;
	value* mindmg;
	value* scale;
	value* scale_head;
	value* adaptive_scale;
	value* autostop;
	value* autostop_key;
	value* autostop_lethal;
	value* baim_mode;
	value* scout_jumpshot;
	value* autorevolver;
	value* autoscope;
	struct
	{
		value* lethal;
		value* air;
		value* resolver_unsure;
	}baim;
	int index;
};

class config : public singleton<config>
{
public:
	config();
	static void set( const std::string& item );
	static void assign( const std::string& item );
	static void write( const std::string& item );
	bool load( int slot ) const;
	bool save( int slot ) const;
	std::array<weaponconfig_t, 6> weapon_config =
	{
		weaponconfig_t( 0 ),
		weaponconfig_t( 1 ),
		weaponconfig_t( 2 ),
		weaponconfig_t( 3 ),
		weaponconfig_t( 4 ),
		weaponconfig_t( 5 ),
	};

	std::array<antiaimconfig_t, 3> antiaim_config =
	{
		antiaimconfig_t( 0 ),
		antiaimconfig_t( 1 ),
		antiaimconfig_t( 2 ),
	};
private:
	std::string	config_name;
};
