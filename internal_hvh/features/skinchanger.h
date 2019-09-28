#pragma once

struct Kit_t
{
	int id;
	std::string name;

	bool operator < ( const Kit_t& other ) const { return ( name < other.name ); }
};

class skinchanger : public singleton<skinchanger>
{
public:
	void spawn_glove();
	void apply_glove( ) const;
	void run( const ClientFrameStage_t stage );
	void parse_kits();

	std::vector<Kit_t> k_skins;
	std::vector<std::string> k_pure_skin_names;
	std::vector<size_t> k_skin_name_to_kit;

	uintptr_t glove_handle = 0;
};