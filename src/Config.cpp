#include "Config.h"

BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

const std::vector<std::string> Config::GetConfigs()
{
	if (!DirectoryExists("CSS-Internal"))
		CreateDirectory("CSS-Internal", NULL);

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

template<class T>
void LoadSetting(nlohmann::json& preset, T& setting)
{
	if (preset.is_null())
		return;

	setting = preset;
}

template<class T>
void SaveSetting(nlohmann::json& preset, T& setting)
{
	preset = setting;
}

void LoadColor(nlohmann::json& preset, float color[3])
{
	color[0] = preset["0"];
	color[1] = preset["1"];
	color[2] = preset["2"];
}

void SaveColor(nlohmann::json& preset, float color[3])
{
	preset["0"] = color[0];
	preset["1"] = color[1];
	preset["2"] = color[2];
}

void Config::LoadConfig(std::string configname)
{
	std::ifstream input_file = std::ifstream(("CSS-Internal\\" + configname).c_str());

	if (!input_file.good())
		return;

	nlohmann::json preset;

	try
	{
		preset << input_file;
	}
	catch (...)
	{
		input_file.close();
		return;
	}

	LoadSetting<bool>(preset["Aimbot"]["Enalbed"], Settings::Aimbot::Enalbed);
	LoadSetting<float>(preset["Aimbot"]["Fov"], Settings::Aimbot::Fov);
	LoadSetting<float>(preset["Aimbot"]["Smooth"], Settings::Aimbot::Smooth);
	LoadSetting<int>(preset["Aimbot"]["Hitbox"], Settings::Aimbot::Hitbox);
	LoadSetting<bool>(preset["Aimbot"]["Silent"], Settings::Aimbot::Silent);
	LoadSetting<bool>(preset["Aimbot"]["AutoFire"], Settings::Aimbot::AutoFire);
	LoadSetting<bool>(preset["Aimbot"]["OnKey"], Settings::Aimbot::OnKey);
	LoadSetting<int>(preset["Aimbot"]["Key"], Settings::Aimbot::Key);

	LoadSetting<bool>(preset["Visuals"]["Enalbed"], Settings::Visuals::Enalbed);
	LoadSetting<bool>(preset["Visuals"]["Box"], Settings::Visuals::Box);
	LoadSetting<bool>(preset["Visuals"]["Health"], Settings::Visuals::Health);
	LoadSetting<bool>(preset["Visuals"]["SnapLine"], Settings::Visuals::SnapLine);
	LoadSetting<bool>(preset["Visuals"]["Name"], Settings::Visuals::Name);
	LoadSetting<bool>(preset["Visuals"]["Weapon"], Settings::Visuals::Weapon);

	LoadSetting<bool>(preset["Misc"]["BunnyHop"], Settings::Misc::BunnyHop);

	LoadColor(preset["Colors"]["ESP_NotVisible_T"], Settings::Colors::ESP_NotVisible_T);
	LoadColor(preset["Colors"]["ESP_NotVisible_CT"], Settings::Colors::ESP_NotVisible_CT);
	LoadColor(preset["Colors"]["ESP_Visible_T"], Settings::Colors::ESP_Visible_T);
	LoadColor(preset["Colors"]["ESP_Visible_CT"], Settings::Colors::ESP_Visible_CT);
	LoadColor(preset["Colors"]["Watermark"], Settings::Colors::Watermark);

	input_file.close();
}

void Config::SaveConfig(std::string configname, bool newconfig)
{
	std::ofstream output_file(("CSS-Internal\\" + configname + (newconfig ? ".JSON" : "")).c_str());

	if (!output_file.good())
		return;

	nlohmann::json preset;

	SaveSetting<bool>(preset["Aimbot"]["Enalbed"], Settings::Aimbot::Enalbed);
	SaveSetting<float>(preset["Aimbot"]["Fov"], Settings::Aimbot::Fov);
	SaveSetting<float>(preset["Aimbot"]["Smooth"], Settings::Aimbot::Smooth);
	SaveSetting<int>(preset["Aimbot"]["Hitbox"], Settings::Aimbot::Hitbox);
	SaveSetting<bool>(preset["Aimbot"]["Silent"], Settings::Aimbot::Silent);
	SaveSetting<bool>(preset["Aimbot"]["AutoFire"], Settings::Aimbot::AutoFire);
	SaveSetting<bool>(preset["Aimbot"]["OnKey"], Settings::Aimbot::OnKey);
	SaveSetting<int>(preset["Aimbot"]["Key"], Settings::Aimbot::Key);

	SaveSetting<bool>(preset["Visuals"]["Enalbed"], Settings::Visuals::Enalbed);
	SaveSetting<bool>(preset["Visuals"]["Box"], Settings::Visuals::Box);
	SaveSetting<bool>(preset["Visuals"]["Health"], Settings::Visuals::Health);
	SaveSetting<bool>(preset["Visuals"]["SnapLine"], Settings::Visuals::SnapLine);
	SaveSetting<bool>(preset["Visuals"]["Name"], Settings::Visuals::Name);
	SaveSetting<bool>(preset["Visuals"]["Weapon"], Settings::Visuals::Weapon);

	SaveSetting<bool>(preset["Misc"]["BunnyHop"], Settings::Misc::BunnyHop);

	SaveColor(preset["Colors"]["ESP_NotVisible_T"], Settings::Colors::ESP_NotVisible_T);
	SaveColor(preset["Colors"]["ESP_NotVisible_CT"], Settings::Colors::ESP_NotVisible_CT);
	SaveColor(preset["Colors"]["ESP_Visible_T"], Settings::Colors::ESP_Visible_T);
	SaveColor(preset["Colors"]["ESP_Visible_CT"], Settings::Colors::ESP_Visible_CT);
	SaveColor(preset["Colors"]["Watermark"], Settings::Colors::Watermark);

	output_file << std::setw(4) << preset << std::endl;
	output_file.close();
}

void Config::DeleteConfig(std::string configname)
{
	std::remove(("CSS-Internal\\" + configname).c_str());
}