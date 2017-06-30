#pragma once

#include "Hooks.h"
#include "Settings.h"
#include "Config.h"

enum class Tabs : int
{
	Aimbot,
	Visuals,
	Misc,
	Colors,
	Configs,
	Players
};

namespace Menu
{
	void EndScene();
	void AimbotTab();
	void VisualsTab();
	void MiscTab();
	void ColorsTab();
	void ConfigsTab();
	void PlayersTab();

	extern Tabs TabSelected;
}