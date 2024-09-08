#pragma once
#include <Windows.h>
#include "OS-ImGui/OS-ImGui.h"
#include "Game.h"

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

	enum AimBotType:int
	{
		kAIM_BOT_TYPE_MEMORY = 0,
		kAIM_BOT_TYPE_LOGITECH_DRIVER,
		kAIM_BOT_TYPE_KMBOX,
	};

	enum AimBotPolicy:int
	{
		kAIM_BOT_POLICY_CLOSEST_TARGET = 0,
		kAIM_BOT_POLICY_CLOSEST_CROSSHAIR,
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
	inline AimBotType AimType = kAIM_BOT_TYPE_MEMORY;
	inline int AimBotHotKey = 2;
	inline AimBotPos  AimPosition = kAIM_BOT_POS_HEAD;
	inline int  AimPositionIndex = head;
	inline bool ShowAimRangle = true;
	inline float AimRangle = 10;
	inline float AimSmooth = 0.75;
	inline AimBotPolicy AimPolicy = kAIM_BOT_POLICY_CLOSEST_TARGET;

	// PID≤Œ ˝
	inline float Aim_PID_p = 0.5f;
	inline float Aim_PID_i = 0.1f;
	inline float Aim_PID_d = 0.5f;
	// ¡È√Ù∂» 30

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

	// kmbox
	inline char kmbox_ip[256];
	inline char kmbox_port[128];
	inline char kmbox_uuid[128];

	void load();
	void save();
}
