#include "Menu.h"
#include "Math.h"
#include "SDK\CBaseEntity.h"

Tabs Menu::TabSelected = Tabs::Aimbot;

void Menu::EndScene()
{
	if (!Settings::Menu::Opened)
		return;

	ImGui::SetNextWindowSize(ImVec2(629, 342), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("CSS-Internal", &Settings::Menu::Opened, ImVec2(300, 250), 0.75f);
	{
		ImGui::Columns(2, NULL, false);
		{
			ImGui::SetColumnOffset(1, 130);
			ImGui::BeginChild("COL2", ImVec2(0, 280), true);
			{
				if (ImGui::Button("Aimbot", ImVec2(-1, 30)))
					TabSelected = Tabs::Aimbot;
				else if (ImGui::Button("Visuals", ImVec2(-1, 30)))
					TabSelected = Tabs::Visuals;
				else if (ImGui::Button("Misc", ImVec2(-1, 30)))
					TabSelected = Tabs::Misc;
				else if (ImGui::Button("Colors", ImVec2(-1, 30)))
					TabSelected = Tabs::Colors;
				else if (ImGui::Button("Config", ImVec2(-1, 30)))
					TabSelected = Tabs::Configs;
				else if (ImGui::Button("Players", ImVec2(-1, 30)))
					TabSelected = Tabs::Players;

				ImGui::EndChild();
			}
		}

		ImGui::NextColumn();
		{
			ImGui::BeginChild("COL3", ImVec2(0, 0), true);
			{
				switch (TabSelected)
				{
				case Tabs::Aimbot:
					AimbotTab();
					break;
				case Tabs::Visuals:
					VisualsTab();
					break;
				case Tabs::Misc:
					MiscTab();
					break;
				case Tabs::Colors:
					ColorsTab();
					break;
				case Tabs::Configs:
					ConfigsTab();
					break;
				case Tabs::Players:
					PlayersTab();
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

void Menu::AimbotTab()
{
	ImGui::Checkbox("Enabled", &Settings::Aimbot::Enalbed);
	ImGui::SliderFloat("FOV", &Settings::Aimbot::Fov, 0.f, 180.f);
	ImGui::SliderFloat("Smooth", &Settings::Aimbot::Smooth, 0.f, 50.f);
	ImGui::SliderInt("Hitbox", &Settings::Aimbot::Hitbox, 0, 18);
	ImGui::Checkbox("Silent", &Settings::Aimbot::Silent);
	ImGui::Checkbox("Auto Fire", &Settings::Aimbot::AutoFire);
	ImGui::Checkbox("On Key", &Settings::Aimbot::OnKey);
	ImGui::SliderInt("Key", &Settings::Aimbot::Key, 0, 256);
}

void Menu::VisualsTab()
{
	ImGui::Checkbox("Enabled", &Settings::Visuals::Enalbed);
	ImGui::Checkbox("Boxes", &Settings::Visuals::Box);
	ImGui::Checkbox("Health Bar", &Settings::Visuals::Health);
	ImGui::Checkbox("Snap Lines", &Settings::Visuals::SnapLine);
	ImGui::Checkbox("Player Names", &Settings::Visuals::Name);
	ImGui::Checkbox("Player Weapons", &Settings::Visuals::Weapon);
	ImGui::Checkbox("Far ESP", &Settings::Visuals::FarESP);
}

void Menu::MiscTab()
{
	ImGui::Checkbox("Auto Jump", &Settings::Misc::BunnyHop);
}

void Menu::ColorsTab()
{
	static int selectedcolor = 1;

	ImGui::Columns(2, NULL, false);
	{
		ImGui::SetColumnOffset(1, 210);
		ImGui::BeginChild("COL2", ImVec2(0, 0), true);
		{
			if (ImGui::Selectable("ESP T", selectedcolor == 1))
				selectedcolor = 1;
			if (ImGui::Selectable("ESP CT", selectedcolor == 2))
				selectedcolor = 2;
			if (ImGui::Selectable("ESP T Visible", selectedcolor == 3))
				selectedcolor = 3;
			if (ImGui::Selectable("ESP CT Visible", selectedcolor == 4))
				selectedcolor = 4;
			if (ImGui::Selectable("Watermark", selectedcolor == 5))
				selectedcolor = 5;

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild("COL3", ImVec2(0, 0), true);
		{
			switch (selectedcolor)
			{
			case 1:
				ImGui::ColorPicker3(Settings::Colors::ESP_NotVisible_T);
				break;
			case 2:
				ImGui::ColorPicker3(Settings::Colors::ESP_NotVisible_CT);
				break;
			case 3:
				ImGui::ColorPicker3(Settings::Colors::ESP_Visible_T);
				break;
			case 4:
				ImGui::ColorPicker3(Settings::Colors::ESP_Visible_CT);
				break;
			case 5:
				ImGui::ColorPicker3(Settings::Colors::Watermark);
				break;
			default:
				break;
			}

			ImGui::EndChild();
		}
	}
}

void Menu::ConfigsTab()
{
	static std::vector<std::string>configs = Config::GetConfigs();
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
					if (ImGui::Selectable(configs[i].c_str(), isselected))
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
			ImGui::InputText("##newconfig", buf, ARRAYSIZE(buf));

			if (ImGui::Button("New Config", ImVec2(-1, 0)))
			{
				Config::SaveConfig(buf, true);
				configs = Config::GetConfigs();
			}

			ImGui::Separator();

			if (ImGui::Button("Save", ImVec2(-1, 0)))
			{
				Config::SaveConfig(configs[config_selected]);
				configs = Config::GetConfigs();
			}

			if (ImGui::Button("Load", ImVec2(-1, 0)))
			{
				Config::LoadConfig(configs[config_selected]);
				configs = Config::GetConfigs();
			}

			if (ImGui::Button("Delete", ImVec2(-1, 0)))
				ImGui::OpenPopup("delete_warning");

			if (ImGui::BeginPopup("delete_warning"))
			{
				ImGui::Text("Are you sure you want to delete config '%s' ?", configs[config_selected].c_str());
				if (ImGui::Button("Yes", ImVec2(50, 20)))
				{
					Config::DeleteConfig(configs[config_selected]);
					configs = Config::GetConfigs();
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

void Menu::PlayersTab()
{
	static Vector eyeangle[32];
	static int player_selected = 1;

	ImGui::Columns(2);
	{
		ImGui::SetColumnOffset(1, 300);
		ImGui::BeginChild("COL2", ImVec2(0, 0), true);
		{
			for (int i = 0; i <= entitylist->GetHighestEntityIndex(); i++)
			{
				auto pEntity = static_cast<C_CSPlayer*>(entitylist->GetClientEntity(i));

				if (!pEntity || i == engine->GetLocalPlayer())
					continue;

				player_info_t info;
				if (!engine->GetPlayerInfo(i, &info))
					continue;

				eyeangle[i] = *pEntity->GetEyeAngles();
				Math::NormalizeAngles(eyeangle[i]);

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
			ImGui::Text(("Angle X: " + std::to_string(eyeangle[player_selected].x)).c_str());
			ImGui::Text(("Angle Y: " + std::to_string(eyeangle[player_selected].y)).c_str());
			ImGui::EndChild();
		}
	}
}