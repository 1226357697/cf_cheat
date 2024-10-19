#pragma once

#include "Game.h"
#include "OS-ImGui/OS-ImGui.h"

struct  MenuConfig_
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
	bool ShowBoneESP = false;
	bool ShowBoxESP = false;
	bool ShowHealthBar = false;
	bool ShowEyeRay = false;
	bool ShowPlayerName = false;
	ShowHPWay ShowPlayerHP = kHP_WAY_NONE;
	bool ShowPlayerDistance = false;
	bool ShowC4 = false;
	bool ShowRadar = true;
	bool ShowLineToEnemy = false;
	int RadarType = 0;
	bool ShowWeaponESP = false;

	ImColor BoneColor = ImColor(255, 255, 255, 255);
	ImColor BoxColor = ImColor(255, 255, 255, 255);
	ImColor AngleColor = ImColor(255, 255, 255, 255);
	ImColor LineToEnemyColor = ImColor(255, 255, 255, 220);
	ImColor EyeRayColor = ImColor(255, 0, 0, 255);

	// AIMBOT
	bool AimBot = false;
	AimBotType AimType = kAIM_BOT_TYPE_MEMORY;
	int AimBotHotKey = 2;
	AimBotPos  AimPosition = kAIM_BOT_POS_HEAD;
	int  AimPositionIndex = head;
	bool ShowAimRangle = true;
	float AimRangle = 10;
	float AimSmooth = 0.75;
	AimBotPolicy AimPolicy = kAIM_BOT_POLICY_CLOSEST_TARGET;

	// PID≤Œ ˝
	float Aim_PID_p = 0.5f;
	float Aim_PID_i = 0.1f;
	float Aim_PID_d = 0.5f;
	// ¡È√Ù∂» 30

	// Global
	bool TeamCheck = true;
	bool ShowMenu = true;

	// KNIFE
	bool  knife_hack = false; //0
	float knife_data_marks = 300; //0
	float knife_data_tap_distance = 340;// 325
	float knife_data_tap_range = 1; // 0
	float knife_data_attack_distance = 345; // 350
	float knife_data_attack_range = 1; // 0
	float knife_data_attack_speed = 1; //1.3
	float knife_data_secondary_distance = 345; //0
	float knife_data_secondary_range = 1; // 0
	float knife_data_movement_speed = 0;

	static void load();
	static void save();
};

extern MenuConfig_  MenuConfig;
