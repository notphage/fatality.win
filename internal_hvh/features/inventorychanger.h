#pragma once


struct inventory_item_t
{
	uint64_t id;
	int definition_index;
	int paint_kit;
	int seed;
	float wear = 0.f;
	int stat_trak;
	std::array<uint32_t, 2> equipped_state;
	CEconItem* item = nullptr;
	bool update = false;
	bool fullupdate = false;
};

class inventorychanger : public singleton<inventorychanger>
{
public:
	uint64_t get_highest_id() const;
	void update();
	void update_selected();
	void add();
	void set( int index );
	void update_items( bool recheck = false );
	void load();
	void save( bool autosave = false );
	void remove();
	void update_equipped( const ClientFrameStage_t stage );
	std::vector<inventory_item_t> &get_items() { return items; }
	std::vector<std::string> &get_list_items() { return list_items; }
	CPlayerInventory* get_inventory() const { return inventory; }
private:
	std::vector<inventory_item_t> items;
	CPlayerInventory* inventory = nullptr;
	std::vector<std::string> list_items;
};