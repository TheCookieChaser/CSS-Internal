#include "menu.h"
#include "tools/entity.h"
#include "tools/config.h"

cmenu* menu = new cmenu();

bool color_picker(const char* label, ImColor& col, ImGuiColorEditFlags flags = 0)
{
	float color[3];
	color[0] = col.Value.x;
	color[1] = col.Value.y;
	color[2] = col.Value.z;
	if (!ImGui::ColorPicker3(label, color, flags))
		return false;

	col.Value.x = color[0];
	col.Value.y = color[1];
	col.Value.z = color[2];

	return true;
}

void cmenu::render()
{
	if (!config.menu_opened)
		return;

	ImGui::SetNextWindowSize(ImVec2(629, 342), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("CSS-Internal", &config.menu_opened, ImVec2(300, 250), 0.75f);
	{
		ImGui::Columns(2, NULL, false);
		{
			ImGui::SetColumnOffset(1, 130);
			ImGui::BeginChild("COL2", ImVec2(0, 280), true);
			{
				if (ImGui::Button("Aimbot", ImVec2(-1, 30)))
					m_tab_selected = tabs_e::Aimbot;
				else if (ImGui::Button("Visuals", ImVec2(-1, 30)))
					m_tab_selected = tabs_e::Visuals;
				else if (ImGui::Button("Misc", ImVec2(-1, 30)))
					m_tab_selected = tabs_e::Misc;
				else if (ImGui::Button("Colors", ImVec2(-1, 30)))
					m_tab_selected = tabs_e::Colors;
				else if (ImGui::Button("Config", ImVec2(-1, 30)))
					m_tab_selected = tabs_e::Configs;
				else if (ImGui::Button("Players", ImVec2(-1, 30)))
					m_tab_selected = tabs_e::Players;

				ImGui::EndChild();
			}
		}

		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL3", ImVec2(0, 0), true);
			{
				switch (m_tab_selected)
				{
				case tabs_e::Aimbot:
					aimbot_tab();
					break;
				case tabs_e::Visuals:
					visuals_tab();
					break;
				case tabs_e::Misc:
					misc_tab();
					break;
				case tabs_e::Colors:
					colors_tab();
					break;
				case tabs_e::Configs:
					configs_tab();
					break;
				case tabs_e::Players:
					players_tab();
					break;
				default:
					break;
				}

				ImGui::EndChild();
			}
		}
	}

	ImGui::End();
}

void cmenu::aimbot_tab()
{
	ImGui::Checkbox("Enabled", &config.aimbot_enabled);
	ImGui::SliderFloat("FOV", &config.aimbot_fov, 0.f, 360.f);
	ImGui::SliderFloat("Smooth", &config.aimbot_smooth, 0.f, 50.f);
	ImGui::SliderInt("Hitbox", &config.aimbot_hitbox, 0, 18);
	ImGui::Checkbox("Silent", &config.aimbot_silent);
	ImGui::Checkbox("Auto Fire", &config.aimbot_autofire);
	ImGui::Checkbox("On Key", &config.aimbot_on_key);
	ImGui::SliderInt("Key", &config.aimbot_key, 0, 256);
}

void cmenu::visuals_tab()
{
	ImGui::Checkbox("Enabled", &config.visuals_enabled);
	ImGui::Checkbox("Box", &config.visuals_box);
	ImGui::Checkbox("Health Bar", &config.visuals_health);
	ImGui::Checkbox("Snap Lines", &config.visuals_snapline);
	ImGui::Checkbox("Names", &config.visuals_name);
	ImGui::Checkbox("Weapons", &config.visuals_weapon);
}

void cmenu::misc_tab()
{
	ImGui::Checkbox("Auto Jump", &config.misc_autojump);
}

void cmenu::colors_tab()
{
	color_picker("ESP T", config.colors_esp_t);
	color_picker("ESP CT", config.colors_esp_ct);
	color_picker("ESP T Visible", config.colors_esp_visible_t);
	color_picker("ESP CT Visible", config.colors_esp_visible_ct);
	color_picker("Watermark", config.colors_watermark);
}

void cmenu::configs_tab()
{
	static std::vector<std::string>configs = config_manager::get_configs();
	static int config_selected = 0;
	ImGui::Columns(2);
	{
		ImGui::SetColumnOffset(1, 330);
		ImGui::BeginChild("COL2", ImVec2(0, 0), true);
		{
			if (configs.size() <= 0)
				ImGui::Text("No configs found!");
			else
			{
				for (size_t i = 0; i < configs.size(); i++)
				{
					bool isselected = config_selected == i;
					if (ImGui::Selectable(configs.at(i).c_str(), isselected))
						config_selected = i;
				}
			}

			ImGui::EndChild();
		}

	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("COL3", ImVec2(0, 0), true);
		{
			static char buf[64];
			ImGui::InputText("##newconfig", buf, IM_ARRAYSIZE(buf));

			if (ImGui::Button("New", ImVec2(-1, 0)))
			{
				config_manager::save_config(std::string(buf) + ".json");
				configs = config_manager::get_configs();
			}

			ImGui::Separator();

			if (ImGui::Button("Save", ImVec2(-1, 0)))
			{
				config_manager::save_config(configs.at(config_selected));
				configs = config_manager::get_configs();
			}

			if (ImGui::Button("Load", ImVec2(-1, 0)))
			{
				config_manager::load_config(configs.at(config_selected));
				configs = config_manager::get_configs();
			}

			if (ImGui::Button("Remove", ImVec2(-1, 0)))
				ImGui::OpenPopup("remove_warning");

			if (ImGui::BeginPopup("remove_warning"))
			{
				ImGui::Text("Are you sure you want to remove config '%s' ?", configs.at(config_selected).c_str());
				if (ImGui::Button("Yes", ImVec2(50, 20)))
				{
					config_manager::remove_config(configs.at(config_selected));
					configs = config_manager::get_configs();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("No", ImVec2(50, 20)))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			ImGui::EndChild();
		}
	}
}

void cmenu::players_tab()
{
	static std::array<Vector, 65> eyeangles{ Vector(0, 0, 0) };
	static int player_selected = 0;

	ImGui::Columns(2);
	{
		ImGui::SetColumnOffset(1, 300);
		ImGui::BeginChild("COL2", ImVec2(0, 0), true);
		{
			for (int i = 0; i <= g_entitylist->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_CSPlayer*>(g_entitylist->GetClientEntity(i));

				if (!pEntity || i == g_engine->GetLocalPlayer())
					continue;

				player_info_t info;
				if (!g_engine->GetPlayerInfo(i, &info))
					continue;

				eyeangles.at(i) = pEntity->get_eye_angles();

				bool isselected = player_selected == i;
				if (ImGui::Selectable(info.szName, isselected))
					player_selected = i;
			}

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("COL3", ImVec2(0, 0), true);
		{
			ImGui::Text(("Viewangle x: " + std::to_string(eyeangles.at(player_selected).x)).c_str());
			ImGui::Text(("Viewangle y: " + std::to_string(eyeangles.at(player_selected).y)).c_str());
			ImGui::EndChild();
		}
	}
}