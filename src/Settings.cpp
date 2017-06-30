#include "Settings.h"

namespace Settings
{
	namespace Menu
	{
		bool Opened;
	}
	namespace Aimbot
	{
		bool Enalbed;
		float Fov;
		float Smooth;
		int Hitbox;
		bool Silent;
		bool AutoFire;
		bool OnKey;
		int Key;
	}
	namespace Visuals
	{
		bool Enalbed;
		bool Box;
		bool Health;
		bool SnapLine;
		bool Name;
		bool Weapon;
		bool FarESP;
	}
	namespace Misc
	{
		bool BunnyHop;
	}
}

float Settings::Colors::ESP_NotVisible_T[3]{ 163.f / 255.f, 33.f / 255.f, 13.f / 255.f };
float Settings::Colors::ESP_NotVisible_CT[3]{0, 84.f / 255.f, 181.f / 255.f };
float Settings::Colors::ESP_Visible_T[3]{ 255.f / 255.f, 208.f / 255.f, 0 };
float Settings::Colors::ESP_Visible_CT[3]{ 91.f / 255.f, 219.f / 255.f, 0 };
float Settings::Colors::Watermark[3]{ 150.f / 255.f, 255.f / 255.f, 150.f / 255.f };