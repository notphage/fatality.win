#include "../include_cheat.h"

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

template <typename Key, typename Value>
struct Node_t
{
	int previous_id;		//0x0000
	int next_id;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	Key key;				//0x0010
	Value value;			//0x0014
};

template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory;		//0x0000
	int allocation_count;			//0x0004
	int grow_size;					//0x0008
	int start_element;				//0x000C
	int next_available;				//0x0010
	int _unknown;					//0x0014
	int last_element;				//0x0018
}; //Size=0x001C

   // could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t
{
	char* buffer;	//0x0000
	int capacity;	//0x0004
	int grow_size;	//0x0008
	int length;		//0x000C
}; //Size=0x0010

struct CPaintKit
{
	int id;						//0x0000

	String_t name;				//0x0004
	String_t description;		//0x0014
	String_t item_name;			//0x0024
	String_t material_name;		//0x0034
	String_t image_inventory;	//0x0044

	char pad_0x0054[ 0x8C ];		//0x0054
}; //Size=0x00E0


bool is_knife( const int idx )
{
	return ( idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY
			 || idx == WEAPON_KNIFE_FALCHION || idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_M9_BAYONET || idx == WEAPON_KNIFE_GUT
			 || idx == WEAPON_KNIFE_PUSH || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_TACTICAL );
}

void clearRefCountedVector( CUtlVector<IRefCounted*>& vec )
{
	for ( auto& elem : vec )
	{
		if ( elem )
		{
			elem->unreference();
			elem = nullptr;
		}
	}
	vec.RemoveAll();
}

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_pClient->GetAllClasses();

	_( CEconWearable, "CEconWearable" );

	while ( strcmp( clazz->m_pNetworkName, CEconWearable ) )
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

void skinchanger::spawn_glove()
{
	if ( !g_pLocalPlayer || !g_pEngine->IsInGame() )
		return;

	const auto wearables = g_pLocalPlayer->get_wearables();

	auto glove = reinterpret_cast< C_BaseAttributableItem* >( g_pEntitiyList->GetClientEntityFromHandle( wearables[ 0 ] ) );
	if ( !glove )
	{
		const auto our_glove = reinterpret_cast< C_BaseAttributableItem* >( g_pEntitiyList->GetClientEntityFromHandle( glove_handle ) );
		if ( our_glove )
		{
			wearables[ 0 ] = glove_handle;
			glove = our_glove;
		}
	}

	if ( !g_pLocalPlayer->get_alive() )
	{
		if ( glove )
		{
			glove->SetDestroyedOnRecreateEntities();
			glove->Release();
		}

		return;
	}

	if ( !glove )
	{
		static auto create_wearable_fn = GetWearableCreateFn();

		const auto entry = g_pEntitiyList->GetHighestEntityIndex() + 1;
		const auto serial = rand() % 0x1000;

		create_wearable_fn( entry, serial );
		glove = reinterpret_cast< C_BaseAttributableItem* >( g_pEntitiyList->GetClientEntity( entry ) );

		static const Vector new_pos = { 10000.f, 10000.f, 10000.f };
		glove->set_abs_origin( new_pos );

		wearables[ 0 ] = entry | serial << 16;
		glove_handle = wearables[ 0 ];
	}

	apply_glove();
}

void skinchanger::apply_glove() const
{
	auto glove = static_cast< C_BaseCombatWeapon* >( g_pEntitiyList->GetClientEntityFromHandle( glove_handle ) );
	if ( !glove )
		return;

	auto econ_data = inventorychanger::get().get_inventory()->GetItemInLoadout( g_pLocalPlayer->get_team(), 41 );
	if ( !econ_data )
		return;

	auto soc_data = econ_data->GetSOCData();
	if ( !soc_data )
		return;

	const auto id = *soc_data->GetItemID();
	static const uint64_t mask = std::numeric_limits<uint32_t>::max();
	const auto low_id = mask & id;
	const auto high_id = id >> 32;
	glove->get_item_id_low() = low_id;
	glove->get_item_id_high() = high_id;
	glove->get_account_id() = g_pLocalPlayer->get_player_info().xuidlow;

	auto &items = inventorychanger::get().get_items();
	inventory_item_t* inv_item = nullptr;
	for ( auto &item : items )
		if ( item.item && *item.item->GetItemID() == *soc_data->GetItemID() )
		{
			inv_item = &item;
			break;
		}

	if ( !inv_item )
		return;

	if ( k_weapon_info.count( inv_item->definition_index ) && glove->get_item_definiton_index() != inv_item->definition_index )
	{
		glove->get_item_definiton_index() = inv_item->definition_index;
		_u( model_name, k_weapon_info.at( inv_item->definition_index ).model );
		glove->set_model_idx( g_pModelInfo->GetModelIndex( model_name ) );
		glove->PreDataUpdate( 0 );
	}
}

void skinchanger::run( const ClientFrameStage_t stage )
{
	if ( stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		return;

	spawn_glove();

	const auto local_player = get_entity( g_pEngine->GetLocalPlayer() );
	if ( !local_player || !local_player->get_alive() || !g_pEngine->IsInGame() )
		return;

	const auto weapons = local_player->get_weapons();
	for ( size_t i = 0; weapons[ i ] != INVALID_EHANDLE_INDEX; i++ )
	{
		auto weapon = static_cast< C_BaseCombatWeapon* >( g_pEntitiyList->GetClientEntityFromHandle( weapons[ i ] ) );
		if ( !weapon || weapon->get_owner_ent() == -1 )
			continue;

		const auto info = local_player->get_player_info();

		if ( info.xuidlow != weapon->get_xuid_low() )
			continue;

		if ( info.xuidhigh != weapon->get_xuid_high() )
			continue;

		auto econ_item = weapon->get_econ_item_view();
		if ( !econ_item )
			continue;

		auto weapon_def = econ_item->get_static_data();
		if ( !weapon_def )
			continue;

		auto econ_data = inventorychanger::get().get_inventory()->GetItemInLoadout( local_player->get_team(), weapon_def->GetEquippedPosition() );
		if ( !econ_data )
			continue;

		auto soc_data = econ_data->GetSOCData();
		if ( !soc_data )
			continue;

		const auto id = *soc_data->GetItemID();
		static const uint64_t mask = std::numeric_limits<uint32_t>::max();
		const auto low_id = mask & id;
		const auto high_id = id >> 32;
		weapon->get_item_id_low() = low_id;
		weapon->get_item_id_high() = high_id;
		weapon->get_account_id() = local_player->get_player_info().xuidlow;

		auto &items = inventorychanger::get().get_items();
		inventory_item_t* inv_item = nullptr;
		for ( auto &item : items )
			if ( item.item && *item.item->GetItemID() == *soc_data->GetItemID() )
			{
				inv_item = &item;
				break;
			}

		if ( !inv_item )
			continue;

		//if ( inv_item->fullupdate )
		//{
		//	auto &view = weapon->m_AttributeManager.m_Item;

		//	weapon->m_bCustomMaterialInitialized = false;
		//	weapon->m_CustomMaterials.RemoveAll();
		//	view.m_CustomMaterials.RemoveAll();
		//	clearRefCountedVector( view.m_VisualsDataProcessors ); // prevent memory leak

		//	weapon->PostDataUpdate( 0 );
		//	weapon->OnDataChanged( 0 );

		//	inv_item->fullupdate = false;
		//}

		const auto world_model_handle = weapon->get_world_model_handle();
		if ( world_model_handle == INVALID_EHANDLE_INDEX )
			continue;

		auto world_model_weapon = reinterpret_cast< C_BaseWeaponWorldModel* >( g_pEntitiyList->GetClientEntityFromHandle( world_model_handle ) );
		if ( world_model_weapon )
			world_model_weapon->get_model_idx() = weapon->get_model_idx() + 1;

		if ( k_weapon_info.count( inv_item->definition_index ) && weapon->get_item_definiton_index() != inv_item->definition_index )
		{
			weapon->get_item_definiton_index() = inv_item->definition_index;
			_u( model_name, k_weapon_info.at( inv_item->definition_index ).model );
			weapon->set_model_idx( g_pModelInfo->GetModelIndex( model_name ) );
			weapon->PreDataUpdate( 0 );
		}
	}

	const auto view_model_handle = local_player->get_view_model();
	if ( view_model_handle == INVALID_EHANDLE_INDEX )
		return;

	auto view_model = static_cast< C_BaseViewModel* >( g_pEntitiyList->GetClientEntityFromHandle( view_model_handle ) );
	if ( !view_model )
		return;

	const auto view_model_weapon_handle = view_model->get_weapon();
	if ( view_model_weapon_handle == INVALID_EHANDLE_INDEX )
		return;

	auto view_model_weapon = static_cast< C_BaseCombatWeapon* >( g_pEntitiyList->GetClientEntityFromHandle( view_model_weapon_handle ) );
	if ( !view_model_weapon )
		return;

	if ( k_weapon_info.count( view_model_weapon->get_item_definiton_index() ) )
	{
		auto& override_model = k_weapon_info.at( view_model_weapon->get_item_definiton_index() ).model;
		_u( model_name, override_model );
		view_model->get_model_idx() = g_pModelInfo->GetModelIndex( model_name );
	}
}

void skinchanger::parse_kits()
{
	static auto sig_address = sig( "client.dll", "E8 ?? ?? ?? ?? FF 76 0C 8D 48 04 E8" );
	const auto item_system_offset = *reinterpret_cast< int32_t* >( sig_address + 1 );
	const auto item_system_fn = reinterpret_cast< CCStrike15ItemSystem* ( *)( ) >( sig_address + 5 + item_system_offset );
	auto item_schema = reinterpret_cast< CCStrike15ItemSchema* >( uintptr_t( item_system_fn() ) + sizeof( void* ) );


	const auto get_paint_kit_definition_offset = *reinterpret_cast< int32_t* >( sig_address + 11 + 1 );
	auto get_paint_kit_definition_fn = reinterpret_cast< CPaintKit* ( __thiscall * )( CCStrike15ItemSchema*, int ) >( sig_address + 11 + 5 + get_paint_kit_definition_offset );
	const auto start_element_offset = *reinterpret_cast< intptr_t* >( uintptr_t( get_paint_kit_definition_fn ) + 8 + 2 );

	const auto head_offset = start_element_offset - 12;
	const auto map_head = reinterpret_cast< Head_t<int, CPaintKit*>* >( uintptr_t( item_schema ) + head_offset );

	for ( auto i = 0; i <= map_head->last_element; ++i )
	{
		const auto paint_kit = map_head->memory[ i ].value;

		if ( paint_kit->id == 9001 )
			continue;

		const auto wide_name = std::wstring( g_pLocalize->Find( paint_kit->item_name.buffer + 1 ) );
		const auto name = std::string( wide_name.begin(), wide_name.end() );

		/*if ( paint_kit->id < 10000 )*/
		k_skins.push_back( { paint_kit->id, _w2( name ) } );
	}

	std::sort( k_skins.begin(), k_skins.end() );
	for ( auto &skin : k_skins )
	{
		k_pure_skin_names.push_back( skin.name );
		k_skin_name_to_kit.push_back( skin.id );
	}

	for ( auto& item : k_weapon_names )
	{
		k_pure_weapon_names.emplace_back( item.name );
		k_idx_to_defidex.push_back( item.definition_index );
	}

	erase_end;
}
