#include "../include_cheat.h"
#include <menu/config/json.h>
#include <fstream>

uint64_t inventorychanger::get_highest_id() const
{
	uint64_t max = 1;
	for ( auto i : inventory->GetBaseTypeCache()->GetEconItems() )
	{
		if ( *i->GetItemID() > max )
			max = *i->GetItemID();
	}

	return max;
}

bool is_knife2( const int idx )
{
	return ( idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY
			 || idx == WEAPON_KNIFE_FALCHION || idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_M9_BAYONET || idx == WEAPON_KNIFE_GUT
			 || idx == WEAPON_KNIFE_PUSH || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_TACTICAL );
}

void inventorychanger::update()
{
	if ( !inventory )
		return;

	if ( items.empty() )
		return;

	auto &item = items[ vars::menu.inventory_list_idx.get<int>() ];

	item.update = true;
	item.definition_index = k_idx_to_defidex[ vars::inventory.definition_index.get<int>() ];
	item.paint_kit = skinchanger::get().k_skin_name_to_kit[ vars::inventory.paint_kit.get<int>() ];
	item.stat_trak = vars::inventory.stat_trak.get<int>();
	item.seed = vars::inventory.seed.get<int>();
	item.wear = vars::inventory.wear.get<float>();
	item.fullupdate = true;

	update_items( true );
}

void inventorychanger::update_selected()
{
	if ( !inventory )
		return;

	if ( items.empty() )
		return;

	static auto oldidx = -1;

	if ( vars::menu.inventory_list_idx.get<int>() == oldidx )
		return;

	oldidx = vars::menu.inventory_list_idx.get<int>();

	auto &item = items[ vars::menu.inventory_list_idx.get<int>() ];

	item.update = true;

	auto wpn_idx = 0;
	auto kit_idx = 0;
	for ( auto& curr : k_weapon_names )
	{
		if ( curr.definition_index == item.definition_index )
			break;

		wpn_idx++;
	}

	for ( auto& curr : skinchanger::get().k_skins )
	{
		if ( curr.id == item.paint_kit )
			break;

		kit_idx++;
	}

	vars::inventory.definition_index.set( wpn_idx );
	vars::inventory.paint_kit.set( kit_idx );
	vars::inventory.stat_trak.set( item.stat_trak );
	vars::inventory.seed.set( item.seed );
	vars::inventory.wear.set( item.wear );
}

void inventorychanger::add()
{
	if ( !inventory )
		return;

	inventory_item_t item;
	item.item = nullptr;
	item.id = get_highest_id() + 1;
	item.update = true;
	item.definition_index = k_idx_to_defidex[ vars::inventory.definition_index.get<int>() ];
	item.paint_kit = skinchanger::get().k_skin_name_to_kit[ vars::inventory.paint_kit.get<int>() ];
	item.stat_trak = vars::inventory.stat_trak.get<int>();
	item.seed = vars::inventory.seed.get<int>();
	item.wear = vars::inventory.wear.get<float>();
	item.fullupdate = true;

	items.push_back( item );

	std::string wpn_name;
	std::string kit_name;
	for ( auto& curr : k_weapon_names )
		if ( curr.definition_index == item.definition_index )
		{
			wpn_name = curr.name;
			break;
		}

	for ( auto& curr : skinchanger::get().k_skins )
		if ( curr.id == item.paint_kit )
		{
			kit_name = curr.name;
			break;
		}

	_u( name, wpn_name );
	_u( kit, kit_name );
	auto str = std::string( name ).append( " ( " ).append( kit ).append( " )" );

	list_items.emplace_back( _w2( str ) );

	update_items();
}

void inventorychanger::set( const int index )
{
	if ( !inventory )
		return;

	if ( index >= items.size() )
		return;

	auto item = &items[ index ];

	item->definition_index = k_idx_to_defidex[ vars::inventory.definition_index.get<int>() ];
	item->paint_kit = skinchanger::get().k_skin_name_to_kit[ vars::inventory.paint_kit.get<int>() ];
	item->seed = vars::inventory.seed.get<int>();
	item->wear = vars::inventory.wear.get<int>();
	item->stat_trak = vars::inventory.stat_trak.get<int>();
	item->equipped_state = item->item->GetEquippedState();
	item->update = true;
	item->fullupdate = true;

	update_items();
}

void inventorychanger::update_items( bool recheck )
{
	if ( !inventory )
		return;

	auto i = -1;
	for ( auto& cfg_item : items )
	{
		i++;
		if ( cfg_item.item )
		{
			if ( !cfg_item.update )
				continue;

			inventory->RemoveItem( cfg_item.item );
			cfg_item.item = nullptr;
			cfg_item.update = false;
		}

		auto item = util::CreateEconItem();
		cfg_item.item = item;

		if ( cfg_item.id == 0 )
			cfg_item.id = get_highest_id() + 1;

		std::string wpn_name;
		std::string kit_name;
		for ( auto& curr : k_weapon_names )
			if ( curr.definition_index == cfg_item.definition_index )
			{
				wpn_name = curr.name;
				break;
			}

		for ( auto& curr : skinchanger::get().k_skins )
			if ( curr.id == cfg_item.paint_kit )
			{
				kit_name = curr.name;
				break;
			}

		_( fatality, "fatality.win" );

		_u( name, wpn_name );
		_u( kit, kit_name );
		auto str = std::string( name ).append( " ( " ).append( kit ).append( " )" );

		list_items[ i ] = _w2( str );

		*item->GetAccountID() = inventory->GetSteamID();
		*item->GetDefIndex() = cfg_item.definition_index;
		*item->GetItemID() = cfg_item.id;
		*item->GetInventory() = 1;
		*item->GetFlags() = 0;
		*item->GetOriginalID() = 0;
		item->SetCustomDesc( fatality );
		if ( cfg_item.stat_trak >= 0 )
			item->SetStatTrak( cfg_item.stat_trak );
		item->SetPaintKit( cfg_item.paint_kit );
		item->SetPaintSeed( cfg_item.seed );
		item->SetPaintWear( cfg_item.wear );
		item->SetOrigin( 8 );
		if ( !is_knife2( cfg_item.definition_index ) )
			item->SetRarity( ITEM_RARITY_ANCIENT );
		if ( is_knife2( cfg_item.definition_index ) )
			item->SetQuality( ITEM_QUALITY_UNUSUAL );


		item->SetLevel( 1 );
		for ( auto& state : cfg_item.equipped_state )
			if ( state != 0 )
				item->UpdateEquippedState( state );

		inventory->AddEconItem( item, 1, 0, 1 );
	}
}

using json = nlohmann::json;

void to_json( json& j, const inventory_item_t& item )
{
	_( id, "id" );
	_( definition_index, "definition_index" );
	_( paint_kit, "paint_kit" );
	_( seed, "seed" );
	_( stat_trak, "stat_trak" );
	_( wear, "wear" );
	_( state0, "state0" );
	_( state1, "state1" );

	j = json
	{
	{ id, item.id },
	{ definition_index, item.definition_index },
	{ paint_kit, item.paint_kit },
	{ seed,item.seed },
	{ stat_trak, item.stat_trak },
	{ wear, item.wear },
	{ state0,item.equipped_state[ 0 ] },
	{ state1, item.equipped_state[ 1 ] }
	};
}

void from_json( const json& j, inventory_item_t& item )
{
	_( id, "id" );
	_( definition_index, "definition_index" );
	_( paint_kit, "paint_kit" );
	_( seed, "seed" );
	_( stat_trak, "stat_trak" );
	_( wear, "wear" );
	_( state0, "state0" );
	_( state1, "state1" );

	item.id = j.at( id ).get<uint64_t>();
	item.definition_index = j.at( definition_index ).get<int>();
	item.paint_kit = j.at( paint_kit ).get<int>();
	item.seed = j.at( seed ).get<int>();
	item.stat_trak = j.at( stat_trak ).get<int>();
	item.wear = j.at( wear ).get<float>();
	item.equipped_state[ 0 ] = j.at( state0 ).get<uint32_t>();
	item.equipped_state[ 1 ] = j.at( state1 ).get<uint32_t>();
}

bool is_file_exists( const char *fileName )
{
	std::ifstream infile( fileName );
	return infile.good();
}

void inventorychanger::load()
{
	if ( !inventory )
		inventory = **reinterpret_cast< CPlayerInventory*** >( sig( "client.dll", "8B 3D ? ? ? ? 85 FF 74 1A" ) + 0x2 );

	_( inventory, "inventory" );

	if ( !is_file_exists( inventory ) )
		return;

	try
	{
		items = json::parse( std::ifstream( inventory ) ).get<std::vector<inventory_item_t>>();
	}
	catch ( ... )
	{
		return;
	}

	for ( auto& item : items )
	{
		std::string wpn_name;
		std::string kit_name;
		for ( auto& curr : k_weapon_names )
			if ( curr.definition_index == item.definition_index )
			{
				wpn_name = curr.name;
				break;
			}

		for ( auto& curr : skinchanger::get().k_skins )
			if ( curr.id == item.paint_kit )
			{
				kit_name = curr.name;
				break;
			}


		_u( name, wpn_name );
		_u( kit, kit_name );
		auto str = std::string( name ).append( " ( " ).append( kit ).append( " )" );
		list_items.emplace_back( _w2( str ) );
	}


	update_items();
}

void inventorychanger::save( const bool autosave )
{
	if ( !autosave )
		set( vars::menu.inventory_list_idx.get<int>() );

	_( inventory, "inventory" );

	std::ofstream( inventory ) << nlohmann::json( items );
}

void inventorychanger::remove()
{
	if ( !inventory )
		return;

	if ( items.empty() )
		return;

	const auto cfg_item = items[ vars::menu.inventory_list_idx.get<int>() ];

	if ( cfg_item.item )
		inventory->RemoveItem( cfg_item.item );

	items.erase( items.begin() + vars::menu.inventory_list_idx.get<int>() );
	list_items.erase( list_items.begin() + vars::menu.inventory_list_idx.get<int>() );
}

void inventorychanger::update_equipped( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_START )
		return;

	if ( !inventory || g_pGlobals->framecount % 10000 )
		return;

	for ( auto &item : items )
	{
		if ( !item.item )
			continue;

		item.equipped_state = item.item->GetEquippedState();
		save( true );
	}
}

