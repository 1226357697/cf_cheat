#include "menu_config.hpp"
#include "IniFile.h"
#include <shlobj.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

static fs::path get_config_path()
{
	fs::path path;
	const char* config_name = "crossfire.ini";
	char path_buffer[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path_buffer)))
	{
		path = path_buffer;
		path = path/"cf_hack";
		if (!fs::exists(path)) 
			fs::create_directories(path);
		path = path / config_name;
	}
	else
	{
		strcpy(path_buffer, "./");
		strcat(path_buffer, config_name);
	}

	return path;
}


static void loadESPConfig(IniFile& ini)
{
	MenuConfig::ShowAimRangle = ini.readBool("ESP", "ShowBoneESP", MenuConfig::ShowAimRangle);
	MenuConfig::ShowBoxESP = ini.readBool("ESP", "ShowBoxESP", MenuConfig::ShowBoxESP);
	MenuConfig::ShowHealthBar = ini.readBool("ESP", "ShowHealthBar", MenuConfig::ShowHealthBar);
	MenuConfig::ShowEyeRay = ini.readBool("ESP", "ShowEyeRay", MenuConfig::ShowEyeRay);
	MenuConfig::ShowPlayerName = ini.readBool("ESP", "ShowPlayerName", MenuConfig::ShowPlayerName);
	MenuConfig::ShowPlayerHP = (MenuConfig::ShowHPWay)ini.readInt("ESP", "ShowPlayerHP", MenuConfig::ShowPlayerHP);
	MenuConfig::ShowPlayerDistance = ini.readBool("ESP", "ShowPlayerDistance", MenuConfig::ShowPlayerDistance);
	MenuConfig::ShowC4 = ini.readBool("ESP", "ShowC4", MenuConfig::ShowC4);
	MenuConfig::ShowRadar = ini.readBool("ESP", "ShowRadar", MenuConfig::ShowRadar);
	MenuConfig::ShowLineToEnemy = ini.readBool("ESP", "ShowLineToEnemy", MenuConfig::ShowLineToEnemy);
	MenuConfig::RadarType =  ini.readInt("ESP", "RadarType", MenuConfig::RadarType);
	MenuConfig::ShowWeaponESP = ini.readBool("ESP", "ShowWeaponESP", MenuConfig::ShowWeaponESP);

	MenuConfig::BoneColor = ini.readInt("ESP", "BoneColor", MenuConfig::BoneColor);
	MenuConfig::BoxColor = ini.readInt("ESP", "BoxColor", MenuConfig::BoxColor);
	MenuConfig::AngleColor = ini.readInt("ESP", "AngleColor", MenuConfig::AngleColor);
	MenuConfig::LineToEnemyColor = ini.readInt("ESP", "LineToEnemyColor", MenuConfig::LineToEnemyColor);
	MenuConfig::EyeRayColor = ini.readInt("ESP", "EyeRayColor", MenuConfig::EyeRayColor);

}

static void loadAimBotConfig(IniFile& ini)
{
	MenuConfig::AimBot = ini.readBool("AimBot", "AimBot", MenuConfig::AimBot);
	MenuConfig::AimBotHotKey = ini.readInt("AimBot", "AimBotHotKey", MenuConfig::AimBotHotKey);
	MenuConfig::AimPosition = (MenuConfig::AimBotPos)ini.readInt("AimBot", "AimPosition", MenuConfig::AimPosition);
	MenuConfig::AimPositionIndex = ini.readInt("AimBot", "AimPositionIndex", MenuConfig::AimPositionIndex);
	MenuConfig::ShowAimRangle = ini.readBool("AimBot", "ShowAimRangle", MenuConfig::ShowAimRangle);
	MenuConfig::AimRangle = ini.readFloat("AimBot", "AimRangle", MenuConfig::AimRangle);
	MenuConfig::AimSmooth = ini.readFloat("AimBot", "AimSmooth", MenuConfig::AimSmooth);
}

static void loadGlobalConfig(IniFile& ini)
{
	MenuConfig::TeamCheck = ini.readBool("Global", "TeamCheck", MenuConfig::TeamCheck);
	MenuConfig::ShowMenu = ini.readBool("Global", "ShowMenu", MenuConfig::ShowMenu);
}

void MenuConfig::load()
{
	fs::path cfg_path = get_config_path();
	if(!fs::exists(cfg_path))
		return ;
  IniFile ini(cfg_path.string());
  // ESP
  loadESPConfig(ini);

  // AIMBOT
  loadAimBotConfig(ini);

  // GLOBAL
  loadGlobalConfig(ini);
}


static void saveESPConfig(IniFile& ini)
{
	 ini.writeBool("ESP", "ShowBoneESP", MenuConfig::ShowAimRangle);
	 ini.writeBool("ESP", "ShowBoxESP", MenuConfig::ShowBoxESP);
	 ini.writeBool("ESP", "ShowHealthBar", MenuConfig::ShowHealthBar);
	 ini.writeBool("ESP", "ShowEyeRay", MenuConfig::ShowEyeRay);
	 ini.writeBool("ESP", "ShowPlayerName", MenuConfig::ShowPlayerName);
	 ini.writeInt("ESP", "ShowPlayerHP", (int)MenuConfig::ShowPlayerHP);
	 ini.writeBool("ESP", "ShowPlayerDistance", MenuConfig::ShowPlayerDistance);
	 ini.writeBool("ESP", "ShowC4", MenuConfig::ShowC4);
	 ini.writeBool("ESP", "ShowRadar", MenuConfig::ShowRadar);
	 ini.writeBool("ESP", "ShowLineToEnemy", MenuConfig::ShowLineToEnemy);
	 ini.writeInt("ESP", "RadarType", MenuConfig::RadarType);
	 ini.writeBool("ESP", "ShowWeaponESP", MenuConfig::ShowWeaponESP);

	 ini.writeInt("ESP", "BoneColor", MenuConfig::BoneColor);
	 ini.writeInt("ESP", "BoxColor", MenuConfig::BoxColor);
	 ini.writeInt("ESP", "AngleColor", MenuConfig::AngleColor);
	 ini.writeInt("ESP", "LineToEnemyColor", MenuConfig::LineToEnemyColor);
	 ini.writeInt("ESP", "EyeRayColor", MenuConfig::EyeRayColor);
}

static void saveAimBotConfig(IniFile& ini)
{
	 ini.writeBool("AimBot", "AimBot", MenuConfig::AimBot);
	 ini.writeInt("AimBot", "AimBotHotKey", MenuConfig::AimBotHotKey);
	 (MenuConfig::AimBotPos)ini.writeInt("AimBot", "AimPosition", MenuConfig::AimPosition);
	 ini.writeInt("AimBot", "AimPositionIndex", MenuConfig::AimPositionIndex);
	 ini.writeBool("AimBot", "ShowAimRangle", MenuConfig::ShowAimRangle);
	 ini.writeFloat("AimBot", "AimRangle", MenuConfig::AimRangle);
	 ini.writeFloat("AimBot", "AimSmooth", MenuConfig::AimSmooth);
}

static void saveGlobalConfig(IniFile& ini)
{
	ini.writeBool("Global", "TeamCheck", MenuConfig::TeamCheck);
	ini.writeBool("Global", "ShowMenu", MenuConfig::ShowMenu);
}

static bool create_file(const fs::path& path)
{
	std::ofstream outFile(path);
	return fs::exists(path);
}

void MenuConfig::save()
{
	fs::path cfg_path = get_config_path();
	if (!fs::exists(cfg_path) && !create_file(cfg_path.string()))
		return;

  IniFile ini(cfg_path.string());
  // ESP
  saveESPConfig(ini);

  // AIMBOT
  saveAimBotConfig(ini);

  // GLOBAL
  saveGlobalConfig(ini);
}
