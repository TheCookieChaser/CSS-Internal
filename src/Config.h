#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "json.h"
#include "Settings.h"

namespace Config
{
	const std::vector<std::string> GetConfigs();
	void LoadConfig(std::string configname);
	void SaveConfig(std::string configname, bool newconfig = false);
	void DeleteConfig(std::string configname);
}