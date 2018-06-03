#pragma once

#include "../tools/entity.h"
#include "../tools/draw_manager.h"
#include "../tools/config.h"

class cvisuals
{
public:
	struct rect_s
	{
		float left, top, right, bottom;
	};

	void render();
	void render_esp();
	void render_players(C_CSPlayer * player);
	ImColor get_player_color(C_CSPlayer * pEntity);
	ImColor get_health_color(int health);
	bool get_bounding_box(C_BasePlayer * entity, rect_s& rect);
};

extern cvisuals* visuals;