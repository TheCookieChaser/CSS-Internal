#pragma once

#include <vector>
#include <json/single_include/nlohmann/json.hpp>
#include "../sdk/Color.h"

struct config_s
{
	config_s() {};

	bool menu_opened = false;

	bool aimbot_enabled = false;
	float aimbot_fov = 0;
	float aimbot_smooth = 0;
	int aimbot_hitbox = 0;
	bool aimbot_silent = false;
	bool aimbot_psilent = false;
	bool aimbot_autofire = false;
	bool aimbot_nospread = false;
	float aimbot_mindmg = 20;
	bool aimbot_on_key = false;
	int aimbot_key = 0;

	bool visuals_enabled = false;
	bool visuals_box = false;
	bool visuals_health = false;
	bool visuals_snapline = false;
	bool visuals_name = false;
	bool visuals_weapon = false;
	bool visuals_triggerbot_hitbox = false;
	bool visuals_triggerbot_hit_point = false;
	bool visuals_spread_circle = false;
	bool visuals_no_flash = false;

	bool misc_autojump = false;
	int misc_triggerbot = 0;
	bool misc_triggerbot_on_key = false;
	int misc_triggerbot_key = 0;
	bool misc_triggerbot_filter[6]{ false, false, false, false, false, false };

	Color colors_esp_t = Color(163, 33, 13, 255);
	Color colors_esp_ct = Color(0, 84, 181, 255);
	Color colors_esp_visible_t = Color(255, 208, 0, 255);
	Color colors_esp_visible_ct = Color(91, 219, 0, 255);
	Color colors_watermark = Color(0, 255, 0, 255);
};

extern config_s config;

namespace config_manager
{
	std::vector<std::string> get_configs();
	void load_config(std::string configname);
	void save_config(std::string configname);
	void remove_config(std::string configname);
}