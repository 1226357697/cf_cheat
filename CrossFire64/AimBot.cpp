#include "AimBot.h"
#include "Logger.h"
#include "logitech_mouse/logitech_mouse.h"

#define _USE_MATH_DEFINES
#include <math.h>
#define DEG_TO_RAD(deg) ((deg) * (M_PI / 180.0))
#define RAD_TO_DEG(rad) ((rad) * (180.0 / M_PI))


static void mouse_abs_move(int x, int y)
{
	POINT p;
	GetCursorPos(&p);
	x = x - p.x;
	x = y - p.y;
	logi_mouse_move_rel(x, y);
	//INPUT input;
	//input.type = INPUT_MOUSE;
	//input.mi.dx = x;
	//input.mi.dy = y;
	//input.mi.mouseData = 0;
	//input.mi.dwFlags = MOUSEEVENTF_HWHEEL | MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;   //MOUSEEVENTF_ABSOLUTE 代表决对位置  MOUSEEVENTF_MOVE代表移动事件
	//input.mi.time = 0;
	//input.mi.dwExtraInfo = 0;

	//SendInput(1, &input, sizeof(INPUT));
}

AimBot::AimBot(Game& game)
  :game_(game)
{
	logi_mouse_open();
	setType(MenuConfig::AimType);
	setBoneIndex(MenuConfig::AimPositionIndex);
	setSmooth(MenuConfig::AimSmooth);
	static int s_vkey_map[6] = {
						VK_RBUTTON,
						VK_XBUTTON1,
						VK_XBUTTON2,
						VK_CAPITAL,
						VK_SHIFT,
						VK_CONTROL
	};

	setHotKey(s_vkey_map[MenuConfig::AimBotHotKey]);

	pidctrlX_.setProportion(0.2f);
	pidctrlX_.setIntegral(0.1f);
	pidctrlX_.setDifferential(0.2f);

	pidctrlY_.setProportion(0.2f);
	pidctrlY_.setIntegral(0.1f);
	pidctrlY_.setDifferential(0.2f);

}

AimBot::~AimBot()
{
	logi_mouse_close();
}

void AimBot::setSmooth(float smooth)
{
	if (smooth > 0.0f && smooth <= 1.0f)
	{
		smooth_ = smooth;
	}
}

void AimBot::setHotKey(int vkey)
{
	vkey_ = vkey;
}

void AimBot::setBoneIndex(int boneIndex)
{
	boneIndex_ = boneIndex;
}

void AimBot::setType(MenuConfig::AimBotType t)
{
	type_ = t;
}


// 函数用于将角度标准化到 -PI 到 PI 之间
double normalizeAngle(double angle) {
	while (angle > M_PI) angle -= 2.0 * M_PI;
	while (angle < -M_PI) angle += 2.0 * M_PI;
	return angle;
}

// 线性插值函数
double lerpAngle(double current, double target, double t) {
	double delta = normalizeAngle(target - current);
	return current + delta * t;
}

void AimBot::aimbot(const FrameContext& frame_ctx)
{
	static int prev_index = -1;
	int player_index =0;
	float min_dist = -1;
	D3DXVECTOR3 aimPos = invalidWorldPos;
	D3DXVECTOR2 aimPos2d = invalidScreenPos;
	const APawn& localPlayer = frame_ctx.playerArray[frame_ctx.localPlayerIndex];

	//if (prev_index != -1)
	//{
	//	const APawn& player = frame_ctx.playerArray[prev_index];

	//	bool check = true;
	//	if (!player.valid || player.hp == 0)
	//		check = false;

	//	if (MenuConfig::TeamCheck && localPlayer.team == player.team)
	//		check = false;

	//	if (check)
	//	{
	//		const PlayerBoneData& headBone = player.Bone[boneIndex_];
	//		if (headBone.valid() && headBone.inScreen())
	//		{
	//			float dist = game_.GetDistance2D(headBone.screenPos, { frame_ctx.game_window_size.x / 2, frame_ctx.game_window_size.y / 2 });
	//			if (min_dist == -1 || dist < min_dist)
	//			{
	//				aimPos = headBone.worldPos;
	//				aimPos2d = headBone.screenPos;
	//				min_dist = dist;
	//			}
	//		}
	//	}
	//	
	//}


	//if (prev_index == -1)
	{
		for (int i = 0; i < frame_ctx.playerArray.size(); ++i)
		{
			const APawn& player = frame_ctx.playerArray[i];

			if (!player.valid || player.hp == 0 || frame_ctx.localPlayerIndex == i)
				continue;

			if (MenuConfig::TeamCheck && localPlayer.team == player.team)
				continue;

			const PlayerBoneData& headBone = player.Bone[boneIndex_];
			if (headBone.valid() && headBone.inScreen())
			{
				float dist = game_.GetDistance2D(headBone.screenPos, { frame_ctx.game_window_size.x / 2, frame_ctx.game_window_size.y / 2 });
				if (min_dist == -1 || dist < min_dist)
				{
					aimPos = headBone.worldPos;
					aimPos2d = headBone.screenPos;
					min_dist = dist;
					player_index = i;
				}
			}
		}

	}
	
	
	if (GetAsyncKeyState(vkey_) & 0x8000)
	{
		float center_x = frame_ctx.game_window_size.x / 2;
		float center_y = frame_ctx.game_window_size.y / 2;
		float r = MenuConfig::AimRangle;
		float circle_dis_x = center_x - aimPos2d.x;
		float circle_dis_y = center_y - aimPos2d.y;
		if (!(circle_dis_x * circle_dis_x + circle_dis_y * circle_dis_y > r * r))
		{
			if (type_ == MenuConfig::kAIM_BOT_TYPE_MEMORY)
			{
				ViewAngle angle = { 0,0 };
				{
					D3DXVECTOR3 fovPos = game_.getFOVPos();
					D3DXVECTOR3 dist = aimPos - fovPos;
					angle.pitch = (float)atan2(-dist.y, sqrt(dist.x * dist.x + dist.z * dist.z));
					angle.yaw = (float)atan2(dist.x, dist.z);

					if(smooth_ >= 1.0f )
						smooth_ = 0.9999999999f;
					angle.pitch = lerpAngle(localPlayer.viewAngle.pitch, angle.pitch, smooth_);
					angle.yaw = lerpAngle(localPlayer.viewAngle.yaw, angle.yaw, smooth_);
					angle.pitch = DEG_TO_RAD(RAD_TO_DEG(angle.pitch) * ( smooth_)) + localPlayer.viewAngle.pitch;
					angle.yaw = DEG_TO_RAD( RAD_TO_DEG(angle.yaw) * ( smooth_)) + localPlayer.viewAngle.yaw;
					game_.setLocalPlayerAngle(angle);
					Gui.Text("+", { aimPos2d.x, aimPos2d.y }, ImColor(255, 0, 0, 255));
				}
			}
			else if (type_ == MenuConfig::kAIM_BOT_TYPE_LOGITECH_DRIVER)
			{
				POINT cur_pt;
				POINT pt;
				pt.x = aimPos2d.x;
				pt.y = aimPos2d.y;
				if (ClientToScreen(game_.window(), &pt) && GetCursorPos(&cur_pt))
				{
					//Logger::info("x:%d y:%d", pt.x, pt.y);
					//pt.x -= cur_pt.x;
					//pt.y -= cur_pt.y;
					pidctrlX_.setTarget(pt.x);
					pidctrlY_.setTarget(pt.y);
					pt.x = pidctrlX_.run(cur_pt.x);
					pt.y = pidctrlY_.run(cur_pt.y);
					//Logger::info("adjust x:%d y:%d", pt.x, pt.y);
					//pt.x -= cur_pt.x;
					//pt.y -= cur_pt.y;
					logi_mouse_move_rel(pt.x, pt.y);
					//mouse_abs_move(pt.x, pt.y);
					//Logger::info("x:%d y:%d", pt.x, pt.y);
					//if (abs(pt.x) >30 || abs(pt.y) > 30)
					{
						//Logger::info("x:%d y:%d", pt.x, pt.y);
						//kb_.mov_mouse(pt.x*0.7, pt.y * 0.7);
						//kb_.mov_mouse(10,10);
					}
					Gui.Text("+", { aimPos2d.x, aimPos2d.y }, ImColor(255, 0, 0, 255));
					prev_index = player_index;
				}
			}
			else
			{
				//assert(false);
			}

		}
		
	}
}

void AimBot::setPID_p(float p)
{
	pidctrlX_.setProportion(p);
	pidctrlY_.setProportion(p);
}

void AimBot::setPID_i(float i)
{
	pidctrlX_.setIntegral(i);
	pidctrlY_.setIntegral(i);
}

void AimBot::setPID_d(float d)
{
	pidctrlX_.setDifferential(d);
	pidctrlY_.setDifferential(d);
}

bool AimBot::connectKmBox(const char* ip, const char* port, const char* uuid)
{
	return kb_.connect(ip, port, uuid);
}
