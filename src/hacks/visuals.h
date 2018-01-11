#pragma once

#include "../tools/entity.h"
#include "../tools/draw_manager.h"
#include "../tools/config.h"

class cvisuals
{
public:
	void render();
	void render_esp();
	void render_players(C_CSPlayer * player);
	ImColor get_player_color(C_CSPlayer * pEntity);
	ImColor get_health_color(int health);
};

extern cvisuals* visuals;