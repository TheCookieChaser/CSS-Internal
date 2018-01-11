#pragma once

#include <imgui/imgui.h>

class cmenu
{
public:
	void render();

private:
	enum class tabs_e
	{
		Aimbot,
		Visuals,
		Misc,
		Colors,
		Configs,
		Players
	};

	void aimbot_tab();
	void visuals_tab();
	void misc_tab();
	void colors_tab();
	void configs_tab();
	void players_tab();

	tabs_e m_tab_selected;
};

extern cmenu* menu;