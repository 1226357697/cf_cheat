#include "Render.h"
#include <vector>
#include <map>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "util.h"
#include "Game.h"
#include "menu_config.hpp"
#include "Logger.h"
#include "HealthBar.h"

#define DEG_TO_RAD(deg) ((deg) * (M_PI / 180.0))
#define RAD_TO_DEG(rad) ((rad) * (180.0 / M_PI))


inline double DegreesToRadians(double degrees) 
{
	return DEG_TO_RAD(degrees);
}

inline double RadiansToDegrees(double radians)
{
	return RAD_TO_DEG(radians);
}

static const std::vector<int>  BoneIndexRightLeg{ foot_r , shank_r , hip_r ,hip };
static const std::vector<int>  BoneIndexLeftLeg{ foot_l , shank_l , hip_l, hip };
static const std::vector<int>  BoneIndexSpine{ hip , spine };
static const std::vector<int>  BoneIndexRightArm{ hand_r , elbow_r, shoulder_r, spine };
static const std::vector<int>  BoneIndexLeftArm{ hand_l , elbow_l, shoulder_l, spine };
static const std::vector<int>  BoneIndexHead{ spine , head };
const float boneThickness = 1.8;
const float boxThickness = 1.8;
const float LineThickness= 1.8;
const float HealtWidthSacle = 0.2;
const float HealtMaxWidth = 7.0f;
const float HealtMinWidth = 4.0f;

static int s_render_frame = 0;
static Vec2 s_game_window_size;

void DrawBonePart(std::array<D3DXVECTOR2, BoneCount>& BonePos, const std::vector<int>& BonePartIndex)
{
	assert(BonePartIndex.size() % 2 == 0);
	D3DXVECTOR2 prevPosint;
	bool bInit = false;
	for (size_t i = 0; i < BonePartIndex.size(); i++)
	{
		D3DXVECTOR2 pos = BonePos[BonePartIndex[i]];

		if (pos == invalidScreenPos)
			continue;

		if (!bInit)
		{
			prevPosint = pos;
			bInit = true;
			continue;
		}

		Gui.Line(
			Vec2(prevPosint.x, prevPosint.y),
			Vec2(pos.x, pos.y),
			MenuConfig::BoneColor,
			boneThickness
		);

		prevPosint = pos;
	}
}

bool Render::CalcPlayerBoneRect(const std::array<D3DXVECTOR2, BoneCount>& screenBonePos, std::pair<Vec2, Vec2>& Rect)
{
	Vec2 inValidPosint = Vec2{ 0, 0 };
	float head_circlerad = game_.GetDistance2D(screenBonePos[neck], screenBonePos[head]);
	Vec2 leftTop, rightBottom, boxSize;
	leftTop, rightBottom = inValidPosint;
	bool bInit = false;
	for (int j = 0; j < screenBonePos.size(); ++j)
	{
		const D3DXVECTOR2& pos = screenBonePos[j];
		if (pos == invalidScreenPos)
			continue;

		if (!bInit)
		{
			leftTop = rightBottom = Vec2{ pos.x, pos.y };
			bInit = true;
			continue;
		}

		leftTop.x = min(pos.x, leftTop.x);
		leftTop.y = min(pos.y, leftTop.y);
		rightBottom.x = max(pos.x, rightBottom.x);
		rightBottom.y = max(pos.y, rightBottom.y);
	}
	if (leftTop == inValidPosint && rightBottom == inValidPosint)
		return false;

	leftTop.y -= head_circlerad * 2;
	boxSize.x = rightBottom.x - leftTop.x;
	boxSize.y = rightBottom.y - leftTop.y;

	Rect = std::make_pair(leftTop, boxSize);
	return true;
}

void Render::DrawHealthBar(std::ptrdiff_t Sign, float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, bool Horizontal)
{
	static std::map<std::ptrdiff_t, HealthBar> HealthBarMap;
	if (!HealthBarMap.count(Sign))
	{
		HealthBarMap.insert({ Sign,HealthBar() });
	}
	if (HealthBarMap.count(Sign))
	{
		if (Horizontal)
			HealthBarMap[Sign].DrawHealthBar_Horizontal(MaxHealth, CurrentHealth, Pos, Size);
		else
			HealthBarMap[Sign].DrawHealthBar_Vertical(MaxHealth, CurrentHealth, Pos, Size);
	}
}

void Render::RadarSetting(Base_Radar& Radar)
{
	Radar.SetPos({85,85});
	Radar.SetProportion(2000);
	Radar.SetRange(MenuConfig::RadarRange);
	Radar.SetSize(65*2);
	Radar.SetCrossColor(MenuConfig::CrossLineColor);
	Radar.ShowCrossLine = MenuConfig::ShowCrossLine;
	Radar.Opened = true;
}

void CalculateEndPoint(const D3DXVECTOR3& startPoint, float yaw, float pitch, float length, D3DXVECTOR3& endPoint) 
{

	// 计算方向向量
	float cosYaw = cosf(yaw);
	float sinYaw = sinf(yaw);
	float cosPitch = cosf(pitch);
	float sinPitch = sinf(pitch);

	// 计算视线的终点位置
	endPoint.x = startPoint.x + length * cosPitch * cosYaw;
	endPoint.y = startPoint.y + length * sinPitch;
	endPoint.z = startPoint.z + length * cosPitch * sinYaw;
}

void Render::PlayerESP()
{
	std::array<D3DXVECTOR3, BoneCount> localPlayerBonePos;
	std::array<D3DXVECTOR3, BoneCount> BonePos;
	std::array<D3DXVECTOR2, BoneCount> boneScreenPos;
	std::array<ImVec2, 15>  BonePosArray;
	ViewAngle localAngle;

	if (MenuConfig::ShowBoneESP || MenuConfig::ShowBoxESP
		|| MenuConfig::ShowPlayerName)
	{
		if ((s_render_frame % 30) == 0)
		{
			auto [w, h] = game_.getWindowSize();
			s_game_window_size = Vec2(w,h);
		}
		s_render_frame++;

		int local_player_index = game_.getLocalPlayerIndex();
		// 有可能刚进入对局，数据有问题
		if (game_.validPlayerIndex(local_player_index))
		{
			std::ptrdiff_t local_player = game_.getPlayer(local_player_index);
			uint8_t local_player_team = game_.getPlayerTeam(local_player);

			// Radar Data
			//Base_Radar Radar;
			//RadarSetting(Radar);
			D3DXVECTOR3 AimPos = invalidWorldPos;
			D3DXVECTOR2 AimPos2d = invalidScreenPos;
			float tmpdis = -1;

			Vec2 gameWindowCentre = s_game_window_size / 2;
			game_.getPlayerAngle(local_player, localAngle);
			game_.getPlayerBones(local_player, localPlayerBonePos);


			for (int i = 0; i < player_count; ++i)
			{
				std::ptrdiff_t player = 0;
				uint8_t player_team = 0;
				int player_hp = 0;
				std::string name;

				if (i == local_player_index)
					continue;

				player = game_.getPlayer(i);
				if (!game_.validPlayer(player))
					continue;

				player_team = game_.getPlayerTeam(player);
				player_hp = game_.getPlayerHP(player);
				name = game_.getPlayerName(player);

				// 队友
				if (local_player_team == player_team && MenuConfig::TeamCheck)
					continue;

				if (player_hp == 0)
					continue;


				ViewAngle playerAngle;

				bool canDrawBone = true;
				game_.getPlayerBones(player, BonePos);
				for (int j = 0; j < BonePos.size(); ++j)
				{
					if (!game_.WorldToScreen(BonePos[j], boneScreenPos[j]))
					{
						boneScreenPos[j] = invalidScreenPos;
						canDrawBone = false;
					}
				}

				game_.getPlayerAngle(player, playerAngle);

				if (boneScreenPos[i] != invalidScreenPos)
				{
					float dis = game_.GetDistance2D(boneScreenPos[i], {gameWindowCentre.x, gameWindowCentre.y });
					if (tmpdis == -1)
					{
						tmpdis = dis;
						AimPos = BonePos[head];
						AimPos2d = boneScreenPos[head];
					}
					else
					{
						if (dis < tmpdis)
						{
							tmpdis = dis;
							AimPos = BonePos[head];
							AimPos2d = boneScreenPos[head];
						}
					}
				}

				// Add entity to radar
			/*	if (MenuConfig::ShowRadar)
				{
					Radar.AddPoint(Vec3{ localPlayerBonePos[head].x,localPlayerBonePos[head].y, localPlayerBonePos[head].z },
						RadiansToDegrees(-(localAngle.yaw + DegreesToRadians(-90))),
						Vec3{ BonePos[head].x,BonePos[head].y, BonePos[head].z },
						ImColor(237, 85, 106, 200),
						MenuConfig::RadarType,
						RadiansToDegrees(-(playerAngle.yaw + DegreesToRadians(-90))));
				}*/



				if(MenuConfig::ShowBoneESP && canDrawBone)
				{
					// draw bone
					if (canDrawBone)
					{
						float head_circlerad = game_.GetDistance2D(boneScreenPos[neck], boneScreenPos[head]);
						D3DXVECTOR2& headPos = boneScreenPos[head];

						DrawBonePart(boneScreenPos, BoneIndexRightLeg);
						DrawBonePart(boneScreenPos, BoneIndexLeftLeg);
						DrawBonePart(boneScreenPos, BoneIndexSpine);
						DrawBonePart(boneScreenPos, BoneIndexRightArm);
						DrawBonePart(boneScreenPos, BoneIndexLeftArm);
						DrawBonePart(boneScreenPos, BoneIndexHead);
						Gui.Circle({ headPos.x, headPos.y - head_circlerad }, head_circlerad, MenuConfig::BoneColor, boneThickness);

						//const D3DXVECTOR2& pos = boneScreenPos[foot_l];
						//Gui.Text(name.c_str(), { pos.x,pos.y }, ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)));
					}
				}

				std::pair<Vec2, Vec2> Rect;
				if(!CalcPlayerBoneRect(boneScreenPos, Rect))
					continue;

				auto [leftTop, boxSize] = Rect;
				if (MenuConfig::ShowBoxESP)
				{
					Gui.Rectangle(leftTop, boxSize, MenuConfig::BoxColor, boxThickness);
				}

				if (MenuConfig::ShowPlayerName)
				{
					Gui.Text(name, {leftTop.x + boxSize.x / 2 ,leftTop.y + boxSize.y }, MenuConfig::BoxColor, 15.0f, true);
				}

				if (MenuConfig::ShowPlayerHP)
				{
					float healthBarWidth = boxSize.x * HealtWidthSacle;
					healthBarWidth = healthBarWidth > HealtMaxWidth ? HealtMaxWidth : healthBarWidth;
					healthBarWidth = healthBarWidth < HealtMinWidth ? HealtMinWidth : healthBarWidth;

					DrawHealthBar(player, 100, player_hp, { leftTop.x - healthBarWidth, leftTop.y}, { healthBarWidth, boxSize.y}, false);
				}

				if (MenuConfig::ShowPlayerDistance)
				{
					float distance = game_.GetDistance3D(localPlayerBonePos[head], BonePos[head]);
					Gui.Text(std::to_string((int)distance /10) + "m", {leftTop.x + boxSize.x / 2 ,leftTop.y + boxSize.y + 15.0f}, MenuConfig::BoxColor, 15.0f, true);

				}

				if (MenuConfig::ShowLineToEnemy)
				{
					auto [w, h] = s_game_window_size;
					Gui.Line({w/2, 0}, { leftTop .x + boxSize .x / 2, leftTop .y }, MenuConfig::BoxColor, LineThickness);
				}

				if (MenuConfig::ShowPlayerAngle)
				{
					ViewAngle angle;
					if (game_.getPlayerAngle(player, angle) && boneScreenPos[head] != invalidScreenPos && BonePos[head] != invalidWorldPos)
					{
						char buffer[100];
						angle.yaw += DegreesToRadians(-90);
						angle.yaw = -angle.yaw;
						angle.pitch = -angle.pitch;
						float Length = 70;
						Vec2 StartPoint;
						D3DXVECTOR2 EndPoint;
						D3DXVECTOR3 Temp;
						StartPoint = Vec2{boneScreenPos[head].x, boneScreenPos[head].y};

						CalculateEndPoint(BonePos[head], angle.yaw, angle.pitch, Length, Temp);
						
						if (game_.WorldToScreen(Temp, EndPoint))
						{
							Gui.Line(StartPoint, { EndPoint.x, EndPoint.y }, MenuConfig::AngleColor, LineThickness);
						}
					}
				}

				if (MenuConfig::ShowC4)
				{
					if (game_.playerHasC4(player))
					{
						Gui.Text(util::CharToUtf8( ">>>携带C4<<<"), {leftTop.x + boxSize.x / 2 ,leftTop.y + boxSize.y + 15.0f *2}, MenuConfig::BoxColor, 15.0f, true);
					}
				}

				if (MenuConfig::ShowRadar)
				{

					float position_x = 83;
					float position_y = 86;

					float radius_x = 64;
					float radius_y = 64;

					D3DXVECTOR3 playerPos = BonePos[head];
					D3DXVECTOR3 localPlayerPos = localPlayerBonePos[head];

					float part1 = pow((localPlayerPos.x - playerPos.x), 2);
					float part2 = pow((localPlayerPos.z - playerPos.z), 2);
					float underRadical = part1 + part2;
					float  result = (float)sqrt(underRadical);
					result /= 64.4;

					float dx = localPlayerPos.x - playerPos.x;
					float dy = localPlayerPos.z - playerPos.z;
					float yaw = atan2f(dy, dx);

					if (result > radius_x)
						result = radius_x;
					float myYaw = localAngle.yaw;
					myYaw *= -1;
					myYaw = yaw - myYaw;

					const float radb = myYaw;

					float x2 = position_x - (cos(radb) * result);
					float y2 = position_y + (sin(radb) * result);
					//D3DRECT back2 = { x2 - 1, y2 - 1, x2 + 2, y2 + 2 };
					Gui.CircleFilled({ x2 , y2 }, 2, ImColor(255, 0, 0, 255));
					//Gui.CircleFilled({ position_x , position_y }, 2, MenuConfig::BoneColor);
					std::pair<Vec2, Vec2> Line1;
					std::pair<Vec2, Vec2> Line2;
					Line1.first = Vec2{ position_x- radius_x,position_y };
					Line1.second = Vec2{ position_x+ radius_x,position_y };
					Line2.first = Vec2{ position_x,position_y - radius_y };
					Line2.second = Vec2{position_x,position_y + radius_y };

					Gui.Line(Line1.first, Line1.second, ImColor(255, 255, 255, 255), 1);
					Gui.Line(Line2.first, Line2.second, ImColor(255, 255, 255, 255), 1);

				}
			
			}

			if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && AimPos != invalidWorldPos)
			{
				ViewAngle angle = {0,0};

				D3DXVECTOR3 playerPos = localPlayerBonePos[head];
				D3DXVECTOR3 fDist = AimPos - playerPos;
				angle.pitch = (float)atan2(-fDist.y, sqrt(fDist.x * fDist.x + fDist.z * fDist.z));
				angle.yaw = (float)atan2(fDist.x, fDist.z);
				//Logger::info("pitch %f yaw %f", angle.pitch, angle.yaw);
				game_.setLocalPlayerAngle(local_player, angle);
				Gui.Text("+", { AimPos2d .x, AimPos2d .y}, ImColor(255, 0, 0, 255));
			}
			/*if (MenuConfig::ShowRadar)
			{
				Radar.Render();
			}*/
		}
	}

}

void DrawMenu()
{
	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	{
		// esp menu
		if (ImGui::CollapsingHeader("ESP"))
		{
			Gui.MyCheckBox("BoxESP", &MenuConfig::ShowBoxESP);
			ImGui::SameLine();
			ImGui::ColorEdit4("##BoxColor", reinterpret_cast<float*>(&MenuConfig::BoxColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox("BoneESP", &MenuConfig::ShowBoneESP);
			ImGui::SameLine();
			ImGui::ColorEdit4("##BoneColor", reinterpret_cast<float*>(&MenuConfig::BoneColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox("PlayerName", &MenuConfig::ShowPlayerName);
			Gui.MyCheckBox("PlayerHP", &MenuConfig::ShowPlayerHP);
			Gui.MyCheckBox("PlayerDistance", &MenuConfig::ShowPlayerDistance);
			Gui.MyCheckBox("TeamCheck", &MenuConfig::TeamCheck);

			Gui.MyCheckBox("LineToEnemy", &MenuConfig::ShowLineToEnemy);
			ImGui::SameLine();
			ImGui::ColorEdit4("##LineToEnemyColor", reinterpret_cast<float*>(&MenuConfig::LineToEnemyColor), ImGuiColorEditFlags_NoInputs);


			Gui.MyCheckBox("Show C4", &MenuConfig::ShowC4);

			Gui.MyCheckBox("ViewAngle", &MenuConfig::ShowPlayerAngle);
			ImGui::SameLine();
			ImGui::ColorEdit4("##ViewAngle", reinterpret_cast<float*>(&MenuConfig::AngleColor), ImGuiColorEditFlags_NoInputs);


			Gui.MyCheckBox("Show Radar", &MenuConfig::ShowRadar);
			

		}
		ImGui::Text("[HOME] HideMenu");
	}
	ImGui::End();
}




Render::Render(Game& game)
	:game_(game)
{
}

void Render::run()
{
	static ULONGLONG lastTick = 0;
	ULONGLONG currentTick = GetTickCount64();
	if ((GetAsyncKeyState(VK_HOME) & 0x8000) && currentTick - lastTick >= 150) {
		// Check key state per 150ms once to avoid loop
		MenuConfig::ShowMenu = !MenuConfig::ShowMenu;
		lastTick = currentTick;
	}
	if (MenuConfig::ShowMenu)
		DrawMenu();

	PlayerESP();
}
