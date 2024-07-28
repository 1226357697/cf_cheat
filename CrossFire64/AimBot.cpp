#include "AimBot.h"

AimBot::AimBot(Game& game)
  :game_(game)
{
	setHotKey(MenuConfig::AimBotHotKey);
	setBoneIndex(head);
}

void AimBot::setHotKey(int vkey)
{
  vkey_ = vkey;
}

void AimBot::setBoneIndex(int boneIndex)
{
	boneIndex_ = boneIndex;
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
			angle.pitch = (float)atan2(-dist.y, sqrt(dist.x * dist.x + dist.z * dist.z));
			angle.yaw = (float)atan2(dist.x, dist.z);
			game_.setLocalPlayerAngle(angle);
			Gui.Text("+", { aimPos2d.x, aimPos2d.y }, ImColor(255, 0, 0, 255));
		}
		
	}
}
