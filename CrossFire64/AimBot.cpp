#include "AimBot.h"

#define _USE_MATH_DEFINES
#include <math.h>
#define DEG_TO_RAD(deg) ((deg) * (M_PI / 180.0))
#define RAD_TO_DEG(rad) ((rad) * (180.0 / M_PI))

AimBot::AimBot(Game& game)
  :game_(game)
{
	setHotKey(MenuConfig::AimBotHotKey);
	setBoneIndex(head);
	setSmooth(MenuConfig::AimSmooth);
}

void AimBot::setSmooth(float smooth)
{
	if (smooth > 0.0f && smooth < 1.0f)
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
	float min_dist = -1;
	D3DXVECTOR3 aimPos = invalidWorldPos;
	D3DXVECTOR2 aimPos2d = invalidScreenPos;
	const APawn& localPlayer = frame_ctx.playerArray[frame_ctx.localPlayerIndex];
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
			float dist = game_.GetDistance2D(headBone.screenPos, { frame_ctx.game_window_size.x /2, frame_ctx.game_window_size.y/2 });
			if (min_dist == -1 || dist < min_dist)
			{
				aimPos = headBone.worldPos;
				aimPos2d = headBone.screenPos;
				min_dist = dist;
			}
		}
	}

	if (GetAsyncKeyState(vkey_) & 0x8000)
	{
		ViewAngle angle = {0,0};
		float center_x = frame_ctx.game_window_size.x / 2;
		float center_y = frame_ctx.game_window_size.y / 2;
		float r = MenuConfig::AimRangle;
		float circle_dis_x = center_x - aimPos2d.x;
		float circle_dis_y = center_y - aimPos2d.y;
		if (!(circle_dis_x * circle_dis_x + circle_dis_y * circle_dis_y > r * r))
		{
			D3DXVECTOR3 fovPos = game_.getFOVPos();
			D3DXVECTOR3 dist = aimPos - fovPos;
			angle.pitch = (float)atan2(-dist.y, sqrt(dist.x * dist.x + dist.z * dist.z)) ;
			angle.yaw = (float)atan2(dist.x, dist.z);

			angle.pitch = lerpAngle(localPlayer.viewAngle.pitch, angle.pitch, smooth_);
			angle.yaw = lerpAngle(localPlayer.viewAngle.yaw, angle.yaw, smooth_);
			//angle.pitch = DEG_TO_RAD(RAD_TO_DEG(angle.pitch) * ( smooth_)) + localPlayer.viewAngle.pitch;
			//angle.yaw = DEG_TO_RAD( RAD_TO_DEG(angle.yaw) * ( smooth_)) + localPlayer.viewAngle.yaw;
			game_.setLocalPlayerAngle(angle);
			Gui.Text("+", { aimPos2d.x, aimPos2d.y }, ImColor(255, 0, 0, 255));
		}
		
	}
}
