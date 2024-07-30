#pragma once
#include <Windows.h>
#include "OS-ImGui/OS-ImGui.h"
namespace MenuConfig
{
	enum ShowHPWay:int
	{
		kHP_WAY_NONE = 0,
		kHP_WAY_LIFEBAR ,
		kHP_WAY_LIFEVALUE,
		kHP_WAY_LIFEBAR_AND_VALUE
	};

	enum AimBotPos :int
	{
		kAIM_BOT_POS_HEAD = 0,
		kAIM_BOT_POS_NECK,
		kAIM_BOT_POS_SPINE,
	};

	// ESP
	inline bool ShowBoneESP = false;
	inline bool ShowBoxESP = false;
	inline bool ShowHealthBar = false;
	inline bool ShowEyeRay = false;
	inline bool ShowPlayerName = false;
	inline ShowHPWay ShowPlayerHP = kHP_WAY_NONE;
	inline bool ShowPlayerDistance = false;
	inline bool ShowC4 = false;
	inline bool ShowRadar = true;
	inline bool ShowLineToEnemy = false;
	inline int RadarType = 0;
	inline bool ShowWeaponESP = false;

	inline ImColor BoneColor = ImColor(255, 255, 255, 255);
	inline ImColor BoxColor = ImColor(255, 255, 255, 255);
	inline ImColor AngleColor = ImColor(255, 255, 255, 255);
	inline ImColor LineToEnemyColor = ImColor(255, 255, 255, 220);
	inline ImColor EyeRayColor = ImColor(255, 0, 0, 255);

	// AIMBOT
	inline bool AimBot = false;
	inline int AimBotHotKey = 2;
	inline AimBotPos  AimPosition = kAIM_BOT_POS_HEAD;
	inline int  AimPositionIndex = 0;
	inline bool ShowAimRangle = true;
	inline float AimRangle = 10;
	inline float AimSmooth = 0.75;

	// Global
	inline bool TeamCheck = true;
	inline bool ShowMenu = true;

	// KNIFE
	inline bool  knife_hack = false; //0
	inline float knife_data_marks = 300; //0
	inline float knife_data_tap_distance = 340;// 325
	inline float knife_data_tap_range = 1; // 0
	inline float knife_data_attack_distance = 345; // 350
	inline float knife_data_attack_range = 1; // 0
	inline float knife_data_attack_speed = 1; //1.3
	inline float knife_data_secondary_distance = 345; //0
	inline float knife_data_secondary_range = 1; // 0
	inline float knife_data_movement_speed = 0;

	void load();
	void save();
}
