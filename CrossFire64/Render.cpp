#include "Render.h"
#include <vector>
#include <assert.h>

#include "Game.h"
#include "menu_config.hpp"
#include "OS-ImGui/OS-ImGui.h"
#include "Logger.h"


static const std::vector<int>  BoneIndexRightLeg{ foot_r , shank_r , hip_r ,hip };
static const std::vector<int>  BoneIndexLeftLeg{ foot_l , shank_l , hip_l, hip };
static const std::vector<int>  BoneIndexSpine{ hip , spine };
static const std::vector<int>  BoneIndexRightArm{ hand_r , elbow_r, shoulder_r, spine };
static const std::vector<int>  BoneIndexLeftArm{ hand_l , elbow_l, shoulder_l, spine };
static const std::vector<int>  BoneIndexHead{ spine , head };
const float boneThickness = 2.1;
const float boxThickness= 2.1;

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
			ImVec2(prevPosint.x, prevPosint.y),
			ImVec2(pos.x, pos.y),
			MenuConfig::BoneColor,
			boneThickness
		);

		prevPosint = pos;
	}
}

void Render::PlayerESP()
{
	std::array<D3DXVECTOR3, BoneCount> BonePos;
	std::array<D3DXVECTOR2, BoneCount> boneScreenPos;
	std::array<ImVec2, 15>  BonePosArray;
	if (MenuConfig::ShowBoneESP)
	{
		int local_player_index = game_.getLocalPlayerIndex();
		// 有可能刚进入对局，数据有问题
		if (game_.validPlayerIndex(local_player_index))
		{
			std::ptrdiff_t local_player = game_.getPlayer(local_player_index);
			uint8_t local_player_team = game_.getPlayerTeam(local_player);
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
				if (local_player_team == player_team)
					continue;

				if (player_hp == 0)
					continue;

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

				int head_circlerad = game_.GetDistance2D(boneScreenPos[neck], boneScreenPos[head]);
				// draw bone
				if (canDrawBone)
				{
					D3DXVECTOR2& headPos = boneScreenPos[head];

					DrawBonePart(boneScreenPos, BoneIndexRightLeg);
					DrawBonePart(boneScreenPos, BoneIndexLeftLeg);
					DrawBonePart(boneScreenPos, BoneIndexSpine);
					DrawBonePart(boneScreenPos, BoneIndexRightArm);
					DrawBonePart(boneScreenPos, BoneIndexLeftArm);
					DrawBonePart(boneScreenPos, BoneIndexHead);
					Gui.Circle({ headPos.x, headPos.y - head_circlerad }, head_circlerad, MenuConfig::BoneColor, boneThickness);

					const D3DXVECTOR2& pos = boneScreenPos[foot_l];
					Gui.Text(name.c_str(), { pos.x,pos.y }, ImGui::GetColorU32(IM_COL32(255, 0, 0, 255)));
				}

				{
					Vec2 leftTop, rightBottom, boxSize;
					leftTop, rightBottom = Vec2{ 0, 0 };
					bool bInit = false;
					for (int j = 0; j < BonePos.size(); ++j)
					{
						D3DXVECTOR2& pos = boneScreenPos[j];
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
					leftTop.y -= head_circlerad *2;
					boxSize.x = rightBottom.x - leftTop.x;
					boxSize.y = rightBottom.y - leftTop.y;
					Gui.Rectangle(leftTop, boxSize, MenuConfig::BoxColor, boxThickness);

				}
				

			}
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

			ImGui::Combo("BoxType", &MenuConfig::BoxType, "Normal\0Dynamic");

			Gui.MyCheckBox("BoneESP", &MenuConfig::ShowBoneESP);
			ImGui::SameLine();
			ImGui::ColorEdit4("##BoneColor", reinterpret_cast<float*>(&MenuConfig::BoneColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox("PlayerName", &MenuConfig::ShowPlayerName);
			Gui.MyCheckBox("TeamCheck", &MenuConfig::TeamCheck);

			Gui.MyCheckBox("LineToEnemy", &MenuConfig::ShowLineToEnemy);
			ImGui::SameLine();
			ImGui::ColorEdit4("##LineToEnemyColor", reinterpret_cast<float*>(&MenuConfig::LineToEnemyColor), ImGuiColorEditFlags_NoInputs);

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
	static DWORD lastTick = 0;
	DWORD currentTick = GetTickCount();
	if ((GetAsyncKeyState(VK_HOME) & 0x8000) && currentTick - lastTick >= 150) {
		// Check key state per 150ms once to avoid loop
		MenuConfig::ShowMenu = !MenuConfig::ShowMenu;
		lastTick = currentTick;
	}
	if (MenuConfig::ShowMenu)
		DrawMenu();

	PlayerESP();
}
