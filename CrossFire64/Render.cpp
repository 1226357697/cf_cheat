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
#include "VMProtectSDK.h"

#define DEG_TO_RAD(deg) ((deg) * (M_PI / 180.0))
#define RAD_TO_DEG(rad) ((rad) * (180.0 / M_PI))

static const std::vector<int>  BoneIndexRightLeg{ foot_r , shank_r , hip_r ,hip };
static const std::vector<int>  BoneIndexLeftLeg{ foot_l , shank_l , hip_l, hip };
static const std::vector<int>  BoneIndexSpine{ hip , spine };
static const std::vector<int>  BoneIndexRightArm{ hand_r , elbow_r, shoulder_r, spine };
static const std::vector<int>  BoneIndexLeftArm{ hand_l , elbow_l, shoulder_l, spine };
static const std::vector<int>  BoneIndexHead{ spine , head };
const float boneThickness = 1.8f;
const float boxThickness = 1.8f;
const float LineThickness= 1.8f;
const float HealtWidthSacle = 0.2f;
const float HealtMaxWidth = 7.0f;
const float HealtMinWidth = 4.0f;
const float textSize = 15.0f;



inline double DegreesToRadians(double degrees)
{
	return DEG_TO_RAD(degrees);
}

inline double RadiansToDegrees(double radians)
{
	return RAD_TO_DEG(radians);
}

inline D3DXVECTOR2 IM2DX2d(const ImVec2 & vec2)
{
	return D3DXVECTOR2{ vec2 .x, vec2 .y};
}

inline ImVec2 DX2IM2d(const D3DXVECTOR2& vec2)
{
	return ImVec2{ vec2.x, vec2.y };
}

bool Render::calcPlayerBox(const APawn& pawn, Rect& rt)
{
	Vec2 inValidPosint = Vec2{ 0, 0 };
	float head_circlerad = 0;
	Vec2 leftTop, rightBottom, boxSize;
	leftTop, rightBottom = inValidPosint;
	bool bInit = false;
	const PlayerBoneData& neckBone = pawn.Bone[neck];
	const PlayerBoneData& headBone = pawn.Bone[head];

	if(neckBone.inScreen() && headBone.inScreen())
		head_circlerad = game_.GetDistance2D(neckBone.screenPos, headBone.screenPos);

	for (int j = 0; j < pawn.Bone.size(); ++j)
	{
		const D3DXVECTOR2& pos = pawn.Bone[j].screenPos;
		if (pos == invalidScreenPos)
			continue;

		if (!bInit)
		{
			leftTop = rightBottom = DX2IM2d(pos);
			bInit = true;
			continue;
		}

		leftTop.x = min(pos.x, leftTop.x);
		leftTop.y = min(pos.y, leftTop.y);
		rightBottom.x = max(pos.x, rightBottom.x);
		rightBottom.y = max(pos.y, rightBottom.y);
	}
	if (!bInit)
		return false;

	leftTop.y -= head_circlerad * 2;
	boxSize.x = rightBottom.x - leftTop.x;
	boxSize.y = rightBottom.y - leftTop.y;

	rt.x = leftTop.x;
	rt.y = leftTop.y;
	rt.w = boxSize.x;
	rt.h = boxSize.y;

	if(isinf(rt.x)|| isinf(rt.y) || isinf(rt.w)|| isinf(rt.h))
		return false;
	return true;
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
			leftTop = rightBottom = DX2IM2d(pos);
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

void CalculateEndPoint(const D3DXVECTOR3& startPoint, float yaw, float pitch, float length, D3DXVECTOR3& endPoint) 
{
	float cosYaw = cosf(yaw);
	float sinYaw = sinf(yaw);
	float cosPitch = cosf(pitch);
	float sinPitch = sinf(pitch);

	endPoint.x = startPoint.x + length * cosPitch * cosYaw;
	endPoint.y = startPoint.y + length * sinPitch;
	endPoint.z = startPoint.z + length * cosPitch * sinYaw;
}

void Render::resetFrameContext()
{
	if ((render_frame_count_ % 2) != 0)
		return ;

	if ((render_frame_count_ % 30) == 0)
	{
		auto [w, h] = game_.getWindowSize();
		frame_ctx_.game_window_size = Vec2(w, h);
	}

	frame_ctx_.localPlayerIndex = game_.getLocalPlayerIndex();
	if (game_.validPlayerIndex(frame_ctx_.localPlayerIndex))
	{
		std::ptrdiff_t local_player = game_.getPlayer(frame_ctx_.localPlayerIndex);

		for (int i = 0;i < player_count; ++i)
		{
			APawn& pawn = frame_ctx_.playerArray[i];
			std::array<D3DXVECTOR3, BoneCount> BonePos;
			std::ptrdiff_t player = game_.getPlayer(i);

			pawn.valid = game_.validPlayer(player);
			if (!pawn.valid)
				continue;

			pawn.raw_address = player;
			pawn.index = i;
			pawn.team = game_.getPlayerTeam(player);
			pawn.name = game_.getPlayerName(player);
			pawn.weapon_name = game_.getPlayerWeaponName(player);
			pawn.hp = game_.getPlayerHP(player);
			pawn.has_c4 = game_.playerHasC4(player);
			game_.getPlayerAngle(player, pawn.viewAngle);

			// ¹Ç÷À
			game_.getPlayerBones(player, BonePos);
			for (int j = 0; j < BonePos.size(); ++j)
			{
				PlayerBoneData& bone = pawn.Bone[j];
				bone.worldPos = BonePos[j];
				if (!game_.WorldToScreen(bone.worldPos, bone.screenPos))
					bone.screenPos = invalidScreenPos;
			}

			// ·½¿ò
			Rect rt;
			bool ret = calcPlayerBox(pawn, rt);
			pawn.box = std::make_pair(rt, ret);

		}
	}
}

void Render::addRanderRectText(const Rect& rect, int index, const std::string& txt)
{
	Gui.Text(txt, { rect.x + rect.w / 2 ,rect.y + rect.h + (index * textSize) }, MenuConfig::BoxColor, textSize, true);
}

static void DrawBonePart(const std::array<PlayerBoneData, BoneCount>& Bone, const std::vector<int>& BonePartIndex)
{
	assert(BonePartIndex.size() % 2 == 0);

	D3DXVECTOR2 prevPosint;
	bool bInit = false;
	for (size_t i = 0; i < BonePartIndex.size(); ++i)
	{
		const PlayerBoneData& pos = Bone[BonePartIndex[i]];

		if (!pos.inScreen())
		{
			bInit = false;
			continue;
		}

		if (!bInit)
		{
			prevPosint = pos.screenPos;
			bInit = true;
			continue;
		}

		Gui.Line(
			DX2IM2d(prevPosint),
			DX2IM2d(pos.screenPos),
			MenuConfig::BoneColor,
			boneThickness
		);

		prevPosint = pos.screenPos;
	}
}

static ViewAngle adjustViewAngle(ViewAngle angle)
{
	angle.yaw += DegreesToRadians(-90);
	angle.yaw = -angle.yaw;
	angle.pitch = -angle.pitch;
	return angle;
}

void Render::drawPayerBone(const APawn& pawn)
{	
	const std::array<PlayerBoneData, BoneCount>& Bone = pawn.Bone;
	DrawBonePart(Bone, BoneIndexRightLeg);
	DrawBonePart(Bone, BoneIndexLeftLeg);
	DrawBonePart(Bone, BoneIndexSpine);
	DrawBonePart(Bone, BoneIndexRightArm);
	DrawBonePart(Bone, BoneIndexLeftArm);
	DrawBonePart(Bone, BoneIndexHead);

	const PlayerBoneData& neckBone = Bone[neck];
	const PlayerBoneData& headBone = Bone[head];
	if (neckBone.inScreen() && headBone.inScreen())
	{
		float head_circlerad = game_.GetDistance2D(neckBone.screenPos, headBone.screenPos);
		Gui.Circle({ headBone.screenPos.x, headBone.screenPos.y - head_circlerad }, head_circlerad, MenuConfig::BoneColor, boneThickness);
	}
}

void Render::updateFPS()
{
	//static double elapsedTime = 0.0;
	//static std::chrono::high_resolution_clock::time_point lastTime(std::chrono::high_resolution_clock::now());

	//auto currentTime = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<double> deltaTime = currentTime - lastTime;
	//lastTime = currentTime;
	//elapsedTime += deltaTime.count();

	//if (elapsedTime >= 1.0) {
	//	fps_ = render_frame_count_ / elapsedTime;
	//	//render_frame_count_ = 0;
	//	//elapsedTime = 0.0;
	//}
	render_frame_count_++;
}

void Render::RadarSetting(Radar& Radar)
{
	const float position_x = 83;
	const float position_y = 86;
	const float radius_x = 64;
	const float radius_y = 64;

	Radar.SetPos({ position_x, position_y });
	Radar.SetProportion(64.4f);
	Radar.SetRange(64.0f);
	Radar.SetSize(64 * 2);
	Radar.SetCrossColor(ImColor(255, 255, 255, 255));
	Radar.ShowCrossLine = true;
	Radar.Opened = true;
}

void Render::PlayerESP()
{
	VMProtectBeginMutation(__FUNCSIG__);
	if ((render_frame_count_ % 120) == 0)
	{
		if(!game_.update())
			return ;
	}

	resetFrameContext();

	Radar Radar;
	RadarSetting(Radar);
	if(!game_.validPlayerIndex(frame_ctx_.localPlayerIndex))
		return;


	const APawn& localPlayer = frame_ctx_.playerArray[frame_ctx_.localPlayerIndex];
	for (int i = 0; i < frame_ctx_.playerArray.size(); ++i)
	{
		const APawn& player = frame_ctx_.playerArray[i];

		if (!player.valid || player.hp == 0 || frame_ctx_.localPlayerIndex == i)
			continue;

		if (MenuConfig::TeamCheck && localPlayer.team == player.team)
			continue;
			
		if (MenuConfig::ShowBoneESP)
			drawPayerBone(player);

		const auto& [rect, box_ret] = player.box;
		if (box_ret)
		{
			int text_index = 0;
			if (MenuConfig::ShowBoxESP)
				Gui.Rectangle(Vec2{ rect.x, rect.y }, Vec2{ rect.w, rect.h }, MenuConfig::BoxColor, boxThickness);

			if (MenuConfig::ShowLineToEnemy)
			{
				auto [w, h] = frame_ctx_.game_window_size;
				Gui.Line({ w / 2, 0 }, { rect.x + rect.w / 2, rect.y }, MenuConfig::BoxColor, LineThickness);
			}

			if (MenuConfig::ShowEyeRay)
			{
				const PlayerBoneData& headBone =  player.Bone[head];
				if (headBone.valid() && headBone.inScreen())
				{
					float Length = 70.0f;
					ViewAngle angle = adjustViewAngle(player.viewAngle);
					Vec2 StartPoint;
					D3DXVECTOR2 EndPoint;
					D3DXVECTOR3 Temp;
					StartPoint = Vec2{ headBone.screenPos.x, headBone.screenPos.y };

					CalculateEndPoint(headBone.worldPos, angle.yaw, angle.pitch, Length, Temp);
					if (game_.WorldToScreen(Temp, EndPoint))
					{
						Gui.Line(StartPoint, { EndPoint.x, EndPoint.y }, MenuConfig::AngleColor, LineThickness);
					}
				}
			}

			if (MenuConfig::ShowRadar)
			{
				if (player.Bone[head].valid() && localPlayer.Bone[head].valid())
				{
					D3DXVECTOR3 playerPos = player.Bone[head].worldPos;
					D3DXVECTOR3 localPlayerPos = localPlayer.Bone[head].worldPos;

					Radar.AddPoint(Vec3{ localPlayerPos.x, localPlayerPos.y, localPlayerPos.z},
						adjustViewAngle(localPlayer.viewAngle).yaw,
						Vec3{ playerPos.x, playerPos.y, playerPos.z },
						ImColor(255, 0, 0, 255),
						1,
						adjustViewAngle(player.viewAngle).yaw
					);

				}
			}

			if (MenuConfig::ShowPlayerName)
				addRanderRectText(rect, text_index++, player.name);

			if (MenuConfig::ShowPlayerHP != MenuConfig::kHP_WAY_NONE)
			{
				constexpr int max_hp = 100;
				constexpr int bar_mark = 1;
				constexpr int value_mark = 2;
				int mark = 0;
				if (MenuConfig::ShowPlayerHP == MenuConfig::kHP_WAY_LIFEBAR)
					mark |= bar_mark;

				if (MenuConfig::ShowPlayerHP == MenuConfig::kHP_WAY_LIFEVALUE)
					mark |= value_mark;

				if (MenuConfig::ShowPlayerHP == MenuConfig::kHP_WAY_LIFEBAR_AND_VALUE)
					mark |= (bar_mark | value_mark);

				if (mark & bar_mark)
				{
					float healthBarWidth = rect.w * HealtWidthSacle;
					healthBarWidth = healthBarWidth > HealtMaxWidth ? HealtMaxWidth : healthBarWidth;
					healthBarWidth = healthBarWidth < HealtMinWidth ? HealtMinWidth : healthBarWidth;
					DrawHealthBar((std::ptrdiff_t)&player, max_hp, player.hp, { rect.x - healthBarWidth, rect.y }, { healthBarWidth, rect.h }, false);
				}

				if (mark & value_mark)
				{
					char buffer[32] = { '\0' };
					addRanderRectText(rect, text_index++, util::tiny_format_string(xorstr_("HP: %d/%d"), player.hp, max_hp));
				}
			}

			if (MenuConfig::ShowPlayerDistance)
			{
				if (localPlayer.Bone[head].inScreen() && player.Bone[head].inScreen())
				{
					float distance = game_.GetDistance3D(localPlayer.Bone[head].worldPos, player.Bone[head].worldPos);
					addRanderRectText(rect, text_index++, std::to_string((int)distance / 10) + xorstr_("m"));
				}
			}

			if (MenuConfig::ShowWeaponESP)
				addRanderRectText(rect, text_index++, player.weapon_name);

			if (MenuConfig::ShowC4)
			{
				if (player.has_c4)
					addRanderRectText(rect, text_index++, util::CharToUtf8(xorstr_(">>>Ð¯´øC4<<<")));
			}
		} //! if (box_ret)
	}//! for (int i = 0; i < frame_ctx_.playerArray.size(); ++i)

	Radar.Render();

	if (MenuConfig::AimBot)
	{
		if(MenuConfig::ShowAimRangle)
			Gui.Circle(Vec2{frame_ctx_.game_window_size.x / 2, frame_ctx_.game_window_size.y / 2 }, MenuConfig::AimRangle,ImColor(255, 255, 255, 255), 1.3f);
		aimbot_.aimbot(frame_ctx_);
	}

	VMProtectEnd();
}



void Render::DrawMenu()
{
	
	ImGui::Begin(xorstr_("Menu"), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	{
		// esp menu
		if (ImGui::CollapsingHeader(xorstr_("ESP")))
		{
			Gui.MyCheckBox(xorstr_("BoxESP"), &MenuConfig::ShowBoxESP);
			ImGui::SameLine();
			ImGui::ColorEdit4(xorstr_("##BoxColor"), reinterpret_cast<float*>(&MenuConfig::BoxColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox(xorstr_("BoneESP"), &MenuConfig::ShowBoneESP);
			ImGui::SameLine();
			ImGui::ColorEdit4(xorstr_("##BoneColor"), reinterpret_cast<float*>(&MenuConfig::BoneColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox(xorstr_("LineToEnemy"), &MenuConfig::ShowLineToEnemy);
			ImGui::SameLine();
			ImGui::ColorEdit4(xorstr_("##LineToEnemyColor"), reinterpret_cast<float*>(&MenuConfig::LineToEnemyColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox(xorstr_("ViewAngle"), &MenuConfig::ShowEyeRay);
			ImGui::SameLine();
			ImGui::ColorEdit4(xorstr_("##ViewAngle"), reinterpret_cast<float*>(&MenuConfig::AngleColor), ImGuiColorEditFlags_NoInputs);

			Gui.MyCheckBox(xorstr_("PlayerName"), &MenuConfig::ShowPlayerName);
			ImGui::Combo(xorstr_("PlayerHP"), (int*)&MenuConfig::ShowPlayerHP, xorstr_("None\0Bar\0Value\0Bar+Value\0"));

			Gui.MyCheckBox(xorstr_("PlayerDistance"), &MenuConfig::ShowPlayerDistance);
			Gui.MyCheckBox(xorstr_("PlayerWeapon"), &MenuConfig::ShowWeaponESP);
			Gui.MyCheckBox(xorstr_("Show C4"), &MenuConfig::ShowC4);

			Gui.MyCheckBox(xorstr_("Show Radar"), &MenuConfig::ShowRadar);
		}

		if (ImGui::CollapsingHeader(xorstr_("AimBot")))
		{
			Gui.MyCheckBox(xorstr_("AimBot"), &MenuConfig::AimBot);

			if (ImGui::Combo(xorstr_("Type"), (int*)&MenuConfig::AimType, xorstr_("MEMORY\0KMBOX\0")))
			{
				aimbot_.setType(MenuConfig::AimType);
			}

			if (ImGui::Combo(xorstr_("AimKey"), &MenuConfig::AimBotHotKey, xorstr_("RBUTTON\0XBUTTON1\0XBUTTON2\0CAPITAL\0SHIFT\0CONTROL\0")))
			{
				static int s_vkey_map[6] = {
						VK_RBUTTON,
						VK_XBUTTON1,
						VK_XBUTTON2,
						VK_CAPITAL,
						VK_SHIFT,
						VK_CONTROL
				};

				aimbot_.setHotKey(s_vkey_map[MenuConfig::AimBotHotKey]);
			}

			float SmoothMin = 0.1f, SmoothMax = 1.f;
			if (Gui.SliderScalarEx1(xorstr_("Smooth"), ImGuiDataType_Float, &MenuConfig::AimSmooth, &SmoothMin, &SmoothMax, xorstr_("%.2f"), ImGuiSliderFlags_None))
			{
				aimbot_.setSmooth(MenuConfig::AimSmooth);
			}

			if (ImGui::Combo(xorstr_("AimPos"), (int*)&MenuConfig::AimPosition, xorstr_("Head\0Neck\0Spine\0")))
			{
				if(MenuConfig::AimPosition == MenuConfig::kAIM_BOT_POS_HEAD)
					MenuConfig::AimPositionIndex = head;
				else if (MenuConfig::AimPosition == MenuConfig::kAIM_BOT_POS_NECK)
					MenuConfig::AimPositionIndex = neck;
				else if (MenuConfig::AimPosition == MenuConfig::kAIM_BOT_POS_SPINE)
					MenuConfig::AimPositionIndex = spine;

				aimbot_.setBoneIndex(MenuConfig::AimPositionIndex);
			}


			Gui.MyCheckBox(xorstr_("Show AimBot Range"), &MenuConfig::ShowAimRangle);
			ImGui::SliderFloat(xorstr_("Aim Range"), &MenuConfig::AimRangle, 20, 180, xorstr_("%.2f"), ImGuiColorEditFlags_NoInputs);
		}


		if (ImGui::CollapsingHeader(xorstr_("KMBOX")))
		{
			ImGui::InputText("ip", MenuConfig::kmbox_ip, sizeof(MenuConfig::kmbox_ip) - 1);
			ImGui::InputText("port", MenuConfig::kmbox_port, sizeof(MenuConfig::kmbox_port) - 1);
			ImGui::InputText("uuid", MenuConfig::kmbox_uuid, sizeof(MenuConfig::kmbox_uuid)-1);

			if(ImGui::Button("connect"))
			{
				// 192.168.2.188
				// 33792
				// 8628E04E
				bool ret = aimbot_.connectKmBox(MenuConfig::kmbox_ip, MenuConfig::kmbox_port, MenuConfig::kmbox_uuid);
				ImGui::Text(ret ? "connect success" : "connect failed");
			}
		}

#if 0

		if (ImGui::CollapsingHeader(xorstr_("Knife Hack")))
		{
			
			if (Gui.MyCheckBox(xorstr_("Enable"), &MenuConfig::knife_hack))
			{
				game_.hookKnifeData(MenuConfig::knife_hack);
			}
			
			ImGui::InputFloat(xorstr_("marks"), &MenuConfig::knife_data_marks);
			ImGui::InputFloat(xorstr_("tap_distance"), &MenuConfig::knife_data_tap_distance);
			ImGui::InputFloat(xorstr_("tap_range"), &MenuConfig::knife_data_tap_range);
			ImGui::InputFloat(xorstr_("attack_distanc"), &MenuConfig::knife_data_attack_distance);
			ImGui::InputFloat(xorstr_("attack_range"), &MenuConfig::knife_data_attack_range);
			ImGui::InputFloat(xorstr_("attack_speed"), &MenuConfig::knife_data_attack_speed);
			ImGui::InputFloat(xorstr_("secondary_distance"), &MenuConfig::knife_data_secondary_distance);
			ImGui::InputFloat(xorstr_("secondary_range"), &MenuConfig::knife_data_secondary_range);
			ImGui::InputFloat(xorstr_("movement_speed"), &MenuConfig::knife_data_movement_speed);

			if(ImGui::Button(xorstr_("apply")))
			{
				KnifeData data;
				data.marks = MenuConfig::knife_data_marks;
				data.tap_distance = MenuConfig::knife_data_tap_distance;
				data.tap_range = MenuConfig::knife_data_tap_range;
				data.attack_distance = MenuConfig::knife_data_attack_distance;
				data.attack_range = MenuConfig::knife_data_attack_range;
				data.attack_speed = MenuConfig::knife_data_attack_speed;
				data.secondary_distance = MenuConfig::knife_data_secondary_distance;
				data.secondary_range = MenuConfig::knife_data_secondary_range;
				data.movement_speed = MenuConfig::knife_data_movement_speed;
				game_.setKnifeData(data);
			}
		}
#endif

		ImGui::Separator();
		Gui.MyCheckBox(xorstr_("TeamCheck"), &MenuConfig::TeamCheck);

		ImGui::Text(xorstr_("[HOME] HideMenu"));

		if (ImGui::Button(xorstr_("save config")))
			MenuConfig::save();
	}
	ImGui::End();
}


Render::Render(Game& game)
	:game_(game), aimbot_(game)
{
	// 192.168.2.188
	// 33792
	// 8628E04E
	strcpy(MenuConfig::kmbox_ip, "192.168.2.188");
	strcpy(MenuConfig::kmbox_port, "33792");
	strcpy(MenuConfig::kmbox_uuid, "8628E04E");
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

	updateFPS();
}
