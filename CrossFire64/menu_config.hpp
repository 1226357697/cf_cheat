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

	inline bool ShowMenu = true;

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
	inline bool AimBot = true;
	inline int AimBotHotKey = VK_RBUTTON;
	// 0: head 1: neck 3: spine
	inline AimBotPos  AimPosition = kAIM_BOT_POS_HEAD;
	inline int  AimPositionIndex = 0;

	inline bool TeamCheck = true;
}
