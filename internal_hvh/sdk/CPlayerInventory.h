#pragma once

class CSharedObjectTypeCache
{
public:
	void AddObject( void* obj )
	{
		typedef void( __thiscall* tOriginal )( void*, void* );
		util::get_vfunc<tOriginal>( this, 1 )( this, obj );
	}

	void RemoveObject( void* obj )
	{
		typedef void( __thiscall* tOriginal )( void*, void* );
		util::get_vfunc<tOriginal>( this, 3 )( this, obj );
	}

	std::vector<CEconItem*> GetEconItems()
	{
		std::vector<CEconItem*> ret;

		auto size = *reinterpret_cast< size_t* >( this + 0x18 );

		auto data = *reinterpret_cast< uintptr_t** >( this + 0x4 );

		for ( size_t i = 0; i < size; i++ )
			ret.push_back( reinterpret_cast< CEconItem* >( data[ i ] ) );

		return ret;
	}
};


class CPlayerInventory
{
public:
	void RemoveItem( uint64_t ID );
	void RemoveItem( CEconItem* item );
	void ClearInventory();
	CSharedObjectTypeCache* GetBaseTypeCache();
	uint32_t GetSteamID();
	CUtlVector< C_EconItemView* >* GetInventoryItems();
	C_EconItemView* GetInventoryItemByItemID( int64_t id );
	bool AddEconItem( CEconItem* item, int a3, int a4, char a5 );
	C_EconItemView* GetItemInLoadout( int team, int slot );
};


class CSInventoryManager
{
public:
	C_EconItemView * FindOrCreateReferenceEconItem( int64_t ID );
	char FilloutItemFromEconItem(C_EconItemView* view, CEconItem* item );
	bool EquipItemInLoadout( int team, int slot, uint64_t item_id );
};

extern CSInventoryManager* g_pCSInventoryManager;