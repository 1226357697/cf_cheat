#pragma once
#include <Windows.h>
#include "OS-ImGui/OS-ImGui.h"
namespace MenuConfig
{
	inline bool ShowBoneESP = false;
	inline bool ShowBoxESP = false;
	inline bool ShowHealthBar = false;
	//inline bool ShowWeaponESP = true;
	//inline bool ShowEyeRay = true;
	inline bool ShowPlayerName = false;
	inline bool ShowPlayerHP = false;
	inline bool ShowPlayerDistance = false;
	inline bool ShowPlayerAngle = false;
	inline bool ShowC4 = false;
	inline bool TeamCheck = false;

	inline bool AimBot = true;
	inline int AimBotHotKey = 0;
	// 0: head 1: neck 3: spine
	inline int  AimPosition = 0;
	inline DWORD  AimPositionIndex = 0;// BONEINDEX::head;

	inline ImColor BoneColor = ImColor(255, 255, 255, 255);
	inline ImColor BoxColor = ImColor(255, 255, 255, 255);
	inline ImColor AngleColor = ImColor(255, 255, 255, 255);
	inline ImColor EyeRayColor = ImColor(255, 0, 0, 255);

	inline bool ShowMenu = true;

	inline bool ShowRadar = true;
	inline float RadarRange = 150;
	inline bool ShowCrossLine = true;
	inline ImColor CrossLineColor = ImColor(34, 34, 34, 180);
	// 0: circle 1: arrow 2: circle with arrow
	inline int RadarType = 2;
	inline float Proportion = 2300;

	inline bool TriggerBot = true;
	inline int TriggerHotKey = 0;

	inline bool ShowLineToEnemy = false;
	inline ImColor LineToEnemyColor = ImColor(255, 255, 255, 220);
}
