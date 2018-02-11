#include "config.h"
#include <Windows.h>
#include <fstream>

config_s config;

BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::vector<std::string> config_manager::get_configs()
{
	if (!DirectoryExists("CSS-Internal"))
		CreateDirectory("CSS-Internal", nullptr);

	std::string config_extension = ".JSON";
	std::vector<std::string> names;

	WIN32_FIND_DATAA find_data;
	HANDLE preset_file = FindFirstFileA(("CSS-Internal\\*" + config_extension).c_str(), &find_data);

	if (preset_file != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			names.push_back(find_data.cFileName);
		} while (FindNextFileA(preset_file, &find_data));

		FindClose(preset_file);
	}

	return names;
}

void config_manager::load_config(std::string configname)
{
	std::ifstream input_file = std::ifstream(("CSS-Internal\\" + configname).c_str());

	if (!input_file.good())
		return;

	nlohmann::json j;

	try
	{
		input_file >> j;
	}
	catch (...)
	{
		input_file.close();
		return;
	}

	config.aimbot_enabled = j.at("aimbot").at("enabled").get<bool>();
	config.aimbot_fov = j.at("aimbot").at("fov").get<float>();
	config.aimbot_smooth = j.at("aimbot").at("smooth").get<float>();
	config.aimbot_hitbox = j.at("aimbot").at("hitbox").get<int>();
	config.aimbot_silent = j.at("aimbot").at("silent").get<bool>();
	config.aimbot_autofire = j.at("aimbot").at("autofire").get<bool>();
	config.aimbot_on_key = j.at("aimbot").at("on_key").get<bool>();
	config.aimbot_key = j.at("aimbot").at("key").get<int>();

	config.visuals_enabled = j.at("visuals").at("enabled").get<bool>();
	config.visuals_box = j.at("visuals").at("box").get<bool>();
	config.visuals_health = j.at("visuals").at("health").get<bool>();
	config.visuals_snapline = j.at("visuals").at("snapline").get<bool>();
	config.visuals_name = j.at("visuals").at("name").get<bool>();
	config.visuals_weapon = j.at("visuals").at("weapon").get<bool>();

	config.misc_autojump = j.at("misc").at("autojump").get<bool>();

	input_file.close();
}

void config_manager::save_config(std::string configname)
{
	auto j = nlohmann::json {
		{ "aimbot", {
			{ "enabled", config.aimbot_enabled },
			{ "fov", config.aimbot_fov },
			{ "smooth", config.aimbot_smooth },
			{ "hitbox", config.aimbot_hitbox },
			{ "silent", config.aimbot_silent },
			{ "autofire", config.aimbot_autofire },
			{ "on_key", config.aimbot_on_key },
			{ "key", config.aimbot_key }
		} },
		{ "visuals", {
			{ "enabled", config.visuals_enabled },
			{ "box", config.visuals_box },
			{ "health", config.visuals_health },
			{ "snapline", config.visuals_snapline },
			{ "name", config.visuals_name },
			{ "weapon", config.visuals_weapon }
		} },
		{ "misc", {
			{ "autojump", config.misc_autojump },
		} },
	};

	std::ofstream o(("CSS-Internal\\" + configname).c_str());
	o << std::setw(4) << j << std::endl;
}

void config_manager::remove_config(std::string configname)
{
	std::remove(("CSS-Internal\\" + configname).c_str());
}