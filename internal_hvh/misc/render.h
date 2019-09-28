#pragma once

#include "../includes.h"
#include "d3dfont.h"
#include "../singleton.h"


namespace fonts
{
	extern std::shared_ptr<c_font> esp;
	extern std::shared_ptr<c_font> esp_small;
	extern std::shared_ptr<c_font> lby;

	extern std::shared_ptr<c_font> controlfont;
	extern std::shared_ptr<c_font> tabfont;
	extern std::shared_ptr<c_font> tabfontthicc;
	extern std::shared_ptr<c_font> subtabfont;
	extern std::shared_ptr<c_font> smallfont;
	extern std::shared_ptr<c_font> keybindfont;
}

class Color;

class render : public singleton<render>
{
	struct vertex
	{
		float x, y, z, rhw;
		uint32_t color;
	};

public:
	render() = default;
	~render() = default;
	void init( IDirect3DDevice9* dev );

	void limit( Vector2D pos, Vector2D size ) const;

	void reset_limit() const;
	
	void line( Vector2D from, Vector2D to, Color color ) const;

	void rect( Vector2D from, Vector2D size, Color color ) const;
	void rect_linear_gradient( Vector2D from, Vector2D to, Color color1, Color color2, bool horizontal = false ) const;
	void rect_filled( Vector2D from, Vector2D size, Color color ) const;
	void rect_filled_linear_gradient( Vector2D from, Vector2D size, Color color1, Color color2, bool horizontal = false ) const;
	void rect_filled_radial_gradient( Vector2D from, Vector2D size, Color color1, Color color2 );
	void rect_filled_diamond_gradient( Vector2D from, Vector2D size, Color color1, Color color2 ) const;

	void triangle( Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color ) const;
	void triangle_filled( Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color ) const;
	void triangle_filled_linear_gradient( Vector2D pos1, Vector2D pos2, Vector2D pos3, Color color1, Color color2, Color color3 ) const;

	void circle( Vector2D center, float radius, Color color );
	void circle_filled( Vector2D center, float radius, Color color );
	void circle_filled_radial_gradient( Vector2D center, float radius, Color color1, Color color2 );
	void circle_filled_rainbow( Vector2D center, float radius, bool rotate ) const;

	void filled_triangle( POINT pos1, POINT pos2, POINT pos3, Color color ) const;

	void filled_box( const int x, const int y, const int w, const int h, const Color color ) const;
	void filled_box_outlined( const int x, const int y, const int w, const int h, const Color color, const Color outline, const int thickness = 1 ) const;
	void bordered_box( const int x, const int y, const int w, const int h, const Color color, const int thickness = 1 ) const;
	void bordered_box_outlined( const int x, const int y, const int w, const int h, const Color color, const Color outline, const int thickness = 1 ) const;

	static void text( Vector2D pos, std::wstring string, Color color, std::shared_ptr<c_font>& font, uint8_t flags = 0 );
	void text( Vector2D pos, std::string string, Color color, std::shared_ptr<c_font>& font, uint8_t flags = 0 ) const;
	static void text_radial( Vector2D pos, std::wstring string, Color color1, Color color2, std::shared_ptr<c_font>& font,
					  uint8_t flags = 0 );
	void text_radial( Vector2D pos, std::string string, Color color1, Color color2, std::shared_ptr<c_font>& font,
					  uint8_t flags = 0) const;

	Vector2D get_text_size( std::string string, std::shared_ptr<c_font>& font ) const;
	static Vector2D get_text_size( std::wstring string, std::shared_ptr<c_font>& font );

	void scale( Vector2D& vec ) const;
	Vector2D get_center() const;
	D3DVIEWPORT9 get_dimensions() const;
	IDirect3DDevice9* get_device() const { return dev; }
	bool get_init() const { return m_init; }

	void init_device_objects( IDirect3DDevice9* dev );
	void invalidate_device_objects();
	void setup_render_state() const;

private:
	static constexpr auto points = 64;

	IDirect3DDevice9* dev;
	D3DVIEWPORT9 port;
	std::vector<Vector2D> lookup_table;
	std::vector<std::shared_ptr<c_font>> fonts;

	bool m_init = false;

	void build_lookup_table();
};
