#include "menu_config.hpp"
#include "IniFile.h"
#include <shlobj.h>
#include <filesystem>
#include <fstream>
#include "xorstr.hpp"

namespace fs = std::filesystem;

static fs::path get_config_path()
{
	fs::path path;
	const char* config_name = xorstr_("crossfire.ini");
	char path_buffer[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path_buffer)))
	{
		path = path_buffer;
		path = path/ xorstr_("cf_hack");
		if (!fs::exists(path)) 
			fs::create_directories(path);
		path = path / config_name;
	}
	else
	{
		strcpy(path_buffer, xorstr_("./"));
		strcat(path_buffer, config_name);
	}

	return path;
}


static void loadESPConfig(IniFile& ini)
{
	MenuConfig::ShowBoneESP = ini.readBool(xorstr_("ESP"), xorstr_("ShowBoneESP"), MenuConfig::ShowBoneESP);
	MenuConfig::ShowBoxESP = ini.readBool(xorstr_("ESP"), xorstr_("ShowBoxESP"), MenuConfig::ShowBoxESP);
	MenuConfig::ShowHealthBar = ini.readBool(xorstr_("ESP"), xorstr_("ShowHealthBar"), MenuConfig::ShowHealthBar);
	MenuConfig::ShowEyeRay = ini.readBool(xorstr_("ESP"), xorstr_("ShowEyeRay"), MenuConfig::ShowEyeRay);
	MenuConfig::ShowPlayerName = ini.readBool(xorstr_("ESP"), xorstr_("ShowPlayerName"), MenuConfig::ShowPlayerName);
	MenuConfig::ShowPlayerHP = (MenuConfig::ShowHPWay)ini.readInt(xorstr_("ESP"), xorstr_("ShowPlayerHP"), MenuConfig::ShowPlayerHP);
	MenuConfig::ShowPlayerDistance = ini.readBool(xorstr_("ESP"), xorstr_("ShowPlayerDistance"), MenuConfig::ShowPlayerDistance);
	MenuConfig::ShowC4 = ini.readBool(xorstr_("ESP"), xorstr_("ShowC4"), MenuConfig::ShowC4);
	MenuConfig::ShowRadar = ini.readBool(xorstr_("ESP"), xorstr_("ShowRadar"), MenuConfig::ShowRadar);
	MenuConfig::ShowLineToEnemy = ini.readBool(xorstr_("ESP"), xorstr_("ShowLineToEnemy"), MenuConfig::ShowLineToEnemy);
	MenuConfig::RadarType =  ini.readInt(xorstr_("ESP"), xorstr_("RadarType"), MenuConfig::RadarType);
	MenuConfig::ShowWeaponESP = ini.readBool(xorstr_("ESP"), xorstr_("ShowWeaponESP"), MenuConfig::ShowWeaponESP);

	MenuConfig::BoneColor = ini.readInt(xorstr_("ESP"), xorstr_("BoneColor"), MenuConfig::BoneColor);
	MenuConfig::BoxColor = ini.readInt(xorstr_("ESP"), xorstr_("BoxColor"), MenuConfig::BoxColor);
	MenuConfig::AngleColor = ini.readInt(xorstr_("ESP"), xorstr_("AngleColor"), MenuConfig::AngleColor);
	MenuConfig::LineToEnemyColor = ini.readInt(xorstr_("ESP"), xorstr_("LineToEnemyColor"), MenuConfig::LineToEnemyColor);
	MenuConfig::EyeRayColor = ini.readInt(xorstr_("ESP"), xorstr_("EyeRayColor"), MenuConfig::EyeRayColor);

}

static void loadAimBotConfig(IniFile& ini)
{
	MenuConfig::AimBot = ini.readBool(xorstr_("AimBot"), xorstr_("AimBot"), MenuConfig::AimBot);
	MenuConfig::AimType = (MenuConfig::AimBotType)ini.readInt(xorstr_("AimBot"), xorstr_("AimType"), MenuConfig::AimType);
	MenuConfig::AimBotHotKey = ini.readInt(xorstr_("AimBot"), xorstr_("AimBotHotKey"), MenuConfig::AimBotHotKey);
	MenuConfig::AimPosition = (MenuConfig::AimBotPos)ini.readInt(xorstr_("AimBot"), xorstr_("AimPosition"), MenuConfig::AimPosition);
	MenuConfig::AimPositionIndex = ini.readInt(xorstr_("AimBot"), xorstr_("AimPositionIndex"), MenuConfig::AimPositionIndex);
	MenuConfig::ShowAimRangle = ini.readBool(xorstr_("AimBot"), xorstr_("ShowAimRangle"), MenuConfig::ShowAimRangle);
	MenuConfig::AimRangle = ini.readFloat(xorstr_("AimBot"), xorstr_("AimRangle"), MenuConfig::AimRangle);
	MenuConfig::AimSmooth = ini.readFloat(xorstr_("AimBot"), xorstr_("AimSmooth"), MenuConfig::AimSmooth);

	// aim pid
	MenuConfig::Aim_PID_p = ini.readFloat(xorstr_("AimBot"), xorstr_("Aim_PID_p"), MenuConfig::Aim_PID_p);
	MenuConfig::Aim_PID_i = ini.readFloat(xorstr_("AimBot"), xorstr_("Aim_PID_i"), MenuConfig::Aim_PID_i);
	MenuConfig::Aim_PID_d = ini.readFloat(xorstr_("AimBot"), xorstr_("Aim_PID_d"), MenuConfig::Aim_PID_d);
}

static void loadGlobalConfig(IniFile& ini)
{
	MenuConfig::TeamCheck = ini.readBool(xorstr_("Global"), xorstr_("TeamCheck"), MenuConfig::TeamCheck);
	MenuConfig::ShowMenu = ini.readBool(xorstr_("Global"), xorstr_("ShowMenu"), MenuConfig::ShowMenu);
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
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowBoneESP"), MenuConfig::ShowBoneESP);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowBoxESP"), MenuConfig::ShowBoxESP);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowHealthBar"), MenuConfig::ShowHealthBar);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowEyeRay"), MenuConfig::ShowEyeRay);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowPlayerName"), MenuConfig::ShowPlayerName);
	 ini.writeInt(xorstr_("ESP"), xorstr_("ShowPlayerHP"), (int)MenuConfig::ShowPlayerHP);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowPlayerDistance"), MenuConfig::ShowPlayerDistance);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowC4"), MenuConfig::ShowC4);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowRadar"), MenuConfig::ShowRadar);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowLineToEnemy"), MenuConfig::ShowLineToEnemy);
	 ini.writeInt(xorstr_("ESP"), xorstr_("RadarType"), MenuConfig::RadarType);
	 ini.writeBool(xorstr_("ESP"), xorstr_("ShowWeaponESP"), MenuConfig::ShowWeaponESP);

	 ini.writeInt(xorstr_("ESP"), xorstr_("BoneColor"), MenuConfig::BoneColor);
	 ini.writeInt(xorstr_("ESP"), xorstr_("BoxColor"), MenuConfig::BoxColor);
	 ini.writeInt(xorstr_("ESP"), xorstr_("AngleColor"), MenuConfig::AngleColor);
	 ini.writeInt(xorstr_("ESP"), xorstr_("LineToEnemyColor"), MenuConfig::LineToEnemyColor);
	 ini.writeInt(xorstr_("ESP"), xorstr_("EyeRayColor"), MenuConfig::EyeRayColor);
}

static void saveAimBotConfig(IniFile& ini)
{
	 ini.writeBool(xorstr_("AimBot"), xorstr_("AimBot"), MenuConfig::AimBot);
	 ini.writeInt(xorstr_("AimBot"), xorstr_("AimType"), MenuConfig::AimType);
	 ini.writeInt(xorstr_("AimBot"), xorstr_("AimBotHotKey"), MenuConfig::AimBotHotKey);
	 ini.writeInt(xorstr_("AimBot"), xorstr_("AimPosition"), MenuConfig::AimPosition);
	 ini.writeInt(xorstr_("AimBot"), xorstr_("AimPositionIndex"), MenuConfig::AimPositionIndex);
	 ini.writeBool(xorstr_("AimBot"), xorstr_("ShowAimRangle"), MenuConfig::ShowAimRangle);
	 ini.writeFloat(xorstr_("AimBot"), xorstr_("AimRangle"), MenuConfig::AimRangle);
	 ini.writeFloat(xorstr_("AimBot"), xorstr_("AimSmooth"), MenuConfig::AimSmooth);

	 // aim pid
	 ini.writeFloat(xorstr_("AimBot"), xorstr_("Aim_PID_p"), MenuConfig::Aim_PID_p);
	 ini.writeFloat(xorstr_("AimBot"), xorstr_("Aim_PID_i"), MenuConfig::Aim_PID_i);
	 ini.writeFloat(xorstr_("AimBot"), xorstr_("Aim_PID_d"), MenuConfig::Aim_PID_d);
}

static void saveGlobalConfig(IniFile& ini)
{
	ini.writeBool(xorstr_("Global"), xorstr_("TeamCheck"), MenuConfig::TeamCheck);
	ini.writeBool(xorstr_("Global"), xorstr_("ShowMenu"), MenuConfig::ShowMenu);
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
