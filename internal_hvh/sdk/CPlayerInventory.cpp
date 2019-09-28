#include "../include_cheat.h"

void CPlayerInventory::RemoveItem( uint64_t ID )
{
	static auto fnRemoveItem = reinterpret_cast< int( __thiscall* )( void*, int64_t ) >( sig( "client.dll", "55 8B EC 83 E4 F8 56 57 FF 75 0C 8B F1" ) );

	fnRemoveItem( this, ID );
}

void CPlayerInventory::RemoveItem( CEconItem* item )
{
	RemoveItem( *item->GetItemID() );
	GetBaseTypeCache()->RemoveObject( item );
	util::DestroyEconItem( item );
}

void CPlayerInventory::ClearInventory()
{
	auto BaseTypeCache = this->GetBaseTypeCache();
	auto items = BaseTypeCache->GetEconItems();
	for ( auto item : items )
	{
		RemoveItem( *item->GetItemID() );
		BaseTypeCache->RemoveObject( item );
	}
}

CSharedObjectTypeCache* CPlayerInventory::GetBaseTypeCache()
{
	static auto fnGCSDK_CGCClient_FindSOCache = reinterpret_cast< uintptr_t( __thiscall* )( uintptr_t, uint64_t, uint64_t, bool ) >( sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 1C 0F 10 45 08" ) );
	static auto fnGCSDK_CSharedObjectCache_CreateBaseTypeCache = reinterpret_cast< CSharedObjectTypeCache*( __thiscall* )( uintptr_t, int ) >( sig( "client.dll", "55 8B EC 51 53 56 8B D9 8D 45 08" ) );

	auto SOCahce = fnGCSDK_CGCClient_FindSOCache( g_GCClientSystem + 0x60, *reinterpret_cast< uint64_t* >( this + 0x8 ), *reinterpret_cast< uint64_t* >( this + 0x10 ), 0 );

	return fnGCSDK_CSharedObjectCache_CreateBaseTypeCache( SOCahce, 1 );
}

uint32_t CPlayerInventory::GetSteamID()
{
	return *reinterpret_cast< uint32_t* >( this + 0x8 );
}

CUtlVector< C_EconItemView* >* CPlayerInventory::GetInventoryItems()
{
	return reinterpret_cast< CUtlVector<C_EconItemView*>* >( this + 0x2C );
}

C_EconItemView* CPlayerInventory::GetInventoryItemByItemID( int64_t id )
{
	static auto fnGetInventoryItemByItemID = reinterpret_cast< C_EconItemView*( __thiscall* )( void*, int64_t ) >( sig( "client.dll", "55 8B EC 8B 55 08 83 EC 10 8B C2" ) );

	auto econ = fnGetInventoryItemByItemID( this, id );

	if ( !econ || !*( BYTE* )( ( uintptr_t )econ + 0x204 ) )
		return g_pCSInventoryManager->FindOrCreateReferenceEconItem( id );
	else
		return econ;
}

bool CPlayerInventory::AddEconItem( CEconItem* item, int a3, int a4, char a5 )
{
	static auto fnAddEconItem = reinterpret_cast< C_EconItemView*( __thiscall* )( void*, CEconItem*, int, int, char ) >( sig( "client.dll", "55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 14 53 56 57 8B F9 8B 08" ) );

	GetBaseTypeCache()->AddObject( item );


	auto ret = fnAddEconItem( this, item, a3, a4, a5 );

	if ( ret )
	{
		auto i = GetInventoryItemByItemID( *item->GetItemID() );
		if ( i )
			*reinterpret_cast< bool* >( ( uintptr_t )i + 0xA1 ) = 1;
	}

	return ret;
}

C_EconItemView* CPlayerInventory::GetItemInLoadout( int team, int slot )
{
	typedef C_EconItemView*( __thiscall* tOriginal )( void*, unsigned int, signed int );
	return util::get_vfunc<tOriginal>( this, 8 )( this, team, slot );
}

C_EconItemView* CSInventoryManager::FindOrCreateReferenceEconItem( int64_t ID )
{
	static auto fnFindOrCreateReferenceEconItem = reinterpret_cast< C_EconItemView*( __thiscall* )( void*, int64_t ) >( sig( "client.dll", "55 8B EC 51 8B 55 0C 53 56" ) );
	return fnFindOrCreateReferenceEconItem( this, ID );
}

char CSInventoryManager::FilloutItemFromEconItem( C_EconItemView * view, CEconItem * item )
{
	static auto fnFilloutItemFromEconItem = reinterpret_cast< char( __thiscall* )( void*, void*, void* ) >( sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86" ) );
	return fnFilloutItemFromEconItem( this, view, item );
}

bool CSInventoryManager::EquipItemInLoadout( int team, int slot, uint64_t item_id )
{
	static auto fnEquipItemInLoadout = reinterpret_cast< bool( __thiscall* )( void*, int, int, uint64_t, bool ) >( sig( "client.dll", "55 8B EC 83 E4 F8 83 EC 2C 83 3D" ) );
	return fnEquipItemInLoadout( this, team, slot, item_id, true );
}
