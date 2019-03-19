#pragma once

#include "../tools/entity.h"
#include "../tools/config.h"

class cvisuals
{
public:
	struct rect_s
	{
		float left, top, right, bottom;
	};

	void initialize();
	bool is_initialized() const { return initialized; }
	void render();
	void render_esp();
	void render_player(C_CSPlayer * player);
	void render_spread_circle();
	Color get_player_color(C_CSPlayer * pEntity);
	Color get_health_color(int health);
	bool get_bounding_box(C_BasePlayer * entity, rect_s& rect);
private:
	HFont courier_new = 0;
	bool initialized = false;
};

extern cvisuals* visuals;