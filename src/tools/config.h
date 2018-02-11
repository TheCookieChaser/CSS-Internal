#pragma once

#include <vector>
#include <json/single_include/nlohmann/json.hpp>
#include <imgui/imgui.h>

struct config_s
{
	config_s() {};

	bool menu_opened = false;

	bool aimbot_enabled = false;
	float aimbot_fov = 0;
	float aimbot_smooth = 0;
	int aimbot_hitbox = 0;
	bool aimbot_silent = false;
	bool aimbot_autofire = false;
	bool aimbot_on_key = false;
	int aimbot_key = 0;

	bool visuals_enabled = false;
	bool visuals_box = false;
	bool visuals_health = false;
	bool visuals_snapline = false;
	bool visuals_name = false;
	bool visuals_weapon = false;

	bool misc_autojump = false;

	ImColor colors_esp_t = ImColor(163, 33, 13, 255);
	ImColor colors_esp_ct = ImColor(0, 84, 181, 255);
	ImColor colors_esp_visible_t = ImColor(255, 208, 0, 255);
	ImColor colors_esp_visible_ct = ImColor(91, 219, 0, 255);
	ImColor colors_watermark = ImColor(150, 255, 150, 255);
};

extern config_s config;

namespace config_manager
{
	std::vector<std::string> get_configs();
	void load_config(std::string configname);
	void save_config(std::string configname);
	void remove_config(std::string configname);
}